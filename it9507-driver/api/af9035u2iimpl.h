#ifndef __AF9035U2IIMPL_H__
#define __AF9035U2IIMPL_H__

#ifndef UNDER_CE
#endif
#include "modulatorType.h"
#include "modulatorError.h"
#include "modulatorUser.h"


Dword Af9035u2i_getDriver (
    IN  Modulator*    modulator,
    OUT Handle*         handle
);


Dword Af9035u2i_exitDriver (
    IN  Modulator*    modulator
);


Dword Af9035u2i_writeControlBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    IN  Byte*           buffer
);


Dword Af9035u2i_readControlBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
);


Dword Af9035u2i_readDataBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
);

Dword Af9035u2i_read9035u2iRegisters (
    IN  Modulator*    modulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Dword           readBufferLength,
    OUT Byte*           readBuffer
);

Dword Af9035u2i_write9035u2iRegisters (
    IN  Modulator*    modulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Dword           writeBufferLength,
    IN  Byte*           writeBuffer
) ;

Dword Af9035u2i_setSlaveAddress (
    IN  Modulator*    modulator,
	IN  Byte            SlaveAddress
);
#endif
