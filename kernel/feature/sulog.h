#ifndef __KSU_H_SULOG
#define __KSU_H_SULOG

#include <linux/version.h>
#include <linux/types.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)

/* 5.10+ 高版本内核：正常的外部函数原型声明 */
bool ksu_sulog_is_enabled(void);
void ksu_sulog_init(void);
void ksu_sulog_exit(void);

#else

/* 5.4 低版本内核：由于 sulog 模块未被 Kbuild 编译，使用 static inline 内联空实现 */
static inline bool ksu_sulog_is_enabled(void) { return false; }
static inline void ksu_sulog_init(void) {}
static inline void ksu_sulog_exit(void) {}

/* 拦截 Hook 调用的空实现 */
static inline void ksu_sulog_capture_sucompat(void) {}
static inline void ksu_sulog_emit_pending(void) {}
static inline void ksu_sulog_capture_root_execve(void) {}

#endif

#endif // __KSU_H_SULOG
