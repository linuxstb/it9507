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

struct dvb_modulator_parameters {
	__u32			frequency_khz;   /* frequency in KHz */
	fe_transmit_mode_t	transmission_mode;
	fe_modulation_t		constellation;
	fe_guard_interval_t	guard_interval;
	fe_code_rate_t 		code_rate_HP;
	__u16			bandwidth_hz;    /* Bandwidth in Hz */
	__u16			cellid;
};

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
	int			  *gain;	 
} GetOutputGainRequest, *PGetOutputGainRequest;

/**
 * Modulator API commands
 */

#define ITE_MOD_SET_PARAMETERS    _IOW('k', 0x40, struct dvb_modulator_parameters)
#define ITE_MOD_ADJUSTOUTPUTGAIN  _IOW('k', 0x42, SetGainRequest)
#define ITE_MOD_GETGAINRANGE      _IOW('k', 0x43, GetGainRangeRequest)
#define ITE_MOD_GETOUTPUTGAIN     _IOW('k', 0x44, GetOutputGainRequest)
#define ITE_MOD_STARTTRANSFER     _IO('k', 0x45)
#define ITE_MOD_STOPTRANSFER      _IO('k', 0x46)

#endif
