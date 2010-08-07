#include <common.h>
#include "lpc3250.h"
#include "spi_lpc3250.h"

void init_spi1(void)
{
	unsigned int tmp;
	// Config IO
	GPIO->p2_dir_set = OUTP_STATE_GPIO(5);
	GPIO->p2_outp_set = OUTP_STATE_GPIO(5);
	GPIO->p2_mux_clr = P2_GPIO05_SSEL0;
	GPIO->p_mux_clr = (P_SPI1DATAIO_SSP0_MOSI |
		P_SPI1DATAIN_SSP0_MISO | P_SPI1CLK_SCK0 );

	// Turn on
	tmp = CLKPWR->clkpwr_spi_clk_ctrl;
	tmp |= (CLKPWR_SPICTRL_SPI1_BLOCK|CLKPWR_SPICTRL_SPI1_CLKEN);
	CLKPWR->clkpwr_spi_clk_ctrl = tmp;

	// SPI_GLOB
	SPI1->global = SPI_GLOB_ENABLE;

	// SPI_CON
	// S25FL129P->40MHz(0x03 Read)
	// W25X40->20MHz(0x03 Read)
	// W25X40A->%0MHz(0x03 Read)
	// 100MHz/((3+1)*2)=12.5MHz
	SPI1->con = (SPI_CON_UNIDIR | SPI_CON_MODE11 | SPI_CON_RXTX |
		SPI_CON_BITNUM(8) | SPI_CON_MS | SPI_CON_RATE(3));

	// SPI_FRM
	SPI1->frm = 0;

	// SPI_IER
	SPI1->ier = 0;

	// SPI_TIM_CTRL
	SPI1->tim_ctrl = 0;
}

void spi1_cs_on(void)
{
	GPIO->p3_outp_clr = OUTP_STATE_GPIO(5);
}

void spi1_cs_off(void)
{
	GPIO->p3_outp_set = OUTP_STATE_GPIO(5);
}

void spi1_write_byte(unsigned char c)
{
	unsigned int tmp;
	tmp = SPI1->con;
	tmp |= SPI_CON_RXTX;
	SPI1->con = tmp;
	SPI1->dat = c;
	// wait to SPI_STAT_SHIFTACT be active
	for (tmp=0; tmp<10; tmp++) {
		if (SPI1->stat & SPI_STAT_SHIFTACT)
			break;
	}
	while (SPI1->stat & SPI_STAT_SHIFTACT);
}

unsigned char spi1_read_byte(void)
{
	unsigned char r;
	unsigned int tmp;
	tmp = SPI1->con;
	if (tmp & SPI_CON_RXTX) {
		tmp &= ~SPI_CON_RXTX;
		SPI1->con = tmp;
	}
	if ((SPI1->stat & SPI_STAT_BE) == 0) {
		r = SPI1->dat;
		while (SPI1->stat & SPI_STAT_BE);
	} else {
		r = SPI1->dat; // dummy read
		while (SPI1->stat & SPI_STAT_BE);
		r = SPI1->dat; // real read
		while (SPI1->stat & SPI_STAT_BE);
	}
	return r;
}
