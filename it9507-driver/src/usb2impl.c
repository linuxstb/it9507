/*#define __USB2IMPL_C__ for Linux  */
// all for Linux
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>
#include <linux/usb.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/firmware.h>
#include <linux/delay.h>
#include <linux/vmalloc.h>

#include "usb2impl.h"
#include "it950x-core.h"
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 33)				
#include <linux/autoconf.h>
#else
#include <generated/autoconf.h>
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 39)
#include <linux/smp_lock.h>
#endif

#ifdef UNDER_CE

Handle Usb2_handle = NULL;


Dword Usb2_getDriver (
    IN  Modulator*    modulator,
    OUT Handle*         handle
) {
    return (Error_NO_ERROR);
}


Dword Usb2_exitDriver (
    IN  Modulator*    modulator
) {
    return (Error_NO_ERROR);
}


Dword Usb2_writeControlBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    IN  Byte*           buffer
) {
    return (Error_NO_ERROR);
}


Dword Usb2_readControlBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
) {
    return (Error_NO_ERROR);
}


Dword Usb2_readDataBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
) {
    return (Error_NO_ERROR);
}

#else

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

Handle Usb2_handle = 0;



Dword Usb2_getDriver (
    IN  Modulator*    modulator,
    OUT Handle*         handle
) {
    Dword error = ModulatorError_NO_ERROR;

    return (error);
}


Dword Usb2_exitDriver (
    IN  Modulator*    modulator
) {
    Dword error = ModulatorError_NO_ERROR;

    return (error);
}


Dword Usb2_writeControlBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    IN  Byte*           buffer
) {
    Dword     ret;
    int		  act_len;
	Byte *pTmpBuffer = kzalloc(sizeof(buffer)*bufferLength, GFP_KERNEL);
	ret = 0;

	if (pTmpBuffer) 
		memcpy(pTmpBuffer, buffer, bufferLength);
//deb_data(" ---------Usb2_writeControlBus----------\n", ret);	
	ret = usb_bulk_msg(usb_get_dev( modulator->userData),
			usb_sndbulkpipe(usb_get_dev(modulator->userData), 0x02),
			pTmpBuffer,
			bufferLength,
			&act_len,
			1000000);
   
	if (ret) deb_data(" Usb2_writeControlBus fail : 0x%08lx\n", ret);

	return (Error_NO_ERROR);
}


Dword Usb2_readControlBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
) {
	Dword     ret;
	int       nBytesRead;
	Byte *pTmpBuffer = kzalloc(sizeof(buffer)*bufferLength, GFP_KERNEL);
	ret = 0;

//deb_data(" ---------Usb2_readControlBus----------\n", ret);			
   ret = usb_bulk_msg(usb_get_dev(modulator->userData),
				usb_rcvbulkpipe(usb_get_dev(modulator->userData),129),
				pTmpBuffer,
				bufferLength,
				&nBytesRead,
				1000000);
	if (pTmpBuffer)
		memcpy(buffer, pTmpBuffer, bufferLength);   
	 
	if (ret) 	deb_data(" Usb2_readControlBus fail : 0x%08lx\n", ret);

	return (Error_NO_ERROR);
}


Dword Usb2_readDataBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
) {

    return (Error_NO_ERROR);
}

#endif
