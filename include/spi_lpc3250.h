#ifndef __SPI_LPC3250_H__
#define __SPI_LPC3250_H__

void init_spi1(void);
void spi1_cs_on(void);
void spi1_cs_off(void);
void spi1_write_byte(unsigned char c);
unsigned char spi1_read_byte(void);

#endif // __SPI_LPC3250_H__