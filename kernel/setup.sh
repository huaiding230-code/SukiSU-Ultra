#!/bin/bash
set -euo pipefail

GKI_ROOT=$(pwd)

initialize_variables() {
    if [ -d "$GKI_ROOT/common/drivers" ]; then
         DRIVER_DIR="$GKI_ROOT/common/drivers"
    elif [ -d "$GKI_ROOT/drivers" ]; then
         DRIVER_DIR="$GKI_ROOT/drivers"
    else
         echo '[ERROR] "drivers/" directory not found.'
         exit 127
    fi

    DRIVER_MAKEFILE="$DRIVER_DIR/Makefile"
    DRIVER_KCONFIG="$DRIVER_DIR/Kconfig"
}

setup_sukisu_and_susfs() 
{
    echo "[+] 1. 挂载 SukiSU-Ultra 到 drivers/kernelsu..."
    cd "$DRIVER_DIR"
    rm -rf kernelsu
    # 创建指向 KernelSU/kernel 的软链接
    ln -sf "$(realpath --relative-to="$DRIVER_DIR" "$GKI_ROOT/KernelSU/kernel")" "kernelsu" && echo "[+] 软链接创建成功。"

    # 配置 drivers/Makefile 与 Kconfig
    grep -q "kernelsu" "$DRIVER_MAKEFILE" || echo "obj-y += kernelsu/" >> "$DRIVER_MAKEFILE"
    grep -q "drivers/kernelsu/Kconfig" "$DRIVER_KCONFIG" || sed -i "/endmenu/i\source \"drivers/kernelsu/Kconfig\"" "$DRIVER_KCONFIG"

    echo "[+] 2. 拉取并注入 SUSFS 源码与内核补丁..."
    cd "$GKI_ROOT"
    mkdir -p fs include/linux
    rm -rf /tmp/susfs4ksu
    git clone --depth=1 https://github.com/huaiding230-code/susfs4ksu.git /tmp/susfs4ksu

    # 复制 susfs C 源文件及头文件
    SUSFS_C=$(find /tmp/susfs4ksu -name "susfs.c" | head -n 1)
    cp "$SUSFS_C" fs/
    find /tmp/susfs4ksu -name "susfs*.h" -exec cp {} include/linux/ \;

    # 修复 susfs.h 前置结构体声明
    if [ -f include/linux/susfs.h ]; then
        sed -i '/int susfs_add_sus_path/i struct filename;\nstruct stat;' include/linux/susfs.h
    fi

    # 配置 fs/Makefile 编译 susfs.o
    sed -i '/susfs.o/d' fs/Makefile
    echo 'obj-$(CONFIG_KSU_SUSFS) += susfs.o' >> fs/Makefile

    # 补充 Kconfig 配置项
    if ! grep -q "CONFIG_KSU_SUSFS" fs/Kconfig; then
        echo -e '\nconfig KSU_SUSFS\n\tbool "SUSFS support"\n\tdefault y\n' >> fs/Kconfig
    fi

    # 应用 Linux 内核挂钩 Patch
    PATCH_FILE="/tmp/susfs4ksu/kernel_patches/50_add_susfs_in_kernel-5.4.patch"
    if [ -f "$PATCH_FILE" ]; then
        echo "[+] 应用 5.4 内核 SUSFS 挂钩补丁..."
        patch -p1 < "$PATCH_FILE" || echo "[!] 警告: Patch 应用有冲突，请检查内核源码。"
    fi
    # ----------------------------------------------------
    # 针对 oplus 驱动与 DTS Makefile 报错的精准

    #  精准去除 lemonadev/Makefile 行首误加的 TAB 键（彻底解决 recipe commences before first target）
    DTS_MAKEFILE="$GKI_ROOT/arch/arm64/boot/dts/vendor/oplus/lemonadev/Makefile"
    if [ -f "$DTS_MAKEFILE" ]; then
        sed -i 's/^\t//' "$DTS_MAKEFILE"
    fi

    echo "[+] SukiSU-Ultra 与 SUSFS 完整集成结束！"
}

initialize_variables
setup_sukisu_and_susfs
