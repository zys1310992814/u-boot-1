#include <common.h>
#include <command.h>
#include <spi_lpc3250.h>

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

/*
 * SPI中内存的分布
 *
 * 0x000000	->	0x13579BDF
 * 0x000004	->	size of uboot
 * 0x000008..	->	uboot
 *
 * 0x040000	->	len of kernel
 * 0x040004	->	crc of kernel
 * 0x040008	->	kernel
 *
 * 0x800000	->	len of appfs
 * 0x800004	->	crc of appfs
 * 0x800008	->	appfs
 *
 */
int do_spi_applet_install(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	unsigned int *src;
	unsigned int len;
	unsigned int *p;

	if (argc<4) {
		cmd_usage(cmdtp);
		return 1;
	}

	src  = (unsigned int *)simple_strtoul(argv[2], NULL, 16);
	len = simple_strtoul(argv[3], NULL, 16);

	switch (argv[1][0]){
		case 'u':
		case 'U':
			src -= 1;// 1=>sizeof 0x13579BDF
			src[0] = 0x13579BDF;
			//Uboot's size is used by LPC32XX's Bootstrap
			//the size shouldn't large than 56K
			//refer LPC32XX's user manual
			src[1] = 56*1024;
			spi_flash_write(src, 0, len+4);
			break;
		case 'k':
		case 'K':
			src    -= 2;
			src[0]  = len;
			src[1]  = crc32(0, src+2, len);
			spi_flash_write(src, CONFIG_KERNEL_OFFSET, len+8);
			break;
		case 'a':
		case 'A':
			src    -= 2;
			src[0]  = len;
			src[1]  = crc32(0, src+2, len);
			spi_flash_write(src, CONFIG_APPFS_OFFSET, len+8);
			break;
		default:
			cmd_usage(cmdtp);
			return 1;
	}
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

U_BOOT_CMD(
	sfai, 4, 0, do_spi_applet_install,
	"spi flash applet install",
	"<name> <src> <size> - auto erase and install <name> from <addr> into spi flash\n"
	"<name> - name of applet ,such as uboot/kernel/appfs\n"
	"<src>  - identifies the source address\n"
	"<size> - size of applet\n"
);

