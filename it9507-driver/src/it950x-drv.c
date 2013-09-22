#include "it950x-core.h"
#include "modulatorFirmware.h"
#include "IQ_fixed_table.h"


#define FW_VER         0x08060000

int dvb_usb_it950x_debug;
module_param_named(debug,dvb_usb_it950x_debug, int, 0644);


#define  p_reg_top_pwrdw_hwen  0xD809 
#define reg_top_pwrdw_hwen_pos 0
#define reg_top_pwrdw_hwen_len 1

#define    p_reg_top_pwrdw 0xD80B 
#define reg_top_pwrdw_pos 0
#define reg_top_pwrdw_len 1

#define    r_reg_top_gpioh1_i	0xD8AE 
#define	reg_top_gpioh1_i_pos 0
#define	reg_top_gpioh1_i_len 1

#define    p_reg_top_gpioh1_o	0xD8AF 
#define	reg_top_gpioh1_o_pos 0
#define	reg_top_gpioh1_o_len 1

#define    p_reg_top_gpioh1_en	0xD8B0 
#define	reg_top_gpioh1_en_pos 0
#define	reg_top_gpioh1_en_len 1

#define    p_reg_top_gpioh1_on	0xD8B1 
#define	reg_top_gpioh1_on_pos 0
#define	reg_top_gpioh1_on_len 1

#define    r_reg_top_gpioh5_i	0xD8BA 
#define	reg_top_gpioh5_i_pos 0
#define	reg_top_gpioh5_i_len 1

#define    p_reg_top_gpioh5_o	0xD8BB 
#define	reg_top_gpioh5_o_pos 0
#define	reg_top_gpioh5_o_len 1

#define    p_reg_top_gpioh5_en	0xD8BC 
#define	reg_top_gpioh5_en_pos 0
#define	reg_top_gpioh5_en_len 1

#define    p_reg_top_gpioh5_on	0xD8BD 
#define	reg_top_gpioh5_on_pos 0
#define	reg_top_gpioh5_on_len 1

#define    p_reg_top_gpioh7_o	0xD8C3 
#define	reg_top_gpioh7_o_pos 0
#define	reg_top_gpioh7_o_len 1


static DEFINE_MUTEX(mymutex);

static u32 DRV_NIMReset(
	void* handle);

static u32 DRV_InitNIMSuspendRegs(
	void* handle);
	
u32 DRV_TunerSuspend(
	void * handle,
	u8 ucChip,
	bool bOn);

static u32 DRV_NIMSuspend(
	void * handle,
	bool bSuspend);

static u32 DRV_NIMReset(
	void* handle);

static u32 DRV_InitNIMSuspendRegs(
	void* handle);

static u32 DRV_Initialize(
	void* handle);

static u32 DL_Initialize(
	void* handle);

static u32 DRV_getFirmwareVersionFromFile( 
		void* handle,
	 	Processor	processor, 
		u32* 		version
)
{

	
	PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;
	u8 chip_version = 0;
	u32 chip_Type;
	u8 var[2];
	u32 error = Error_NO_ERROR;

	u32 OFDM_VER1;
    u32 OFDM_VER2;
    u32 OFDM_VER3;
    u32 OFDM_VER4;

    u32 LINK_VER1;
    u32 LINK_VER2;
    u32 LINK_VER3;    
    u32 LINK_VER4;    
    

	error = IT9507_readRegister((Modulator*) &pdc->modulator, processor, chip_version_7_0, &chip_version);
	error = IT9507_readRegisters((Modulator*) &pdc->modulator, processor, chip_version_7_0+1, 2, var);
	
	if(error) deb_data("DRV_getFirmwareVersionFromFile fail");
	
	chip_Type = var[1]<<8 | var[0];	
	if(chip_Type == 0x9135 && chip_version == 2){
#if 0
	  /* NOT USED */
		OFDM_VER1 = DVB_V2_OFDM_VERSION1;
		OFDM_VER2 = DVB_V2_OFDM_VERSION2;
		OFDM_VER3 = DVB_V2_OFDM_VERSION3;
		OFDM_VER4 = DVB_V2_OFDM_VERSION4;

		LINK_VER1 = DVB_V2_LL_VERSION1;
		LINK_VER2 = DVB_V2_LL_VERSION2;
		LINK_VER3 = DVB_V2_LL_VERSION3;    
		LINK_VER4 = DVB_V2_LL_VERSION4;
#endif
	}else{
		OFDM_VER1 = DVB_OFDM_VERSION1;
    	OFDM_VER2 = DVB_OFDM_VERSION2;
   	 	OFDM_VER3 = DVB_OFDM_VERSION3;
    	OFDM_VER4 = DVB_OFDM_VERSION4;

   		LINK_VER1 = DVB_LL_VERSION1;
    	LINK_VER2 = DVB_LL_VERSION2;
    	LINK_VER3 = DVB_LL_VERSION3;    
    	LINK_VER4 = DVB_LL_VERSION4;
	}

    if(processor == Processor_OFDM) {
        *version = (u32)( (OFDM_VER1 << 24) + (OFDM_VER2 << 16) + (OFDM_VER3 << 8) + OFDM_VER4);
    }
    else { //LINK
        *version = (u32)( (LINK_VER1 << 24) + (LINK_VER2 << 16) + (LINK_VER3 << 8) + LINK_VER4);    
    }
    
    return *version;
}

u32 DRV_getDeviceType(void *handle)
{
	u32 dwError = Error_NO_ERROR;
   	PDEVICE_CONTEXT PDC = (PDEVICE_CONTEXT) handle;

	dwError =  EagleUser_getDeviceType((Modulator*) &PDC->modulator, &PDC->deviceType);

    return(dwError);
}

static u32  DRV_Initialize(
	    void *      handle
)
{
	u32 error = Error_NO_ERROR;
	u32 error_rx = Error_NO_ERROR;
	PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;
	u8 temp = 0;
	//u8 usb_dma_reg;
	u8 chip_version = 0; 
	u32 fileVersion, cmdVersion = 0; 

	deb_data("- Enter %s Function -\n",__FUNCTION__);

	if(EagleUser_getDeviceType((Modulator*) &pdc->modulator, &pdc->deviceType) != 0)
		printk("- EagleUser_getDeviceType fail -\n");

	if(IT9507_setSlaveIICAddress((Modulator*) &pdc->modulator, SLAVE_DEMOD_2WIREADDR) != 0)
		printk("- IT9507_setSlaveIICAddress fail -\n");	
	
	
	if(pdc->modulator.booted) //from Standard_setBusTuner() > Standard_getFirmwareVersion()
    	{
        	//use "#define version" to get fw version (from firmware.h title)
        	error = DRV_getFirmwareVersionFromFile(handle, Processor_OFDM, &fileVersion);

        	//use "Command_QUERYINFO" to get fw version 
        	error = IT9507_getFirmwareVersion((Modulator*) &pdc->modulator, Processor_OFDM, &cmdVersion);
        	if(error) deb_data("DRV_Initialize : IT9507_getFirmwareVersion : error = 0x%08u\n", error);

        	if(cmdVersion != fileVersion)
        	{
            		deb_data("Reboot: Outside Fw = 0x%x, Inside Fw = 0x%x", fileVersion, cmdVersion);
            		error = IT9507_TXreboot((Modulator*) &pdc->modulator);
            		pdc->bBootCode = true;
            		if(error) 
            		{
                		deb_data("IT9507_reboot : error = 0x%08u\n", error);
                		return error;
            		}
            		else {
                		return Error_NOT_READY;
            		}
        	}
        	else
        	{
            		deb_data("	Fw version is the same!\n");
  	      		error = Error_NO_ERROR;
        	}
	}//pdc->IT950x.booted

	//			case StreamType_DVBT_DATAGRAM:
	deb_data("    StreamType_DVBT_DATAGRAM\n");
	error = IT9507_initialize ((Modulator*) &pdc->modulator, 0);
				 
	if (error) deb_data("IT950x_initialize _Device initialize fail : 0x%08x\n", error);
	else deb_data("    Device initialize TX Ok\n");

    IT9507_getFirmwareVersion ((Modulator*) &pdc->modulator, Processor_OFDM, &cmdVersion);
    deb_data("    FwVer OFDM = 0x%x, ", cmdVersion);
    IT9507_getFirmwareVersion ((Modulator*) &pdc->modulator, Processor_LINK, &cmdVersion);
    deb_data("FwVer LINK = 0x%x\n", cmdVersion);
    
	/* Solve 0-byte packet error. write Link 0xDD8D[3] = 1 */
	//error = Demodulator_readRegister((Demodulator*) &pdc->demodulator, Processor_LINK, 0xdd8d, &usb_dma_reg);
	//usb_dma_reg |= 0x08;             /*reg_usb_min_len*/
	//error = Demodulator_writeRegister((Demodulator*) &pdc->demodulator, Processor_LINK, 0xdd8d, usb_dma_reg);
    
    return error;
	
}

static u32 DRV_InitDevInfo(
    	void *      handle,
    	u8        ucSlaveDemod
)
{
    u32 dwError = Error_NO_ERROR;    
   	PDEVICE_CONTEXT PDC = (PDEVICE_CONTEXT) handle;
    PDC->fc[ucSlaveDemod].ulCurrentFrequency = 0;  
    PDC->fc[ucSlaveDemod].ucCurrentBandWidth = 0;

    PDC->fc[ucSlaveDemod].ulDesiredFrequency = 0;	
    PDC->fc[ucSlaveDemod].ucDesiredBandWidth = 6000;	

    //For PID Filter Setting
    //PDC->fc[ucSlaveDemod].ulcPIDs = 0;    
    PDC->fc[ucSlaveDemod].bEnPID = false;

    PDC->fc[ucSlaveDemod].bApOn = false;
    
    PDC->fc[ucSlaveDemod].bResetTs = false;



    PDC->fc[ucSlaveDemod].tunerinfo.bTunerOK = false;
    PDC->fc[ucSlaveDemod].tunerinfo.bSettingFreq = false;

    return dwError;
}	

//get EEPROM_IRMODE/bIrTblDownload/bRAWIr/architecture config from EEPROM
static u32 DRV_GetEEPROMConfig(
	void* handle)
{       
    u32 dwError = Error_NO_ERROR;
	u8 chip_version = 0;
	u32 chip_Type;
	u8  var[2];
    PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;
	//bIrTblDownload option
    u8 btmp = 0;
	int ucSlaveDemod;
	
	deb_data("- Enter %s Function -",__FUNCTION__);

	//patch for read eeprom valid bit
	dwError = IT9507_readRegister((Modulator*) &pdc->modulator, Processor_LINK, chip_version_7_0, &chip_version);
	dwError = IT9507_readRegisters((Modulator*) &pdc->modulator, Processor_LINK, chip_version_7_0+1, 2, var);

	if(dwError) deb_data("DRV_GetEEPROMConfig fail---cant read chip version");

	chip_Type = var[1]<<8 | var[0];
	if(chip_Type==0x9135 && chip_version == 2) //Om2
	{
		pdc->chip_version = 2;
		dwError = IT9507_readRegisters((Modulator*) &pdc->modulator, Processor_LINK, 0x461d, 1, &btmp);
		deb_data("Chip Version is %d---and Read 461d---valid bit = 0x%02X", chip_version, btmp);
	}
	else 
	{
		pdc->chip_version = 1; //Om1
		dwError = IT9507_readRegisters((Modulator*) &pdc->modulator, Processor_LINK, 0x4979, 1, &btmp);
		deb_data("Chip Version is %d---and Read 4979---valid bit = 0x%02X", chip_version, btmp);
	}
	if (dwError) 
	{
		deb_data("0x461D eeprom valid bit read fail!");
		goto exit;
    }

	if(btmp == 0)
	{
	  printk("it905x: btmp=%d\n",btmp);
		deb_data("=============No need read eeprom");
		pdc->bSupportSelSuspend = false;
		pdc->bDualTs = false;
    	pdc->architecture = Architecture_DCA;
    	//pdc->modulator.chipNumber = 1;    
    	pdc->bDCAPIP = false;
		pdc->fc[0].tunerinfo.TunerId = 0x38;
	}
	else
	{
		deb_data("=============Need read eeprom");
		dwError = IT9507_readRegisters((Modulator*) &pdc->modulator, Processor_LINK, EEPROM_IRMODE, 1, &btmp);
    	if (dwError) goto exit;
    	deb_data("EEPROM_IRMODE = 0x%02X, ", btmp);	

//selective suspend
    	pdc->bSupportSelSuspend = false;
	    //btmp = 0; //not support in v8.12.12.3
   		//dwError = IT9507_readRegisters((Demodulator*) &pdc->Demodulator, Processor_LINK, EEPROM_SELSUSPEND, 1, &btmp);
   	 	//if (dwError) goto exit;
    	//if(btmp<2)
    	//	PDC->bSupportSelSuspend = btmp ? true:false; 
    	deb_data("SelectiveSuspend = %s", pdc->bSupportSelSuspend?"ON":"OFF");
    	    
//bDualTs option   
    	pdc->bDualTs = false;
    	pdc->architecture = Architecture_DCA;
    	//pdc->modulator.chipNumber = 1;    
    	pdc->bDCAPIP = false;

    	dwError = IT9507_readRegisters((Modulator*) &pdc->modulator, Processor_LINK, EEPROM_TSMODE, 1, &btmp);
    	if (dwError) goto exit;
    	deb_data("EEPROM_TSMODE = 0x%02X", btmp);

    	if (btmp == 0)     
    	{  
        	deb_data("TSMode = TS1 mode\n");
    	}
    	else if (btmp == 1) 
   		{
        	deb_data("TSMode = DCA+PIP mode\n");
			pdc->architecture = Architecture_DCA;
        	//pdc->modulator.chipNumber = 2;
        	pdc->bDualTs = true;
        	pdc->bDCAPIP = true;
    	}
    	else if (btmp == 2) 
    	{ 
        	deb_data("TSMode = DCA mode\n");
        	//pdc->modulator.chipNumber = 2;
    	}
    	else if (btmp == 3) 
    	{
        	deb_data("TSMode = PIP mode\n");
        	pdc->architecture = Architecture_PIP;
        	//pdc->modulator.chipNumber = 2;
        	pdc->bDualTs = true;
    	}

//tunerID option, in Omega, not need to read register, just assign 0x38;
		dwError = IT9507_readRegisters((Modulator*) &pdc->modulator, Processor_LINK, EEPROM_TUNERID, 1, &btmp);
		if (btmp==0x51) {
			pdc->fc[0].tunerinfo.TunerId = 0x51;  	
		}
		else if (btmp==0x52) {
			pdc->fc[0].tunerinfo.TunerId = 0x52;  	
		}
		else if (btmp==0x60) {
			pdc->fc[0].tunerinfo.TunerId = 0x60;  	
		}
		else if (btmp==0x61) {
			pdc->fc[0].tunerinfo.TunerId = 0x61;  	
		}
		else if (btmp==0x62) {
			pdc->fc[0].tunerinfo.TunerId = 0x62;  	
		}
		else {
			pdc->fc[0].tunerinfo.TunerId = 0x38;  	
		}		
	
		deb_data("pdc->fc[0].tunerinfo.TunerId = 0x%x", pdc->fc[0].tunerinfo.TunerId);
		if (pdc->bDualTs) {
			pdc->fc[1].tunerinfo.TunerId = pdc->fc[0].tunerinfo.TunerId;
			deb_data("pdc->fc[1].tunerinfo.TunerId = 0x%x", pdc->fc[1].tunerinfo.TunerId);
		}

		//dwError = IT9507_writeRegister((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, EEPROM_SUSPEND, 0);
		dwError = IT9507_readRegisters((Modulator*) &pdc->modulator, Processor_LINK, EEPROM_SUSPEND, 1, &btmp);
		deb_data("EEPROM susped mode=%d", btmp);
    	
    }
		//pdc->modulator.chipNumber = 1; 
//init some device info
	for(ucSlaveDemod = 0; ucSlaveDemod <= (u8)pdc->bDualTs; ucSlaveDemod++)
	{
		dwError = DRV_InitDevInfo(handle, ucSlaveDemod);
	}
	
exit:
    return(dwError);
}

static u32 DRV_SetBusTuner(
	 void * handle, 
	 u16 tunerId
)
{
	u32 dwError = Error_NO_ERROR;
	u32 	 version = 0;

	PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;

	deb_data("- Enter %s Function -",__FUNCTION__);
	deb_data("tunerId =0x%x\n", tunerId);

	dwError = IT9507_getFirmwareVersion ((Modulator*) &pdc->modulator, Processor_LINK, &version);
    	if (version != 0) {
        	pdc->modulator.booted = true;
    	} 
    	else {
        	pdc->modulator.booted = false;
    	}
	if (dwError) {deb_data("Demodulator_getFirmwareVersion  error\n");}

    	return(dwError); 
}

u32 NIM_ResetSeq(IN  void *	handle)
{
	u32 dwError = Error_NO_ERROR;
	PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;
	int i;
	//u8 ucValue = 0;
	
	u8 bootbuffer[6];
	//checksum = 0xFEDC
	bootbuffer[0] = 0x05;
	bootbuffer[1] = 0x00;
	bootbuffer[2] = 0x23;
	bootbuffer[3] = 0x01;
	bootbuffer[4] = 0xFE;
	bootbuffer[5] = 0xDC;	

	//aaa
	//reset 9133 -> boot -> demod init

	//GPIOH5 init
	dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK, p_reg_top_gpioh5_en, reg_top_gpioh5_en_pos, reg_top_gpioh5_en_len, 0);
	dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK, p_reg_top_gpioh5_on, reg_top_gpioh5_on_pos, reg_top_gpioh5_on_len, 0);
		
	//dwError = IT9507_writeRegisterBits((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, p_reg_top_gpioh5_o, reg_top_gpioh5_o_pos, reg_top_gpioh5_o_len, 0);
	//mdelay(100);

	deb_data("aaa start DRV_NIMReset");
	dwError = DRV_NIMReset(handle);

	dwError = DRV_InitNIMSuspendRegs(handle);
	
	deb_data("aaa start writeGenericRegisters");
	dwError = IT9507_writeGenericRegisters ((Modulator*) &pdc->modulator, 0x3a, 0x06, bootbuffer);
	
	deb_data("aaa start readGenericRegisters");
	dwError = IT9507_readGenericRegisters ((Modulator*) &pdc->modulator, 0x3a, 0x05, bootbuffer);
	deb_data("aaa print I2C reply");
	for(i=0; i<5; i++)
		deb_data("aaa bootbuffer[%d] = 0x%x", i, bootbuffer[i]);
	
//	mdelay(50); //delay for Fw boot	
	msleep(50); //delay for Fw boot	

	//IT9507_readRegister((Demodulator*) &pdc->Demodulator, Processor_LINK, 0x4100, &ucValue);
	
	//Demod & Tuner init
	deb_data("aaa DL_Initialize");
	dwError = DRV_Initialize(handle);

	return dwError;
}


//set tuner power saving and control
static u32 DRV_ApCtrl(
	void* handle,
	u8 ucSlaveDemod,
	bool bOn)
{
	u32 dwError = Error_NO_ERROR;
	PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;
	int i;
    u32 version = 0;
	
	deb_data("- Enter %s Function -\n",__FUNCTION__);
	deb_data("ucSlaveDemod = %d, bOn = %s \n", ucSlaveDemod, bOn?"ON":"OFF");

	if(bOn) 
	{
		pdc->fc[ucSlaveDemod].tunerinfo.bTunerInited = true;
		//[OMEGA] 
		if (pdc->architecture == Architecture_PIP)
		{
			if (pdc->fc[0].bTimerOn || pdc->fc[1].bTimerOn) {				
				deb_data("Already all power on ");
				return 0;
			}
		}
	}
	else 
	{	
		pdc->fc[ucSlaveDemod].tunerinfo.bTunerInited = false;	
		pdc->fc[ucSlaveDemod].tunerinfo.bTunerLock = false;
		
		//[OMEGA] if other one still alive, do not pwr down, just need to set freq;
		if (pdc->architecture == Architecture_PIP)
		{
			if (pdc->fc[0].bTimerOn || pdc->fc[1].bTimerOn) 
			{				
				deb_data("CLOSE 1<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
				IT9507_acquireTxChannel((Modulator*) &pdc->modulator, pdc->fc[!ucSlaveDemod].ucCurrentBandWidth, pdc->fc[!ucSlaveDemod].ulCurrentFrequency);
				return 0;
			}
		}
	}
   	//[OMEGA] clock from tuner, so close sequence demod->tuner, and 9133->9137. vice versa.
   	// BUT! demod->tuner:57mADC, tuner->demod:37mADC
	if (bOn)  //pwr on
	{
		if(pdc->chip_version == 1)
		{
			deb_data("aaa Power On\n");
			if (ucSlaveDemod == 1){
				dwError = NIM_ResetSeq(pdc);
				if(dwError)
				{								
					NIM_ResetSeq(pdc);
				}
			}
			else
			{
				//aaa			
				//CAP_KdPrint(("aaa DRV_TunerSuspend chip_%d", ucSlaveDemod));
				//dwError = DRV_TunerSuspend(handle, ucSlaveDemod, !bOn);
				//dwError = DRV_TunerPowerCtrl(handle, ucSlaveDemod, bOn);
				//if(dwError) TUNER_KdPrint(("DRV_ApCtrl::DRV_TunerSuspend Fail: 0x%04X", dwError));
				//DummyCmd
				pdc->UsbCtrlTimeOut = 1;
				for(i=0; i<5 ;i++) 
				{        
					deb_data("DRV_ApCtrl::DummyCmd %d\n", i);
					dwError = IT9507_getFirmwareVersion ((Modulator*) &pdc->modulator, Processor_LINK, &version);
//					mdelay(1);
					msleep(1);
					//if (!dwError) break;
				}
				pdc->UsbCtrlTimeOut = 5;
				
				deb_data("aaa IT9507_controlTunerPowerSaving chip_%d\n", ucSlaveDemod);
	//			dwError = IT9507_controlTunerPower((Demodulator*) &pdc->Demodulator, bOn);
	//			if(dwError) deb_data("DRV_ApCtrl::IT9507_controlTunerPowerSaving error = 0x%04ld", dwError);

				deb_data("aaa IT9507_controlPowerSaving chip_%d\n", ucSlaveDemod);
				dwError = IT9507_controlPowerSaving((Modulator*) &pdc->modulator, bOn);
				if(dwError) deb_data("DRV_ApCtrl::IT9507_controlPowerSaving error = 0x%04x", dwError);
			}
		}
		else
		{
			deb_data("aaa Power On\n");
			if (ucSlaveDemod == 1){
					deb_data("aaa GPIOH5 off\n");
					dwError = DRV_NIMSuspend(handle, false);
			}
			else
			{
				//DummyCmd
				pdc->UsbCtrlTimeOut = 1;
				for(i=0; i<5 ;i++) 
				{        
					deb_data("DRV_ApCtrl::DummyCmd %d\n", i);
					dwError = IT9507_getFirmwareVersion ((Modulator*) &pdc->modulator, Processor_LINK, &version);
//					mdelay(1);
					msleep(1);					
					//if (!dwError) break;
				}
				pdc->UsbCtrlTimeOut = 5;
			}
			//aaa			
			//CAP_KdPrint(("aaa DRV_TunerSuspend chip_%d", ucSlaveDemod));
			//dwError = DRV_TunerSuspend(handle, ucSlaveDemod, !bOn);
			//dwError = DRV_TunerPowerCtrl(handle, ucSlaveDemod, bOn);
			//if(dwError) TUNER_KdPrint(("DRV_ApCtrl::DRV_TunerSuspend Fail: 0x%04x", dwError));

			deb_data("aaa IT9507_controlTunerPowerSaving chip_%d\n", ucSlaveDemod);
	//		dwError = IT9507_controlTunerPower((Demodulator*) &pdc->Demodulator, bOn);
			if(dwError) deb_data("DRV_ApCtrl::IT9507_controlTunerPowerSaving error = 0x%04x\n", dwError);

			deb_data("aaa IT9507_controlPowerSaving chip_%d\n", ucSlaveDemod);
			dwError = IT9507_controlPowerSaving((Modulator*) &pdc->modulator, bOn);
//			mdelay(50);
			msleep(50);
			if(dwError) deb_data("DRV_ApCtrl::IT9507_controlPowerSaving error = 0x%04x\n", dwError);
		}
        deb_data("aaa Power On End-----------\n");		
    }
	else //pwr down:  DCA DUT: 36(all) -> 47-159(no GPIOH5, sequence change)
	{
		deb_data("aaa Power OFF\n");
	/*	if ( (ucSlaveDemod == 0) && (pdc->Demodulator.chipNumber == 2) ){
			
			//deb_data("aaa IT9507_controlTunerLeakage for Chip_1");
			//dwError = IT9507_controlTunerLeakage((Demodulator*) &pdc->Demodulator, 1, bOn);
			//if(dwError) deb_data("DRV_ApCtrl::IT9507_controlTunerLeakage error = 0x%04x", dwError);

			//deb_data("aaa GPIOH5 on");
			//dwError = DRV_NIMSuspend(handle, true);
			
		}*/
	
		deb_data("aaa IT9507_controlPowerSaving chip_%d\n", ucSlaveDemod);
		dwError = IT9507_controlPowerSaving((Modulator*) &pdc->modulator, bOn);
		if(dwError) deb_data("DRV_ApCtrl::IT9507_controlPowerSaving error = 0x%04x\n", dwError);
		
		deb_data("aaa IT9507_controlTunerPowerSaving chip_%d", ucSlaveDemod);
//		dwError = IT9507_controlTunerPower((Demodulator*) &pdc->Demodulator, bOn);
		if(dwError) deb_data("DRV_ApCtrl::IT9507_controlTunerPowerSaving error = 0x%04x\n", dwError);
		//deb_data("aaa DRV_TunerSuspend chip_%d", ucSlaveDemod);
		//dwError = DRV_TunerSuspend(handle, ucSlaveDemod, !bOn);
		//dwError = DRV_TunerPowerCtrl(handle, ucSlaveDemod, bOn);		
		//if(dwError) deb_data("DRV_ApCtrl::DRV_TunerSuspend Fail: 0x%04x", dwError);
	
		deb_data("aaa Power OFF End-----------\n");
    }

	return(dwError);
}


static u32 DRV_TunerWakeup(
      void *     handle
)
{   
    	u32 dwError = Error_NO_ERROR;

    	PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT) handle;

	deb_data("- Enter %s Function -\n",__FUNCTION__);

	//tuner power on
	dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK,  p_reg_top_gpioh7_o, reg_top_gpioh7_o_pos, reg_top_gpioh7_o_len, 1);

    return(dwError);

}

static u32 DRV_NIMSuspend(
    void *      handle,
    bool        bSuspend

)
{
    u32 dwError = Error_NO_ERROR;

    PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT) handle;

    deb_data("- Enter DRV_NIMSuspend : bSuspend = %s\n", bSuspend ? "ON":"OFF");

    if(bSuspend) { //sleep
    	dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK, p_reg_top_gpioh5_o, reg_top_gpioh5_o_pos, reg_top_gpioh5_o_len, 1);
		if(dwError) return (dwError);
    } else {        //resume 
		dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK, p_reg_top_gpioh5_o, reg_top_gpioh5_o_pos, reg_top_gpioh5_o_len, 0);
		if(dwError) return (dwError);
    }

    return(dwError);
}

static u32 DRV_InitNIMSuspendRegs(
    void *      handle
)
{
    u32 dwError = Error_NO_ERROR;

    PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT) handle;
    deb_data("- Enter %s Function -\n",__FUNCTION__);

    dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK, p_reg_top_gpioh5_en, reg_top_gpioh5_en_pos, reg_top_gpioh5_en_len, 1);
    dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK, p_reg_top_gpioh5_on, reg_top_gpioh5_on_pos, reg_top_gpioh5_on_len, 1);
    dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK, p_reg_top_gpioh5_o, reg_top_gpioh5_o_pos, reg_top_gpioh5_o_len, 0);

    dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK, p_reg_top_pwrdw, reg_top_pwrdw_pos, reg_top_pwrdw_len, 1);

    dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK, p_reg_top_pwrdw_hwen, reg_top_pwrdw_hwen_pos, reg_top_pwrdw_hwen_len, 1);

    return(dwError);
}

static u32 DRV_NIMReset(
    void *      handle
)
{


    u32   dwError = Error_NO_ERROR;

    PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;
    deb_data("- Enter %s Function -\n",__FUNCTION__);
    //Set AF0350 GPIOH1 to 0 to reset AF0351

    dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK,  p_reg_top_gpioh1_en, reg_top_gpioh1_en_pos, reg_top_gpioh1_en_len, 1);
    dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK,  p_reg_top_gpioh1_on, reg_top_gpioh1_on_pos, reg_top_gpioh1_on_len, 1);
    dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK,  p_reg_top_gpioh1_o, reg_top_gpioh1_o_pos, reg_top_gpioh1_o_len, 0);

//    mdelay(50);
	msleep(50);

    dwError = IT9507_writeRegisterBits((Modulator*) &pdc->modulator, Processor_LINK,  p_reg_top_gpioh1_o, reg_top_gpioh1_o_pos, reg_top_gpioh1_o_len, 1);

    return(dwError);
}

//************** DL_ *************//

static u32 DL_NIMSuspend(
    void *      handle,
    bool	bSuspend
)
{
	u32 dwError = Error_NO_ERROR;

	mutex_lock(&mymutex);

    dwError = DRV_NIMSuspend(handle, bSuspend);

    mutex_unlock(&mymutex);

    return (dwError);
}

static u32 DL_Initialize(
	    void *      handle
)
{
	u32 dwError = Error_NO_ERROR;
    mutex_lock(&mymutex);

    dwError = DRV_Initialize(handle);

	mutex_unlock(&mymutex);

	return (dwError); 
    
}

static u32 DL_SetBusTuner(
	 void * handle, 
	 u16 tunerId
)
{
	u32 dwError = Error_NO_ERROR;
	
	mutex_lock(&mymutex);

    dwError = DRV_SetBusTuner(handle, tunerId);

    mutex_unlock(&mymutex);

	return (dwError);

}

static u32  DL_GetEEPROMConfig(
	 void *      handle
)
{   
	u32 dwError = Error_NO_ERROR;
    mutex_lock(&mymutex);

    dwError = DRV_GetEEPROMConfig(handle);

    mutex_unlock(&mymutex);

    return(dwError);
} 

static u32 DL_TunerWakeup(
      void *     handle
)
{    
	u32 dwError = Error_NO_ERROR;
    mutex_lock(&mymutex);

    dwError = DRV_TunerWakeup(handle);

    mutex_unlock(&mymutex);
   
    	return(dwError);
}

u32 DL_TunerPowerCtrl(void* handle, u8 bPowerOn)
{
	u32 dwError = Error_NO_ERROR;
	u8    ucSlaveDemod=0;
  	PDEVICE_CONTEXT PDC = (PDEVICE_CONTEXT) handle;

    mutex_lock(&mymutex);

	deb_data("enter DL_TunerPowerCtrl:  bOn = %s\n", bPowerOn?"ON":"OFF");

/*    for (ucSlaveDemod=0; ucSlaveDemod<PDC->modulator.chipNumber; ucSlaveDemod++)
    {
    	dwError = DRV_TunerPowerCtrl(PDC, ucSlaveDemod, bPowerOn);
    	if(dwError) deb_data("  DRV_TunerPowerCtrl Fail: 0x%08x\n", dwError);
    }*/

    mutex_unlock(&mymutex);

    return (dwError);
}

u32 DL_ApPwCtrl (
	void* handle,
    bool  bChipCtl,
    bool  bOn
)
{
    u32 dwError = Error_NO_ERROR;
	u8    i = 0;
	PDEVICE_CONTEXT PDC = (PDEVICE_CONTEXT)handle;
	
	mutex_lock(&mymutex);

	deb_data("- Enter %s Function -",__FUNCTION__);
	deb_data("  chip =  %d  bOn = %s\n", bChipCtl, bOn?"ON":"OFF");

	if(bChipCtl) {    // 913x
#if 0
		if(bOn) {		// resume
			deb_data("IT9130x Power ON\n");		
			dwError = IT9507_writeRegisterBits((Modulator*) &PDC->modulator, Processor_LINK, p_reg_top_gpioh5_o, reg_top_gpioh5_o_pos, reg_top_gpioh5_o_len, 0);
			dwError = Demodulator_controlPowerSaving ((Demodulator*) &PDC->demodulator, bOn);				
			if(dwError) { 
				deb_data("ApCtrl::IT913x chip resume error = 0x%04x\n", dwError); 
				goto exit;
			}
		} else {       // suspend
			deb_data("IT9130x Power OFF\n");	
			dwError = IT9507_writeRegisterBits((Modulator*) &PDC->modulator, Processor_LINK, p_reg_top_gpioh5_o, reg_top_gpioh5_o_pos, reg_top_gpioh5_o_len, 1);
			dwError = Demodulator_controlPowerSaving ((Demodulator*) &PDC->demodulator, bOn);	
			if(dwError) { 
				deb_data("ApCtrl::IT913x chip suspend error = 0x%04x\n", dwError); 
				goto exit;
			}			
		}
#endif
	} else {          // 9507
		if(bOn) {	  // resume
			deb_data("IT950x Power ON\n");				
			dwError = IT9507_controlPowerSaving ((Modulator*) &PDC->modulator, bOn);				
			if(dwError) { 
				deb_data("ApCtrl::IT9507_controlPowerSaving error = 0x%04x\n", dwError); 
				goto exit;
			}
		} else {      // suspend
			//deb_data("IT950x TxMode RF OFF\n");				
			dwError = IT9507_setTxModeEnable((Modulator*) &PDC->modulator, 0);
			if(dwError) {
				deb_data("ApCtrl::IT9507_setTxModeEnable error = 0x%04x\n", dwError);
				goto exit;				
			}
			deb_data("IT950x Power OFF\n");							
			dwError = IT9507_controlPowerSaving ((Modulator*) &PDC->modulator, bOn);			
			if(dwError) {
				deb_data("ApCtrl::IT9507_controlPowerSaving error = 0x%04x\n", dwError);
				goto exit;
			}
		}			
	}

exit:
    mutex_unlock(&mymutex);
    	return(dwError);
}

u32 DL_ApCtrl (
	void* handle,
    u8  ucSlaveDemod,
    bool  bOn
)
{
    u32 dwError = Error_NO_ERROR;
	u8    i = 0;
	PDEVICE_CONTEXT PDC = (PDEVICE_CONTEXT)handle;
	
	mutex_lock(&mymutex);

	deb_data("- Enter %s Function -",__FUNCTION__);
	deb_data("  chip =  %d  bOn = %s\n", ucSlaveDemod, bOn?"ON":"OFF");

    if(PDC->architecture != Architecture_PIP)
    {

	//	if ( PDC->modulator.chipNumber == 2 && bOn) dwError = DL_NIMSuspend(PDC, false);

		/*for (i=0; i<PDC->modulator.chipNumber; i++)
		{	 
		   if (bOn) 
			dwError = DRV_ApCtrl (PDC, i, bOn);
		   else 
			if (PDC->bTunerPowerOff != true) dwError = DRV_ApCtrl (PDC, i, bOn);

			if(!bOn)
			{
					PDC->fc[i].ulDesiredFrequency = 0;
			PDC->fc[i].ucDesiredBandWidth = 0;
			}
		}*/

	//	if(PDC->modulator.chipNumber == 2 && !bOn) dwError = DL_NIMSuspend(PDC, true);
	}
    else
    {
		if (bOn) {

			PDC->fc[ucSlaveDemod].GraphBuilt = 1;

			if (PDC->fc[0].GraphBuilt == 0 ||  PDC->fc[1].GraphBuilt == 0)
			dwError = DL_NIMSuspend(PDC, false);

			dwError = DRV_ApCtrl (PDC, ucSlaveDemod, bOn);
		} else {

			PDC->fc[ucSlaveDemod].GraphBuilt = 0;

			if (PDC->bTunerPowerOff != true) dwError = DRV_ApCtrl (PDC, ucSlaveDemod, bOn);

			if (PDC->fc[0].GraphBuilt == 0 && PDC->fc[1].GraphBuilt == 0 && PDC->bTunerPowerOff == true)
			dwError = DL_NIMSuspend(PDC, true);
		}
    }
    mutex_unlock(&mymutex);

   	return(dwError);
}


u32 DL_CheckTunerInited(
	void* handle,
	u8 ucSlaveDemod,
	bool *bOn )
{
	u32 dwError = Error_NO_ERROR;
	PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;
	
    mutex_lock(&mymutex);

    deb_data("- Enter %s Function -\n",__FUNCTION__);

    *bOn = pdc->fc[ucSlaveDemod].tunerinfo.bTunerInited;

    mutex_unlock(&mymutex);

    return(dwError);
}

u32 DL_DemodIOCTLFun(Modulator *modulator, u32 IOCTLCode, unsigned long pIOBuffer)
{
    u32 dwError = Error_NO_ERROR;

    mutex_lock(&mymutex);

    deb_data("DemodIOCTLFun function");

    switch (IOCTLCode)
    {
        case IOCTL_ITE_DEMOD_GETDEVICETYPE_TX:
        {
            PTxGetDeviceTypeRequest pRequest = (PTxGetDeviceTypeRequest) pIOBuffer;
            pRequest->error = EagleUser_getDeviceType(modulator, &pRequest->DeviceType);
            break;
        }
        case IOCTL_ITE_DEMOD_SETDEVICETYPE_TX:
        {
            PTxSetDeviceTypeRequest pRequest = (PTxSetDeviceTypeRequest) pIOBuffer;

            if(EagleUser_setSystemConfig(modulator) != 0)
                printk("\n- EagleUser_setSystemConfig fail -\n");    
            else 
                printk("\n- EagleUser_setSystemConfig ok -\n");                
            break;
        }        
        case IOCTL_ITE_DEMOD_ADJUSTOUTPUTGAIN_TX: 
        {
            PSetGainRequest pRequest = (PSetGainRequest) pIOBuffer;
            pRequest->error = IT9507_adjustOutputGain(modulator, &pRequest->GainValue);
            break;
        }
        case IOCTL_ITE_DEMOD_ENABLETXMODE_TX:
        {
            PTxModeRequest pRequest = (PTxModeRequest) pIOBuffer;
            pRequest->error = IT9507_setTxModeEnable(modulator, pRequest->OnOff);
            deb_data("IT950x TxMode RF %s\n", pRequest->OnOff?"ON":"OFF");    
            break;
        }
        case IOCTL_ITE_DEMOD_SETMODULE_TX:
        {
            ChannelModulation temp;
            PSetModuleRequest pRequest = (PSetModuleRequest) pIOBuffer;
            temp.constellation = pRequest->constellation;
            temp.highCodeRate = pRequest->highCodeRate;
            temp.interval = pRequest->interval;
            temp.transmissionMode = pRequest->transmissionMode;
            pRequest->error = IT9507_setTXChannelModulation(modulator, &temp);
            pRequest->error = IT9507_setTxModeEnable(modulator, 1);
            deb_data("IT950x TxMode RF ON\n");                
            break;
        }
        case IOCTL_ITE_DEMOD_ACQUIRECHANNEL_TX:
        {
            PTxAcquireChannelRequest pRequest = (PTxAcquireChannelRequest) pIOBuffer;
            pRequest->error = IT9507_acquireTxChannel(modulator, pRequest->bandwidth, pRequest->frequency);
            break;
        }
        case IOCTL_ITE_DEMOD_GETFIRMWAREVERSION_TX:
        {
            PTxGetFirmwareVersionRequest pRequest = (PTxGetFirmwareVersionRequest) pIOBuffer;
            pRequest->error = IT9507_getFirmwareVersion (modulator, pRequest->processor, pRequest->version);
            break;
        }
        case IOCTL_ITE_DEMOD_CONTROLPIDFILTER_TX:
        {
            PTxControlPidFilterRequest pRequest = (PTxControlPidFilterRequest) pIOBuffer;
            pRequest->error = IT9507_controlPidFilter (modulator, pRequest->chip, pRequest->control);
            
            break;
        }
        case IOCTL_ITE_DEMOD_CONTROLPOWERSAVING:
        {
            PControlPowerSavingRequest pRequest = (PControlPowerSavingRequest) pIOBuffer;
            //DAVE: pRequest->error = Demodulator_controlPowerSaving ((Demodulator*) handle, pRequest->control);
            pRequest->error = IT9507_writeRegisterBits(modulator, Processor_LINK, p_reg_top_gpioh5_o, reg_top_gpioh5_o_pos, reg_top_gpioh5_o_len, !pRequest->control);
            break;
        }
        case IOCTL_ITE_DEMOD_CONTROLPOWERSAVING_TX:
        {
            PTxControlPowerSavingRequest pRequest = (PTxControlPowerSavingRequest) pIOBuffer;
            pRequest->error = IT9507_setTxModeEnable(modulator, pRequest->control);
            pRequest->error = IT9507_controlPowerSaving (modulator, pRequest->control);            
            break;
        }
        case IOCTL_ITE_DEMOD_WRITEGENERICREGISTERS_TX:
        {
            PWriteGenericRegistersRequest pRequest = (PWriteGenericRegistersRequest) pIOBuffer;
            pRequest->error = IT9507_writeGenericRegisters (modulator, pRequest->slaveAddress, pRequest->bufferLength, pRequest->buffer);

            break;
        }
        case IOCTL_ITE_DEMOD_READGENERICREGISTERS_TX:
        {
            PReadGenericRegistersRequest pRequest = (PReadGenericRegistersRequest) pIOBuffer;
            pRequest->error = IT9507_readGenericRegisters (modulator, pRequest->slaveAddress, pRequest->bufferLength, pRequest->buffer);

            break;
        }
        case IOCTL_ITE_DEMOD_FINALIZE:
        {
            PFinalizeRequest pRequest = (PFinalizeRequest) pIOBuffer;
            //pRequest->error = Demodulator_finalize ((Demodulator*) handle);
            //pRequest->error = Demodulator_controlPowerSaving ((Demodulator*) handle, 0);
            break;
        }
        case IOCTL_ITE_DEMOD_FINALIZE_TX:
        {
            PTxFinalizeRequest pRequest = (PTxFinalizeRequest) pIOBuffer;
            //pRequest->error = IT9507_finalize (modulator);
            //pRequest->error = IT9507_controlPowerSaving (modulator, 0);
            break;
        }
        case IOCTL_ITE_DEMOD_GETDRIVERINFO_TX:
        {
            //deb_data("IOCTL_ITE_DEMOD_GETDRIVERINFO %x, %x\n", pIOBuffer, handle);

            PTxModDriverInfo pDriverInfo = (PTxModDriverInfo)pIOBuffer;
            u32 dwFWVerionLink = 0;
            u32 dwFWVerionOFDM = 0;
            
            strcpy((char *)pDriverInfo->DriverVerion, DRIVER_RELEASE_VERSION);
            sprintf((char *)pDriverInfo->APIVerion, "%X.%X.%X.%X", (u8)(Eagle_Version_NUMBER>>8), (u8)(Eagle_Version_NUMBER), Eagle_Version_DATE, Eagle_Version_BUILD);

            IT9507_getFirmwareVersion (modulator, Processor_LINK, &dwFWVerionLink);
            sprintf((char *)pDriverInfo->FWVerionLink, "%X.%X.%X.%X", (u8)(dwFWVerionLink>>24), (u8)(dwFWVerionLink>>16), (u8)(dwFWVerionLink>>8), (u8)dwFWVerionLink);
            deb_data("Modulator_getFirmwareVersion Processor_LINK %s\n", (char *)pDriverInfo->FWVerionLink);
 
            IT9507_getFirmwareVersion (modulator, Processor_OFDM, &dwFWVerionOFDM);
            sprintf((char *)pDriverInfo->FWVerionOFDM, "%X.%X.%X.%X", (u8)(dwFWVerionOFDM>>24), (u8)(dwFWVerionOFDM>>16), (u8)(dwFWVerionOFDM>>8), (u8)dwFWVerionOFDM);
            deb_data("Modulator_getFirmwareVersion Processor_OFDM %s\n", (char *)pDriverInfo->FWVerionOFDM);

            strcpy((char *)pDriverInfo->Company, "ITEtech");
            strcpy((char *)pDriverInfo->SupportHWInfo, "Ganymede DVBT");

            pDriverInfo->error = Error_NO_ERROR;

            break;
        }
        case IOCTL_ITE_DEMOD_WRITEREGISTERS_TX:
        {
            PTxWriteRegistersRequest pRequest = (PTxWriteRegistersRequest) pIOBuffer;
            pRequest->error = IT9507_writeRegisters (modulator, pRequest->processor, pRequest->registerAddress, pRequest->bufferLength, pRequest->buffer);

            break;
        }
        case IOCTL_ITE_DEMOD_WRITEEEPROMVALUES_TX:
        {
            PWriteEepromValuesRequest pRequest = (PWriteEepromValuesRequest) pIOBuffer;
            pRequest->error = IT9507_writeEepromValues (modulator, pRequest->registerAddress, pRequest->bufferLength, pRequest->buffer);

            break;
        }
        case IOCTL_ITE_DEMOD_WRITEREGISTERBITS_TX:
        {
            PTxWriteRegisterBitsRequest pRequest = (PTxWriteRegisterBitsRequest) pIOBuffer;
            pRequest->error = IT9507_writeRegisterBits (modulator, pRequest->processor, pRequest->registerAddress, pRequest->position, pRequest->length, pRequest->value);

            break;
        }
        case IOCTL_ITE_DEMOD_READREGISTERS_TX:
        {
        //    deb_data("IOCTL_ITE_DEMOD_READREGISTERS\n");
            PTxReadRegistersRequest pRequest = (PTxReadRegistersRequest) pIOBuffer;
            pRequest->error = IT9507_readRegisters (modulator, pRequest->processor, pRequest->registerAddress, pRequest->bufferLength, pRequest->buffer);

            break;
        }
        case IOCTL_ITE_DEMOD_READEEPROMVALUES_TX:
        {
            PTxReadEepromValuesRequest pRequest = (PTxReadEepromValuesRequest) pIOBuffer;
            pRequest->error = IT9507_readEepromValues (modulator, pRequest->registerAddress, pRequest->bufferLength, pRequest->buffer);

            break;
        }
        case IOCTL_ITE_DEMOD_READREGISTERBITS_TX:
        {
            PTxReadRegisterBitsRequest pRequest = (PTxReadRegisterBitsRequest) pIOBuffer;
            pRequest->error = IT9507_readRegisterBits (modulator, pRequest->processor, pRequest->registerAddress, pRequest->position, pRequest->length, pRequest->value);

            break;
        }        
        case IOCTL_ITE_DEMOD_GETGAINRANGE_TX:
        {
            PGetGainRangeRequest pRequest = (PGetGainRangeRequest) pIOBuffer;        
            pRequest->error = IT9507_getGainRange (modulator, pRequest->frequency, pRequest->bandwidth, pRequest->maxGain, pRequest->minGain);
            break;
        }
        case IOCTL_ITE_DEMOD_GETTPS_TX:
        {
            PGetTPSRequest pRequest = (PGetTPSRequest) pIOBuffer;        
            pRequest->error = IT9507_getTPS (modulator, pRequest->pTps);
            break;
        }
        case IOCTL_ITE_DEMOD_SETTPS_TX:
        {
            PSetTPSRequest pRequest = (PSetTPSRequest) pIOBuffer;        
            pRequest->error = IT9507_setTPS (modulator, pRequest->tps);
            break;
        }
        case IOCTL_ITE_DEMOD_GETOUTPUTGAIN_TX:
        {
            PGetOutputGainRequest pRequest = (PGetOutputGainRequest) pIOBuffer;        
            pRequest->error = IT9507_getOutputGain (modulator, pRequest->gain);
            break;
        }
        case IOCTL_ITE_DEMOD_SENDHWPSITABLE_TX:
        {
            PSendHwPSITableRequest pRequest = (PSendHwPSITableRequest) pIOBuffer;        
            pRequest->error = IT9507_sendHwPSITable (modulator, pRequest->pbuffer);
            break;
        }
        case IOCTL_ITE_DEMOD_ACCESSFWPSITABLE_TX:
        {
            PAccessFwPSITableRequest pRequest = (PAccessFwPSITableRequest) pIOBuffer;        
            pRequest->error = IT9507_accessFwPSITable (modulator, pRequest->psiTableIndex, pRequest->pbuffer);
            break;
        }        
        case IOCTL_ITE_DEMOD_SETFWPSITABLETIMER_TX:
        {
            PSetFwPSITableTimerRequest pRequest = (PSetFwPSITableTimerRequest) pIOBuffer;        
            pRequest->error = IT9507_setFwPSITableTimer (modulator, pRequest->psiTableIndex, pRequest->timer);
            break;
        }        
        case IOCTL_ITE_DEMOD_SETIQTABLE_TX:
        {
            PTxSetIQTableRequest pRequest = (PTxSetIQTableRequest) pIOBuffer;        
            pRequest->error = IT9507_setIQtable (modulator, pRequest->IQ_table_ptr, pRequest->tableGroups);
            printk("Set IQtable group length is %d\n", pRequest->tableGroups);
            break;
        }            

        case IOCTL_ITE_DEMOD_SETDCCALIBRATIONVALUE_TX:
        {
            PTxSetDCCalibrationValueRequest pRequest = (PTxSetDCCalibrationValueRequest) pIOBuffer;        
            pRequest->error = IT9507_setDCCalibrationValue (modulator, pRequest->dc_i, pRequest->dc_q);
            printk("Set DC Calibration dc_i %d, dc_q %d\n", pRequest->dc_i, pRequest->dc_q);
            break;
        }
        
        default:
            //DBGPRINT(MP_WARN, (TEXT("\nDemodIOCTLFun Oid 0x%08x\n"), IOCTLCode));
            //DBGPRINT(MP_WARN, (TEXT("DemodInformation--->[not suported!!!]\n")));
            printk("NOT IMPLETEMENTED IOCONTROL!\n");
            dwError = ENOTTY;
    }

    mutex_unlock(&mymutex);

    return(dwError);
}

u32 Device_init(struct usb_device *udev, PDEVICE_CONTEXT PDC, bool bBoot)
{
	u32 error = Error_NO_ERROR;
	u8 filterIdx=0;
	int errcount=0;

	PDC->modulator.userData = (void *)udev;
	dev_set_drvdata(&udev->dev, PDC);

	deb_data("- Enter %s Function -\n",__FUNCTION__);

        printk("        DRIVER_RELEASE_VERSION  : %s\n", DRIVER_RELEASE_VERSION);
	printk("        EAGLE_FW_RELEASE_LINK_VERSION: %d.%d.%d.%d\n", DVB_LL_VERSION1, DVB_LL_VERSION2, DVB_LL_VERSION3, DVB_LL_VERSION4);
	printk("        EAGLE_FW_RELEASE_OFDM_VERSION: %d.%d.%d.%d\n", DVB_OFDM_VERSION1, DVB_OFDM_VERSION2, DVB_OFDM_VERSION3, DVB_OFDM_VERSION4);	
	printk("        API_TX_RELEASE_VERSION  : %X.%X.%X\n", Eagle_Version_NUMBER, Eagle_Version_DATE, Eagle_Version_BUILD);


	//************* Set Device init Info *************//
	PDC->bEnterSuspend = false;
    	PDC->bSurpriseRemoval = false;
    	PDC->bDevNotResp = false;
    	PDC->bSelectiveSuspend = false; 
	PDC->bTunerPowerOff = false;

	if (bBoot)
	{
		PDC->bSupportSelSuspend = false;
//		PDC->modulator.userData = (Handle)PDC;
//		PDC->modulator.chipNumber = 1; 
//		PDC->demodulator.userData = (Handle)PDC;
		PDC->architecture=Architecture_DCA;
		PDC->modulator.frequency = 666000;
		PDC->modulator.bandwidth = 8000;
		PDC->fc[0].tunerinfo.TunerId = 0;
		PDC->fc[1].tunerinfo.TunerId = 0;
		PDC->bDualTs=false;	
        	PDC->FilterCnt = 0;
		PDC->UsbCtrlTimeOut = 1;
	}
	else {
        	PDC->UsbCtrlTimeOut = 5;
    	}//bBoot

	if (bBoot) {
		//************* Set USB Info *************//
		PDC->MaxPacketSize = 0x0200; //default USB2.0
		PDC->UsbMode = (PDC->MaxPacketSize == 0x200)?0x0200:0x0110;  
		deb_data("USB mode= 0x%x\n", PDC->UsbMode);

		PDC->TsPacketCount = (PDC->UsbMode == 0x200)?TS_PACKET_COUNT_HI:TS_PACKET_COUNT_FU;
		PDC->TsFrames = (PDC->UsbMode == 0x200)?TS_FRAMES_HI:TS_FRAMES_FU;
		PDC->TsFrameSize = TS_PACKET_SIZE*PDC->TsPacketCount;
		PDC->TsFrameSizeDw = PDC->TsFrameSize/4;
	}
	PDC->bEP12Error = false;
    	PDC->bEP45Error = false; 
    	PDC->ForceWrite = false;    
    	PDC->ulActiveFilter = 0;
	
	if(bBoot)
    	{
		//patch for eeepc
        	//error = DL_SetBusTuner (PDC, Bus_USB, 0x38);
        	//PDC->UsbCtrlTimeOut = 5;
        
        	error = DL_SetBusTuner (PDC, 0x38);
        	if (error)
        	{ 
            		deb_data("First DL_SetBusTuner fail : 0x%08x\n",error );
			errcount++;
            		goto Exit; 
        	}

        	error =DL_GetEEPROMConfig(PDC);
        	if (error)
        	{
            		deb_data("DL_GetEEPROMConfig fail : 0x%08x\n", error);
			errcount++;
            		goto Exit;
        	}
	}//bBoot
	
	error = DL_SetBusTuner(PDC, PDC->fc[0].tunerinfo.TunerId);
	
    	if (error)
    	{
        	deb_data("DL_SetBusTuner fail!\n");
		errcount++;
        	goto Exit;
    	}

	
	 /*if (PDC->modulator.chipNumber == 2 && !PDC->modulator.booted) //plug/cold-boot/S4
    	{
        	error = DL_NIMReset(PDC);            
    	}
    	else if(PDC->modulator.chipNumber == 2 && PDC->modulator.booted) //warm-boot/(S1)
    	{
        	error = DL_NIMSuspend(PDC, false); 
		error = DL_TunerWakeup(PDC); //actually for mt2266
    	}*/
	
	
	//if(PDC->modulator.chipNumber == 1 && PDC->modulator.booted) //warm-boot/(S1)
	if(PDC->modulator.booted) //warm-boot/(S1)
	{
		error = DL_TunerWakeup(PDC);
	}
	if(error) deb_data("DL_NIMReset or DL_NIMSuspend or DL_TunerWakeup fail!\n"); 

	error = DL_Initialize(PDC);
	if (error) {
		deb_data("DL_Initialize fail! 0x%08x\n", error);
		errcount++;
		goto Exit;
	}

	deb_data("	%s success \n",__FUNCTION__);


	error = IT9507_writeRegister((Modulator*) &PDC->modulator, Processor_OFDM, 0xF7C6, 0x1);
	if(error)	printk( "AirHD Reg Write fail!\n");
	else printk( "AirHD Reg Write ok!\n");
	
Exit:
	
	if(errcount)
        printk( "[Device_init] Error %d\n", errcount);
	return (error);
}
