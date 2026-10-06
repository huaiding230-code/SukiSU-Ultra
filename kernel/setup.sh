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

# Sets up KernelSU environment directly from pre-cloned directory
setup_kernelsu() {
    echo "[+] Setting up KernelSU..."

    # 1. 优先使用 build.yml 已经克隆到 /tmp/SukiSU-Ultra 的源码，不存在则尝试本地 KernelSU
    if [ -d "/tmp/SukiSU-Ultra/kernel" ]; then
        KSU_SRC_PATH="/tmp/SukiSU-Ultra/kernel"
    elif [ -d "$GKI_ROOT/KernelSU/kernel" ]; then
        KSU_SRC_PATH="$GKI_ROOT/KernelSU/kernel"
    else
        echo '[ERROR] SukiSU kernel source not found in /tmp/SukiSU-Ultra/kernel or KernelSU/kernel.'
        exit 1
    fi

    # 2. 创建到 drivers/kernelsu 的软链接
    cd "$DRIVER_DIR"
    rm -rf kernelsu
    ln -sf "$(realpath --relative-to="$DRIVER_DIR" "$KSU_SRC_PATH")" "kernelsu" && echo "[+] Symlink created -> $KSU_SRC_PATH"

    # 3. 注入配置项到 drivers/Makefile 与 drivers/Kconfig
    grep -q "kernelsu" "$DRIVER_MAKEFILE" || printf "\nobj-\$(CONFIG_KSU) += kernelsu/\n" >> "$DRIVER_MAKEFILE" && echo "[+] Modified Makefile."
    grep -q "source \"drivers/kernelsu/Kconfig\"" "$DRIVER_KCONFIG" || sed -i "/endmenu/i\source \"drivers/kernelsu/Kconfig\"" "$DRIVER_KCONFIG" && echo "[+] Modified Kconfig."
    
    # 4. 自动开启 SukiSU / KernelSU 内部对 SUSFS 的支持宏
    if [ -f "$KSU_SRC_PATH/Kconfig" ]; then
        grep -q "CONFIG_KSU_SUSFS" "$KSU_SRC_PATH/Kconfig" || echo "config KSU_SUSFS\n\tbool \"Enable SUSFS support\"\n\tdefault y" >> "$KSU_SRC_PATH/Kconfig"
        grep -q "CONFIG_KSU_SUSFS_HAS_MAGIC_MOUNT" "$KSU_SRC_PATH/Kconfig" || echo "config KSU_SUSFS_HAS_MAGIC_MOUNT\n\tbool \"Enable SUSFS Magic Mount\"\n\tdefault y" >> "$KSU_SRC_PATH/Kconfig"
    fi
    
    echo '[+] Done.'
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
