#ifndef NTS3250_PRV_H
#define NTS3250_PRV_H

/***********************************************************************
 * NAND timing information
 **********************************************************************/
/* NAND256R3A2CZA6 device */
#define PHY_NAND_TCEA_DELAY		22222222
#define PHY_NAND_BUSY_DELAY		10000000
#define PHY_NAND_NAND_TA		33333333
#define PHY_NAND_RD_HIGH		66666666
#define PHY_NAND_RD_LOW			33333333
#define PHY_NAND_WR_HIGH		50000000
#define PHY_NAND_WR_LOW			25000000

/***********************************************************************
 * PHY register and bitfield data
 **********************************************************************/
/* PHY address (configured via PHY ADRx pins) */
#define PHYDEF_PHYADDR		0x04

/* PHY Register indices */
#define PHY_REG_BMCR			0x00	// Basic Mode Control Register
#define PHY_REG_BMSR			0x01	// Basic Mode Status Register
#define PHY_REG_IDR1			0x02	// PHY Identifier 1
#define PHY_REG_IDR2			0x03	// PHY Identifier 2
#define PHY_REG_ANAR			0x04	// Auto-Negotiation Advertisement
#define PHY_REG_ANLPAR			0x05	// Auto-Neg. Link Partner Abitily
#define PHY_REG_ANER			0x06	// Auto-Neg. Expansion Register
#define PHY_REG_ANNPTR			0x07	// Auto-Neg. Next Page TX

/* BMCR register specific control bits */
#define PHY_BMCR_RESET_BIT		0x8000	// Reset bit
#define PHY_BMCR_SPEED_BIT		0x2000	// 100M speed enable bit
#define PHY_BMCR_AUTON_BIT		0x1000	// Auto-negotiation rate bit

/* BMSR register specific control bits */
#define PHY_BMSR_LINKUP_STATUS		0x0004	// Link up status bit
#define PHY_BMSR_AUTON_COMPLETE		0x0020	// Auto-negotiation complete bit
#define PHY_BMSR_10M_HALF		0x0800	// 10MBase with half duplex support
#define PHY_BMSR_10M_FULL		0x1000	// 10MBase with full duplex support
#define PHY_BMSR_TX_HALF		0x2000	// TX with half duplex support
#define PHY_BMSR_TX_FULL		0x4000	// TX with full duplex support
#define PHY_BMSR_T4_ABLE		0x8000	// T4 able

/* Maximum ethernet frame size, maximum RX and TX packets */
#define ENET_MAXF_SIZE			1536
#define ENET_MAX_TX_PACKETS		16
#define ENET_MAX_RX_PACKETS		16

#endif /* NTS3250_PRV_H */
