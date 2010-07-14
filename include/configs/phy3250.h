/*
 * nts3250 configuation data
 * modified from NXP phy3250
 */

#ifndef __NTS3250_H__
#define __NTS3250_H__
//#define  DEBUG
//#define MII_DEBUG

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
#define CONFIG_NR_DRAM_BANKS	1
#define PHYS_SDRAM_1		0x80000000 /* SDRAM Bank #1 */
#define PHYS_SDRAM_SIZE		0x02000000

/*
 * NOR FLASH not supported
 */
//#define CONFIG_SYS_NO_FLASH
//#undef CONFIG_ENV_IS_IN_FLASH

/*
 * NOR Flash configuration
 */

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
/*-----------------------------------------------------------------------*/

#define CONFIG_SYS_FLASH_BASE		0xe0000000 
#define CONFIG_SYS_FLASH_BANKS_LIST	{ CONFIG_SYS_FLASH_BASE }

#define CONFIG_SYS_FLASH_INCREMENT	0	/* there is only one bank	*/

#define CONFIG_SYS_MAX_FLASH_BANKS	1	/* max number of memory banks		*/
#define CONFIG_SYS_FLASH_SIZE		0x02000000
#define CONFIG_SYS_MAX_FLASH_SECT	512	/* max number of sectors on one chip	*/
#define CONFIG_SYS_FLASH_CFI_WIDTH	FLASH_CFI_16BIT

#define CONFIG_SYS_FLASH_CFI		/* The flash is CFI compatible  */
#define CONFIG_FLASH_CFI_DRIVER	/* Use common CFI driver	*/
//#define	CONFIG_SYS_FLASH_CFI_AMD_RESET
#define CONFIG_SYS_FLASH_USE_BUFFER_WRITE 
//#define CONFIG_SYS_FLASH_ERASE_TOUT	 120000	/* Timeout for Chip Erase (in ms) */
//#define CONFIG_SYS_FLASH_ERASEBLOCK_TOUT	5000	/* Timeout for Block Erase (in ms) */
//#define CONFIG_SYS_FLASH_WRITE_TOUT	5000	/* Timeout for Flash Write (in ms) */

/*
 *env in norflash
*/
#define CONFIG_SYS_MONITOR_BASE        TEXT_BASE       /* start of monitor */
#define CONFIG_SYS_MONITOR_LEN        (128 * 1024)    /* Reserve 128 kB for Mon */ 
#define CONFIG_ENV_IS_IN_FLASH	1	/* use FLASH for environment vars	*/
#define CONFIG_ENV_OFFSET		0x40000
#define CONFIG_ENV_SECT_SIZE		0x20000  // 128K
#define CONFIG_ENV_SIZE			0x20000
#define CONFIG_ENV_ADDR			0xe01e0000
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
//#define CONFIG_BOOTDELAY 3
#define CONFIG_ZERO_BOOTDELAY_CHECK /* check for keypress on bootdelay==0 */

/*
 * Interrupts are not supported in this boot loader
 */
//#undef CONFIG_USE_IRQ

/*
 * Use verbose help
 */
#define CONFIG_SYS_LONGHELP

//net

//#define CONFIG_TSEC_ENET        /* TSEC ethernet support */
//#ifndef CONFIG_NET_MULTI
//#define CONFIG_NET_MULTI        1
//#endif
//#define CONFIG_RESET_PHY_R
//#define CONFIG_GMII             1       /* MII PHY management */
#undef   USE_PHY_RMII
#define  CONFIG_MII
#define  CONFIG_CMD_MII

/*`
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
//#define CFG_ENV_IS_IN_NAND

/* allow to overwrite serial and ethaddr */
//#define CONFIG_ENV_OVERWRITE

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
#define CONFIG_IPADDR          192.168.23.193
#define CONFIG_SERVERIP		   192.168.23.66
#define CONFIG_BOOTFILE        "uImage"  /* File to load */
#define CONFIG_BOOTARGS        "console=ttyS0,115200n8"

#define CONFIG_LOADADDR         0x80000000 /* default location for tftp and bootm */
#define CONFIG_BOOTDELAY        2       /* -1 disables auto-boot */
//#define CONFIG_MENUPROMPT       "Hit SPACE key to stop autoboot %2d "
#define CONFIG_STOP_BOOT_KEY    ' '
#define CONFIG_BOOT_FAIL_RESET  1
//#define CONFIG_BOOT_FAIL_RETRY  1
#define CONFIG_BAUDRATE         115200
//#define CONFIG_ROOTPATH		/home/user/ltib/rootfs
//#define CONFIG_BOOTARGS		
#define CONFIG_BOOTCOMMAND	"bootm 0xE0060000"

//#define MTDBOOTCOMMAND "mtdboot="				\
 "setenv bootargs root=/dev/mtdblock3 rw rootfstype=jffs2 "	\
	"ip=$(ipaddr) ea_ethaddr=$(ethaddr) "			\
	"console=ttyS0,115200n8; "				\
	"tftp;"					\
	"bootm $(loadaddr)\0"
	
//#define CONFIG_EXTRA_ENV_SETTINGS \
	"loadkernel_tftp=tftpboot $(loadaddr) uImage\0"
/*
 * BOOTP options
 */
#define CONFIG_BOOTP_SUBNETMASK
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_HOSTNAME
#define CONFIG_BOOTP_BOOTFILESIZE

#endif  /* __NTS3250_H__*/

