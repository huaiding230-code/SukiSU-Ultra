#ifndef __KSU_H_APP_PROFILE
#define __KSU_H_APP_PROFILE

#include <linux/types.h>
#include "uapi/app_profile.h"

#ifndef KSU_MAX_GROUPS
#define KSU_MAX_GROUPS 32
#endif

/* 补充完整定义，解决 allowlist.c 报不完整类型错误 */
struct root_profile {
    uid_t uid;
    gid_t gid;
    int groups_count;
    gid_t groups[KSU_MAX_GROUPS];
    kernel_cap_t capabilities;
    u64 namespaces;
    char selinux_domain[128];
    int flags;
};

struct non_root_profile {
    bool umount_modules;
    int flags;
};

struct app_profile {
    int version;
    char key[64];
    uid_t curr_uid;
    bool allow_su;
    union {
        struct {
            bool use_default;
            struct root_profile profile;
        } rp_config;
        struct {
            bool use_default;
            struct non_root_profile profile;
        } nrp_config;
    };
};

#define TIF_KSU_DISABLE_ESCAPE_WITH_ROOT 63

// Escalate current process to root with the appropriate profile
int escape_with_root_profile(void);

void escape_to_root_for_init(void);

#endif
