#include <linux/version.h>
#include <linux/types.h>
#include <linux/slab.h>

#ifndef __KSU_H_SEPOLICY
#define __KSU_H_SEPOLICY

#include "sepolicy.h"
#include "ss/policydb.h"

#ifndef KSU_SEPOLICY_CMD_NORMAL_PERM
#define KSU_SEPOLICY_CMD_NORMAL_PERM 0
#define KSU_SEPOLICY_CMD_XPERM 1
#define KSU_SEPOLICY_CMD_TYPE_STATE 2
#define KSU_SEPOLICY_CMD_TYPE 3
#define KSU_SEPOLICY_CMD_TYPE_ATTR 4
#define KSU_SEPOLICY_CMD_ATTR 5
#define KSU_SEPOLICY_CMD_TYPE_TRANSITION 6
#define KSU_SEPOLICY_CMD_TYPE_CHANGE 7
#define KSU_SEPOLICY_CMD_GENFSCON 8
#endif

#ifndef KSU_SEPOLICY_SUBCMD_NORMAL_PERM_ALLOW
#define KSU_SEPOLICY_SUBCMD_NORMAL_PERM_ALLOW 0
#define KSU_SEPOLICY_SUBCMD_NORMAL_PERM_DENY 1
#define KSU_SEPOLICY_SUBCMD_NORMAL_PERM_AUDITALLOW 2
#define KSU_SEPOLICY_SUBCMD_NORMAL_PERM_DONTAUDIT 3
#define KSU_SEPOLICY_SUBCMD_XPERM_ALLOW 4
/* === 在这里插入缺失的宏定义 === */
#define KSU_SEPOLICY_SUBCMD_XPERM_AUDITALLOW 5
#define KSU_SEPOLICY_SUBCMD_XPERM_DONTAUDIT 6
#define KSU_SEPOLICY_SUBCMD_TYPE_STATE_PERMISSIVE 7
#define KSU_SEPOLICY_SUBCMD_TYPE_STATE_ENFORCE 8
#define KSU_SEPOLICY_SUBCMD_TYPE_CHANGE_CHANGE 9
#define KSU_SEPOLICY_SUBCMD_TYPE_CHANGE_MEMBER 10
/* ============================== */

#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
struct selinux_policy *ksu_dup_sepolicy(struct selinux_policy *old_pol);

void ksu_destroy_sepolicy(struct selinux_policy *orig);
#endif

// Operation on types
bool ksu_type(struct policydb *db, const char *name, const char *attr);
bool ksu_attribute(struct policydb *db, const char *name);
bool ksu_permissive(struct policydb *db, const char *type);
bool ksu_enforce(struct policydb *db, const char *type);
bool ksu_typeattribute(struct policydb *db, const char *type, const char *attr);
bool ksu_exists(struct policydb *db, const char *type);

// Access vector rules
bool ksu_allow(struct policydb *db, const char *src, const char *tgt, const char *cls, const char *perm);
bool ksu_deny(struct policydb *db, const char *src, const char *tgt, const char *cls, const char *perm);
bool ksu_auditallow(struct policydb *db, const char *src, const char *tgt, const char *cls, const char *perm);
bool ksu_dontaudit(struct policydb *db, const char *src, const char *tgt, const char *cls, const char *perm);

// Extended permissions access vector rules
bool ksu_allowxperm(struct policydb *db, const char *src, const char *tgt, const char *cls, const char *range);
bool ksu_auditallowxperm(struct policydb *db, const char *src, const char *tgt, const char *cls, const char *range);
bool ksu_dontauditxperm(struct policydb *db, const char *src, const char *tgt, const char *cls, const char *range);

// Type rules
bool ksu_type_transition(struct policydb *db, const char *src, const char *tgt, const char *cls, const char *def,
                         const char *obj);
bool ksu_type_change(struct policydb *db, const char *src, const char *tgt, const char *cls, const char *def);
bool ksu_type_member(struct policydb *db, const char *src, const char *tgt, const char *cls, const char *def);

// File system labeling
bool ksu_genfscon(struct policydb *db, const char *fs_name, const char *path, const char *ctx);

#endif
