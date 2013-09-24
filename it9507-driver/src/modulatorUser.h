#ifndef __EAGLEUSER_H__
#define __EAGLEUSER_H__


//#include <stdio.h>
#include "modulatorType.h"

#define EagleUser_MAX_PKT_SIZE               255


#define EagleUser_RETRY_MAX_LIMIT            10


/** Define I2C master speed, the default value 0x07 means 366KHz (1000000000 / (24.4 * 16 * EagleUser_I2C_SPEED)). */
#define EagleUser_IIC_SPEED              0x07

/** Define I2C address of secondary chip when Diversity mode or PIP mode is active. */
#define EagleUser_IIC_ADDRESS            0x38
#define EagleUser_SlaveIIC_ADDRESS       0x3A
#define EagleUser_DEVICETYPE			 0

/** Define USB frame size */
#define EagleUser_USB20_MAX_PACKET_SIZE      512
#define EagleUser_USB20_FRAME_SIZE           (188 * 348)
#define EagleUser_USB20_FRAME_SIZE_DW        (EagleUser_USB20_FRAME_SIZE / 4)
#define EagleUser_USB11_MAX_PACKET_SIZE      64
#define EagleUser_USB11_FRAME_SIZE           (188 * 21)
#define EagleUser_USB11_FRAME_SIZE_DW        (EagleUser_USB11_FRAME_SIZE / 4)
#define EagleUser_MAXFRAMESIZE			63


typedef enum {
	NA = 0,
	RestSlave,
	RfEnable,
	LoClk,
	LoData,
	LoLe,
	LnaPowerDown,
	IrDa,
	UvFilter,
	ChSelect3,
	ChSelect2,
	ChSelect1,
	ChSelect0,
	PowerDownSlave,
	UartTxd,
	MuxSelect,
	lnaGain,
	intrEnable
} HwFunction;


typedef enum {
	EVB01v01 = 0,
	DB0101v01,
	DB0102v01,
	DB0101v03,
	DB0102v02,
	SDAVsender,
	EVB01v02,
	DTVCAM_Yuelee,
	DTVCAM_Sunnic,
	HDAVsender,
	AVsenderHC,
	DB0101v04,
	IndexEnd
} PCBIndex;

#endif
