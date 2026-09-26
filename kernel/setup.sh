#!/bin/sh
set -eu

GKI_ROOT=$(pwd)

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

# 仅保留软链接和 Makefile/Kconfig 写入逻辑，移除所有危险的 git checkout
setup_kernelsu() {
    echo "[+] Setting up KernelSU symlink and configs..."
    initialize_variables
    
    cd "$DRIVER_DIR"
    # 建立软链接，将 drivers/kernelsu 指向我们已经克隆好的 KernelSU/kernel 目录
    ln -sf "$(realpath --relative-to="$DRIVER_DIR" "$GKI_ROOT/KernelSU/kernel")" "kernelsu" && echo "[+] Symlink created."

    # 自动向内核的 Makefile 和 Kconfig 写入编译挂载项
    grep -q "kernelsu" "$DRIVER_MAKEFILE" || printf "\nobj-\$(CONFIG_KSU) += kernelsu/\n" >> "$DRIVER_MAKEFILE" && echo "[+] Modified Makefile."
    grep -q "source \"drivers/kernelsu/Kconfig\"" "$DRIVER_KCONFIG" || sed -i "/endmenu/i\source \"drivers/kernelsu/Kconfig\"" "$DRIVER_KCONFIG" && echo "[+] Modified Kconfig."
    echo '[+] Done.'
}

# 直接执行配置，不再接收任何可能导致切分支的参数
setup_kernelsu
