#include "IT9507.h"
#include <linux/delay.h>

u8 IT9507Cmd_sequence = 0;
//u8 CmdSequence_EagleSlaveIIC = 0;

const u8 Eagle_bitMask[Eagle_MAX_BIT] = {
	0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF
};

/** local functions */
static unsigned int c_fN_min[9] = {
	53000, 74000, 111000, 148000, 222000, 296000, 445000, 573000, 950000
};

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


u32 IT9507Cmd_addChecksum (
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


u32 IT9507Cmd_removeChecksum (
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

u32 IT9507_writeRegister (
    IN  Modulator*    modulator,
    IN  Processor       processor,
    IN  u32           registerAddress,
    IN  u8            value
) {
   	return (IT9507_writeRegisters(modulator, processor, registerAddress, 1, &value));
}


u32 IT9507_writeRegisters (
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



u32 IT9507_writeGenericRegisters (
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


u32 IT9507_writeRegisterBits (
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


u32 IT9507_readRegister (
    IN  Modulator*    modulator,
    IN  Processor       processor,
    IN  u32           registerAddress,
    OUT u8*           value
) {
    return (IT9507_readRegisters (modulator, processor, registerAddress, 1, value));
}


u32 IT9507_readRegisters (
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


u32 IT9507_readGenericRegisters (
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
    IN  TsInterface   streamType,
	IN  u8            busId,
	IN  u8            i2cAddr
) {

	u32 error = ModulatorError_NO_ERROR;

	u32 version = 0;
	u8 c1_default_value[2],c2_default_value[2],c3_default_value[2];

	modulator->frequency = 642000;	
	modulator->calibrationInfo.ptrIQtableEx =  IQ_fixed_table0;
	modulator->calibrationInfo.tableGroups = IQ_TABLE_NROW;
	modulator->busId = busId; 
	modulator->i2cAddr = i2cAddr;

	error = EagleUser_setBus(modulator);
    if (error) goto exit;

	if (modulator->busId == 0xFF) {
		goto exit;
	}


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
	error = IT9507_setTsInterface (modulator, streamType);
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


u32 IT9507_finalize (
    IN  Modulator*    modulator
) {
	u32 error = ModulatorError_NO_ERROR;

	error = EagleUser_Finalize(modulator);

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


u32 IT9507_setTsInterface (
    IN  Modulator*    modulator,
    IN  TsInterface   streamType
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

	if(streamType == PARALLEL_TS_INPUT){
	/** Enable tsip */
		error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_eagle_reg_tsip_en, eagle_reg_tsip_en_pos, eagle_reg_tsip_en_len, 1);
		if (error) goto exit;
		error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_eagle_reg_tsis_en, eagle_reg_tsis_en_pos, eagle_reg_tsis_en_len, 0);
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_OFDM, p_eagle_reg_ts_in_src, 1);
		if (error) goto exit;
	}else{
		/** Enable tsis */
		error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_eagle_reg_tsip_en, eagle_reg_tsip_en_pos, eagle_reg_tsip_en_len, 0);
		if (error) goto exit;
		error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_eagle_reg_tsis_en, eagle_reg_tsis_en_pos, eagle_reg_tsis_en_len, 1);
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_OFDM, p_eagle_reg_ts_in_src, 0);
		if (error) goto exit;
	}

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

	modulator->tsInterfaceType = streamType;

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
		if (error) goto exit;		
		
		if (modulator->busId == Bus_USB) 
			goto exit;

		EagleUser_delay (modulator, 10);

		version = 1;
		for (i = 0; i < 30; i++) {
			error = IT9507_getFirmwareVersion (modulator, Processor_LINK, &version);
			if (error == ModulatorError_NO_ERROR) break;
			EagleUser_delay (modulator, 10);
		}
		if (error) 
			goto exit;
		
		if (version != 0)
			error = ModulatorError_REBOOT_FAIL;
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
		if (modulator->busId == Bus_USB) {
			error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_eagle_reg_afe_mem0, 3, 1, 0);
			if (error) goto exit;
			error = IT9507_writeRegister (modulator, Processor_OFDM, p_eagle_reg_dyn0_clk, 0);
			if (error) goto exit;
		} 

		/** Fixed current leakage */
		switch (modulator->busId) {
			case Bus_USB :
				error = IT9507_writeRegisterBits (modulator, Processor_LINK, p_eagle_reg_top_hostb_mpeg_ser_mode, eagle_reg_top_hostb_mpeg_ser_mode_pos, eagle_reg_top_hostb_mpeg_ser_mode_len, 0);
				if (error) goto exit;
				/** Disable HostB parallel */  
				error = IT9507_writeRegisterBits (modulator, Processor_LINK, p_eagle_reg_top_hostb_mpeg_par_mode, eagle_reg_top_hostb_mpeg_par_mode_pos, eagle_reg_top_hostb_mpeg_par_mode_len, 0);
				if (error) goto exit;
			
				break;
		}
	} else {
		/** Power down case */
		if (modulator->busId == Bus_USB) {
			
			error = IT9507_writeRegisterBits (modulator, Processor_OFDM, p_eagle_reg_afe_mem0, 3, 1, 1);
		
		} 

		/** Fixed current leakage */
		switch (modulator->busId) {
			case Bus_USB :			
				/** Enable HostB parallel */
				error = IT9507_writeRegisterBits (modulator, Processor_LINK, p_eagle_reg_top_hostb_mpeg_ser_mode, eagle_reg_top_hostb_mpeg_ser_mode_pos, eagle_reg_top_hostb_mpeg_ser_mode_len, 0);
				if (error) goto exit;
				error = IT9507_writeRegisterBits (modulator, Processor_LINK, p_eagle_reg_top_hostb_mpeg_par_mode, eagle_reg_top_hostb_mpeg_par_mode_pos, eagle_reg_top_hostb_mpeg_par_mode_len, 1);
				if (error) goto exit;						
				
				break;
		}
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



