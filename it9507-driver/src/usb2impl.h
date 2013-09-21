#ifndef __USB2IMPL_H__
#define __USB2IMPL_H__

#ifndef UNDER_CE
#endif
#include "modulatorType.h"
#include "modulatorError.h"
#include "modulatorUser.h"


Dword Usb2_getDriver (
    IN  Modulator*    modulator,
    OUT Handle*         handle
);


Dword Usb2_exitDriver (
    IN  Modulator*    modulator
);


Dword Usb2_writeControlBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    IN  Byte*           buffer
);


Dword Usb2_readControlBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
);


Dword Usb2_readDataBus (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
);

#endif

