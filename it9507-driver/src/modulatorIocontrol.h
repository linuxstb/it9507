/**
 *
 * Copyright (c) 2013 ITE Corporation. All rights reserved. 
 *
 * Module Name:
 *   iocontrol.h
 *
 * Abstract:
 *   The structure and IO code for IO control call.
 *
 */

#ifndef __IOCONTROL_H__
#define __IOCONTROL_H__

#include "modulatorType.h"


/* Use 'k' as magic number */
#define AFA_IOC_MAGIC  'k'

typedef struct {
    u8			chip;
    Processor		processor;
    u32			registerAddress;
    u8			bufferLength;
    u8			buffer[256];
    u32			error;
    u8			reserved[16];
} TxWriteRegistersRequest, *PTxWriteRegistersRequest;

typedef struct {
    u8			chip;
    u16			registerAddress;
    u8			bufferLength;
    u8			buffer[256];
    u32			error;
    u8			reserved[16];
} WriteEepromValuesRequest, *PWriteEepromValuesRequest;

typedef struct {
    u8			chip;
    Processor		processor;
    u32			registerAddress;
    u8			position;
    u8			length;
    u8			value;
    u32			error;
    u8			reserved[16];
} TxWriteRegisterBitsRequest, *PTxWriteRegisterBitsRequest;

typedef struct {
    u8			chip;
    Processor		processor;
    u32			registerAddress;
    u8			bufferLength;
    u8			buffer[256];
    u32			error;
    u8			reserved[16];
} TxReadRegistersRequest, *PTxReadRegistersRequest;

typedef struct {
    u8			chip;
    u16			registerAddress;
    u8			bufferLength;
    u8			buffer[256];
    u32			error;
    u8			reserved[16];
} TxReadEepromValuesRequest, *PTxReadEepromValuesRequest;

typedef struct {
    u8			chip;
    Processor		processor;
    u32			registerAddress;
    u8			position;
    u8			length;
    u8*			value;
    u32			error;
    u8			reserved[16];
} ReadRegisterBitsRequest, *PReadRegisterBitsRequest;

typedef struct {
    u8			chip;
    Processor		processor;
    u32			registerAddress;
    u8			position;
    u8			length;
    u8*			value;
    u32			error;
    u8			reserved[16];
} TxReadRegisterBitsRequest, *PTxReadRegisterBitsRequest;

typedef struct {
    u32				error;
    u8				reserved[16];
} FinalizeRequest, *PFinalizeRequest;

typedef struct {
    u32				error;
    u8				reserved[16];
} TxFinalizeRequest, *PTxFinalizeRequest;

typedef struct {
    Processor			processor;
	u32*				version;
    u32				error;
    u8				reserved[16];
} TxGetFirmwareVersionRequest, *PTxGetFirmwareVersionRequest;

typedef struct {
    u8				chip;
    u8 				transmissionMode;
	u8				constellation;
	u8				interval;
	u8				highCodeRate;
    u32				error;
    u8				reserved[16];
} SetModuleRequest, *PSetModuleRequest;

typedef struct {
    u8				chip;
    u16				bandwidth;
    u32				frequency;
    u32				error;
    u8				reserved[16];
} TxAcquireChannelRequest, *PTxAcquireChannelRequest;

typedef struct {
    u8				OnOff;
    u32				error;
    u8				reserved[16];
} TxModeRequest, *PTxModeRequest;

typedef struct {
    u8				DeviceType;
    u32				error;
    u8				reserved[16];
} TxSetDeviceTypeRequest, *PTxSetDeviceTypeRequest;

typedef struct {
    u8				DeviceType;
    u32				error;
    u8				reserved[16];
} TxGetDeviceTypeRequest, *PTxGetDeviceTypeRequest;

typedef struct {
    int				GainValue;
    u32				error;
} SetGainRequest, *PSetGainRequest;

typedef struct {
    u8			chip;
	u8			control;
    u32			error;
    u8			reserved[16];
} TxControlPidFilterRequest, *PTxControlPidFilterRequest;

typedef struct {
    u8				chip;
    u8				control;
    u32				error;
    u8				reserved[16];
} ControlPowerSavingRequest, *PControlPowerSavingRequest;

typedef struct {
    u8				chip;
    u8				control;
    u32				error;
    u8				reserved[16];
} TxControlPowerSavingRequest, *PTxControlPowerSavingRequest;

typedef struct {
    u8			chip;
    u32			slaveAddress;
    u8			bufferLength;
    u8			buffer[256];
    u32			error;
    u8			reserved[16];
} WriteGenericRegistersRequest, *PWriteGenericRegistersRequest;

typedef struct {
    u8			chip;
    u32			slaveAddress;
    u8			bufferLength;
    u8			buffer[256];
    u32			error;
    u8			reserved[16];
} ReadGenericRegistersRequest, *PReadGenericRegistersRequest;

typedef struct {
    u8                DriverVerion[16];   /** XX.XX.XX.XX Ex., 1.2.3.4			*/
    u8                APIVerion[32];      /** XX.XX.XXXXXXXX.XX Ex., 1.2.3.4	*/
    u8                FWVerionLink[16];   /** XX.XX.XX.XX Ex., 1.2.3.4			*/
    u8                FWVerionOFDM[16];   /** XX.XX.XX.XX Ex., 1.2.3.4			*/
    u8                DateTime[24];       /** Ex.,"2004-12-20 18:30:00" or "DEC 20 2004 10:22:10" with compiler __DATE__ and __TIME__  definitions */
    u8                Company[8];         /** Ex.,"ITEtech"					*/
    u8                SupportHWInfo[32];  /** Ex.,"Jupiter DVBT/DVBH"			*/
    u32               error;
    u8                reserved[128];
} TxModDriverInfo, *PTxModDriverInfo;

typedef struct {
       u8                      chip;
    u32                        error;
    u8                 reserved[16];
} StartTransferRequest, *PStartTransferRequest;

typedef struct {
       u8                      chip;
    u32                        error;
    u8                 reserved[16];
} StopTransferRequest, *PStopTransferRequest;


typedef struct {
	u32*			len;
    u8*			cmd;
    u8			reserved[16];
} CmdRequest, *PCmdRequest;

typedef struct {
    u32			error;
	u32          frequency;
	u16           bandwidth;    
	int*			maxGain;
	int*			minGain;
    u8			reserved[16];	
} GetGainRangeRequest, *PGetGainRangeRequest;

typedef struct {
    u32			error;
    pTPS           pTps;
    u8			reserved[16];	
} GetTPSRequest, *PGetTPSRequest;

typedef struct {
    u32		   error;
    TPS           tps;
    u8		   reserved[16];	
} SetTPSRequest, *PSetTPSRequest;

typedef struct {
    u32		   error;
	int			  *gain;	 
    u8		   reserved[16];	
} GetOutputGainRequest, *PGetOutputGainRequest;

typedef struct {
    u32		   error;
	u8         *pbuffer;
    u8		   reserved[16];	
} SendHwPSITableRequest, *PSendHwPSITableRequest;

typedef struct {
    u32		   error;
	u8		   psiTableIndex;
	u8         *pbuffer;
    u8		   reserved[16];	
} AccessFwPSITableRequest, *PAccessFwPSITableRequest;

typedef struct {
    u32		   error;
	u8		   psiTableIndex;
	u16           timer;
    u8		   reserved[16];	
} SetFwPSITableTimerRequest, *PSetFwPSITableTimerRequest;

typedef struct {
    u8* 				pBuffer;
    u32 				pdwBufferLength;
    u32				error;
    u8				reserved[16];
} SetLowBitRateTransferRequest, *PSetLowBitRateTransferRequest;

typedef struct {
	IQtable*           IQ_table_ptr;
	u16				tableGroups;
    u32				error;
    u8				reserved[16];
} TxSetIQTableRequest, *PTxSetIQTableRequest;

typedef struct {
    int					dc_i;
	int					dc_q;
    u32				error;
    u8				reserved[16];
} TxSetDCCalibrationValueRequest, *PTxSetDCCalibrationValueRequest;

/**
 * Demodulator API commands
 */
#define IOCTRL_ITE_GROUP_STANDARD           0x000
#define IOCTRL_ITE_GROUP_DVBT				0x100
#define IOCTRL_ITE_GROUP_DVBH				0x200
#define IOCTRL_ITE_GROUP_FM                 0x300
#define IOCTRL_ITE_GROUP_TDMB				0x400
#define IOCTRL_ITE_GROUP_OTHER              0x500

/**
 * Power off the demodulators.
 * Paramters:  FinalizeRequest struct
 */
#define IOCTL_ITE_DEMOD_FINALIZE \
       _IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x0D, FinalizeRequest)


/**
 *
 * Paramters: 	ControlPowerSavingRequest struct
 */
#define IOCTL_ITE_DEMOD_CONTROLPOWERSAVING \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x1E, ControlPowerSavingRequest)

#define IOCTL_ITE_DEMOD_SETMODULE_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x21, SetModuleRequest)
	
#define IOCTL_ITE_DEMOD_ACQUIRECHANNEL_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x22, TxAcquireChannelRequest)

#define IOCTL_ITE_DEMOD_ENABLETXMODE_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x23, TxModeRequest)
	
/**
 * Read a sequence of bytes from the contiguous registers in demodulator.
 * Paramters:   ReadRegistersRequest struct
 */
#define IOCTL_ITE_DEMOD_READREGISTERS_TX \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x24, TxReadRegistersRequest)

/**
 * Write a sequence of bytes to the contiguous registers in demodulator.
 * Paramters:   TxWriteRegistersRequest struct
 */
#define IOCTL_ITE_DEMOD_WRITEREGISTERS_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x27, TxWriteRegistersRequest)
	
#define IOCTL_ITE_DEMOD_SETDEVICETYPE_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x28, TxSetDeviceTypeRequest)

#define IOCTL_ITE_DEMOD_GETDEVICETYPE_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x29, TxGetDeviceTypeRequest)

#define IOCTL_ITE_DEMOD_ADJUSTOUTPUTGAIN_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x2B, SetGainRequest)

#define IOCTL_ITE_DEMOD_GETGAINRANGE_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x2C, GetGainRangeRequest)

#define IOCTL_ITE_DEMOD_GETOUTPUTGAIN_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x2D, GetOutputGainRequest)

/**
 * Set the output stream type of chip. Because the device could output in 
 * many stream type, therefore host have to choose one type before receive 
 * data.
 * Paramters:   TxGetFirmwareVersionRequest struct
 */
#define IOCTL_ITE_DEMOD_GETFIRMWAREVERSION_TX \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x2E, TxGetFirmwareVersionRequest)

/**
 *
 * Paramters: 	TxControlPowerSavingRequest struct
 */
#define IOCTL_ITE_DEMOD_CONTROLPOWERSAVING_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x2F, TxControlPowerSavingRequest)

/**
 * Power off the demodulators.
 * Paramters: 	TxFinalizeRequest struct
 */
#define IOCTL_ITE_DEMOD_FINALIZE_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x30, TxFinalizeRequest)

/**
 * Write a sequence of bytes to the contiguous cells in the EEPROM.
 * Paramters:   WriteEepromValuesRequest struct
 */
#define IOCTL_ITE_DEMOD_WRITEEEPROMVALUES_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x31, WriteEepromValuesRequest)

/**
 * Read a sequence of bytes from the contiguous cells in the EEPROM.
 * Paramters:   ReadEepromValuesRequest struct
 */
#define IOCTL_ITE_DEMOD_READEEPROMVALUES_TX \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x32, TxReadEepromValuesRequest)

/**
 * Write one byte to the contiguous registers in demodulator.
 * Paramters:   TxWriteRegisterBitsRequest struct
 */
#define IOCTL_ITE_DEMOD_WRITEREGISTERBITS_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x34, TxWriteRegisterBitsRequest)

/**
 * Read a sequence of bytes from the contiguous registers in demodulator.
 * Paramters:   TxReadRegisterBitsRequest struct
 */
#define IOCTL_ITE_DEMOD_READREGISTERBITS_TX \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x35, TxReadRegisterBitsRequest)

/**
 * Write a sequence of bytes to the I2C generic interface in demodulator.
 * Paramters:   WriteGenericRegistersRequest struct
 */
#define IOCTL_ITE_DEMOD_WRITEGENERICREGISTERS_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x39, WriteGenericRegistersRequest)

/**
 * Read a sequence of bytes from the I2C generic interface in demodulator.
 * Paramters:   ReadGenericRegistersRequest struct
 */
#define IOCTL_ITE_DEMOD_READGENERICREGISTERS_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x3A, ReadGenericRegistersRequest)


/***************************************************************************/
/*                                DVBT                                     */
/***************************************************************************/

/**
 * Reset PID from PID filter.
 * Paramters:	ResetPidRequest struct
 */
#define IOCTL_ITE_DEMOD_RESETPID \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_DVBT + 0x02, ResetPidRequest)

/**
 * Enable PID filter.
 * Paramters:	ControlPidFilterRequest struct
 */
#define IOCTL_ITE_DEMOD_CONTROLPIDFILTER \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_DVBT + 0x03, ControlPidFilterRequest)

/**
 * Add PID to PID filter.
 * Paramters:	AddPidAtRequest struct
 */
#define IOCTL_ITE_DEMOD_ADDPIDAT \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_DVBT + 0x04, AddPidAtRequest)

/**
 * Add PID to PID filter.
 * Paramters:	AddPidAtRequest struct
 */
#define IOCTL_ITE_DEMOD_ADDPIDAT_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_DVBT + 0x08, TxAddPidAtRequest)

/**
 * Reset PID from PID filter.
 * Paramters:	ResetPidRequest struct
 */
#define IOCTL_ITE_DEMOD_RESETPID_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_DVBT + 0x10, TxResetPidRequest)

/**
 * Enable PID filter.
 * Paramters:	TxControlPidFilterRequest struct
 */
#define IOCTL_ITE_DEMOD_CONTROLPIDFILTER_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_DVBT + 0x11, TxControlPidFilterRequest)

/**
 * Enable Set IQTable From File.
 * Paramters:	TxSetIQTableRequest struct
 */
#define IOCTL_ITE_DEMOD_SETIQTABLE_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_DVBT + 0x12, TxSetIQTableRequest)

/**
 * Enable Set DC Calibration Value From File.
 * Paramters:	TxSetDCCalibrationValueRequest struct
 */
#define IOCTL_ITE_DEMOD_SETDCCALIBRATIONVALUE_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_DVBT + 0x13, TxSetDCCalibrationValueRequest)


/***************************************************************************/
/*                                DVBH                                     */
/***************************************************************************/


/***************************************************************************/
/*                                 FM                                      */
/***************************************************************************/


/***************************************************************************/
/*                                TDMB                                     */
/***************************************************************************/


/***************************************************************************/
/*                                OTHER                                    */
/***************************************************************************/

/**
 * Get driver information.
 * Paramters: 	DemodDriverInfo struct
 */
#define IOCTL_ITE_DEMOD_GETDRIVERINFO \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_OTHER + 0x00, DemodDriverInfo)

/**
 * Start capture data stream
 * Paramters: StartCaptureRequest struct
 */
#define IOCTL_ITE_DEMOD_STARTCAPTURE \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_OTHER + 0x01, StartCaptureRequest)

/**
 * Stop capture data stream
 * Paramters: StopCaptureRequest struct
 */
#define IOCTL_ITE_DEMOD_STOPCAPTURE \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_OTHER + 0x02, StopCaptureRequest)

/**
 * Get EEPROM I2C address
 * Paramters: 	GetEEPROMI2CAddrRequest struct
 */
#define IOCTL_ITE_DEMOD_GETEEPROMI2CADDR \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_OTHER + 0x03, GetEEPROMI2CAddrRequest)

/**
 * Get ring buffer event
 * Paramters: 	GetBufferEventRequest struct
 */
#define IOCTL_ITE_DEMOD_GETBUFFEREVENT \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_OTHER + 0x04, GetBufferEventRequest)

/**
 * Get data length of current ring buffer
 * Paramters: 	GetTSBufferLenRequest struct
 */
#define IOCTL_ITE_DEMOD_GETTSDATABUFFERLEN \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_OTHER + 0x05, GetTSBufferLenRequest)

/**
 * Clean all ring buffer data in driver
 * Paramters: 	CleanTSDataBufferRequest struct
 */
#define IOCTL_ITE_DEMOD_CLEANTSDATABUFFER \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_OTHER + 0x06, CleanTSDataBufferRequest)

/**
 * Start Transfer data stream
 * Paramters: StartTransferRequest struct
 */
#define IOCTL_ITE_DEMOD_STARTTRANSFER_TX \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_OTHER + 0x07, StartTransferRequest)
	
#define IOCTL_ITE_DEMOD_STOPTRANSFER_TX \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_OTHER + 0x08, StopTransferRequest)
	
#define IOCTL_ITE_DEMOD_GETDRIVERINFO_TX \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_OTHER + 0x09, TxModDriverInfo)
	
#define IOCTL_ITE_DEMOD_STARTTRANSFER_CMD_TX \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_OTHER + 0x0A, StartTransferRequest)
	
#define IOCTL_ITE_DEMOD_STOPTRANSFER_CMD_TX \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_OTHER + 0x0B, StopTransferRequest)
	
#define IOCTL_ITE_DEMOD_WRITECMD_TX \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_OTHER + 0x0C, CmdRequest)	
	
#define IOCTL_ITE_DEMOD_GETTPS_TX \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_OTHER + 0x0D, GetTPSRequest)	
	
#define IOCTL_ITE_DEMOD_SETTPS_TX \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_OTHER + 0x0E, SetTPSRequest)		

#define IOCTL_ITE_DEMOD_SENDHWPSITABLE_TX \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_OTHER + 0x0F, SendHwPSITableRequest)		
		
#define IOCTL_ITE_DEMOD_ACCESSFWPSITABLE_TX \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_OTHER + 0x10, AccessFwPSITableRequest)		

#define IOCTL_ITE_DEMOD_SETFWPSITABLETIMER_TX \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_OTHER + 0x11, SetFwPSITableTimerRequest)		

#define IOCTL_ITE_DEMOD_SETLOWBRATETRANS_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_OTHER + 0x12, SetLowBitRateTransferRequest)	


u32 DemodIOCTLFun(
    void *       demodulator,
    u32        IOCTLCode,
    unsigned long        pIOBuffer);

#endif
