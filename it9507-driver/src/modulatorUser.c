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

#define    GPIOH1_I	0xd8ae
#define    GPIOH1_O	0xd8af
#define    GPIOH1_EN	0xd8b0 
#define    GPIOH1_ON	0xd8b1
#define    GPIOH3_I	0xd8b2
#define    GPIOH3_O	0xd8b3
#define    GPIOH3_EN	0xd8b4
#define    GPIOH3_ON	0xd8b5
#define    GPIOH2_I	0xd8b6
#define    GPIOH2_O	0xd8b7
#define    GPIOH2_EN	0xd8b8
#define    GPIOH2_ON	0xd8b9
#define    GPIOH5_I	0xd8ba
#define    GPIOH5_O	0xd8bb
#define    GPIOH5_EN	0xd8bc
#define    GPIOH5_ON	0xd8bd
#define    GPIOH4_I	0xd8be
#define    GPIOH4_O	0xd8bf
#define    GPIOH4_EN	0xd8c0 
#define    GPIOH4_ON	0xd8c1 
#define    GPIOH7_I	0xd8c2
#define    GPIOH7_O	0xd8c3
#define    GPIOH7_EN	0xd8c4
#define    GPIOH7_ON	0xd8c5
#define    GPIOH6_I	0xd8c6
#define    GPIOH6_O	0xd8c7
#define    GPIOH6_EN	0xd8c8
#define    GPIOH6_ON	0xd8c9
#define    GPIOH8_I	0xd8ce
#define    GPIOH8_O	0xd8cf 
#define    GPIOH8_EN	0xd8d0 
#define    GPIOH8_ON	0xd8d1 

u32 EagleUser_setSystemConfig (
    IN  Modulator*    modulator
) {
	u32 error = 0;

        /* restSlave */	
	error = IT9507_writeRegister (modulator, Processor_LINK, GPIOH1_O, 1);//gpiox_en
	if (error) goto exit;
	error = IT9507_writeRegister (modulator, Processor_LINK, GPIOH1_EN, 1);//gpiox_en
	if (error) goto exit;
	error = IT9507_writeRegister (modulator, Processor_LINK, GPIOH1_ON, 1);//gpiox_on
	if (error) goto exit;
	EagleUser_delay(modulator, 10);

        /* powerDownSlave */
	error = IT9507_writeRegister (modulator, Processor_LINK, GPIOH5_O, 0);//gpiox_en
	if (error) goto exit;
	error = IT9507_writeRegister (modulator, Processor_LINK, GPIOH5_EN, 1);//gpiox_en
	if (error) goto exit;
	error = IT9507_writeRegister (modulator, Processor_LINK, GPIOH5_ON, 1);//gpiox_on
	if (error) goto exit;

        /* rfEnable */
	error = IT9507_writeRegister (modulator, Processor_LINK, GPIOH2_EN, 1);//gpiox_en
	if (error) goto exit;
	error = IT9507_writeRegister (modulator, Processor_LINK, GPIOH2_ON, 1);//gpiox_on
	if (error) goto exit;

        /* uvFilter */
	error = IT9507_writeRegister (modulator, Processor_LINK, GPIOH8_EN, 1);//gpiox_en
	if (error) goto exit;
	error = IT9507_writeRegister (modulator, Processor_LINK, GPIOH8_ON, 1);//gpiox_on
	if (error) goto exit;

exit:
    return (ModulatorError_NO_ERROR);
}

u32 EagleUser_getTsInputType (
	IN  Modulator*    modulator,
	OUT  TsInterface*  tsInStreamType
) {
	u32 error = ModulatorError_NO_ERROR;
	u8 temp = 0;
	*tsInStreamType = (TsInterface)temp;

	error = IT9507_readRegister (modulator, Processor_LINK, 0x4979, &temp);//has eeprom ??
	if((temp == 1) && (error == ModulatorError_NO_ERROR)){
		error = IT9507_readRegister (modulator, Processor_LINK, 0x49CA, &temp);
		if(error == ModulatorError_NO_ERROR){
			*tsInStreamType = (TsInterface)temp;
		}
	}
	return (error);
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

	if(modulator->tsInterfaceType != StreamType_DVBT_DATAGRAM){
		
		error = IT9507_writeRegister (modulator, Processor_LINK, GPIOH1_O, 1); //RX(IT9133) rest 
		if (error) goto exit;

		EagleUser_delay(modulator, 10);

		error = IT9507_writeRegister (modulator, Processor_LINK, GPIOH5_O, 0); //RX(IT9133) power up
		if (error) goto exit;


	}

	// RF Enable
	error = IT9507_writeRegister (modulator, Processor_LINK, GPIOH2_O, 0); //RF out power down
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
	  error = IT9507_writeRegister (modulator, Processor_LINK, GPIOH8_O, 0);  /* uvFilter */
		if (error) goto exit;

	}else{
	  error = IT9507_writeRegister (modulator, Processor_LINK, GPIOH8_O, 1); /* uvFilter */
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
			error = IT9507_writeRegister (modulator, Processor_LINK, GPIOH2_O, 1); //RF power up 
			if (error) goto exit;
	}else{
			error = IT9507_writeRegister (modulator, Processor_LINK, GPIOH2_O, 0); //RF power down 
			if (error) goto exit;
	}
exit :
	return (error);
}
