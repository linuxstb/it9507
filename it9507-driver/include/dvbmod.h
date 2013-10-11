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

#ifndef _DVBMOD_H_
#define _DVBMOD_H_

#include <linux/types.h>
#include <linux/dvb/frontend.h>

/**
 * The defination of ChannelInformation.
 */
typedef struct {
    __u32 frequency;                    /** Channel frequency in KHz.                                */
    fe_transmit_mode_t transmissionMode; /** Number of carriers used for OFDM signal                  */
    fe_modulation_t constellation;        /** Constellation scheme (FFT mode) in use                   */
    fe_guard_interval_t interval;                  /** Fraction of symbol length used as guard (Guard Interval) */
    fe_code_rate_t highCodeRate;              /** FEC coding ratio of high-priority stream                 */
} ChannelModulation;

/* Use 'k' as magic number */
#define AFA_IOC_MAGIC  'k'

typedef struct {
    fe_transmit_mode_t transmissionMode;
    fe_modulation_t constellation;
    fe_guard_interval_t interval;
    fe_code_rate_t highCodeRate;
    __u32				error;
} SetModuleRequest, *PSetModuleRequest;

typedef struct {
    __u16				bandwidth;
    __u32				frequency;
    __u32				error;
} TxAcquireChannelRequest, *PTxAcquireChannelRequest;

typedef struct {
    int				GainValue;
    __u32				error;
} SetGainRequest, *PSetGainRequest;


typedef struct {
    __u32			error;
	__u32          frequency;
	__u16           bandwidth;    
	int*			maxGain;
	int*			minGain;
} GetGainRangeRequest, *PGetGainRangeRequest;

typedef struct {
    __u32		   error;
    __u16 cellid;
} SetTPSCellIdRequest, *PSetTPSCellIdRequest;

typedef struct {
    __u32		   error;
	int			  *gain;	 
} GetOutputGainRequest, *PGetOutputGainRequest;

/**
 * Modulator API commands
 */

#define ITE_MOD_SETMODULE         _IOW('k', 0x40, SetModuleRequest)
#define ITE_MOD_ACQUIRECHANNEL    _IOW('k', 0x41, TxAcquireChannelRequest)
#define ITE_MOD_ADJUSTOUTPUTGAIN  _IOW('k', 0x42, SetGainRequest)
#define ITE_MOD_GETGAINRANGE      _IOW('k', 0x43, GetGainRangeRequest)
#define ITE_MOD_GETOUTPUTGAIN     _IOW('k', 0x44, GetOutputGainRequest)
#define ITE_MOD_STARTTRANSFER     _IO('k', 0x45)
#define ITE_MOD_STOPTRANSFER      _IO('k', 0x46)
#define ITE_MOD_SETTPSCELLID      _IOR('k', 0x47, SetTPSCellIdRequest)

#endif
