#ifndef __KSU_H_MANAGER_OBSERVER
#define __KSU_H_MANAGER_OBSERVER

#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
/* 5.10+ 高版本内核：正常引用编译进来的 observer.o 实体 */
int ksu_observer_init(void);
void ksu_observer_exit(void);
#else
/* 5.4 内核：由于 Kbuild 跳过了 observer.o，此处自动替换为空函数，防止链接报错 */
static inline int ksu_observer_init(void) { return 0; }
static inline void ksu_observer_exit(void) {}
#endif

#endif // __KSU_H_MANAGER_OBSERVER
