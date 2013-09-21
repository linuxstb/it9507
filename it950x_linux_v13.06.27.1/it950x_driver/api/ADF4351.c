#include "ADF4351.h"

//---------- default setting 666000KHz
#define default_Reg0	0x00330030
#define default_Reg1	0x08008069
#define default_Reg2	0x00005842   //#define default_Reg2	0x00005E42
#define default_Reg3	0x004004B3
#define default_Reg4	0x00AD003C
#define default_Reg5	0x00580005

#define PFD	26	//PFD = 26MHz

Dword ADF4351_Reg0 = default_Reg0;	
Dword ADF4351_Reg1 = default_Reg1;	
Dword ADF4351_Reg2 = default_Reg2;	
Dword ADF4351_Reg3 = default_Reg3;	
Dword ADF4351_Reg4 = default_Reg4;	
Dword ADF4351_Reg5 = default_Reg5;	

void SET_LE(Modulator*    modulator){
	if(modulator->systemConfig.loLe != UNUSED){
		IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.loLe+1, 1); 		
	}
}

void CLR_LE(Modulator*    modulator){
	if(modulator->systemConfig.loLe != UNUSED){
		IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.loLe+1, 0); 		
	}
}
void SET_SCL(Modulator*    modulator){
	if(modulator->systemConfig.loClk != UNUSED){
		IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.loClk+1, 1); 		
	}
}
void CLR_SCL(Modulator*    modulator){
	if(modulator->systemConfig.loClk != UNUSED){
		IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.loClk+1, 0); 		
	}
}

void SET_DATA(Modulator*    modulator){
	if(modulator->systemConfig.loData != UNUSED){
		IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.loData+1, 1); 		
	}
}
void CLR_DATA(Modulator*    modulator){
	if(modulator->systemConfig.loData != UNUSED){
		IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.loData+1, 0); 		
	}
}



Dword ADF4351_busInit (Modulator*    modulator) {
    Dword error = ModulatorError_NO_ERROR;
	if(modulator->systemConfig.loClk != UNUSED){ //output
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.loClk+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.loClk+3, 1);//gpiox_on
		if (error) goto exit;		
	}

	if(modulator->systemConfig.loData != UNUSED){ //output
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.loData+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.loData+3, 1);//gpiox_on
		if (error) goto exit;		
	}

	if(modulator->systemConfig.loLe != UNUSED){ //output
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.loLe+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.loLe+3, 1);//gpiox_on
		if (error) goto exit;	
	}
exit:
	return error;
}


Dword ADF4351_writeData (
		Modulator*  modulator,
		Byte		count,
		Byte*		buf		
) {
    Dword error = ModulatorError_NO_ERROR;
	unsigned	char	ValueToWrite = 0;
    unsigned	char	i = 0;
	unsigned	char	j = 0;

	EagleUser_delay(modulator,0);
	CLR_SCL(modulator);
	CLR_LE(modulator);
	EagleUser_delay(modulator,0);


	for(i=count;i>0;i--)
 	{
	 	ValueToWrite = *(buf + i - 1);
		for(j=0; j<8; j++)
		{
			if(0x80 == (ValueToWrite & 0x80))
			{
				SET_DATA(modulator);	  //Send one to SDO pin
			}
			else
			{
				CLR_DATA(modulator);	  //Send zero to SDO pin
			}
			EagleUser_delay(modulator,0);
			SET_SCL(modulator);
			EagleUser_delay(modulator,0);
			ValueToWrite <<= 1;	//Rotate data
			CLR_SCL(modulator);
		}
	}
	CLR_DATA(modulator);
	EagleUser_delay(modulator,0);
	SET_LE(modulator);
	EagleUser_delay(modulator,0);
	CLR_LE(modulator);
	return error;
}


Dword ADF4351_test (
		Modulator*  modulator		
) {

    Dword error = ModulatorError_NO_ERROR;
	Dword reg;
	Byte buf[4];

	reg = default_Reg5;
	buf[3] = (Byte)(reg>>24);
	buf[2] = (Byte)(reg>>16);
	buf[1] = (Byte)(reg>>8);				
 	buf[0] = (Byte)(reg);				
	ADF4351_writeData(modulator, 4,buf);		

	reg = default_Reg4;
	buf[3] = (Byte)(reg>>24);
	buf[2] = (Byte)(reg>>16);
	buf[1] = (Byte)(reg>>8);				
 	buf[0] = (Byte)(reg);				
	ADF4351_writeData(modulator, 4,buf);

	reg = default_Reg3;
	buf[3] = (Byte)(reg>>24);
	buf[2] = (Byte)(reg>>16);
	buf[1] = (Byte)(reg>>8);				
 	buf[0] = (Byte)(reg);				
	ADF4351_writeData(modulator, 4,buf);		

	reg = default_Reg2;
	buf[3] = (Byte)(reg>>24);
	buf[2] = (Byte)(reg>>16);
	buf[1] = (Byte)(reg>>8);				
 	buf[0] = (Byte)(reg);				
	ADF4351_writeData(modulator, 4,buf);	

	reg = default_Reg1;
	buf[3] = (Byte)(reg>>24);
	buf[2] = (Byte)(reg>>16);
	buf[1] = (Byte)(reg>>8);				
 	buf[0] = (Byte)(reg);				
	ADF4351_writeData(modulator, 4,buf);		

	reg = default_Reg0;
	buf[3] = (Byte)(reg>>24);
	buf[2] = (Byte)(reg>>16);
	buf[1] = (Byte)(reg>>8);				
 	buf[0] = (Byte)(reg);				
	ADF4351_writeData(modulator, 4,buf);		
	return error;
}


Dword ADF4351_setFrequency (
		Modulator*  modulator,
		Dword freq_KHz	//KHz
) {
    Dword error = ModulatorError_NO_ERROR;
	Dword reg;
	Byte buf[4];
	Dword VCO_KHz;
	Dword DividerSelect;
	Dword DividerValue;
	Dword INTvalue;
	Dword MOD;
	Dword FRAC;

	Word Rcounter = 130;
	Byte Tdiv2 = 0;
	Byte LDF = 1;
	Dword Fpfd;

	//---set Divider Select
	if((4400000>=freq_KHz) && (freq_KHz>2200000)){
		DividerSelect = 0;	//  RF out = VCO/1
		DividerValue = 1;
	}else if((2200000>=freq_KHz) && (freq_KHz>1100000)){
		DividerSelect = 1;  //  RF out = VCO/2
		DividerValue = 2;
	}if((1100000>=freq_KHz) && (freq_KHz>550000)){
		DividerSelect = 2;	//  RF out = VCO/4
		DividerValue = 4;
	}else if((550000>=freq_KHz) && (freq_KHz>275000)){
		DividerSelect = 3;	//  RF out = VCO/8
		DividerValue = 8;
	}else if((275000>=freq_KHz) && (freq_KHz>137500)){
		DividerSelect = 4;	//  RF out = VCO/16
		DividerValue = 16;
	}else if((137500>=freq_KHz) && (freq_KHz>68750)){
		DividerSelect = 5;	//  RF out = VCO/32
		DividerValue = 32;
	}else if((68750>=freq_KHz) && (freq_KHz>=34375)){
		DividerSelect = 6;	//  RF out = VCO/64
		DividerValue = 64;
	}else{
		error = ModulatorError_FREQ_OUT_OF_RANGE;
		goto exit;
	}

	if (1){
		VCO_KHz = freq_KHz*DividerValue;

		INTvalue = VCO_KHz/(PFD*1000);

		MOD = PFD;

		FRAC = (VCO_KHz%(PFD*1000))/1000;

		if(FRAC == 0)
			MOD = 1;
	}else{

		ADF4351_setRcounter(modulator, Rcounter);
		ADF4351_setTdiv2(modulator, Tdiv2);
		ADF4351_setLDF(modulator, LDF);

		Fpfd = (PFD * 1000) / (Rcounter * (1 + Tdiv2));
		
		VCO_KHz = freq_KHz*DividerValue;
		
		INTvalue = VCO_KHz/Fpfd;
		MOD = 1;
		FRAC = 0;


	}

	reg = ADF4351_Reg5;
	buf[3] = (Byte)(reg>>24);
	buf[2] = (Byte)(reg>>16);
	buf[1] = (Byte)(reg>>8);				
 	buf[0] = (Byte)(reg);	
	
	ADF4351_writeData(modulator, 4,buf);		

	reg = (ADF4351_Reg4 & 0x008FFFFF) | (DividerSelect<<20) | 4;
	buf[3] = (Byte)(reg>>24);
	buf[2] = (Byte)(reg>>16);
	buf[1] = (Byte)(reg>>8);				
 	buf[0] = (Byte)(reg);
	ADF4351_writeData(modulator, 4,buf);

	reg = ADF4351_Reg3 ;
	buf[3] = (Byte)(reg>>24);
	buf[2] = (Byte)(reg>>16);
	buf[1] = (Byte)(reg>>8);				
 	buf[0] = (Byte)(reg);
	ADF4351_writeData(modulator, 4,buf);		

	reg = ADF4351_Reg2;
	buf[3] = (Byte)(reg>>24);
	buf[2] = (Byte)(reg>>16);
	buf[1] = (Byte)(reg>>8);				
 	buf[0] = (Byte)(reg);
	ADF4351_writeData(modulator, 4,buf);	

	reg = (ADF4351_Reg1 & 0x1FFFC000) | (MOD<<3) | 1;
	buf[3] = (Byte)(reg>>24);
	buf[2] = (Byte)(reg>>16);
	buf[1] = (Byte)(reg>>8);				
 	buf[0] = (Byte)(reg);	
	ADF4351_writeData(modulator, 4,buf);		

	reg = (ADF4351_Reg0 & 0) | (INTvalue<<15) | (FRAC<<3) | 0;
	buf[3] = (Byte)(reg>>24);
	buf[2] = (Byte)(reg>>16);
	buf[1] = (Byte)(reg>>8);				
 	buf[0] = (Byte)(reg);
	ADF4351_writeData(modulator, 4,buf);		

exit:
	return error;
}

void ADF4351_setRcounter(
	Modulator*  modulator,
	Word Rcounter
){
	Dword reg;
	Byte buf[4];
	ADF4351_Reg2 = (ADF4351_Reg2 & 0xFF003FFF) + (Rcounter<<14);
	reg = ADF4351_Reg2;
	buf[3] = (Byte)(reg>>24);
	buf[2] = (Byte)(reg>>16);
	buf[1] = (Byte)(reg>>8);				
 	buf[0] = (Byte)(reg);
	ADF4351_writeData(modulator, 4,buf);	

	reg = ADF4351_Reg0;
	buf[3] = (Byte)(reg>>24);
	buf[2] = (Byte)(reg>>16);
	buf[1] = (Byte)(reg>>8);				
 	buf[0] = (Byte)(reg);
	ADF4351_writeData(modulator, 4,buf);	

}

void ADF4351_setTdiv2(
	  Modulator*  modulator,
	  Byte Tdiv2
) {
	Dword reg;
	Byte buf[4];
	ADF4351_Reg2 = (ADF4351_Reg2 & 0xFEFFFFFF) + (Tdiv2<<24);
	reg = ADF4351_Reg2;
	buf[3] = (Byte)(reg>>24);
	buf[2] = (Byte)(reg>>16);
	buf[1] = (Byte)(reg>>8);				
 	buf[0] = (Byte)(reg);
	ADF4351_writeData(modulator, 4,buf);	

	reg = ADF4351_Reg0;
	buf[3] = (Byte)(reg>>24);
	buf[2] = (Byte)(reg>>16);
	buf[1] = (Byte)(reg>>8);				
 	buf[0] = (Byte)(reg);
	ADF4351_writeData(modulator, 4,buf);	
}

void ADF4351_setLDF(
	Modulator*  modulator,
	Byte LDF
){
	Dword reg;
	Byte buf[4];
	ADF4351_Reg2 = (ADF4351_Reg2 & 0xFFFFFEFF) + (LDF<<8);
	reg = ADF4351_Reg2;
	buf[3] = (Byte)(reg>>24);
	buf[2] = (Byte)(reg>>16);
	buf[1] = (Byte)(reg>>8);				
 	buf[0] = (Byte)(reg);
	ADF4351_writeData(modulator, 4,buf);	

	reg = ADF4351_Reg0;
	buf[3] = (Byte)(reg>>24);
	buf[2] = (Byte)(reg>>16);
	buf[1] = (Byte)(reg>>8);				
 	buf[0] = (Byte)(reg);
	ADF4351_writeData(modulator, 4,buf);	
}