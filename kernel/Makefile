obj-$(CONFIG_KSU) += ksu.o

# 显式把所有子目录下的 .c 文件全部编进来，彻底消灭 undefined symbol
ksu-objs := \
    policy/allowlist.o \
    policy/app_profile.o \
    manager/apk_sign.o \
    manager/pkg_observer.o \
    manager/manager.o \
    manager/manager_observer.o \
    feature/sucompat.o \
    feature/adb_root.o \
    feature/sulog.o \
    feature/selinux_hide.o \
    feature/kernel_umount.o \
    sulog/event.o \
    sulog/fd.o \
    supercall/dispatch.o \
    supercall/supercall.o \
    supercall/perm.o \
    hook/lsm_hook.o \
    hook/syscall_event_bridge.o \
    selinux/selinux.o \
    selinux/rules.o \
    selinux/sepolicy.o \
    infra/file_wrapper.o \
    infra/su_mount_ns.o \
    infra/boot_mode.o

# 将自动扫描到的 .o 统一打入 ksu-objs（注意改成 ksu-y，不要写成 kernelsu-y）
ALL_SRCS := $(shell find $(src) -name "*.c")
ALL_OBJS := $(patsubst $(src)/%.c, %.o, $(ALL_SRCS))

ksu-y += $(ALL_OBJS)
ksu-objs += $(ksu-y)

# ==============================================================================
# SELinux 头文件搜索路径配置
# ==============================================================================
# 汇总内核源码树 (srctree) 和 编译输出目录 (objtree) 中所有与 SELinux 相关的头文件路径
SELINUX_INCLUDES := -I$(objtree)/security/selinux \
                    -I$(objtree)/security/selinux/include \
                    -I$(srctree)/security/selinux \
                    -I$(srctree)/security/selinux/include \
                    -I$(objtree) \
                    -I$(srctree)

# 1. 递归传递给子目录：确保 policy/、supercall/ 等所有子文件夹中的 C 文件都能找到头文件
subdir-ccflags-y += $(SELINUX_INCLUDES)

# 2. 作用于当前目录：给当前 Make 作用域下的所有 C 文件注入头文件搜索路径
ccflags-y        += $(SELINUX_INCLUDES)

# 3. 旧内核兼容：向后兼容早期 Linux/Android 内核的 Makefile 变量命名规则
EXTRA_CFLAGS     += $(SELINUX_INCLUDES)

# ==============================================================================
# 1. 自动递归扫描模块下的所有 .c 源文件
# ==============================================================================
ALL_SRCS := $(shell find $(src) -name "*.c")
ALL_OBJS := $(patsubst $(src)/%.c, %.o, $(ALL_SRCS))

# ==============================================================================
# 2 & 3. 将扫描到的所有目标文件 (.o) 统一打入 kernelsu 编译列表
# ==============================================================================
kernelsu-y += $(ALL_OBJS)
kernelsu-objs += $(kernelsu-y)

# =================================================================
# 解决多线程并发编译抢跑 (flask.h 丢失问题)
# =================================================================
FLASK_H := $(objtree)/security/selinux/include/flask.h

# 声明模式依赖：编译本模块下的任意 .o 之前，必须先存在 flask.h
$(obj)/%.o: $(FLASK_H)

# 生成规则：若 flask.h 不存在，阻塞调用内核原生的 selinux 编译脚本提前生成
$(FLASK_H):
	$(Q)$(MAKE) -f $(srctree)/scripts/Makefile.build obj=security/selinux

ifeq ($(filter /%,$(src)),)
KSU_KERNEL_DIR := $(srctree)/$(src)
else
KSU_KERNEL_DIR := $(src)
endif

ifeq ($(CONFIG_KSU_SUSFS), y)
## As we are building gki only, no KBUILD_EXTMOD is needed
## Since there is still a build error for 6.12 as $(src) returns absolute path, so here we just hardcode it
ccflags-y += -I$(srctree)/drivers/kernelsu -I$(srctree)/drivers/kernelsu/include
else
ccflags-y += -I$(KSU_KERNEL_DIR) -I$(KSU_KERNEL_DIR)/include
endif

MDIR    := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

REPO_OWNER  := SukiSU-Ultra
REPO_NAME   := SukiSU-Ultra
REPO_BRANCH := main

VERSION_BASE    := 40000
VERSION_OFFSET  := 2815

GIT_BIN  := /usr/bin/env PATH="$$PATH":/usr/bin:/usr/local/bin git
CURL_BIN := /usr/bin/env PATH="$$PATH":/usr/bin:/usr/local/bin curl

KSU_SRC := $(if $(filter $(srctree),$(src)),$(src),$(srctree)/$(src))
KSU_SRC := $(if $(wildcard $(KSU_SRC)/../.git),$(KSU_SRC),$(MDIR))

git_short_sha    = $(shell cd $(KSU_SRC) && $(GIT_BIN) rev-parse --short=8 HEAD 2>/dev/null)
git_branch       = $(shell cd $(KSU_SRC) && $(GIT_BIN) rev-parse --abbrev-ref HEAD 2>/dev/null)
git_commit_count = $(shell cd $(KSU_SRC) && $(GIT_BIN) rev-list --count $(REPO_BRANCH) 2>/dev/null)
git_latest_tag   = $(shell cd $(KSU_SRC) && $(GIT_BIN) describe --tags --abbrev=0 2>/dev/null | sed 's/^v//')

$(shell cd $(KSU_SRC) 2>/dev/null && [ -f ../.git/shallow ] && $(GIT_BIN) fetch --unshallow 2>/dev/null)

GITHUB_API_BASE  := https://api.github.com/repos/$(REPO_OWNER)/$(REPO_NAME)
KSU_GITHUB_VER   := $(shell $(CURL_BIN) -s "$(GITHUB_API_BASE)/releases/latest" 2>/dev/null | grep '"tag_name":' | sed -E 's/.*"v?([^"]+)".*/\1/')
GITHUB_COMMITS   := $(shell $(CURL_BIN) -sI "$(GITHUB_API_BASE)/commits?sha=$(REPO_BRANCH)&per_page=1" 2>/dev/null | grep -i "link:" | sed -n 's/.*page=\([0-9]*\)>; rel="last".*/\1/p')
LOCAL_COUNT     := $(if $(GITHUB_COMMITS),$(GITHUB_COMMITS),$(git_commit_count))
KSU_VERSION     := $(if $(LOCAL_COUNT),$(shell expr $(VERSION_BASE) + $(LOCAL_COUNT) - $(VERSION_OFFSET)),13000)
VERSION_TAG     := $(or $(KSU_GITHUB_VER),$(git_latest_tag),4.2.0)
KSU_VERSION_FULL := $(if $(call git_short_sha),v$(VERSION_TAG)-$(call git_short_sha)@$(call git_branch),v$(VERSION_TAG)-$(REPO_NAME)-unknown@unknown)
$(info -- $(REPO_NAME) version: $(KSU_VERSION) [$(KSU_VERSION_FULL)])

ccflags-y += -DKSU_VERSION=$(KSU_VERSION)
ccflags-y += -DKSU_VERSION_FULL=\"$(KSU_VERSION_FULL)\"

KERNEL_VERSION := $(VERSION).$(PATCHLEVEL)
KERNEL_TYPE := Non-GKI
# Check for GKI 2.0 (5.10+ or 6.x+)
ifneq ($(shell test \( $(VERSION) -ge 5 -a $(PATCHLEVEL) -ge 10 \) -o $(VERSION) -ge 6; echo $$?),0)
# Check for GKI 1.0 (5.4)
ifeq ($(shell test $(VERSION)-$(PATCHLEVEL) = 5-4; echo $$?),0)
KERNEL_TYPE := GKI 1.0
endif
else
KERNEL_TYPE := GKI 2.0
endif
$(info -- KERNEL_VERSION: $(KERNEL_VERSION))
$(info -- KERNEL_TYPE: $(KERNEL_TYPE))

ifeq ($(CONFIG_KSU_SUSFS), y)
$(info -- $(REPO_NAME): using SUSFS_INLINE_HOOK)
else
$(info -- $(REPO_NAME): using Unknown)
endif

ifeq ($(CONFIG_KPM), y)
$(info -- KPM is enabled)
else
$(info -- KPM is disabled)
endif

# Check new vfs_getattr()
ifeq ($(shell grep -A1 "^int vfs_getattr" $(srctree)/fs/stat.c | grep -q "query_flags" ; echo $$?),0)
ccflags-y += -DKSU_HAS_NEW_VFS_GETATTR
endif

ifeq ($(shell grep -q " current_sid(void)" $(srctree)/security/selinux/include/objsec.h; echo $$?),0)
ccflags-y += -DKSU_COMPAT_HAS_CURRENT_SID
endif

ifeq ($(shell grep -q "struct selinux_state " $(srctree)/security/selinux/include/security.h; echo $$?),0)
ccflags-y += -DKSU_COMPAT_HAS_SELINUX_STATE
endif

ifeq ($(shell grep -q "strncpy_from_user_nofault" $(srctree)/include/linux/uaccess.h; echo $$?),0)
ccflags-y += -DKSU_HAS_STRNCPY_FROM_USER_NOFAULT
endif

ifeq ($(shell grep -q "atomic_t\s\+filter_count" $(srctree)/include/linux/seccomp.h; echo $$?),0)
ccflags-y += -DKSU_OPTIONAL_SECCOMP_FILTER_CNT
endif

# some old kernel backport this, check if put_seccomp_filter still exist
ifneq ($(shell grep -wq "put_seccomp_filter" $(srctree)/kernel/seccomp.c $(srctree)/include/linux/seccomp.h; echo $$?),0)
ifeq ($(shell grep -wq "seccomp_filter_release" $(srctree)/kernel/seccomp.c $(srctree)/include/linux/seccomp.h; echo $$?),0)
ccflags-y += -DKSU_OPTIONAL_SECCOMP_FILTER_RELEASE
endif
endif

ifeq ($(shell grep -q "^DEFINE_RWLOCK(policy_rwlock);" $(srctree)/security/selinux/ss/services.c; echo $$?),0)
ccflags-y += -DKSU_COMPAT_HAS_EXPORTED_POLICY_RWLOCK
endif

ifeq ($(shell grep -q "cpus_ptr;" $(srctree)/include/linux/sched.h; echo $$?),0)
ccflags-y += -DKSU_COMPAT_HAS_BACKPORTED_CPUS_PTR
endif

ifeq ($(shell grep -q "selinux_inode" $(srctree)/security/selinux/include/objsec.h; echo $$?),0)
ccflags-y += -DKSU_HAS_SELINUX_INODE
endif

ifeq ($(shell grep -q "selinux_cred" $(srctree)/security/selinux/include/objsec.h; echo $$?),0)
ccflags-y += -DKSU_HAS_SELINUX_CRED
endif

ifeq ($(shell grep -q "struct type_datum \*\*type_val_to_struct;" $(srctree)/security/selinux/ss/policydb.h; echo $$?),0)
ccflags-y += -DKSU_TYPE_VAL_TO_STRUCT
endif

# half-assed-backport from 5.1
ifeq ($(shell grep -q "struct type_datum \*\*type_val_to_struct_array;" $(srctree)/security/selinux/ss/policydb.h; echo $$?),0)
ccflags-y += -DKSU_TYPE_VAL_TO_STRUCT_ARRAY
endif

# https://github.com/torvalds/linux/commit/11276d5306b8e5b438a36bbff855fe792d7eaa61
ifeq ($(shell grep -q "DEFINE_STATIC_KEY_TRUE" $(srctree)/include/linux/jump_label.h 2>/dev/null; echo $$?),0)
$(info -- $(REPO_NAME)/compat: modern static_key_interface found)
ccflags-y += -DKSU_HAS_MODERN_STATIC_KEY_INTERFACE
endif

ifdef KSU_MANAGER_PACKAGE
ccflags-y += -DKSU_MANAGER_PACKAGE=\"$(KSU_MANAGER_PACKAGE)\"
$(info -- KernelSU Manager package name: $(KSU_MANAGER_PACKAGE))
endif

## For susfs stuff ##
ifeq ($(shell test -e $(srctree)/fs/susfs.c; echo $$?),0)
$(eval SUSFS_VERSION=$(shell cat $(srctree)/include/linux/susfs.h | grep -E '^#define SUSFS_VERSION' | cut -d' ' -f3 | sed 's/"//g'))
$(info )
$(info -- SUSFS_VERSION: $(SUSFS_VERSION))
else
$(info -- You have not integrated susfs in your kernel yet.)
$(info -- Read: https://gitlab.com/simonpunk/susfs4ksu)
endif

ccflags-y += -Wno-strict-prototypes -Wno-int-conversion -Wno-gcc-compat -Wno-missing-prototypes
ccflags-y += -Wno-declaration-after-statement -Wno-unused-function

ccflags-y += -I$(srctree)/$(src)/include -I$(srctree)/$(src)/include/uapi -I$(srctree)/$(src)/policy -I$(srctree)/$(src)/feature

# Keep a new line here!! Because someone may append config
