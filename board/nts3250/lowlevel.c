#include <lpc3250.h>
/* EMC Static Configuration */
// tHCLK = 10ns
#define EMC_CS0_WIDTH		EMC_STC_MEMWIDTH_16
#define EMC_CS0_CS_TO_WE	4
#define EMC_CS0_CS_TO_OE	4
#define EMC_CS0_CS_TO_RD	18
#define EMC_CS0_CS_TO_RD_PM	0
#define EMC_CS0_CS_TO_WR	18
#define EMC_CS0_OE_TO_HIZ	4

#define EMC_CS1_WIDTH		EMC_STC_MEMWIDTH_16
#define EMC_CS1_CS_TO_WE	4
#define EMC_CS1_CS_TO_OE	4
#define EMC_CS1_CS_TO_RD	18
#define EMC_CS1_CS_TO_RD_PM	0
#define EMC_CS1_CS_TO_WR	18
#define EMC_CS1_OE_TO_HIZ	4

#define EMC_CS2_WIDTH		EMC_STC_MEMWIDTH_8
#define EMC_CS2_CS_TO_WE	6
#define EMC_CS2_CS_TO_OE	6
#define EMC_CS2_CS_TO_RD	20
#define EMC_CS2_CS_TO_RD_PM	0
#define EMC_CS2_CS_TO_WR	20
#define EMC_CS2_OE_TO_HIZ	4

#define EMC_CS3_WIDTH		EMC_STC_MEMWIDTH_16
#define EMC_CS3_CS_TO_WE	6
#define EMC_CS3_CS_TO_OE	6
#define EMC_CS3_CS_TO_RD	20
#define EMC_CS3_CS_TO_RD_PM	0
#define EMC_CS3_CS_TO_WR	20
#define EMC_CS3_OE_TO_HIZ	4

/*
 * EMC SDRAM Configuration
 *
 * IS42S16160C
 * 4,194,304 Words x 4 Banks x 16 Bits Organization
 * CAS Latency: 2 and 3
 * 8K Refresh Cycles/64 mS
 * IS42S16160C x 1 : 0000 1110 0x0D
 * IS42S16160C x 2 : 1000 1110 0x8D
 */

#define SDRAM_CAS	0x3
#define SDRAM_RAS	0x2

#define SDRAM_OP	0x0
#define SDRAM_BT	0x0
#define SDRAM_BL	0x1 // LPC3250 UM P136 0 for 32-bit, 1 for 16-bit

#define SDRAM_MODE	(((SDRAM_CAS<<4)|SDRAM_BL)<<10) // 1_CHIP_16BIT_4BANK_13ROW_9COL

#define SDRAM_CFG_MD	0x2D // See UM10326 7.19

#define SDRAM_TRP	3 // 20ns
#define SDRAM_TRAS	6 // 45ns
#define SDRAM_TSREX	9 // for ddr
#define SDRAM_TWR	2 // 2clk
#define SDRAM_TRC	8 // 65ns
#define SDRAM_TRFC	11 // 75ns
#define SDRAM_TXSR	9 // 75ns
#define SDRAM_TRRD	2 // 15ns
#define SDRAM_TMRD	2 // 2clk
#define SDRAM_TCDLR	1 // 1clk
#define SDRAM_REFRESH	650 // (64ms/8192/10ns)/1.2 = 650clk

/***********************************************************************
 Clock
***********************************************************************/
void init_clock(void)
{
	unsigned int tmp;
	CLKPWR->clkpwr_sysclk_ctrl = CLKPWR_SYSCTRL_BP_TRIG(0x50);
	CLKPWR->clkpwr_hclkpll_ctrl = (CLKPWR_HCLKPLL_POWER_UP |
		CLKPWR_HCLKPLL_POSTDIV_BYPASS | CLKPWR_HCLKPLL_PREDIV_PLUS1(1) |
		CLKPWR_HCLKPLL_PLLM(32));
	while ((CLKPWR->clkpwr_hclkpll_ctrl&CLKPWR_HCLKPLL_PLL_STS)==0);
	CLKPWR->clkpwr_hclk_div = (CLKPWR_HCLKDIV_DDRCLK_STOP |
		CLKPWR_HCLKDIV_PCLK_DIV(15) | CLKPWR_HCLKDIV_DIV_2POW(1));
	tmp = CLKPWR->clkpwr_pwr_ctrl;
	tmp &= ~CLKPWR_CTRL_FORCE_PCLK;
	CLKPWR->clkpwr_pwr_ctrl = tmp;
	tmp |= CLKPWR_SELECT_RUN_MODE;
	CLKPWR->clkpwr_pwr_ctrl = tmp;
}


inline void sdram_adjust_timing(void)
{
	EMC->emcdynamictrp     = EMC_DYN_PRE_CMD_PER(SDRAM_TRP - 1);
	EMC->emcdynamictras    = EMC_DYN_ACTPRE_CMD_PER(SDRAM_TRAS - 1);
	EMC->emcdynamictsrex   = EMC_DYN_SELF_RFSH_EXIT(SDRAM_TSREX - 1);
	EMC->emcdynamictwr     = EMC_DYN_WR_RECOVERT_TIME(SDRAM_TWR - 1);
	EMC->emcdynamictrc     = EMC_DYN_ACT2CMD_PER(SDRAM_TRC - 1);
	EMC->emcdynamictrfc    = EMC_DYN_AUTOREFRESH_PER(SDRAM_TRFC - 1);
	EMC->emcdynamictxsr    = EMC_DYN_EXIT_SRFSH_TIME(SDRAM_TXSR - 1);
	EMC->emcdynamictrrd    = EMC_DYN_BANKA2BANKB_LAT(SDRAM_TRRD - 1);
	EMC->emcdynamictmrd    = EMC_DYN_LM2ACT_CMD_TIME(SDRAM_TMRD - 1);
	EMC->emcdynamictcdlr   = EMC_DYN_LASTDIN_CMD_TIME(SDRAM_TCDLR - 1);
	EMC->emcdynamicrefresh = EMC_DYN_REFRESH_IVAL(SDRAM_REFRESH);
}

void setup_sdram(void)
{
	volatile unsigned int tmp, tmp32;
	CLKPWR->clkpwr_sdramclk_ctrl = CLKPWR_SDRCLK_HCLK_DLY(0x0E);
	EMC->emccontrol = EMC_DYN_SDRAM_CTRL_EN;
	EMC->emcconfig = 0;
	EMC->emcdynamicconfig0 = (SDRAM_CFG_MD<<7) | EMC_DYN_DEV_LP_SDR_SDRAM;
	EMC->emcdynamicrascas0 = (EMC_SET_CAS_IN_HALF_CYCLES(SDRAM_CAS*2) |
			EMC_SET_RAS_IN_CYCLES(SDRAM_RAS));
	EMC->emcdynamicreadconfig = (EMC_SDR_CLK_NODLY_CMD_DEL |
			EMC_SDR_READCAP_POS_POL | EMC_SDR_CLK_DLY_CMD_NODELY);
	sdram_adjust_timing();
	tmp = (EMC_DYN_CLK_ALWAYS_ON | EMC_DYN_CLKEN_ALWAYS_ON |
			EMC_DYN_DIS_INV_MEMCLK);
	EMC->emcdynamiccontrol = (tmp | EMC_DYN_NOP_MODE);
	EMC->emcdynamiccontrol = (tmp | EMC_DYN_PALL_MODE);
	EMC->emcdynamicrefresh = EMC_DYN_REFRESH_IVAL(128);
	sdram_adjust_timing();
	EMC->emcdynamiccontrol = (tmp | EMC_DYN_CMD_MODE);
	tmp32 = * (volatile unsigned int *)(EMC_DYCS0_BASE + SDRAM_MODE);
	EMC->emcdynamiccontrol = tmp;
}

void setup_stc(void)
{
	EMC->emcstatic_regs[0].emcstaticconfig  = (EMC_CS0_WIDTH | EMC_STC_BLS_EN_BIT);
	EMC->emcstatic_regs[0].emcstaticwaitwen = (EMC_CS0_CS_TO_WE - 1);
	EMC->emcstatic_regs[0].emcstaticwait0en = (EMC_CS0_CS_TO_OE);
	EMC->emcstatic_regs[0].emcstaticwaitrd  = (EMC_CS0_CS_TO_RD - 1);
	EMC->emcstatic_regs[0].emcstaticpage    = (EMC_CS0_CS_TO_RD_PM - 1);
	EMC->emcstatic_regs[0].emcstaticwr      = (EMC_CS0_CS_TO_WR - 2);
	EMC->emcstatic_regs[0].emcstaticturn    = (EMC_CS0_OE_TO_HIZ - 1);
	EMC->emcstatic_regs[1].emcstaticconfig  = (EMC_CS1_WIDTH | EMC_STC_BLS_EN_BIT);
	EMC->emcstatic_regs[1].emcstaticwaitwen = (EMC_CS1_CS_TO_WE - 1);
	EMC->emcstatic_regs[1].emcstaticwait0en = (EMC_CS1_CS_TO_OE);
	EMC->emcstatic_regs[1].emcstaticwaitrd  = (EMC_CS1_CS_TO_RD - 1);
	EMC->emcstatic_regs[1].emcstaticpage    = (EMC_CS1_CS_TO_RD_PM - 1);
	EMC->emcstatic_regs[1].emcstaticwr      = (EMC_CS1_CS_TO_WR - 2);
	EMC->emcstatic_regs[1].emcstaticturn    = (EMC_CS1_OE_TO_HIZ - 1);
	EMC->emcstatic_regs[2].emcstaticconfig  = (EMC_CS2_WIDTH | EMC_STC_BLS_EN_BIT);
	EMC->emcstatic_regs[2].emcstaticwaitwen = (EMC_CS2_CS_TO_WE - 1);
	EMC->emcstatic_regs[2].emcstaticwait0en = (EMC_CS2_CS_TO_OE);
	EMC->emcstatic_regs[2].emcstaticwaitrd  = (EMC_CS2_CS_TO_RD - 1);
	EMC->emcstatic_regs[2].emcstaticpage    = (EMC_CS2_CS_TO_RD_PM - 1);
	EMC->emcstatic_regs[2].emcstaticwr      = (EMC_CS2_CS_TO_WR - 2);
	EMC->emcstatic_regs[2].emcstaticturn    = (EMC_CS2_OE_TO_HIZ - 1);
	EMC->emcstatic_regs[3].emcstaticconfig  = (EMC_CS3_WIDTH | EMC_STC_BLS_EN_BIT);
	EMC->emcstatic_regs[3].emcstaticwaitwen = (EMC_CS3_CS_TO_WE - 1);
	EMC->emcstatic_regs[3].emcstaticwait0en = (EMC_CS3_CS_TO_OE);
	EMC->emcstatic_regs[3].emcstaticwaitrd  = (EMC_CS3_CS_TO_RD - 1);
	EMC->emcstatic_regs[3].emcstaticpage    = (EMC_CS3_CS_TO_RD_PM - 1);
	EMC->emcstatic_regs[3].emcstaticwr      = (EMC_CS3_CS_TO_WR - 2);
	EMC->emcstatic_regs[3].emcstaticturn    = (EMC_CS3_OE_TO_HIZ - 1);
}


void init_emc(void)
{
	/* Mirror IRAM at address 0x0 */
	CLKPWR->clkpwr_bootmap = CLKPWR_BOOTMAP_SEL_BIT;

	/* Disable buffers in AHB ports */
	EMC->emcahn_regs [0].emcahbcontrol = 0;
	EMC->emcahn_regs [3].emcahbcontrol = 0;
	EMC->emcahn_regs [4].emcahbcontrol = 0;

	/* Enable port timeouts */
	EMC->emcahn_regs [0].emcahbtimeout = EMC_AHB_SET_TIMEOUT(400);
	EMC->emcahn_regs [3].emcahbtimeout = EMC_AHB_SET_TIMEOUT(400);
	EMC->emcahn_regs [4].emcahbtimeout = EMC_AHB_SET_TIMEOUT(400);

	/* Setup Static Memeory Controller */
	setup_stc();

	/* Setup SDRAM Controller */
	setup_sdram();

	/* Enable buffers in AHB ports */
	EMC->emcahn_regs [0].emcahbcontrol = EMC_AHB_PORTBUFF_EN;
	EMC->emcahn_regs [3].emcahbcontrol = EMC_AHB_PORTBUFF_EN;
	EMC->emcahn_regs [4].emcahbcontrol = EMC_AHB_PORTBUFF_EN;
}


void  nts3250_lowlevel_init(void)
{
	init_clock();
	init_emc();
}
