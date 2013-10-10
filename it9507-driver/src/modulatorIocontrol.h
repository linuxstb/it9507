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

#include <linux/types.h>

/**
 * The type defination of Bandwidth.
 */
typedef enum {
    Bandwidth_6M = 0,           /** Signal bandwidth is 6MHz */
    Bandwidth_7M,               /** Signal bandwidth is 7MHz */
    Bandwidth_8M,               /** Signal bandwidth is 8MHz */
    Bandwidth_5M,               /** Signal bandwidth is 5MHz */
	Bandwidth_4M,               /** Signal bandwidth is 4MHz */
	Bandwidth_3M,               /** Signal bandwidth is 3MHz */
	Bandwidth_2_5M ,            /** Signal bandwidth is 2.5MHz */
	Bandwidth_2M                /** Signal bandwidth is 2MHz */
} Bandwidth;

/**
 * The type defination of TransmissionMode.
 */
typedef enum {
    TransmissionMode_2K = 0,    /** OFDM frame consists of 2048 different carriers (2K FFT mode) */
    TransmissionMode_8K = 1,    /** OFDM frame consists of 8192 different carriers (8K FFT mode) */
    TransmissionMode_4K = 2     /** OFDM frame consists of 4096 different carriers (4K FFT mode) */
} TransmissionModes;


/**
 * The type defination of Interval.
 */
typedef enum {
    Interval_1_OVER_32 = 0,     /** Guard interval is 1/32 of symbol length */
    Interval_1_OVER_16,         /** Guard interval is 1/16 of symbol length */
    Interval_1_OVER_8,          /** Guard interval is 1/8 of symbol length  */
    Interval_1_OVER_4           /** Guard interval is 1/4 of symbol length  */
} Interval;


/**
 * The type defination of CodeRate.
 */
typedef enum {
    CodeRate_1_OVER_2 = 0,      /** Signal uses FEC coding ratio of 1/2 */
    CodeRate_2_OVER_3,          /** Signal uses FEC coding ratio of 2/3 */
    CodeRate_3_OVER_4,          /** Signal uses FEC coding ratio of 3/4 */
    CodeRate_5_OVER_6,          /** Signal uses FEC coding ratio of 5/6 */
    CodeRate_7_OVER_8,          /** Signal uses FEC coding ratio of 7/8 */
    CodeRate_NONE               /** None, NXT doesn't have this one     */
} CodeRate;




/**
 * The type defination of Constellation.
 */
typedef enum {

    Constellation_QPSK = 0,     /** Signal uses QPSK constellation  */
    Constellation_16QAM,        /** Signal uses 16QAM constellation */
    Constellation_64QAM         /** Signal uses 64QAM constellation */
} Constellation;

/**
 * The defination of ChannelInformation.
 */
typedef struct {
    u32 frequency;                    /** Channel frequency in KHz.                                */
    TransmissionModes transmissionMode; /** Number of carriers used for OFDM signal                  */
    Constellation constellation;        /** Constellation scheme (FFT mode) in use                   */
    Interval interval;                  /** Fraction of symbol length used as guard (Guard Interval) */
    CodeRate highCodeRate;              /** FEC coding ratio of high-priority stream                 */
    Bandwidth bandwidth;
} ChannelModulation;

typedef struct _TPS{
    u16 cellid;
} TPS, *pTPS;

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
