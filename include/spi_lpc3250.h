#ifndef __SPI_LPC3250_H__
#define __SPI_LPC3250_H__

//CMD Code For W25X40A and S25FL129

#define SF_CMD_READ		0x03
#define SF_CMD_READ_ID		0x90
#define SF_CMD_PP		0x02
#define SF_CMD_RDSR		0x05
#define SF_CMD_WREN		0x06
#define SF_CMD_BE		0xC7
#define SF_CMD_SE		0xD8
#define BLOCK_SIZE		0x10000
#define BLOCK_MASK		(~0xFFFF)
#define PAGE_SIZE		0x100
#define PAGE_MASK		(~0xFF)


void init_spi1(void);

#define spi1_cs_on()	spi1_cs(1)	//兼容考虑，推荐直接调用spi1_cs(1)
#define spi1_cs_off()	spi1_cs(0)	//兼容考虑，推荐直接调用spi1_cs(0)
void spi1_cs(int active);

void spi1_write_byte(unsigned char c);
unsigned char spi1_read_byte(void);
void spi_flash_read(unsigned int src, unsigned char *des, unsigned int cnt);
void install_spi_to_norflash(void);

#endif // __SPI_LPC3250_H__
