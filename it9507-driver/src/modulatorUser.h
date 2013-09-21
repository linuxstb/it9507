#ifndef __EAGLEUSER_H__
#define __EAGLEUSER_H__


//#include <stdio.h>
#include "IT9507.h"
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

u32 EagleUser_setSystemConfig (
    IN  Modulator*    modulator
);

u32 EagleUser_getTsInputType (
	IN  Modulator*    modulator,
	OUT  TsInterface*  tsInStreamType
);

u32 EagleUser_getDeviceType (
	IN  Modulator*    modulator,
	OUT  u8*		  deviceType	   
);

/**
 * Memory copy Function
 */
u32 EagleUser_memoryCopy (
    IN  Modulator*    modulator,
    IN  void*           dest,
    IN  void*           src,
    IN  u32           count
);


/**
 * Delay Function
 */
u32 EagleUser_delay (
    IN  Modulator*    modulator,
    IN  u32           dwMs
);


/**
 * Enter critical section
 */
u32 EagleUser_enterCriticalSection (
    IN  Modulator*    modulator
);


/**
 * Leave critical section
 */
u32 EagleUser_leaveCriticalSection (
    IN  Modulator*    modulator
);


/**
 * Config MPEG2 interface
 */
u32 EagleUser_mpegConfig (
    IN  Modulator*    modulator
);


/**
 * Write data via "Control Bus"
 * I2C mode : uc2WireAddr mean modulator chip address, the default value is 0x38
 * USB mode : uc2WireAddr is useless, don't have to send this data
 */
u32 EagleUser_busTx (
    IN  Modulator*    modulator,
    IN  u32           bufferLength,
    IN  u8*           buffer
);


/**
 * Read data via "Control Bus"
 * I2C mode : uc2WireAddr mean modulator chip address, the default value is 0x38
 * USB mode : uc2WireAddr is useless, don't have to send this data
 */
u32 EagleUser_busRx (
    IN  Modulator*    modulator,
    IN  u32           bufferLength,
    OUT u8*           buffer
);


u32 EagleUser_setBus (
	IN  Modulator*	modulator
);

u32 EagleUser_Initialization  (
    IN  Modulator*    modulator
); 

u32 EagleUser_Finalize  (
    IN  Modulator*    modulator
);

u32 EagleUser_acquireChannel (
	IN  Modulator*    modulator,
	IN  u16          bandwidth,
	IN  u32         frequency
);

u32 EagleUser_setTxModeEnable (
	IN  Modulator*            modulator,
	IN  u8                    enable	
);

u32 EagleUser_getChannelIndex (
	IN  Modulator*            modulator,
	IN  u8*                    index	
);


#endif
