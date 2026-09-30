/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __KSU_H_CPU_SPOOF
#define __KSU_H_CPU_SPOOF

#include "../uapi/supercall.h"
#include <linux/types.h>
#include "../include/ksu.h"

/* 前置声明结构体，防止 -Wvisibility 局部作用域警告与类型冲突 */
struct ksu_set_spoof_cpu_cmd;

int ksu_set_spoof_cpu(const struct ksu_set_spoof_cpu_cmd *cmd);

#endif /* __KSU_H_CPU_SPOOF */
