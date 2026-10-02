#ifndef __KSU_H_ADB_ROOT
#define __KSU_H_ADB_ROOT

#include <linux/version.h>
#include <linux/types.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0) && defined(CONFIG_KSU_FEATURE_ADBROOT)

/* 5.10+ 内核且开启宏时的正常声明 */
void ksu_adb_root_init(void);
void ksu_adb_root_exit(void);
long ksu_adb_root_handle_execveat(const char *filename, void ***envp_user_ptr);

#else

/* 5.4 内核或未开启宏时的 static inline 内联空实现 (防止链接阶段报 undefined symbol) */
static inline void ksu_adb_root_init(void) { }
static inline void ksu_adb_root_exit(void) { }
static inline int ksu_adb_root_handle_execveat(const struct pt_regs *regs) { return 0; }

#endif

#endif // __KSU_H_ADB_ROOT
