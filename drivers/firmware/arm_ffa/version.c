// SPDX-License-Identifier: GPL-2.0-only
#define pr_fmt(fmt) "ARM FF-A: " fmt

#include <linux/arm-smccc.h>
#include <linux/arm_ffa.h>
#include <linux/cleanup.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/mutex.h>

static DEFINE_MUTEX(ffa_version_lock);
static int ffa_version;

int arm_ffa_version_get(void (*invoke)(const struct arm_smccc_1_2_regs *args,
				       struct arm_smccc_1_2_regs *res),
			u32 input_version, u32 *version)
{
	struct arm_smccc_1_2_regs args = {
		.a0 = FFA_VERSION,
		.a1 = input_version,
	}, res;

	guard(mutex)(&ffa_version_lock);
	if (!ffa_version) {
		invoke(&args, &res);

		if ((s32)res.a0 == FFA_RET_NOT_SUPPORTED)
			ffa_version = -EOPNOTSUPP;
		else
			ffa_version = res.a0;
	}

	if (ffa_version < 0)
		return ffa_version;

	*version = ffa_version;
	return 0;
}
EXPORT_SYMBOL_GPL(arm_ffa_version_get);
