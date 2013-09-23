#include "modulatorUser.h"

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

#include "it950x-core.h"
#include "modulatorType.h"
#include "modulatorError.h"
#include "modulatorUser.h"
#include "it9507-priv.h"

u32 EagleUser_setSystemConfig (
    IN  Modulator*    modulator
) {
	u32 error = 0;

        /* restSlave */	
	error = IT9507_writeRegister (modulator, Processor_LINK, GPIO_O(GPIOH1), 1);
	if (error) goto exit;
	error = IT9507_writeRegister (modulator, Processor_LINK, GPIO_EN(GPIOH1), 1);
	if (error) goto exit;
	error = IT9507_writeRegister (modulator, Processor_LINK, GPIO_ON(GPIOH1), 1);
	if (error) goto exit;
	EagleUser_delay(modulator, 10);

        /* powerDownSlave */
	error = IT9507_writeRegister (modulator, Processor_LINK, GPIO_O(GPIOH5), 0);
	if (error) goto exit;
	error = IT9507_writeRegister (modulator, Processor_LINK, GPIO_EN(GPIOH5), 1);
	if (error) goto exit;
	error = IT9507_writeRegister (modulator, Processor_LINK, GPIO_ON(GPIOH5), 1);
	if (error) goto exit;

        /* rfEnable */
	error = IT9507_writeRegister (modulator, Processor_LINK, GPIO_EN(GPIOH2), 1);
	if (error) goto exit;
	error = IT9507_writeRegister (modulator, Processor_LINK, GPIO_ON(GPIOH2), 1);
	if (error) goto exit;

        /* uvFilter */
	error = IT9507_writeRegister (modulator, Processor_LINK, GPIO_EN(GPIOH8), 1);
	if (error) goto exit;
	error = IT9507_writeRegister (modulator, Processor_LINK, GPIO_ON(GPIOH8), 1);
	if (error) goto exit;

exit:
    return (ModulatorError_NO_ERROR);
}

u32 EagleUser_getDeviceType (
	IN  Modulator*    modulator,
	OUT  u8*		  deviceType	   
){	
	u32 error = ModulatorError_NO_ERROR;
	u8 temp;

	
	error = IT9507_readRegister (modulator, Processor_LINK, 0x4979, &temp);//has eeprom ??
	if((temp == 1) && (error == ModulatorError_NO_ERROR)){
		error = IT9507_readRegister (modulator, Processor_LINK, 0x49D5, &temp);	
		if(error == ModulatorError_NO_ERROR){
			*deviceType = temp;	
		}else if(temp == 0){ // No eeprom 
			*deviceType = EagleUser_DEVICETYPE;
		}
	}
	else
	{
		*deviceType = 1;
	}
	
	return(error);
}




u32 EagleUser_memoryCopy (
    IN  Modulator*    modulator,
    IN  void*           dest,
    IN  void*           src,
    IN  u32           count
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  memcpy(dest, src, (size_t)count);
     *  return (0);
     */
    return (ModulatorError_NO_ERROR);
}

u32 EagleUser_delay (
    IN  Modulator*    modulator,
    IN  u32           dwMs
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  delay(dwMs);
     *  return (0);
     */
	msleep(dwMs); 
    return (ModulatorError_NO_ERROR);
}


u32 EagleUser_enterCriticalSection (
    IN  Modulator*    modulator
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  return (0);
     */
    return (ModulatorError_NO_ERROR);
}


u32 EagleUser_leaveCriticalSection (
    IN  Modulator*    modulator
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  return (0);
     */
    return (ModulatorError_NO_ERROR);
}


u32 EagleUser_mpegConfig (
    IN  Modulator*    modulator
) {
    /*
     *  ToDo:  Add code here
     *
     */
    return (ModulatorError_NO_ERROR);
}


u32 EagleUser_busTx (
    IN  Modulator*    modulator,
    IN  u32           bufferLength,
    IN  u8*           buffer
) {
    u32     ret;
    int		  act_len;
	u8 *pTmpBuffer = kzalloc(sizeof(buffer)*bufferLength, GFP_KERNEL);
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
   
	if (ret) deb_data(" Usb2_writeControlBus fail : 0x%08x\n", ret);

	return (Error_NO_ERROR);
}


u32 EagleUser_busRx (
    IN  Modulator*    modulator,
    IN  u32           bufferLength,
    OUT u8*           buffer
) {
	u32     ret;
	int       nu8sRead;
	u8 *pTmpBuffer = kzalloc(sizeof(buffer)*bufferLength, GFP_KERNEL);
	ret = 0;

//deb_data(" ---------Usb2_readControlBus----------\n", ret);			
   ret = usb_bulk_msg(usb_get_dev(modulator->userData),
				usb_rcvbulkpipe(usb_get_dev(modulator->userData),129),
				pTmpBuffer,
				bufferLength,
				&nu8sRead,
				1000000);
	if (pTmpBuffer)
		memcpy(buffer, pTmpBuffer, bufferLength);   
	 
	if (ret) 	deb_data(" Usb2_readControlBus fail : 0x%08x\n", ret);

	return (Error_NO_ERROR);
}


 u32 EagleUser_Initialization  (
    IN  Modulator*    modulator
) {
	/*
     *  ToDo:  Add code here
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
	u32 error = 0;
	 error = EagleUser_setSystemConfig(modulator);
	 if (error) goto exit;

	// RF Enable
	error = IT9507_writeRegister (modulator, Processor_LINK, GPIO_O(GPIOH2), 0); //RF out power down
	if (error) goto exit;
exit:
    return (error);

 }


u32 EagleUser_acquireChannel (
	IN  Modulator*    modulator,
	IN  u16          bandwidth,
	IN  u32         frequency
){

	/*
     *  ToDo:  Add code here
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
	u32 error = 0;

	if(frequency <= 300000){ // <=300000KHz v-filter gpio set to Lo
	  error = IT9507_writeRegister (modulator, Processor_LINK, GPIO_O(GPIOH8), 0);  /* uvFilter */
		if (error) goto exit;

	}else{
	  error = IT9507_writeRegister (modulator, Processor_LINK, GPIO_O(GPIOH8), 1); /* uvFilter */
		if (error) goto exit;
	}	
exit:
	return (error);
}

u32 EagleUser_setTxModeEnable (
	IN  Modulator*            modulator,
	IN  u8                    enable	
) {
	/*
     *  ToDo:  Add code here
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
	u32 error = ModulatorError_NO_ERROR;
	if(enable){
			error = IT9507_writeRegister (modulator, Processor_LINK, GPIO_O(GPIOH2), 1); //RF power up 
			if (error) goto exit;
	}else{
			error = IT9507_writeRegister (modulator, Processor_LINK, GPIO_O(GPIOH2), 0); //RF power down 
			if (error) goto exit;
	}
exit :
	return (error);
}
