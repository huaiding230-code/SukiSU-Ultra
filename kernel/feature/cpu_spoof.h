/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __KSU_H_CPU_SPOOF
#define __KSU_H_CPU_SPOOF

#include "uapi/supercall.h"
#include <linux/types.h>
#include "../include/ksu.h"

// 在这里补上结构体定义，解决可见性与不完整类型报错
struct ksu_set_spoof_cpu_cmd {
    unsigned int cpu_index;
    unsigned int midr;
    unsigned long bogomips;
    unsigned long hwcap;
    unsigned long hwcap2;
};

int ksu_set_spoof_cpu(const struct ksu_set_spoof_cpu_cmd *cmd);

#endif /* __KSU_H_CPU_SPOOF */
