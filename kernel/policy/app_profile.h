#ifndef __KSU_POLICY_APP_PROFILE_H
#define __KSU_POLICY_APP_PROFILE_H

#include <linux/types.h>
#include "uapi/app_profile.h"  /* 官方自带的结构体定义，由它统一提供 */

/* 只保留需要的函数声明（如果原本有的话），不要再写 struct 定义 */
void setup_groups(struct root_profile *profile, struct cred *cred);

#endif

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
