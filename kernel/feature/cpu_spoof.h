/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __KSU_H_CPU_SPOOF
#define __KSU_H_CPU_SPOOF

#include "uapi/supercall.h"
#include <linux/types.h>
#include "../include/ksu.h"

/* 前置声明结构体，防止 -Wvisibility 局部作用域警告与类型冲突 */
struct ksu_set_spoof_cpu_cmd;

#ifdef CONFIG_KSU_SPOOF_CPU
int ksu_set_spoof_cpu(const struct ksu_set_spoof_cpu_cmd *cmd);
#else

/* 未开启 CPU Spoof 时的空实现存根，必须带 static inline 防止重定义 */
static inline int ksu_set_spoof_cpu(const struct ksu_set_spoof_cpu_cmd *cmd)
{
    return 0;
}

#endif /* CONFIG_KSU_SPOOF_CPU */

#endif /* __KSU_H_CPU_SPOOF */
