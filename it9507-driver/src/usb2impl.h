#ifndef __USB2IMPL_H__
#define __USB2IMPL_H__

#ifndef UNDER_CE
#endif
#include "modulatorType.h"
#include "modulatorError.h"
#include "modulatorUser.h"


u32 Usb2_getDriver (
    IN  Modulator*    modulator,
    OUT Handle*         handle
);


u32 Usb2_exitDriver (
    IN  Modulator*    modulator
);


u32 Usb2_writeControlBus (
    IN  Modulator*    modulator,
    IN  u32           bufferLength,
    IN  u8*           buffer
);


u32 Usb2_readControlBus (
    IN  Modulator*    modulator,
    IN  u32           bufferLength,
    OUT u8*           buffer
);


u32 Usb2_readDataBus (
    IN  Modulator*    modulator,
    IN  u32           bufferLength,
    OUT u8*           buffer
);

#endif

