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
#include "error.h"
#include "modulatorIocontrol.h"
#include "IT9507.h"
#include "userdef.h"
#include "modulatorFirmware.h"
#include "modulatorType.h"
#include "Common.h"
#include <linux/version.h>
#include <linux/mutex.h>
#include <linux/mm.h>
#include <linux/gfp.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 39)
#include <linux/smp_lock.h>
#endif

#define   DRIVER_RELEASE_VERSION    "v13.06.27.1"

/************** DTVCAM & AVSENDER **************/
//#define   DTVCAM_POWER_CTRL
//#define   AVSENDER_POWER_CTRL

#define URB_TEST	0
#define URB_COUNT_TX   16
#define URB_COUNT_TX_LOW_BRATE 64
#define URB_COUNT_TX_CMD   1
#define URB_COUNT_RX   16
#define URB_BUFSIZE_TX 32712//65424//16356//32712
#define URB_BUFSIZE_TX_LOW_BRATE 1504 // 188 * 8
#define URB_BUFSIZE_TX_CMD 188//65424//16356//32712
#define URB_BUFSIZE_RX 188 * 348//32712//65424//16356//32712
#define CLEAN_HARDWARE_BUFFER_SIZE 1000
//***************** Customization *****************
//#define QuantaMID 1
//#define EEEPC 1
//***************** from compat.h *****************
#if LINUX_VERSION_CODE <=  KERNEL_VERSION(2,6,18)
//typedef int bool;
#define true 1
#define false 0
#endif
//***************** from device.h *****************//
#define AFA_USB_DEVICE

#define SLAVE_DEMOD_2WIREADDR  0x3A

#define TS_PACKET_SIZE         188
#define TS_PACKET_COUNT_HI     348
#define TS_PACKET_COUNT_FU     21

//***************** from driver.h *****************//
#define TS_FRAMES_HI 128
#define TS_FRAMES_FU 128
#define MAX_USB20_IRP_NUM  5
#define MAX_USB11_IRP_NUM  2

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

/*AirHD no use, RC, after kernel 38 support*/
struct it950x_config {
	u8  dual_mode:1;
	u16 mt2060_if1[2];
	u16 firmware_size;
	u16 firmware_checksum;
	u32 eeprom_sum;
};

typedef struct _TUNER_INFO {
    Bool bTunerInited;
    Bool bSettingFreq;
    BYTE TunerId;
    Bool bTunerOK;
	Bool bTunerLock;//AirHD
    Tuner_struct MXL5005_Info;
} TUNER_INFO, *PTUNER_INFO;

typedef struct _FILTER_CONTEXT_HW {
    DWORD ulCurrentFrequency;
    WORD  ucCurrentBandWidth;  
    DWORD ulDesiredFrequency;
    WORD  ucDesiredBandWidth;   
    //ULONG ulBandWidth;   
    Bool bTimerOn;
   // PKSFILTER filter;
    Byte GraphBuilt;
    TUNER_INFO tunerinfo; 
    //SIGNAL_STATISTICS ss;
    //SIGNAL_RETRAIN sr;  
    //DWORD   gdwOrigFCW;        //move from AF901x.cpp [global variable]
    //BYTE    gucOrigUnplugTh;   //move from AF901x.cpp [global variable]
    //BYTE    gucPreShiftIdx;    //move from AF901x.cpp [global variable]    
    // PKSFILTERFACTORY  pFilterFactory;
    int  bEnPID;
    //ULONG ulcPIDs;
    //ULONG aulPIDs[32];
    Bool bApOn;
    int bResetTs;
    Byte OvrFlwChk;
} FILTER_CONTEXT_HW, *PFILTER_CONTEXT_HW;  

typedef struct _DEVICE_CONTEXT {
    FILTER_CONTEXT_HW fc[2];
    Byte DeviceNo;
    Bool bBootCode;
    Bool bEP12Error;
    Bool bEP45Error;
    //bool bDebugMsg;
    //bool bDevExist;
    Bool bDualTs;
    Bool bIrTblDownload;    
    Byte BulkOutData[256];
    u32 WriteLength;
    Bool bSurpriseRemoval;
    Bool bDevNotResp;
    Bool bEnterSuspend;
    Bool bSupportSuspend;
    Bool bSupportSelSuspend;
    u16 regIdx; 
    Byte eepromIdx;
    u16 UsbMode;
    u16 MaxPacketSize;
    u32 MaxIrpSize;
    u32 TsFrames;
    u32 TsFrameSize;
    u32 TsFrameSizeDw;
    u32 TsPacketCount;
    //BYTE  ucDemod2WireAddr;
    //USB_IDLE_CALLBACK_INFO cbinfo;          // callback info for selective suspend          // our selective suspend IRP    

    Bool    bSelectiveSuspend;
    u32   ulActiveFilter;
    //BYTE    ucSerialNo; 
    Architecture architecture;
    //BYTE Tuner_Id;
    StreamType StreamType;
    Bool bDCAPIP;
    Bool bSwapFilter;
    Byte FilterCnt;
    Bool  bTunerPowerOff;
    //PKSPIN PinSave;
    Byte UsbCtrlTimeOut;
	
	Modulator modulator;
	DefaultDemodulator demodulator;
	Byte deviceType;    
	Handle handle_driver;
    
    Bool ForceWrite;
    Byte chip_version;
    Bool bProprietaryIr;
    Byte bIrType;

    u32		nReadTmpBusLength;
    u32		nWriteTmpBusLength;
    Byte 	*pReadTmpBusBuffer;
    Byte 	*pWriteTmpBusBuffer;
	
} DEVICE_CONTEXT, *PDEVICE_CONTEXT;


struct it950x_ofdm_channel {
	u32 RF_kHz;
	u8  Bw;
	s16 nfft;
	s16 guard;
	s16 nqam;
	s16 vit_hrch;
	s16 vit_select_hp;
	s16 vit_alpha;
	s16 vit_code_rate_hp;
	s16 vit_code_rate_lp;
	u8  intlv_native;
};

struct tuner_priv {
        struct tuner_config *cfg;
        struct i2c_adapter   *i2c;

        u32 frequency;
        u32 bandwidth;
        u16 if1_freq;
        u8  fmfreq;
};

//extern struct usb_device *udevs;
//extern PDEVICE_CONTEXT PDC;
extern int it950x_device_count;

extern DWORD Device_init(struct usb_device *udev,PDEVICE_CONTEXT PDCs, Bool bBoot);
extern DWORD DL_ApCtrl(void* handle, Byte ucSlaveDemod, Bool bOn);
extern DWORD DL_ApPwCtrl(void* handle, Bool ucSlaveDemod, Bool bOn);
extern DWORD DL_Tuner_SetFreqBw(void *handle, BYTE ucSlaveDemod,u32 ucFreq,u8 ucBw);
extern DWORD DL_isLocked(void *handle,BYTE ucSlaveDemod, Bool *bLock);
extern DWORD DL_getSignalStrength(void *handle,BYTE ucSlaveDemod, BYTE* strength);
extern DWORD DL_getSignalStrengthDbm(void *handle, BYTE ucSlaveDemod, Long* strengthDbm);
extern DWORD DL_getChannelStatistic(BYTE ucSlaveDemod, ChannelStatistic*  channelStatistic);
extern DWORD DL_getChannelModulation(void *handle, BYTE ucSlaveDemod, ChannelModulation*    channelModulation);
extern DWORD DL_getSNR(void *handle, BYTE ucSlaveDemod, Constellation* constellation, BYTE* snr);
extern DWORD DL_ReSetInterval(void);
extern DWORD DL_Reboot(void* handle);
extern DWORD DL_CheckTunerInited(void *handle, BYTE ucSlaveDemod, Bool *bOn);
extern DWORD DL_DemodIOCTLFun(void* handle, DWORD IOCTLCode, unsigned long pIOBuffer);
extern DWORD DL_LoadIQtable_Fromfile(void *handle);
//extern Bool DevicePower, DeviceReboot;
// TunerInited0, TunerInited1
//extern DWORD DL_NIMSuspend(void* handle, bool bSuspend);
//extern DWORD DL_SetArchitecture(void* handle, Architecture architecture);
//extern DWORD DL_ApReset (void * handle, BYTE ucSlaveDemod, Bool bOn);
//extern DWORD DL_WriteRegisters (void * handle, BYTE ucSlaveDemod, Processor	processor, DWORD wReg, BYTE ucpValueLength, BYTE* ucpValue);
//extern DWORD DL_ReadRegisters (void * handle, BYTE ucSlaveDemod, Processor	processor, DWORD wReg, BYTE ucpValueLength, BYTE* ucpValue);
//extern void it9507_set_remote_config(struct usb_device *udev, struct dvb_usb_device_properties *props);

#endif

