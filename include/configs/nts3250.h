/*
 * NTS3250 board configuation data
 */

#ifndef __NTS3250_H__
#define __NTS3250_H__

/*
 * Chip speific options
 */
#define NTS_WDG		1		/* Enable NTS CPLD WDG */
#define NTS_WDG_WORD	0x5A		/* WDG reset word */
#define NTS_WDG_ADDR	0xE2000003	/* WDG reset address */
#define NTS_WDG_RESET	*((volatile unsigned char *)NTS_WDG_ADDR)=NTS_WDG_WORD

/*
 * Ethernet buffer support in uncached IRAM and buffer size
 */
#define USE_IRAM_FOR_ETH_BUFFERS
#define IRAM_ETH_BUFF_BASE	0x08010000	/* Uncached IRAM */
#define IRAM_ETH_BUFF_SIZE	0x00010000

/*
 * Skip low level init of MMU, SDRAM, muxing, etc. if u-boot is loaded
 * and executed from S1L
 */
//#define CFG_BOOT_USES1L
#ifdef CFG_BOOT_USES1L
#define CONFIG_SKIP_LOWLEVEL_INIT
#endif

/*
 * Linux machine type
 */
#define MACH_TYPE_UBOOTSYS	MACH_TYPE_LPC3XXX

/*
 * System UART selection, valid selections include UART3, UART4,
 * UART5, and UART6
 */
#define CFG_UART_SEL UART5

/*
 * SDRAM physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	1
#define PHYS_SDRAM_1		0x80000000	/* SDRAM Bank #1 */
#define PHYS_SDRAM_SIZE		0x02000000	/* SDRAM Bank #1 32M */

/*
 * 1KHz clock tick
 */
#define CONFIG_SYS_HZ		1000

/*
 * ARM926ejs options
 */
#define CONFIG_ARM926EJS	1

/*
 * FLASH and environment organization
 */

#define CONFIG_SYS_FLASH_BASE		0xE0000000
#define CONFIG_SYS_FLASH_BANKS_LIST	{ CONFIG_SYS_FLASH_BASE }

#define CONFIG_SYS_FLASH_INCREMENT	0	/* there is only one bank */

#define CONFIG_SYS_MAX_FLASH_BANKS	1	/* max number of memory banks */
#define CONFIG_SYS_FLASH_SIZE		0x02000000
#define CONFIG_SYS_MAX_FLASH_SECT	256	/* max number of sectors on one chip */
#define CONFIG_SYS_FLASH_SECT_SIZE	0x20000
#define CONFIG_SYS_FLASH_CFI_WIDTH	FLASH_CFI_16BIT

#define CONFIG_SYS_FLASH_CFI		/* The flash is CFI compatible */
#define CONFIG_FLASH_CFI_DRIVER		/* Use common CFI driver */
#define CONFIG_SYS_FLASH_USE_BUFFER_WRITE
#define CONFIG_FLASH_SHOW_PROGRESS	45

/*
 * Applets Offsets
 */
#define CONFIG_KERNEL_OFFSET		0x40000
#define CONFIG_APPFS_OFFSET		0x800000

/*
 * ENV in norflash
*/
#define CONFIG_SYS_MONITOR_BASE		TEXT_BASE	/* start of monitor */
#define CONFIG_SYS_MONITOR_LEN		(128*1024)	/* Reserve 128 kB for Mon */
#define CONFIG_ENV_IS_NOWHERE		1		/* use default environment vars */
#define CONFIG_ENV_SIZE			0x2000		/* 8K */

#define CONFIG_BOOT_PARA_ADDR		0x80000100

/*
 * Area and size for malloc
 */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 128 * 1024)
#define CONFIG_SYS_GBL_DATA_SIZE	128

/*
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE		(32*1024) /* 32K stack */

/*
 * ATAG support
 */
#define CONFIG_CMDLINE_TAG		1
#define CONFIG_SETUP_MEMORY_TAGS	1
#define CONFIG_INITRD_TAG		1

/*
 * Default baud rate and baud rate table, console config
 */
#define CONFIG_CONS_INDEX		1
#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

/*
 * Default load address for programs
 */
#define CONFIG_SYS_LOAD_ADDR		0x80001000


/*
 * Interrupts are not supported in this boot loader
 */
#undef CONFIG_USE_IRQ

/*
 * Use verbose help
 */
#define CONFIG_SYS_LONGHELP
/*
 * MII
 */
#undef  USE_PHY_RMII
#define CONFIG_MII
#define CONFIG_CMD_MII

/*
 * Command line configuration.
 */
#include <config_cmd_default.h>

#undef CONFIG_CMD_DHCP
#define CONFIG_CMD_ENV
#define CONFIG_CMD_ECHO /* ECHO command */
#define CONFIG_CMD_CACHE /* Cache support */
#define CONFIG_CMD_RUN
#undef CONFIG_CMD_LOADB
#undef CONFIG_CMD_LOADS
#undef CONFIG_CMD_SAVES
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_PING
#define CONFIG_CMD_NET
#undef CONFIG_CMD_BDI
#undef CONFIG_CMD_SAVEENV
#undef CONFIG_CMD_ELF
#undef CONFIG_CMD_MISC
#define CONFIG_CMD_FLASH
#undef CONFIG_CMD_NAND
#define CONFIG_CMD_SPIFLASH
/*
 * Prompt, command buffer
 */
#define CONFIG_BOOTDELAY	2
#define CONFIG_ZERO_BOOTDELAY_CHECK  /* check for keypress on bootdelay==0 */

#define CONFIG_SYS_CBSIZE	256  /* Console I/O Buffer Size */
#define CONFIG_SYS_PROMPT	"u-boot> "  /* Monitor Command Prompt */
#define CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)  /* Print Buffer Size */
#define CONFIG_SYS_MAXARGS	16  /* max number of command args */
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE  /* Boot Argument Buffer Size */

/*
 * Default range for the memory tests
 */
#define CONFIG_SYS_MEMTEST_START	0x80010000
#define CONFIG_SYS_MEMTEST_END		0x81000000

/*
 * Support for various capabilities
 */
#undef CONFIG_AUTO_COMPLETE
#define CONFIG_CMDLINE_EDITING

/*
 * Network setup
 */
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_IPADDR		192.168.30.193
#define CONFIG_SERVERIP		192.168.30.66
#define CONFIG_BOOTARGS		"console=ttyS0,115200n8"
#define CONFIG_BOOTCOMMAND	"bootm e0040000 - -"

/*
 * ENV options
 */
#define CONFIG_EXTRA_ENV_SETTINGS \
	"inst_uboot=sfai u 80001000 $(filesize)\0"		\
	"inst_kernel=sfai k 80001000 $(filesize)\0"		\
	"inst_appfs=sfai a 80001000 $(filesize)\0"

#endif  /* __NTS3250_H__*/
