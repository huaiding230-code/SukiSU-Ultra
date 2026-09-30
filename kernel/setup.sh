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
setup_kernelsu() {
    echo "[+] Setting up KernelSU..."

    # 1. 优先指向 build.yml 拉取到 /tmp 的个人魔改仓库，没有则退回本地 KernelSU
    if [ -d "/tmp/SukiSU-Ultra/kernel" ]; then
        KSU_SRC_PATH="/tmp/SukiSU-Ultra/kernel"
    else
        KSU_SRC_PATH="$GKI_ROOT/KernelSU/kernel"
    fi

    # 2. 建立软链接
    cd "$DRIVER_DIR"
    rm -rf kernelsu
    ln -sf "$(realpath --relative-to="$DRIVER_DIR" "$KSU_SRC_PATH")" "kernelsu" && echo "[+] Symlink created -> $KSU_SRC_PATH"

    # 3. 修改 drivers/Makefile 与 Kconfig
    grep -q "kernelsu" "$DRIVER_MAKEFILE" || printf "\nobj-\$(CONFIG_KSU) += kernelsu/\n" >> "$DRIVER_MAKEFILE" && echo "[+] Modified Makefile."
    grep -q "source \"drivers/kernelsu/Kconfig\"" "$DRIVER_KCONFIG" || sed -i "/endmenu/i\source \"drivers/kernelsu/Kconfig\"" "$DRIVER_KCONFIG" && echo "[+] Modified Kconfig."

    # 4. 切回内核根目录，自动打入 SUSFS 5.4 补丁
    cd "$GKI_ROOT"
    PATCH_FILE="/tmp/susfs4ksu/kernel_patches/50_add_susfs_in_kernel-5.4.patch"
    if [ -f "$PATCH_FILE" ]; then
        echo "[+] 应用 5.4 内核 SUSFS 挂钩补丁..."
        patch -p1 < "$PATCH_FILE" || echo "[!] 警告: Patch 应用有冲突，请检查日志。"
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
