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

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>
#include <linux/usb.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <asm/uaccess.h>
#include <linux/version.h>
#include <linux/mutex.h>
#include <linux/mm.h>
#include <linux/gfp.h>
#include "modulatorType.h"
#include "modulatorError.h"
#include "../include/dvbmod.h"

#define   DRIVER_RELEASE_VERSION    "v13.06.27.1"

#define URB_TEST	0
#define URB_COUNT_TX   8
#define URB_BUFSIZE_TX 32712//65424//16356//32712

//***************** from device.h *****************//

#define SLAVE_DEMOD_2WIREADDR  0x3A


//***************** from afdrv.h *****************//
//#define GANY_ONLY 0x42F5
#define EEPROM_FLB_OFS  8

#define EEPROM_IRMODE      (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x10)   //00:disabled, 01:HID
#define EEPROM_SELSUSPEND  (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x28)   //Selective Suspend Mode
#define EEPROM_TSMODE      (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x28+1) //0:one ts, 1:dual ts
#define EEPROM_2WIREADDR   (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x28+2) //MPEG2 2WireAddr
#define EEPROM_SUSPEND     (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x28+3) //Suspend Mode
#define EEPROM_IRTYPE      (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x28+4) //0:NEC, 1:RC6
#define EEPROM_SAWBW1      (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x28+5)
#define EEPROM_XTAL1       (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x28+6) //0:28800, 1:20480
#define EEPROM_SPECINV1    (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x28+7)
#define EEPROM_TUNERID     (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x30+4) //
#define EEPROM_IFFREQL     (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x30) 
#define EEPROM_IFFREQH     (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x30+1)   
#define EEPROM_IF1L        (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x30+2)   
#define EEPROM_IF1H        (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x30+3)
#define EEPROM_SHIFT       (0x10)                 //EEPROM Addr Shift for slave front end


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
	ChannelModulation channelModulation;
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

