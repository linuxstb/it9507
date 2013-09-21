
#include "af9035u2iimpl.h"

#ifdef UNDER_CE

Dword Af9035u2i_getDriver (
    IN  Modulator*    modulator,
    OUT Handle*         handle
) {
    return (Error_NO_ERROR);
}


Dword Af9035u2i_exitDriver (
    IN  Modulator*    modulator
) {
    return (Error_NO_ERROR);
}

Dword Af9035u2i_writeControlBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    IN  Byte*           buffer
) {
    return (Error_NO_ERROR);
}

Dword Af9035u2i_readControlBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
) {
    return (Error_NO_ERROR);
}

Dword Af9035u2i_readDataBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
) {
    return (Error_NO_ERROR);
}

#else

Byte CmdSequence_af9035u2i = 0;
Byte InterfaceIndex_af9035u2i = 2;
//Byte SlaveAddress_af9035u2i = 0x38;


Dword Af9035u2i_addChecksum (
    IN  Modulator*    modulator,
    OUT Dword*          bufferLength,
    OUT Byte*           buffer
) {
    Dword error  = ModulatorError_NO_ERROR;
    Dword loop   = (*bufferLength - 1) / 2;
    Dword remain = (*bufferLength - 1) % 2;
    Dword i;
    Word  checksum = 0;
	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);

    for (i = 0; i < loop; i++)
        checksum = checksum + (Word) (buffer[2 * i + 1] << 8) + (Word) (buffer[2 * i + 2]);
    if (remain)
        checksum = checksum + (Word) (buffer[*bufferLength - 1] << 8);
    
    checksum = ~checksum;
    buffer[*bufferLength]     = (Byte) ((checksum & 0xFF00) >> 8);
    buffer[*bufferLength + 1] = (Byte) (checksum & 0x00FF);
    buffer[0]                 = (Byte) (*bufferLength + 1);
    *bufferLength            += 2;

    return (error);
}


Dword Af9035u2i_removeChecksum (
    IN  Modulator*    modulator,
    OUT Dword*          bufferLength,
    OUT Byte*           buffer
) {
    Dword error    = ModulatorError_NO_ERROR;
    Dword loop     = (*bufferLength - 3) / 2;
    Dword remain   = (*bufferLength - 3) % 2;
    Dword i;
    Word  checksum = 0;
    Byte  bufferTemp[63];

	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);

    for (i = 0; i < *bufferLength; i++)
        bufferTemp[i] = buffer[i];

    for (i = 0; i < loop; i++)
        checksum = checksum + (Word) (bufferTemp[2 * i + 1] << 8) + (Word) (bufferTemp[2 * i + 2]);
    if (remain)
        checksum = checksum + (Word) (bufferTemp[*bufferLength - 3] << 8);    
    
    checksum = ~checksum;
    if (((Word)(bufferTemp[*bufferLength - 2] << 8) + (Word)(bufferTemp[*bufferLength - 1])) != checksum) {
        error = ModulatorError_WRONG_CHECKSUM;
        goto exit;
    }
    if (bufferTemp[2])
        error = ModulatorError_AF9035U2I | bufferTemp[2];

    for (i = 0; i < *bufferLength - 5; i++)
        buffer[i] = bufferTemp[i + 3];

    *bufferLength -= 5;

exit :
    return (error);
}


Handle Af9035u2i_handle = NULL;
bool (__cdecl *Af9035u2i_initialize) (
);
void (__cdecl *Af9035u2i_finalize) (
);
bool (__cdecl *Af9035u2i_writeControl) (
    BYTE*       poutBuf,
    ULONG       WriteLen,
    ULONG*      pnBytesWrite
);
bool (__cdecl *Af9035u2i_readControl) (
    BYTE*       pinBuf,
    ULONG       ReadLen,
    ULONG*      pnBytesRead
);
bool (__cdecl *Af9035u2i_readData) (
    BYTE*       pinBuf,
    ULONG       ReadLen
);


Dword Af9035u2i_getDriver (
    IN  Modulator*    modulator,
    OUT Handle*         handle
) {
    Dword error = ModulatorError_NO_ERROR;
    HINSTANCE instance = NULL;
	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);

    instance = LoadLibrary ("AFBDAEX.dll");
    Af9035u2i_initialize = (bool (__cdecl *) (
            )) GetProcAddress (instance, "af35_init");
    Af9035u2i_finalize = (void (__cdecl *) (
            )) GetProcAddress (instance, "af35_exit");
    Af9035u2i_writeControl = (bool (__cdecl *) (
                    BYTE*       poutBuf,
                    ULONG       WriteLen,
                    ULONG*      pnBytesWrite
            )) GetProcAddress (instance, "af35_WriteBulkData");
    Af9035u2i_readControl = (bool (__cdecl *) (
                    BYTE*       pinBuf,
                    ULONG       ReadLen,
                    ULONG*      pnBytesRead
            )) GetProcAddress (instance, "af35_ReadBulkData");
    Af9035u2i_readData = (bool (__cdecl *) (
                    BYTE*       pinBuf,
                    ULONG       ReadLen
            )) GetProcAddress (instance, "af35_GetTsData");

    if (!Af9035u2i_initialize ())
        error = ModulatorError_DRIVER_INVALID;
    else
        *handle = (Handle) instance;

    return (error);
}


Dword Af9035u2i_exitDriver (
    IN  Modulator*    modulator
) {
    Dword error = ModulatorError_NO_ERROR;
	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);

    if (Af9035u2i_finalize != NULL)
        Af9035u2i_finalize ();
    Af9035u2i_handle = NULL;
    return (error);
}


Dword Af9035u2i_writeControlBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    IN  Byte*           buffer
) {
    Dword   error = ModulatorError_NO_ERROR;
    ULONG   writeLength;
    ULONG   readLength;
    Byte    buffer_af9035u2i[63];
    Word    command_af9035u2i;
    Byte    i;

    if (Af9035u2i_handle == NULL) {
        error = Af9035u2i_getDriver (modulator, &Af9035u2i_handle);
        if (error) goto exit;
    }

    command_af9035u2i   = IT9507Cmd_buildCommand (Command_GENERIC_WRITE, Processor_LINK);
    buffer_af9035u2i[1] = (Byte) (command_af9035u2i >> 8);
    buffer_af9035u2i[2] = (Byte) command_af9035u2i;
    buffer_af9035u2i[3] = (Byte) CmdSequence_af9035u2i++;
    buffer_af9035u2i[4] = (Byte) bufferLength;
    buffer_af9035u2i[5] = InterfaceIndex_af9035u2i;
    buffer_af9035u2i[6] =  modulator->i2cAddr;
    for (i = 0; i < bufferLength; i++)
        buffer_af9035u2i[i + 7] = buffer[i];

    bufferLength += 7;
    error = Af9035u2i_addChecksum (modulator, &bufferLength, buffer_af9035u2i);
    if (error) goto exit;

    if (!Af9035u2i_writeControl (buffer_af9035u2i, bufferLength, &writeLength)) {
        error = ModulatorError_INTERFACE_FAIL;
        goto exit;
    }

    bufferLength = 5;
    if (!Af9035u2i_readControl (buffer_af9035u2i, bufferLength, &readLength)) {
        error = ModulatorError_INTERFACE_FAIL;
		 goto exit;
    }

    error = Af9035u2i_removeChecksum (modulator, &bufferLength, buffer_af9035u2i);

    for (i = 0; i < bufferLength; i++)
        buffer[i] = buffer_af9035u2i[i];

exit :
    return (error);
}


Dword Af9035u2i_readControlBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
) {
    Dword   error = ModulatorError_NO_ERROR;
    ULONG   writeLength;
    ULONG   readLength;
    Byte    buffer_af9035u2i[63];
    Word    command_af9035u2i;
    Byte    i;
    Dword   writeBufferLength_9035u2i = 7;

    if (Af9035u2i_handle == NULL) {
        error = Af9035u2i_getDriver (modulator, &Af9035u2i_handle);
        if (error) goto exit;
    }

    command_af9035u2i   = IT9507Cmd_buildCommand (Command_GENERIC_READ, Processor_LINK);
    buffer_af9035u2i[1] = (Byte) (command_af9035u2i >> 8);
    buffer_af9035u2i[2] = (Byte) command_af9035u2i;
    buffer_af9035u2i[3] = (Byte) CmdSequence_af9035u2i++;
    buffer_af9035u2i[4] = (Byte) bufferLength;
    buffer_af9035u2i[5] = InterfaceIndex_af9035u2i;
    buffer_af9035u2i[6] =  modulator->i2cAddr;

    error = Af9035u2i_addChecksum (modulator, &writeBufferLength_9035u2i, buffer_af9035u2i);
    if (error) goto exit;

    if (!Af9035u2i_writeControl (buffer_af9035u2i, writeBufferLength_9035u2i, &writeLength)) {
        error = ModulatorError_INTERFACE_FAIL;
        goto exit;
    }

    bufferLength += 5;
    if (!Af9035u2i_readControl (buffer_af9035u2i, bufferLength, &readLength)) {
        error = ModulatorError_INTERFACE_FAIL;
        goto exit;
    }

    error = Af9035u2i_removeChecksum (modulator, &bufferLength, buffer_af9035u2i);

    for (i = 0; i < bufferLength; i++)
        buffer[i] = buffer_af9035u2i[i];

exit :
    return (error);
}


Dword Af9035u2i_readDataBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
) {
    Dword error = ModulatorError_NO_ERROR;

    if (Af9035u2i_handle == NULL) {
        error = Af9035u2i_getDriver (modulator, &Af9035u2i_handle);
        if (error) goto exit;
    }
    if (!Af9035u2i_readData (buffer, bufferLength)) {
        EagleUser_delay (modulator, 1000);
        if (!Af9035u2i_readData (buffer, bufferLength))
            error = ModulatorError_INTERFACE_FAIL;
    }

exit :
    return (error);
}

Dword Af9035u2i_read9035u2iRegisters (
    IN  Modulator*    modulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Dword           readBufferLength,
    OUT Byte*           readBuffer
) {
    Dword       error = ModulatorError_NO_ERROR;
    Word        command;
    Byte        buffer[255];
    Dword       bufferLength;
    Dword       i, k;
    Dword       maxFrameSize;
	ULONG   writeLength;
	ULONG   readLength;
	Byte registerAddressLength;
    EagleUser_enterCriticalSection (modulator);
	chip = 0;
	if (Af9035u2i_handle == NULL) {
        error = Af9035u2i_getDriver (modulator, &Af9035u2i_handle);
        if (error) goto exit;
    }


    if (readBufferLength == 0) goto exit;
    
    maxFrameSize = EagleUser_MAXFRAMESIZE;

    if ((readBufferLength + 5) > EagleUser_MAX_PKT_SIZE) {
        error = ModulatorError_INVALID_DATA_LENGTH;
        goto exit;
    }

    if ((readBufferLength + 5) > maxFrameSize) {
        error = ModulatorError_INVALID_DATA_LENGTH;
        goto exit;
    }

	if (processor == Processor_LINK) {
		if (registerAddress > 0x000000FF) {
			registerAddressLength = 2;
		} else {
			registerAddressLength = 1;
		}
	} else {
		registerAddressLength = 2;
	}


 
    /** add frame header */
    command   = IT9507Cmd_buildCommand (Command_REG_DEMOD_READ, processor);
    buffer[1] = (Byte) (command >> 8);
    buffer[2] = (Byte) command;
    buffer[3] = 0;
    buffer[4] = (Byte) readBufferLength;
    buffer[5] = registerAddressLength;
    buffer[6] = (Byte) (registerAddress >> 24); /** Get first byte of reg. address  */
    buffer[7] = (Byte) (registerAddress >> 16); /** Get second byte of reg. address */
    buffer[8] = (Byte) (registerAddress >> 8);  /** Get third byte of reg. address  */
    buffer[9] = (Byte) (registerAddress);       /** Get fourth byte of reg. address */

    /** add frame check-sum */
    bufferLength = 10;
    error = Af9035u2i_addChecksum (modulator, &bufferLength, buffer);
    if (error) goto exit;


    /** send frame */
    i = 0;
   if (!Af9035u2i_writeControl (buffer, bufferLength, &writeLength)) {
        error = ModulatorError_INTERFACE_FAIL;
        goto exit;
    }

   
   /** get reply frame */
   bufferLength = 5 + readBufferLength;
   if (!Af9035u2i_readControl (buffer, bufferLength, &readLength)) {
        error = ModulatorError_INTERFACE_FAIL;
        goto exit;
   }

   error = Af9035u2i_removeChecksum (modulator, &bufferLength, buffer);

    for (k = 0; k < readBufferLength; k++) {
        readBuffer[k] = buffer[k + 3];
    }


exit :
    EagleUser_leaveCriticalSection (modulator);
    return (error);
}


Dword Af9035u2i_write9035u2iRegisters (
    IN  Modulator*    modulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Dword           writeBufferLength,
    IN  Byte*           writeBuffer
) {
    Dword       error = ModulatorError_NO_ERROR;
    Word        command;
    Byte        buffer[255];
    Dword       bufferLength;    
    Dword       i;
    Dword       maxFrameSize;

	ULONG   writeLength;
	ULONG   readLength;
	Byte    registerAddressLength;


    EagleUser_enterCriticalSection (modulator);

	if (Af9035u2i_handle == NULL) {
        error = Af9035u2i_getDriver (modulator, &Af9035u2i_handle);
        if (error) goto exit;
    }

	chip = 0;
    if (writeBufferLength == 0) goto exit;
    
    maxFrameSize = EagleUser_MAXFRAMESIZE;

    if ((writeBufferLength + 12) > maxFrameSize) {
        error = ModulatorError_INVALID_DATA_LENGTH;
        goto exit;
    }


	if (processor == Processor_LINK) {
		if (registerAddress > 0x000000FF) {
			registerAddressLength = 2;
		} else {
			registerAddressLength = 1;
		}
	} else {
		registerAddressLength = 2;
	}


    /** add frame header */
    command   = IT9507Cmd_buildCommand (Command_REG_DEMOD_WRITE, processor);
    buffer[1] = (Byte) (command >> 8);
    buffer[2] = (Byte) command;
    buffer[3] = (Byte) 0;
    buffer[4] = (Byte) writeBufferLength;
    buffer[5] = (Byte) registerAddressLength;
    buffer[6] = (Byte) ((registerAddress) >> 24); /** Get first byte of reg. address  */
    buffer[7] = (Byte) ((registerAddress) >> 16); /** Get second byte of reg. address */
    buffer[8] = (Byte) ((registerAddress) >> 8);  /** Get third byte of reg. address  */
    buffer[9] = (Byte) (registerAddress );        /** Get fourth byte of reg. address */

    /** add frame data */
    for (i = 0; i < writeBufferLength; i++) {    
        buffer[10 + i] = writeBuffer[i];
    }


//--------------------
	/** add frame check-sum */
    bufferLength = 10 + writeBufferLength;
	error = Af9035u2i_addChecksum (modulator, &bufferLength, buffer);
    if (error) goto exit;

    if (!Af9035u2i_writeControl (buffer, bufferLength, &writeLength)) {
        error = ModulatorError_INTERFACE_FAIL;
        goto exit;
    }

    bufferLength = 5;
    if (!Af9035u2i_readControl (buffer, bufferLength, &readLength)) {
        error = ModulatorError_INTERFACE_FAIL;
		 goto exit;
    }

    error = Af9035u2i_removeChecksum (modulator, &bufferLength, buffer);
    if (error) goto exit;


exit :
    EagleUser_leaveCriticalSection (modulator);
    return (error);
}

Dword Af9035u2i_setSlaveAddress (
    IN  Modulator*    modulator,
	IN  Byte            SlaveAddress
) {
    Dword error = ModulatorError_NO_ERROR;

	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);

     modulator->i2cAddr = SlaveAddress;
    return (error);
}


#endif