#include <config.h>
#include <version.h>

#define IRAM_STK_TEMP 0x0003C000
.globl nts3250_sys_init
.globl dcache_kill

.globl lowlevel_init
lowlevel_init:
	/*
	 * Setup basic stack and save return address
	 */
	ldr sp, =IRAM_STK_TEMP
	mov pc, lr

dcache_kill:
	/*
	 * The data cache may have been enabled when booting from
	 * S1l, so disable it now to prevent problems.
	 */
flush1:
	mrc p15, 0, r15, c7, c10, 3
	bne flush1
	mcr p15, 0, r0, c1, c0, 0
	bic r0, r0, #4
	mrc p15, 0, r0, c1, c0, 0
	mov pc, lr
