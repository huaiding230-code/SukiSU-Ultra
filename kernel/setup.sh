#!/bin/sh
set -eu

GKI_ROOT=$(pwd)

display_usage() {
    echo "Usage: $0 [--cleanup | <commit-or-tag>]"
    echo "  --cleanup:              Cleans up previous modifications made by the script."
    echo "  <commit-or-tag>:        Sets up or updates the KernelSU to specified tag or commit."
    echo "  -h, --help:             Displays this usage information."
    echo "  (no args):              Sets up or updates the KernelSU environment to the latest tagged version."
}

initialize_variables() {
    if test -d "$GKI_ROOT/common/drivers"; then
         DRIVER_DIR="$GKI_ROOT/common/drivers"
    elif test -d "$GKI_ROOT/drivers"; then
         DRIVER_DIR="$GKI_ROOT/drivers"
    else
         echo '[ERROR] "drivers/" directory not found.'
         exit 127
    fi

    DRIVER_MAKEFILE=$DRIVER_DIR/Makefile
    DRIVER_KCONFIG=$DRIVER_DIR/Kconfig
}

# Reverts modifications made by this script
perform_cleanup() {
    echo "[+] Cleaning up..."
    [ -L "$DRIVER_DIR/kernelsu" ] && rm "$DRIVER_DIR/kernelsu" && echo "[-] Symlink removed."
    grep -q "kernelsu" "$DRIVER_MAKEFILE" && sed -i '/kernelsu/d' "$DRIVER_MAKEFILE" && echo "[-] Makefile reverted."
    grep -q "drivers/kernelsu/Kconfig" "$DRIVER_KCONFIG" && sed -i '/drivers\/kernelsu\/Kconfig/d' "$DRIVER_KCONFIG" && echo "[-] Kconfig reverted."
    if [ -d "$GKI_ROOT/KernelSU" ]; then
        rm -rf "$GKI_ROOT/KernelSU" && echo "[-] KernelSU directory deleted."
    fi
}

# Sets up or update KernelSU environment
setup_sukisu_and_susfs() {
    echo "[+] 1. 挂载 SukiSU-Ultra 到 drivers/kernelsu..."
    cd "$DRIVER_DIR"
    rm -rf kernelsu

    # 兼容路径：优先匹配 build.yml 克隆的 /tmp/SukiSU-Ultra/kernel
    if [ -d "/tmp/SukiSU-Ultra/kernel" ]; then
        KSU_SRC_PATH="/tmp/SukiSU-Ultra/kernel"
    elif [ -d "$GKI_ROOT/KernelSU/kernel" ]; then
        KSU_SRC_PATH="$GKI_ROOT/KernelSU/kernel"
    else
        echo "[-] 错误: 未找到 SukiSU 源码目录！" && exit 1
    fi

    # 创建指向有效源码路径的软链接
    ln -sf "$(realpath --relative-to="$DRIVER_DIR" "$KSU_SRC_PATH")" "kernelsu" && echo "[+] 软链接创建成功 -> $KSU_SRC_PATH"

    # 配置 drivers/Makefile 与 Kconfig
    grep -q "kernelsu" "$DRIVER_MAKEFILE" || echo "obj-y += kernelsu/" >> "$DRIVER_MAKEFILE"
    grep -q "drivers/kernelsu/Kconfig" "$DRIVER_KCONFIG" || sed -i "/endmenu/i\source \"drivers/kernelsu/Kconfig\"" "$DRIVER_KCONFIG"

    echo "[+] 2. 应用 SUSFS 5.4 内核挂钩补丁..."
    cd "$GKI_ROOT"

    # 应用 Linux 5.4 内核挂钩 Patch
    PATCH_FILE="/tmp/susfs4ksu/kernel_patches/50_add_susfs_in_kernel-5.4.patch"
    if [ -f "$PATCH_FILE" ]; then
        echo "[+] 应用 5.4 内核 SUSFS 挂钩补丁..."
        patch -p1 < "$PATCH_FILE" || echo "[!] 警告: Patch 应用有冲突，请检查日志。"
    fi

    echo "[+] setup.sh 挂载与补丁处理结束！"
}

# Process command-line arguments
if [ "$#" -eq 0 ]; then
    initialize_variables
    setup_kernelsu
elif [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
    display_usage
elif [ "$1" = "--cleanup" ]; then
    initialize_variables
    perform_cleanup
else
    initialize_variables
    setup_kernelsu "$@"
fi
