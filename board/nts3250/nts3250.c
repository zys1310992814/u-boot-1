#include <common.h>
#include <configs/nts3250.h>
#include <asm/mach-types.h>
#include <lpc3250.h>
#include <spi_lpc3250.h>
#include <net.h>
#include "nts3250_prv.h"

DECLARE_GLOBAL_DATA_PTR;
extern int dcache_kill(void);

void reset_timer (void)
{
	unsigned int clkdlycnt, tbaseclk;

	/* Reset timer */
	TIMER_CNTR0->tcr = TIMER_CNTR_TCR_RESET;
	TIMER_CNTR0->tcr = 0;
	TIMER_CNTR0->tc = 0;

	/* Clear and enable match function */
	TIMER_CNTR0->ir = TIMER_CNTR_MTCH_BIT(0);

	/* Count mode is PCLK edge */
	TIMER_CNTR0->ctcr = TIMER_CNTR_SET_MODE(TIMER_CNTR_CTCR_TIMER_MODE);

	/* Set prescale counter value for a 1mS tick */
	tbaseclk = sys_get_rate(CLKPWR_PERIPH_CLK);
	clkdlycnt = (tbaseclk / CONFIG_SYS_HZ);
	TIMER_CNTR0->pr = clkdlycnt - 1;

	/* Enable the timer */
	TIMER_CNTR0->tcr = TIMER_CNTR_TCR_EN;
}

ulong get_timer (ulong base)
{
	ulong tcr = TIMER_CNTR0->tc;
	if (tcr >= base)
		tcr = tcr - base;
	else
		tcr = 0xFFFFFFFF - base + tcr;
	return tcr;
}

void timer_init(void)
{
	/* Enable timer system clock */
	CLKPWR->clkpwr_timers_pwms_clk_ctrl_1 |=
		CLKPWR_TMRPWMCLK_TIMER0_EN;

	reset_timer();
}

/*
 * Use the watchdog timer to reset the system
 */
void reset_cpu(ulong addr)
{
	/* Enable the WDT clock */
	CLKPWR->clkpwr_timer_clk_ctrl |= CLKPWR_PWMCLK_WDOG_EN;

	/* Force RESETOUT_N active */
	WDT->wdtim_mctrl = WDT_RESFRC1;
	WDT->wdtim_ctrl = WDT_COUNT_ENAB;

	/* Wait forever */
	while (1);
}

void udelay(unsigned long usec)
{
	unsigned long tbaseclk;

	/* Enable timer system clock */
	CLKPWR->clkpwr_timers_pwms_clk_ctrl_1 |=
		CLKPWR_TMRPWMCLK_TIMER1_EN;

	/* Reset timer */
	TIMER_CNTR1->tcr = TIMER_CNTR_TCR_RESET;
	TIMER_CNTR1->tcr = 0;
	TIMER_CNTR1->tc = 0;
	TIMER_CNTR1->pr = 0;

	/* Clear and enable match function */
	TIMER_CNTR1->ir = TIMER_CNTR_MTCH_BIT(0);

	/* Count mode is PCLK edge */
	TIMER_CNTR1->ctcr = TIMER_CNTR_SET_MODE(TIMER_CNTR_CTCR_TIMER_MODE);

	/* Set prescale counter value for a 1uS tick */
	tbaseclk = sys_get_rate(CLKPWR_PERIPH_CLK);
	tbaseclk = (tbaseclk / 1000000) - 1;
	TIMER_CNTR1->pr = tbaseclk;

	/* Set match for number of usecs */
	TIMER_CNTR1->mr[0] = usec;

	/* Interrupt on match 0 */
	TIMER_CNTR1->mcr = TIMER_CNTR_MCR_MTCH(0);

	/* Enable the timer */
	TIMER_CNTR1->tcr = TIMER_CNTR_TCR_EN;

	/* Loop until match occurs */
	while ((TIMER_CNTR1->ir & TIMER_CNTR_MTCH_BIT(0)) == 0);

	/* Stop timer */
	TIMER_CNTR1->tcr = 0;

	/* Disable timer system clock */
	CLKPWR->clkpwr_timers_pwms_clk_ctrl_1 &=
		~CLKPWR_TMRPWMCLK_TIMER1_EN;
}

/*
 * Miscellaneous platform dependent initialisations
 */

int board_init (void)
{
	/* Kill data cache */
	dcache_kill();

	/* arch number of Logic-Board - MACH_TYPE_LPC3XXX */
	gd->bd->bi_arch_number = MACH_TYPE_UBOOTSYS;

	/* adress of boot parameters */
	gd->bd->bi_boot_params = CONFIG_BOOT_PARA_ADDR;

#ifdef CONFIG_CMD_NAND
	/* Enable clocks to the SLC NAND controller */
	CLKPWR->clkpwr_nand_clk_ctrl = 0x05;

	/* Reset SLC NAND controller & clear ECC */
	SLCNAND->slc_ctrl = (SLCCTRL_SW_RESET | SLCCTRL_ECC_CLEAR);

	/* 8-bit bus, no DMA, CE normal */
	SLCNAND->slc_cfg = 0;

	/* Interrupts disabled and cleared */
	SLCNAND->slc_ien = 0;
	SLCNAND->slc_icr = (SLCSTAT_INT_TC | SLCSTAT_INT_RDY_EN);

	// TBD fix me with optimal timings later
	/* Start with slowest timings possible */
	SLCNAND->slc_tac = (SLCTAC_WDR(15) |
		SLCTAC_WWIDTH(15) |
		SLCTAC_WHOLD(15) |
		SLCTAC_WSETUP(15) |
		SLCTAC_RDR(15) |
		SLCTAC_RWIDTH(15) |
		SLCTAC_RHOLD(15) |
		SLCTAC_RSETUP(15));
#endif

	init_spi1();
	return 0;
}

int dram_init (void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_SIZE;
	return 0;
}

