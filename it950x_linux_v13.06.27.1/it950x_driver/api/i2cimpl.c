#include "i2cimpl.h"

#ifdef UNDER_CE

#include <ceddk.h>
#define IOCTL_CHIP_SELECT       CTL_CODE(0, 5, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_CHIP_INTERRUPT    CTL_CODE(0, 6, METHOD_BUFFERED, FILE_ANY_ACCESS)


HANDLE I2c_handle = NULL;
Byte I2c_address = 0x38;


Dword I2c_setClockDelay (
    IN  Modulator*    modulator,
    IN  Dword           dwWClkDelay,
    IN  Dword           dwWDatDelay,
    IN  Dword           dwWAClkDelay,
    IN  Dword           dwRClkDelay,
    IN  Dword           dwRDatDelay,
    IN  Dword           dwRAClkDelay,
    IN  Dword           dwRNClkDelay
) {
    return (ModulatorError_NO_ERROR);
}


Dword I2c_setChipAddress (
    IN  Modulator*    modulator,
    IN  Byte            chipAddress
) {
    Dword error = ModulatorError_NO_ERROR;
    Bool result;
    DWORD returnBytes;

    if (I2c_handle == NULL) {
        error = I2c_getDriver (modulator, &I2c_handle);
        if (error) goto exit;
    }
    result = DeviceIoControl (
                    I2c_handle,
                    IOCTL_CHIP_SELECT,
                    (PBYTE) &chipAddress,
                    sizeof (BYTE),
                    NULL,
                    0,
                    &returnBytes,
                    NULL);
    if (!result)
        error = ModulatorError_INTERFACE_FAIL;

    I2c_address = chipAddress;

exit :
    return (error);
}


Dword I2c_getChipAddress (
    IN  Modulator*    modulator,
    IN  Byte*           chipAddress
) {
    Dword error = ModulatorError_NO_ERROR;

    *chipAddress = I2c_address;
    return (error);
}


Dword I2c_getDriver (
    IN  Modulator*    modulator,
    OUT Handle*         handle
) {
    Dword error = ModulatorError_NO_ERROR;
    WCHAR szComPort[30];

    wsprintf (szComPort, L"I2C%d:", 1);
    *handle = CreateFile (szComPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (*handle == NULL)
        error = ModulatorError_DRIVER_INVALID;

    return (error);
}


Dword I2c_writeControlBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    IN  Byte*           buffer
)
{
    Dword error = ModulatorError_NO_ERROR;
    Bool result;
    DWORD returnBytes;

    if (I2c_handle == NULL) {
        error = I2c_getDriver (modulator, &I2c_handle);
        if (error) goto exit;
    }
    result = WriteFile (
                    I2c_handle,
                    (LPVOID) buffer,
                    bufferLength,
                    &returnBytes,
                    NULL);
    if (!result)
        error = ModulatorError_INTERFACE_FAIL;

exit :
    return (error);
}


Dword I2c_readControlBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
)
{
    Dword   error = ModulatorError_NO_ERROR;
    Bool result;
    DWORD returnBytes;

    if (I2c_handle == NULL) {
        error = I2c_getDriver (modulator, &I2c_handle);
        if (error) goto exit;
    }
    result = ReadFile (
                    I2c_handle,
                    (LPVOID) buffer,
                    bufferLength,
                    &returnBytes,
                    NULL);

exit :
    return (error);
}

#else

#include "winio.h"
#define USE_WINIO
Handle I2c_handle = NULL;
Byte Port0Value = 0x66;
Byte Port1Value = 0x04;
Dword I2c_writeClockDelay  = 0x400;
Dword I2c_writeDataDelay  = 0x000;
Dword I2c_writeAckClockDelay = 0x400;
Dword I2c_readClockDelay  = 0x400;
Dword I2c_readDataDelay  = 0x000;
Dword I2c_readAckClockDelay = 0x400;
Dword I2c_roundClockDelay = 0x400;

Word I2c_IO_PORT = 0x378;
Word I2c_OUT_CLOCK = 0x40;
Word I2c_OUT_DATA = 0x20;
Word I2c_IN_DATA = 0x10;
#define I2c_SDA_WRITE I2c_IO_PORT
#define I2c_SDA_READ I2c_IO_PORT + 1
#define I2c_SCL_WRITE I2c_IO_PORT

Word Tuner_I2c_OUT_CLOCK = 0x04;
Word Tuner_I2c_OUT_DATA = 0x02;
Word Tuner_I2c_IN_DATA = 0x01;

#define I2c_Reset I2c_IO_PORT + 2
Word I2c_IO_Reset = 0x01;


short _stdcall Inp32(short PortAddress);
void _stdcall Out32(short PortAddress, short data);


short MyInp32(short PortAddress)
{
    Dword value;
#ifdef USE_WINIO
    GetPortVal (PortAddress, &value, 1);
#else
    value = Inp32(PortAddress);
#endif
    return ((Byte)value);
}

void MyOut32(short PortAddress, short data)
{
#ifdef USE_WINIO
    SetPortVal (PortAddress, (Dword) data, 1);
#else
    Out32(PortAddress, data);
#endif
}


bool IntGetSDA (
    IN  Modulator*    modulator
) {
    Byte ucValue;

	if(modulator == NULL)
			return (False);
    ucValue = (Byte) MyInp32 (I2c_SDA_READ);
    return ((ucValue & I2c_IN_DATA) == I2c_IN_DATA);
}


bool IntTunerGetSDA (
    IN  Modulator*    modulator
) {
    Byte ucValue;
	if(modulator == NULL)
		return (False);
	
    ucValue = (Byte) MyInp32 (I2c_SDA_READ);
    return ((ucValue & Tuner_I2c_IN_DATA) == Tuner_I2c_IN_DATA);
}


bool IntGetSCL (
    IN  Modulator*    modulator
) {
	if(modulator == NULL)
		return (False);
	else
		return (True);
}


bool IntTunerGetSCL (
    IN  Modulator*    modulator
) {
	if(modulator == NULL)
		return (False);
	else
		return (True);
}


void IntSetSDAHigh (
    IN  Modulator*    modulator
){
	if(modulator != NULL){

		Port0Value |= I2c_OUT_DATA;
		MyOut32 (I2c_SDA_WRITE, (Byte) Port0Value);
	}
}


void IntTunerSetSDAHigh (
    IN  Modulator*    modulator
){
	if(modulator != NULL){

    Port0Value |= Tuner_I2c_OUT_DATA;
    MyOut32 (I2c_SDA_WRITE, (Byte) Port0Value);
	}
}


void IntSetSDALow (
    IN  Modulator*    modulator
) {
	if(modulator != NULL){

		Port0Value &= ~I2c_OUT_DATA;
		MyOut32 (I2c_SDA_WRITE, (Byte) Port0Value);
	}
}


void IntTunerSetSDALow (
    IN  Modulator*    modulator
) {
	if(modulator != NULL){

		Port0Value &= ~Tuner_I2c_OUT_DATA;
		MyOut32 (I2c_SDA_WRITE, (Byte) Port0Value);
	}
}


void IntSetSCLHigh (
    IN  Modulator*    modulator
) {
	if(modulator != NULL){
		Port0Value |= I2c_OUT_CLOCK;
		MyOut32 (I2c_SCL_WRITE, (Byte) Port0Value);
	}
}


void IntTunerSetSCLHigh (
    IN  Modulator*    modulator
) {
	if(modulator != NULL){

		Port0Value |= Tuner_I2c_OUT_CLOCK;
		MyOut32 (I2c_SCL_WRITE, (Byte) Port0Value);
	}
}


void IntSetSCLLow (
    IN  Modulator*    modulator
) {
	if(modulator != NULL){

		Port0Value &= ~I2c_OUT_CLOCK;
		MyOut32 (I2c_SCL_WRITE, (Byte) Port0Value);
	}
}


void IntTunerSetSCLLow (
    IN  Modulator*    modulator
) {
	if(modulator != NULL){

		Port0Value &= ~Tuner_I2c_OUT_CLOCK;
		MyOut32 (I2c_SCL_WRITE, (Byte) Port0Value);
	}
}


Dword IntSetDataHigh (
    IN  Modulator*    modulator
) {
	
    Dword error = ModulatorError_NO_ERROR;
    Word wait;
	wait=0;
	if(modulator == NULL)
			return (ModulatorError_NULL_HANDLE_PTR);
    IntSetSDAHigh (modulator);

#ifndef USE_WINIO
    do {
        EagleUser_delay (modulator, 0);
        if (IntGetSDA (modulator))
        return (error);
    } while (wait++ < 5);

    error = ModulatorError_I2C_DATA_HIGH_FAIL;
#endif
    return (error);
}


Dword IntTunerSetDataHigh (
    IN  Modulator*    modulator
) {
    Dword error = ModulatorError_NO_ERROR;
    Word wait=0;

	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);

    IntTunerSetSDAHigh (modulator);
    do {
        EagleUser_delay (modulator, 0);
        if (IntTunerGetSDA (modulator))
        return (error);
    } while (wait++ < 5);

    error = ModulatorError_I2C_DATA_HIGH_FAIL;
    return (error);
}


Dword IntSetDataLow (
    IN  Modulator*    modulator
) {
    Dword error = ModulatorError_NO_ERROR;
	Word wait ;
	wait = 0;
	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);    

    IntSetSDALow (modulator);

#ifndef USE_WINIO
    do {
        EagleUser_delay (modulator, 0);
        if (!IntGetSDA (modulator))
        return (error);
    } while (wait++ < 5);

    error = ModulatorError_I2C_DATA_LOW_FAIL;
#endif
    return (error);
}


Dword IntTunerSetDataLow (
    IN  Modulator*    modulator
) {
    Dword error = ModulatorError_NO_ERROR;
	Word wait = 0;

	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);    

    IntTunerSetSDALow (modulator);
    do {
        EagleUser_delay (modulator, 0);
        if (!IntTunerGetSDA (modulator))
        return (error);
    } while (wait++ < 5);

    error = ModulatorError_I2C_DATA_LOW_FAIL;
    return (error);
}


void IntSetClockHigh (
    IN  Modulator*    modulator
) {
	if(modulator != NULL)
		IntSetSCLHigh (modulator);
}


void IntTunerSetClockHigh (
    IN  Modulator*    modulator
) {
	if(modulator != NULL)
		IntTunerSetSCLHigh (modulator);
}


void IntSetClockLow (
    IN  Modulator*    modulator
) {
	if(modulator != NULL)
		IntSetSCLLow (modulator);
}


void IntTunerSetClockLow (
    IN  Modulator*    modulator
) {
	if(modulator != NULL)
		IntTunerSetSCLLow (modulator);
}


Dword IntStart2Wire (
    IN  Modulator*    modulator
) {
    Dword   error = ModulatorError_NO_ERROR;

	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);

    IntSetClockHigh (modulator);

    EagleUser_delay (modulator, 0);

    error = IntSetDataHigh (modulator);
    if (error) goto exit;

    EagleUser_delay (modulator, 0);

    error = IntSetDataLow (modulator);
    if (error) goto exit;

    EagleUser_delay (modulator, 0);

    IntSetClockLow (modulator);

exit :
    return (error);
}


Dword IntTunerStart2Wire (
    IN  Modulator*    modulator
) {
    Dword   error = ModulatorError_NO_ERROR;

	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);

    IntTunerSetClockHigh (modulator);

    EagleUser_delay (modulator, 0);

    error = IntTunerSetDataHigh (modulator);
    if (error) goto exit;

    EagleUser_delay (modulator, 0);

    error = IntTunerSetDataLow (modulator);
    if (error) goto exit;

    EagleUser_delay (modulator, 0);

    IntTunerSetClockLow (modulator);

exit :
    return (error);
}


Dword IntStop2Wire (
    IN  Modulator*    modulator
) {
    Dword error = ModulatorError_NO_ERROR;

	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);

    error = IntSetDataLow (modulator);
    if (error) goto exit;

    IntSetClockLow (modulator);
    IntSetClockHigh (modulator);

    EagleUser_delay (modulator, 0);

    error = IntSetDataHigh (modulator);
    if (error) goto exit;

exit :
    return (error);
}


Dword IntTunerStop2Wire (
    IN  Modulator*    modulator
) {
    Dword error = ModulatorError_NO_ERROR;

	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);

    error = IntTunerSetDataLow (modulator);
    if (error) goto exit;

    IntTunerSetClockLow (modulator);
    IntTunerSetClockHigh (modulator);

    EagleUser_delay (modulator, 0);

    error = IntTunerSetDataHigh (modulator);
    if (error) goto exit;

exit :
    return (error);
}


Dword IntWriteAck (
    IN  Modulator*    modulator,
    IN  Byte            byte
) {
    Dword error = ModulatorError_NO_ERROR;
    short bit;
    Dword i;
	i = 0;
	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);

    for (bit = 7; bit >= 0; bit--) {
        if (byte & 0x80) {
            error = IntSetDataHigh (modulator);               /** SDA --> HI */
            if (error) {
                goto exit;
            }
        } else {
            error = IntSetDataLow (modulator);                /** SDA --> LO */
            if (error) {
                goto exit;
            }
        }
        byte <<= 1;

#ifndef USE_WINIO
        /** Add delay to extend data clock */
        i = 0;
        while (i < I2c_writeDataDelay) i++;
#endif

        IntSetClockHigh (modulator);                          /** SCL --> HI */

#ifndef USE_WINIO
        /** Add delay to extend i2c clock */
        i = 0;
        while (i < I2c_writeClockDelay) i++;
#endif

        IntSetClockLow (modulator);                           /** SCL --> LO */
    }


    /** ACK phase */
    IntSetSDAHigh (modulator);
    IntSetClockHigh (modulator);                              /** SCL --> HI */

#ifndef USE_WINIO
    /** Add delay to extend i2c clock */
    i = 0 ;
    while (i < I2c_writeAckClockDelay) i++;
#endif

    if (IntGetSDA (modulator)) {
        IntSetClockLow (modulator);                           /** SCL --> LO */
        error = ModulatorError_I2C_WRITE_NO_ACK;
        goto exit;
    }
    IntSetClockLow (modulator);                               /** SCL --> LO */

exit :
    return (error);
}


Dword IntTunerWriteAck (
    IN  Modulator*    modulator,
    IN  Byte            byte
) {
    Dword error = ModulatorError_NO_ERROR;
    short bit;
    Dword i;

	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);

    for (bit = 7; bit >= 0; bit--) {
        if (byte & 0x80) {
            error = IntTunerSetDataHigh (modulator);               /** SDA --> HI */
            if (error) {
                goto exit;
            }
        } else {
            error = IntTunerSetDataLow (modulator);                /** SDA --> LO */
            if (error) {
                goto exit;
            }
        }
        byte <<= 1;

        /** Add delay to extend data clock */
        i = 0;
        while (i < I2c_writeDataDelay) i++;

        IntTunerSetClockHigh (modulator);                          /** SCL --> HI */

        /** Add delay to extend i2c clock */
        i = 0;
        while (i < I2c_writeClockDelay) i++;

        IntTunerSetClockLow (modulator);                           /** SCL --> LO */
    }


    /** ACK phase */
    IntTunerSetSDAHigh (modulator);
    IntTunerSetClockHigh (modulator);                              /** SCL --> HI */


    /** Add delay to extend i2c clock */
    i = 0 ;
    while (i < I2c_writeAckClockDelay) i++;

    if (IntTunerGetSDA (modulator)) {
        IntTunerSetClockLow (modulator);                           /** SCL --> LO */
        error = ModulatorError_I2C_WRITE_NO_ACK;
        goto exit;
    }
    IntTunerSetClockLow (modulator);                               /** SCL --> LO */

exit :
    return (error);
}


Dword IntReadAck (
    IN  Modulator*    modulator,
    IN  Byte*           ucpByte
) {
    Dword error = ModulatorError_NO_ERROR;
    short bit;
    Byte byte = 0;
    Dword   i;
	i = 0;
	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);

    for (bit = 7; bit >= 0; bit--) {
        byte <<= 1;
        IntSetClockHigh (modulator);

        if (IntGetSDA (modulator))     byte++;

#ifndef USE_WINIO
        /** Add delay to extend data clock */
        i = 0;
        while (i < I2c_readDataDelay) i++;
#endif

        IntSetClockLow (modulator);

#ifndef USE_WINIO
        /** Add delay to extend i2c clock */
        i = 0;
        while (i < I2c_readClockDelay) i++;
#endif
    }

    /** ACK phase */
    /** Set SDA low */
    error = IntSetDataLow (modulator);
    if (error) goto exit;

    /** Set SCL high */
    IntSetClockHigh (modulator);                          /** SCL --> HI */

#ifndef USE_WINIO
    /** Add delay to extend i2c clock */
    i = 0;
    while (i < I2c_readAckClockDelay) i++;
#endif

    /** Set SCL low */
    IntSetClockLow (modulator);                           /** SCL --> LO */
    *ucpByte = byte;

    IntSetSDAHigh (modulator);

exit :
    return (error);
}


Dword IntTunerReadAck (
    IN  Modulator*    modulator,
    IN  Byte*           ucpByte
) {
    Dword error = ModulatorError_NO_ERROR;
    short bit;
    Byte byte = 0;
    Dword   i;

	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);

    for (bit = 7; bit >= 0; bit--) {
        byte <<= 1;
        IntTunerSetClockHigh (modulator);

        if (IntTunerGetSDA (modulator))     byte++;

        /** Add delay to extend data clock */
        i = 0;
        while (i < I2c_readDataDelay) i++;

        IntTunerSetClockLow (modulator);

        /** Add delay to extend i2c clock */
        i = 0;
        while (i < I2c_readClockDelay) i++;
    }

    /** ACK phase */
    /** Set SDA low */
    error = IntTunerSetDataLow (modulator);
    if (error) goto exit;

    /** Set SCL high */
    IntTunerSetClockHigh (modulator);                          /** SCL --> HI */


    /** Add delay to extend i2c clock */
    i = 0;
    while (i < I2c_readAckClockDelay) i++;

    /** Set SCL low */
    IntTunerSetClockLow (modulator);                           /** SCL --> LO */
    *ucpByte = byte;

    IntTunerSetSDAHigh (modulator);

exit :
    return (error);
}


Dword IntLastReadAck (
    IN  Modulator*    modulator,
    IN  Byte*           ucpByte
) {
    Dword error = ModulatorError_NO_ERROR;
    short bit;
    Byte byte = 0;
    Dword   i;
	i = 0;

	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);

    for (bit = 7; bit >= 0; bit--) {
        byte <<= 1;
        IntSetClockHigh (modulator);

        if (IntGetSDA (modulator)) byte++;

#ifndef USE_WINIO
        /** Add delay to extend data clock */
        i = 0;
        while (i < I2c_readDataDelay) i++;
#endif

        IntSetClockLow (modulator);

#ifndef USE_WINIO
        /** add delay to extend i2c clock */
        i = 0;
        while (i < I2c_readClockDelay) i++;
#endif
    }

    error = IntSetDataHigh (modulator);
    if (error) goto exit;

    IntSetClockHigh (modulator);

#ifndef USE_WINIO
    /** Add delay to extend i2c clock */
    i = 0;
    while (i < I2c_roundClockDelay) i++;
#endif

    IntSetClockLow (modulator);
    *ucpByte = byte;

    IntSetSDAHigh (modulator);

exit :
    return (error);
}


Dword IntTunerLastReadAck (
    IN  Modulator*    modulator,
    IN  Byte*           ucpByte
) {
    Dword error = ModulatorError_NO_ERROR;
    short bit;
    Byte byte = 0;
    Dword   i;

	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);

    for (bit = 7; bit >= 0; bit--) {
        byte <<= 1;
        IntTunerSetClockHigh (modulator);

        if (IntTunerGetSDA (modulator)) byte++;

        /** Add delay to extend data clock */
        i = 0;
        while (i < I2c_readDataDelay) i++;

        IntTunerSetClockLow (modulator);

        /** add delay to extend i2c clock */
        i = 0;
        while (i < I2c_readClockDelay) i++;
    }

    error = IntTunerSetDataHigh (modulator);
    if (error) goto exit;

    IntTunerSetClockHigh (modulator);

    /** Add delay to extend i2c clock */
    i = 0;
    while (i < I2c_roundClockDelay) i++;

    IntTunerSetClockLow (modulator);
    *ucpByte = byte;

    IntTunerSetSDAHigh (modulator);

exit :
    return (error);
}


Dword I2c_setClockDelay (
    IN  Modulator*    modulator,
    IN  Dword           dwWClkDelay,
    IN  Dword           dwWDatDelay,
    IN  Dword           dwWAClkDelay,
    IN  Dword           dwRClkDelay,
    IN  Dword           dwRDatDelay,
    IN  Dword           dwRAClkDelay,
    IN  Dword           dwRNClkDelay
) {

	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);

    I2c_writeClockDelay  = dwWClkDelay;
    I2c_writeDataDelay  = dwWDatDelay;
    I2c_writeAckClockDelay = dwWAClkDelay;
    I2c_readClockDelay  = dwRClkDelay;
    I2c_readDataDelay  = dwRDatDelay;
    I2c_readAckClockDelay = dwRAClkDelay;
    I2c_roundClockDelay = dwRNClkDelay;
    return (ModulatorError_NO_ERROR);
}


Dword I2c_setChipAddress (
    IN  Modulator*    modulator,
    IN  Byte            chipAddress
) {
    Dword error = ModulatorError_NO_ERROR;

	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);
	else
		modulator->i2cAddr = chipAddress;
    return (error);
}


Dword I2c_getChipAddress (
    IN  Modulator*    modulator,
    IN  Byte*           chipAddress
) {
    Dword error = ModulatorError_NO_ERROR;

	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);
	else
		*chipAddress = modulator->i2cAddr;
    return (error);
}


Dword I2c_getDriver (
    IN  Modulator*    modulator,
    OUT Handle*         handle
) {
    Dword error = ModulatorError_NO_ERROR;
	 HINSTANCE instance = NULL;

	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);
#ifdef USE_WINIO
    instance = LoadLibrary ("WinIo.dll");

    if (!InitializeWinIo())    
        error = ModulatorError_DRIVER_INVALID;
    else
        *handle = (Handle) instance;
#endif
    return (error);
}


Dword I2c_writeControlBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    IN  Byte*           buffer
) {
    Dword   error = ModulatorError_NO_ERROR;
    Dword   i;
	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);

#ifdef USE_WINIO
    if (I2c_handle == NULL) {
        error = I2c_getDriver (modulator, &I2c_handle);
        if (error) goto exit;
    }
#endif
    error = IntStart2Wire (modulator);
    if (error) goto exit;

    for (i = 0; i < 2000; i++) {
        error = IntWriteAck (modulator, modulator->i2cAddr);
        if (error == ModulatorError_NO_ERROR) break;
        IntStop2Wire (modulator);
#ifndef USE_WINIO
        EagleUser_delay (modulator, 1);
#endif
        IntStart2Wire (modulator);
    }
    if (error) goto exit;

    for (i = 0; i < bufferLength; i++) {
        error = IntWriteAck (modulator, *(buffer + i));
        if (error) goto exit;
    }

exit :
    if (error)
        IntStop2Wire (modulator);
    else
        error = IntStop2Wire (modulator);

    return (error);

}


Dword TunerI2c_writeControlBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    IN  Byte*           buffer
) {
    Dword   error = ModulatorError_NO_ERROR;
    Dword   i;

	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);

    error = IntTunerStart2Wire (modulator);
    if (error) goto exit;

    for (i = 0; i < 100; i++) {
        error = IntTunerWriteAck (modulator, modulator->i2cAddr);
        if (error == ModulatorError_NO_ERROR) break;
        IntTunerStop2Wire (modulator);
        EagleUser_delay (modulator, 20);
        IntTunerStart2Wire (modulator);
    }
    if (error) goto exit;

    for (i = 0; i < bufferLength; i++) {
        error = IntTunerWriteAck (modulator, *(buffer + i));
        if (error) goto exit;
    }

exit :
    if (error)
        IntTunerStop2Wire (modulator);
    else
        error = IntTunerStop2Wire (modulator);

    return (error);

}


Dword I2c_readControlBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
) {
    Dword   error = ModulatorError_NO_ERROR;
    Dword   i;

	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);

#ifdef USE_WINIO
    if (I2c_handle == NULL) {
        error = I2c_getDriver (modulator, &I2c_handle);
        if (error) goto exit;
    }
#endif
    error = IntStart2Wire (modulator);
    if (error) goto exit;

    for (i = 0; i < 2000; i++) {
        error = IntWriteAck (modulator, modulator->i2cAddr | 0x01);
        if (error == ModulatorError_NO_ERROR) break;
        IntStop2Wire (modulator);
#ifndef USE_WINIO
        EagleUser_delay (modulator, 1);
#endif
        IntStart2Wire (modulator);
    }
    if (error) goto exit;

    for (i = 0; i < bufferLength; i++) {
        if (i == (bufferLength - 1)) {
            error = IntLastReadAck (modulator, buffer + i);
            if (error) goto exit;
        } else {
            error = IntReadAck (modulator, buffer + i);
            if (error) goto exit;
        }
    }

exit:
    if (error)
        IntStop2Wire (modulator);
    else
        error = IntStop2Wire (modulator);

    return (error);
}


Dword TunerI2c_readControlBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
) {
    Dword   error = ModulatorError_NO_ERROR;
    Dword   i;

	if(modulator == NULL)
		return (ModulatorError_NULL_HANDLE_PTR);


    error = IntTunerStart2Wire (modulator);
    if (error) goto exit;

    for (i = 0; i < 100; i++) {
        error = IntTunerWriteAck (modulator, modulator->i2cAddr | 0x01);
        if (error == ModulatorError_NO_ERROR) break;
        IntTunerStop2Wire (modulator);
        EagleUser_delay (modulator, 20);
        IntTunerStart2Wire (modulator);
    }
    if (error) goto exit;

    for (i = 0; i < bufferLength; i++) {
        if (i == (bufferLength - 1)) {
            error = IntTunerLastReadAck (modulator, buffer + i);
            if (error) goto exit;
        } else {
            error = IntTunerReadAck (modulator, buffer + i);
            if (error) goto exit;
        }
    }

exit:
    if (error)
        IntTunerStop2Wire (modulator);
    else
        error = IntTunerStop2Wire (modulator);

    return (error);
}


void IntSetResetLow (
    IN  Modulator* modulator
) {
	if(modulator != NULL){		
		Port0Value &= ~I2c_IO_Reset;
		MyOut32 (I2c_Reset, (Byte) Port0Value);
	}
}


void IntSetResetHigh (
    IN  Modulator* modulator
) {
	if(modulator != NULL){	
		Port0Value |= I2c_IO_Reset;
		MyOut32 (I2c_Reset, (Byte) Port0Value);
	}
}


void I2c_reset(
    IN Modulator*     modulator
) {

    IntSetResetLow (modulator);

    EagleUser_delay (modulator, 20);

    IntSetResetHigh (modulator);

    EagleUser_delay (modulator, 20);

    IntSetResetLow (modulator);
}

#endif
