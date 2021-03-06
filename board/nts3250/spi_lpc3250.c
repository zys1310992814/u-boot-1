#include <common.h>
#include <spi_lpc3250.h>
#include "lpc3250.h"

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

#define spi1_cs_on()	spi1_cs(1)
#define spi1_cs_off()	spi1_cs(0)
void spi1_cs(int active)
{
	if (active) {
		GPIO->p3_outp_clr = OUTP_STATE_GPIO(5);
	} else {
		GPIO->p3_outp_set = OUTP_STATE_GPIO(5);
	}
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

void spi_flash_read(unsigned int src, unsigned char *des, unsigned int cnt)
{
	int i;
	spi1_cs_on();

	spi1_write_byte(SF_CMD_READ);
	spi1_write_byte((src>>16) & 0xFF);
	spi1_write_byte((src>>8) & 0xFF);
	spi1_write_byte((src) & 0xFF);

	for (i=0; i<cnt; i++) {
		des[i] = spi1_read_byte();
	}

	spi1_cs_off();
}

void install_spi_to_norflash(void)
{
	char *src;
	unsigned int dest, end, len;

	flash_init();

	//UBoot
	printf("<1/3> Install uboot...\r\n");
	{
		src  = (char *)_armboot_start;
		dest = CONFIG_SYS_FLASH_BASE;
		len  = _bss_start - _armboot_start;

		end    = (dest+len) | (CONFIG_SYS_FLASH_SECT_SIZE-1);

		//fix magic number of LPC32xx
		*((unsigned int *)src) = 0x13579BD1;

		flash_sect_erase(dest, end);
		flash_write(src, dest, len);
		printf("Finish\r\n");
	}

	//kernel
	printf("<2/3> Install kernel...\r\n");
	install_spi_applet_to_norflash(CONFIG_KERNEL_OFFSET);

	//Appfs partition
	printf("<3/3> Install Appfs...\r\n");
	install_spi_applet_to_norflash(CONFIG_APPFS_OFFSET);
}

void install_spi_applet_to_norflash(unsigned int offset)
{
	struct{
		unsigned int crc;
		unsigned int len;
	}hdr;
	unsigned int startaddr,endaddr;

	//读取头部信息，并检查镜像大小
	spi_flash_read(offset, (char *)&hdr, sizeof(hdr));
	if (hdr.len==0 || hdr.len==0xFFFFFFFF) {
		printf("Fail: Length invalid!\r\n");
		return -1;
	}

	//读取镜像内容，并校验镜像CRC
	spi_flash_read(offset+sizeof(hdr), CONFIG_SYS_LOAD_ADDR, hdr.len);
	if (hdr.crc != crc32(0, (char *)CONFIG_SYS_LOAD_ADDR, hdr.len)) {
		printf("Fail:CRC check failed!\r\n");
		return -2;
	}

	//擦除并写入
	startaddr = CONFIG_SYS_FLASH_BASE+offset;
	endaddr   = (startaddr+hdr.len)  | (CONFIG_SYS_FLASH_SECT_SIZE-1);//sect_size-1 means mask of sector addr
	flash_sect_erase(startaddr, endaddr);
	flash_write(CONFIG_SYS_LOAD_ADDR, startaddr, hdr.len);

	printf("OK!\r\n");
	return 0;
}

