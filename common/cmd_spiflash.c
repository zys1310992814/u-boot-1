#include <common.h>
#include <command.h>
#include <spi_lpc3250.h>

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

unsigned int time_flag = 0;
unsigned int time_out = 0x2000;

void clear_time_flag(void)
{
	time_flag = 0;
}

void set_time_out(unsigned int t)
{
	time_out = t;
}

void print_dot(void)
{
	if (time_flag == 0) {
		printf(".");
	}
	time_flag += 1;
	if (time_flag > time_out) {
		time_flag = 0;
	}
}

void spi_flash_wait_busy(void)
{
	unsigned char t;
	while (1) {
		spi1_cs_on();
		spi1_write_byte(SF_CMD_RDSR);
		t = spi1_read_byte();
		spi1_cs_off();
		print_dot();
		if ((t&0x01)==0x00) {
			break;
		}
		udelay(1);
	}
}

void spi_flash_info(unsigned char *id)
{
	spi1_cs_on();
	spi1_write_byte(SF_CMD_READ_ID);
	spi1_write_byte(0x00);
	spi1_write_byte(0x00);
	spi1_write_byte(0x00);
	id[0] = spi1_read_byte();
	id[1] = spi1_read_byte();
	spi1_cs_off();
	udelay(1);
}

void spi_flash_bulk_erase(void)
{
	set_time_out(10000);
	clear_time_flag();
	spi1_cs_on();
	spi1_write_byte(SF_CMD_WREN);
	spi1_cs_off();
	udelay(1);

	spi1_cs_on();
	spi1_write_byte(SF_CMD_BE);
	spi1_cs_off();
	udelay(1);

	spi_flash_wait_busy();
}

void spi_flash_sector_erase(unsigned int addr, int cnt)
{
	int i;
	set_time_out(10000);
	clear_time_flag();
	addr &= BLOCK_MASK;
	for (i=0; i<cnt; i+=BLOCK_SIZE) {
		spi1_cs_on();
		spi1_write_byte(SF_CMD_WREN);
		spi1_cs_off();
		udelay(1);

		spi1_cs_on();
		spi1_write_byte(SF_CMD_SE);
		spi1_write_byte((addr>>16) & 0xFF);
		spi1_write_byte((addr>>8) & 0xFF);
		spi1_write_byte(addr & 0xFF);
		spi1_cs_off();
		udelay(1);

		spi_flash_wait_busy();
		addr += BLOCK_SIZE;
	}
}

void spi_flash_write(unsigned char *src, unsigned int des, int cnt)
{
	int i;
	unsigned int size;

	set_time_out(120000);
	clear_time_flag();
	des &= PAGE_MASK;
	while (cnt > 0) {
		// We can only write a continue page
		size = (cnt > PAGE_SIZE) ? PAGE_SIZE : cnt;

		// Set write enable
		spi1_cs_on();
		spi1_write_byte(SF_CMD_WREN);
		spi1_cs_off();
		udelay(1);

		// Write command and address bytes
		spi1_cs_on();
		spi1_write_byte(SF_CMD_PP);
		spi1_write_byte((des>>16) & 0xFF);
		spi1_write_byte((des>>8) & 0xFF);
		spi1_write_byte(des & 0xFF);

		// write data
		for (i=0; i<size; i++) {
			spi1_write_byte(src[i]);
			print_dot();
		}

		spi1_cs_off();
		udelay(1);
		spi_flash_wait_busy();

		cnt -= size;
		src += size;
		des += size;
	}
}

void spi_flash_read(unsigned int src, unsigned char *des, int cnt)
{
	int i;
	set_time_out(120000);
	clear_time_flag();
	spi1_cs_on();
	spi1_write_byte(SF_CMD_READ);
	spi1_write_byte((src>>16) & 0xFF);
	spi1_write_byte((src>>8) & 0xFF);
	spi1_write_byte((src) & 0xFF);

	for (i=0; i<cnt; i++) {
		des[i] = spi1_read_byte();
		print_dot();
	}

	spi1_cs_off();
	udelay(1);
}

void spi_flash_display(unsigned int addr, int cnt)
{
	int i, t;
	spi1_cs_on();
	spi1_write_byte(SF_CMD_READ);
	spi1_write_byte((addr>>16) & 0xFF);
	spi1_write_byte((addr>>8) & 0xFF);
	spi1_write_byte((addr) & 0xFF);

	for (i=0; i<cnt; i++) {
		t = spi1_read_byte();
		if ((i%16)==0) {
			printf("0x%08X:", i);
		}
		printf(" %02X", t);
		if ((i%16)==15) {
			printf("\n");
		}
		if (ctrlc()) {
			i++;
			break;
		}
	}
	if ((i%16)!=0) {
		printf("\n");
	}

	spi1_cs_off();
	udelay(1);
}

int do_sf_show_information(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	unsigned char id[2];
	spi_flash_info(id);
	printf("Man_ID: 0x%02X\n", id[0]);
	printf("Dev_ID: 0x%02X\n", id[1]);
	return 0;
}

int do_sf_sector_erase(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	unsigned int addr, cnt;
	if (argc<3) {
		cmd_usage(cmdtp);
		return 1;
	}
	addr = simple_strtoul(argv[1], NULL, 16);
	cnt = simple_strtoul(argv[2], NULL, 16);
	addr &= BLOCK_MASK;
	spi_flash_sector_erase(addr, cnt);
	printf("\nerased 0x%08X blocks from 0x%08X\n", cnt, addr);
	return 0;
}

int do_sf_bulk_erase(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	spi_flash_bulk_erase();
	printf("\nbulk erase done\n");
	return 0;
}

int do_spi_write(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	unsigned int src, des, cnt;
	unsigned char *srcp;
	if (argc<4) {
		cmd_usage(cmdtp);
		return 1;
	}
	src = simple_strtoul(argv[1], NULL, 16);
	des = simple_strtoul(argv[2], NULL, 16);
	cnt = simple_strtoul(argv[3], NULL, 16);
	srcp = (unsigned char *)src;
	des &= PAGE_MASK;
	spi_flash_write(srcp, des, cnt);
	printf("\nwrite 0x%08X bytes from 0x%08X to 0x%08X\n", cnt, src, des);
	return 0;
}

int do_spi_read(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	unsigned int src, des, cnt;
	unsigned char *desp;
	if (argc<4) {
		cmd_usage(cmdtp);
		return 1;
	}
	src = simple_strtoul(argv[1], NULL, 16);
	des = simple_strtoul(argv[2], NULL, 16);
	cnt = simple_strtoul(argv[3], NULL, 16);
	desp = (unsigned char *)des;
	spi_flash_read(src, desp, cnt);
	printf("\nread 0x%08X bytes from 0x%08X to 0x%08X\n", cnt, src, des);
	return 0;
}

int do_spi_display(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	unsigned int addr, cnt;
	unsigned char *desp;
	if (argc<3) {
		cmd_usage(cmdtp);
		return 1;
	}
	addr = simple_strtoul(argv[1], NULL, 16);
	cnt = simple_strtoul(argv[2], NULL, 16);
	spi_flash_display(addr, cnt);
	return 0;
}

U_BOOT_CMD(
	sfsi, 1, 0, do_sf_show_information,
	"spi flash show information",
	"show DEVICE_ID and JEDEC_ID\n"
);

U_BOOT_CMD(
	sfse, 3, 0, do_sf_sector_erase,
	"spi flash sector erase",
	"<addr> <cnt> - erase <cnt> from <addr>\n"
	"<addr> - identifies the start address to erase\n"
	"<cnt> - number of bytes to erase\n"
);

U_BOOT_CMD(
	sfbe, 1, 0, do_sf_bulk_erase,
	"spi flash bulk erase",
	"erase all blocks\n"
);

U_BOOT_CMD(
	sfwr, 4, 0, do_spi_write,
	"spi flash write",
	"<src> <des> <cnt> - Copy <cnt> bytes from <src> to <des>\n"
	"<src> - identifies the sourse address in memeory\n"
	"<des> - identifies the target address in spiflash\n"
	"<cnt> - number of bytes to copy\n"
);

U_BOOT_CMD(
	sfrd, 4, 0, do_spi_read,
	"spi flash read",
	"<src> <des> <cnt> - Copy <cnt> bytes from <src> to <des>\n"
	"<src> - identifies the sourse address in spiflash\n"
	"<des> - identifies the target address in memory\n"
	"<cnt> - number of bytes to copy\n"
);

U_BOOT_CMD(
	sfds, 3, 0, do_spi_display,
	"spi flash display",
	"<addr> <cnt> - Display <cnt> bytes from <addr>\n"
	"<addr> - identifies the address in spiflash\n"
	"<cnt> - number of bytes to display\n"
);
