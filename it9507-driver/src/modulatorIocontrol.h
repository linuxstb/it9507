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
    int				GainValue;
    u32				error;
} SetGainRequest, *PSetGainRequest;

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
    u32		   error;
    TPS           tps;
    u8		   reserved[16];	
} SetTPSRequest, *PSetTPSRequest;

typedef struct {
    u32		   error;
	int			  *gain;	 
    u8		   reserved[16];	
} GetOutputGainRequest, *PGetOutputGainRequest;

/**
 * Demodulator API commands
 */
#define IOCTRL_ITE_GROUP_STANDARD           0x000
#define IOCTRL_ITE_GROUP_OTHER              0x500

#define IOCTL_ITE_DEMOD_SETMODULE_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x21, SetModuleRequest)
	
#define IOCTL_ITE_DEMOD_ACQUIRECHANNEL_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x22, TxAcquireChannelRequest)

#define IOCTL_ITE_DEMOD_ADJUSTOUTPUTGAIN_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x2B, SetGainRequest)

#define IOCTL_ITE_DEMOD_GETGAINRANGE_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x2C, GetGainRangeRequest)

#define IOCTL_ITE_DEMOD_GETOUTPUTGAIN_TX \
	_IOW(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_STANDARD + 0x2D, GetOutputGainRequest)

#define IOCTL_ITE_DEMOD_STARTTRANSFER_TX \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_OTHER + 0x07, StartTransferRequest)
	
#define IOCTL_ITE_DEMOD_STOPTRANSFER_TX \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_OTHER + 0x08, StopTransferRequest)
	
#define IOCTL_ITE_DEMOD_SETTPS_TX \
	_IOR(AFA_IOC_MAGIC, IOCTRL_ITE_GROUP_OTHER + 0x0E, SetTPSRequest)

u32 DemodIOCTLFun(void *demodulator, u32 IOCTLCode, unsigned long pIOBuffer);

#endif
