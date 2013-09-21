#ifndef __I2CIMPL_H__
#define __I2CIMPL_H__


#include "modulatorType.h"
#include "modulatorError.h"
#include "modulatorUser.h"


extern Word I2c_IO_PORT;


Dword I2c_setClockDelay (
    IN  Modulator*    modulator,
    IN  Dword           dwWClkDelay,
    IN  Dword           dwWDatDelay,
    IN  Dword           dwWAClkDelay,
    IN  Dword           dwRClkDelay,
    IN  Dword           dwRDatDelay,
    IN  Dword           dwRAClkDelay,
    IN  Dword           dwRNClkDelay
);


Dword I2c_setChipAddress (
    IN  Modulator*    modulator,
    IN  Byte            chipAddress
);


Dword I2c_getChipAddress (
    IN  Modulator*    modulator,
    IN  Byte*           chipAddress
);


Dword I2c_getDriver (
    IN  Modulator*    modulator,
    OUT Handle*         handle
);


Dword I2c_writeControlBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    IN  Byte*           buffer
);


Dword I2c_readControlBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
);


Dword TunerI2c_writeControlBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    IN  Byte*           buffer
);


Dword TunerI2c_readControlBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
);

void IntTunerSetSDAHigh (
    IN  Modulator*    modulator
);

void IntTunerSetSDALow (
    IN  Modulator*    modulator
);

void IntTunerSetSCLHigh (
    IN  Modulator*    modulator
);

void IntTunerSetSCLLow (
    IN  Modulator*    modulator
);

void I2c_reset (
    IN Modulator*     modulator
);

#endif