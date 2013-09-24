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
#include "it950x-priv.h"
#include "modulatorType.h"
#include "modulatorError.h"
#include "modulatorUser.h"
#include "modulatorFirmware.h"
#include "modulatorRegister.h"
#include "modulatorVariable.h"
#include "modulatorVersion.h"
#include "IQ_fixed_table.h"

static u8 IT9507Cmd_sequence = 0;

static u32 IT9507Cmd_addChecksum (
    IN  Modulator*    modulator,
    OUT u32*          bufferLength,
    OUT u8*           buffer
) {
    u32 error  = ModulatorError_NO_ERROR;
    u32 loop   = (*bufferLength - 1) / 2;
    u32 remain = (*bufferLength - 1) % 2;
    u32 i;
    u16  checksum = 0;

	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);

    for (i = 0; i < loop; i++)
        checksum = checksum + (u16) (buffer[2 * i + 1] << 8) + (u16) (buffer[2 * i + 2]);
    if (remain)
        checksum = checksum + (u16) (buffer[*bufferLength - 1] << 8);
    
    checksum = ~checksum;
    buffer[*bufferLength]     = (u8) ((checksum & 0xFF00) >> 8);
    buffer[*bufferLength + 1] = (u8) (checksum & 0x00FF);
    buffer[0]                 = (u8) (*bufferLength + 1);
    *bufferLength            += 2;

    return (error);
}


static u32 IT9507Cmd_removeChecksum (
    IN  Modulator*    modulator,
    OUT u32*          bufferLength,
    OUT u8*           buffer
) {
    u32 error    = ModulatorError_NO_ERROR;
    u32 loop     = (*bufferLength - 3) / 2;
    u32 remain   = (*bufferLength - 3) % 2;
    u32 i;
    u16  checksum = 0;
	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);

    for (i = 0; i < loop; i++)
        checksum = checksum + (u16) (buffer[2 * i + 1] << 8) + (u16) (buffer[2 * i + 2]);
    if (remain)
        checksum = checksum + (u16) (buffer[*bufferLength - 3] << 8);    
    
    checksum = ~checksum;
    if (((u16)(buffer[*bufferLength - 2] << 8) + (u16)(buffer[*bufferLength - 1])) != checksum) {
        error = ModulatorError_WRONG_CHECKSUM;
        goto exit;
    }
    if (buffer[2])
        error = ModulatorError_FIRMWARE_STATUS | buffer[2];
    
    buffer[0]      = (u8) (*bufferLength - 3);
    *bufferLength -= 2;

exit :
    return (error);
}


static u32 IT9507_writeRegisters (
    IN  Modulator*    modulator,
    IN  Processor     processor,
    IN  u32         registerAddress,
    IN  u8          writeBufferLength,
    IN  u8*         writeBuffer
) {
  	u32 error = ModulatorError_NO_ERROR;

	u8 registerAddressLength;
	u16        command;
    u8        buffer[255];
    u32       bufferLength;
    u32       remainLength;
    u32       sendLength;
    u32       i,cnt;
   
    u8       maxFrameSize = EagleUser_MAXFRAMESIZE;

	if (processor == Processor_LINK) {
		if (registerAddress > 0x000000FF) {
			registerAddressLength = 2;
		} else {
			registerAddressLength = 1;
		}
	} else {
			registerAddressLength = 2;
	}
	
	if (writeBufferLength == 0) goto exit;
    if (registerAddressLength > 4) {
        error  = ModulatorError_PROTOCOL_FORMAT_INVALID;
        goto exit;
    }

      
    if ((writeBufferLength + 12) > maxFrameSize) {
        error = ModulatorError_INVALID_DATA_LENGTH;
        goto exit;
    }



    /** add frame header */
    command   = IT9507Cmd_buildCommand (Command_REG_DEMOD_WRITE, processor);
    buffer[1] = (u8) (command >> 8);
    buffer[2] = (u8) command;
    buffer[3] = (u8) IT9507Cmd_sequence++;
    buffer[4] = (u8) writeBufferLength;
    buffer[5] = (u8) registerAddressLength;
    buffer[6] = (u8) ((registerAddress) >> 24); /** Get first byte of reg. address  */
    buffer[7] = (u8) ((registerAddress) >> 16); /** Get second byte of reg. address */
    buffer[8] = (u8) ((registerAddress) >> 8);  /** Get third byte of reg. address  */
    buffer[9] = (u8) (registerAddress );        /** Get fourth byte of reg. address */

    /** add frame data */
    for (i = 0; i < writeBufferLength; i++) {    
        buffer[10 + i] = writeBuffer[i];
    }

    /** add frame check-sum */
    bufferLength = 10 + writeBufferLength;
    error = IT9507Cmd_addChecksum (modulator, &bufferLength, buffer);
    if (error) goto exit;    

    /** send frame */
    i = 0;
    sendLength = 0;
    remainLength = bufferLength;
    while (remainLength > 0) {
        i     = (remainLength > EagleUser_MAX_PKT_SIZE) ? (EagleUser_MAX_PKT_SIZE) : (remainLength);
		for (cnt = 0; cnt < EagleUser_RETRY_MAX_LIMIT; cnt++) {
			error = EagleUser_busTx (modulator, i, &buffer[sendLength]);
			if (error == 0) break;
			EagleUser_delay (modulator, 1);
		}
        if (error) goto exit;

        sendLength   += i;
        remainLength -= i;
    }

    /** get reply frame */
    bufferLength = 5;
    
	for (cnt = 0; cnt < EagleUser_RETRY_MAX_LIMIT; cnt++) {
		error = EagleUser_busRx (modulator, bufferLength, buffer);
		if (error == 0) break;
		EagleUser_delay (modulator, 1);
	}
    if (error) goto exit;

    /** remove check-sum from reply frame */
    error = IT9507Cmd_removeChecksum (modulator, &bufferLength, buffer);
    if (error) goto exit;

exit :
   
	
return (error);
}

static u32 IT9507_writeRegister (
    IN  Modulator*    modulator,
    IN  Processor       processor,
    IN  u32           registerAddress,
    IN  u8            value
) {
   	return (IT9507_writeRegisters(modulator, processor, registerAddress, 1, &value));
}

static u32 IT9507_readRegisters (
    IN  Modulator*    modulator,
    IN  Processor       processor,
    IN  u32           registerAddress,
    IN  u8            readBufferLength,
    OUT u8*           readBuffer
) {
    u32 error = ModulatorError_NO_ERROR;
	
	u8 registerAddressLength;
	u16        command;
    u8        buffer[255];
    u32       bufferLength;
    u32       sendLength;
    u32       remainLength;
    u32       i, k, cnt;
    
    u8       maxFrameSize = EagleUser_MAXFRAMESIZE;
		
	if (processor == Processor_LINK) {
		if (registerAddress > 0x000000FF) {
			registerAddressLength = 2;
		} else {
			registerAddressLength = 1;
		}
	} else {
		registerAddressLength = 2;
	}

    if (readBufferLength == 0) goto exit;
    if (registerAddressLength > 4) {
        error  = ModulatorError_PROTOCOL_FORMAT_INVALID;
        goto exit;
    }

    if ((readBufferLength + 5) > EagleUser_MAX_PKT_SIZE) {
        error = ModulatorError_INVALID_DATA_LENGTH;
        goto exit;
    }

    if ((readBufferLength + 5) > maxFrameSize) {
        error = ModulatorError_INVALID_DATA_LENGTH;
        goto exit;
    }



    /** add frame header */
    command   = IT9507Cmd_buildCommand (Command_REG_DEMOD_READ, processor);
    buffer[1] = (u8) (command >> 8);
    buffer[2] = (u8) command;
    buffer[3] = (u8) IT9507Cmd_sequence++;
    buffer[4] = (u8) readBufferLength;
    buffer[5] = (u8) registerAddressLength;
    buffer[6] = (u8) (registerAddress >> 24); /** Get first byte of reg. address  */
    buffer[7] = (u8) (registerAddress >> 16); /** Get second byte of reg. address */
    buffer[8] = (u8) (registerAddress >> 8);  /** Get third byte of reg. address  */
    buffer[9] = (u8) (registerAddress);       /** Get fourth byte of reg. address */

    /** add frame check-sum */
    bufferLength = 10;
    error = IT9507Cmd_addChecksum (modulator, &bufferLength, buffer);
    if (error) goto exit;


    /** send frame */
    i = 0;
    sendLength   = 0;
    remainLength = bufferLength;
    while (remainLength > 0) {
        i     = (remainLength > EagleUser_MAX_PKT_SIZE) ? (EagleUser_MAX_PKT_SIZE) : (remainLength);        
      	for (cnt = 0; cnt < EagleUser_RETRY_MAX_LIMIT; cnt++) {
			error = EagleUser_busTx (modulator, i, &buffer[sendLength]);
			if (error == 0) break;
			EagleUser_delay (modulator, 1);
		}
        if (error) goto exit;

        sendLength   += i;
        remainLength -= i;
    }

    /** get reply frame */
    bufferLength = 5 + readBufferLength;

	for (cnt = 0; cnt < EagleUser_RETRY_MAX_LIMIT; cnt++) {
		error = EagleUser_busRx (modulator, bufferLength, buffer);
		if (error == 0) break;
		EagleUser_delay (modulator, 1);
	}
    if (error) goto exit;

    /** remove check-sum from reply frame */
    error = IT9507Cmd_removeChecksum (modulator, &bufferLength, buffer);
    if (error) goto exit;

    for (k = 0; k < readBufferLength; k++) {
        readBuffer[k] = buffer[k + 3];
    }
	
exit:
	return (error);
}

static u32 IT9507_readRegister (
    IN  Modulator*    modulator,
    IN  Processor       processor,
    IN  u32           registerAddress,
    OUT u8*           value
) {
    return (IT9507_readRegisters (modulator, processor, registerAddress, 1, value));
}


static u32 IT9507_writeRegisterBits (
    IN  Modulator*    modulator,
    IN  Processor       processor,
    IN  u32           registerAddress,
    IN  u8            position,
    IN  u8            length,
    IN  u8            value
)
{
    u32 error = ModulatorError_NO_ERROR;

	u8 temp;

	if (length == 8) {
		error = IT9507_writeRegisters (modulator, processor, registerAddress, 1, &value);
		
	} else {
		error = IT9507_readRegisters (modulator, processor, registerAddress, 1, &temp);
		if (error) goto exit;
		

		temp = (u8)REG_CREATE (value, temp, position, length);

		error = IT9507_writeRegisters (modulator, processor, registerAddress, 1, &temp);
		if (error) goto exit;
		
	}
exit:

	return (error);
}


u32 IT9507Cmd_sendCommand (
    IN  Modulator*    modulator,
    IN  u16            command,
    IN  Processor       processor,
    IN  u32           writeBufferLength,
    IN  u8*           writeBuffer,
    IN  u32           readBufferLength,
    OUT u8*           readBuffer
) {
    u32       error = ModulatorError_NO_ERROR;
    u8        buffer[255];
    u32       bufferLength;
    u32       remainLength;
    u32       sendLength;
    u32       i, k, cnt;
    
    u32       maxFrameSize = EagleUser_MAXFRAMESIZE;
    
    if ((writeBufferLength + 6) > maxFrameSize) {
        error = ModulatorError_INVALID_DATA_LENGTH;
        goto exit;
    }

    if ((readBufferLength + 5) > EagleUser_MAX_PKT_SIZE) {
        error  = ModulatorError_INVALID_DATA_LENGTH;
        goto exit;
    }

    if ((readBufferLength + 5) > maxFrameSize) {
        error = ModulatorError_INVALID_DATA_LENGTH;
        goto exit;
    }


    if (writeBufferLength == 0) {
        command   = IT9507Cmd_buildCommand (command, processor);
        buffer[1] = (u8) (command >> 8);
        buffer[2] = (u8) command;
        buffer[3] = (u8) IT9507Cmd_sequence++;
        bufferLength = 4;
        error = IT9507Cmd_addChecksum (modulator, &bufferLength, buffer);
        if (error) goto exit;

        // send command packet
        i = 0;
        sendLength = 0;
        remainLength = bufferLength;
        while (remainLength > 0) {
            i = (remainLength > EagleUser_MAX_PKT_SIZE) ? (EagleUser_MAX_PKT_SIZE) : (remainLength);        

			for (cnt = 0; cnt < EagleUser_RETRY_MAX_LIMIT; cnt++) {
				error = EagleUser_busTx (modulator, i, &buffer[sendLength]);
				if (error == 0) break;
				EagleUser_delay (modulator, 1);
			}
            if (error) goto exit;

            sendLength   += i;
            remainLength -= i;
        }
    } else {
        command   = IT9507Cmd_buildCommand (command, processor);
        buffer[1] = (u8) (command >> 8);
        buffer[2] = (u8) command;
        buffer[3] = (u8) IT9507Cmd_sequence++;
        for (k = 0; k < writeBufferLength; k++)
            buffer[k + 4] = writeBuffer[k];
        
        
        bufferLength = 4 + writeBufferLength;
        error = IT9507Cmd_addChecksum (modulator, &bufferLength, buffer);
        if (error) goto exit;

        
        /** send command */
        i = 0;
        sendLength = 0;
        remainLength = bufferLength;
        while (remainLength > 0) {
            i     = (remainLength > EagleUser_MAX_PKT_SIZE) ? (EagleUser_MAX_PKT_SIZE) : (remainLength);        

			for (cnt = 0; cnt < EagleUser_RETRY_MAX_LIMIT; cnt++) {
				error = EagleUser_busTx (modulator, i, &buffer[sendLength]);
				if (error == 0) break;
				EagleUser_delay (modulator, 1);
			}
            if (error) goto exit;

            sendLength   += i;
            remainLength -= i;
        }
    }

    bufferLength = 5 + readBufferLength;

	for (cnt = 0; cnt < EagleUser_RETRY_MAX_LIMIT; cnt++) {
		error = EagleUser_busRx (modulator, bufferLength, buffer);
		if (error == 0) break;
		EagleUser_delay (modulator, 1);
	}
    if (error) goto exit;

    error = IT9507Cmd_removeChecksum (modulator, &bufferLength, buffer);
    if (error) goto exit;

    if (readBufferLength) {
        for (k = 0; k < readBufferLength; k++) {
            readBuffer[k] = buffer[k + 3];
        }
    }

exit :
    
    return (error);
}

static u32 IT9507_writeGenericRegisters (
    IN  Modulator*    modulator,
    IN  u8            slaveAddress,
    IN  u8            bufferLength,
    IN  u8*           buffer
) {
    u8 writeBuffer[256];
	u8 i;
	
	writeBuffer[0] = bufferLength;
	writeBuffer[1] = 2;
	writeBuffer[2] = slaveAddress;

	for (i = 0; i < bufferLength; i++) {
		writeBuffer[3 + i] = buffer[i];
	}
	return (IT9507Cmd_sendCommand (modulator, Command_GENERIC_WRITE, Processor_LINK, bufferLength + 3, writeBuffer, 0, NULL));
}


static u32 IT9507_readGenericRegisters (
    IN  Modulator*    modulator,
    IN  u8            slaveAddress,
    IN  u8            bufferLength,
    IN  u8*           buffer
) {
    u8 writeBuffer[3];

	writeBuffer[0] = bufferLength;
	writeBuffer[1] = 2;
	writeBuffer[2] = slaveAddress;

	return (IT9507Cmd_sendCommand (modulator, Command_GENERIC_READ, Processor_LINK, 3, writeBuffer, bufferLength, buffer));
}




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

//u8 CmdSequence_EagleSlaveIIC = 0;

/* Code originally in eagleTuner.c */
//#define PI 3.141593
#define BYTESHIFT 512

static int setIO(const int THETA){
   int reverse;
   int sign;

   int x_in;
   int x_in_reverse;
      
   int mi;
   int xi;
   int yi;
      
   int ph2amp;
//   int ph2amp_scaling;

   if ( ( 0 <= THETA ) && ( THETA < 4096 ) ) // 0 ~ 4096
   {
      reverse = 0;
      sign    = 1;
   }
   else if ( ( 4096 <= THETA ) && ( THETA < 8192 ) ) // 4096 ~ 8192
   {
      reverse = 1;
      sign    = 1;
   }
   else if ( ( 8192 <= THETA ) && ( THETA < 12288 ) ) // 8192 ~ 12288
   {
      reverse = 0;
      sign    = -1;
   }
   else
   {
      reverse = 1;
      sign    = -1;
   }


   //x_in = ( double ) ( ( int ) THETA % 4096 );
   x_in =( ( int ) THETA % 4096 );
   if ( reverse == 0 )
      x_in_reverse = x_in;
   else
      x_in_reverse = 4096 - x_in;
   
   if ( ( 0 <= x_in_reverse ) && ( x_in_reverse < 256 ) ) // section 1 : 0 ~ 256
   {
      mi = 25*16;
      xi = 256 *0;
      yi = 0*16;
   }
   else if ( ( 256 <= x_in_reverse ) && ( x_in_reverse < 512 ) ) // section 2 : 256 ~ 512
   {
      mi = 25*16;
      xi = 256 * 1;
      yi = 400*16;
   }
   else if ( ( 512 <= x_in_reverse ) && ( x_in_reverse < 768 ) ) // section 3 : 512 ~ 768
   {
      mi = 24*16;
      xi = 256 * 2;
      yi = 801*16;
   }
   else if ( ( 768 <= x_in_reverse ) && ( x_in_reverse < 1024 ) ) // section 4 : 768 ~ 1024
   {
      mi = 24*16;
      xi = 256 * 3;
      yi = 1186*16;
   }
   else if ( ( 1024 <= x_in_reverse ) && ( x_in_reverse < 1280 ) ) // section 5 : 1024 ~ 1280
   {
      mi = 23*16;
      xi = 256 * 4;
      yi = 1564*16;
   }
   else if ( ( 1280 <= x_in_reverse ) && ( x_in_reverse < 1536 ) ) // section 6 : 1280 ~ 1536
   {
      mi = 22*16;
      xi = 256 * 5;
      yi = 1927*16;
   }
   else if ( ( 1536 <= x_in_reverse ) && ( x_in_reverse < 1792 ) ) // section 7 : 1536 ~ 1792
   {
      mi = 20*16;
      xi = 256 * 6;
      yi = 2277*16;
   }
   else if ( ( 1792 <= x_in_reverse ) && ( x_in_reverse < 2048 ) ) // section 8 : 1792 ~ 2048
   {
      mi = 19*16;
      xi = 256 * 7;
      yi = 2595*16;
   }
   else if ( ( 2048 <= x_in_reverse ) && ( x_in_reverse < 2304 ) ) // section 9 : 2048 ~ 2304
   {
      mi = 17*16;
      xi = 256 * 8;
      yi = 2894*16;
   }
   else if ( ( 2304 <= x_in_reverse ) && ( x_in_reverse < 2560 ) ) // section 10 : 2304 ~ 2560
   {
      mi = 15*16;
      xi = 256 * 9;
      yi = 3165*16;
   }
   else if ( ( 2560 <= x_in_reverse ) && ( x_in_reverse < 2816 ) ) // section 11 : 2560 ~ 2816
   {
      mi = 13*16;
      xi = 256 * 10;
      yi = 3405*16;
   }
   else if ( ( 2816 <= x_in_reverse ) && ( x_in_reverse < 3072 ) ) // section 12 : 2816 ~ 3072
   {
      mi = 11*16;
      xi = 256 * 11;
      yi = 3609*16;
   }
   else if ( ( 3072 <= x_in_reverse ) && ( x_in_reverse < 3328 ) ) // section 13 : 3072 ~ 3328
   {
      mi = 8*16;
      xi = 256 * 12;
      yi = 3788*16;
   }
   else if ( ( 3328 <= x_in_reverse ) && ( x_in_reverse < 3584 ) ) // section 14 : 3328 ~ 3584
   {
      mi = 6*16;
      xi = 256 * 13;
      yi = 3920*16;
   }    
   else if ( ( 3584 <= x_in_reverse ) && ( x_in_reverse < 3840 ) ) // section 15 : 3584 ~ 3840
   {
      mi = 4*16;
      xi = 256 * 14;
      yi = 4015*16;
   }
   else // section 16 : 3840 ~ 4096
   {
      mi = 1*16;
      xi = 256 * 15;
      yi = 4078*16;
   }

   ph2amp = sign * ( mi * (x_in_reverse - xi)/16 + yi );


   return ph2amp;
}


static u32 interpolation(
	Modulator*    modulator,
    int fIn, 
    int *ptrdAmp, 
	int *ptrdPhi
	)
{
  // Using binary search to find the frequency interval in the table
	u16 TABLE_NROW = modulator->calibrationInfo.tableGroups;
	u32   error = ModulatorError_NO_ERROR;
    int idx = TABLE_NROW/2;
    int preIdx = -1;
    int lower = 0;
    int upper = TABLE_NROW;
	int outdAmp;
    int outdPhi;
	int diff;


	int temp1,temp2,temp3,temp4;

    while( ! ( (fIn - (int)(modulator->calibrationInfo.ptrIQtableEx[idx].frequency)) >= 0 && (fIn - (int)(modulator->calibrationInfo.ptrIQtableEx[idx+1].frequency) < 0) ) ){
	  if((fIn - (int)modulator->calibrationInfo.ptrIQtableEx[idx].frequency)==0)
		  break;
      preIdx = idx;
      if(fIn - (int)(modulator->calibrationInfo.ptrIQtableEx[idx+1].frequency) >= 0){
        idx = (preIdx + upper)/2;
        lower = preIdx;
      }else if(fIn - (int)(modulator->calibrationInfo.ptrIQtableEx[idx].frequency) < 0){
        idx = (preIdx + lower)/2;
        upper = preIdx;
      }

	  if(lower == TABLE_NROW - 1)
		  break;
    }

	if((fIn != (int)modulator->calibrationInfo.ptrIQtableEx[idx].frequency) && 
		((modulator->calibrationInfo.ptrIQtableEx[idx+1].frequency - modulator->calibrationInfo.ptrIQtableEx[idx].frequency)>100000)){
		// out of Calibration range
			error = ModulatorError_OUT_OF_CALIBRATION_RANGE;
			goto exit;
	}

    //printf("Found: %f \t %f\n", IQ_table[idx][0], IQ_table[idx+1][0]);
  // Perform linear interpolation

//--------test
	temp1 = modulator->calibrationInfo.ptrIQtableEx[idx].dAmp;
	temp2 = (fIn - (int)modulator->calibrationInfo.ptrIQtableEx[idx].frequency);
	temp3 = (modulator->calibrationInfo.ptrIQtableEx[idx+1].dAmp - modulator->calibrationInfo.ptrIQtableEx[idx].dAmp);
	temp4 = (modulator->calibrationInfo.ptrIQtableEx[idx+1].frequency - modulator->calibrationInfo.ptrIQtableEx[idx].frequency);
	
	outdAmp = temp1 + temp2*temp3/temp4;
	
	//test	

	//outdAmp = modulator->calibrationInfo.ptrIQtableEx[idx].dAmp + ((fIn - (int)modulator->calibrationInfo.ptrIQtableEx[idx].frequency) 
	//	       * (modulator->calibrationInfo.ptrIQtableEx[idx+1].dAmp - modulator->calibrationInfo.ptrIQtableEx[idx].dAmp)) / (modulator->calibrationInfo.ptrIQtableEx[idx+1].frequency - modulator->calibrationInfo.ptrIQtableEx[idx].frequency);
	diff = (modulator->calibrationInfo.ptrIQtableEx[idx+1].dPhi - modulator->calibrationInfo.ptrIQtableEx[idx].dPhi);
	if(diff <= -8192) {
		diff = diff+16384;
	} else if(diff >= 8192) {
		diff = diff-16384;
	}
	//outdPhi = IQ_table[idx][2] + (fIn - IQ_table[idx][0]) * (IQ_table[idx+1][2] - IQ_table[idx][2]) / (IQ_table[idx+1][0] - IQ_table[idx][0]);
			
	//------test---------
	temp1 = modulator->calibrationInfo.ptrIQtableEx[idx].dPhi;
	temp2 = (fIn - (int)modulator->calibrationInfo.ptrIQtableEx[idx].frequency);
	temp3 = diff;
	temp4 = (modulator->calibrationInfo.ptrIQtableEx[idx+1].frequency - modulator->calibrationInfo.ptrIQtableEx[idx].frequency);
	outdPhi = temp1 + temp2*temp3/temp4;
	//----------------

	
	//outdPhi = modulator->calibrationInfo.ptrIQtableEx[idx].dPhi + ((fIn - (int)modulator->calibrationInfo.ptrIQtableEx[idx].frequency) * diff) / (modulator->calibrationInfo.ptrIQtableEx[idx+1].frequency - modulator->calibrationInfo.ptrIQtableEx[idx].frequency);
	
		
	if (outdPhi>=16384) {
		outdPhi = outdPhi%16384;
    }
	while (outdPhi<0) {
		outdPhi = outdPhi+16384;
	}
    (*ptrdAmp) = outdAmp;
    (*ptrdPhi) = outdPhi;
exit:
	return error;
}

static u32 EagleTuner_setIQCalibration(
	IN  Modulator*    modulator,
    IN  u32         frequency	
) {
	u32   error = ModulatorError_NO_ERROR;
	u32 reg = 0;
	u8 c1_tmp_highbyte;
	u8 c1_tmp_lowbyte;
	u8 c2_tmp_highbyte;
	u8 c2_tmp_lowbyte;
	u8 c3_tmp_highbyte;
	u8 c3_tmp_lowbyte;
	u8 val[6];
	int dAmp = 0;
    int dPhi = 0;
    int* ptrdAmp = &dAmp;
    int* ptrdPhi = &dPhi;
	int alpha;
	int beta;
	int dphi;
	int c1;
	int c2;
	int c3;
	int test;
	int test2;

    error = interpolation(modulator, frequency, ptrdAmp, ptrdPhi);
	if(error == ModulatorError_OUT_OF_CALIBRATION_RANGE){
		//out of range --> set to default
		val[0] = 0x00;
		val[1] = 0x02;
		val[2] = 0x00;
		val[3] = 0x00;
		val[4] = 0x00;
		val[5] = 0x02;
		error = ModulatorError_NO_ERROR;
		goto exit;

	}

	alpha = 512 - dAmp;
	beta = 512 + dAmp;
	dphi = dPhi;

	
   test = setIO(dPhi); 
 
   dPhi = dPhi+4096;
   if (dPhi>=16384) {
	dPhi = dPhi%16384;
   }

   test2 = setIO(dPhi); 

   c1 = (int) (alpha*beta/16*test2/4096/BYTESHIFT);
   c2 = (int) (-1*alpha*beta/16*test/4096/BYTESHIFT);
   c3 = (int) (alpha * alpha/BYTESHIFT);

	if (c1 < 0) {
    c1 = c1+2048;
	}
	if (c2 < 0) {
    c2 = c2+2048;
	}
	if (c3 < 0) {
    c3 = c3+2048;
	}
  
  c1_tmp_highbyte = (unsigned char) (c1>>8);
  c1_tmp_lowbyte = (unsigned char) (c1- c1_tmp_highbyte*256);
  c2_tmp_highbyte = (unsigned char) (c2>>8);
  c2_tmp_lowbyte = (unsigned char) (c2- c2_tmp_highbyte*256);
  c3_tmp_highbyte = (unsigned char) (c3>> 8);
  c3_tmp_lowbyte = (unsigned char) (c3- c3_tmp_highbyte*256);


  reg = 0xF752; //p_eagle_reg_iqik_c1_7_0
  val[0] = (u8) c1_tmp_lowbyte;
  val[1] = (u8) c1_tmp_highbyte;
  val[2] = (u8) c2_tmp_lowbyte;
  val[3] = (u8) c2_tmp_highbyte;
  val[4] = (u8) c3_tmp_lowbyte;
  val[5] = (u8) c3_tmp_highbyte;
exit:
  error = IT9507_writeRegisters(modulator, Processor_OFDM, reg, 6, val);

  return (error);
}


static u32 EagleTuner_calIQCalibrationValue(
	IN  Modulator*    modulator,
    IN  u32         frequency,
	IN  u8*		  val
) {
	u32   error = ModulatorError_NO_ERROR;
	
	u8 c1_tmp_highbyte;
	u8 c1_tmp_lowbyte;
	u8 c2_tmp_highbyte;
	u8 c2_tmp_lowbyte;
	u8 c3_tmp_highbyte;
	u8 c3_tmp_lowbyte;
	int dAmp = 0;
    int dPhi = 0;
    int* ptrdAmp = &dAmp;
    int* ptrdPhi = &dPhi;
	int alpha;
	int beta;
	int dphi;
	int c1;
	int c2;
	int c3;
	int test;
	int test2;

    error = interpolation(modulator, frequency, ptrdAmp, ptrdPhi);
	if(error == ModulatorError_OUT_OF_CALIBRATION_RANGE){
		//out of range --> set to default
		val[0] = 0x00;
		val[1] = 0x02;
		val[2] = 0x00;
		val[3] = 0x00;
		val[4] = 0x00;
		val[5] = 0x02;
		error = ModulatorError_NO_ERROR;
		goto exit;

	}

	alpha = 512 - dAmp;
	beta = 512 + dAmp;
	dphi = dPhi;

	
   test = setIO(dPhi); 
 
   dPhi = dPhi+4096;
   if (dPhi>=16384) {
	dPhi = dPhi%16384;
   }
   test2 = setIO(dPhi); 

   c1 = (int) (alpha*beta/16*test2/4096/BYTESHIFT);
   c2 = (int) (-1*alpha*beta/16*test/4096/BYTESHIFT);
   c3 = (int) (alpha * alpha/BYTESHIFT);

	if (c1 < 0) {
    c1 = c1+2048;
	}
	if (c2 < 0) {
    c2 = c2+2048;
	}
	if (c3 < 0) {
    c3 = c3+2048;
	}
  
  c1_tmp_highbyte = (unsigned char) (c1>>8);
  c1_tmp_lowbyte = (unsigned char) (c1- c1_tmp_highbyte*256);
  c2_tmp_highbyte = (unsigned char) (c2>>8);
  c2_tmp_lowbyte = (unsigned char) (c2- c2_tmp_highbyte*256);
  c3_tmp_highbyte = (unsigned char) (c3>> 8);
  c3_tmp_lowbyte = (unsigned char) (c3- c3_tmp_highbyte*256);


  
  val[0] = (u8) c1_tmp_lowbyte;
  val[1] = (u8) c1_tmp_highbyte;
  val[2] = (u8) c2_tmp_lowbyte;
  val[3] = (u8) c2_tmp_highbyte;
  val[4] = (u8) c3_tmp_lowbyte;
  val[5] = (u8) c3_tmp_highbyte;
  //error = Eagle_writeRegisters(modulator, Processor_OFDM, reg, 6, val);
exit:
  return (error);
}

/* End of code originally in eagleTuner.c */

const u8 Eagle_bitMask[Eagle_MAX_BIT] = {
	0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF
};

/** local functions */
static unsigned int c_fN_min[9] = {
	53000, 74000, 111000, 148000, 222000, 296000, 445000, 573000, 950000
};

static u32 IT9507_setTsInterface (IN  Modulator*    modulator);

unsigned int IT9507_getLoFreq(unsigned int rf_freq_kHz)
{
	unsigned int nc, nv, mv, lo_freq;
	
	//unsigned int freq_code;
	unsigned long tmp_tg, tmp_cal, tmp_m;

	unsigned int m_bdry;
	unsigned long tmp_numer;
	unsigned int g_fxtal_kHz = 2000;
	unsigned int g_fdiv =3;
	
	//m_bdry = 3480;
	m_bdry = 3660;
	tmp_numer = (unsigned long)g_fxtal_kHz * (unsigned long)m_bdry;
	
	c_fN_min[7] = (unsigned int) (tmp_numer / ((unsigned long)g_fdiv* 4));	
	c_fN_min[6] = (unsigned int) (tmp_numer / ((unsigned long)g_fdiv* 6));
	c_fN_min[5] = (unsigned int) (tmp_numer / ((unsigned long)g_fdiv* 8));
	c_fN_min[4] = (unsigned int) (tmp_numer / ((unsigned long)g_fdiv* 12));
	c_fN_min[3] = (unsigned int) (tmp_numer / ((unsigned long)g_fdiv* 16));
	c_fN_min[2] = (unsigned int) (tmp_numer / ((unsigned long)g_fdiv* 24));
	c_fN_min[1] = (unsigned int) (tmp_numer / ((unsigned long)g_fdiv* 32));



	//*nc = IT9507_get_nc(rf_freq_kHz);
	if ((rf_freq_kHz <= c_fN_min[1]))										{nc = 0;}	/*74*/
	else if ((rf_freq_kHz > c_fN_min[1]) && (rf_freq_kHz <= c_fN_min[2]))	{nc = 1;}	/*74 111*/	
	else if ((rf_freq_kHz > c_fN_min[2]) && (rf_freq_kHz <= c_fN_min[3]))	{nc = 2;}	/*111 148*/
	else if ((rf_freq_kHz > c_fN_min[3]) && (rf_freq_kHz <= c_fN_min[4]))	{nc = 3;}	/*148 222*/	
	else if ((rf_freq_kHz > c_fN_min[4]) && (rf_freq_kHz <= c_fN_min[5]))	{nc = 4;}	/*222 296*/	
	else if ((rf_freq_kHz > c_fN_min[5]) && (rf_freq_kHz <= c_fN_min[6]))	{nc = 5;}	/*296 445*/
	else if ((rf_freq_kHz > c_fN_min[6]) && (rf_freq_kHz <= c_fN_min[7]))	{nc = 6;}	/*445 573*/
	else if ((rf_freq_kHz > c_fN_min[7]) && (rf_freq_kHz <= c_fN_min[8]))	{nc = 7;}	/*573 890*/
	else 																	{nc = 8;}	/*L-band*/
	
	//*nv = IT9507_get_nv(*nc);

	switch(nc) {
		case 0:	nv = 48;	break;
		case 1:	nv = 32;	break;
		case 2:	nv = 24;	break;
		case 3:	nv = 16;	break;
		case 4:	nv = 12;	break;
		case 5:	nv = 8;	break;
		case 6:	nv = 6;	break;
		case 7:	nv = 4;	break;
		case 8: nv = 2; break;	/*L-band*/
		default:	nv = 2;	break;
	}



	if((nc)==8)
		nc = 0;
	tmp_tg = (unsigned long)rf_freq_kHz * (unsigned long)(nv) * (unsigned long)g_fdiv;
	tmp_m = (tmp_tg / (unsigned long)g_fxtal_kHz);
	tmp_cal = tmp_m * (unsigned long)g_fxtal_kHz;
	if ((tmp_tg-tmp_cal) >= (g_fxtal_kHz>>1)) {tmp_m = tmp_m+1;}
	mv = (unsigned int) (tmp_m);

	lo_freq = (((nc)&0x07) << 13) + (mv);
	
	return lo_freq;
}


u32 IT9507Cmd_reboot (
    IN  Modulator*    modulator
) {
    u32       error = ModulatorError_NO_ERROR;
    u16        command;
    u8        buffer[255];
    u32       bufferLength,cnt;
       
    command   = IT9507Cmd_buildCommand (Command_REBOOT, Processor_LINK);
    buffer[1] = (u8) (command >> 8);
    buffer[2] = (u8) command;
    buffer[3] = (u8) IT9507Cmd_sequence++;
    bufferLength = 4;
    error = IT9507Cmd_addChecksum (modulator, &bufferLength, buffer);
    if (error) goto exit;
   
	for (cnt = 0; cnt < EagleUser_RETRY_MAX_LIMIT; cnt++) {
		error = EagleUser_busTx (modulator, bufferLength, buffer);
		if (error == 0) break;
		EagleUser_delay (modulator, 1);
	}
    if (error) goto exit;

exit :
    
    return (error);
}

u32 IT9507_calOutputGain (
	IN  Modulator*    modulator,
	IN  u8		  *defaultValue,
	IN  int			  *gain	   
) {
	u32 error = ModulatorError_NO_ERROR;
	int amp_mul;
	int c1value = 0;
	int c2value = 0;
	int c3value = 0;	
	int c1value_default;
	int c2value_default;
	int c3value_default;	
	
	u32 amp_mul_max1 = 0;
	u32 amp_mul_max2 = 0;
	u32 amp_mul_max3 = 0;
	int amp_mul_max = 0;
	int i = 0;
	
	int gain_X10 = *gain * 10;
	
	bool overflow = false;
	
	if(modulator == NULL){
		error = ModulatorError_NULL_HANDLE_PTR;
		goto exit;
	}

	c1value_default = defaultValue[1]<<8 | defaultValue[0];
	c2value_default = defaultValue[3]<<8 | defaultValue[2];
	c3value_default = defaultValue[5]<<8 | defaultValue[4];	
	
	if (c1value_default>1023) c1value_default = c1value_default-2048;
	if (c2value_default>1023) c2value_default = c2value_default-2048;
	if (c3value_default>1023) c3value_default = c3value_default-2048;

	amp_mul_max1 = 10000*1023/abs(c1value_default);
	if(c2value_default != 0)
		amp_mul_max2 = 10000*1023/abs(c2value_default);
	else
		amp_mul_max2 = 0xFFFFFFFF;
    amp_mul_max3 = 10000*1023/abs(c3value_default);


	if (amp_mul_max1<amp_mul_max3) {
		if (amp_mul_max1<amp_mul_max2) {
				amp_mul_max = (int)amp_mul_max1;
			} else {
				amp_mul_max = (int)amp_mul_max2;
			}
	  } else if (amp_mul_max3<amp_mul_max2) {
        	amp_mul_max =(int)amp_mul_max3;
   	  } else {
   	  	amp_mul_max =(int)amp_mul_max2;
   	  	}

	if(gain_X10>0){
		//d_amp_mul = 1;
		amp_mul = 10000;
		for(i = 0 ; i<gain_X10 ; i+=10){
			if (amp_mul_max>amp_mul) {
				amp_mul = (amp_mul * 11220)/10000;
				c1value = (c1value_default * amp_mul)/10000;
				c2value = (c2value_default* amp_mul)/10000;
				c3value = (c3value_default * amp_mul)/10000;
			}
			if(c1value>0x03ff){
				c1value=0x03ff;
				overflow = true;				
			}
			
			if(c3value>0x03ff){
				c3value=0x03ff;
				overflow = true;				
			}

			if(overflow)
				break;
		}
			
		
	}else if(gain_X10<0){
		//d_amp_mul = 1;
		amp_mul = 10000;
		for(i = 0 ; i>gain_X10 ; i-=10){
			if (amp_mul_max>amp_mul) {
				//d_amp_mul *= 0.501;
				amp_mul = (amp_mul * 8910)/10000;
				
				c1value = (c1value_default * amp_mul)/10000;
				c2value = (c2value_default * amp_mul)/10000;
				c3value = (c3value_default * amp_mul)/10000;
			}
			if(c1value==0){
				overflow = true;
			}
			
			if(c3value==0){
				overflow = true;
			}

			if(overflow)
				break;			
		}
		
	}else{
		c1value = c1value_default;
		c2value = c2value_default;
		c3value = c1value_default;

	}
	if (c1value<0) {c1value=c1value+2048;}
	if (c2value<0) {c2value=c2value+2048;}
	if (c3value<0) {c3value=c3value+2048;}
	c1value = (c1value%2048);
	c2value = (c2value%2048);
	c3value = (c3value%2048);
	*gain = i/10;
exit:	
	return (error);
}


u32 IT9507_selectBandwidth (
	IN  Modulator*    modulator,
	IN  u16          bandwidth          /** KHz              */
) {
	u32 error ;
	u8 temp1 ;
	u8 temp2 ;
	u8 temp3 ;
	u8 temp4 ;
	u8 temp5 ;

	error = ModulatorError_NO_ERROR;
	temp1 = 0;
	temp2 = 0;
	temp3 = 0;
	temp4 = 0;
	temp5 = 0;
	
	switch (bandwidth) {

		case 1000:              /** 1M */
			temp1 = 0x5E;	//0xFBB6
			temp2 = 0x03;	//0xFBB7
			temp3 = 0x01;	//0xFBB8
			temp4 = 0x03;	//0xD814
			temp5 = 0x00;	//0xF741
			error = IT9507_writeRegister (modulator, Processor_LINK, 0xD812, 3);
			if (error) goto exit;
			break;

       	case 1500:              /** 1.5M */
			temp1 = 0x6E;	//0xFBB6
			temp2 = 0x03;	//0xFBB7
			temp3 = 0x01;	//0xFBB8
			temp4 = 0x03;	//0xD814
			temp5 = 0x00;	//0xF741
			error = IT9507_writeRegister (modulator, Processor_LINK, 0xD812, 3);
			if (error) goto exit;
			break; 

		case 2000:              /** 2M */
			temp1 = 0x5E;	//0xFBB6
			temp2 = 0x01;	//0xFBB7
			temp3 = 0x01;	//0xFBB8
			temp4 = 0x03;	//0xD814
			temp5 = 0x00;	//0xF741
			error = IT9507_writeRegister (modulator, Processor_LINK, 0xD812, 3);
			if (error) goto exit;			
			break;

		case 2500:              /** 2M */
			temp1 = 0x66;	//0xFBB6
			temp2 = 0x01;	//0xFBB7
			temp3 = 0x01;	//0xFBB8
			temp4 = 0x03;	//0xD814
			temp5 = 0x00;	//0xF741
			error = IT9507_writeRegister (modulator, Processor_LINK, 0xD812, 3);
			if (error) goto exit;			
			break;

		case 3000:              /** 3M */
			temp1 = 0x6E;	//0xFBB6
			temp2 = 0x01;	//0xFBB7
			temp3 = 0x01;	//0xFBB8
			temp4 = 0x03;	//0xD814
			temp5 = 0x00;	//0xF741
			error = IT9507_writeRegister (modulator, Processor_LINK, 0xD812, 3);
			if (error) goto exit;			
			break;

		case 4000:              /** 4M */
			temp1 = 0x5E;	//0xFBB6
			temp2 = 0x01;	//0xFBB7
			temp3 = 0x01;	//0xFBB8
			temp4 = 0x03;	//0xD814
			temp5 = 0x01;	//0xF741
			error = IT9507_writeRegister (modulator, Processor_LINK, 0xD812, 3);
			if (error) goto exit;			
			break;

		case 5000:              /** 5M */
			temp1 = 0x66;	//0xFBB6
			temp2 = 0x01;	//0xFBB7
			temp3 = 0x01;	//0xFBB8
			temp4 = 0x03;	//0xD814
			temp5 = 0x01;	//0xF741
			error = IT9507_writeRegister (modulator, Processor_LINK, 0xD812, 3);
			if (error) goto exit;			
			break;

		case 6000:              /** 6M */
			temp1 = 0x6E;	//0xFBB6
			temp2 = 0x01;	//0xFBB7
			temp3 = 0x01;	//0xFBB8
			temp4 = 0x03;	//0xD814
			temp5 = 0x01;	//0xF741
			error = IT9507_writeRegister (modulator, Processor_LINK, 0xD812, 3);
			if (error) goto exit;
			break;

		case 7000:              /** 7M */
			temp1 = 0x76;	//0xFBB6
			temp2 = 0x02;	//0xFBB7
			temp3 = 0x01;	//0xFBB8
			temp4 = 0x03;	//0xD814
			temp5 = 0x02;	//0xF741
			error = IT9507_writeRegister (modulator, Processor_LINK, 0xD812, 3);
			if (error) goto exit;			
			break;

		case 8000:              /** 8M */
			temp1 = 0x1E;	//0xFBB6
			temp2 = 0x02;	//0xFBB7
			temp3 = 0x01;	//0xFBB8
			temp4 = 0x03;	//0xD814
			temp5 = 0x02;	//0xF741
			error = IT9507_writeRegister (modulator, Processor_LINK, 0xD812, 3);
			if (error) goto exit;
			break;

		default:
			
			error = ModulatorError_INVALID_BW;
			break;
	}

	if(error == ModulatorError_NO_ERROR){
		error = IT9507_writeRegister (modulator, Processor_OFDM, 0xFBB6, temp1);
		if (error) goto exit;

		error = IT9507_writeRegisterBits (modulator, Processor_OFDM, 0xFBB7, 0, 2, temp2);
		if (error) goto exit;

		error = IT9507_writeRegisterBits (modulator, Processor_OFDM, 0xFBB8, 2, 1, temp3);
		if (error) goto exit;

		error = IT9507_writeRegister (modulator,  Processor_OFDM, 0xF741, temp5);
		if (error) goto exit;

		error = IT9507_writeRegister (modulator, Processor_LINK, 0xD814, temp4);
		if (error) goto exit;

		error = IT9507_writeRegisterBits (modulator, Processor_OFDM, 0xFBB8, 2, 1, 0);
		if (error) goto exit;
		

	}	

	

exit :
	if(error)
		modulator->bandwidth = 0;
	else
		modulator->bandwidth = bandwidth;
	return (error);
}

u32 IT9507_runTxCalibration (
	IN  Modulator*    modulator,
	IN  u16            bandwidth,
    IN  u32           frequency
){
	u32 error = ModulatorError_NO_ERROR;
	u8 c1_default_value[2],c2_default_value[2],c3_default_value[2];

	if((bandwidth !=0) && (frequency !=0)){
		error = EagleTuner_setIQCalibration(modulator,frequency);		
	}else{
		error = ModulatorError_FREQ_OUT_OF_RANGE;
		goto exit;
	}
	if (error) goto exit;
	error = IT9507_readRegisters (modulator, Processor_OFDM, p_eagle_reg_iqik_c1_7_0, 2, c1_default_value);
	if (error) goto exit;
	error = IT9507_readRegisters (modulator, Processor_OFDM, p_eagle_reg_iqik_c2_7_0, 2, c2_default_value);
	if (error) goto exit;
	error = IT9507_readRegisters (modulator, Processor_OFDM, p_eagle_reg_iqik_c3_7_0, 2, c3_default_value);
	if (error) goto exit;
	
	modulator->calibrationInfo.c1DefaultValue = c1_default_value[1]<<8 | c1_default_value[0];
	modulator->calibrationInfo.c2DefaultValue = c2_default_value[1]<<8 | c2_default_value[0];
	modulator->calibrationInfo.c3DefaultValue = c3_default_value[1]<<8 | c3_default_value[0];
	modulator->calibrationInfo.outputGain = 0;

exit:
	return (error);
}

u32 IT9507_setFrequency (
	IN  Modulator*    modulator,
	IN  u32           frequency
) {
	u32 error = ModulatorError_NO_ERROR;
	
	unsigned int tmp;
	u8 freq_code_H,freq_code_L;
	u16 TABLE_NROW = modulator->calibrationInfo.tableGroups;
	
	if(modulator->calibrationInfo.ptrIQtableEx[TABLE_NROW-1].frequency<frequency 
		|| modulator->calibrationInfo.ptrIQtableEx[0].frequency>frequency 
	){
		error = ModulatorError_FREQ_OUT_OF_RANGE;
		goto exit;
	}
	
	/*----- set_lo_freq -----*/
	tmp = IT9507_getLoFreq(frequency);
	freq_code_L = (unsigned char) (tmp & 0xFF);
	freq_code_H = (unsigned char) ((tmp >> 8) & 0xFF);

	error = IT9507_writeRegister (modulator, Processor_OFDM, 0xFB2A, freq_code_L);
	if (error) goto exit;
	error = IT9507_writeRegister (modulator,  Processor_OFDM, 0xFB2B, freq_code_H);
	if (error) goto exit;

	if(frequency>950000)
		error = IT9507_writeRegisterBits (modulator, Processor_OFDM, 0xFB2C, 2, 1,1);
	else
		error = IT9507_writeRegisterBits (modulator, Processor_OFDM, 0xFB2C, 2, 1,0);
	if (error) goto exit;

	error = IT9507_writeRegister (modulator, Processor_OFDM, 0xFB2D, 2);
	if (error) goto exit;

	error = IT9507_writeRegister (modulator, Processor_OFDM, 0xFB2D, 1);
	if (error) goto exit;

	error = IT9507_writeRegister (modulator, Processor_OFDM, 0xFB2D, 0);
	if (error) goto exit;
	modulator->frequency = frequency;

	error = IT9507_runTxCalibration(modulator, modulator->bandwidth, modulator->frequency);

exit :
	return (error);
}

u32 IT9507_getFirmwareVersion (
    IN  Modulator*    modulator,
    IN  Processor       processor,
    OUT u32*          version
) {
    u32 error = ModulatorError_NO_ERROR;

	u8 writeBuffer[1] = {0,};
	u8 readBuffer[4] = {0,};

	/** Check chip version */
	writeBuffer[0] = 1;
	error = IT9507Cmd_sendCommand (modulator, Command_QUERYINFO, processor, 1, writeBuffer, 4, readBuffer);
	if (error) goto exit;
	
	*version = (u32) (((u32) readBuffer[0] << 24) + ((u32) readBuffer[1] << 16) + ((u32) readBuffer[2] << 8) + (u32) readBuffer[3]);

exit :
	return (error);
}

u32 IT9507_loadFirmware (
	IN  Modulator*    modulator,
	IN  u8*           firmwareCodes,
	IN  Segment*        firmwareSegments,
	IN  u16*           firmwarePartitions
) {
	u32 error = ModulatorError_NO_ERROR;
	u32 beginPartition = 0;
	u32 endPartition = 0;
	u32 version;
	u32 firmwareLength;
	u8* firmwareCodesPointer;
	u32 i;
	u8 temp;
	
	/** Set I2C master clock speed. */
	temp = EagleUser_IIC_SPEED;
	error = IT9507_writeRegisters (modulator, Processor_LINK, p_eagle_reg_lnk2ofdm_data_63_56, 1, &temp);
	if (error) goto exit;

	firmwareCodesPointer = firmwareCodes;

	beginPartition = 0;
	endPartition = firmwarePartitions[0];


	for (i = beginPartition; i < endPartition; i++) {
		firmwareLength = firmwareSegments[i].segmentLength;
		if (firmwareSegments[i].segmentType == 1) {
			/** Copy firmware */
			error = IT9507Cmd_sendCommand (modulator, Command_SCATTER_WRITE, Processor_LINK, firmwareLength, firmwareCodesPointer, 0, NULL);
			if (error) goto exit;
		}else{
			error = ModulatorError_INVALID_FW_TYPE;
			goto exit;
		}
		firmwareCodesPointer += firmwareLength;
	}

	/** Boot */
	error = IT9507Cmd_sendCommand (modulator, Command_BOOT, Processor_LINK, 0, NULL, 0, NULL);
	if (error) goto exit;

	EagleUser_delay (modulator, 10);

	/** Check if firmware is running */
	version = 0;
	error = IT9507_getFirmwareVersion (modulator, Processor_LINK, &version);
	if (error) goto exit;
	if (version == 0)
		error = ModulatorError_BOOT_FAIL;

exit :
	return (error);
}

u32 IT9507_loadScript (
	IN  Modulator*    modulator,
	IN  u16*           scriptSets,
	IN  ValueSet*       scripts
) {
	u32 error = ModulatorError_NO_ERROR;
	u16 beginScript;
	u16 endScript;
	u8 i, supportRelay = 0, chipNumber = 0, bufferLens = 1;
	u16 j;
	u8 temp;
	u8 buffer[20] = {0,};
	u32 tunerAddr, tunerAddrTemp;
	
	/** Querry SupportRelayCommandWrite **/
	error = IT9507_readRegisters (modulator, Processor_OFDM, 0x004D, 1, &supportRelay);
	if (error) goto exit;

	
	/** Enable RelayCommandWrite **/
	if (supportRelay) {
		temp = 1;
		error = IT9507_writeRegisters (modulator, Processor_OFDM, 0x004E, 1, &temp);
		if (error) goto exit;
	}

	if ((scriptSets[0] != 0) && (scripts != NULL)) {
		beginScript = 0;
		endScript = scriptSets[0];

		for (i = 0; i < chipNumber; i++) {
			/** Load OFSM init script */
			for (j = beginScript; j < endScript; j++) {
				tunerAddr = tunerAddrTemp = scripts[j].address;
				buffer[0] = scripts[j].value;

				while (j < endScript && bufferLens < 20) {
					tunerAddrTemp += 1;
					if (tunerAddrTemp != scripts[j+1].address)
						break;

					buffer[bufferLens] = scripts[j+1].value;
					bufferLens ++;
					j ++;
				}

				error = IT9507_writeRegisters (modulator, Processor_OFDM, tunerAddr, bufferLens, buffer);
				if (error) goto exit;
				bufferLens = 1;
			}
		}
	}

	/** Disable RelayCommandWrite **/
	if (supportRelay) {
		temp = 0;
		error = IT9507_writeRegisters (modulator, Processor_OFDM, 0x004E, 1, &temp);
		if (error) goto exit;
	}

exit :
	return (error);
}

u32 IT9507_writeEepromValues (
    IN  Modulator*    modulator,
    IN  u16            registerAddress,
    IN  u8            writeBufferLength,
    IN  u8*           writeBuffer
) {
    u32       error = ModulatorError_NO_ERROR;
    u16        command;
    u8        buffer[255];
    u32       bufferLength;
    u32       remainLength;
    u32       sendLength;
    u32       i;
   
    u32       maxFrameSize;
	u8 eepromAddress = 0x01;	
	u8 registerAddressLength = 0x01;

    EagleUser_enterCriticalSection (modulator);

    if (writeBufferLength == 0) goto exit;

    maxFrameSize = EagleUser_MAXFRAMESIZE; 

    if ((u32)(writeBufferLength + 11) > maxFrameSize) {
        error = ModulatorError_INVALID_DATA_LENGTH;
        goto exit;
    }

    /** add frame header */
    command   = IT9507Cmd_buildCommand (Command_REG_EEPROM_WRITE, Processor_LINK);
    buffer[1] = (u8) (command >> 8);
    buffer[2] = (u8) command;
    buffer[3] = (u8) IT9507Cmd_sequence++;
    buffer[4] = (u8) writeBufferLength;
    buffer[5] = (u8) eepromAddress;
    buffer[6] = (u8) registerAddressLength;
    buffer[7] = (u8) (registerAddress >> 8);  /** Get high byte of reg. address */
    buffer[8] = (u8) registerAddress;         /** Get low byte of reg. address  */

    /** add frame data */
    for (i = 0; i < writeBufferLength; i++) {
        buffer[9 + i] = writeBuffer[i];
    }

    /** add frame check-sum */
    bufferLength = 9 + writeBufferLength;
    error = IT9507Cmd_addChecksum (modulator, &bufferLength, buffer);
    if (error) goto exit;

    /** send frame */
    i = 0;
    sendLength = 0;
    remainLength = bufferLength;
    while (remainLength > 0) {
        i     = (remainLength > EagleUser_MAX_PKT_SIZE) ? (EagleUser_MAX_PKT_SIZE) : (remainLength);        
        error = EagleUser_busTx (modulator, i, &buffer[sendLength]);
        if (error) goto exit;

        sendLength   += i;
        remainLength -= i;
    }

    /** get reply frame */
    bufferLength = 5;
    error = EagleUser_busRx (modulator, bufferLength, buffer);
    if (error) goto exit;

    /** remove check-sum from reply frame */
    error = IT9507Cmd_removeChecksum (modulator, &bufferLength, buffer);
    if (error) goto exit;

exit :
    EagleUser_leaveCriticalSection (modulator);
    return (error);
}


u32 IT9507_readEepromValues (
    IN  Modulator*    modulator,
    IN  u16            registerAddress,
    IN  u8            readBufferLength,
    OUT u8*           readBuffer
) {
    u32       error = ModulatorError_NO_ERROR;
    u16        command;
    u8        buffer[255];
    u32       bufferLength;
    u32       remainLength;
    u32       sendLength;
    u32       i, k;
    
    u32   maxFrameSize;
	u8	eepromAddress = 0x01;

	u8	registerAddressLength = 0x01;

    EagleUser_enterCriticalSection (modulator);

    if (readBufferLength == 0) goto exit;

    
    maxFrameSize = EagleUser_MAXFRAMESIZE; 

    if ((u32)(readBufferLength + 5) > EagleUser_MAX_PKT_SIZE) {
        error  = ModulatorError_INVALID_DATA_LENGTH;
        goto exit;
    }
        
    if ((u32)(readBufferLength + 5) > maxFrameSize) {
        error  = ModulatorError_INVALID_DATA_LENGTH;
        goto exit;
    }

    /** add command header */
    command   = IT9507Cmd_buildCommand (Command_REG_EEPROM_READ, Processor_LINK);
    buffer[1] = (u8) (command >> 8);
    buffer[2] = (u8) command;
    buffer[3] = (u8) IT9507Cmd_sequence++;
    buffer[4] = (u8) readBufferLength;
    buffer[5] = (u8) eepromAddress;
    buffer[6] = (u8) registerAddressLength;
    buffer[7] = (u8) (registerAddress >> 8);  /** Get high byte of reg. address */
    buffer[8] = (u8) registerAddress;         /** Get low byte of reg. address  */

    /** add frame check-sum */
    bufferLength = 9;
    error = IT9507Cmd_addChecksum (modulator, &bufferLength, buffer);
    if (error) goto exit;

    /** send frame */
    i = 0;
    sendLength   = 0;
    remainLength = bufferLength;
    while (remainLength > 0) {
        i = (remainLength > EagleUser_MAX_PKT_SIZE) ? (EagleUser_MAX_PKT_SIZE) : (remainLength);        
        error = EagleUser_busTx (modulator, i, &buffer[sendLength]);
        if (error) goto exit;

        sendLength   += i;
        remainLength -= i;
    }

    /** get reply frame */
    bufferLength = 5 + readBufferLength;
    error = EagleUser_busTx (modulator, bufferLength, buffer);
    if (error) goto exit;

    /** remove frame check-sum */
    error = IT9507Cmd_removeChecksum (modulator, &bufferLength, buffer);
    if (error) goto exit;

    for (k = 0; k < readBufferLength; k++) {
        readBuffer[k] = buffer[k + 3];
    }

exit :
    EagleUser_leaveCriticalSection (modulator);
    return (error);
}


u32 IT9507_readRegisterBits (
    IN  Modulator*    modulator,
    IN  Processor       processor,
    IN  u32           registerAddress,
    IN  u8            position,
    IN  u8            length,
    OUT u8*           value
) {
        u32 error = ModulatorError_NO_ERROR;
	
	u8 temp = 0;
	error = IT9507_readRegisters (modulator, processor, registerAddress, 1, &temp);
	if (error) goto exit;

	if (length == 8) {
		*value = temp;
	} else {
		temp = REG_GET (temp, position, length);
		*value = temp;
	}

exit :
	return (error);
}


u32 IT9507_loadIrTable (
    IN  Modulator*    modulator,
    IN  u16            tableLength,
    IN  u8*           table
) {
	u32 error = ModulatorError_NO_ERROR;
	u8 baseHigh;
	u8 baseLow;
	u16 registerBase;
	u16 i;

	error = IT9507_readRegister (modulator, Processor_LINK, ir_table_start_15_8, &baseHigh);
	if (error) goto exit;
	error = IT9507_readRegister (modulator, Processor_LINK, ir_table_start_7_0, &baseLow);
	if (error) goto exit;

	registerBase = (u16) (baseHigh << 8) + (u16) baseLow;

	if (registerBase) {
		for (i = 0; i < tableLength; i++) {
			error = IT9507_writeRegister (modulator, Processor_LINK, registerBase + i, table[i]);
			if (error) goto exit;
		}
	}

exit :
	return (error);
}


u32 IT9507_initialize (
    IN  Modulator*    modulator,
	IN  u8            i2cAddr
) {

	u32 error = ModulatorError_NO_ERROR;

	u32 version = 0;
	u8 c1_default_value[2],c2_default_value[2],c3_default_value[2];

	modulator->frequency = 642000;	
	modulator->calibrationInfo.ptrIQtableEx =  IQ_fixed_table0;
	modulator->calibrationInfo.tableGroups = IQ_TABLE_NROW;
	modulator->i2cAddr = i2cAddr;

	error = IT9507_getFirmwareVersion (modulator, Processor_LINK, &version);
	if (error) goto exit;
	if (version != 0) {
		modulator->booted = true;
	} else {
		modulator->booted = false;	
	}

	modulator->firmwareCodes = EagleFirmware_codes;
	modulator->firmwareSegments = EagleFirmware_segments;
	modulator->firmwarePartitions = EagleFirmware_partitions;
	modulator->scriptSets = EagleFirmware_scriptSets;
	modulator->scripts = EagleFirmware_scripts;
	
	/** Write secondary I2C address to device */
	//error = IT9507_writeRegister (modulator, Processor_LINK, p_eagle_reg_lnk2ofdm_data_63_56, EagleUser_IIC_SPEED);
	//if (error) goto exit;	
	
	error = IT9507_writeRegister (modulator, Processor_LINK, second_i2c_address, 0x00);
	if (error) goto exit;

	
	/** Load firmware */
	if (modulator->firmwareCodes != NULL) {
		if (modulator->booted == false) {
			error = IT9507_loadFirmware (modulator, modulator->firmwareCodes, modulator->firmwareSegments, modulator->firmwarePartitions);
			if (error) goto exit;
			modulator->booted = true;
		}
	}
	error = IT9507_writeRegister (modulator, Processor_LINK, 0xD924, 0);//set UART -> GPIOH4
	if (error) goto exit;


	/** Set I2C master clock 100k in order to support tuner I2C. */
	error = IT9507_writeRegister (modulator, Processor_LINK, p_eagle_reg_lnk2ofdm_data_63_56, 0x7);//1a
	if (error) goto exit;

	/** Load script */
	if (modulator->scripts != NULL) {
		error = IT9507_loadScript (modulator, modulator->scriptSets, modulator->scripts);
		if (error) goto exit;
	}


	error = IT9507_writeRegisterBits (modulator, Processor_OFDM, 0xFB26, 7, 1, 1);
	if (error) goto exit;

	error = IT9507_writeRegister (modulator, Processor_OFDM, 0xFBBD, 0xE0);
	if (error) goto exit;

	error = IT9507_writeRegister (modulator, Processor_OFDM, 0xF99A, 0);
	if (error) goto exit;

	error = EagleUser_Initialization(modulator);
	if (error) goto exit;

	/** Set the desired stream type */
	error = IT9507_setTsInterface (modulator);
	if (error) goto exit;

	/** Set H/W MPEG2 locked detection **/
	error = IT9507_writeRegister (modulator, Processor_LINK, p_eagle_reg_top_lock3_out, 1);
	if (error) goto exit;

	error = IT9507_writeRegister (modulator, Processor_LINK, p_eagle_reg_top_padmiscdrsr, 1);
	if (error) goto exit;
	/** Set registers for driving power 0xD830 **/
	error = IT9507_writeRegister (modulator, Processor_LINK, p_eagle_reg_top_padmiscdr2, 0);
	if (error) goto exit;
	

	/** Set registers for driving power 0xD831 **/
	error = IT9507_writeRegister (modulator, Processor_LINK, p_eagle_reg_top_padmiscdr4, 0);
	if (error) goto exit;

	/** Set registers for driving power 0xD832 **/
	error = IT9507_writeRegister (modulator, Processor_LINK, p_eagle_reg_top_padmiscdr8, 0);
	if (error) goto exit;   

	error = IT9507_writeRegister (modulator, Processor_OFDM, 0xFB2E, 0x11);
	if (error) goto exit;
	error = IT9507_writeRegister (modulator, Processor_OFDM, 0xFBB3, 0x98);
	if (error) goto exit;

	error = IT9507_readRegisters (modulator, Processor_OFDM, p_eagle_reg_iqik_c1_7_0, 2, c1_default_value);
	if (error) goto exit;
	error = IT9507_readRegisters (modulator, Processor_OFDM, p_eagle_reg_iqik_c2_7_0, 2, c2_default_value);
	if (error) goto exit;
	error = IT9507_readRegisters (modulator, Processor_OFDM, p_eagle_reg_iqik_c3_7_0, 2, c3_default_value);
	if (error) goto exit;

	modulator->calibrationInfo.c1DefaultValue = c1_default_value[1]<<8 | c1_default_value[0];
	modulator->calibrationInfo.c2DefaultValue = c2_default_value[1]<<8 | c2_default_value[0];
	modulator->calibrationInfo.c3DefaultValue = c3_default_value[1]<<8 | c3_default_value[0];

exit:

	return (error);
}


u32 IT9507_reset (
    IN  Modulator*    modulator
) {
	u32 error = ModulatorError_NO_ERROR;

	u8 value;
	u8 j;
	/** Enable OFDM reset */
	error = IT9507_writeRegisterBits (modulator, Processor_OFDM, I2C_eagle_reg_ofdm_rst_en, eagle_reg_ofdm_rst_en_pos, eagle_reg_ofdm_rst_en_len, 0x01);
	if (error) goto exit;

	/** Start reset mechanism */
	value = 0x00;
	
	/** Clear ofdm reset */
	for (j = 0; j < 150; j++) {
		error = IT9507_readRegisterBits (modulator, Processor_OFDM, I2C_eagle_reg_ofdm_rst, eagle_reg_ofdm_rst_pos, eagle_reg_ofdm_rst_len, &value);
		if (error) goto exit;
		if (value) break;
		EagleUser_delay (modulator, 10);
	}

	if (j == 150) {
		error = ModulatorError_RESET_TIMEOUT;
		goto exit;
	}

	error = IT9507_writeRegisterBits (modulator, Processor_OFDM, I2C_eagle_reg_ofdm_rst, eagle_reg_ofdm_rst_pos, eagle_reg_ofdm_rst_len, 0);
	if (error) goto exit;

	/** Disable OFDM reset */
	error = IT9507_writeRegisterBits (modulator, Processor_OFDM, I2C_eagle_reg_ofdm_rst_en, eagle_reg_ofdm_rst_en_pos, eagle_reg_ofdm_rst_en_len, 0x00);
	if (error) goto exit;
	

exit :

	return (error);
}


u32 IT9507_setTxModeEnable (
    IN  Modulator*            modulator,
    IN  u8                    enable
) {
	u32 error = ModulatorError_NO_ERROR;

	if(enable){

		//afe Power up
		error = IT9507_writeRegister (modulator, Processor_OFDM, p_eagle_reg_afe_mem0, 0);
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_OFDM, p_eagle_reg_afe_mem1, 0xFC);
		if (error) goto exit;
			
		error = IT9507_writeRegister (modulator, Processor_OFDM, p_eagle_reg_fec_sw_rst, 0);
		if (error) goto exit;

		error = IT9507_writeRegister (modulator, Processor_LINK, 0xDDAB, 0);
		if (error) goto exit;
	
		error = IT9507_writeRegister (modulator, Processor_OFDM, eagle_reg_tx_fifo_overflow, 1); //clear
		if (error) goto exit;
		
	}else{

		error = IT9507_writeRegister (modulator, Processor_LINK, 0xDDAB, 1);
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_OFDM, p_eagle_reg_fec_sw_rst, 1);
		if (error) goto exit;

		//afe Power down
		error = IT9507_writeRegister (modulator, Processor_OFDM, p_eagle_reg_afe_mem0, 1);
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_OFDM, p_eagle_reg_afe_mem1, 0xFE);
		if (error) goto exit;
			
	}
	error = EagleUser_setTxModeEnable(modulator, enable);
exit :
	EagleUser_delay(modulator,100);
	return (error);
}


u32 IT9507_setTXChannelModulation (
    IN  Modulator*            modulator,
    IN  ChannelModulation*      channelModulation
) {
	u32 error = ModulatorError_NO_ERROR;

	u8 temp;

	//u8 temp;
	error = IT9507_setTxModeEnable(modulator,0);
	if (error) goto exit;
	/** Set constellation type */
	temp=(u8)channelModulation->constellation;

	modulator->channelModulation.constellation=channelModulation->constellation;
	error = IT9507_writeRegister (modulator, Processor_OFDM, 0xf721, temp);
	if (error) goto exit;

	modulator->channelModulation.highCodeRate=channelModulation->highCodeRate;
	temp=(u8)channelModulation->highCodeRate;
	error = IT9507_writeRegister (modulator, Processor_OFDM, 0xf723, temp);
	if (error) goto exit;
	/** Set low code rate */

	/** Set guard interval */
	modulator->channelModulation.interval=channelModulation->interval;
	temp=(u8)channelModulation->interval;

	error = IT9507_writeRegister (modulator, Processor_OFDM, p_eagle_reg_tps_gi, temp);
	if (error) goto exit;
	/** Set FFT mode */
	modulator->channelModulation.transmissionMode=channelModulation->transmissionMode;
	temp=(u8)channelModulation->transmissionMode;
	error = IT9507_writeRegister (modulator, Processor_OFDM, 0xf726, temp);
	if (error) goto exit;


	switch (channelModulation->interval){
		case Interval_1_OVER_32:              
			temp = 8;
			break;
		case Interval_1_OVER_16:            
			temp = 4;			
			break;
		case Interval_1_OVER_8:            
			temp = 2;			
			break;
		case Interval_1_OVER_4:             
			temp = 1;
			break;

		default:
			
			error = ModulatorError_INVALID_CONSTELLATION_MODE;
			
			break;
	}

	if(error)
		goto exit;
	error = IT9507_writeRegister (modulator, Processor_OFDM, 0xf7C1, temp);
	if (error) goto exit;

	error = IT9507_writeRegister (modulator, Processor_OFDM, 0xf7C6, 1);
	if (error) goto exit;

exit :
	return (error);
}

u32 IT9507_acquireTxChannel (
	IN  Modulator*            modulator,
    IN  u16            bandwidth,
    IN  u32           frequency
) {
	u32 error = ModulatorError_NO_ERROR;
	u16 TABLE_NROW = modulator->calibrationInfo.tableGroups;
	if(modulator->calibrationInfo.ptrIQtableEx[TABLE_NROW-1].frequency<frequency 
		|| modulator->calibrationInfo.ptrIQtableEx[0].frequency>frequency 
	){
		error = ModulatorError_FREQ_OUT_OF_RANGE;
		goto exit;
	}

	error = IT9507_selectBandwidth (modulator, bandwidth);
	if (error) goto exit;
	modulator->bandwidth = bandwidth;
	
	/** Set frequency */
	
	error = IT9507_setFrequency (modulator, frequency);
	if (error) goto exit;
	
	error = EagleUser_acquireChannel(modulator, bandwidth, frequency);
exit :
	return (error);
}

#if 0
u32 IT9507_resetPSBBuffer (
	IN  Modulator*    modulator
){
	u32 error = ModulatorError_NO_ERROR;
	u32 temp;

	if(modulator->tsInterfaceType == PARALLEL_TS_INPUT)
		temp = 0xF9CC;
	else
		temp = 0xF9CD;

	error = IT9507_writeRegister (modulator, Processor_OFDM, 0xF9A4, 1);
	if (error) goto exit;

	error = IT9507_writeRegister (modulator, Processor_OFDM, temp, 0);
	if (error) goto exit;



	error = IT9507_writeRegister (modulator, Processor_OFDM, 0xF9A4, 0);
	if (error) goto exit;

	error = IT9507_writeRegister (modulator, Processor_OFDM, temp, 1);

exit :


	return (error);
}
#endif

static u32 IT9507_setTsInterface (
    IN  Modulator*    modulator
) {
    u32 error = ModulatorError_NO_ERROR;
	u16 frameSize;
	u8 packetSize;
	u8 buffer[2];

	error = IT9507_writeRegisterBits (modulator, Processor_LINK, p_eagle_reg_dvbt_inten, eagle_reg_dvbt_inten_pos, eagle_reg_dvbt_inten_len, 1);
	if (error) goto exit;
	error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_eagle_reg_mpeg_full_speed, eagle_reg_mpeg_full_speed_pos, eagle_reg_mpeg_full_speed_len, 0);
	if (error) goto exit;

	/** Enable DVB-T mode */
	error = IT9507_writeRegisterBits (modulator, Processor_LINK, p_eagle_reg_dvbt_en, eagle_reg_dvbt_en_pos, eagle_reg_dvbt_en_len, 1);
	if (error) goto exit;
	error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_mp2if_mpeg_ser_mode, mp2if_mpeg_ser_mode_pos, mp2if_mpeg_ser_mode_len, 0);
	if (error) goto exit;
	error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_mp2if_mpeg_par_mode, mp2if_mpeg_par_mode_pos, mp2if_mpeg_par_mode_len, 0);
	if (error) goto exit;
	/** Fix current leakage */
	error = IT9507_writeRegisterBits (modulator, Processor_LINK, p_eagle_reg_top_hostb_mpeg_ser_mode, eagle_reg_top_hostb_mpeg_ser_mode_pos, eagle_reg_top_hostb_mpeg_ser_mode_len, 0);
	if (error) goto exit;
	error = IT9507_writeRegisterBits (modulator, Processor_LINK, p_eagle_reg_top_hostb_mpeg_par_mode, eagle_reg_top_hostb_mpeg_par_mode_pos, eagle_reg_top_hostb_mpeg_par_mode_len, 0);
	if (error) goto exit;
	
	error = IT9507_writeRegister (modulator, Processor_OFDM, 0xF714, 0);
	if (error) goto exit;

	frameSize = EagleUser_USB20_FRAME_SIZE_DW;
	packetSize = (u8) (EagleUser_USB20_MAX_PACKET_SIZE / 4);

	
	error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_eagle_reg_mp2_sw_rst, eagle_reg_mp2_sw_rst_pos, eagle_reg_mp2_sw_rst_len, 1);
	if (error) goto exit;

	/** Reset EP5 */
	error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_eagle_reg_mp2if2_sw_rst, eagle_reg_mp2if2_sw_rst_pos, eagle_reg_mp2if2_sw_rst_len, 1);
	if (error) goto exit;

	
	/** Disable EP5 */
	error = IT9507_writeRegisterBits (modulator, Processor_LINK, p_eagle_reg_ep5_tx_en, eagle_reg_ep5_tx_en_pos, eagle_reg_ep5_tx_en_len, 0);
	if (error) goto exit;

	/** Disable EP5 NAK */
	error = IT9507_writeRegisterBits (modulator, Processor_LINK, p_eagle_reg_ep5_tx_nak, eagle_reg_ep5_tx_nak_pos, eagle_reg_ep5_tx_nak_len, 0);
	if (error) goto exit;


	/** Enable EP5 */
	error = IT9507_writeRegisterBits (modulator, Processor_LINK, p_eagle_reg_ep5_tx_en, eagle_reg_ep5_tx_en_pos, eagle_reg_ep5_tx_en_len, 1);
	if (error) goto exit;

	/** Set EP5 transfer length */
	buffer[p_eagle_reg_ep5_tx_len_7_0 - p_eagle_reg_ep5_tx_len_7_0] = (u8) frameSize;
	buffer[p_eagle_reg_ep5_tx_len_15_8 - p_eagle_reg_ep5_tx_len_7_0] = (u8) (frameSize >> 8);
	error = IT9507_writeRegisters (modulator, Processor_LINK, p_eagle_reg_ep5_tx_len_7_0, 2, buffer);

	/** Set EP5 packet size */
	error = IT9507_writeRegister (modulator, Processor_LINK, p_eagle_reg_ep5_max_pkt, packetSize);
	if (error) goto exit;

	/** Disable 15 SER/PAR mode */
	error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_mp2if_mpeg_ser_mode, mp2if_mpeg_ser_mode_pos, mp2if_mpeg_ser_mode_len, 0);
	if (error) goto exit;
	error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_mp2if_mpeg_par_mode, mp2if_mpeg_par_mode_pos, mp2if_mpeg_par_mode_len, 0);
	if (error) goto exit;

	
	/** Enable mp2if2 */
	error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_eagle_reg_mp2if2_en, eagle_reg_mp2if2_en_pos, eagle_reg_mp2if2_en_len, 1);
	if (error) goto exit;

		/** Enable tsis */
		error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_eagle_reg_tsip_en, eagle_reg_tsip_en_pos, eagle_reg_tsip_en_len, 0);
		if (error) goto exit;
		error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_eagle_reg_tsis_en, eagle_reg_tsis_en_pos, eagle_reg_tsis_en_len, 1);
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_OFDM, p_eagle_reg_ts_in_src, 0);
		if (error) goto exit;

	/** Negate EP4 reset */
	error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_eagle_reg_mp2_sw_rst, eagle_reg_mp2_sw_rst_pos, eagle_reg_mp2_sw_rst_len, 0);
	if (error) goto exit;

	/** Negate EP5 reset */
	error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_eagle_reg_mp2if2_sw_rst, eagle_reg_mp2if2_sw_rst_pos, eagle_reg_mp2if2_sw_rst_len, 0);
	if (error) goto exit;


	/** Split 15 PSB to 1K + 1K and enable flow control */
	error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_eagle_reg_mp2if2_half_psb, eagle_reg_mp2if2_half_psb_pos, eagle_reg_mp2if2_half_psb_len, 0);
	if (error) goto exit;
	error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_eagle_reg_mp2if_stop_en, eagle_reg_mp2if_stop_en_pos, eagle_reg_mp2if_stop_en_len, 1);
	if (error) goto exit;			

	error = IT9507_writeRegister (modulator, Processor_LINK, p_eagle_reg_top_host_reverse, 0);
	if (error) goto exit;

	error = IT9507_writeRegisterBits (modulator, Processor_LINK, p_eagle_reg_ep6_rx_en, eagle_reg_ep6_rx_en_pos, eagle_reg_ep6_rx_en_len, 0);
	if (error) goto exit;

	error = IT9507_writeRegisterBits (modulator, Processor_LINK, p_eagle_reg_ep6_rx_nak, eagle_reg_ep6_rx_nak_pos, eagle_reg_ep6_rx_nak_len, 0);
	if (error) goto exit;


	error = IT9507_writeRegisterBits (modulator, Processor_LINK, p_eagle_reg_ep6_rx_en, eagle_reg_ep6_rx_en_pos, eagle_reg_ep6_rx_en_len, 1);
	if (error) goto exit;

	error = IT9507_writeRegister (modulator, Processor_LINK, p_eagle_reg_ep6_max_pkt, 0x80);
	if (error) goto exit;

	error = IT9507_writeRegister (modulator, Processor_LINK, p_eagle_reg_ep6_cnt_num_7_0, 0x16);
	if (error) goto exit;

	error = EagleUser_mpegConfig (modulator);

exit :
	return (error);
}

u32 IT9507_getIrCode (
    IN  Modulator*    modulator,
    OUT u32*          code
)  {
	u32 error = ModulatorError_NO_ERROR;
	u8 readBuffer[4];

	error = IT9507Cmd_sendCommand (modulator, Command_IR_GET, Processor_LINK, 0, NULL, 4, readBuffer);
	if (error) goto exit;

	*code = (u32) ((readBuffer[0] << 24) + (readBuffer[1] << 16) + (readBuffer[2] << 8) + readBuffer[3]);

exit :
	return (error);
}


u32 IT9507_TXreboot (
    IN  Modulator*    modulator
)  {
	u32 error = ModulatorError_NO_ERROR;
	u32 version;
	u8 i;
	
	error = IT9507_getFirmwareVersion (modulator, Processor_LINK, &version);
	if (error) goto exit;
	if (version == 0xFFFFFFFF) goto exit;       
	if (version != 0) {
		
		error = IT9507Cmd_reboot (modulator);
		EagleUser_delay (modulator, 1);
		goto exit;
	}

	modulator->booted = false;

exit :
	return (error);
}


u32 IT9507_controlPowerSaving (
    IN  Modulator*    modulator,
    IN  u8            control
) {
	u32 error = ModulatorError_NO_ERROR;

	if (control) {
		/** Power up case */
		error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_eagle_reg_afe_mem0, 3, 1, 0);
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_OFDM, p_eagle_reg_dyn0_clk, 0);
		if (error) goto exit;

		/** Fixed current leakage */
		error = IT9507_writeRegisterBits (modulator, Processor_LINK, p_eagle_reg_top_hostb_mpeg_ser_mode, eagle_reg_top_hostb_mpeg_ser_mode_pos, eagle_reg_top_hostb_mpeg_ser_mode_len, 0);
		if (error) goto exit;
		/** Disable HostB parallel */  
		error = IT9507_writeRegisterBits (modulator, Processor_LINK, p_eagle_reg_top_hostb_mpeg_par_mode, eagle_reg_top_hostb_mpeg_par_mode_pos, eagle_reg_top_hostb_mpeg_par_mode_len, 0);
		if (error) goto exit;
	} else {
		/** Power down case */
		error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_eagle_reg_afe_mem0, 3, 1, 1);

		/** Fixed current leakage */
		/** Enable HostB parallel */
		error = IT9507_writeRegisterBits (modulator, Processor_LINK, p_eagle_reg_top_hostb_mpeg_ser_mode, eagle_reg_top_hostb_mpeg_ser_mode_pos, eagle_reg_top_hostb_mpeg_ser_mode_len, 0);
		if (error) goto exit;
		error = IT9507_writeRegisterBits (modulator, Processor_LINK, p_eagle_reg_top_hostb_mpeg_par_mode, eagle_reg_top_hostb_mpeg_par_mode_pos, eagle_reg_top_hostb_mpeg_par_mode_len, 1);
		if (error) goto exit;						
	}

exit :
	return (error);
}




u32 IT9507_controlPidFilter (
    IN  Modulator*    modulator,
    IN  u8            control,
	IN  u8            enable
) {
	u32 error = ModulatorError_NO_ERROR;

	error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_mp2if_pid_complement, mp2if_pid_complement_pos, mp2if_pid_complement_len, control);
	if(error) goto exit;
	error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_mp2if_pid_en, mp2if_pid_en_pos, mp2if_pid_en_len, enable);

exit:
	return (error);
}


u32 IT9507_resetPidFilter (
    IN  Modulator*    modulator
) {
	u32 error = ModulatorError_NO_ERROR;

	error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_mp2if_pid_rst, mp2if_pid_rst_pos, mp2if_pid_rst_len, 1);
	if (error) goto exit;

exit :
	return (error);
}


u32 IT9507_addPidToFilter (
    IN  Modulator*    modulator,
    IN  u8            index,
    IN  Pid             pid
) {
	u32 error = ModulatorError_NO_ERROR;

	u8 writeBuffer[2];
	
	/** Enable pid filter */
	if((index>0)&&(index<32)){
		writeBuffer[0] = (u8) pid.value;
		writeBuffer[1] = (u8) (pid.value >> 8);

		error = IT9507_writeRegisters (modulator, Processor_OFDM, p_mp2if_pid_dat_l, 2, writeBuffer);
		if (error) goto exit;

		error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_mp2if_pid_index_en, mp2if_pid_index_en_pos, mp2if_pid_index_en_len, 1);
		if (error) goto exit;

		error = IT9507_writeRegister (modulator, Processor_OFDM, p_mp2if_pid_index, index);
		if (error) goto exit;
	}else{
		error = ModulatorError_INDEX_OUT_OF_RANGE;
	}

exit :

	return (error);
}

u32 IT9507_sendHwPSITable (
	IN  Modulator*    modulator,
	IN  u8*            pbuffer
) {
 	u32 error = ModulatorError_NO_ERROR;
	u8 temp_timer[5];
	u8 tempbuf[10] ;
	u8 i,temp;
	
	error = IT9507_readRegisters (modulator, Processor_OFDM, psi_table1_timer_H, 10, temp_timer);		//save pei table timer	
	if (error) goto exit;

	for(i=0;i<10;i++)
		tempbuf[i] = 0;

	error = IT9507_writeRegisters (modulator, Processor_OFDM, psi_table1_timer_H, 10, tempbuf);		//stop send FW psi table	
	if (error) goto exit;

	for(i=0 ; i<50 ;i++){
		error = IT9507_readRegister (modulator, Processor_OFDM, p_reg_psi_access, &temp);		//wait per table send	
		if (error) goto exit;
		if(temp == 0) break;
		EagleUser_delay(modulator,1);
	}

	error = IT9507_writeRegister (modulator, Processor_OFDM, p_reg_psi_index, 0);
	if (error) goto exit;

	for(i=0;i<188;i++){
		temp = pbuffer[i];	
		error = IT9507_writeRegister (modulator, Processor_OFDM, p_reg_psi_dat,  temp); //write data to HW psi table buffer
		if (error) goto exit;
	}

	error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_reg_psi_access, reg_psi_access_pos, reg_psi_access_len, 1); //send psi tabledata
	if (error) goto exit;

	
	error = IT9507_writeRegisters (modulator, Processor_OFDM, psi_table1_timer_H, 10, temp_timer);		//set org timer	
	if (error) goto exit;

	

exit :

	return (error);
}

u32 IT9507_accessFwPSITable (
	IN  Modulator*    modulator,
	IN  u8		  psiTableIndex,
	IN  u8*         pbuffer
) {
	u32 error ;
	u8 i;
	u8 temp[2];
	error = ModulatorError_NO_ERROR;
	
	temp[0] = 0;
	temp[1] = 0;
	if((psiTableIndex>0)&&(psiTableIndex<6)){
		error = IT9507_writeRegisters (modulator, Processor_OFDM, psi_table1_timer_H+(psiTableIndex-1)*2, 2, temp);		//set timer	= 0 & stop
		if (error) goto exit;

		for(i=0;i<188;i++){
			error = IT9507_writeRegister (modulator, Processor_OFDM, (PSI_table1+(psiTableIndex-1)*188)+i, pbuffer[i]);
			if (error) goto exit;

		}

	}else{
		error = ModulatorError_INVALID_INDEX;
	}
	
exit :


	return (error);
}

u32 IT9507_setFwPSITableTimer (
	IN  Modulator*    modulator,
	IN  u8		  psiTableIndex,
	IN  u16          timer_ms
) {
    u32 error ;
	u8 temp[2];
	error = ModulatorError_NO_ERROR;

	temp[0] = (u8)(timer_ms>>8);
	temp[1] = (u8)timer_ms;
	


	if((psiTableIndex>0)&&(psiTableIndex<6)){	
		error = IT9507_writeRegisters (modulator, Processor_OFDM, psi_table1_timer_H+(psiTableIndex-1)*2, 2,temp);		
	}else{
		error = ModulatorError_INVALID_INDEX;
	}	
	return (error);
}


u32 IT9507_setSlaveIICAddress (
    IN  Modulator*    modulator,
	IN  u8          SlaveAddress
){
	u32 error = ModulatorError_NO_ERROR;

	if(modulator != NULL)
		modulator->slaveIICAddr = SlaveAddress;
	else
		error  = ModulatorError_NULL_HANDLE_PTR;
    return (error);
}

u32 IT9507_adjustOutputGain (
	IN  Modulator*    modulator,
	IN  int			  *gain	   
){
	u32 error = ModulatorError_NO_ERROR;
	int amp_mul;
	int c1value = 0;
	int c2value = 0;
	int c3value = 0;	
	int c1value_default;
	int c2value_default;
	int c3value_default;	
	
	u32 amp_mul_max1 = 0;
	u32 amp_mul_max2 = 0;
	u32 amp_mul_max3 = 0;
	int amp_mul_max = 0;
	int i = 0;
	
	int gain_X10 = *gain * 10;
	bool overflow = false;

	c1value_default = modulator->calibrationInfo.c1DefaultValue;
	c2value_default = modulator->calibrationInfo.c2DefaultValue;
	c3value_default = modulator->calibrationInfo.c3DefaultValue;	
	
	if (c1value_default>1023) c1value_default = c1value_default-2048;
	if (c2value_default>1023) c2value_default = c2value_default-2048;
	if (c3value_default>1023) c3value_default = c3value_default-2048;

	amp_mul_max1 = 10000*1023/abs(c1value_default);
	if(c2value_default != 0)
		amp_mul_max2 = 10000*1023/abs(c2value_default);
	else
		amp_mul_max2 = 0xFFFFFFFF;
    amp_mul_max3 = 10000*1023/abs(c3value_default);


	if (amp_mul_max1<amp_mul_max3) {
		if (amp_mul_max1<amp_mul_max2) {
				amp_mul_max = (int)amp_mul_max1;
			} else {
				amp_mul_max = (int)amp_mul_max2;
			}
	  } else if (amp_mul_max3<amp_mul_max2) {
        	amp_mul_max =(int)amp_mul_max3;
   	  } else {
   	  	amp_mul_max =(int)amp_mul_max2;
   	  	}

	if(gain_X10>0){
		//d_amp_mul = 1;
		amp_mul = 10000;
		for(i = 0 ; i<gain_X10 ; i+=10){
			if (amp_mul_max>amp_mul) {
				amp_mul = (amp_mul * 11220)/10000;
				c1value = (c1value_default * amp_mul)/10000;
				c2value = (c2value_default* amp_mul)/10000;
				c3value = (c3value_default * amp_mul)/10000;
			}
			if(c1value>0x03ff){
				c1value=0x03ff;
				overflow = true;				
			}
			/*if(c2value>0x03ff){
				c2value=0x03ff;
				overflow = true;
				
			}*/
			if(c3value>0x03ff){
				c3value=0x03ff;
				overflow = true;				
			}

			if(overflow)
				break;
		}
			
		
	}else if(gain_X10<0){
		//d_amp_mul = 1;
		amp_mul = 10000;
		for(i = 0 ; i>gain_X10 ; i-=10){
			if (amp_mul_max>amp_mul) {
				//d_amp_mul *= 0.501;
			amp_mul = (amp_mul * 8910)/10000;
			
			c1value = (c1value_default * amp_mul)/10000;
			c2value = (c2value_default * amp_mul)/10000;
			c3value = (c3value_default * amp_mul)/10000;
			}
			if(c1value==0){
				overflow = true;
			}
			/*if(c2value==0){
				overflow = true;
			}*/
			if(c3value==0){
				overflow = true;
			}

			if(overflow)
				break;			
		}
		
	}else{
		c1value = c1value_default;
		c2value = c2value_default;
		c3value = c3value_default;

	}
	if (c1value<0) {c1value=c1value+2048;}
	if (c2value<0) {c2value=c2value+2048;}
	if (c3value<0) {c3value=c3value+2048;}
	c1value = (c1value%2048);
	c2value = (c2value%2048);
	c3value = (c3value%2048);
	*gain = i/10;
	modulator->calibrationInfo.outputGain = *gain;
	
	error = IT9507_writeRegister (modulator, Processor_OFDM, p_eagle_reg_iqik_c1_7_0, (u8)(c1value&0x00ff));
	if (error) goto exit;		
	error = IT9507_writeRegister (modulator, Processor_OFDM, p_eagle_reg_iqik_c1_10_8, (u8)(c1value>>8));
	if (error) goto exit;		
	error = IT9507_writeRegister (modulator, Processor_OFDM, p_eagle_reg_iqik_c2_7_0, (u8)(c2value&0x00ff));
	if (error) goto exit;
	error = IT9507_writeRegister (modulator, Processor_OFDM, p_eagle_reg_iqik_c2_10_8, (u8)(c2value>>8));
	if (error) goto exit;
	error = IT9507_writeRegister (modulator, Processor_OFDM, p_eagle_reg_iqik_c3_7_0, (u8)(c3value&0x00ff));
	if (error) goto exit;
	error = IT9507_writeRegister (modulator, Processor_OFDM, p_eagle_reg_iqik_c3_10_8, (u8)(c3value>>8));
	if (error) goto exit;

exit:

	return (error);
}

u32 IT9507_getGainRange (
	IN  Modulator*    modulator,
	IN  u32           frequency,
	IN  u16            bandwidth,    
	OUT int*			maxGain,
	OUT int*			minGain
){
	u32 error = ModulatorError_NO_ERROR;
	u8 val[6];
	u16 TABLE_NROW = modulator->calibrationInfo.tableGroups;
	if(modulator->calibrationInfo.ptrIQtableEx[TABLE_NROW-1].frequency<frequency 
		|| modulator->calibrationInfo.ptrIQtableEx[0].frequency>frequency 
	){
		error = ModulatorError_FREQ_OUT_OF_RANGE;
		goto exit;
	}

	if((bandwidth !=0) && (frequency !=0)){
		error = EagleTuner_calIQCalibrationValue(modulator,frequency,val);
	}else{
		error = ModulatorError_FREQ_OUT_OF_RANGE;
		goto exit;
	}

	*maxGain = 100;
	IT9507_calOutputGain(modulator, val, maxGain);

	*minGain = -100;
	IT9507_calOutputGain(modulator, val, minGain);
exit:		
	return (error);
}

u32 IT9507_getOutputGain (
	IN  Modulator*    modulator,
	OUT  int			  *gain	   
){
   
    *gain = modulator->calibrationInfo.outputGain;

    return(ModulatorError_NO_ERROR);
}

u32 IT9507_suspendMode (
    IN  Modulator*    modulator,
    IN  u8          enable
){
	u32   error = ModulatorError_NO_ERROR;

	u8 temp;
	error = IT9507_readRegister (modulator, Processor_OFDM, p_eagle_reg_afe_mem0, &temp);//get power setting

	if(error == ModulatorError_NO_ERROR){
		if(enable){
		// suspend mode	
			temp = temp | 0x2D; //set bit0/2/3/5 to 1		
		}else{
		// resume mode	
			temp = temp & 0xD2; //set bit0/2/3/5 to 0	
		}
		error = IT9507_writeRegister (modulator, Processor_OFDM, p_eagle_reg_afe_mem0, temp);
	}
	return (error);
}


u32 IT9507_setTPS (
    IN  Modulator*    modulator,
    IN  TPS           tps
){
	u32   error = ModulatorError_NO_ERROR;
	//---- set TPS Cell ID
	

	error = IT9507_writeRegister (modulator, Processor_OFDM, 0xF727, (u8)(tps.cellid>>8));
	if (error) goto exit;

	error = IT9507_writeRegister (modulator, Processor_OFDM, 0xF728, (u8)(tps.cellid));
		
exit:	
	return (error);

}

u32 IT9507_getTPS (
    IN  Modulator*    modulator,
    IN  pTPS           pTps
){
	u32   error = ModulatorError_NO_ERROR;
	//---- get TPS Cell ID
	u8 temp;
	u16 cellID = 0;

	error = IT9507_readRegister (modulator, Processor_OFDM, 0xF727, &temp);//get cell id
	if (error) goto exit;
	cellID = temp<<8;

	error = IT9507_readRegister (modulator, Processor_OFDM, 0xF728, &temp);//get cell id
	cellID = cellID | temp;	
	pTps->cellid = cellID;

exit:	
	return (error);
}

u32 IT9507_setIQtable (
	IN  Modulator*    modulator,
    IN  IQtable *IQ_table_ptr,
	IN  u16 tableGroups
){
	u32   error = ModulatorError_NO_ERROR;

	if(IQ_table_ptr == NULL){
		error = ModulatorError_NULL_PTR;
		modulator->calibrationInfo.ptrIQtableEx =  IQ_fixed_table0; // set to default table
		modulator->calibrationInfo.tableGroups = IQ_TABLE_NROW;
	}else{
		modulator->calibrationInfo.ptrIQtableEx = IQ_table_ptr;
		modulator->calibrationInfo.tableGroups = tableGroups;
	}
	return (error);
}


u32 IT9507_setDCCalibrationValue (
	IN  Modulator*	modulator,
    IN	int			dc_i,
	IN	int			dc_q
){
	u32   error = ModulatorError_NO_ERROR;
	u16	dc_i_temp,dc_q_temp;
	if(dc_i<0)
		dc_i_temp = (u16)(512 + dc_i) & 0x01FF;
	else
		dc_i_temp = ((u16)dc_i) & 0x01FF;
	

	if(dc_q<0)
		dc_q_temp = (u16)(512 + dc_q) & 0x01FF;
	else
		dc_q_temp = ((u16)dc_q) & 0x01FF;

	error = IT9507_writeRegister (modulator, Processor_OFDM, p_eagle_reg_iqik_dc_i_7_0, (u8)(dc_i_temp));
	if (error) goto exit;

	error = IT9507_writeRegister (modulator, Processor_OFDM, p_eagle_reg_iqik_dc_i_8, (u8)(dc_i_temp>>8));
	if (error) goto exit;

	error = IT9507_writeRegister (modulator, Processor_OFDM, p_eagle_reg_iqik_dc_q_7_0, (u8)(dc_q_temp));
	if (error) goto exit;

	error = IT9507_writeRegister (modulator, Processor_OFDM, p_eagle_reg_iqik_dc_q_8, (u8)(dc_q_temp>>8));
	if (error) goto exit;
exit:
	return (error);
}

u32 IT9507_isTsBufferOverflow (
	IN  Modulator*	modulator,
    IN	bool		*overflow	
){
	u32   error = ModulatorError_NO_ERROR;
	u8	temp = 0;
	error = IT9507_readRegister (modulator, Processor_OFDM, eagle_reg_tx_fifo_overflow, &temp);
	if (error) goto exit;

	if(temp) {
		*overflow = true;
		error = IT9507_writeRegister (modulator, Processor_OFDM, eagle_reg_tx_fifo_overflow, 1); //clear
		if (error) goto exit;
	} else {
		*overflow = false;
	}

exit:
	return (error);
}


#define FW_VER         0x08060000

int dvb_usb_it950x_debug;
module_param_named(debug,dvb_usb_it950x_debug, int, 0644);


#define  p_reg_top_pwrdw_hwen  0xD809 
#define reg_top_pwrdw_hwen_pos 0
#define reg_top_pwrdw_hwen_len 1

#define    p_reg_top_pwrdw 0xD80B 
#define reg_top_pwrdw_pos 0
#define reg_top_pwrdw_len 1

#define    r_reg_top_gpioh1_i	0xD8AE 
#define	reg_top_gpioh1_i_pos 0
#define	reg_top_gpioh1_i_len 1

#define    p_reg_top_gpioh1_o	0xD8AF 
#define	reg_top_gpioh1_o_pos 0
#define	reg_top_gpioh1_o_len 1

#define    p_reg_top_gpioh1_en	0xD8B0 
#define	reg_top_gpioh1_en_pos 0
#define	reg_top_gpioh1_en_len 1

#define    p_reg_top_gpioh1_on	0xD8B1 
#define	reg_top_gpioh1_on_pos 0
#define	reg_top_gpioh1_on_len 1

#define    r_reg_top_gpioh5_i	0xD8BA 
#define	reg_top_gpioh5_i_pos 0
#define	reg_top_gpioh5_i_len 1

#define    p_reg_top_gpioh5_o	0xD8BB 
#define	reg_top_gpioh5_o_pos 0
#define	reg_top_gpioh5_o_len 1

#define    p_reg_top_gpioh5_en	0xD8BC 
#define	reg_top_gpioh5_en_pos 0
#define	reg_top_gpioh5_en_len 1

#define    p_reg_top_gpioh5_on	0xD8BD 
#define	reg_top_gpioh5_on_pos 0
#define	reg_top_gpioh5_on_len 1

#define    p_reg_top_gpioh7_o	0xD8C3 
#define	reg_top_gpioh7_o_pos 0
#define	reg_top_gpioh7_o_len 1


static DEFINE_MUTEX(mymutex);

static u32 DRV_NIMReset(
	void* handle);

static u32 DRV_InitNIMSuspendRegs(
	void* handle);
	
u32 DRV_TunerSuspend(
	void * handle,
	u8 ucChip,
	bool bOn);

static u32 DRV_NIMSuspend(
	void * handle,
	bool bSuspend);

static u32 DRV_NIMReset(
	void* handle);

static u32 DRV_InitNIMSuspendRegs(
	void* handle);

static u32 DRV_Initialize(
	void* handle);

static u32 DL_Initialize(
	void* handle);

static u32 DRV_getFirmwareVersionFromFile( 
		void* handle,
	 	Processor	processor, 
		u32* 		version
)
{

	
	PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;
	u8 chip_version = 0;
	u32 chip_Type;
	u8 var[2];
	u32 error = Error_NO_ERROR;

	u32 OFDM_VER1;
    u32 OFDM_VER2;
    u32 OFDM_VER3;
    u32 OFDM_VER4;

    u32 LINK_VER1;
    u32 LINK_VER2;
    u32 LINK_VER3;    
    u32 LINK_VER4;    
    

	error = IT9507_readRegister((Modulator*) &pdc->modulator, processor, chip_version_7_0, &chip_version);
	error = IT9507_readRegisters((Modulator*) &pdc->modulator, processor, chip_version_7_0+1, 2, var);
	
	if(error) deb_data("DRV_getFirmwareVersionFromFile fail");
	
	chip_Type = var[1]<<8 | var[0];	
	if(chip_Type == 0x9135 && chip_version == 2){
#if 0
	  /* NOT USED */
		OFDM_VER1 = DVB_V2_OFDM_VERSION1;
		OFDM_VER2 = DVB_V2_OFDM_VERSION2;
		OFDM_VER3 = DVB_V2_OFDM_VERSION3;
		OFDM_VER4 = DVB_V2_OFDM_VERSION4;

		LINK_VER1 = DVB_V2_LL_VERSION1;
		LINK_VER2 = DVB_V2_LL_VERSION2;
		LINK_VER3 = DVB_V2_LL_VERSION3;    
		LINK_VER4 = DVB_V2_LL_VERSION4;
#endif
	}else{
		OFDM_VER1 = DVB_OFDM_VERSION1;
    	OFDM_VER2 = DVB_OFDM_VERSION2;
   	 	OFDM_VER3 = DVB_OFDM_VERSION3;
    	OFDM_VER4 = DVB_OFDM_VERSION4;

   		LINK_VER1 = DVB_LL_VERSION1;
    	LINK_VER2 = DVB_LL_VERSION2;
    	LINK_VER3 = DVB_LL_VERSION3;    
    	LINK_VER4 = DVB_LL_VERSION4;
	}

    if(processor == Processor_OFDM) {
        *version = (u32)( (OFDM_VER1 << 24) + (OFDM_VER2 << 16) + (OFDM_VER3 << 8) + OFDM_VER4);
    }
    else { //LINK
        *version = (u32)( (LINK_VER1 << 24) + (LINK_VER2 << 16) + (LINK_VER3 << 8) + LINK_VER4);    
    }
    
    return *version;
}

u32 DRV_getDeviceType(void *handle)
{
	u32 dwError = Error_NO_ERROR;
   	PDEVICE_CONTEXT PDC = (PDEVICE_CONTEXT) handle;

	dwError =  EagleUser_getDeviceType((Modulator*) &PDC->modulator, &PDC->deviceType);

    return(dwError);
}

static u32  DRV_Initialize(
	    void *      handle
)
{
	u32 error = Error_NO_ERROR;
	u32 error_rx = Error_NO_ERROR;
	PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;
	u8 temp = 0;
	//u8 usb_dma_reg;
	u8 chip_version = 0; 
	u32 fileVersion, cmdVersion = 0; 

	deb_data("- Enter %s Function -\n",__FUNCTION__);

	if(EagleUser_getDeviceType((Modulator*) &pdc->modulator, &pdc->deviceType) != 0)
		printk("- EagleUser_getDeviceType fail -\n");

	if(IT9507_setSlaveIICAddress((Modulator*) &pdc->modulator, SLAVE_DEMOD_2WIREADDR) != 0)
		printk("- IT9507_setSlaveIICAddress fail -\n");	
	
	
	if(pdc->modulator.booted) //from Standard_setBusTuner() > Standard_getFirmwareVersion()
    	{
        	//use "#define version" to get fw version (from firmware.h title)
        	error = DRV_getFirmwareVersionFromFile(handle, Processor_OFDM, &fileVersion);

        	//use "Command_QUERYINFO" to get fw version 
        	error = IT9507_getFirmwareVersion((Modulator*) &pdc->modulator, Processor_OFDM, &cmdVersion);
        	if(error) deb_data("DRV_Initialize : IT9507_getFirmwareVersion : error = 0x%08u\n", error);

        	if(cmdVersion != fileVersion)
        	{
            		deb_data("Reboot: Outside Fw = 0x%x, Inside Fw = 0x%x", fileVersion, cmdVersion);
            		error = IT9507_TXreboot((Modulator*) &pdc->modulator);
            		pdc->bBootCode = true;
            		if(error) 
            		{
                		deb_data("IT9507_reboot : error = 0x%08u\n", error);
                		return error;
            		}
            		else {
                		return Error_NOT_READY;
            		}
        	}
        	else
        	{
            		deb_data("	Fw version is the same!\n");
  	      		error = Error_NO_ERROR;
        	}
	}//pdc->IT950x.booted

	//			case StreamType_DVBT_DATAGRAM:
	deb_data("    StreamType_DVBT_DATAGRAM\n");
	error = IT9507_initialize ((Modulator*) &pdc->modulator, 0);
				 
	if (error) deb_data("IT950x_initialize _Device initialize fail : 0x%08x\n", error);
	else deb_data("    Device initialize TX Ok\n");

    IT9507_getFirmwareVersion ((Modulator*) &pdc->modulator, Processor_OFDM, &cmdVersion);
    deb_data("    FwVer OFDM = 0x%x, ", cmdVersion);
    IT9507_getFirmwareVersion ((Modulator*) &pdc->modulator, Processor_LINK, &cmdVersion);
    deb_data("FwVer LINK = 0x%x\n", cmdVersion);
    
	/* Solve 0-byte packet error. write Link 0xDD8D[3] = 1 */
	//error = Demodulator_readRegister((Demodulator*) &pdc->demodulator, Processor_LINK, 0xdd8d, &usb_dma_reg);
	//usb_dma_reg |= 0x08;             /*reg_usb_min_len*/
	//error = Demodulator_writeRegister((Demodulator*) &pdc->demodulator, Processor_LINK, 0xdd8d, usb_dma_reg);
    
    return error;
	
}

static u32 DRV_InitDevInfo(
    	void *      handle,
    	u8        ucSlaveDemod
)
{
    u32 dwError = Error_NO_ERROR;    
   	PDEVICE_CONTEXT PDC = (PDEVICE_CONTEXT) handle;
    PDC->fc[ucSlaveDemod].ulCurrentFrequency = 0;  
    PDC->fc[ucSlaveDemod].ucCurrentBandWidth = 0;

    PDC->fc[ucSlaveDemod].ulDesiredFrequency = 0;	
    PDC->fc[ucSlaveDemod].ucDesiredBandWidth = 6000;	

    //For PID Filter Setting
    //PDC->fc[ucSlaveDemod].ulcPIDs = 0;    
    PDC->fc[ucSlaveDemod].bEnPID = false;

    PDC->fc[ucSlaveDemod].bApOn = false;
    
    PDC->fc[ucSlaveDemod].bResetTs = false;



    PDC->fc[ucSlaveDemod].tunerinfo.bTunerOK = false;
    PDC->fc[ucSlaveDemod].tunerinfo.bSettingFreq = false;

    return dwError;
}	

//get EEPROM_IRMODE/bIrTblDownload/bRAWIr/architecture config from EEPROM
static u32 DRV_GetEEPROMConfig(
	void* handle)
{       
    u32 dwError = Error_NO_ERROR;
	u8 chip_version = 0;
	u32 chip_Type;
	u8  var[2];
    PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;
	//bIrTblDownload option
    u8 btmp = 0;
	int ucSlaveDemod;
	
	deb_data("- Enter %s Function -",__FUNCTION__);

	//patch for read eeprom valid bit
	dwError = IT9507_readRegister((Modulator*) &pdc->modulator, Processor_LINK, chip_version_7_0, &chip_version);
	dwError = IT9507_readRegisters((Modulator*) &pdc->modulator, Processor_LINK, chip_version_7_0+1, 2, var);

	if(dwError) deb_data("DRV_GetEEPROMConfig fail---cant read chip version");

	chip_Type = var[1]<<8 | var[0];
	if(chip_Type==0x9135 && chip_version == 2) //Om2
	{
		pdc->chip_version = 2;
		dwError = IT9507_readRegisters((Modulator*) &pdc->modulator, Processor_LINK, 0x461d, 1, &btmp);
		deb_data("Chip Version is %d---and Read 461d---valid bit = 0x%02X", chip_version, btmp);
	}
	else 
	{
		pdc->chip_version = 1; //Om1
		dwError = IT9507_readRegisters((Modulator*) &pdc->modulator, Processor_LINK, 0x4979, 1, &btmp);
		deb_data("Chip Version is %d---and Read 4979---valid bit = 0x%02X", chip_version, btmp);
	}
	if (dwError) 
	{
		deb_data("0x461D eeprom valid bit read fail!");
		goto exit;
    }

	if(btmp == 0)
	{
	  printk("it905x: btmp=%d\n",btmp);
		deb_data("=============No need read eeprom");
		pdc->bSupportSelSuspend = false;
		pdc->bDualTs = false;
    	pdc->architecture = Architecture_DCA;
    	//pdc->modulator.chipNumber = 1;    
    	pdc->bDCAPIP = false;
		pdc->fc[0].tunerinfo.TunerId = 0x38;
	}
	else
	{
		deb_data("=============Need read eeprom");
		dwError = IT9507_readRegisters((Modulator*) &pdc->modulator, Processor_LINK, EEPROM_IRMODE, 1, &btmp);
    	if (dwError) goto exit;
    	deb_data("EEPROM_IRMODE = 0x%02X, ", btmp);	

//selective suspend
    	pdc->bSupportSelSuspend = false;
	    //btmp = 0; //not support in v8.12.12.3
   		//dwError = IT9507_readRegisters((Demodulator*) &pdc->Demodulator, Processor_LINK, EEPROM_SELSUSPEND, 1, &btmp);
   	 	//if (dwError) goto exit;
    	//if(btmp<2)
    	//	PDC->bSupportSelSuspend = btmp ? true:false; 
    	deb_data("SelectiveSuspend = %s", pdc->bSupportSelSuspend?"ON":"OFF");
    	    
//bDualTs option   
    	pdc->bDualTs = false;
    	pdc->architecture = Architecture_DCA;
    	//pdc->modulator.chipNumber = 1;    
    	pdc->bDCAPIP = false;

    	dwError = IT9507_readRegisters((Modulator*) &pdc->modulator, Processor_LINK, EEPROM_TSMODE, 1, &btmp);
    	if (dwError) goto exit;
    	deb_data("EEPROM_TSMODE = 0x%02X", btmp);

    	if (btmp == 0)     
    	{  
        	deb_data("TSMode = TS1 mode\n");
    	}
    	else if (btmp == 1) 
   		{
        	deb_data("TSMode = DCA+PIP mode\n");
			pdc->architecture = Architecture_DCA;
        	//pdc->modulator.chipNumber = 2;
        	pdc->bDualTs = true;
        	pdc->bDCAPIP = true;
    	}
    	else if (btmp == 2) 
    	{ 
        	deb_data("TSMode = DCA mode\n");
        	//pdc->modulator.chipNumber = 2;
    	}
    	else if (btmp == 3) 
    	{
        	deb_data("TSMode = PIP mode\n");
        	pdc->architecture = Architecture_PIP;
        	//pdc->modulator.chipNumber = 2;
        	pdc->bDualTs = true;
    	}

//tunerID option, in Omega, not need to read register, just assign 0x38;
		dwError = IT9507_readRegisters((Modulator*) &pdc->modulator, Processor_LINK, EEPROM_TUNERID, 1, &btmp);
		if (btmp==0x51) {
			pdc->fc[0].tunerinfo.TunerId = 0x51;  	
		}
		else if (btmp==0x52) {
			pdc->fc[0].tunerinfo.TunerId = 0x52;  	
		}
		else if (btmp==0x60) {
			pdc->fc[0].tunerinfo.TunerId = 0x60;  	
		}
		else if (btmp==0x61) {
			pdc->fc[0].tunerinfo.TunerId = 0x61;  	
		}
		else if (btmp==0x62) {
			pdc->fc[0].tunerinfo.TunerId = 0x62;  	
		}
		else {
			pdc->fc[0].tunerinfo.TunerId = 0x38;  	
		}		
	
		deb_data("pdc->fc[0].tunerinfo.TunerId = 0x%x", pdc->fc[0].tunerinfo.TunerId);
		if (pdc->bDualTs) {
			pdc->fc[1].tunerinfo.TunerId = pdc->fc[0].tunerinfo.TunerId;
			deb_data("pdc->fc[1].tunerinfo.TunerId = 0x%x", pdc->fc[1].tunerinfo.TunerId);
		}

		//dwError = IT9507_writeRegister((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, EEPROM_SUSPEND, 0);
		dwError = IT9507_readRegisters((Modulator*) &pdc->modulator, Processor_LINK, EEPROM_SUSPEND, 1, &btmp);
		deb_data("EEPROM susped mode=%d", btmp);
    	
    }
		//pdc->modulator.chipNumber = 1; 
//init some device info
	for(ucSlaveDemod = 0; ucSlaveDemod <= (u8)pdc->bDualTs; ucSlaveDemod++)
	{
		dwError = DRV_InitDevInfo(handle, ucSlaveDemod);
	}
	
exit:
    return(dwError);
}

static u32 DRV_SetBusTuner(
	 void * handle, 
	 u16 tunerId
)
{
	u32 dwError = Error_NO_ERROR;
	u32 	 version = 0;

	PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;

	deb_data("- Enter %s Function -",__FUNCTION__);
	deb_data("tunerId =0x%x\n", tunerId);

	dwError = IT9507_getFirmwareVersion ((Modulator*) &pdc->modulator, Processor_LINK, &version);
    	if (version != 0) {
        	pdc->modulator.booted = true;
    	} 
    	else {
        	pdc->modulator.booted = false;
    	}
	if (dwError) {deb_data("Demodulator_getFirmwareVersion  error\n");}

    	return(dwError); 
}

u32 NIM_ResetSeq(IN  void *	handle)
{
	u32 dwError = Error_NO_ERROR;
	PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;
	int i;
	//u8 ucValue = 0;
	
	u8 bootbuffer[6];
	//checksum = 0xFEDC
	bootbuffer[0] = 0x05;
	bootbuffer[1] = 0x00;
	bootbuffer[2] = 0x23;
	bootbuffer[3] = 0x01;
	bootbuffer[4] = 0xFE;
	bootbuffer[5] = 0xDC;	

	//aaa
	//reset 9133 -> boot -> demod init

	//GPIOH5 init
	dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK, p_reg_top_gpioh5_en, reg_top_gpioh5_en_pos, reg_top_gpioh5_en_len, 0);
	dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK, p_reg_top_gpioh5_on, reg_top_gpioh5_on_pos, reg_top_gpioh5_on_len, 0);
		
	//dwError = IT9507_writeRegisterBits((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, p_reg_top_gpioh5_o, reg_top_gpioh5_o_pos, reg_top_gpioh5_o_len, 0);
	//mdelay(100);

	deb_data("aaa start DRV_NIMReset");
	dwError = DRV_NIMReset(handle);

	dwError = DRV_InitNIMSuspendRegs(handle);
	
	deb_data("aaa start writeGenericRegisters");
	dwError = IT9507_writeGenericRegisters ((Modulator*) &pdc->modulator, 0x3a, 0x06, bootbuffer);
	
	deb_data("aaa start readGenericRegisters");
	dwError = IT9507_readGenericRegisters ((Modulator*) &pdc->modulator, 0x3a, 0x05, bootbuffer);
	deb_data("aaa print I2C reply");
	for(i=0; i<5; i++)
		deb_data("aaa bootbuffer[%d] = 0x%x", i, bootbuffer[i]);
	
//	mdelay(50); //delay for Fw boot	
	msleep(50); //delay for Fw boot	

	//IT9507_readRegister((Demodulator*) &pdc->Demodulator, Processor_LINK, 0x4100, &ucValue);
	
	//Demod & Tuner init
	deb_data("aaa DL_Initialize");
	dwError = DRV_Initialize(handle);

	return dwError;
}


//set tuner power saving and control
static u32 DRV_ApCtrl(
	void* handle,
	u8 ucSlaveDemod,
	bool bOn)
{
	u32 dwError = Error_NO_ERROR;
	PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;
	int i;
    u32 version = 0;
	
	deb_data("- Enter %s Function -\n",__FUNCTION__);
	deb_data("ucSlaveDemod = %d, bOn = %s \n", ucSlaveDemod, bOn?"ON":"OFF");

	if(bOn) 
	{
		pdc->fc[ucSlaveDemod].tunerinfo.bTunerInited = true;
		//[OMEGA] 
		if (pdc->architecture == Architecture_PIP)
		{
			if (pdc->fc[0].bTimerOn || pdc->fc[1].bTimerOn) {				
				deb_data("Already all power on ");
				return 0;
			}
		}
	}
	else 
	{	
		pdc->fc[ucSlaveDemod].tunerinfo.bTunerInited = false;	
		pdc->fc[ucSlaveDemod].tunerinfo.bTunerLock = false;
		
		//[OMEGA] if other one still alive, do not pwr down, just need to set freq;
		if (pdc->architecture == Architecture_PIP)
		{
			if (pdc->fc[0].bTimerOn || pdc->fc[1].bTimerOn) 
			{				
				deb_data("CLOSE 1<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
				IT9507_acquireTxChannel((Modulator*) &pdc->modulator, pdc->fc[!ucSlaveDemod].ucCurrentBandWidth, pdc->fc[!ucSlaveDemod].ulCurrentFrequency);
				return 0;
			}
		}
	}
   	//[OMEGA] clock from tuner, so close sequence demod->tuner, and 9133->9137. vice versa.
   	// BUT! demod->tuner:57mADC, tuner->demod:37mADC
	if (bOn)  //pwr on
	{
		if(pdc->chip_version == 1)
		{
			deb_data("aaa Power On\n");
			if (ucSlaveDemod == 1){
				dwError = NIM_ResetSeq(pdc);
				if(dwError)
				{								
					NIM_ResetSeq(pdc);
				}
			}
			else
			{
				//aaa			
				//CAP_KdPrint(("aaa DRV_TunerSuspend chip_%d", ucSlaveDemod));
				//dwError = DRV_TunerSuspend(handle, ucSlaveDemod, !bOn);
				//dwError = DRV_TunerPowerCtrl(handle, ucSlaveDemod, bOn);
				//if(dwError) TUNER_KdPrint(("DRV_ApCtrl::DRV_TunerSuspend Fail: 0x%04X", dwError));
				//DummyCmd
				pdc->UsbCtrlTimeOut = 1;
				for(i=0; i<5 ;i++) 
				{        
					deb_data("DRV_ApCtrl::DummyCmd %d\n", i);
					dwError = IT9507_getFirmwareVersion ((Modulator*) &pdc->modulator, Processor_LINK, &version);
//					mdelay(1);
					msleep(1);
					//if (!dwError) break;
				}
				pdc->UsbCtrlTimeOut = 5;
				
				deb_data("aaa IT9507_controlTunerPowerSaving chip_%d\n", ucSlaveDemod);
	//			dwError = IT9507_controlTunerPower((Demodulator*) &pdc->Demodulator, bOn);
	//			if(dwError) deb_data("DRV_ApCtrl::IT9507_controlTunerPowerSaving error = 0x%04ld", dwError);

				deb_data("aaa IT9507_controlPowerSaving chip_%d\n", ucSlaveDemod);
				dwError = IT9507_controlPowerSaving((Modulator*) &pdc->modulator, bOn);
				if(dwError) deb_data("DRV_ApCtrl::IT9507_controlPowerSaving error = 0x%04x", dwError);
			}
		}
		else
		{
			deb_data("aaa Power On\n");
			if (ucSlaveDemod == 1){
					deb_data("aaa GPIOH5 off\n");
					dwError = DRV_NIMSuspend(handle, false);
			}
			else
			{
				//DummyCmd
				pdc->UsbCtrlTimeOut = 1;
				for(i=0; i<5 ;i++) 
				{        
					deb_data("DRV_ApCtrl::DummyCmd %d\n", i);
					dwError = IT9507_getFirmwareVersion ((Modulator*) &pdc->modulator, Processor_LINK, &version);
//					mdelay(1);
					msleep(1);					
					//if (!dwError) break;
				}
				pdc->UsbCtrlTimeOut = 5;
			}
			//aaa			
			//CAP_KdPrint(("aaa DRV_TunerSuspend chip_%d", ucSlaveDemod));
			//dwError = DRV_TunerSuspend(handle, ucSlaveDemod, !bOn);
			//dwError = DRV_TunerPowerCtrl(handle, ucSlaveDemod, bOn);
			//if(dwError) TUNER_KdPrint(("DRV_ApCtrl::DRV_TunerSuspend Fail: 0x%04x", dwError));

			deb_data("aaa IT9507_controlTunerPowerSaving chip_%d\n", ucSlaveDemod);
	//		dwError = IT9507_controlTunerPower((Demodulator*) &pdc->Demodulator, bOn);
			if(dwError) deb_data("DRV_ApCtrl::IT9507_controlTunerPowerSaving error = 0x%04x\n", dwError);

			deb_data("aaa IT9507_controlPowerSaving chip_%d\n", ucSlaveDemod);
			dwError = IT9507_controlPowerSaving((Modulator*) &pdc->modulator, bOn);
//			mdelay(50);
			msleep(50);
			if(dwError) deb_data("DRV_ApCtrl::IT9507_controlPowerSaving error = 0x%04x\n", dwError);
		}
        deb_data("aaa Power On End-----------\n");		
    }
	else //pwr down:  DCA DUT: 36(all) -> 47-159(no GPIOH5, sequence change)
	{
		deb_data("aaa Power OFF\n");
	/*	if ( (ucSlaveDemod == 0) && (pdc->Demodulator.chipNumber == 2) ){
			
			//deb_data("aaa IT9507_controlTunerLeakage for Chip_1");
			//dwError = IT9507_controlTunerLeakage((Demodulator*) &pdc->Demodulator, 1, bOn);
			//if(dwError) deb_data("DRV_ApCtrl::IT9507_controlTunerLeakage error = 0x%04x", dwError);

			//deb_data("aaa GPIOH5 on");
			//dwError = DRV_NIMSuspend(handle, true);
			
		}*/
	
		deb_data("aaa IT9507_controlPowerSaving chip_%d\n", ucSlaveDemod);
		dwError = IT9507_controlPowerSaving((Modulator*) &pdc->modulator, bOn);
		if(dwError) deb_data("DRV_ApCtrl::IT9507_controlPowerSaving error = 0x%04x\n", dwError);
		
		deb_data("aaa IT9507_controlTunerPowerSaving chip_%d", ucSlaveDemod);
//		dwError = IT9507_controlTunerPower((Demodulator*) &pdc->Demodulator, bOn);
		if(dwError) deb_data("DRV_ApCtrl::IT9507_controlTunerPowerSaving error = 0x%04x\n", dwError);
		//deb_data("aaa DRV_TunerSuspend chip_%d", ucSlaveDemod);
		//dwError = DRV_TunerSuspend(handle, ucSlaveDemod, !bOn);
		//dwError = DRV_TunerPowerCtrl(handle, ucSlaveDemod, bOn);		
		//if(dwError) deb_data("DRV_ApCtrl::DRV_TunerSuspend Fail: 0x%04x", dwError);
	
		deb_data("aaa Power OFF End-----------\n");
    }

	return(dwError);
}


static u32 DRV_TunerWakeup(
      void *     handle
)
{   
    	u32 dwError = Error_NO_ERROR;

    	PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT) handle;

	deb_data("- Enter %s Function -\n",__FUNCTION__);

	//tuner power on
	dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK,  p_reg_top_gpioh7_o, reg_top_gpioh7_o_pos, reg_top_gpioh7_o_len, 1);

    return(dwError);

}

static u32 DRV_NIMSuspend(
    void *      handle,
    bool        bSuspend

)
{
    u32 dwError = Error_NO_ERROR;

    PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT) handle;

    deb_data("- Enter DRV_NIMSuspend : bSuspend = %s\n", bSuspend ? "ON":"OFF");

    if(bSuspend) { //sleep
    	dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK, p_reg_top_gpioh5_o, reg_top_gpioh5_o_pos, reg_top_gpioh5_o_len, 1);
		if(dwError) return (dwError);
    } else {        //resume 
		dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK, p_reg_top_gpioh5_o, reg_top_gpioh5_o_pos, reg_top_gpioh5_o_len, 0);
		if(dwError) return (dwError);
    }

    return(dwError);
}

static u32 DRV_InitNIMSuspendRegs(
    void *      handle
)
{
    u32 dwError = Error_NO_ERROR;

    PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT) handle;
    deb_data("- Enter %s Function -\n",__FUNCTION__);

    dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK, p_reg_top_gpioh5_en, reg_top_gpioh5_en_pos, reg_top_gpioh5_en_len, 1);
    dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK, p_reg_top_gpioh5_on, reg_top_gpioh5_on_pos, reg_top_gpioh5_on_len, 1);
    dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK, p_reg_top_gpioh5_o, reg_top_gpioh5_o_pos, reg_top_gpioh5_o_len, 0);

    dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK, p_reg_top_pwrdw, reg_top_pwrdw_pos, reg_top_pwrdw_len, 1);

    dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK, p_reg_top_pwrdw_hwen, reg_top_pwrdw_hwen_pos, reg_top_pwrdw_hwen_len, 1);

    return(dwError);
}

static u32 DRV_NIMReset(
    void *      handle
)
{


    u32   dwError = Error_NO_ERROR;

    PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;
    deb_data("- Enter %s Function -\n",__FUNCTION__);
    //Set AF0350 GPIOH1 to 0 to reset AF0351

    dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK,  p_reg_top_gpioh1_en, reg_top_gpioh1_en_pos, reg_top_gpioh1_en_len, 1);
    dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK,  p_reg_top_gpioh1_on, reg_top_gpioh1_on_pos, reg_top_gpioh1_on_len, 1);
    dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK,  p_reg_top_gpioh1_o, reg_top_gpioh1_o_pos, reg_top_gpioh1_o_len, 0);

//    mdelay(50);
	msleep(50);

    dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK,  p_reg_top_gpioh1_o, reg_top_gpioh1_o_pos, reg_top_gpioh1_o_len, 1);

    return(dwError);
}

//************** DL_ *************//

static u32 DL_NIMSuspend(
    void *      handle,
    bool	bSuspend
)
{
	u32 dwError = Error_NO_ERROR;

	mutex_lock(&mymutex);

    dwError = DRV_NIMSuspend(handle, bSuspend);

    mutex_unlock(&mymutex);

    return (dwError);
}

static u32 DL_Initialize(
	    void *      handle
)
{
	u32 dwError = Error_NO_ERROR;
    mutex_lock(&mymutex);

    dwError = DRV_Initialize(handle);

	mutex_unlock(&mymutex);

	return (dwError); 
    
}

static u32 DL_SetBusTuner(
	 void * handle, 
	 u16 tunerId
)
{
	u32 dwError = Error_NO_ERROR;
	
	mutex_lock(&mymutex);

    dwError = DRV_SetBusTuner(handle, tunerId);

    mutex_unlock(&mymutex);

	return (dwError);

}

static u32  DL_GetEEPROMConfig(
	 void *      handle
)
{   
	u32 dwError = Error_NO_ERROR;
    mutex_lock(&mymutex);

    dwError = DRV_GetEEPROMConfig(handle);

    mutex_unlock(&mymutex);

    return(dwError);
} 

static u32 DL_TunerWakeup(
      void *     handle
)
{    
	u32 dwError = Error_NO_ERROR;
    mutex_lock(&mymutex);

    dwError = DRV_TunerWakeup(handle);

    mutex_unlock(&mymutex);
   
    	return(dwError);
}

u32 DL_TunerPowerCtrl(void* handle, u8 bPowerOn)
{
	u32 dwError = Error_NO_ERROR;
	u8    ucSlaveDemod=0;
  	PDEVICE_CONTEXT PDC = (PDEVICE_CONTEXT) handle;

    mutex_lock(&mymutex);

	deb_data("enter DL_TunerPowerCtrl:  bOn = %s\n", bPowerOn?"ON":"OFF");

/*    for (ucSlaveDemod=0; ucSlaveDemod<PDC->modulator.chipNumber; ucSlaveDemod++)
    {
    	dwError = DRV_TunerPowerCtrl(PDC, ucSlaveDemod, bPowerOn);
    	if(dwError) deb_data("  DRV_TunerPowerCtrl Fail: 0x%08x\n", dwError);
    }*/

    mutex_unlock(&mymutex);

    return (dwError);
}

u32 DL_ApPwCtrl (
	void* handle,
    bool  bChipCtl,
    bool  bOn
)
{
    u32 dwError = Error_NO_ERROR;
	u8    i = 0;
	PDEVICE_CONTEXT PDC = (PDEVICE_CONTEXT)handle;
	
	mutex_lock(&mymutex);

	deb_data("- Enter %s Function -",__FUNCTION__);
	deb_data("  chip =  %d  bOn = %s\n", bChipCtl, bOn?"ON":"OFF");

	if(bChipCtl) {    // 913x
#if 0
		if(bOn) {		// resume
			deb_data("IT9130x Power ON\n");		
			dwError = IT9507_writeRegisterBits((Modulator*) &PDC->modulator, Processor_LINK, p_reg_top_gpioh5_o, reg_top_gpioh5_o_pos, reg_top_gpioh5_o_len, 0);
			dwError = Demodulator_controlPowerSaving ((Demodulator*) &PDC->demodulator, bOn);				
			if(dwError) { 
				deb_data("ApCtrl::IT913x chip resume error = 0x%04x\n", dwError); 
				goto exit;
			}
		} else {       // suspend
			deb_data("IT9130x Power OFF\n");	
			dwError = IT9507_writeRegisterBits((Modulator*) &PDC->modulator, Processor_LINK, p_reg_top_gpioh5_o, reg_top_gpioh5_o_pos, reg_top_gpioh5_o_len, 1);
			dwError = Demodulator_controlPowerSaving ((Demodulator*) &PDC->demodulator, bOn);	
			if(dwError) { 
				deb_data("ApCtrl::IT913x chip suspend error = 0x%04x\n", dwError); 
				goto exit;
			}			
		}
#endif
	} else {          // 9507
		if(bOn) {	  // resume
			deb_data("IT950x Power ON\n");				
			dwError = IT9507_controlPowerSaving ((Modulator*) &PDC->modulator, bOn);				
			if(dwError) { 
				deb_data("ApCtrl::IT9507_controlPowerSaving error = 0x%04x\n", dwError); 
				goto exit;
			}
		} else {      // suspend
			//deb_data("IT950x TxMode RF OFF\n");				
			dwError = IT9507_setTxModeEnable((Modulator*) &PDC->modulator, 0);
			if(dwError) {
				deb_data("ApCtrl::IT9507_setTxModeEnable error = 0x%04x\n", dwError);
				goto exit;				
			}
			deb_data("IT950x Power OFF\n");							
			dwError = IT9507_controlPowerSaving ((Modulator*) &PDC->modulator, bOn);			
			if(dwError) {
				deb_data("ApCtrl::IT9507_controlPowerSaving error = 0x%04x\n", dwError);
				goto exit;
			}
		}			
	}

exit:
    mutex_unlock(&mymutex);
    	return(dwError);
}

u32 DL_ApCtrl (
	void* handle,
    u8  ucSlaveDemod,
    bool  bOn
)
{
    u32 dwError = Error_NO_ERROR;
	u8    i = 0;
	PDEVICE_CONTEXT PDC = (PDEVICE_CONTEXT)handle;
	
	mutex_lock(&mymutex);

	deb_data("- Enter %s Function -",__FUNCTION__);
	deb_data("  chip =  %d  bOn = %s\n", ucSlaveDemod, bOn?"ON":"OFF");

    if(PDC->architecture != Architecture_PIP)
    {

	//	if ( PDC->modulator.chipNumber == 2 && bOn) dwError = DL_NIMSuspend(PDC, false);

		/*for (i=0; i<PDC->modulator.chipNumber; i++)
		{	 
		   if (bOn) 
			dwError = DRV_ApCtrl (PDC, i, bOn);
		   else 
			if (PDC->bTunerPowerOff != true) dwError = DRV_ApCtrl (PDC, i, bOn);

			if(!bOn)
			{
					PDC->fc[i].ulDesiredFrequency = 0;
			PDC->fc[i].ucDesiredBandWidth = 0;
			}
		}*/

	//	if(PDC->modulator.chipNumber == 2 && !bOn) dwError = DL_NIMSuspend(PDC, true);
	}
    else
    {
		if (bOn) {

			PDC->fc[ucSlaveDemod].GraphBuilt = 1;

			if (PDC->fc[0].GraphBuilt == 0 ||  PDC->fc[1].GraphBuilt == 0)
			dwError = DL_NIMSuspend(PDC, false);

			dwError = DRV_ApCtrl (PDC, ucSlaveDemod, bOn);
		} else {

			PDC->fc[ucSlaveDemod].GraphBuilt = 0;

			if (PDC->bTunerPowerOff != true) dwError = DRV_ApCtrl (PDC, ucSlaveDemod, bOn);

			if (PDC->fc[0].GraphBuilt == 0 && PDC->fc[1].GraphBuilt == 0 && PDC->bTunerPowerOff == true)
			dwError = DL_NIMSuspend(PDC, true);
		}
    }
    mutex_unlock(&mymutex);

   	return(dwError);
}


u32 DL_CheckTunerInited(
	void* handle,
	u8 ucSlaveDemod,
	bool *bOn )
{
	u32 dwError = Error_NO_ERROR;
	PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;
	
    mutex_lock(&mymutex);

    deb_data("- Enter %s Function -\n",__FUNCTION__);

    *bOn = pdc->fc[ucSlaveDemod].tunerinfo.bTunerInited;

    mutex_unlock(&mymutex);

    return(dwError);
}

u32 DL_DemodIOCTLFun(Modulator *modulator, u32 IOCTLCode, unsigned long pIOBuffer)
{
    u32 dwError = Error_NO_ERROR;

    mutex_lock(&mymutex);

    deb_data("DemodIOCTLFun function");

    switch (IOCTLCode)
    {
        case IOCTL_ITE_DEMOD_GETDEVICETYPE_TX:
        {
            PTxGetDeviceTypeRequest pRequest = (PTxGetDeviceTypeRequest) pIOBuffer;
            pRequest->error = EagleUser_getDeviceType(modulator, &pRequest->DeviceType);
            break;
        }
        case IOCTL_ITE_DEMOD_SETDEVICETYPE_TX:
        {
            PTxSetDeviceTypeRequest pRequest = (PTxSetDeviceTypeRequest) pIOBuffer;

            if(EagleUser_setSystemConfig(modulator) != 0)
                printk("\n- EagleUser_setSystemConfig fail -\n");    
            else 
                printk("\n- EagleUser_setSystemConfig ok -\n");                
            break;
        }        
        case IOCTL_ITE_DEMOD_ADJUSTOUTPUTGAIN_TX: 
        {
            PSetGainRequest pRequest = (PSetGainRequest) pIOBuffer;
            pRequest->error = IT9507_adjustOutputGain(modulator, &pRequest->GainValue);
            break;
        }
        case IOCTL_ITE_DEMOD_ENABLETXMODE_TX:
        {
            PTxModeRequest pRequest = (PTxModeRequest) pIOBuffer;
            pRequest->error = IT9507_setTxModeEnable(modulator, pRequest->OnOff);
            deb_data("IT950x TxMode RF %s\n", pRequest->OnOff?"ON":"OFF");    
            break;
        }
        case IOCTL_ITE_DEMOD_SETMODULE_TX:
        {
            ChannelModulation temp;
            PSetModuleRequest pRequest = (PSetModuleRequest) pIOBuffer;
            temp.constellation = pRequest->constellation;
            temp.highCodeRate = pRequest->highCodeRate;
            temp.interval = pRequest->interval;
            temp.transmissionMode = pRequest->transmissionMode;
            pRequest->error = IT9507_setTXChannelModulation(modulator, &temp);
            pRequest->error = IT9507_setTxModeEnable(modulator, 1);
            deb_data("IT950x TxMode RF ON\n");                
            break;
        }
        case IOCTL_ITE_DEMOD_ACQUIRECHANNEL_TX:
        {
            PTxAcquireChannelRequest pRequest = (PTxAcquireChannelRequest) pIOBuffer;
            pRequest->error = IT9507_acquireTxChannel(modulator, pRequest->bandwidth, pRequest->frequency);
            break;
        }
        case IOCTL_ITE_DEMOD_GETFIRMWAREVERSION_TX:
        {
            PTxGetFirmwareVersionRequest pRequest = (PTxGetFirmwareVersionRequest) pIOBuffer;
            pRequest->error = IT9507_getFirmwareVersion (modulator, pRequest->processor, pRequest->version);
            break;
        }
        case IOCTL_ITE_DEMOD_CONTROLPIDFILTER_TX:
        {
            PTxControlPidFilterRequest pRequest = (PTxControlPidFilterRequest) pIOBuffer;
            pRequest->error = IT9507_controlPidFilter (modulator, pRequest->chip, pRequest->control);
            
            break;
        }
        case IOCTL_ITE_DEMOD_CONTROLPOWERSAVING:
        {
            PControlPowerSavingRequest pRequest = (PControlPowerSavingRequest) pIOBuffer;
            //DAVE: pRequest->error = Demodulator_controlPowerSaving ((Demodulator*) handle, pRequest->control);
            pRequest->error = IT9507_writeRegisterBits(modulator, Processor_LINK, p_reg_top_gpioh5_o, reg_top_gpioh5_o_pos, reg_top_gpioh5_o_len, !pRequest->control);
            break;
        }
        case IOCTL_ITE_DEMOD_CONTROLPOWERSAVING_TX:
        {
            PTxControlPowerSavingRequest pRequest = (PTxControlPowerSavingRequest) pIOBuffer;
            pRequest->error = IT9507_setTxModeEnable(modulator, pRequest->control);
            pRequest->error = IT9507_controlPowerSaving (modulator, pRequest->control);            
            break;
        }
        case IOCTL_ITE_DEMOD_WRITEGENERICREGISTERS_TX:
        {
            PWriteGenericRegistersRequest pRequest = (PWriteGenericRegistersRequest) pIOBuffer;
            pRequest->error = IT9507_writeGenericRegisters (modulator, pRequest->slaveAddress, pRequest->bufferLength, pRequest->buffer);

            break;
        }
        case IOCTL_ITE_DEMOD_READGENERICREGISTERS_TX:
        {
            PReadGenericRegistersRequest pRequest = (PReadGenericRegistersRequest) pIOBuffer;
            pRequest->error = IT9507_readGenericRegisters (modulator, pRequest->slaveAddress, pRequest->bufferLength, pRequest->buffer);

            break;
        }
        case IOCTL_ITE_DEMOD_FINALIZE:
        {
            PFinalizeRequest pRequest = (PFinalizeRequest) pIOBuffer;
            //pRequest->error = Demodulator_finalize ((Demodulator*) handle);
            //pRequest->error = Demodulator_controlPowerSaving ((Demodulator*) handle, 0);
            break;
        }
        case IOCTL_ITE_DEMOD_FINALIZE_TX:
        {
            PTxFinalizeRequest pRequest = (PTxFinalizeRequest) pIOBuffer;
            //pRequest->error = IT9507_finalize (modulator);
            //pRequest->error = IT9507_controlPowerSaving (modulator, 0);
            break;
        }
        case IOCTL_ITE_DEMOD_GETDRIVERINFO_TX:
        {
            //deb_data("IOCTL_ITE_DEMOD_GETDRIVERINFO %x, %x\n", pIOBuffer, handle);

            PTxModDriverInfo pDriverInfo = (PTxModDriverInfo)pIOBuffer;
            u32 dwFWVerionLink = 0;
            u32 dwFWVerionOFDM = 0;
            
            strcpy((char *)pDriverInfo->DriverVerion, DRIVER_RELEASE_VERSION);
            sprintf((char *)pDriverInfo->APIVerion, "%X.%X.%X.%X", (u8)(Eagle_Version_NUMBER>>8), (u8)(Eagle_Version_NUMBER), Eagle_Version_DATE, Eagle_Version_BUILD);

            IT9507_getFirmwareVersion (modulator, Processor_LINK, &dwFWVerionLink);
            sprintf((char *)pDriverInfo->FWVerionLink, "%X.%X.%X.%X", (u8)(dwFWVerionLink>>24), (u8)(dwFWVerionLink>>16), (u8)(dwFWVerionLink>>8), (u8)dwFWVerionLink);
            deb_data("Modulator_getFirmwareVersion Processor_LINK %s\n", (char *)pDriverInfo->FWVerionLink);
 
            IT9507_getFirmwareVersion (modulator, Processor_OFDM, &dwFWVerionOFDM);
            sprintf((char *)pDriverInfo->FWVerionOFDM, "%X.%X.%X.%X", (u8)(dwFWVerionOFDM>>24), (u8)(dwFWVerionOFDM>>16), (u8)(dwFWVerionOFDM>>8), (u8)dwFWVerionOFDM);
            deb_data("Modulator_getFirmwareVersion Processor_OFDM %s\n", (char *)pDriverInfo->FWVerionOFDM);

            strcpy((char *)pDriverInfo->Company, "ITEtech");
            strcpy((char *)pDriverInfo->SupportHWInfo, "Ganymede DVBT");

            pDriverInfo->error = Error_NO_ERROR;

            break;
        }
        case IOCTL_ITE_DEMOD_WRITEREGISTERS_TX:
        {
            PTxWriteRegistersRequest pRequest = (PTxWriteRegistersRequest) pIOBuffer;
            pRequest->error = IT9507_writeRegisters (modulator, pRequest->processor, pRequest->registerAddress, pRequest->bufferLength, pRequest->buffer);

            break;
        }
        case IOCTL_ITE_DEMOD_WRITEEEPROMVALUES_TX:
        {
            PWriteEepromValuesRequest pRequest = (PWriteEepromValuesRequest) pIOBuffer;
            pRequest->error = IT9507_writeEepromValues (modulator, pRequest->registerAddress, pRequest->bufferLength, pRequest->buffer);

            break;
        }
        case IOCTL_ITE_DEMOD_WRITEREGISTERBITS_TX:
        {
            PTxWriteRegisterBitsRequest pRequest = (PTxWriteRegisterBitsRequest) pIOBuffer;
            pRequest->error = IT9507_writeRegisterBits (modulator, pRequest->processor, pRequest->registerAddress, pRequest->position, pRequest->length, pRequest->value);

            break;
        }
        case IOCTL_ITE_DEMOD_READREGISTERS_TX:
        {
        //    deb_data("IOCTL_ITE_DEMOD_READREGISTERS\n");
            PTxReadRegistersRequest pRequest = (PTxReadRegistersRequest) pIOBuffer;
            pRequest->error = IT9507_readRegisters (modulator, pRequest->processor, pRequest->registerAddress, pRequest->bufferLength, pRequest->buffer);

            break;
        }
        case IOCTL_ITE_DEMOD_READEEPROMVALUES_TX:
        {
            PTxReadEepromValuesRequest pRequest = (PTxReadEepromValuesRequest) pIOBuffer;
            pRequest->error = IT9507_readEepromValues (modulator, pRequest->registerAddress, pRequest->bufferLength, pRequest->buffer);

            break;
        }
        case IOCTL_ITE_DEMOD_READREGISTERBITS_TX:
        {
            PTxReadRegisterBitsRequest pRequest = (PTxReadRegisterBitsRequest) pIOBuffer;
            pRequest->error = IT9507_readRegisterBits (modulator, pRequest->processor, pRequest->registerAddress, pRequest->position, pRequest->length, pRequest->value);

            break;
        }        
        case IOCTL_ITE_DEMOD_GETGAINRANGE_TX:
        {
            PGetGainRangeRequest pRequest = (PGetGainRangeRequest) pIOBuffer;        
            pRequest->error = IT9507_getGainRange (modulator, pRequest->frequency, pRequest->bandwidth, pRequest->maxGain, pRequest->minGain);
            break;
        }
        case IOCTL_ITE_DEMOD_GETTPS_TX:
        {
            PGetTPSRequest pRequest = (PGetTPSRequest) pIOBuffer;        
            pRequest->error = IT9507_getTPS (modulator, pRequest->pTps);
            break;
        }
        case IOCTL_ITE_DEMOD_SETTPS_TX:
        {
            PSetTPSRequest pRequest = (PSetTPSRequest) pIOBuffer;        
            pRequest->error = IT9507_setTPS (modulator, pRequest->tps);
            break;
        }
        case IOCTL_ITE_DEMOD_GETOUTPUTGAIN_TX:
        {
            PGetOutputGainRequest pRequest = (PGetOutputGainRequest) pIOBuffer;        
            pRequest->error = IT9507_getOutputGain (modulator, pRequest->gain);
            break;
        }
        case IOCTL_ITE_DEMOD_SENDHWPSITABLE_TX:
        {
            PSendHwPSITableRequest pRequest = (PSendHwPSITableRequest) pIOBuffer;        
            pRequest->error = IT9507_sendHwPSITable (modulator, pRequest->pbuffer);
            break;
        }
        case IOCTL_ITE_DEMOD_ACCESSFWPSITABLE_TX:
        {
            PAccessFwPSITableRequest pRequest = (PAccessFwPSITableRequest) pIOBuffer;        
            pRequest->error = IT9507_accessFwPSITable (modulator, pRequest->psiTableIndex, pRequest->pbuffer);
            break;
        }        
        case IOCTL_ITE_DEMOD_SETFWPSITABLETIMER_TX:
        {
            PSetFwPSITableTimerRequest pRequest = (PSetFwPSITableTimerRequest) pIOBuffer;        
            pRequest->error = IT9507_setFwPSITableTimer (modulator, pRequest->psiTableIndex, pRequest->timer);
            break;
        }        
        case IOCTL_ITE_DEMOD_SETIQTABLE_TX:
        {
            PTxSetIQTableRequest pRequest = (PTxSetIQTableRequest) pIOBuffer;        
            pRequest->error = IT9507_setIQtable (modulator, pRequest->IQ_table_ptr, pRequest->tableGroups);
            printk("Set IQtable group length is %d\n", pRequest->tableGroups);
            break;
        }            

        case IOCTL_ITE_DEMOD_SETDCCALIBRATIONVALUE_TX:
        {
            PTxSetDCCalibrationValueRequest pRequest = (PTxSetDCCalibrationValueRequest) pIOBuffer;        
            pRequest->error = IT9507_setDCCalibrationValue (modulator, pRequest->dc_i, pRequest->dc_q);
            printk("Set DC Calibration dc_i %d, dc_q %d\n", pRequest->dc_i, pRequest->dc_q);
            break;
        }
        
        default:
            //DBGPRINT(MP_WARN, (TEXT("\nDemodIOCTLFun Oid 0x%08x\n"), IOCTLCode));
            //DBGPRINT(MP_WARN, (TEXT("DemodInformation--->[not suported!!!]\n")));
            printk("NOT IMPLETEMENTED IOCONTROL!\n");
            dwError = ENOTTY;
    }

    mutex_unlock(&mymutex);

    return(dwError);
}

u32 Device_init(struct usb_device *udev, PDEVICE_CONTEXT PDC, bool bBoot)
{
	u32 error = Error_NO_ERROR;
	u8 filterIdx=0;
	int errcount=0;

	PDC->modulator.userData = (void *)udev;
	dev_set_drvdata(&udev->dev, PDC);

	deb_data("- Enter %s Function -\n",__FUNCTION__);

        printk("        DRIVER_RELEASE_VERSION  : %s\n", DRIVER_RELEASE_VERSION);
	printk("        EAGLE_FW_RELEASE_LINK_VERSION: %d.%d.%d.%d\n", DVB_LL_VERSION1, DVB_LL_VERSION2, DVB_LL_VERSION3, DVB_LL_VERSION4);
	printk("        EAGLE_FW_RELEASE_OFDM_VERSION: %d.%d.%d.%d\n", DVB_OFDM_VERSION1, DVB_OFDM_VERSION2, DVB_OFDM_VERSION3, DVB_OFDM_VERSION4);	
	printk("        API_TX_RELEASE_VERSION  : %X.%X.%X\n", Eagle_Version_NUMBER, Eagle_Version_DATE, Eagle_Version_BUILD);


	//************* Set Device init Info *************//
	PDC->bEnterSuspend = false;
    	PDC->bSurpriseRemoval = false;
    	PDC->bDevNotResp = false;
    	PDC->bSelectiveSuspend = false; 
	PDC->bTunerPowerOff = false;

	if (bBoot)
	{
		PDC->bSupportSelSuspend = false;
//		PDC->modulator.userData = (Handle)PDC;
//		PDC->modulator.chipNumber = 1; 
//		PDC->demodulator.userData = (Handle)PDC;
		PDC->architecture=Architecture_DCA;
		PDC->modulator.frequency = 666000;
		PDC->modulator.bandwidth = 8000;
		PDC->fc[0].tunerinfo.TunerId = 0;
		PDC->fc[1].tunerinfo.TunerId = 0;
		PDC->bDualTs=false;	
        	PDC->FilterCnt = 0;
		PDC->UsbCtrlTimeOut = 1;
	}
	else {
        	PDC->UsbCtrlTimeOut = 5;
    	}//bBoot

	if (bBoot) {
		//************* Set USB Info *************//
		PDC->MaxPacketSize = 0x0200; //default USB2.0
		PDC->UsbMode = (PDC->MaxPacketSize == 0x200)?0x0200:0x0110;  
		deb_data("USB mode= 0x%x\n", PDC->UsbMode);

		PDC->TsPacketCount = (PDC->UsbMode == 0x200)?TS_PACKET_COUNT_HI:TS_PACKET_COUNT_FU;
		PDC->TsFrames = (PDC->UsbMode == 0x200)?TS_FRAMES_HI:TS_FRAMES_FU;
		PDC->TsFrameSize = TS_PACKET_SIZE*PDC->TsPacketCount;
		PDC->TsFrameSizeDw = PDC->TsFrameSize/4;
	}
	PDC->bEP12Error = false;
    	PDC->bEP45Error = false; 
    	PDC->ForceWrite = false;    
    	PDC->ulActiveFilter = 0;
	
	if(bBoot)
    	{
		//patch for eeepc
        	//error = DL_SetBusTuner (PDC, Bus_USB, 0x38);
        	//PDC->UsbCtrlTimeOut = 5;
        
        	error = DL_SetBusTuner (PDC, 0x38);
        	if (error)
        	{ 
            		deb_data("First DL_SetBusTuner fail : 0x%08x\n",error );
			errcount++;
            		goto Exit; 
        	}

        	error =DL_GetEEPROMConfig(PDC);
        	if (error)
        	{
            		deb_data("DL_GetEEPROMConfig fail : 0x%08x\n", error);
			errcount++;
            		goto Exit;
        	}
	}//bBoot
	
	error = DL_SetBusTuner(PDC, PDC->fc[0].tunerinfo.TunerId);
	
    	if (error)
    	{
        	deb_data("DL_SetBusTuner fail!\n");
		errcount++;
        	goto Exit;
    	}

	
	 /*if (PDC->modulator.chipNumber == 2 && !PDC->modulator.booted) //plug/cold-boot/S4
    	{
        	error = DL_NIMReset(PDC);            
    	}
    	else if(PDC->modulator.chipNumber == 2 && PDC->modulator.booted) //warm-boot/(S1)
    	{
        	error = DL_NIMSuspend(PDC, false); 
		error = DL_TunerWakeup(PDC); //actually for mt2266
    	}*/
	
	
	//if(PDC->modulator.chipNumber == 1 && PDC->modulator.booted) //warm-boot/(S1)
	if(PDC->modulator.booted) //warm-boot/(S1)
	{
		error = DL_TunerWakeup(PDC);
	}
	if(error) deb_data("DL_NIMReset or DL_NIMSuspend or DL_TunerWakeup fail!\n"); 

	error = DL_Initialize(PDC);
	if (error) {
		deb_data("DL_Initialize fail! 0x%08x\n", error);
		errcount++;
		goto Exit;
	}

	deb_data("	%s success \n",__FUNCTION__);


	error = IT9507_writeRegister((Modulator*) &PDC->modulator, Processor_OFDM, 0xF7C6, 0x1);
	if(error)	printk( "AirHD Reg Write fail!\n");
	else printk( "AirHD Reg Write ok!\n");
	
Exit:
	
	if(errcount)
        printk( "[Device_init] Error %d\n", errcount);
	return (error);
}
