#ifndef __KSU_H_SUCOMPAT
#define __KSU_H_SUCOMPAT

#include <asm/ptrace.h>
#include <linux/types.h>

extern bool ksu_su_compat_enabled;

#ifdef CONFIG_KSU_SUCOMPAT

#ifdef CONFIG_KSU_SUSFS
int ksu_handle_stat_sucompat(int orig_nr, struct pt_regs *regs);
int ksu_handle_faccessat_sucompat(int orig_nr, struct pt_regs *regs);
int ksu_handle_execveat_sucompat(int *fd, struct filename **filename_ptr, void *argv, void *__never_use_envp,
                                 int *__never_use_flags);
#else
int ksu_handle_faccessat_sucompat(int *dfd, const char __user **filename_user, int *mode, int *__unused_flags);
int ksu_handle_stat_sucompat(int *dfd, const char __user **filename_user, int *flags);
int ksu_handle_execve_sucompat(int *fd, const char __user **filename_user, void *argv, void *__never_use_envp,
                               int *__never_use_flags);
int ksu_handle_execveat_sucompat(int *fd, struct filename **filename_ptr, void *argv, void *__never_use_envp,
                                 int *__never_use_flags);
#endif

#else // 未开启 CONFIG_KSU_SUCOMPAT 时的空桩兜底

static inline int ksu_handle_faccessat_sucompat(int *dfd, const char __user **filename_user, int *mode, int *__unused_flags) { return 0; }
static inline int ksu_handle_stat_sucompat(int *dfd, const char __user **filename_user, int *flags) { return 0; }
static inline int ksu_handle_execve_sucompat(int *fd, const char __user **filename_user, void *argv, void *__never_use_envp, int *__never_use_flags) { return 0; }
static inline int ksu_handle_execveat_sucompat(int *fd, void *filename_ptr, void *argv, void *__never_use_envp, int *__never_use_flags) { return 0; }

#endif // CONFIG_KSU_SUCOMPAT

void ksu_sucompat_init(void);
void ksu_sucompat_exit(void);

#endif
