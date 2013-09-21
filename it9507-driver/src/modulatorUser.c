#include "modulatorUser.h"

#include "ADF4351.h"
DeviceDescription HwDesc[] =
{
	//GPIOH1    GPIOH2    GPIOH3     GPIOH4   GPIOH5          GPIOH6        GPIOH7  GPIOH8  
	{RestSlave, RfEnable, MuxSelect, UartTxd, PowerDownSlave, NA,			IrDa,   intrEnable	},	//0:EVB-01 v01
	{RestSlave, RfEnable, NA,        NA,	  PowerDownSlave, NA,			IrDa,   UvFilter	},	//1:DB-01-01 v01
	{RestSlave, RfEnable, MuxSelect, NA,      PowerDownSlave, NA,			lnaGain,UvFilter	},	//2:DB-01-02 v01
	{RestSlave, RfEnable, NA,        NA,      PowerDownSlave, NA,           IrDa,   UvFilter	},	//3:DB-01-01 v03
	{RestSlave, RfEnable, MuxSelect, NA,      PowerDownSlave, NA,           NA,     UvFilter	},	//4:DB-01-02 v03
	{NA,        RfEnable, NA,        NA,      NA,             NA,           NA,     NA			},	//5:SD AV sender
	{NA,        RfEnable, LoClk,     LoData,  LoLe,           LnaPowerDown, IrDa,   UvFilter	},	//6:EVB-01 v02
	{NA,        RfEnable, MuxSelect, NA,      NA,             NA,           NA,     UvFilter    },	//7:1080P DTV CAM Yuelee
	{NA,        RfEnable, NA,        NA,      NA,             NA,           NA,     UvFilter    },	//8:1080P DTV CAM Sunnic
	{NA,        RfEnable, NA,        NA,      NA,             NA,           NA,     NA			},	//9:HD AV sender
	{NA,        RfEnable, NA,        NA,      NA,             NA,           NA,     UvFilter	},	//10:AV sender HC
	{RestSlave, RfEnable, NA,        NA,      PowerDownSlave, NA,           IrDa,   UvFilter	},	//11:DB-01-01 v04
};


Dword EagleUser_getSystemConfig (
    IN  Modulator*    modulator,
    IN  Byte          pcbIndex,
	IN  SystemConfig* Config  
) {
	DeviceDescription HwSetting;
	SystemConfig PCBconfig;
	Dword error = 0;
	
	PCBconfig.restSlave		= UNUSED;
	PCBconfig.rfEnable		= UNUSED;
	PCBconfig.loClk			= UNUSED;
	PCBconfig.loData		= UNUSED;
	PCBconfig.loLe			= UNUSED;
	PCBconfig.lnaPowerDown	= UNUSED;
	PCBconfig.irDa			= UNUSED;
	PCBconfig.uvFilter		= UNUSED;
	PCBconfig.chSelect0		= UNUSED;
	PCBconfig.chSelect1		= UNUSED;		
	PCBconfig.chSelect2		= UNUSED;
	PCBconfig.chSelect3		= UNUSED;
	PCBconfig.muxSelect		= UNUSED;
	PCBconfig.uartTxd		= UNUSED;
	PCBconfig.powerDownSlave= UNUSED;
	PCBconfig.intrEnable	= UNUSED;
	PCBconfig.lnaGain		= UNUSED;

	if(pcbIndex>=IndexEnd){
		error = ModulatorError_INVALID_SYSTEM_CONFIG;
		pcbIndex = 3;
	}

	if(pcbIndex<IndexEnd){
		HwSetting = HwDesc[pcbIndex];
		if(HwSetting.GPIO1 != NA){
			if(HwSetting.GPIO1 == RestSlave)
				PCBconfig.restSlave = GPIOH1;
			else if(HwSetting.GPIO1 == RfEnable)
				PCBconfig.rfEnable = GPIOH1;
			else if(HwSetting.GPIO1 == LoClk)
				PCBconfig.loClk = GPIOH1;
			else if(HwSetting.GPIO1 == LoData)
				PCBconfig.loData = GPIOH1;
			else if(HwSetting.GPIO1 == LoLe)
				PCBconfig.loLe = GPIOH1;
			else if(HwSetting.GPIO1 == LnaPowerDown)
				PCBconfig.lnaPowerDown = GPIOH1;
			else if(HwSetting.GPIO1 == IrDa)
				PCBconfig.irDa = GPIOH1;
			else if(HwSetting.GPIO1 == UvFilter)
				PCBconfig.uvFilter = GPIOH1;
			else if(HwSetting.GPIO1 == ChSelect3)
				PCBconfig.chSelect3 = GPIOH1;
			else if(HwSetting.GPIO1 == ChSelect2)
				PCBconfig.chSelect2 = GPIOH1;
			else if(HwSetting.GPIO1 == ChSelect1)
				PCBconfig.chSelect1 = GPIOH1;
			else if(HwSetting.GPIO1 == ChSelect0)
				PCBconfig.chSelect0 = GPIOH1;
			else if(HwSetting.GPIO1 == PowerDownSlave)
				PCBconfig.powerDownSlave = GPIOH1;
			else if(HwSetting.GPIO1 == UartTxd)
				PCBconfig.uartTxd = GPIOH1;
			else if(HwSetting.GPIO1 == MuxSelect)
				PCBconfig.muxSelect = GPIOH1;
			else if(HwSetting.GPIO1 == lnaGain)
				PCBconfig.lnaGain = GPIOH1;
			else if(HwSetting.GPIO1 == intrEnable)
				PCBconfig.intrEnable = GPIOH1;
		}


		if(HwSetting.GPIO2 != NA){
			if(HwSetting.GPIO2 == RestSlave)
				PCBconfig.restSlave = GPIOH2;
			else if(HwSetting.GPIO2 == RfEnable)
				PCBconfig.rfEnable = GPIOH2;
			else if(HwSetting.GPIO2 == LoClk)
				PCBconfig.loClk = GPIOH2;
			else if(HwSetting.GPIO2 == LoData)
				PCBconfig.loData = GPIOH2;
			else if(HwSetting.GPIO2 == LoLe)
				PCBconfig.loLe = GPIOH2;
			else if(HwSetting.GPIO2 == LnaPowerDown)
				PCBconfig.lnaPowerDown = GPIOH2;
			else if(HwSetting.GPIO2 == IrDa)
				PCBconfig.irDa = GPIOH2;
			else if(HwSetting.GPIO2 == UvFilter)
				PCBconfig.uvFilter = GPIOH2;
			else if(HwSetting.GPIO2 == ChSelect3)
				PCBconfig.chSelect3 = GPIOH2;
			else if(HwSetting.GPIO2 == ChSelect2)
				PCBconfig.chSelect2 = GPIOH2;
			else if(HwSetting.GPIO2 == ChSelect1)
				PCBconfig.chSelect1 = GPIOH2;
			else if(HwSetting.GPIO2 == ChSelect0)
				PCBconfig.chSelect0 = GPIOH2;
			else if(HwSetting.GPIO2 == PowerDownSlave)
				PCBconfig.powerDownSlave = GPIOH1;
			else if(HwSetting.GPIO2 == UartTxd)
				PCBconfig.uartTxd = GPIOH2;
			else if(HwSetting.GPIO2 == MuxSelect)
				PCBconfig.muxSelect = GPIOH2;
				else if(HwSetting.GPIO2 == lnaGain)
				PCBconfig.lnaGain = GPIOH2;
			else if(HwSetting.GPIO2 == intrEnable)
				PCBconfig.intrEnable = GPIOH2;
		}

		if(HwSetting.GPIO3 != NA){
			if(HwSetting.GPIO3 == RestSlave)
				PCBconfig.restSlave = GPIOH3;
			else if(HwSetting.GPIO3 == RfEnable)
				PCBconfig.rfEnable = GPIOH3;
			else if(HwSetting.GPIO3 == LoClk)
				PCBconfig.loClk = GPIOH3;
			else if(HwSetting.GPIO3 == LoData)
				PCBconfig.loData = GPIOH3;
			else if(HwSetting.GPIO3 == LoLe)
				PCBconfig.loLe = GPIOH3;
			else if(HwSetting.GPIO3 == LnaPowerDown)
				PCBconfig.lnaPowerDown = GPIOH3;
			else if(HwSetting.GPIO3 == IrDa)
				PCBconfig.irDa = GPIOH3;
			else if(HwSetting.GPIO3 == UvFilter)
				PCBconfig.uvFilter = GPIOH3;
			else if(HwSetting.GPIO3 == ChSelect3)
				PCBconfig.chSelect3 = GPIOH3;
			else if(HwSetting.GPIO3 == ChSelect2)
				PCBconfig.chSelect2 = GPIOH3;
			else if(HwSetting.GPIO3 == ChSelect1)
				PCBconfig.chSelect1 = GPIOH3;
			else if(HwSetting.GPIO3 == ChSelect0)
				PCBconfig.chSelect0 = GPIOH3;
			else if(HwSetting.GPIO3 == PowerDownSlave)
				PCBconfig.powerDownSlave = GPIOH1;
			else if(HwSetting.GPIO3 == UartTxd)
				PCBconfig.uartTxd = GPIOH3;
			else if(HwSetting.GPIO3 == MuxSelect)
				PCBconfig.muxSelect = GPIOH3;
			else if(HwSetting.GPIO3 == lnaGain)
				PCBconfig.lnaGain = GPIOH3;
			else if(HwSetting.GPIO3 == intrEnable)
				PCBconfig.intrEnable = GPIOH3;
		}

		if(HwSetting.GPIO4 != NA){
			if(HwSetting.GPIO4 == RestSlave)
				PCBconfig.restSlave = GPIOH4;
			else if(HwSetting.GPIO4 == RfEnable)
				PCBconfig.rfEnable = GPIOH4;
			else if(HwSetting.GPIO4 == LoClk)
				PCBconfig.loClk = GPIOH4;
			else if(HwSetting.GPIO4 == LoData)
				PCBconfig.loData = GPIOH4;
			else if(HwSetting.GPIO4 == LoLe)
				PCBconfig.loLe = GPIOH4;
			else if(HwSetting.GPIO4 == LnaPowerDown)
				PCBconfig.lnaPowerDown = GPIOH4;
			else if(HwSetting.GPIO4 == IrDa)
				PCBconfig.irDa = GPIOH4;
			else if(HwSetting.GPIO4 == UvFilter)
				PCBconfig.uvFilter = GPIOH4;
			else if(HwSetting.GPIO4 == ChSelect3)
				PCBconfig.chSelect3 = GPIOH4;
			else if(HwSetting.GPIO4 == ChSelect2)
				PCBconfig.chSelect2 = GPIOH4;
			else if(HwSetting.GPIO4 == ChSelect1)
				PCBconfig.chSelect1 = GPIOH4;
			else if(HwSetting.GPIO4 == ChSelect0)
				PCBconfig.chSelect0 = GPIOH4;
			else if(HwSetting.GPIO4 == PowerDownSlave)
				PCBconfig.powerDownSlave = GPIOH4;
			else if(HwSetting.GPIO4 == UartTxd)
				PCBconfig.uartTxd = GPIOH4;
			else if(HwSetting.GPIO4 == MuxSelect)
				PCBconfig.muxSelect = GPIOH4;
			else if(HwSetting.GPIO4 == lnaGain)
				PCBconfig.lnaGain = GPIOH4;
			else if(HwSetting.GPIO4 == intrEnable)
				PCBconfig.intrEnable = GPIOH4;
		}


		if(HwSetting.GPIO5 != NA){
			if(HwSetting.GPIO5 == RestSlave)
				PCBconfig.restSlave = GPIOH5;
			else if(HwSetting.GPIO5 == RfEnable)
				PCBconfig.rfEnable = GPIOH5;
			else if(HwSetting.GPIO5 == LoClk)
				PCBconfig.loClk = GPIOH5;
			else if(HwSetting.GPIO5 == LoData)
				PCBconfig.loData = GPIOH5;
			else if(HwSetting.GPIO5 == LoLe)
				PCBconfig.loLe = GPIOH5;
			else if(HwSetting.GPIO5 == LnaPowerDown)
				PCBconfig.lnaPowerDown = GPIOH5;
			else if(HwSetting.GPIO5 == IrDa)
				PCBconfig.irDa = GPIOH5;
			else if(HwSetting.GPIO5 == UvFilter)
				PCBconfig.uvFilter = GPIOH5;
			else if(HwSetting.GPIO5 == ChSelect3)
				PCBconfig.chSelect3 = GPIOH5;
			else if(HwSetting.GPIO5 == ChSelect2)
				PCBconfig.chSelect2 = GPIOH5;
			else if(HwSetting.GPIO5 == ChSelect1)
				PCBconfig.chSelect1 = GPIOH5;
			else if(HwSetting.GPIO5 == ChSelect0)
				PCBconfig.chSelect0 = GPIOH5;
			else if(HwSetting.GPIO5 == PowerDownSlave)
				PCBconfig.powerDownSlave = GPIOH5;
			else if(HwSetting.GPIO5 == UartTxd)
				PCBconfig.uartTxd = GPIOH5;
			else if(HwSetting.GPIO5 == MuxSelect)
				PCBconfig.muxSelect = GPIOH5;
			else if(HwSetting.GPIO5 == lnaGain)
				PCBconfig.lnaGain = GPIOH5;
			else if(HwSetting.GPIO5 == intrEnable)
				PCBconfig.intrEnable = GPIOH5;
		}

		if(HwSetting.GPIO6 != NA){
			if(HwSetting.GPIO6 == RestSlave)
				PCBconfig.restSlave = GPIOH6;
			else if(HwSetting.GPIO6 == RfEnable)
				PCBconfig.rfEnable = GPIOH6;
			else if(HwSetting.GPIO6 == LoClk)
				PCBconfig.loClk = GPIOH6;
			else if(HwSetting.GPIO6 == LoData)
				PCBconfig.loData = GPIOH6;
			else if(HwSetting.GPIO6 == LoLe)
				PCBconfig.loLe = GPIOH6;
			else if(HwSetting.GPIO6 == LnaPowerDown)
				PCBconfig.lnaPowerDown = GPIOH6;
			else if(HwSetting.GPIO6 == IrDa)
				PCBconfig.irDa = GPIOH6;
			else if(HwSetting.GPIO6 == UvFilter)
				PCBconfig.uvFilter = GPIOH6;
			else if(HwSetting.GPIO6 == ChSelect3)
				PCBconfig.chSelect3 = GPIOH6;
			else if(HwSetting.GPIO6 == ChSelect2)
				PCBconfig.chSelect2 = GPIOH6;
			else if(HwSetting.GPIO6 == ChSelect1)
				PCBconfig.chSelect1 = GPIOH6;
			else if(HwSetting.GPIO6 == ChSelect0)
				PCBconfig.chSelect0 = GPIOH6;
			else if(HwSetting.GPIO6 == PowerDownSlave)
				PCBconfig.powerDownSlave = GPIOH6;
			else if(HwSetting.GPIO6 == UartTxd)
				PCBconfig.uartTxd = GPIOH6;
			else if(HwSetting.GPIO6 == MuxSelect)
				PCBconfig.muxSelect = GPIOH6;
			else if(HwSetting.GPIO6 == lnaGain)
				PCBconfig.lnaGain = GPIOH6;
			else if(HwSetting.GPIO1 == intrEnable)
				PCBconfig.intrEnable = GPIOH6;
		}

		if(HwSetting.GPIO7 != NA){
			if(HwSetting.GPIO7 == RestSlave)
				PCBconfig.restSlave = GPIOH7;
			else if(HwSetting.GPIO7 == RfEnable)
				PCBconfig.rfEnable = GPIOH7;
			else if(HwSetting.GPIO7 == LoClk)
				PCBconfig.loClk = GPIOH7;
			else if(HwSetting.GPIO7 == LoData)
				PCBconfig.loData = GPIOH7;
			else if(HwSetting.GPIO7 == LoLe)
				PCBconfig.loLe = GPIOH7;
			else if(HwSetting.GPIO7 == LnaPowerDown)
				PCBconfig.lnaPowerDown = GPIOH7;
			else if(HwSetting.GPIO7 == IrDa)
				PCBconfig.irDa = GPIOH7;
			else if(HwSetting.GPIO7 == UvFilter)
				PCBconfig.uvFilter = GPIOH7;
			else if(HwSetting.GPIO7 == ChSelect3)
				PCBconfig.chSelect3 = GPIOH7;
			else if(HwSetting.GPIO7 == ChSelect2)
				PCBconfig.chSelect2 = GPIOH7;
			else if(HwSetting.GPIO7 == ChSelect1)
				PCBconfig.chSelect1 = GPIOH7;
			else if(HwSetting.GPIO7 == ChSelect0)
				PCBconfig.chSelect0 = GPIOH7;
			else if(HwSetting.GPIO7 == PowerDownSlave)
				PCBconfig.powerDownSlave = GPIOH7;
			else if(HwSetting.GPIO7 == UartTxd)
				PCBconfig.uartTxd = GPIOH7;
			else if(HwSetting.GPIO7 == MuxSelect)
				PCBconfig.muxSelect = GPIOH7;
			else if(HwSetting.GPIO7 == lnaGain)
				PCBconfig.lnaGain = GPIOH7;
			else if(HwSetting.GPIO7 == intrEnable)
				PCBconfig.intrEnable = GPIOH7;
		}

		if(HwSetting.GPIO8 != NA){
			if(HwSetting.GPIO8 == RestSlave)
				PCBconfig.restSlave = GPIOH8;
			else if(HwSetting.GPIO8 == RfEnable)
				PCBconfig.rfEnable = GPIOH8;
			else if(HwSetting.GPIO8 == LoClk)
				PCBconfig.loClk = GPIOH8;
			else if(HwSetting.GPIO8 == LoData)
				PCBconfig.loData = GPIOH8;
			else if(HwSetting.GPIO8 == LoLe)
				PCBconfig.loLe = GPIOH8;
			else if(HwSetting.GPIO8 == LnaPowerDown)
				PCBconfig.lnaPowerDown = GPIOH8;
			else if(HwSetting.GPIO8 == IrDa)
				PCBconfig.irDa = GPIOH8;
			else if(HwSetting.GPIO8 == UvFilter)
				PCBconfig.uvFilter = GPIOH8;
			else if(HwSetting.GPIO8 == ChSelect3)
				PCBconfig.chSelect3 = GPIOH8;
			else if(HwSetting.GPIO8 == ChSelect2)
				PCBconfig.chSelect2 = GPIOH8;
			else if(HwSetting.GPIO8 == ChSelect1)
				PCBconfig.chSelect1 = GPIOH8;
			else if(HwSetting.GPIO8 == ChSelect0)
				PCBconfig.chSelect0 = GPIOH8;
			else if(HwSetting.GPIO8 == PowerDownSlave)
				PCBconfig.powerDownSlave = GPIOH8;
			else if(HwSetting.GPIO8 == UartTxd)
				PCBconfig.uartTxd = GPIOH8;
			else if(HwSetting.GPIO8 == MuxSelect)
				PCBconfig.muxSelect = GPIOH8;
			else if(HwSetting.GPIO8 == lnaGain)
				PCBconfig.lnaGain = GPIOH8;
			else if(HwSetting.GPIO8 == intrEnable)
				PCBconfig.intrEnable = GPIOH8;
		}
	}

	*Config = PCBconfig;

	return (error);
}






Dword EagleUser_setSystemConfig (
    IN  Modulator*    modulator,
	IN  SystemConfig  systemConfig
) {
	Dword error = 0;
	Byte pinCnt = 0;
	
	if(systemConfig.restSlave != UNUSED){ //output
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.restSlave+1, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.restSlave+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.restSlave+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
		EagleUser_delay(modulator, 10);
	}

   	if(systemConfig.lnaPowerDown != UNUSED){ //output
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.lnaPowerDown+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.lnaPowerDown+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.loClk != UNUSED){ //output
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.loClk+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.loClk+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.loData != UNUSED){ //output
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.loData+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.loData+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.loLe != UNUSED){ //output
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.loLe+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.loLe+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.muxSelect != UNUSED){ //output
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.muxSelect+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.muxSelect+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.powerDownSlave != UNUSED){ //output
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.powerDownSlave+1, 0);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.powerDownSlave+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.powerDownSlave+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.rfEnable != UNUSED){ //output
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.rfEnable+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.rfEnable+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.uartTxd != UNUSED){ //output
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.uartTxd+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.uartTxd+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.uvFilter != UNUSED){ //output
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.uvFilter+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.uvFilter+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(systemConfig.lnaGain != UNUSED){ //output
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.lnaGain+2, 1);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)systemConfig.lnaGain+3, 1);//gpiox_on
		if (error) goto exit;
		pinCnt++;
	}

	if(pinCnt>8)
		error = ModulatorError_INVALID_SYSTEM_CONFIG;

	if(error == ModulatorError_NO_ERROR)
		modulator->systemConfig = systemConfig;
exit:
    return (ModulatorError_NO_ERROR);
}

Dword EagleUser_getTsInputType (
	IN  Modulator*    modulator,
	OUT  TsInterface*  tsInStreamType
) {
	Dword error = ModulatorError_NO_ERROR;
	Byte temp = 0;
	*tsInStreamType = (TsInterface)temp;

	error = IT9507_readRegister (modulator, Processor_LINK, 0x4979, &temp);//has eeprom ??
	if((temp == 1) && (error == ModulatorError_NO_ERROR)){
		error = IT9507_readRegister (modulator, Processor_LINK, 0x49CA, &temp);
		if(error == ModulatorError_NO_ERROR){
			*tsInStreamType = (TsInterface)temp;
		}
	}
	return (error);
}

Dword EagleUser_getDeviceType (
	IN  Modulator*    modulator,
	OUT  Byte*		  deviceType	   
){	
	Dword error = ModulatorError_NO_ERROR;
	Byte temp;

	
	error = IT9507_readRegister (modulator, Processor_LINK, 0x4979, &temp);//has eeprom ??
	if((temp == 1) && (error == ModulatorError_NO_ERROR)){
		error = IT9507_readRegister (modulator, Processor_LINK, 0x49D5, &temp);	
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




Dword EagleUser_memoryCopy (
    IN  Modulator*    modulator,
    IN  void*           dest,
    IN  void*           src,
    IN  Dword           count
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  memcpy(dest, src, (size_t)count);
     *  return (0);
     */
    return (ModulatorError_NO_ERROR);
}

Dword EagleUser_delay (
    IN  Modulator*    modulator,
    IN  Dword           dwMs
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  delay(dwMs);
     *  return (0);
     */
	msleep(dwMs); 
    return (ModulatorError_NO_ERROR);
}


Dword EagleUser_enterCriticalSection (
    IN  Modulator*    modulator
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  return (0);
     */
    return (ModulatorError_NO_ERROR);
}


Dword EagleUser_leaveCriticalSection (
    IN  Modulator*    modulator
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  return (0);
     */
    return (ModulatorError_NO_ERROR);
}


Dword EagleUser_mpegConfig (
    IN  Modulator*    modulator
) {
    /*
     *  ToDo:  Add code here
     *
     */
    return (ModulatorError_NO_ERROR);
}


Dword EagleUser_busTx (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    IN  Byte*           buffer
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  short i;
     *
     *  start();
     *  write_i2c(uc2WireAddr);
     *  ack();
     *  for (i = 0; i < bufferLength; i++) {
     *      write_i2c(*(ucpBuffer + i));
     *      ack();
     *  }
     *  stop();
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
	Dword error = 0;
	if(modulator->busId == Bus_USB)
		error = Usb2_writeControlBus(modulator,bufferLength,buffer);
	else
		error = ModulatorError_INVALID_BUS_TYPE;
    return (error);
}


Dword EagleUser_busRx (
    IN  Modulator*    modulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  short i;
     *
     *  start();
     *  write_i2c(uc2WireAddr | 0x01);
     *  ack();
     *  for (i = 0; i < bufferLength - 1; i++) {
     *      read_i2c(*(ucpBuffer + i));
     *      ack();
     *  }
     *  read_i2c(*(ucpBuffer + bufferLength - 1));
     *  nack();
     *  stop();
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
	Dword error = 0;
	if(modulator->busId == Bus_USB)
		error = Usb2_readControlBus(modulator,bufferLength,buffer);
	else
		error = ModulatorError_INVALID_BUS_TYPE;
	return (error);
}


Dword EagleUser_setBus (
    IN  Modulator*    modulator
) {
	/*
     *  ToDo:  Add code here
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
    Dword error = ModulatorError_NO_ERROR;

    return(error);
}


 Dword EagleUser_Initialization  (
    IN  Modulator*    modulator
) {
	/*
     *  ToDo:  Add code here
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
	Dword error = 0;
	 error = EagleUser_setSystemConfig(modulator, modulator->systemConfig);
	 if (error) goto exit;

	if(modulator->tsInterfaceType != StreamType_DVBT_DATAGRAM){
		
		if(modulator->systemConfig.restSlave != UNUSED){
			error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)modulator->systemConfig.restSlave+1, 1); //RX(IT9133) rest 
			if (error) goto exit;
		}

		EagleUser_delay(modulator, 10);
		if(modulator->systemConfig.powerDownSlave != UNUSED){
			error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)modulator->systemConfig.powerDownSlave+1, 0); //RX(IT9133) power up
			if (error) goto exit;
		}


	}

	if(modulator->systemConfig.rfEnable != UNUSED){
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)modulator->systemConfig.rfEnable+1, 0); //RF out power down
		if (error) goto exit;
	}
	if(modulator->systemConfig.lnaGain != UNUSED){
		error = IT9507_writeRegister (modulator, Processor_LINK, (Dword)modulator->systemConfig.lnaGain+1, 0); //lna Gain
		if (error) goto exit;
	}
	if(modulator->systemConfig.loClk != UNUSED)
		ADF4351_busInit(modulator);
exit:
    return (error);

 }


Dword EagleUser_Finalize  (
    IN  Modulator*    modulator
) {
	/*
     *  ToDo:  Add code here
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
	Dword error = 0;
	if(modulator->busId == Bus_USB)
		error = Usb2_exitDriver(modulator);
	else
		error = ModulatorError_INVALID_BUS_TYPE;
    return (error);

 }
 

Dword EagleUser_acquireChannel (
	IN  Modulator*    modulator,
	IN  Word          bandwidth,
	IN  Dword         frequency
){

	/*
     *  ToDo:  Add code here
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
	Dword error = 0;

	if(modulator->systemConfig.loClk != UNUSED)
		ADF4351_setFrequency(modulator, frequency);//External Lo control

	if(frequency <= 300000){ // <=300000KHz v-filter gpio set to Lo
		 if(modulator->systemConfig.uvFilter != UNUSED){
			 error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.uvFilter+1, 0); 
			 if (error) goto exit;
		 }

	}else{
		 if(modulator->systemConfig.uvFilter != UNUSED){
			 error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.uvFilter+1, 1); 
			 if (error) goto exit;
		 }
	}	
exit:
	return (error);
}

Dword EagleUser_setTxModeEnable (
	IN  Modulator*            modulator,
	IN  Byte                    enable	
) {
	/*
     *  ToDo:  Add code here
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
	Dword error = ModulatorError_NO_ERROR;
	if(enable){
		if(modulator->systemConfig.rfEnable != UNUSED){
			error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.rfEnable+1, 1); //RF power up 
			if (error) goto exit;
		}	
	}else{
		if(modulator->systemConfig.rfEnable != UNUSED){
			error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.rfEnable+1, 0); //RF power down 
			if (error) goto exit;
		}
		
	}
exit :
	return (error);
}


Dword EagleUser_getChannelIndex (
	IN  Modulator*            modulator,
	IN  Byte*                    index	
) {

	/*
     *  ToDo:  Add code here
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
	Dword error = ModulatorError_NO_ERROR;
	Byte Freqindex = 0;
	Byte temp = 0;
	// get HW setting
	if(modulator->systemConfig.muxSelect != UNUSED){
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.muxSelect+1, 1); //MUX
		if (error) goto exit;
	}

	if(modulator->systemConfig.chSelect0 != UNUSED){ //input
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect0+2, 0);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect0+3, 1);//gpiox_on
		if (error) goto exit;
	}

	if(modulator->systemConfig.chSelect1 != UNUSED){ //input
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect1+2, 0);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect1+3, 1);//gpiox_on
		if (error) goto exit;					
	}

	if(modulator->systemConfig.chSelect2 != UNUSED){ //input
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect2+2, 0);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect2+3, 1);//gpiox_on
		if (error) goto exit;					
	}

	if(modulator->systemConfig.chSelect3 != UNUSED){ //input
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect3+2, 0);//gpiox_en
		if (error) goto exit;
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect3+3, 1);//gpiox_on
		if (error) goto exit;					
	}
	//--- get HW freq setting
	if(modulator->systemConfig.chSelect0 != UNUSED){
		error = IT9507_readRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect0, &temp); 					
	}
	if (error) goto exit;
	Freqindex = Freqindex | (temp);

	if(modulator->systemConfig.chSelect1 != UNUSED){
		error = IT9507_readRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect1, &temp); 
	}
	if (error) goto exit;
	Freqindex = Freqindex | (temp<<1);

	if(modulator->systemConfig.chSelect2 != UNUSED){
		error = IT9507_readRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect2, &temp); 					
	}
	if (error) goto exit;
	Freqindex = Freqindex | (temp<<2);

	if(modulator->systemConfig.chSelect3 != UNUSED){
		error = IT9507_readRegister (modulator, Processor_LINK, modulator->systemConfig.chSelect3, &temp); 					
	}
	if (error) goto exit;
	Freqindex = Freqindex | (temp<<3);

	error = IT9507_readRegister (modulator, Processor_LINK, 0x49E5, &temp);
	if (error) goto exit;
	Freqindex = Freqindex | (temp<<4);
	//--------------------


	error = EagleUser_setSystemConfig (modulator, modulator->systemConfig);
	if (error) goto exit;


	if(modulator->systemConfig.muxSelect != UNUSED){
		error = IT9507_writeRegister (modulator, Processor_LINK, modulator->systemConfig.muxSelect+1, 0); //MUX					
	}

	*index = Freqindex;
exit :
	return (error);
}

