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
#include "modulatorRegister.h"
#include "modulatorVariable.h"
#include "modulatorVersion.h"
#include "IQ_fixed_table.h"

/* LINK processor firmware extracted from modulatorFirmware.h in ITE driver 
   md5sum should be b4217064db5436e8c3ade7c6e982d12e 
 */
#define IT9507_FIRMWARE "dvb-mod-it9507.fw"

static u8 IT9507Cmd_sequence = 0;

/* These were hard-coded in the ITE driver */
#define DVB_OFDM_VERSION1 255
#define DVB_OFDM_VERSION2 9
#define DVB_OFDM_VERSION3 8
#define DVB_OFDM_VERSION4 0
#define DVB_OFDM_VERSION ((u32)0xff090800)

/* From modulatorUser.h */
#define EagleUser_MAX_PKT_SIZE               255
#define EagleUser_RETRY_MAX_LIMIT            10

/** Define I2C master speed, the default value 0x07 means 366KHz (1000000000 / (24.4 * 16 * EagleUser_I2C_SPEED)). */
#define EagleUser_IIC_SPEED              0x07

/** Define I2C address of secondary chip when Diversity mode or PIP mode is active. */
#define EagleUser_IIC_ADDRESS            0x38
#define EagleUser_SlaveIIC_ADDRESS       0x3A
#define EagleUser_DEVICETYPE			 0

/** Define USB frame size */
#define EagleUser_USB20_MAX_PACKET_SIZE      512
#define EagleUser_USB20_FRAME_SIZE           (188 * 348)
#define EagleUser_USB20_FRAME_SIZE_DW        (EagleUser_USB20_FRAME_SIZE / 4)
#define EagleUser_USB11_MAX_PACKET_SIZE      64
#define EagleUser_USB11_FRAME_SIZE           (188 * 21)
#define EagleUser_USB11_FRAME_SIZE_DW        (EagleUser_USB11_FRAME_SIZE / 4)
#define EagleUser_MAXFRAMESIZE			63


static u32 IT9507Cmd_addChecksum (
    IN  struct it950x_state*    state,
    OUT u32*          bufferLength,
    OUT u8*           buffer
) {
    u32 error  = ModulatorError_NO_ERROR;
    u32 loop   = (*bufferLength - 1) / 2;
    u32 remain = (*bufferLength - 1) % 2;
    u32 i;
    u16  checksum = 0;

	if(state == NULL)
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
    IN  struct it950x_state*    state,
    OUT u32*          bufferLength,
    OUT u8*           buffer
) {
    u32 error    = ModulatorError_NO_ERROR;
    u32 loop     = (*bufferLength - 3) / 2;
    u32 remain   = (*bufferLength - 3) % 2;
    u32 i;
    u16  checksum = 0;
	if(state == NULL)
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

static u32 EagleUser_busTx (
    IN  struct it950x_state*    state,
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
	ret = usb_bulk_msg(usb_get_dev( state->udev),
			usb_sndbulkpipe(usb_get_dev(state->udev), 0x02),
			pTmpBuffer,
			bufferLength,
			&act_len,
			1000000);
   
	if (ret) deb_data(" Usb2_writeControlBus fail : 0x%08x\n", ret);

	return (Error_NO_ERROR);
}


static u32 EagleUser_busRx (
    IN  struct it950x_state*    state,
    IN  u32           bufferLength,
    OUT u8*           buffer
) {
	u32     ret;
	int       nu8sRead;
	u8 *pTmpBuffer = kzalloc(sizeof(buffer)*bufferLength, GFP_KERNEL);
	ret = 0;

//deb_data(" ---------Usb2_readControlBus----------\n", ret);			
   ret = usb_bulk_msg(usb_get_dev(state->udev),
				usb_rcvbulkpipe(usb_get_dev(state->udev),129),
				pTmpBuffer,
				bufferLength,
				&nu8sRead,
				1000000);
	if (pTmpBuffer)
		memcpy(buffer, pTmpBuffer, bufferLength);   
	 
	if (ret) 	deb_data(" Usb2_readControlBus fail : 0x%08x\n", ret);

	return (Error_NO_ERROR);
}

static u32 it950x_io (
    struct it950x_state*    state,
    Processor     processor,
    int cmd,
    u32         registerAddress,
    u8          ioBufferLength,
    u8*         ioBuffer
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
	
    if (ioBufferLength == 0) goto exit;

    if (registerAddressLength > 4) {
        error  = ModulatorError_PROTOCOL_FORMAT_INVALID;
        goto exit;
    }

    if (cmd == Command_REG_DEMOD_WRITE) {
      if ((ioBufferLength + 12) > maxFrameSize) {
        error = ModulatorError_INVALID_DATA_LENGTH;
        goto exit;
      }
    } else if (cmd == Command_REG_DEMOD_READ) {
      if ((ioBufferLength + 5) > EagleUser_MAX_PKT_SIZE) {
        error = ModulatorError_INVALID_DATA_LENGTH;
        goto exit;
      }
      if ((ioBufferLength + 5) > maxFrameSize) {
        error = ModulatorError_INVALID_DATA_LENGTH;
        goto exit;
      }
    }


    /** add frame header */
    command   = IT9507Cmd_buildCommand (cmd, processor);
    //buffer[0] is the total length of the command packet, excluding this byte (filled in by addChecksum)
    buffer[1] = (u8) (command >> 8);
    buffer[2] = (u8) command;
    buffer[3] = (u8) IT9507Cmd_sequence++;
    buffer[4] = (u8) ioBufferLength;
    buffer[5] = (u8) registerAddressLength;
    buffer[6] = (u8) ((registerAddress) >> 24); /** Get first byte of reg. address  */
    buffer[7] = (u8) ((registerAddress) >> 16); /** Get second byte of reg. address */
    buffer[8] = (u8) ((registerAddress) >> 8);  /** Get third byte of reg. address  */
    buffer[9] = (u8) (registerAddress );        /** Get fourth byte of reg. address */

    bufferLength = 10;
    if ((cmd == Command_REG_DEMOD_WRITE) || (cmd == Command_GENERIC_WRITE)) {
      /** add frame data */
      memcpy(buffer+10,ioBuffer,ioBufferLength);

      bufferLength += ioBufferLength;
    } 

    /** add frame check-sum */
    error = IT9507Cmd_addChecksum (state, &bufferLength, buffer);
    if (error) goto exit;    

    /** send frame */
    i = 0;
    sendLength = 0;
    remainLength = bufferLength;
    while (remainLength > 0) {
        i     = (remainLength > EagleUser_MAX_PKT_SIZE) ? (EagleUser_MAX_PKT_SIZE) : (remainLength);
		for (cnt = 0; cnt < EagleUser_RETRY_MAX_LIMIT; cnt++) {
			error = EagleUser_busTx (state, i, &buffer[sendLength]);
			if (error == 0) break;
			msleep(1);
		}
        if (error) goto exit;

        sendLength   += i;
        remainLength -= i;
    }

    /** get reply frame */
    if (cmd == Command_REG_DEMOD_WRITE) {      
      bufferLength = 5;
    } else if (cmd == Command_REG_DEMOD_READ) {
      bufferLength = 5 + ioBufferLength;
    }
    
	for (cnt = 0; cnt < EagleUser_RETRY_MAX_LIMIT; cnt++) {
		error = EagleUser_busRx (state, bufferLength, buffer);
		if (error == 0) break;
		msleep(1);
	}
    if (error) goto exit;

    /** remove check-sum from reply frame */
    error = IT9507Cmd_removeChecksum (state, &bufferLength, buffer);
    if (error) goto exit;

    if (cmd == Command_REG_DEMOD_READ) {
      memcpy(ioBuffer,buffer+3,ioBufferLength);
    }

exit :
   
	
return (error);
}

static u32 it950x_wr_regs (
    IN  struct it950x_state*    state,
    IN  Processor     processor,
    IN  u32         registerAddress,
    IN  u8          writeBufferLength,
    IN  u8*         writeBuffer
) {
  return it950x_io(state,processor,Command_REG_DEMOD_WRITE,registerAddress,writeBufferLength,writeBuffer);
}

static u32 it950x_wr_reg (
    IN  struct it950x_state*    state,
    IN  Processor       processor,
    IN  u32           registerAddress,
    IN  u8            value
) {
  return (it950x_io(state,processor,Command_REG_DEMOD_WRITE,registerAddress, 1, &value));
}

static u32 it950x_rd_regs (
    IN  struct it950x_state*    state,
    IN  Processor       processor,
    IN  u32           registerAddress,
    IN  u8            readBufferLength,
    OUT u8*           readBuffer
) {
  return it950x_io(state,processor,Command_REG_DEMOD_READ,registerAddress,readBufferLength,readBuffer);
}

static u32 it950x_rd_reg (
    IN  struct it950x_state*    state,
    IN  Processor       processor,
    IN  u32           registerAddress,
    OUT u8*           value
) {
  return it950x_io(state,processor,Command_REG_DEMOD_READ,registerAddress,1,value);
}


static u32 it950x_wr_regbits (
    IN  struct it950x_state*    state,
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
		error = it950x_wr_regs (state, processor, registerAddress, 1, &value);
		
	} else {
		error = it950x_rd_regs (state, processor, registerAddress, 1, &temp);
		if (error) goto exit;
		

		temp = (u8)REG_CREATE (value, temp, position, length);

		error = it950x_wr_regs (state, processor, registerAddress, 1, &temp);
		if (error) goto exit;
		
	}
exit:

	return (error);
}


static u32 IT9507Cmd_sendCommand (
    IN  struct it950x_state*    state,
    IN  u16            command,
    IN  Processor       processor,
    IN  u32           writeBufferLength,
    IN  const u8*           writeBuffer,
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
        error = IT9507Cmd_addChecksum (state, &bufferLength, buffer);
        if (error) goto exit;

        // send command packet
        i = 0;
        sendLength = 0;
        remainLength = bufferLength;
        while (remainLength > 0) {
            i = (remainLength > EagleUser_MAX_PKT_SIZE) ? (EagleUser_MAX_PKT_SIZE) : (remainLength);        

			for (cnt = 0; cnt < EagleUser_RETRY_MAX_LIMIT; cnt++) {
				error = EagleUser_busTx (state, i, &buffer[sendLength]);
				if (error == 0) break;
				msleep(1);
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
        error = IT9507Cmd_addChecksum (state, &bufferLength, buffer);
        if (error) goto exit;

        
        /** send command */
        i = 0;
        sendLength = 0;
        remainLength = bufferLength;
        while (remainLength > 0) {
            i     = (remainLength > EagleUser_MAX_PKT_SIZE) ? (EagleUser_MAX_PKT_SIZE) : (remainLength);        

			for (cnt = 0; cnt < EagleUser_RETRY_MAX_LIMIT; cnt++) {
				error = EagleUser_busTx (state, i, &buffer[sendLength]);
				if (error == 0) break;
				msleep(1);
			}
            if (error) goto exit;

            sendLength   += i;
            remainLength -= i;
        }
    }

    bufferLength = 5 + readBufferLength;

	for (cnt = 0; cnt < EagleUser_RETRY_MAX_LIMIT; cnt++) {
		error = EagleUser_busRx (state, bufferLength, buffer);
		if (error == 0) break;
		msleep(1);
	}
    if (error) goto exit;

    error = IT9507Cmd_removeChecksum (state, &bufferLength, buffer);
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
    IN  struct it950x_state*    state,
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
	return (IT9507Cmd_sendCommand (state, Command_GENERIC_WRITE, Processor_LINK, bufferLength + 3, writeBuffer, 0, NULL));
}


static u32 IT9507_readGenericRegisters (
    IN  struct it950x_state*    state,
    IN  u8            slaveAddress,
    IN  u8            bufferLength,
    IN  u8*           buffer
) {
    u8 writeBuffer[3];

	writeBuffer[0] = bufferLength;
	writeBuffer[1] = 2;
	writeBuffer[2] = slaveAddress;

	return (IT9507Cmd_sendCommand (state, Command_GENERIC_READ, Processor_LINK, 3, writeBuffer, bufferLength, buffer));
}




static u32 EagleUser_setSystemConfig (
    IN  struct it950x_state*    state
) {
	u32 error = 0;

        /* restSlave */	
	error = it950x_wr_reg (state, Processor_LINK, GPIO_O(GPIOH1), 1);
	if (error) goto exit;
	error = it950x_wr_reg (state, Processor_LINK, GPIO_EN(GPIOH1), 1);
	if (error) goto exit;
	error = it950x_wr_reg (state, Processor_LINK, GPIO_ON(GPIOH1), 1);
	if (error) goto exit;
	msleep(10);

        /* powerDownSlave */
	error = it950x_wr_reg (state, Processor_LINK, GPIO_O(GPIOH5), 0);
	if (error) goto exit;
	error = it950x_wr_reg (state, Processor_LINK, GPIO_EN(GPIOH5), 1);
	if (error) goto exit;
	error = it950x_wr_reg (state, Processor_LINK, GPIO_ON(GPIOH5), 1);
	if (error) goto exit;

        /* rfEnable */
	error = it950x_wr_reg (state, Processor_LINK, GPIO_EN(GPIOH2), 1);
	if (error) goto exit;
	error = it950x_wr_reg (state, Processor_LINK, GPIO_ON(GPIOH2), 1);
	if (error) goto exit;

        /* uvFilter */
	error = it950x_wr_reg (state, Processor_LINK, GPIO_EN(GPIOH8), 1);
	if (error) goto exit;
	error = it950x_wr_reg (state, Processor_LINK, GPIO_ON(GPIOH8), 1);
	if (error) goto exit;

exit:
    return (ModulatorError_NO_ERROR);
}


static u32 EagleUser_getDeviceType (
	IN  struct it950x_state*    state,
	OUT  u8*		  deviceType	   
){	
	u32 error = ModulatorError_NO_ERROR;
	u8 temp;

	
	error = it950x_rd_reg (state, Processor_LINK, 0x4979, &temp);//has eeprom ??
	if((temp == 1) && (error == ModulatorError_NO_ERROR)){
		error = it950x_rd_reg (state, Processor_LINK, 0x49D5, &temp);	
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



static u32 EagleUser_mpegConfig (
    IN  struct it950x_state*    state
) {
    /*
     *  ToDo:  Add code here
     *
     */
    return (ModulatorError_NO_ERROR);
}


 u32 EagleUser_Initialization  (
    IN  struct it950x_state*    state
) {
	/*
     *  ToDo:  Add code here
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
	u32 error = 0;
	 error = EagleUser_setSystemConfig(state);
	 if (error) goto exit;

	// RF Enable
	error = it950x_wr_reg (state, Processor_LINK, GPIO_O(GPIOH2), 0); //RF out power down
	if (error) goto exit;
exit:
    return (error);

 }


static u32 EagleUser_acquireChannel (
	IN  struct it950x_state*    state,
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
	  error = it950x_wr_reg (state, Processor_LINK, GPIO_O(GPIOH8), 0);  /* uvFilter */
		if (error) goto exit;

	}else{
	  error = it950x_wr_reg (state, Processor_LINK, GPIO_O(GPIOH8), 1); /* uvFilter */
		if (error) goto exit;
	}	
exit:
	return (error);
}

static u32 EagleUser_setTxModeEnable (
	IN  struct it950x_state*            state,
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
			error = it950x_wr_reg (state, Processor_LINK, GPIO_O(GPIOH2), 1); //RF power up 
			if (error) goto exit;
	}else{
			error = it950x_wr_reg (state, Processor_LINK, GPIO_O(GPIOH2), 0); //RF power down 
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
	struct it950x_state*    state,
    int fIn, 
    int *ptrdAmp, 
	int *ptrdPhi
	)
{
  // Using binary search to find the frequency interval in the table
	u16 TABLE_NROW = state->calibrationInfo.tableGroups;
	u32   error = ModulatorError_NO_ERROR;
    int idx = TABLE_NROW/2;
    int preIdx = -1;
    int lower = 0;
    int upper = TABLE_NROW;
	int outdAmp;
    int outdPhi;
	int diff;


	int temp1,temp2,temp3,temp4;

    while( ! ( (fIn - (int)(state->calibrationInfo.ptrIQtableEx[idx].frequency)) >= 0 && (fIn - (int)(state->calibrationInfo.ptrIQtableEx[idx+1].frequency) < 0) ) ){
	  if((fIn - (int)state->calibrationInfo.ptrIQtableEx[idx].frequency)==0)
		  break;
      preIdx = idx;
      if(fIn - (int)(state->calibrationInfo.ptrIQtableEx[idx+1].frequency) >= 0){
        idx = (preIdx + upper)/2;
        lower = preIdx;
      }else if(fIn - (int)(state->calibrationInfo.ptrIQtableEx[idx].frequency) < 0){
        idx = (preIdx + lower)/2;
        upper = preIdx;
      }

	  if(lower == TABLE_NROW - 1)
		  break;
    }

	if((fIn != (int)state->calibrationInfo.ptrIQtableEx[idx].frequency) && 
		((state->calibrationInfo.ptrIQtableEx[idx+1].frequency - state->calibrationInfo.ptrIQtableEx[idx].frequency)>100000)){
		// out of Calibration range
			error = ModulatorError_OUT_OF_CALIBRATION_RANGE;
			goto exit;
	}

    //printf("Found: %f \t %f\n", IQ_table[idx][0], IQ_table[idx+1][0]);
  // Perform linear interpolation

//--------test
	temp1 = state->calibrationInfo.ptrIQtableEx[idx].dAmp;
	temp2 = (fIn - (int)state->calibrationInfo.ptrIQtableEx[idx].frequency);
	temp3 = (state->calibrationInfo.ptrIQtableEx[idx+1].dAmp - state->calibrationInfo.ptrIQtableEx[idx].dAmp);
	temp4 = (state->calibrationInfo.ptrIQtableEx[idx+1].frequency - state->calibrationInfo.ptrIQtableEx[idx].frequency);
	
	outdAmp = temp1 + temp2*temp3/temp4;
	
	//test	

	//outdAmp = state->calibrationInfo.ptrIQtableEx[idx].dAmp + ((fIn - (int)state->calibrationInfo.ptrIQtableEx[idx].frequency) 
	//	       * (state->calibrationInfo.ptrIQtableEx[idx+1].dAmp - state->calibrationInfo.ptrIQtableEx[idx].dAmp)) / (state->calibrationInfo.ptrIQtableEx[idx+1].frequency - state->calibrationInfo.ptrIQtableEx[idx].frequency);
	diff = (state->calibrationInfo.ptrIQtableEx[idx+1].dPhi - state->calibrationInfo.ptrIQtableEx[idx].dPhi);
	if(diff <= -8192) {
		diff = diff+16384;
	} else if(diff >= 8192) {
		diff = diff-16384;
	}
	//outdPhi = IQ_table[idx][2] + (fIn - IQ_table[idx][0]) * (IQ_table[idx+1][2] - IQ_table[idx][2]) / (IQ_table[idx+1][0] - IQ_table[idx][0]);
			
	//------test---------
	temp1 = state->calibrationInfo.ptrIQtableEx[idx].dPhi;
	temp2 = (fIn - (int)state->calibrationInfo.ptrIQtableEx[idx].frequency);
	temp3 = diff;
	temp4 = (state->calibrationInfo.ptrIQtableEx[idx+1].frequency - state->calibrationInfo.ptrIQtableEx[idx].frequency);
	outdPhi = temp1 + temp2*temp3/temp4;
	//----------------

	
	//outdPhi = state->calibrationInfo.ptrIQtableEx[idx].dPhi + ((fIn - (int)state->calibrationInfo.ptrIQtableEx[idx].frequency) * diff) / (state->calibrationInfo.ptrIQtableEx[idx+1].frequency - state->calibrationInfo.ptrIQtableEx[idx].frequency);
	
		
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
	IN  struct it950x_state*    state,
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

    error = interpolation(state, frequency, ptrdAmp, ptrdPhi);
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
  error = it950x_wr_regs(state, Processor_OFDM, reg, 6, val);

  return (error);
}


static u32 EagleTuner_calIQCalibrationValue(
	IN  struct it950x_state*    state,
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

    error = interpolation(state, frequency, ptrdAmp, ptrdPhi);
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
  //error = Eagle_writeRegisters(state, Processor_OFDM, reg, 6, val);
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

static u32 IT9507_setTsInterface (IN  struct it950x_state*    state);

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


static u32 IT9507Cmd_reboot (
    IN  struct it950x_state*    state
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
    error = IT9507Cmd_addChecksum (state, &bufferLength, buffer);
    if (error) goto exit;
   
	for (cnt = 0; cnt < EagleUser_RETRY_MAX_LIMIT; cnt++) {
		error = EagleUser_busTx (state, bufferLength, buffer);
		if (error == 0) break;
		msleep(1);
	}
    if (error) goto exit;

exit :
    
    return (error);
}

static u32 IT9507_calOutputGain (
	IN  struct it950x_state*    state,
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
	
	if(state == NULL){
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


static u32 IT9507_selectBandwidth (
	IN  struct it950x_state*    state,
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
			error = it950x_wr_reg (state, Processor_LINK, 0xD812, 3);
			if (error) goto exit;
			break;

       	case 1500:              /** 1.5M */
			temp1 = 0x6E;	//0xFBB6
			temp2 = 0x03;	//0xFBB7
			temp3 = 0x01;	//0xFBB8
			temp4 = 0x03;	//0xD814
			temp5 = 0x00;	//0xF741
			error = it950x_wr_reg (state, Processor_LINK, 0xD812, 3);
			if (error) goto exit;
			break; 

		case 2000:              /** 2M */
			temp1 = 0x5E;	//0xFBB6
			temp2 = 0x01;	//0xFBB7
			temp3 = 0x01;	//0xFBB8
			temp4 = 0x03;	//0xD814
			temp5 = 0x00;	//0xF741
			error = it950x_wr_reg (state, Processor_LINK, 0xD812, 3);
			if (error) goto exit;			
			break;

		case 2500:              /** 2M */
			temp1 = 0x66;	//0xFBB6
			temp2 = 0x01;	//0xFBB7
			temp3 = 0x01;	//0xFBB8
			temp4 = 0x03;	//0xD814
			temp5 = 0x00;	//0xF741
			error = it950x_wr_reg (state, Processor_LINK, 0xD812, 3);
			if (error) goto exit;			
			break;

		case 3000:              /** 3M */
			temp1 = 0x6E;	//0xFBB6
			temp2 = 0x01;	//0xFBB7
			temp3 = 0x01;	//0xFBB8
			temp4 = 0x03;	//0xD814
			temp5 = 0x00;	//0xF741
			error = it950x_wr_reg (state, Processor_LINK, 0xD812, 3);
			if (error) goto exit;			
			break;

		case 4000:              /** 4M */
			temp1 = 0x5E;	//0xFBB6
			temp2 = 0x01;	//0xFBB7
			temp3 = 0x01;	//0xFBB8
			temp4 = 0x03;	//0xD814
			temp5 = 0x01;	//0xF741
			error = it950x_wr_reg (state, Processor_LINK, 0xD812, 3);
			if (error) goto exit;			
			break;

		case 5000:              /** 5M */
			temp1 = 0x66;	//0xFBB6
			temp2 = 0x01;	//0xFBB7
			temp3 = 0x01;	//0xFBB8
			temp4 = 0x03;	//0xD814
			temp5 = 0x01;	//0xF741
			error = it950x_wr_reg (state, Processor_LINK, 0xD812, 3);
			if (error) goto exit;			
			break;

		case 6000:              /** 6M */
			temp1 = 0x6E;	//0xFBB6
			temp2 = 0x01;	//0xFBB7
			temp3 = 0x01;	//0xFBB8
			temp4 = 0x03;	//0xD814
			temp5 = 0x01;	//0xF741
			error = it950x_wr_reg (state, Processor_LINK, 0xD812, 3);
			if (error) goto exit;
			break;

		case 7000:              /** 7M */
			temp1 = 0x76;	//0xFBB6
			temp2 = 0x02;	//0xFBB7
			temp3 = 0x01;	//0xFBB8
			temp4 = 0x03;	//0xD814
			temp5 = 0x02;	//0xF741
			error = it950x_wr_reg (state, Processor_LINK, 0xD812, 3);
			if (error) goto exit;			
			break;

		case 8000:              /** 8M */
			temp1 = 0x1E;	//0xFBB6
			temp2 = 0x02;	//0xFBB7
			temp3 = 0x01;	//0xFBB8
			temp4 = 0x03;	//0xD814
			temp5 = 0x02;	//0xF741
			error = it950x_wr_reg (state, Processor_LINK, 0xD812, 3);
			if (error) goto exit;
			break;

		default:
			
			error = ModulatorError_INVALID_BW;
			break;
	}

	if(error == ModulatorError_NO_ERROR){
		error = it950x_wr_reg (state, Processor_OFDM, 0xFBB6, temp1);
		if (error) goto exit;

		error = it950x_wr_regbits (state, Processor_OFDM, 0xFBB7, 0, 2, temp2);
		if (error) goto exit;

		error = it950x_wr_regbits (state, Processor_OFDM, 0xFBB8, 2, 1, temp3);
		if (error) goto exit;

		error = it950x_wr_reg (state,  Processor_OFDM, 0xF741, temp5);
		if (error) goto exit;

		error = it950x_wr_reg (state, Processor_LINK, 0xD814, temp4);
		if (error) goto exit;

		error = it950x_wr_regbits (state, Processor_OFDM, 0xFBB8, 2, 1, 0);
		if (error) goto exit;
		

	}	

	

exit :
	if(error)
		state->bandwidth = 0;
	else
		state->bandwidth = bandwidth;
	return (error);
}

static u32 IT9507_runTxCalibration (
	IN  struct it950x_state*    state,
	IN  u16            bandwidth,
    IN  u32           frequency
){
	u32 error = ModulatorError_NO_ERROR;
	u8 c1_default_value[2],c2_default_value[2],c3_default_value[2];

	if((bandwidth !=0) && (frequency !=0)){
		error = EagleTuner_setIQCalibration(state,frequency);		
	}else{
		error = ModulatorError_FREQ_OUT_OF_RANGE;
		goto exit;
	}
	if (error) goto exit;
	error = it950x_rd_regs (state, Processor_OFDM, p_eagle_reg_iqik_c1_7_0, 2, c1_default_value);
	if (error) goto exit;
	error = it950x_rd_regs (state, Processor_OFDM, p_eagle_reg_iqik_c2_7_0, 2, c2_default_value);
	if (error) goto exit;
	error = it950x_rd_regs (state, Processor_OFDM, p_eagle_reg_iqik_c3_7_0, 2, c3_default_value);
	if (error) goto exit;
	
	state->calibrationInfo.c1DefaultValue = c1_default_value[1]<<8 | c1_default_value[0];
	state->calibrationInfo.c2DefaultValue = c2_default_value[1]<<8 | c2_default_value[0];
	state->calibrationInfo.c3DefaultValue = c3_default_value[1]<<8 | c3_default_value[0];
	state->calibrationInfo.outputGain = 0;

exit:
	return (error);
}

static u32 IT9507_setFrequency (
	IN  struct it950x_state*    state,
	IN  u32           frequency
) {
	u32 error = ModulatorError_NO_ERROR;
	
	unsigned int tmp;
	u8 freq_code_H,freq_code_L;
	u16 TABLE_NROW = state->calibrationInfo.tableGroups;
	
	if(state->calibrationInfo.ptrIQtableEx[TABLE_NROW-1].frequency<frequency 
		|| state->calibrationInfo.ptrIQtableEx[0].frequency>frequency 
	){
		error = ModulatorError_FREQ_OUT_OF_RANGE;
		goto exit;
	}
	
	/*----- set_lo_freq -----*/
	tmp = IT9507_getLoFreq(frequency);
	freq_code_L = (unsigned char) (tmp & 0xFF);
	freq_code_H = (unsigned char) ((tmp >> 8) & 0xFF);

	error = it950x_wr_reg (state, Processor_OFDM, 0xFB2A, freq_code_L);
	if (error) goto exit;
	error = it950x_wr_reg (state,  Processor_OFDM, 0xFB2B, freq_code_H);
	if (error) goto exit;

	if(frequency>950000)
		error = it950x_wr_regbits (state, Processor_OFDM, 0xFB2C, 2, 1,1);
	else
		error = it950x_wr_regbits (state, Processor_OFDM, 0xFB2C, 2, 1,0);
	if (error) goto exit;

	error = it950x_wr_reg (state, Processor_OFDM, 0xFB2D, 2);
	if (error) goto exit;

	error = it950x_wr_reg (state, Processor_OFDM, 0xFB2D, 1);
	if (error) goto exit;

	error = it950x_wr_reg (state, Processor_OFDM, 0xFB2D, 0);
	if (error) goto exit;
	state->frequency = frequency;

	error = IT9507_runTxCalibration(state, state->bandwidth, state->frequency);

exit :
	return (error);
}

static u32 IT9507_getFirmwareVersion (
    IN  struct it950x_state*    state,
    IN  Processor       processor,
    OUT u32*          version
) {
    u32 error = ModulatorError_NO_ERROR;

	u8 writeBuffer[1] = {0,};
	u8 readBuffer[4] = {0,};

	/** Check chip version */
	writeBuffer[0] = 1;
	error = IT9507Cmd_sendCommand (state, Command_QUERYINFO, processor, 1, writeBuffer, 4, readBuffer);
	if (error) goto exit;
	
	*version = (u32) (((u32) readBuffer[0] << 24) + ((u32) readBuffer[1] << 16) + ((u32) readBuffer[2] << 8) + (u32) readBuffer[3]);

exit :
	return (error);
}

static int it950x_load_firmware (
	IN  struct it950x_state*    state
) {
	u32 error = ModulatorError_NO_ERROR;
	u32 version;
        const u8 *p;
        size_t remaining;
	u8 temp;
        int to_send;
	const struct firmware *fw = NULL;
        int i,j;
        /* ITE driver uploads 23 chunks of 48 bytes, then 1 chunk of 14, 
           then 71 chunks of 48 bytes, then 1 chunk of 34. */
        int chunk_sizes[] = { 
	  48, 23,
          14, 1,
          48, 71,
          34, 1,
          0
	};

	/** Set I2C master clock speed. */
	temp = EagleUser_IIC_SPEED;
	error = it950x_wr_regs (state, Processor_LINK, p_eagle_reg_lnk2ofdm_data_63_56, 1, &temp);
	if (error) goto exit;

	if (request_firmware(&fw, IT9507_FIRMWARE, &state->udev->dev)) {
	  printk("it950x:%s: firmware %s not found\n", __func__, IT9507_FIRMWARE);
	  return ModulatorError_INVALID_FW_TYPE; /* FIXME */
	}

        if (fw->size != 4560) {
	  printk("it950x:%s: Unexpected firmware size (%d, expected 4560)\n", __func__, (int)fw->size);
	  return ModulatorError_INVALID_FW_TYPE; /* FIXME */
        }

        j = 0;
        p = fw->data;
        while (chunk_sizes[j]) {
          for (i=0;i<chunk_sizes[j+1];i++) {
            error = IT9507Cmd_sendCommand (state, Command_SCATTER_WRITE, Processor_LINK, chunk_sizes[j], p, 0, NULL);
            if (error) goto exit;
            p += chunk_sizes[j];
          }
          j += 2;
        }

        release_firmware(fw);

	/** Boot */
	error = IT9507Cmd_sendCommand (state, Command_BOOT, Processor_LINK, 0, NULL, 0, NULL);
	if (error) goto exit;

	msleep(10);

	/** Check if firmware is running */
	version = 0;
	error = IT9507_getFirmwareVersion (state, Processor_LINK, &version);
	if (error) goto exit;
	if (version == 0)
		error = ModulatorError_BOOT_FAIL;

        printk("it950x: LINK firmware uploaded successfully - version=0x%08x\n",version);
exit :
	return (error);
}

static u32 IT9507_writeEepromValues (
    IN  struct it950x_state*    state,
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
    error = IT9507Cmd_addChecksum (state, &bufferLength, buffer);
    if (error) goto exit;

    /** send frame */
    i = 0;
    sendLength = 0;
    remainLength = bufferLength;
    while (remainLength > 0) {
        i     = (remainLength > EagleUser_MAX_PKT_SIZE) ? (EagleUser_MAX_PKT_SIZE) : (remainLength);        
        error = EagleUser_busTx (state, i, &buffer[sendLength]);
        if (error) goto exit;

        sendLength   += i;
        remainLength -= i;
    }

    /** get reply frame */
    bufferLength = 5;
    error = EagleUser_busRx (state, bufferLength, buffer);
    if (error) goto exit;

    /** remove check-sum from reply frame */
    error = IT9507Cmd_removeChecksum (state, &bufferLength, buffer);
    if (error) goto exit;

exit :
    return (error);
}


static u32 IT9507_readEepromValues (
    IN  struct it950x_state*    state,
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
    error = IT9507Cmd_addChecksum (state, &bufferLength, buffer);
    if (error) goto exit;

    /** send frame */
    i = 0;
    sendLength   = 0;
    remainLength = bufferLength;
    while (remainLength > 0) {
        i = (remainLength > EagleUser_MAX_PKT_SIZE) ? (EagleUser_MAX_PKT_SIZE) : (remainLength);        
        error = EagleUser_busTx (state, i, &buffer[sendLength]);
        if (error) goto exit;

        sendLength   += i;
        remainLength -= i;
    }

    /** get reply frame */
    bufferLength = 5 + readBufferLength;
    error = EagleUser_busTx (state, bufferLength, buffer);
    if (error) goto exit;

    /** remove frame check-sum */
    error = IT9507Cmd_removeChecksum (state, &bufferLength, buffer);
    if (error) goto exit;

    for (k = 0; k < readBufferLength; k++) {
        readBuffer[k] = buffer[k + 3];
    }

exit :
    return (error);
}


static u32 it950x_rd_regbits (
    IN  struct it950x_state*    state,
    IN  Processor       processor,
    IN  u32           registerAddress,
    IN  u8            position,
    IN  u8            length,
    OUT u8*           value
) {
        u32 error = ModulatorError_NO_ERROR;
	
	u8 temp = 0;
	error = it950x_rd_regs (state, processor, registerAddress, 1, &temp);
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


static u32 IT9507_initialize (
    IN  struct it950x_state*    state,
	IN  u8            i2cAddr
) {

	u32 error = ModulatorError_NO_ERROR;

	u32 version = 0;
	u8 c1_default_value[2],c2_default_value[2],c3_default_value[2];

	state->frequency = 642000;	
	state->calibrationInfo.ptrIQtableEx =  IQ_fixed_table0;
	state->calibrationInfo.tableGroups = IQ_TABLE_NROW;
	state->i2cAddr = i2cAddr;

	/** Write secondary I2C address to device */
	//error = it950x_wr_reg (state, Processor_LINK, p_eagle_reg_lnk2ofdm_data_63_56, EagleUser_IIC_SPEED);
	//if (error) goto exit;	
	
	error = it950x_wr_reg (state, Processor_LINK, second_i2c_address, 0x00);
	if (error) goto exit;

        if (!state->booted) {	
          /** Load firmware */
	  error = it950x_load_firmware (state);
	  if (error) goto exit;
	  state->booted = true;
	}

	error = it950x_wr_reg (state, Processor_LINK, 0xD924, 0);//set UART -> GPIOH4
	if (error) goto exit;


	/** Set I2C master clock 100k in order to support tuner I2C. */
	error = it950x_wr_reg (state, Processor_LINK, p_eagle_reg_lnk2ofdm_data_63_56, 0x7);//1a
	if (error) goto exit;



	error = it950x_wr_regbits (state, Processor_OFDM, 0xFB26, 7, 1, 1);
	if (error) goto exit;

	error = it950x_wr_reg (state, Processor_OFDM, 0xFBBD, 0xE0);
	if (error) goto exit;

	error = it950x_wr_reg (state, Processor_OFDM, 0xF99A, 0);
	if (error) goto exit;

	error = EagleUser_Initialization(state);
	if (error) goto exit;

	/** Set the desired stream type */
	error = IT9507_setTsInterface (state);
	if (error) goto exit;

	/** Set H/W MPEG2 locked detection **/
	error = it950x_wr_reg (state, Processor_LINK, p_eagle_reg_top_lock3_out, 1);
	if (error) goto exit;

	error = it950x_wr_reg (state, Processor_LINK, p_eagle_reg_top_padmiscdrsr, 1);
	if (error) goto exit;
	/** Set registers for driving power 0xD830 **/
	error = it950x_wr_reg (state, Processor_LINK, p_eagle_reg_top_padmiscdr2, 0);
	if (error) goto exit;
	

	/** Set registers for driving power 0xD831 **/
	error = it950x_wr_reg (state, Processor_LINK, p_eagle_reg_top_padmiscdr4, 0);
	if (error) goto exit;

	/** Set registers for driving power 0xD832 **/
	error = it950x_wr_reg (state, Processor_LINK, p_eagle_reg_top_padmiscdr8, 0);
	if (error) goto exit;   

	error = it950x_wr_reg (state, Processor_OFDM, 0xFB2E, 0x11);
	if (error) goto exit;
	error = it950x_wr_reg (state, Processor_OFDM, 0xFBB3, 0x98);
	if (error) goto exit;

	error = it950x_rd_regs (state, Processor_OFDM, p_eagle_reg_iqik_c1_7_0, 2, c1_default_value);
	if (error) goto exit;
	error = it950x_rd_regs (state, Processor_OFDM, p_eagle_reg_iqik_c2_7_0, 2, c2_default_value);
	if (error) goto exit;
	error = it950x_rd_regs (state, Processor_OFDM, p_eagle_reg_iqik_c3_7_0, 2, c3_default_value);
	if (error) goto exit;

	state->calibrationInfo.c1DefaultValue = c1_default_value[1]<<8 | c1_default_value[0];
	state->calibrationInfo.c2DefaultValue = c2_default_value[1]<<8 | c2_default_value[0];
	state->calibrationInfo.c3DefaultValue = c3_default_value[1]<<8 | c3_default_value[0];

exit:

	return (error);
}


static u32 IT9507_setTxModeEnable (
    IN  struct it950x_state*            state,
    IN  u8                    enable
) {
	u32 error = ModulatorError_NO_ERROR;

	if(enable){

		//afe Power up
		error = it950x_wr_reg (state, Processor_OFDM, p_eagle_reg_afe_mem0, 0);
		if (error) goto exit;
		error = it950x_wr_reg (state, Processor_OFDM, p_eagle_reg_afe_mem1, 0xFC);
		if (error) goto exit;
			
		error = it950x_wr_reg (state, Processor_OFDM, p_eagle_reg_fec_sw_rst, 0);
		if (error) goto exit;

		error = it950x_wr_reg (state, Processor_LINK, 0xDDAB, 0);
		if (error) goto exit;
	
		error = it950x_wr_reg (state, Processor_OFDM, eagle_reg_tx_fifo_overflow, 1); //clear
		if (error) goto exit;
		
	}else{

		error = it950x_wr_reg (state, Processor_LINK, 0xDDAB, 1);
		if (error) goto exit;
		error = it950x_wr_reg (state, Processor_OFDM, p_eagle_reg_fec_sw_rst, 1);
		if (error) goto exit;

		//afe Power down
		error = it950x_wr_reg (state, Processor_OFDM, p_eagle_reg_afe_mem0, 1);
		if (error) goto exit;
		error = it950x_wr_reg (state, Processor_OFDM, p_eagle_reg_afe_mem1, 0xFE);
		if (error) goto exit;
			
	}
	error = EagleUser_setTxModeEnable(state, enable);
exit :
	msleep(100);
	return (error);
}


static u32 IT9507_setTXChannelModulation (
    IN  struct it950x_state*            state,
    IN  ChannelModulation*      channelModulation
) {
	u32 error = ModulatorError_NO_ERROR;

	u8 temp;

	//u8 temp;
	error = IT9507_setTxModeEnable(state,0);
	if (error) goto exit;
	/** Set constellation type */
	temp=(u8)channelModulation->constellation;

	state->channelModulation.constellation=channelModulation->constellation;
	error = it950x_wr_reg (state, Processor_OFDM, 0xf721, temp);
	if (error) goto exit;

	state->channelModulation.highCodeRate=channelModulation->highCodeRate;
	temp=(u8)channelModulation->highCodeRate;
	error = it950x_wr_reg (state, Processor_OFDM, 0xf723, temp);
	if (error) goto exit;
	/** Set low code rate */

	/** Set guard interval */
	state->channelModulation.interval=channelModulation->interval;
	temp=(u8)channelModulation->interval;

	error = it950x_wr_reg (state, Processor_OFDM, p_eagle_reg_tps_gi, temp);
	if (error) goto exit;
	/** Set FFT mode */
	state->channelModulation.transmissionMode=channelModulation->transmissionMode;
	temp=(u8)channelModulation->transmissionMode;
	error = it950x_wr_reg (state, Processor_OFDM, 0xf726, temp);
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
	error = it950x_wr_reg (state, Processor_OFDM, 0xf7C1, temp);
	if (error) goto exit;

	error = it950x_wr_reg (state, Processor_OFDM, 0xf7C6, 1);
	if (error) goto exit;

exit :
	return (error);
}

static u32 IT9507_acquireTxChannel (
	IN  struct it950x_state*            state,
    IN  u16            bandwidth,
    IN  u32           frequency
) {
	u32 error = ModulatorError_NO_ERROR;
	u16 TABLE_NROW = state->calibrationInfo.tableGroups;
	if(state->calibrationInfo.ptrIQtableEx[TABLE_NROW-1].frequency<frequency 
		|| state->calibrationInfo.ptrIQtableEx[0].frequency>frequency 
	){
		error = ModulatorError_FREQ_OUT_OF_RANGE;
		goto exit;
	}

	error = IT9507_selectBandwidth (state, bandwidth);
	if (error) goto exit;
	state->bandwidth = bandwidth;
	
	/** Set frequency */
	
	error = IT9507_setFrequency (state, frequency);
	if (error) goto exit;
	
	error = EagleUser_acquireChannel(state, bandwidth, frequency);
exit :
	return (error);
}

#if 0
static u32 IT9507_resetPSBBuffer (
	IN  struct it950x_state*    state
){
	u32 error = ModulatorError_NO_ERROR;
	u32 temp;

	if(state->tsInterfaceType == PARALLEL_TS_INPUT)
		temp = 0xF9CC;
	else
		temp = 0xF9CD;

	error = it950x_wr_reg (state, Processor_OFDM, 0xF9A4, 1);
	if (error) goto exit;

	error = it950x_wr_reg (state, Processor_OFDM, temp, 0);
	if (error) goto exit;



	error = it950x_wr_reg (state, Processor_OFDM, 0xF9A4, 0);
	if (error) goto exit;

	error = it950x_wr_reg (state, Processor_OFDM, temp, 1);

exit :


	return (error);
}
#endif

static u32 IT9507_setTsInterface (
    IN  struct it950x_state*    state
) {
    u32 error = ModulatorError_NO_ERROR;
	u16 frameSize;
	u8 packetSize;
	u8 buffer[2];

	error = it950x_wr_regbits (state, Processor_LINK, p_eagle_reg_dvbt_inten, eagle_reg_dvbt_inten_pos, eagle_reg_dvbt_inten_len, 1);
	if (error) goto exit;
	error = it950x_wr_regbits (state, Processor_OFDM, p_eagle_reg_mpeg_full_speed, eagle_reg_mpeg_full_speed_pos, eagle_reg_mpeg_full_speed_len, 0);
	if (error) goto exit;

	/** Enable DVB-T mode */
	error = it950x_wr_regbits (state, Processor_LINK, p_eagle_reg_dvbt_en, eagle_reg_dvbt_en_pos, eagle_reg_dvbt_en_len, 1);
	if (error) goto exit;
	error = it950x_wr_regbits (state, Processor_OFDM, p_mp2if_mpeg_ser_mode, mp2if_mpeg_ser_mode_pos, mp2if_mpeg_ser_mode_len, 0);
	if (error) goto exit;
	error = it950x_wr_regbits (state, Processor_OFDM, p_mp2if_mpeg_par_mode, mp2if_mpeg_par_mode_pos, mp2if_mpeg_par_mode_len, 0);
	if (error) goto exit;
	/** Fix current leakage */
	error = it950x_wr_regbits (state, Processor_LINK, p_eagle_reg_top_hostb_mpeg_ser_mode, eagle_reg_top_hostb_mpeg_ser_mode_pos, eagle_reg_top_hostb_mpeg_ser_mode_len, 0);
	if (error) goto exit;
	error = it950x_wr_regbits (state, Processor_LINK, p_eagle_reg_top_hostb_mpeg_par_mode, eagle_reg_top_hostb_mpeg_par_mode_pos, eagle_reg_top_hostb_mpeg_par_mode_len, 0);
	if (error) goto exit;
	
	error = it950x_wr_reg (state, Processor_OFDM, 0xF714, 0);
	if (error) goto exit;

	frameSize = EagleUser_USB20_FRAME_SIZE_DW;
	packetSize = (u8) (EagleUser_USB20_MAX_PACKET_SIZE / 4);

	
	error = it950x_wr_regbits (state, Processor_OFDM, p_eagle_reg_mp2_sw_rst, eagle_reg_mp2_sw_rst_pos, eagle_reg_mp2_sw_rst_len, 1);
	if (error) goto exit;

	/** Reset EP5 */
	error = it950x_wr_regbits (state, Processor_OFDM, p_eagle_reg_mp2if2_sw_rst, eagle_reg_mp2if2_sw_rst_pos, eagle_reg_mp2if2_sw_rst_len, 1);
	if (error) goto exit;

	
	/** Disable EP5 */
	error = it950x_wr_regbits (state, Processor_LINK, p_eagle_reg_ep5_tx_en, eagle_reg_ep5_tx_en_pos, eagle_reg_ep5_tx_en_len, 0);
	if (error) goto exit;

	/** Disable EP5 NAK */
	error = it950x_wr_regbits (state, Processor_LINK, p_eagle_reg_ep5_tx_nak, eagle_reg_ep5_tx_nak_pos, eagle_reg_ep5_tx_nak_len, 0);
	if (error) goto exit;


	/** Enable EP5 */
	error = it950x_wr_regbits (state, Processor_LINK, p_eagle_reg_ep5_tx_en, eagle_reg_ep5_tx_en_pos, eagle_reg_ep5_tx_en_len, 1);
	if (error) goto exit;

	/** Set EP5 transfer length */
	buffer[p_eagle_reg_ep5_tx_len_7_0 - p_eagle_reg_ep5_tx_len_7_0] = (u8) frameSize;
	buffer[p_eagle_reg_ep5_tx_len_15_8 - p_eagle_reg_ep5_tx_len_7_0] = (u8) (frameSize >> 8);
	error = it950x_wr_regs (state, Processor_LINK, p_eagle_reg_ep5_tx_len_7_0, 2, buffer);

	/** Set EP5 packet size */
	error = it950x_wr_reg (state, Processor_LINK, p_eagle_reg_ep5_max_pkt, packetSize);
	if (error) goto exit;

	/** Disable 15 SER/PAR mode */
	error = it950x_wr_regbits (state, Processor_OFDM, p_mp2if_mpeg_ser_mode, mp2if_mpeg_ser_mode_pos, mp2if_mpeg_ser_mode_len, 0);
	if (error) goto exit;
	error = it950x_wr_regbits (state, Processor_OFDM, p_mp2if_mpeg_par_mode, mp2if_mpeg_par_mode_pos, mp2if_mpeg_par_mode_len, 0);
	if (error) goto exit;

	
	/** Enable mp2if2 */
	error = it950x_wr_regbits (state, Processor_OFDM, p_eagle_reg_mp2if2_en, eagle_reg_mp2if2_en_pos, eagle_reg_mp2if2_en_len, 1);
	if (error) goto exit;

		/** Enable tsis */
		error = it950x_wr_regbits (state, Processor_OFDM, p_eagle_reg_tsip_en, eagle_reg_tsip_en_pos, eagle_reg_tsip_en_len, 0);
		if (error) goto exit;
		error = it950x_wr_regbits (state, Processor_OFDM, p_eagle_reg_tsis_en, eagle_reg_tsis_en_pos, eagle_reg_tsis_en_len, 1);
		if (error) goto exit;
		error = it950x_wr_reg (state, Processor_OFDM, p_eagle_reg_ts_in_src, 0);
		if (error) goto exit;

	/** Negate EP4 reset */
	error = it950x_wr_regbits (state, Processor_OFDM, p_eagle_reg_mp2_sw_rst, eagle_reg_mp2_sw_rst_pos, eagle_reg_mp2_sw_rst_len, 0);
	if (error) goto exit;

	/** Negate EP5 reset */
	error = it950x_wr_regbits (state, Processor_OFDM, p_eagle_reg_mp2if2_sw_rst, eagle_reg_mp2if2_sw_rst_pos, eagle_reg_mp2if2_sw_rst_len, 0);
	if (error) goto exit;


	/** Split 15 PSB to 1K + 1K and enable flow control */
	error = it950x_wr_regbits (state, Processor_OFDM, p_eagle_reg_mp2if2_half_psb, eagle_reg_mp2if2_half_psb_pos, eagle_reg_mp2if2_half_psb_len, 0);
	if (error) goto exit;
	error = it950x_wr_regbits (state, Processor_OFDM, p_eagle_reg_mp2if_stop_en, eagle_reg_mp2if_stop_en_pos, eagle_reg_mp2if_stop_en_len, 1);
	if (error) goto exit;			

	error = it950x_wr_reg (state, Processor_LINK, p_eagle_reg_top_host_reverse, 0);
	if (error) goto exit;

	error = it950x_wr_regbits (state, Processor_LINK, p_eagle_reg_ep6_rx_en, eagle_reg_ep6_rx_en_pos, eagle_reg_ep6_rx_en_len, 0);
	if (error) goto exit;

	error = it950x_wr_regbits (state, Processor_LINK, p_eagle_reg_ep6_rx_nak, eagle_reg_ep6_rx_nak_pos, eagle_reg_ep6_rx_nak_len, 0);
	if (error) goto exit;


	error = it950x_wr_regbits (state, Processor_LINK, p_eagle_reg_ep6_rx_en, eagle_reg_ep6_rx_en_pos, eagle_reg_ep6_rx_en_len, 1);
	if (error) goto exit;

	error = it950x_wr_reg (state, Processor_LINK, p_eagle_reg_ep6_max_pkt, 0x80);
	if (error) goto exit;

	error = it950x_wr_reg (state, Processor_LINK, p_eagle_reg_ep6_cnt_num_7_0, 0x16);
	if (error) goto exit;

	error = EagleUser_mpegConfig (state);

exit :
	return (error);
}

static u32 IT9507_TXreboot (
    IN  struct it950x_state*    state
)  {
	u32 error = ModulatorError_NO_ERROR;
	u32 version;
	u8 i;
	
	error = IT9507_getFirmwareVersion (state, Processor_LINK, &version);
	if (error) goto exit;
	if (version == 0xFFFFFFFF) goto exit;       
	if (version != 0) {
		
		error = IT9507Cmd_reboot (state);
		msleep(1);
		goto exit;
	}

	state->booted = false;

exit :
	return (error);
}


static u32 IT9507_controlPowerSaving (
    IN  struct it950x_state*    state,
    IN  u8            control
) {
	u32 error = ModulatorError_NO_ERROR;

	if (control) {
		/** Power up case */
		error = it950x_wr_regbits (state, Processor_OFDM, p_eagle_reg_afe_mem0, 3, 1, 0);
		if (error) goto exit;
		error = it950x_wr_reg (state, Processor_OFDM, p_eagle_reg_dyn0_clk, 0);
		if (error) goto exit;

		/** Fixed current leakage */
		error = it950x_wr_regbits (state, Processor_LINK, p_eagle_reg_top_hostb_mpeg_ser_mode, eagle_reg_top_hostb_mpeg_ser_mode_pos, eagle_reg_top_hostb_mpeg_ser_mode_len, 0);
		if (error) goto exit;
		/** Disable HostB parallel */  
		error = it950x_wr_regbits (state, Processor_LINK, p_eagle_reg_top_hostb_mpeg_par_mode, eagle_reg_top_hostb_mpeg_par_mode_pos, eagle_reg_top_hostb_mpeg_par_mode_len, 0);
		if (error) goto exit;
	} else {
		/** Power down case */
		error = it950x_wr_regbits (state, Processor_OFDM, p_eagle_reg_afe_mem0, 3, 1, 1);

		/** Fixed current leakage */
		/** Enable HostB parallel */
		error = it950x_wr_regbits (state, Processor_LINK, p_eagle_reg_top_hostb_mpeg_ser_mode, eagle_reg_top_hostb_mpeg_ser_mode_pos, eagle_reg_top_hostb_mpeg_ser_mode_len, 0);
		if (error) goto exit;
		error = it950x_wr_regbits (state, Processor_LINK, p_eagle_reg_top_hostb_mpeg_par_mode, eagle_reg_top_hostb_mpeg_par_mode_pos, eagle_reg_top_hostb_mpeg_par_mode_len, 1);
		if (error) goto exit;						
	}

exit :
	return (error);
}




static u32 IT9507_sendHwPSITable (
	IN  struct it950x_state*    state,
	IN  u8*            pbuffer
) {
 	u32 error = ModulatorError_NO_ERROR;
	u8 temp_timer[5];
	u8 tempbuf[10] ;
	u8 i,temp;
	
	error = it950x_rd_regs (state, Processor_OFDM, psi_table1_timer_H, 10, temp_timer);		//save pei table timer	
	if (error) goto exit;

	for(i=0;i<10;i++)
		tempbuf[i] = 0;

	error = it950x_wr_regs (state, Processor_OFDM, psi_table1_timer_H, 10, tempbuf);		//stop send FW psi table	
	if (error) goto exit;

	for(i=0 ; i<50 ;i++){
		error = it950x_rd_reg (state, Processor_OFDM, p_reg_psi_access, &temp);		//wait per table send	
		if (error) goto exit;
		if(temp == 0) break;
		msleep(1);
	}

	error = it950x_wr_reg (state, Processor_OFDM, p_reg_psi_index, 0);
	if (error) goto exit;

	for(i=0;i<188;i++){
		temp = pbuffer[i];	
		error = it950x_wr_reg (state, Processor_OFDM, p_reg_psi_dat,  temp); //write data to HW psi table buffer
		if (error) goto exit;
	}

	error = it950x_wr_regbits (state, Processor_OFDM, p_reg_psi_access, reg_psi_access_pos, reg_psi_access_len, 1); //send psi tabledata
	if (error) goto exit;

	
	error = it950x_wr_regs (state, Processor_OFDM, psi_table1_timer_H, 10, temp_timer);		//set org timer	
	if (error) goto exit;

	

exit :

	return (error);
}

static u32 IT9507_accessFwPSITable (
	IN  struct it950x_state*    state,
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
		error = it950x_wr_regs (state, Processor_OFDM, psi_table1_timer_H+(psiTableIndex-1)*2, 2, temp);		//set timer	= 0 & stop
		if (error) goto exit;

		for(i=0;i<188;i++){
			error = it950x_wr_reg (state, Processor_OFDM, (PSI_table1+(psiTableIndex-1)*188)+i, pbuffer[i]);
			if (error) goto exit;

		}

	}else{
		error = ModulatorError_INVALID_INDEX;
	}
	
exit :


	return (error);
}

static u32 IT9507_setFwPSITableTimer (
	IN  struct it950x_state*    state,
	IN  u8		  psiTableIndex,
	IN  u16          timer_ms
) {
    u32 error ;
	u8 temp[2];
	error = ModulatorError_NO_ERROR;

	temp[0] = (u8)(timer_ms>>8);
	temp[1] = (u8)timer_ms;
	


	if((psiTableIndex>0)&&(psiTableIndex<6)){	
		error = it950x_wr_regs (state, Processor_OFDM, psi_table1_timer_H+(psiTableIndex-1)*2, 2,temp);		
	}else{
		error = ModulatorError_INVALID_INDEX;
	}	
	return (error);
}


static u32 IT9507_setSlaveIICAddress (
    IN  struct it950x_state*    state,
	IN  u8          SlaveAddress
){
	u32 error = ModulatorError_NO_ERROR;

	if(state != NULL)
		state->slaveIICAddr = SlaveAddress;
	else
		error  = ModulatorError_NULL_HANDLE_PTR;
    return (error);
}

static u32 IT9507_adjustOutputGain (
	IN  struct it950x_state*    state,
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

	c1value_default = state->calibrationInfo.c1DefaultValue;
	c2value_default = state->calibrationInfo.c2DefaultValue;
	c3value_default = state->calibrationInfo.c3DefaultValue;	
	
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
	state->calibrationInfo.outputGain = *gain;
	
	error = it950x_wr_reg (state, Processor_OFDM, p_eagle_reg_iqik_c1_7_0, (u8)(c1value&0x00ff));
	if (error) goto exit;		
	error = it950x_wr_reg (state, Processor_OFDM, p_eagle_reg_iqik_c1_10_8, (u8)(c1value>>8));
	if (error) goto exit;		
	error = it950x_wr_reg (state, Processor_OFDM, p_eagle_reg_iqik_c2_7_0, (u8)(c2value&0x00ff));
	if (error) goto exit;
	error = it950x_wr_reg (state, Processor_OFDM, p_eagle_reg_iqik_c2_10_8, (u8)(c2value>>8));
	if (error) goto exit;
	error = it950x_wr_reg (state, Processor_OFDM, p_eagle_reg_iqik_c3_7_0, (u8)(c3value&0x00ff));
	if (error) goto exit;
	error = it950x_wr_reg (state, Processor_OFDM, p_eagle_reg_iqik_c3_10_8, (u8)(c3value>>8));
	if (error) goto exit;

exit:

	return (error);
}

static u32 IT9507_getGainRange (
	IN  struct it950x_state*    state,
	IN  u32           frequency,
	IN  u16            bandwidth,    
	OUT int*			maxGain,
	OUT int*			minGain
){
	u32 error = ModulatorError_NO_ERROR;
	u8 val[6];
	u16 TABLE_NROW = state->calibrationInfo.tableGroups;
	if(state->calibrationInfo.ptrIQtableEx[TABLE_NROW-1].frequency<frequency 
		|| state->calibrationInfo.ptrIQtableEx[0].frequency>frequency 
	){
		error = ModulatorError_FREQ_OUT_OF_RANGE;
		goto exit;
	}

	if((bandwidth !=0) && (frequency !=0)){
		error = EagleTuner_calIQCalibrationValue(state,frequency,val);
	}else{
		error = ModulatorError_FREQ_OUT_OF_RANGE;
		goto exit;
	}

	*maxGain = 100;
	IT9507_calOutputGain(state, val, maxGain);

	*minGain = -100;
	IT9507_calOutputGain(state, val, minGain);
exit:		
	return (error);
}

static u32 IT9507_getOutputGain (
	IN  struct it950x_state*    state,
	OUT  int			  *gain	   
){
   
    *gain = state->calibrationInfo.outputGain;

    return(ModulatorError_NO_ERROR);
}

static u32 IT9507_setTPS (
    IN  struct it950x_state*    state,
    IN  TPS           tps
){
	u32   error = ModulatorError_NO_ERROR;
	//---- set TPS Cell ID
	

	error = it950x_wr_reg (state, Processor_OFDM, 0xF727, (u8)(tps.cellid>>8));
	if (error) goto exit;

	error = it950x_wr_reg (state, Processor_OFDM, 0xF728, (u8)(tps.cellid));
		
exit:	
	return (error);

}

static u32 IT9507_getTPS (
    IN  struct it950x_state*    state,
    IN  pTPS           pTps
){
	u32   error = ModulatorError_NO_ERROR;
	//---- get TPS Cell ID
	u8 temp;
	u16 cellID = 0;

	error = it950x_rd_reg (state, Processor_OFDM, 0xF727, &temp);//get cell id
	if (error) goto exit;
	cellID = temp<<8;

	error = it950x_rd_reg (state, Processor_OFDM, 0xF728, &temp);//get cell id
	cellID = cellID | temp;	
	pTps->cellid = cellID;

exit:	
	return (error);
}

static u32 IT9507_setIQtable (
	IN  struct it950x_state*    state,
    IN  IQtable *IQ_table_ptr,
	IN  u16 tableGroups
){
	u32   error = ModulatorError_NO_ERROR;

	if(IQ_table_ptr == NULL){
		error = ModulatorError_NULL_PTR;
		state->calibrationInfo.ptrIQtableEx =  IQ_fixed_table0; // set to default table
		state->calibrationInfo.tableGroups = IQ_TABLE_NROW;
	}else{
		state->calibrationInfo.ptrIQtableEx = IQ_table_ptr;
		state->calibrationInfo.tableGroups = tableGroups;
	}
	return (error);
}


static u32 IT9507_setDCCalibrationValue (
	IN  struct it950x_state*	state,
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

	error = it950x_wr_reg (state, Processor_OFDM, p_eagle_reg_iqik_dc_i_7_0, (u8)(dc_i_temp));
	if (error) goto exit;

	error = it950x_wr_reg (state, Processor_OFDM, p_eagle_reg_iqik_dc_i_8, (u8)(dc_i_temp>>8));
	if (error) goto exit;

	error = it950x_wr_reg (state, Processor_OFDM, p_eagle_reg_iqik_dc_q_7_0, (u8)(dc_q_temp));
	if (error) goto exit;

	error = it950x_wr_reg (state, Processor_OFDM, p_eagle_reg_iqik_dc_q_8, (u8)(dc_q_temp>>8));
	if (error) goto exit;
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
	
	deb_data("- Enter %s Function -",__FUNCTION__);

	//patch for read eeprom valid bit
	dwError = it950x_rd_reg(&pdc->state, Processor_LINK, chip_version_7_0, &chip_version);
	dwError = it950x_rd_regs(&pdc->state, Processor_LINK, chip_version_7_0+1, 2, var);

	if(dwError) deb_data("DRV_GetEEPROMConfig fail---cant read chip version");

	chip_Type = var[1]<<8 | var[0];
	if(chip_Type==0x9135 && chip_version == 2) //Om2
	{
		dwError = it950x_rd_regs(&pdc->state, Processor_LINK, 0x461d, 1, &btmp);
		deb_data("Chip Version is %d---and Read 461d---valid bit = 0x%02X", chip_version, btmp);
	}
	else 
	{
		dwError = it950x_rd_regs(&pdc->state, Processor_LINK, 0x4979, 1, &btmp);
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
    	//pdc->state.chipNumber = 1;    
	}
	else
	{
		deb_data("=============Need read eeprom");
		dwError = it950x_rd_regs(&pdc->state, Processor_LINK, EEPROM_IRMODE, 1, &btmp);
    	if (dwError) goto exit;
    	deb_data("EEPROM_IRMODE = 0x%02X, ", btmp);	

//bDualTs option   
    	//pdc->state.chipNumber = 1;    

#if 0
    	dwError = it950x_rd_regs(&pdc->state, Processor_LINK, EEPROM_TSMODE, 1, &btmp);
    	if (dwError) goto exit;
    	deb_data("EEPROM_TSMODE = 0x%02X", btmp);
	// We know this is 0 - i.e. single TS mode.
#endif

		//dwError = it950x_wr_reg((Destate*) &pdc->Destate, 0, Processor_LINK, EEPROM_SUSPEND, 0);
		dwError = it950x_rd_regs(&pdc->state, Processor_LINK, EEPROM_SUSPEND, 1, &btmp);
		deb_data("EEPROM susped mode=%d", btmp);
    	
    }
		//pdc->state.chipNumber = 1; 

//init some device info
    pdc->fc.ulCurrentFrequency = 0;  
    pdc->fc.ucCurrentBandWidth = 0;
    pdc->fc.ulDesiredFrequency = 0;	
    pdc->fc.ucDesiredBandWidth = 6000;	

    //For PID Filter Setting
    //pdc->fc.ulcPIDs = 0;    
    pdc->fc.bEnPID = false;
    pdc->fc.bApOn = false;
    pdc->fc.bResetTs = false;

    pdc->fc.tunerinfo.bTunerOK = false;
    pdc->fc.tunerinfo.bSettingFreq = false;
	
exit:
    return(dwError);
}

//************** DL_ *************//

u32 DL_ApPwCtrl (
	void* handle,
    bool  bOn
)
{
    u32 dwError = Error_NO_ERROR;
	u8    i = 0;
	PDEVICE_CONTEXT PDC = (PDEVICE_CONTEXT)handle;
	
	mutex_lock(&mymutex);

	deb_data("- Enter %s Function -",__FUNCTION__);
	deb_data("  bOn = %s\n", bOn?"ON":"OFF");

		if(bOn) {	  // resume
			deb_data("IT950x Power ON\n");				
			dwError = IT9507_controlPowerSaving (&PDC->state, bOn);				
			if(dwError) { 
				deb_data("ApCtrl::IT9507_controlPowerSaving error = 0x%04x\n", dwError); 
				goto exit;
			}
		} else {      // suspend
			//deb_data("IT950x TxMode RF OFF\n");				
			dwError = IT9507_setTxModeEnable(&PDC->state, 0);
			if(dwError) {
				deb_data("ApCtrl::IT9507_setTxModeEnable error = 0x%04x\n", dwError);
				goto exit;				
			}
			deb_data("IT950x Power OFF\n");							
			dwError = IT9507_controlPowerSaving (&PDC->state, bOn);			
			if(dwError) {
				deb_data("ApCtrl::IT9507_controlPowerSaving error = 0x%04x\n", dwError);
				goto exit;
			}
		}			


exit:
    mutex_unlock(&mymutex);
    	return(dwError);
}

u32 DL_CheckTunerInited(
	void* handle,
	bool *bOn )
{
	u32 dwError = Error_NO_ERROR;
	PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;
	
    mutex_lock(&mymutex);

    deb_data("- Enter %s Function -\n",__FUNCTION__);

    *bOn = pdc->fc.tunerinfo.bTunerInited;

    mutex_unlock(&mymutex);

    return(dwError);
}

u32 DL_DemodIOCTLFun(struct it950x_state *state, u32 IOCTLCode, unsigned long pIOBuffer)
{
    u32 dwError = Error_NO_ERROR;

    mutex_lock(&mymutex);

    deb_data("DemodIOCTLFun function");

    switch (IOCTLCode)
    {
        case IOCTL_ITE_DEMOD_GETDEVICETYPE_TX:
        {
            PTxGetDeviceTypeRequest pRequest = (PTxGetDeviceTypeRequest) pIOBuffer;
            pRequest->error = EagleUser_getDeviceType(state, &pRequest->DeviceType);
            break;
        }
        case IOCTL_ITE_DEMOD_SETDEVICETYPE_TX:
        {
            PTxSetDeviceTypeRequest pRequest = (PTxSetDeviceTypeRequest) pIOBuffer;

            if(EagleUser_setSystemConfig(state) != 0)
                printk("\n- EagleUser_setSystemConfig fail -\n");    
            else 
                printk("\n- EagleUser_setSystemConfig ok -\n");                
            break;
        }        
        case IOCTL_ITE_DEMOD_ADJUSTOUTPUTGAIN_TX: 
        {
            PSetGainRequest pRequest = (PSetGainRequest) pIOBuffer;
            pRequest->error = IT9507_adjustOutputGain(state, &pRequest->GainValue);
            break;
        }
        case IOCTL_ITE_DEMOD_ENABLETXMODE_TX:
        {
            PTxModeRequest pRequest = (PTxModeRequest) pIOBuffer;
            pRequest->error = IT9507_setTxModeEnable(state, pRequest->OnOff);
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
            pRequest->error = IT9507_setTXChannelModulation(state, &temp);
            pRequest->error = IT9507_setTxModeEnable(state, 1);
            deb_data("IT950x TxMode RF ON\n");                
            break;
        }
        case IOCTL_ITE_DEMOD_ACQUIRECHANNEL_TX:
        {
            PTxAcquireChannelRequest pRequest = (PTxAcquireChannelRequest) pIOBuffer;
            pRequest->error = IT9507_acquireTxChannel(state, pRequest->bandwidth, pRequest->frequency);
            break;
        }
        case IOCTL_ITE_DEMOD_GETFIRMWAREVERSION_TX:
        {
            PTxGetFirmwareVersionRequest pRequest = (PTxGetFirmwareVersionRequest) pIOBuffer;
            pRequest->error = IT9507_getFirmwareVersion (state, pRequest->processor, pRequest->version);
            break;
        }
        case IOCTL_ITE_DEMOD_CONTROLPOWERSAVING:
        {
            PControlPowerSavingRequest pRequest = (PControlPowerSavingRequest) pIOBuffer;
            //DAVE: pRequest->error = Destate_controlPowerSaving ((Destate*) handle, pRequest->control);
            pRequest->error = it950x_wr_regbits(state, Processor_LINK, p_reg_top_gpioh5_o, reg_top_gpioh5_o_pos, reg_top_gpioh5_o_len, !pRequest->control);
            break;
        }
        case IOCTL_ITE_DEMOD_CONTROLPOWERSAVING_TX:
        {
            PTxControlPowerSavingRequest pRequest = (PTxControlPowerSavingRequest) pIOBuffer;
            pRequest->error = IT9507_setTxModeEnable(state, pRequest->control);
            pRequest->error = IT9507_controlPowerSaving (state, pRequest->control);            
            break;
        }
        case IOCTL_ITE_DEMOD_WRITEGENERICREGISTERS_TX:
        {
            PWriteGenericRegistersRequest pRequest = (PWriteGenericRegistersRequest) pIOBuffer;
            pRequest->error = IT9507_writeGenericRegisters (state, pRequest->slaveAddress, pRequest->bufferLength, pRequest->buffer);

            break;
        }
        case IOCTL_ITE_DEMOD_READGENERICREGISTERS_TX:
        {
            PReadGenericRegistersRequest pRequest = (PReadGenericRegistersRequest) pIOBuffer;
            pRequest->error = IT9507_readGenericRegisters (state, pRequest->slaveAddress, pRequest->bufferLength, pRequest->buffer);

            break;
        }
        case IOCTL_ITE_DEMOD_FINALIZE:
        {
            PFinalizeRequest pRequest = (PFinalizeRequest) pIOBuffer;
            //pRequest->error = Destate_finalize ((Destate*) handle);
            //pRequest->error = Destate_controlPowerSaving ((Destate*) handle, 0);
            break;
        }
        case IOCTL_ITE_DEMOD_FINALIZE_TX:
        {
            PTxFinalizeRequest pRequest = (PTxFinalizeRequest) pIOBuffer;
            //pRequest->error = IT9507_finalize (state);
            //pRequest->error = IT9507_controlPowerSaving (state, 0);
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

            IT9507_getFirmwareVersion (state, Processor_LINK, &dwFWVerionLink);
            sprintf((char *)pDriverInfo->FWVerionLink, "%X.%X.%X.%X", (u8)(dwFWVerionLink>>24), (u8)(dwFWVerionLink>>16), (u8)(dwFWVerionLink>>8), (u8)dwFWVerionLink);
            deb_data("State_getFirmwareVersion Processor_LINK %s\n", (char *)pDriverInfo->FWVerionLink);
 
            IT9507_getFirmwareVersion (state, Processor_OFDM, &dwFWVerionOFDM);
            sprintf((char *)pDriverInfo->FWVerionOFDM, "%X.%X.%X.%X", (u8)(dwFWVerionOFDM>>24), (u8)(dwFWVerionOFDM>>16), (u8)(dwFWVerionOFDM>>8), (u8)dwFWVerionOFDM);
            deb_data("State_getFirmwareVersion Processor_OFDM %s\n", (char *)pDriverInfo->FWVerionOFDM);

            strcpy((char *)pDriverInfo->Company, "ITEtech");
            strcpy((char *)pDriverInfo->SupportHWInfo, "Ganymede DVBT");

            pDriverInfo->error = Error_NO_ERROR;

            break;
        }
        case IOCTL_ITE_DEMOD_WRITEREGISTERS_TX:
        {
            PTxWriteRegistersRequest pRequest = (PTxWriteRegistersRequest) pIOBuffer;
            pRequest->error = it950x_wr_regs (state, pRequest->processor, pRequest->registerAddress, pRequest->bufferLength, pRequest->buffer);

            break;
        }
        case IOCTL_ITE_DEMOD_WRITEEEPROMVALUES_TX:
        {
            PWriteEepromValuesRequest pRequest = (PWriteEepromValuesRequest) pIOBuffer;
            pRequest->error = IT9507_writeEepromValues (state, pRequest->registerAddress, pRequest->bufferLength, pRequest->buffer);

            break;
        }
        case IOCTL_ITE_DEMOD_WRITEREGISTERBITS_TX:
        {
            PTxWriteRegisterBitsRequest pRequest = (PTxWriteRegisterBitsRequest) pIOBuffer;
            pRequest->error = it950x_wr_regbits (state, pRequest->processor, pRequest->registerAddress, pRequest->position, pRequest->length, pRequest->value);

            break;
        }
        case IOCTL_ITE_DEMOD_READREGISTERS_TX:
        {
        //    deb_data("IOCTL_ITE_DEMOD_READREGISTERS\n");
            PTxReadRegistersRequest pRequest = (PTxReadRegistersRequest) pIOBuffer;
            pRequest->error = it950x_rd_regs (state, pRequest->processor, pRequest->registerAddress, pRequest->bufferLength, pRequest->buffer);

            break;
        }
        case IOCTL_ITE_DEMOD_READEEPROMVALUES_TX:
        {
            PTxReadEepromValuesRequest pRequest = (PTxReadEepromValuesRequest) pIOBuffer;
            pRequest->error = IT9507_readEepromValues (state, pRequest->registerAddress, pRequest->bufferLength, pRequest->buffer);

            break;
        }
        case IOCTL_ITE_DEMOD_READREGISTERBITS_TX:
        {
            PTxReadRegisterBitsRequest pRequest = (PTxReadRegisterBitsRequest) pIOBuffer;
            pRequest->error = it950x_rd_regbits (state, pRequest->processor, pRequest->registerAddress, pRequest->position, pRequest->length, pRequest->value);

            break;
        }        
        case IOCTL_ITE_DEMOD_GETGAINRANGE_TX:
        {
            PGetGainRangeRequest pRequest = (PGetGainRangeRequest) pIOBuffer;        
            pRequest->error = IT9507_getGainRange (state, pRequest->frequency, pRequest->bandwidth, pRequest->maxGain, pRequest->minGain);
            break;
        }
        case IOCTL_ITE_DEMOD_GETTPS_TX:
        {
            PGetTPSRequest pRequest = (PGetTPSRequest) pIOBuffer;        
            pRequest->error = IT9507_getTPS (state, pRequest->pTps);
            break;
        }
        case IOCTL_ITE_DEMOD_SETTPS_TX:
        {
            PSetTPSRequest pRequest = (PSetTPSRequest) pIOBuffer;        
            pRequest->error = IT9507_setTPS (state, pRequest->tps);
            break;
        }
        case IOCTL_ITE_DEMOD_GETOUTPUTGAIN_TX:
        {
            PGetOutputGainRequest pRequest = (PGetOutputGainRequest) pIOBuffer;        
            pRequest->error = IT9507_getOutputGain (state, pRequest->gain);
            break;
        }
        case IOCTL_ITE_DEMOD_SENDHWPSITABLE_TX:
        {
            PSendHwPSITableRequest pRequest = (PSendHwPSITableRequest) pIOBuffer;        
            pRequest->error = IT9507_sendHwPSITable (state, pRequest->pbuffer);
            break;
        }
        case IOCTL_ITE_DEMOD_ACCESSFWPSITABLE_TX:
        {
            PAccessFwPSITableRequest pRequest = (PAccessFwPSITableRequest) pIOBuffer;        
            pRequest->error = IT9507_accessFwPSITable (state, pRequest->psiTableIndex, pRequest->pbuffer);
            break;
        }        
        case IOCTL_ITE_DEMOD_SETFWPSITABLETIMER_TX:
        {
            PSetFwPSITableTimerRequest pRequest = (PSetFwPSITableTimerRequest) pIOBuffer;        
            pRequest->error = IT9507_setFwPSITableTimer (state, pRequest->psiTableIndex, pRequest->timer);
            break;
        }        
        case IOCTL_ITE_DEMOD_SETIQTABLE_TX:
        {
            PTxSetIQTableRequest pRequest = (PTxSetIQTableRequest) pIOBuffer;        
            pRequest->error = IT9507_setIQtable (state, pRequest->IQ_table_ptr, pRequest->tableGroups);
            printk("Set IQtable group length is %d\n", pRequest->tableGroups);
            break;
        }            

        case IOCTL_ITE_DEMOD_SETDCCALIBRATIONVALUE_TX:
        {
            PTxSetDCCalibrationValueRequest pRequest = (PTxSetDCCalibrationValueRequest) pIOBuffer;        
            pRequest->error = IT9507_setDCCalibrationValue (state, pRequest->dc_i, pRequest->dc_q);
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
	u32 version = 0;
        u32 cmdVersion = 0; 
	u8 deviceType;

	PDC->state.udev = udev;
	dev_set_drvdata(&udev->dev, PDC);

	deb_data("- Enter %s Function -\n",__FUNCTION__);

	mutex_lock(&mymutex);
	error = IT9507_getFirmwareVersion (&PDC->state, Processor_LINK, &version);

    	if (version != 0) {
        	PDC->state.booted = true;
    	} 
    	else {
        	PDC->state.booted = false;
    	}

        if (error)  // TODO: Check before using value of version
        {
            deb_data("IT9507_getFirmwareVersion fail : 0x%08x\n", error);
	    errcount++;
            goto Exit;
        }

	//************* Set Device init Info *************//
	if (bBoot)
	{
		PDC->state.frequency = 666000;
		PDC->state.bandwidth = 8000;

        	error =DRV_GetEEPROMConfig(PDC);
        	if (error)
        	{
            		deb_data("DRV_GetEEPROMConfig fail : 0x%08x\n", error);
			errcount++;
            		goto Exit;
        	}
	}//bBoot
	
	if(PDC->state.booted) //warm-boot/(S1)
	{
		//tuner power on
		error = it950x_wr_regbits(&PDC->state, Processor_LINK,  p_reg_top_gpioh7_o, reg_top_gpioh7_o_pos, reg_top_gpioh7_o_len, 1);
		if(error) deb_data("DRV_TunerWakeup fail!\n"); 
	}

	// Start of DL_Initialize

	if(EagleUser_getDeviceType(&PDC->state, &deviceType) != 0)
		printk("- EagleUser_getDeviceType fail -\n");

	if(IT9507_setSlaveIICAddress(&PDC->state, SLAVE_DEMOD_2WIREADDR) != 0)
		printk("- IT9507_setSlaveIICAddress fail -\n");	
	
	
	if(PDC->state.booted) //from Standard_setBusTuner() > Standard_getFirmwareVersion()
    	{
        	//use "Command_QUERYINFO" to get fw version 
        	error = IT9507_getFirmwareVersion(&PDC->state, Processor_OFDM, &cmdVersion);
        	if(error) deb_data("DRV_Initialize : IT9507_getFirmwareVersion : error = 0x%08u\n", error);

        	if(cmdVersion != DVB_OFDM_VERSION)
        	{
            		deb_data("Reboot: Outside Fw = 0x%x, Inside Fw = 0x%x", DVB_OFDM_VERSION, cmdVersion);
            		error = IT9507_TXreboot(&PDC->state);
            		if(error) 
            		{
                		deb_data("IT9507_reboot : error = 0x%08u\n", error);
                		goto Exit;
            		}
            		else {
			  goto Exit;
            		}
        	}
        	else
        	{
            		deb_data("	Fw version is the same!\n");
  	      		error = Error_NO_ERROR;
        	}
	}//PDC->IT950x.booted

	//			case StreamType_DVBT_DATAGRAM:
	deb_data("    StreamType_DVBT_DATAGRAM\n");
	error = IT9507_initialize (&PDC->state, 0);
				 
	if (error) deb_data("IT950x_initialize _Device initialize fail : 0x%08x\n", error);
	else deb_data("    Device initialize TX Ok\n");

    IT9507_getFirmwareVersion (&PDC->state, Processor_OFDM, &cmdVersion);
    deb_data("    FwVer OFDM = 0x%x, ", cmdVersion);
    IT9507_getFirmwareVersion (&PDC->state, Processor_LINK, &cmdVersion);
    deb_data("FwVer LINK = 0x%x\n", cmdVersion);
    
	// End of DL_Initialize

	if (error) {
		deb_data("DL_Initialize fail! 0x%08x\n", error);
		errcount++;
		goto Exit;
	}

	deb_data("	%s success \n",__FUNCTION__);


	error = it950x_wr_reg(&PDC->state, Processor_OFDM, 0xF7C6, 0x1);
	if(error)	printk( "AirHD Reg Write fail!\n");
	else printk( "AirHD Reg Write ok!\n");
	
Exit:
	mutex_unlock(&mymutex);
	
	if(errcount)
        printk( "[Device_init] Error %d\n", errcount);
	return (error);
}
