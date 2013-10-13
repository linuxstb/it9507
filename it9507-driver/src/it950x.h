/**
 * Copyright (c) 2013 ITE Corporation. All rights reserved. 
 *
 * Module Name:
 *   it950x-core.h
 *
 * Abstract:
 *   The definitions of header for core driver.
 *
 */

#ifndef _IT950x_H_
#define _IT950x_H_

#include "../include/dvbmod.h"



//***************** from device.h *****************//

extern int dvb_usb_it950x_debug;
//#define deb_info(args...)   dprintk(dvb_usb_it950x_debug,0x01,args)
//#define deb_fw(args...)     dprintk(dvb_usb_it950x_debug,0x02,args)
//#define deb_fwdata(args...) dprintk(dvb_usb_it950x_debug,0x04,args)
//#define deb_data(args...)   dprintk(dvb_usb_it950x_debug,0x08,args)


#define DEBUG 1
#ifdef DEBUG 
#define deb_data(args...)   printk(KERN_NOTICE args)
#else
#define deb_data(args...)
#endif

/* Ring Buffer Debug */
#define RB_DEBUG 0
#ifdef RB_DEBUG 
#define deb_data(args...)   printk(KERN_NOTICE args)
#else
#define deb_data(args...)
#endif


typedef struct {
    u32 frequency;      /**  */
    int  dAmp;			  /**  */
	int  dPhi;	
} IQtable;

typedef struct {
    IQtable *ptrIQtableEx;
	u16 tableGroups;		//Number of IQtable groups;
	s32	outputGain;	
    u16 c1DefaultValue;
	u16 c2DefaultValue;
	u16 c3DefaultValue;
} CalibrationInfo;

//***************** from device.h *****************//

typedef struct _TUNER_INFO {
    bool bTunerInited;
    bool bSettingFreq;
    bool bTunerOK;
} TUNER_INFO, *PTUNER_INFO;

typedef struct _FILTER_CONTEXT_HW {
    u32 ulCurrentFrequency;
    u16  ucCurrentBandWidth;  
    u32 ulDesiredFrequency;
    u16  ucDesiredBandWidth;   
    TUNER_INFO tunerinfo; 
    bool bApOn;
    int bResetTs;
} FILTER_CONTEXT_HW, *PFILTER_CONTEXT_HW;  

struct it950x_state {
	/** Basic structure */
	struct usb_device *udev;
	u8 i2cAddr;
	u16 bandwidth;
	u32 frequency;    
	bool booted;
	u8 slaveIICAddr;
	CalibrationInfo calibrationInfo;
        FILTER_CONTEXT_HW fc;
};

extern u32 Device_init(struct usb_device *udev,struct it950x_state *state, bool bBoot);
extern u32 DL_ApPwCtrl (struct it950x_state *state, bool  bOn);
extern u32 DL_ReSetInterval(void);
extern u32 DL_Reboot(struct it950x_state *state);
extern u32 DL_CheckTunerInited(struct it950x_state *state, bool *bOn);
extern u32 DL_DemodIOCTLFun(struct it950x_state* state, u32 IOCTLCode, unsigned long pIOBuffer);
extern u32 DL_LoadIQtable_Fromfile(void *handle);

#endif

