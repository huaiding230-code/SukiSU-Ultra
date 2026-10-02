#include <linux/compiler.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <linux/task_work.h>
#include <linux/thread_info.h>
#include <linux/seccomp.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/uidgid.h>

#include "../policy/allowlist.h"
#include "setuid_hook.h"
#include "../include/klog.h" // IWYU pragma: keep
#include "../manager/manager_identity.h"
#include "../infra/seccomp_cache.h"
#include "../supercall/supercall.h"
#include "tp_marker.h"
#include "../feature/kernel_umount.h"

void __init ksu_setuid_hook_init(void)
{
    ksu_kernel_umount_init();
}

void __exit ksu_setuid_hook_exit(void)
{
    pr_info("ksu_core_exit\n");
    ksu_kernel_umount_exit();
}
