#ifndef __KSU_POLICY_APP_PROFILE_H
#define __KSU_POLICY_APP_PROFILE_H

#include <linux/types.h>
#include <linux/sched.h>
#include <linux/cred.h>
#include "uapi/app_profile.h"

#ifndef KERNEL_SU_CONTEXT
#define KERNEL_SU_CONTEXT "u:r:su:s0"
#endif

/* 补充所有被调用的内部函数及 Seccomp 声明 */
void setup_groups(struct root_profile *profile, struct cred *cred);
struct root_profile *ksu_get_root_profile(uid_t uid);
void ksu_put_root_profile(struct root_profile *profile);
void setup_selinux(const char *domain, struct cred *cred);
void setup_mount_ns(u64 namespaces);
extern void put_seccomp_filter(struct task_struct *tsk);

#define TIF_KSU_DISABLE_ESCAPE_WITH_ROOT 63

// Escalate current process to root with the appropriate profile
int escape_with_root_profile(void);

void escape_to_root_for_init(void);

#endif
