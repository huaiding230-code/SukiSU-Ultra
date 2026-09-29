#ifndef __KSU_H_ADB_ROOT
#define __KSU_H_ADB_ROOT

#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0) && defined(CONFIG_KSU_FEATURE_ADBROOT)

/* 5.10+ 内核且开启宏时的正常声明 */
void ksu_adb_root_init(void);
void ksu_adb_root_exit(void);
void ksu_adb_root_handle_execveat(void);

#else

/* 5.4 内核或未开启宏时的 static inline 内联空实现 */
static inline void ksu_adb_root_init(void) { }
static inline void ksu_adb_root_exit(void) { }
static inline void ksu_adb_root_handle_execveat(void) { }

#endif

#endif // __KSU_H_ADB_ROOT
