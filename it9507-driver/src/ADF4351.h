#include "IT9507.h"

u32 ADF4351_busInit (Modulator*    modulator);
u32 ADF4351_writeData (
		Modulator*  modulator,
		u8		count,
		u8*		buf		
);

u32 ADF4351_test (
		Modulator*  modulator		
);

u32 ADF4351_setFrequency (
		Modulator*  modulator,
		u32 freq_KHz	//KHz
);

void ADF4351_setRcounter(Modulator*  modulator, u16 Rcounter);
void ADF4351_setTdiv2(Modulator*  modulator, u8 Tdiv2);
void ADF4351_setLDF(Modulator*  modulator, u8 LDF);
