/*
 * nts3250 configuation data
 * modified from NXP phy3250
 */

#ifndef __NTS3250_H__
#define __NTS3250_H__

//#define  DEBUG
/*
 *
 * Chip speific options
 *
 */

/*
 * Ethernet buffer support in uncached IRAM and buffer size
 */
#define USE_IRAM_FOR_ETH_BUFFERS
#define IRAM_ETH_BUFF_BASE 0x08010000 /* Uncached IRAM */
#define IRAM_ETH_BUFF_SIZE 0x00010000

/*
 * There are 2 boot options for u-boot on the Phytec 3250 board. Option 1
 * or option 2. In either cases, u-boot does not need to be relocated.
 *
 * Option 1 - define CFG_BOOT_USES1L
 * With this option, the S1L loader present in the board initializes the
 * system (including SDRAM, MMUs, some MUX states, etc.). U-boot is loaded
 * into an already initialized system in SDRAM at address 0x83FC0000 (the
 * end of SDRAM in a 64M system). Because most of the system is already
 * initialized, system init is not performed again.
 *
 * Option 2 - undefine CFG_BOOT_USES1L
 * With this option, u-boot is the primary boot loader that is loaded and
 * started from the Phytec kickstart loader (see documentation with the
 * Phytec board for the kickstart loader). In this configuration, u-boot
 * loads and runs from RAM at address 0x00000000 and requires complete
 * system initialization. The kickstart loader will copy the u-boot image
 * from FLASH starting at block 1 into IRAM and start it at address 0x0.
 */
#define CFG_BOOT_USES1L

#ifdef CFG_BOOT_USES1L
/*
 * Skip low level init of MMU, SDRAM, muxing, etc. if u-boot is loaded
 * and executed from S1L
 */
#define CONFIG_SKIP_LOWLEVEL_INIT
#endif

/*
 * Linux machine type
 */
#define MACH_TYPE_UBOOTSYS MACH_TYPE_LPC3XXX

/*
 * System UART selection, valid selections include UART3, UART4,
 * UART5, and UART6
 */
#define CFG_UART_SEL UART5

/*
 * SDRAM physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS    1
#define PHYS_SDRAM_1        0x80000000 /* SDRAM Bank #1 */

/*
 * NOR FLASH not supported
 */
//#define CONFIG_SYS_NO_FLASH
//#undef CONFIG_ENV_IS_IN_FLASH

/*
 * NOR Flash configuration
 */
 

//#define CONFIG_SYS_FLASH_CFI	1   /* The flash is CFI compatible  */

#define CONFIG_SYS_FLASH_ADDR0		0x5555	/* 1st address for flash config cycles	*/
#define CONFIG_SYS_FLASH_ADDR1		0x2aaa	/* 2nd address for flash config cycles	*/
/*


 * The following defines are added for buggy IOP480 byte interface.
 * All other boards should use the standard values (CPCI405 etc.)
 */
#define CONFIG_SYS_FLASH_READ0		0x0000	/* 0 is standard			*/
#define CONFIG_SYS_FLASH_READ1		0x0001	/* 1 is standard			*/
#define CONFIG_SYS_FLASH_READ2		0x0002	/* 2 is standard			*/

#define CONFIG_SYS_FLASH_EMPTY_INFO		/* print 'E' for empty sector on flinfo */




/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
/*-----------------------------------------------------------------------*/

#define CONFIG_SYS_FLASH_BASE		0xe0000000 
#define CONFIG_SYS_FLASH_BANKS_LIST	{ CONFIG_SYS_FLASH_BASE }

#define CONFIG_SYS_FLASH_INCREMENT	0	/* there is only one bank	*/

#define CONFIG_SYS_MAX_FLASH_BANKS	1	/* max number of memory banks		*/
#define CONFIG_SYS_FLASH_SIZE		0x00200000
#define CONFIG_SYS_MAX_FLASH_SECT	1024	/* max number of sectors on one chip	*/
#define CONFIG_SYS_FLASH_CFI_WIDTH	FLASH_CFI_16BIT

#define CONFIG_SYS_FLASH_CFI		/* The flash is CFI compatible  */
#define CONFIG_FLASH_CFI_DRIVER	/* Use common CFI driver	*/
#define CONFIG_FLASH_CFI_LEGACY  //CONFIG_FLASH_CFI_LEGACY
#define	CONFIG_SYS_FLASH_CFI_AMD_RESET

#define CONFIG_SYS_FLASH_ERASE_TOUT	30000	/* Timeout for Chip Erase (in ms) */
#define CONFIG_SYS_FLASH_ERASEBLOCK_TOUT	5000	/* Timeout for Block Erase (in ms) */
#define CONFIG_SYS_FLASH_WRITE_TOUT	1	/* Timeout for Flash Write (in ms) */


//#define CONFIG_SYS_FLASH_BANKS_LIST	{ CONFIG_SYS_FLASH_BASE }

//#define CONFIG_SYS_FLASH_USE_BUFFER_WRITE 1	/* use buffered writes (20x faster)	*/
//#define CONFIG_SYS_FLASH_PROTECTION	1	/* hardware flash protection		*/

//#define CONFIG_SYS_FLASH_ERASE_TOUT	120000	/* Timeout for Flash Erase (in ms)	*/
//#define CONFIG_SYS_FLASH_WRITE_TOUT	500	/* Timeout for Flash Write (in ms)	*/

//#define CONFIG_SYS_FLASH_EMPTY_INFO		/* print 'E' for empty sector on flinfo */
//#define CONFIG_SYS_FLASH_QUIET_TEST	1	/* don't warn upon unknown flash	*/

//#define CONFIG_ENV_IS_IN_FLASH	1	/* use FLASH for environment vars	*/
/*
 * 1KHz clock tick
 */
#define CONFIG_SYS_HZ       1000

/*
 * ARM926ejs options
 */
#define CONFIG_ARM926EJS       1 /* This is an arm926ejs CPU core  */

/*
 *
 * u-boot specific options
 *
 */

/*
 * Address and size of Environment Data
 */
#define CONFIG_ENV_IS_IN_NAND   1
#define CONFIG_ENV_SIZE     0x4000 /* 1 block */
#define CONFIG_ENV_OFFSET   0x168000 /* Block 90 */
#define CONFIG_ENV_ADDR     0x80000100 /* Passed to kernel here */

/*
 * Area and size for malloc
 */
#define CONFIG_SYS_MALLOC_LEN (CONFIG_ENV_SIZE + 128 * 1024)
#define CONFIG_SYS_GBL_DATA_SIZE 128

/*
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE (32*1024) /* 32K stack */

/*
 * ATAG support
 */
#define CONFIG_CMDLINE_TAG      1
#define CONFIG_SETUP_MEMORY_TAGS    1
#define CONFIG_INITRD_TAG       1

/*
 * Default baud rate and baud rate table, console config
 */
#define CONFIG_CONS_INDEX      1
#define CONFIG_BAUDRATE     115200
#define CONFIG_SYS_BAUDRATE_TABLE   { 9600, 19200, 38400, 57600, 115200 }

/*
 * Default load address for programs
 */
#define CONFIG_SYS_LOAD_ADDR    0x80100000 /* default load address */

/*
 * Default boot delay is 3 seconds
 */
#define CONFIG_BOOTDELAY 3
#define CONFIG_ZERO_BOOTDELAY_CHECK /* check for keypress on bootdelay==0 */

/*
 * Interrupts are not supported in this boot loader
 */
#undef CONFIG_USE_IRQ

/*
 * Use verbose help
 */
#define CONFIG_SYS_LONGHELP

/*
 * Command line configuration.
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_DHCP
#define CONFIG_CMD_ENV
#define CONFIG_CMD_ECHO /* ECHO command */
#define CONFIG_CMD_CACHE /* Cache support */
#define CONFIG_CMD_RUN
#define CONFIG_CMD_LOADB
#define CONFIG_CMD_LOADS
#define CONFIG_CMD_SAVES
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_PING
#define CONFIG_CMD_NET
#define CONFIG_CMD_BDI
#define CONFIG_CMD_SAVEENV
#define CONFIG_CMD_ELF
//#undef CONFIG_CMD_MISC
#define CONFIG_CMD_FLASH
/*
 * Prompt, command buffer
 */
#define CONFIG_SYS_CBSIZE       256     /* Console I/O Buffer Size  */
#define CONFIG_SYS_PROMPT   "uboot> "   /* Monitor Command Prompt   */
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16) /* Print Buffer Size */
#define CONFIG_SYS_MAXARGS      16      /* max number of command args   */
#define CONFIG_SYS_BARGSIZE     CONFIG_SYS_CBSIZE   /* Boot Argument Buffer Size    */

/*
 * Default range for the memory tests
 */
#define CONFIG_SYS_MEMTEST_START 0x80010000
#define CONFIG_SYS_MEMTEST_END 0x81000000

/*
 * Support for NAND FLASH, environment store in NAND at block 100
 */
#define CONFIG_CMD_NAND
#define CONFIG_SYS_MAX_NAND_DEVICE 1
#define CONFIG_SYS_NAND_BASE 0x20020000 /* SLC NAND controller */
#define CFG_ENV_IS_IN_NAND

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

/*
 * Support for various capabilities
 */
#define CONFIG_AUTO_COMPLETE
#define CONFIG_CMDLINE_EDITING
#define CONFIG_SYS_LOADS_BAUD_CHANGE

/*
 * Network setup
 */
#define CONFIG_NETMASK         255.255.255.0
#define CONFIG_IPADDR          192.168.21.66
#define CONFIG_SERVERIP        192.168.21.159
#define CONFIG_BOOTFILE        "uImage"  /* File to load */
#define CONFIG_BOOTARGS        "console=ttyS0,115200n8 root=/dev/nfs rw nfsroot=192.168.21.159:/home/athurg/nts3250/rootfs ip=192.168.21.66"

/*
 * BOOTP options
 */
#define CONFIG_BOOTP_SUBNETMASK
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_HOSTNAME
#define CONFIG_BOOTP_BOOTFILESIZE

#endif  /* __NTS3250_H__*/

