#ifndef __EAGLEUSER_H__
#define __EAGLEUSER_H__


//#include <stdio.h>
#include "IT9507.h"
#include "modulatorType.h"
#include "usb2impl.h"

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


typedef struct {
    HwFunction          GPIO1;
    HwFunction          GPIO2;
    HwFunction          GPIO3;
    HwFunction          GPIO4;
	HwFunction          GPIO5;
    HwFunction          GPIO6;
    HwFunction          GPIO7;
    HwFunction          GPIO8;
} DeviceDescription;

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

Dword EagleUser_getSystemConfig (
    IN  Modulator*    modulator,
    IN  Byte          pcbIndex,
	IN  SystemConfig* Config  
);


Dword EagleUser_setSystemConfig (
    IN  Modulator*    modulator,
	IN  SystemConfig  systemConfig
);

Dword EagleUser_getTsInputType (
	IN  Modulator*    modulator,
	OUT  TsInterface*  tsInStreamType
);

Dword EagleUser_getDeviceType (
	IN  Modulator*    modulator,
	OUT  Byte*		  deviceType	   
);

/**
 * Memory copy Function
 */
Dword EagleUser_memoryCopy (
    IN  Modulator*    modulator,
    IN  void*           dest,
    IN  void*           src,
    IN  Dword           count
);


/**
 * Delay Function
 */
Dword EagleUser_delay (
    IN  Modulator*    modulator,
    IN  Dword           dwMs
);


/**
 * Enter critical section
 */
Dword EagleUser_enterCriticalSection (
    IN  Modulator*    modulator
);


/**
 * Leave critical section
 */
Dword EagleUser_leaveCriticalSection (
    IN  Modulator*    modulator
);


/**
 * Config MPEG2 interface
 */
Dword EagleUser_mpegConfig (
    IN  Modulator*    modulator
);


/**
 * Write data via "Control Bus"
 * I2C mode : uc2WireAddr mean modulator chip address, the default value is 0x38
 * USB mode : uc2WireAddr is useless, don't have to send this data
 */
Dword EagleUser_busTx (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    IN  Byte*           buffer
);


/**
 * Read data via "Control Bus"
 * I2C mode : uc2WireAddr mean modulator chip address, the default value is 0x38
 * USB mode : uc2WireAddr is useless, don't have to send this data
 */
Dword EagleUser_busRx (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
);


Dword EagleUser_setBus (
	IN  Modulator*	modulator
);

Dword EagleUser_Initialization  (
    IN  Modulator*    modulator
); 

Dword EagleUser_Finalize  (
    IN  Modulator*    modulator
);

Dword EagleUser_acquireChannel (
	IN  Modulator*    modulator,
	IN  Word          bandwidth,
	IN  Dword         frequency
);

Dword EagleUser_setTxModeEnable (
	IN  Modulator*            modulator,
	IN  Byte                    enable	
);

Dword EagleUser_getChannelIndex (
	IN  Modulator*            modulator,
	IN  Byte*                    index	
);


#endif
