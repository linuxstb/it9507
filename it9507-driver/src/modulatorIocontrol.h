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
    Byte			chip;
    Processor		processor;
    Dword			registerAddress;
    Byte			bufferLength;
    Byte			buffer[256];
    Dword			error;
    Byte			reserved[16];
} WriteRegistersRequest, *PWriteRegistersRequest;

typedef struct {
    Byte			chip;
    Processor		processor;
    Dword			registerAddress;
    Byte			bufferLength;
    Byte			buffer[256];
    Dword			error;
    Byte			reserved[16];
} TxWriteRegistersRequest, *PTxWriteRegistersRequest;

typedef struct {
    Byte			chip;
    Word			registerAddress;
    Byte			bufferLength;
    Byte			buffer[256];
    Dword			error;
    Byte			reserved[16];
} WriteEepromValuesRequest, *PWriteEepromValuesRequest;

typedef struct {
    Byte			chip;
    Processor		processor;
    Dword			registerAddress;
    Byte			position;
    Byte			length;
    Byte			value;
    Dword			error;
    Byte			reserved[16];
} WriteRegisterBitsRequest, *PWriteRegisterBitsRequest;

typedef struct {
    Byte			chip;
    Processor		processor;
    Dword			registerAddress;
    Byte			position;
    Byte			length;
    Byte			value;
    Dword			error;
    Byte			reserved[16];
} TxWriteRegisterBitsRequest, *PTxWriteRegisterBitsRequest;

typedef struct {
    Byte			chip;
    Processor		processor;
    Word			variableIndex;
    Byte			bufferLength;
    Byte			buffer[256];
    Dword			error;
    Byte			reserved[16];
} SetVariablesRequest, *PSetVariablesRequest;

typedef struct {
    Byte			chip;
    Processor		processor;
    Word			variableIndex;
    Byte			position;
    Byte			length;
    Byte			value;
    Dword			error;
    Byte			reserved[16];
} SetVariableBitsRequest, *PSetVariableBitsRequest;

typedef struct {
    Byte			chip;
    Processor		processor;
    Dword			registerAddress;
    Byte			bufferLength;
    Byte			buffer[256];
    Dword			error;
    Byte			reserved[16];
} ReadRegistersRequest, *PReadRegistersRequest;

typedef struct {
    Byte			chip;
    Processor		processor;
    Dword			registerAddress;
    Byte			bufferLength;
    Byte			buffer[256];
    Dword			error;
    Byte			reserved[16];
} TxReadRegistersRequest, *PTxReadRegistersRequest;

typedef struct {
    Byte			chip;
    Word			registerAddress;
    Byte			bufferLength;
    Byte			buffer[256];
    Dword			error;
    Byte			reserved[16];
} ReadEepromValuesRequest, *PReadEepromValuesRequest;

typedef struct {
    Byte			chip;
    Word			registerAddress;
    Byte			bufferLength;
    Byte			buffer[256];
    Dword			error;
    Byte			reserved[16];
} TxReadEepromValuesRequest, *PTxReadEepromValuesRequest;

typedef struct {
    Byte			chip;
    Processor		processor;
    Dword			registerAddress;
    Byte			position;
    Byte			length;
    Byte*			value;
    Dword			error;
    Byte			reserved[16];
} ReadRegisterBitsRequest, *PReadRegisterBitsRequest;

typedef struct {
    Byte			chip;
    Processor		processor;
    Dword			registerAddress;
    Byte			position;
    Byte			length;
    Byte*			value;
    Dword			error;
    Byte			reserved[16];
} TxReadRegisterBitsRequest, *PTxReadRegisterBitsRequest;

typedef struct {
    Byte			chip;
    Processor		processor;
    Word			variableIndex;
    Byte			bufferLength;
    Byte*			buffer;
    Dword			error;
    Byte			reserved[16];
} GetVariablesRequest, *PGetVariablesRequest;

typedef struct {
    Byte			chip;
    Processor		processor;
    Word			variableIndex;
    Byte			position;
    Byte			length;
    Byte*			value;
    Dword			error;
    Byte			reserved[16];
} GetVariableBitsRequest, *PGetVariableBitsRequest;

typedef struct {
    Byte				chipNumber;
    Word				sawBandwidth;
    StreamType			streamType;
    Architecture		architecture;
    Dword				error;
    Byte				reserved[16];
} InitializeRequest, *PInitializeRequest;


typedef struct {
    Dword				error;
    Byte				reserved[16];
} FinalizeRequest, *PFinalizeRequest;

typedef struct {
    Dword				error;
    Byte				reserved[16];
} TxFinalizeRequest, *PTxFinalizeRequest;

typedef struct {
    StreamType*			streamType;
    Dword				error;
    Byte				reserved[16];
} GetStreamTypeRequest, *PGetStreamTypeRequest;

typedef struct {
    Architecture*		architecture;
    Dword				error;
    Byte				reserved[16];
} GetArchitectureRequest, *PGetArchitectureRequest;

typedef struct {
    StreamType			streamType;
    Architecture		architecture;
    Dword				error;
    Byte				reserved[16];
} ReinitializeRequest, *PReinitializeRequest;

typedef struct {
    Processor			processor;
	Dword*				version;
    Dword				error;
    Byte				reserved[16];
} GetFirmwareVersionRequest, *PGetFirmwareVersionRequest;

typedef struct {
    Processor			processor;
	Dword*				version;
    Dword				error;
    Byte				reserved[16];
} TxGetFirmwareVersionRequest, *PTxGetFirmwareVersionRequest;

typedef struct {
    Byte				chip;
    Word				bandwidth;
    Dword				frequency;
    Dword				error;
    Byte				reserved[16];
} AcquireChannelRequest, *PAcquireChannelRequest;

typedef struct {
    Byte				chip;
    Byte 				transmissionMode;
	Byte				constellation;
	Byte				interval;
	Byte				highCodeRate;
    Dword				error;
    Byte				reserved[16];
} SetModuleRequest, *PSetModuleRequest;

typedef struct {
    Byte				chip;
    Word				bandwidth;
    Dword				frequency;
    Dword				error;
    Byte				reserved[16];
} TxAcquireChannelRequest, *PTxAcquireChannelRequest;

typedef struct {
    Byte				OnOff;
    Dword				error;
    Byte				reserved[16];
} TxModeRequest, *PTxModeRequest;

typedef struct {
    Byte				DeviceType;
    Dword				error;
    Byte				reserved[16];
} TxSetDeviceTypeRequest, *PTxSetDeviceTypeRequest;

typedef struct {
    Byte				DeviceType;
    Dword				error;
    Byte				reserved[16];
} TxGetDeviceTypeRequest, *PTxGetDeviceTypeRequest;

typedef struct {
    int				GainValue;
    Dword				error;
} SetGainRequest, *PSetGainRequest;

typedef struct {
    Byte				chip;
    Bool*				locked;
    Dword				error;
    Byte				reserved[16];
} IsLockedRequest, *PIsLockedRequest;

typedef struct {
    Byte*				platformLength;
    Platform*			platforms;
    Dword				error;
    Byte				reserved[16];
} AcquirePlatformRequest, *PAcquirePlatformRequest;

typedef struct {
    Platform*			platform;
    Dword				error;
    Byte				reserved[16];
} SetPlatformRequest, *PSetPlatformRequest;

typedef struct {
    Ip					ip;
    Dword				error;
    Byte				reserved[16];
} AddIpRequest, *PAddIpRequest;

typedef struct {
    Ip					ip;
    Dword				error;
    Byte				reserved[16];
} RemoveIpRequest, *PRemoveIpRequest;

typedef struct {
	Byte				chip;
	Byte				index;
    Pid					pid;
    Dword				error;
    Byte				reserved[16];
} AddPidAtRequest, *PAddPidAtRequest;

typedef struct {
	Byte				chip;
	Byte				index;
    Pid					pid;
    Dword				error;
    Byte				reserved[16];
} TxAddPidAtRequest, *PTxAddPidAtRequest;

typedef struct {
	Byte			chip;
    Dword			error;
    Byte			reserved[16];
} ResetPidRequest, *PResetPidRequest;

typedef struct {
	Byte			chip;
    Dword			error;
    Byte			reserved[16];
} TxResetPidRequest, *PTxResetPidRequest;

typedef struct {
    Word*				bufferLength;
    Byte*				buffer;
    Dword				error;
    Byte				reserved[16];
} GetSectionRequest, *PGetSectionRequest;

typedef struct {
    Byte				chip;
    ChannelStatistic*	channelStatistic;
    Dword				error;
    Byte				reserved[16];
} GetChannelStatisticRequest, *PGetChannelStatisticRequest;

typedef struct {
    Byte				chip;
    Statistic*			statistic;
    Dword				error;
    Byte				reserved[16];
} GetStatisticRequest, *PGetStatisticRequest;

typedef struct {
    Dword*				bufferLength;
    Byte*				buffer;
    Dword				error;
    Byte				reserved[16];
} GetDatagramRequest, *PGetDatagramRequest;

typedef struct {
    Byte			chip;
	Byte			control;
    Dword			error;
    Byte			reserved[16];
} ControlPidFilterRequest, *PControlPidFilterRequest;

typedef struct {
    Byte			chip;
	Byte			control;
    Dword			error;
    Byte			reserved[16];
} TxControlPidFilterRequest, *PTxControlPidFilterRequest;

typedef struct {
    Byte				chip;
    Byte				control;
    Dword				error;
    Byte				reserved[16];
} ControlPowerSavingRequest, *PControlPowerSavingRequest;

typedef struct {
    Byte				chip;
    Byte				control;
    Dword				error;
    Byte				reserved[16];
} TxControlPowerSavingRequest, *PTxControlPowerSavingRequest;

typedef struct {
    Byte			chip;
    Dword			slaveAddress;
    Byte			bufferLength;
    Byte			buffer[256];
    Dword			error;
    Byte			reserved[16];
} WriteGenericRegistersRequest, *PWriteGenericRegistersRequest;

typedef struct {
    Byte			chip;
    Dword			slaveAddress;
    Byte			bufferLength;
    Byte			buffer[256];
    Dword			error;
    Byte			reserved[16];
} ReadGenericRegistersRequest, *PReadGenericRegistersRequest;

typedef struct {
    Byte                DriverVerion[16];   /** XX.XX.XX.XX Ex., 1.2.3.4			*/
    Byte                APIVerion[32];      /** XX.XX.XXXXXXXX.XX Ex., 1.2.3.4	*/
    Byte                FWVerionLink[16];   /** XX.XX.XX.XX Ex., 1.2.3.4			*/
    Byte                FWVerionOFDM[16];   /** XX.XX.XX.XX Ex., 1.2.3.4			*/
    Byte                DateTime[24];       /** Ex.,"2004-12-20 18:30:00" or "DEC 20 2004 10:22:10" with compiler __DATE__ and __TIME__  definitions */
    Byte                Company[8];         /** Ex.,"ITEtech"					*/
    Byte                SupportHWInfo[32];  /** Ex.,"Jupiter DVBT/DVBH"			*/
    Dword               error;
    Byte                reserved[128];
} DemodDriverInfo, *PDemodDriverInfo;

typedef struct {
    Byte                DriverVerion[16];   /** XX.XX.XX.XX Ex., 1.2.3.4			*/
    Byte                APIVerion[32];      /** XX.XX.XXXXXXXX.XX Ex., 1.2.3.4	*/
    Byte                FWVerionLink[16];   /** XX.XX.XX.XX Ex., 1.2.3.4			*/
    Byte                FWVerionOFDM[16];   /** XX.XX.XX.XX Ex., 1.2.3.4			*/
    Byte                DateTime[24];       /** Ex.,"2004-12-20 18:30:00" or "DEC 20 2004 10:22:10" with compiler __DATE__ and __TIME__  definitions */
    Byte                Company[8];         /** Ex.,"ITEtech"					*/
    Byte                SupportHWInfo[32];  /** Ex.,"Jupiter DVBT/DVBH"			*/
    Dword               error;
    Byte                reserved[128];
} TxModDriverInfo, *PTxModDriverInfo;

/**
 * Demodulator Stream control API commands
 */
typedef struct {
	Byte			chip;
    Dword			error;
    Byte			reserved[16];
} StartCaptureRequest, *PStartCaptureRequest;

typedef struct {
	Byte			chip;
    Dword			error;
    Byte			reserved[16];
} StartTransferRequest, *PStartTransferRequest;

typedef struct {
	Byte			chip;
    Dword			error;
    Byte			reserved[16];
} StopTransferRequest, *PStopTransferRequest;

typedef struct {
	Byte			chip;
    Dword			error;
    Byte			reserved[16];
} StopCaptureRequest, *PStopCaptureRequest;

typedef struct {
	Byte			chip;
	Byte            I2CAddr;
    Dword			error;
    Byte			reserved[16];
} GetEEPROMI2CAddrRequest, *PGetEEPROMI2CAddrRequest;

typedef struct {
	Byte			chip;
	Dword           handle;
    Dword			error;
    Byte			reserved[16];
} GetBufferEventRequest, *PGetBufferEventRequest;

typedef struct {
	Byte			chip;
	Dword           length;
    Dword			error;
    Byte			reserved[16];
} GetTSBufferLenRequest, *PGetTSBufferLenRequest;

typedef struct {
	Byte			chip;
    Dword			error;
    Byte			reserved[16];
} CleanTSDataBufferRequest, *PCleanTSDataBufferRequest;

typedef struct {
	Dword*			len;
    Byte*			cmd;
    Byte			reserved[16];
} CmdRequest, *PCmdRequest;

typedef struct {
    Dword			error;
	Dword          frequency;
	Word           bandwidth;    
	int*			maxGain;
	int*			minGain;
    Byte			reserved[16];	
} GetGainRangeRequest, *PGetGainRangeRequest;

typedef struct {
    Dword			error;
    pTPS           pTps;
    Byte			reserved[16];	
} GetTPSRequest, *PGetTPSRequest;

typedef struct {
    Dword		   error;
    TPS           tps;
    Byte		   reserved[16];	
} SetTPSRequest, *PSetTPSRequest;

typedef struct {
    Dword		   error;
	int			  *gain;	 
    Byte		   reserved[16];	
} GetOutputGainRequest, *PGetOutputGainRequest;

typedef struct {
    Dword		   error;
	Byte         *pbuffer;
    Byte		   reserved[16];	
} SendHwPSITableRequest, *PSendHwPSITableRequest;

typedef struct {
    Dword		   error;
	Byte		   psiTableIndex;
	Byte         *pbuffer;
    Byte		   reserved[16];	
} AccessFwPSITableRequest, *PAccessFwPSITableRequest;

typedef struct {
    Dword		   error;
	Byte		   psiTableIndex;
	Word           timer;
    Byte		   reserved[16];	
} SetFwPSITableTimerRequest, *PSetFwPSITableTimerRequest;

typedef struct {
    Byte* 				pBuffer;
    Dword 				pdwBufferLength;
    Dword				error;
    Byte				reserved[16];
} SetLowBitRateTransferRequest, *PSetLowBitRateTransferRequest;

typedef struct {
	IQtable*           IQ_table_ptr;
	Word				tableGroups;
    Dword				error;
    Byte				reserved[16];
} TxSetIQTableRequest, *PTxSetIQTableRequest;

typedef struct {
    int					dc_i;
	int					dc_q;
    Dword				error;
    Byte				reserved[16];
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


/***************************************************************************/
/*                             STANDARD                                    */
/***************************************************************************/

/**
 * Write a sequence of bytes to the contiguous registers in demodulator.
 * Paramters:   WriteRegistersRequest struct
 */
#define IOCTL_ITE_DEMOD_WRITEREGISTERS \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x00, WriteRegistersRequest)

/**
 * Write one byte to the contiguous registers in demodulator.
 * Paramters:   WriteRegisterBitsRequest struct
 */
#define IOCTL_ITE_DEMOD_WRITEREGISTERBITS \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x03, WriteRegisterBitsRequest)

/**
 * Write a sequence of bytes to the contiguous variables in demodulator.
 * Paramters:   SetVariablesRequest struct
 */
#define IOCTL_ITE_DEMOD_SETVARIABLES \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x04, SetVariablesRequest)

/**
 * Write a sequence of bytes to the contiguous variables in demodulator.
 * Paramters:   SetVariableBitsRequest struct
 */
#define IOCTL_ITE_DEMOD_SETVARIABLEBITS \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x05, SetVariableBitsRequest)

/**
 * Read a sequence of bytes from the contiguous registers in demodulator.
 * Paramters:   ReadRegistersRequest struct
 */
#define IOCTL_ITE_DEMOD_READREGISTERS \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x06, ReadRegistersRequest)

/**
 * Read a sequence of bytes from the contiguous registers in demodulator.
 * Paramters:   ReadRegisterBitsRequest struct
 */
#define IOCTL_ITE_DEMOD_READREGISTERBITS \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x09, ReadRegisterBitsRequest)

/**
 * Read a sequence of bytes from the contiguous variables in demodulator. 
 * Paramters:   GetVariablesRequest struct
 */
#define IOCTL_ITE_DEMOD_GETVARIABLES \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x0A, GetVariablesRequest)
	
/**
 * Read a sequence of bytes from the contiguous variables in demodulator. 
 * Paramters:   GetVariableBitsRequest struct
 */
#define IOCTL_ITE_DEMOD_GETVARIABLEBITS \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x0B, GetVariableBitsRequest)

/**
 * Power off the demodulators.
 * Paramters: 	FinalizeRequest struct
 */
#define IOCTL_ITE_DEMOD_FINALIZE \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x0D, FinalizeRequest)



/**
 * Set the output stream type of chip. Because the device could output in 
 * many stream type, therefore host have to choose one type before receive 
 * data.
 * Paramters:   GetFirmwareVersionRequest struct
 */
#define IOCTL_ITE_DEMOD_GETFIRMWAREVERSION \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x13, GetFirmwareVersionRequest)

/**
 * Specify the bandwidth of channel and tune the channel to the specific
 * frequency. Afterwards, host could use output parameter dvbH to determine
 * if there is a DVB-H signal.
 * In DVB-T mode, after calling this function output parameter dvbH should
 * be False and host could use output parameter "locked" to indicate if the 
 * TS is correct.
 * In DVB-H mode, after calling this function output parameter dvbH should
 * be True and host could use Jupiter_acquirePlatorm to get platform. 
 * Paramters:   AcquireChannelRequest struct
 */
#define IOCTL_ITE_DEMOD_ACQUIRECHANNEL \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x14, AcquireChannelRequest)

/**
 * Get all the platforms found in current frequency.
 * Paramters:	IsLockedRequest struct
 */
#define IOCTL_ITE_DEMOD_ISLOCKED \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x15, IsLockedRequest)

/**
 * Get the statistic values of demodulator, it includes Pre-Viterbi BER,
 * Post-Viterbi BER, Abort Count, Signal Presented Flag, Signal Locked Flag,
 * Signal Quality, Signal Strength, Delta-T for DVB-H time slicing.
 * Paramters:	GetStatisticRequest struct
 */
#define IOCTL_ITE_DEMOD_GETSTATISTIC \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x18, GetStatisticRequest)

/**
 * Get the statistic values of demodulator, it includes Pre-Viterbi BER,
 * Post-Viterbi BER, Abort Count, Signal Presented Flag, Signal Locked Flag,
 * Signal Quality, Signal Strength, Delta-T for DVB-H time slicing.
 * Paramters:	GetChannelStatisticRequest struct
 */
#define IOCTL_ITE_DEMOD_GETCHANNELSTATISTIC \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x19, GetChannelStatisticRequest)


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


Dword DemodIOCTLFun(
    void *       demodulator,
    Dword        IOCTLCode,
    unsigned long        pIOBuffer);

#endif
