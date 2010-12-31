/*
::::    :::: ::::::::::::    .::::::    Company    : NTS-intl
 :::     ::   ::  ::  ::   ::      ::   Author     : Athurg.Feng
 ::::    ::       ::        ::          Maintainer : Athurg.Feng
 :: ::   ::       ::         ::         Project    : 
 ::  ::  ::       ::           :::      FileName   : lowlevel.c
 ::   :: ::       ::             ::     Generate   : 
 ::    ::::       ::       ::      ::   Update     : 2010-12-31 16:26:06
::::    :::     ::::::      ::::::::    Version    : 0.0.1

Description:
		本文件包含的是LPC32xx硬件低级初始化代码，用于配置时钟、EMC
	控制器（SDRAM）等设备。
		nts3250_lowlevel_init()函数是本文件的入口函数，其他函数被此
	函数调用。nts3250_lowlevel_init()被lowlevel_init.S中的汇编语言调用。
	主要作用是在uboot启动的stage1阶段初始化时钟和SDRAM，为后面的relocate
	复制uboot到SDRAM作准备。

Changelog:
	V0.0.1	版本发布
*/
#include <lpc3250.h>

//EMC配置，具体配置含义及内容参考《BSP详细设计报告》
/* 静态存储控制器相关配置 */
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
 * EMC SDRAM相关配置
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

/*
 * 	时钟初始化
 *
 * 说明：
 * 	配置值及含义参考LPC32XX用户手册及《BSP详细设计报告》
 */

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


static inline void sdram_adjust_timing(void)
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
/*
 * 	EMC动态总线配置
 *
 * 说明：
 * 	用于初始化动态存储设备控制器（Dynamic Memory Control）。
 * 	SDRAM联接在该控制器上。
 */
static inline void setup_sdram(void)
{
	volatile unsigned int tmp, tmp32;

	//设置HCLK延时
	CLKPWR->clkpwr_sdramclk_ctrl = CLKPWR_SDRCLK_HCLK_DLY(0x0E);

	EMC->emccontrol = EMC_DYN_SDRAM_CTRL_EN;
	EMC->emcconfig = 0;

	EMC->emcdynamicconfig0 = (SDRAM_CFG_MD<<7) | EMC_DYN_DEV_LP_SDR_SDRAM;

	EMC->emcdynamicrascas0 = (EMC_SET_CAS_IN_HALF_CYCLES(SDRAM_CAS*2) |
			EMC_SET_RAS_IN_CYCLES(SDRAM_RAS));
	EMC->emcdynamicreadconfig = (EMC_SDR_CLK_NODLY_CMD_DEL |
			EMC_SDR_READCAP_POS_POL | EMC_SDR_CLK_DLY_CMD_NODELY);

	//根据新HCLK配置重新调整SDRAM
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

/*
 * 	EMC静态总线配置
 *
 * 说明：
 * 	用于初始化静态存储设备控制器（Static Memory Control）。
 * 	包括NorFlash、CPLD、FPGA等设备都是联接在该控制器上。
 * 	具体初始化值及作用参考本文件头的相关宏定义
 */
static inline void setup_stc(void)
{
	//EMC_CS0对应的片选空间
	EMC->emcstatic_regs[0].emcstaticconfig  = (EMC_CS0_WIDTH | EMC_STC_BLS_EN_BIT);
	EMC->emcstatic_regs[0].emcstaticwaitwen = (EMC_CS0_CS_TO_WE - 1);
	EMC->emcstatic_regs[0].emcstaticwait0en = (EMC_CS0_CS_TO_OE);
	EMC->emcstatic_regs[0].emcstaticwaitrd  = (EMC_CS0_CS_TO_RD - 1);
	EMC->emcstatic_regs[0].emcstaticpage    = (EMC_CS0_CS_TO_RD_PM - 1);
	EMC->emcstatic_regs[0].emcstaticwr      = (EMC_CS0_CS_TO_WR - 2);
	EMC->emcstatic_regs[0].emcstaticturn    = (EMC_CS0_OE_TO_HIZ - 1);

	//EMC_CS1对应的片选空间
	EMC->emcstatic_regs[1].emcstaticconfig  = (EMC_CS1_WIDTH | EMC_STC_BLS_EN_BIT);
	EMC->emcstatic_regs[1].emcstaticwaitwen = (EMC_CS1_CS_TO_WE - 1);
	EMC->emcstatic_regs[1].emcstaticwait0en = (EMC_CS1_CS_TO_OE);
	EMC->emcstatic_regs[1].emcstaticwaitrd  = (EMC_CS1_CS_TO_RD - 1);
	EMC->emcstatic_regs[1].emcstaticpage    = (EMC_CS1_CS_TO_RD_PM - 1);
	EMC->emcstatic_regs[1].emcstaticwr      = (EMC_CS1_CS_TO_WR - 2);
	EMC->emcstatic_regs[1].emcstaticturn    = (EMC_CS1_OE_TO_HIZ - 1);

	//EMC_CS2对应的片选空间
	EMC->emcstatic_regs[2].emcstaticconfig  = (EMC_CS2_WIDTH | EMC_STC_BLS_EN_BIT);
	EMC->emcstatic_regs[2].emcstaticwaitwen = (EMC_CS2_CS_TO_WE - 1);
	EMC->emcstatic_regs[2].emcstaticwait0en = (EMC_CS2_CS_TO_OE);
	EMC->emcstatic_regs[2].emcstaticwaitrd  = (EMC_CS2_CS_TO_RD - 1);
	EMC->emcstatic_regs[2].emcstaticpage    = (EMC_CS2_CS_TO_RD_PM - 1);
	EMC->emcstatic_regs[2].emcstaticwr      = (EMC_CS2_CS_TO_WR - 2);
	EMC->emcstatic_regs[2].emcstaticturn    = (EMC_CS2_OE_TO_HIZ - 1);

	//EMC_CS3对应的片选空间
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
	/* 将地址0x00映射到IRAM中（初始为IROM） */
	CLKPWR->clkpwr_bootmap = CLKPWR_BOOTMAP_SEL_BIT;

	/* 关闭AHB端口缓冲 */
	EMC->emcahn_regs [0].emcahbcontrol = 0;
	EMC->emcahn_regs [3].emcahbcontrol = 0;
	EMC->emcahn_regs [4].emcahbcontrol = 0;

	/* 开启端口超时 */
	EMC->emcahn_regs [0].emcahbtimeout = EMC_AHB_SET_TIMEOUT(400);
	EMC->emcahn_regs [3].emcahbtimeout = EMC_AHB_SET_TIMEOUT(400);
	EMC->emcahn_regs [4].emcahbtimeout = EMC_AHB_SET_TIMEOUT(400);

	/* 配置静态存储控制器（NorFlash） */
	setup_stc();

	/* 配置动态存储控制器（SDRAM） */
	setup_sdram();

	/* 重新开启AHB端口缓冲 */
	EMC->emcahn_regs [0].emcahbcontrol = EMC_AHB_PORTBUFF_EN;
	EMC->emcahn_regs [3].emcahbcontrol = EMC_AHB_PORTBUFF_EN;
	EMC->emcahn_regs [4].emcahbcontrol = EMC_AHB_PORTBUFF_EN;
}

void init_uart5(void)
{
	unsigned int tmp;
	/* Turn CLK on, CLKPWR_UART_USE_HCLK -> 1, use hclk */
	tmp = CLKPWR->clkpwr_uart_clk_ctrl;
	tmp |= CLKPWR_UARTCLKCTRL_UART5_EN;
	CLKPWR->clkpwr_uart_clk_ctrl;

	/* CLK mode */
	tmp = UARTCNTL->clkmode;
	tmp &= ~UART_CLKMODE_MASK(5);
	tmp |= UART_CLKMODE_LOAD(UART_CLKMODE_AUTO, 5);
	UARTCNTL->clkmode = tmp;

	/*
	Config 115200
	CLKPWR_UART_Y_DIV(y)  -> 134
	CLKPWR_UART_X_DIV(x)  -> 19
	CLKPWR_UART_USE_HCLK  -> 0, PCLK
	*/
	CLKPWR->clkpwr_uart5_clk_ctrl = (CLKPWR_UART_Y_DIV(134) |
		CLKPWR_UART_X_DIV(19));

	/* Config 8N1, UART_LCR_STOP1BIT -> y, UART_LCR_WLEN_8BITS -> y */
	UART5->lcr = (UART_LCR_STOP1BIT | UART_LCR_WLEN_8BITS);

	/* Reset TX RX FIFO */
	tmp = UART5->iir_fcr;
	tmp |= (UART_FCR_TXFIFO_FLUSH |UART_FCR_RXFIFO_FLUSH);
	UART5->iir_fcr = tmp;
}

void uart5_send_char(unsigned char c)
{
	while ((UART5->lsr&UART_LSR_TEMT)==0);
	UART5->dll_fifo = c;
}

void puth(unsigned int d, int n)
{
	char s[8];
	int i;
	for (i=0; i<n; i++) {
		s[i] = (d>>(4*i))&0xF;
		if (s[i]>=10) {
			s[i] = s[i]+'A'-10;
		} else {
			s[i] = s[i]+'0';
		}
	}
	for (i=n-1; i>=0; i--) {
		uart5_send_char(s[i]);
	}
}
void lowlevel_puts(char * str)
{
	char *p = str;
	while(*p){
		uart5_send_char(*p);
		p++;
	}
}

/*
 * 参数由start.S中传入
 * addr：存储于R0中，其值为u-boot的运行起始地址，程序运行时通过adr指令获取
 * length:存储于R1中，其值为u-boot程序镜像的代码部分大小（从__armboot_start到_bss_start），在链接的时候确认
 */
void  nts3250_lowlevel_init(unsigned int addr, unsigned int length)
{
	extern void stack_setup(void);//定义在cpu/arm926ejs/start.S中

	init_uart5();

	if ((addr<0xE0000000) && (addr>0x10000000)) { //SDRAM
		uart5_send_char('S');
	} else {
		init_clock();
		init_emc();
		if (addr < 0x10000000) {//IRAM（SPI）通过SPI接口实现relocate，然后直接跳到堆栈设置段
			uart5_send_char('I');
			init_spi1();
			spi_flash_read(0x4, (char *)TEXT_BASE, length);
			stack_setup();
			//Never reach here
		} else {
			uart5_send_char('F');
		}
	}
}
