#ifndef __KSU_H_SULOG_EVENT
#define __KSU_H_SULOG_EVENT

#include <linux/types.h>

struct ksu_event_queue;
struct ksu_sulog_pending_event;

#if defined(CONFIG_KSU_SULOG) || defined(CONFIG_KSU_FEATURE_SULOG)

/* 启用 SULOG 模块时的正常声明 */
int ksu_sulog_events_init(void);
void ksu_sulog_events_exit(void);

#ifdef CONFIG_KSU_SUSFS
struct ksu_sulog_pending_event *ksu_sulog_capture_sucompat(const char *filename,
                                                           void *argv_user, gfp_t gfp);
#else
struct ksu_sulog_pending_event *ksu_sulog_capture_root_execve(const char __user *filename,
                                                                     const void *argv, gfp_t gfp);
struct ksu_sulog_pending_event *ksu_sulog_capture_sucompat(const char __user *filename, const void *argv,
                                                                  gfp_t gfp);
#endif
void ksu_sulog_emit_pending(struct ksu_sulog_pending_event *pending, int retval, gfp_t gfp);
int ksu_sulog_emit_grant_root(int retval, __u32 uid, __u32 euid, gfp_t gfp);

struct ksu_event_queue *ksu_sulog_get_queue(void);

#else

/* 未启用 SULOG 模块时，走 static inline 内联空函数 (彻底防止 ld.lld 报 undefined symbol) */
static inline int ksu_sulog_events_init(void) { return 0; }
static inline void ksu_sulog_events_exit(void) { }

static inline struct ksu_sulog_pending_event *ksu_sulog_capture_sucompat(const void *filename,
                                                                          const void *argv_user, gfp_t gfp) { return NULL; }
static inline struct ksu_sulog_pending_event *ksu_sulog_capture_root_execve(const void *filename,
                                                                             const void *argv, gfp_t gfp) { return NULL; }

static inline void ksu_sulog_emit_pending(struct ksu_sulog_pending_event *pending, int retval, gfp_t gfp) { }
static inline int ksu_sulog_emit_grant_root(int retval, __u32 uid, __u32 euid, gfp_t gfp) { return 0; }

static inline struct ksu_event_queue *ksu_sulog_get_queue(void) { return NULL; }

#endif

#endif // __KSU_H_SULOG_EVENT
