#ifndef __KSU_H_KSU
#define __KSU_H_KSU

#include <linux/types.h>
#include <linux/cred.h>
#include <linux/workqueue.h>
#include <linux/version.h>
#include "../manager/manager_observer.h"

/* 通用外部函数声明（排除掉需要在 5.4 降级的函数） */
extern void ksu_load_allow_list(void);
extern void apply_kernelsu_rules(void);
extern void cache_sid(void);
extern void setup_ksu_cred(void);
extern void track_throne(bool active);
extern long ksu_strncpy_from_user_nofault(char *dst, const void __user *unsafe_addr, long count);

/* 正确修改：直接使用 extern 声明，不再手写 static inline 定义 */
extern void ksu_selinux_hide_handle_post_fs_data(void);
extern void ksu_selinux_hide_handle_second_stage(void);

#define KERNEL_SU_VERSION KSU_VERSION

extern struct cred *ksu_cred;
extern bool allow_shell;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
extern struct selinux_policy *backup_sepolicy;
#endif
extern bool ksu_no_custom_rc;

static inline int startswith(char *s, char *prefix)
{
    return strncmp(s, prefix, strlen(prefix));
}

static inline int endswith(const char *s, const char *t)
{
    size_t slen = strlen(s);
    size_t tlen = strlen(t);
    if (tlen > slen)
        return 1;
    return strcmp(s + slen - tlen, t);
}

#endif
