/**
 * Copyright (c) 2006-2013 ITEtech Corporation. All rights reserved.
 *
 * Module Name:
 *     iocontrol.cpp
 *
 * Abstract:
 *     Demodulator IOCTL Query and Set functions
 */


#include "it950x-core.h"
#include "version.h"
#include "IT9507.h"

/*****************************************************************************
*
*  Function:   DemodIOCTLFun
*
*  Arguments:  handle             - The handle of demodulator or modulator.
*              IOCTLCode               - Device IO control code
*              pIOBuffer               - buffer containing data for the IOCTL
*
*  Returns:    Error_NO_ERROR: successful, non-zero error code otherwise.
*
*  Notes:
*
*****************************************************************************/
// IRQL:DISPATCH_LEVEL
DWORD DemodIOCTLFun(
    void *       handle,
    Dword        IOCTLCode,
    unsigned long       pIOBuffer
    )
{
    Dword error = Error_NO_ERROR;

    deb_data("DemodIOCTLFun function");

    switch (IOCTLCode)
    {
		case IOCTL_ITE_DEMOD_GETDEVICETYPE_TX:
		{
			PTxGetDeviceTypeRequest pRequest = (PTxGetDeviceTypeRequest) pIOBuffer;
			pRequest->error = EagleUser_getDeviceType((Modulator*) handle, &pRequest->DeviceType);
			break;
		}
		case IOCTL_ITE_DEMOD_SETDEVICETYPE_TX:
		{
			PTxSetDeviceTypeRequest pRequest = (PTxSetDeviceTypeRequest) pIOBuffer;
			SystemConfig syscfg;

			if(EagleUser_getSystemConfig((Modulator*) handle, pRequest->DeviceType, &syscfg) != 0)
				printk("- DeviceType id: %d, EagleUser_getSystemConfig fail -\n", pRequest->DeviceType);	
			else 
				printk("\n- DeviceType id: %d, EagleUser_getSystemConfig ok -\n", pRequest->DeviceType);				

			((Modulator*) handle)->systemConfig = syscfg;

			if(EagleUser_setSystemConfig((Modulator*) handle, syscfg) != 0)
				printk("\n- EagleUser_setSystemConfig fail -\n");	
			else 
				printk("\n- EagleUser_setSystemConfig ok -\n");				
			break;
		}		
		case IOCTL_ITE_DEMOD_ADJUSTOUTPUTGAIN_TX: 
		{
			PSetGainRequest pRequest = (PSetGainRequest) pIOBuffer;
			pRequest->error = IT9507_adjustOutputGain((Modulator*) handle, &pRequest->GainValue);
			break;
		}
		case IOCTL_ITE_DEMOD_ENABLETXMODE_TX:
		{
			PTxModeRequest pRequest = (PTxModeRequest) pIOBuffer;
			pRequest->error = IT9507_setTxModeEnable((Modulator*) handle, pRequest->OnOff);
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
			pRequest->error = IT9507_setTXChannelModulation((Modulator*) handle, &temp);
			pRequest->error = IT9507_setTxModeEnable((Modulator*) handle, 1);
			deb_data("IT950x TxMode RF ON\n");				
			break;
        }
		case IOCTL_ITE_DEMOD_ACQUIRECHANNEL_TX:
		{
			PTxAcquireChannelRequest pRequest = (PTxAcquireChannelRequest) pIOBuffer;
			pRequest->error = IT9507_acquireTxChannel((Modulator*) handle, pRequest->bandwidth, pRequest->frequency);
			break;
        }
        case IOCTL_ITE_DEMOD_GETFIRMWAREVERSION_TX:
        {
            PTxGetFirmwareVersionRequest pRequest = (PTxGetFirmwareVersionRequest) pIOBuffer;
			pRequest->error = IT9507_getFirmwareVersion ((Modulator*) handle, pRequest->processor, pRequest->version);
    		break;
        }
        case IOCTL_ITE_DEMOD_CONTROLPIDFILTER_TX:
        {
			PTxControlPidFilterRequest pRequest = (PTxControlPidFilterRequest) pIOBuffer;
			pRequest->error = IT9507_controlPidFilter ((Modulator*) handle, pRequest->chip, pRequest->control);
            
    		break;
        }
        case IOCTL_ITE_DEMOD_CONTROLPOWERSAVING:
        {
			PControlPowerSavingRequest pRequest = (PControlPowerSavingRequest) pIOBuffer;
			//DAVE: pRequest->error = Demodulator_controlPowerSaving ((Demodulator*) handle, pRequest->control);
#define    p_reg_top_gpioh5_o 0xD8BB 
#define reg_top_gpioh5_o_pos 0
#define reg_top_gpioh5_o_len 1

			pRequest->error = IT9507_writeRegisterBits((Modulator*) handle, Processor_LINK, p_reg_top_gpioh5_o, reg_top_gpioh5_o_pos, reg_top_gpioh5_o_len, !pRequest->control);
    		break;
        }
        case IOCTL_ITE_DEMOD_CONTROLPOWERSAVING_TX:
        {
			PTxControlPowerSavingRequest pRequest = (PTxControlPowerSavingRequest) pIOBuffer;
			pRequest->error = IT9507_setTxModeEnable((Modulator*) handle, pRequest->control);
			pRequest->error = IT9507_controlPowerSaving ((Modulator*) handle, pRequest->control);			
    		break;
        }
		case IOCTL_ITE_DEMOD_WRITEGENERICREGISTERS_TX:
		{
			PWriteGenericRegistersRequest pRequest = (PWriteGenericRegistersRequest) pIOBuffer;
            pRequest->error = IT9507_writeGenericRegisters ((Modulator*) handle, pRequest->slaveAddress, pRequest->bufferLength, pRequest->buffer);

    		break;
		}
        case IOCTL_ITE_DEMOD_READGENERICREGISTERS_TX:
        {
			PReadGenericRegistersRequest pRequest = (PReadGenericRegistersRequest) pIOBuffer;
			pRequest->error = IT9507_readGenericRegisters ((Modulator*) handle, pRequest->slaveAddress, pRequest->bufferLength, pRequest->buffer);

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
            //pRequest->error = IT9507_finalize ((Modulator*) handle);
            //pRequest->error = IT9507_controlPowerSaving ((Modulator*) handle, 0);
    		break;
        }
        case IOCTL_ITE_DEMOD_GETDRIVERINFO_TX:
        {
			//deb_data("IOCTL_ITE_DEMOD_GETDRIVERINFO %x, %x\n", pIOBuffer, handle);

			PTxModDriverInfo pDriverInfo = (PTxModDriverInfo)pIOBuffer;
			DWORD dwFWVerionLink = 0;
			DWORD dwFWVerionOFDM = 0;
			
            strcpy((char *)pDriverInfo->DriverVerion, DRIVER_RELEASE_VERSION);
			sprintf((char *)pDriverInfo->APIVerion, "%X.%X.%X.%X", (BYTE)(Eagle_Version_NUMBER>>8), (BYTE)(Eagle_Version_NUMBER), Eagle_Version_DATE, Eagle_Version_BUILD);

			IT9507_getFirmwareVersion ((Modulator*) handle, Processor_LINK, &dwFWVerionLink);
			sprintf((char *)pDriverInfo->FWVerionLink, "%X.%X.%X.%X", (BYTE)(dwFWVerionLink>>24), (BYTE)(dwFWVerionLink>>16), (BYTE)(dwFWVerionLink>>8), (BYTE)dwFWVerionLink);
			deb_data("Modulator_getFirmwareVersion Processor_LINK %s\n", (char *)pDriverInfo->FWVerionLink);
 
			IT9507_getFirmwareVersion ((Modulator*) handle, Processor_OFDM, &dwFWVerionOFDM);
			sprintf((char *)pDriverInfo->FWVerionOFDM, "%X.%X.%X.%X", (BYTE)(dwFWVerionOFDM>>24), (BYTE)(dwFWVerionOFDM>>16), (BYTE)(dwFWVerionOFDM>>8), (BYTE)dwFWVerionOFDM);
			deb_data("Modulator_getFirmwareVersion Processor_OFDM %s\n", (char *)pDriverInfo->FWVerionOFDM);

			//strcpy((char *)pDriverInfo->DateTime, ""__DATE__" "__TIME__"");
			strcpy((char *)pDriverInfo->Company, "ITEtech");
//#ifdef JUPITER
//			strcpy((char *)pDriverInfo->SupportHWInfo, "Jupiter DVBT/H");
//#endif
//#ifdef GANYMEDE
			strcpy((char *)pDriverInfo->SupportHWInfo, "Ganymede DVBT");
//#endif
//#ifdef GEMINI
//			strcpy((char *)pDriverInfo->SupportHWInfo, "Gemini DVBT/H,TDMB/DAB,FM");
//#endif
			pDriverInfo->error = Error_NO_ERROR;

    		break;
        }
        case IOCTL_ITE_DEMOD_WRITEREGISTERS_TX:
        {
			PTxWriteRegistersRequest pRequest = (PTxWriteRegistersRequest) pIOBuffer;
            pRequest->error = IT9507_writeRegisters ((Modulator*) handle, pRequest->processor, pRequest->registerAddress, pRequest->bufferLength, pRequest->buffer);

    		break;
        }
        case IOCTL_ITE_DEMOD_WRITEEEPROMVALUES_TX:
        {
			PWriteEepromValuesRequest pRequest = (PWriteEepromValuesRequest) pIOBuffer;
			pRequest->error = IT9507_writeEepromValues ((Modulator*) handle, pRequest->registerAddress, pRequest->bufferLength, pRequest->buffer);

    		break;
        }
        case IOCTL_ITE_DEMOD_WRITEREGISTERBITS_TX:
        {
			PTxWriteRegisterBitsRequest pRequest = (PTxWriteRegisterBitsRequest) pIOBuffer;
			pRequest->error = IT9507_writeRegisterBits ((Modulator*) handle, pRequest->processor, pRequest->registerAddress, pRequest->position, pRequest->length, pRequest->value);

    		break;
        }
        case IOCTL_ITE_DEMOD_READREGISTERS_TX:
        {
		//	deb_data("IOCTL_ITE_DEMOD_READREGISTERS\n");
			PTxReadRegistersRequest pRequest = (PTxReadRegistersRequest) pIOBuffer;
			pRequest->error = IT9507_readRegisters ((Modulator*) handle, pRequest->processor, pRequest->registerAddress, pRequest->bufferLength, pRequest->buffer);

    		break;
        }
        case IOCTL_ITE_DEMOD_READEEPROMVALUES_TX:
        {
			PTxReadEepromValuesRequest pRequest = (PTxReadEepromValuesRequest) pIOBuffer;
			pRequest->error = IT9507_readEepromValues ((Modulator*) handle, pRequest->registerAddress, pRequest->bufferLength, pRequest->buffer);

    		break;
        }
        case IOCTL_ITE_DEMOD_READREGISTERBITS_TX:
        {
			PTxReadRegisterBitsRequest pRequest = (PTxReadRegisterBitsRequest) pIOBuffer;
			pRequest->error = IT9507_readRegisterBits ((Modulator*) handle, pRequest->processor, pRequest->registerAddress, pRequest->position, pRequest->length, pRequest->value);

    		break;
        }        
        case IOCTL_ITE_DEMOD_GETGAINRANGE_TX:
        {
			PGetGainRangeRequest pRequest = (PGetGainRangeRequest) pIOBuffer;		
			pRequest->error = IT9507_getGainRange ((Modulator*) handle, pRequest->frequency, pRequest->bandwidth, pRequest->maxGain, pRequest->minGain);
			break;
		}
        case IOCTL_ITE_DEMOD_GETTPS_TX:
        {
			PGetTPSRequest pRequest = (PGetTPSRequest) pIOBuffer;		
			pRequest->error = IT9507_getTPS ((Modulator*) handle, pRequest->pTps);
			break;
		}
        case IOCTL_ITE_DEMOD_SETTPS_TX:
        {
			PSetTPSRequest pRequest = (PSetTPSRequest) pIOBuffer;		
			pRequest->error = IT9507_setTPS ((Modulator*) handle, pRequest->tps);
			break;
		}
        case IOCTL_ITE_DEMOD_GETOUTPUTGAIN_TX:
        {
			PGetOutputGainRequest pRequest = (PGetOutputGainRequest) pIOBuffer;		
			pRequest->error = IT9507_getOutputGain ((Modulator*) handle, pRequest->gain);
			break;
		}
        case IOCTL_ITE_DEMOD_SENDHWPSITABLE_TX:
        {
			PSendHwPSITableRequest pRequest = (PSendHwPSITableRequest) pIOBuffer;		
			pRequest->error = IT9507_sendHwPSITable ((Modulator*) handle, pRequest->pbuffer);
			break;
		}
        case IOCTL_ITE_DEMOD_ACCESSFWPSITABLE_TX:
        {
			PAccessFwPSITableRequest pRequest = (PAccessFwPSITableRequest) pIOBuffer;		
			pRequest->error = IT9507_accessFwPSITable ((Modulator*) handle, pRequest->psiTableIndex, pRequest->pbuffer);
			break;
		}		
        case IOCTL_ITE_DEMOD_SETFWPSITABLETIMER_TX:
        {
			PSetFwPSITableTimerRequest pRequest = (PSetFwPSITableTimerRequest) pIOBuffer;		
			pRequest->error = IT9507_setFwPSITableTimer ((Modulator*) handle, pRequest->psiTableIndex, pRequest->timer);
			break;
		}		
		case IOCTL_ITE_DEMOD_SETIQTABLE_TX:
		{
			PTxSetIQTableRequest pRequest = (PTxSetIQTableRequest) pIOBuffer;		
			pRequest->error = IT9507_setIQtable ((Modulator*) handle, pRequest->IQ_table_ptr, pRequest->tableGroups);
			printk("Set IQtable group length is %d\n", pRequest->tableGroups);
			break;
		}			

		case IOCTL_ITE_DEMOD_SETDCCALIBRATIONVALUE_TX:
		{
			PTxSetDCCalibrationValueRequest pRequest = (PTxSetDCCalibrationValueRequest) pIOBuffer;		
			pRequest->error = IT9507_setDCCalibrationValue ((Modulator*) handle, pRequest->dc_i, pRequest->dc_q);
			printk("Set DC Calibration dc_i %d, dc_q %d\n", pRequest->dc_i, pRequest->dc_q);
			break;
		}
		
        default:
            //DBGPRINT(MP_WARN, (TEXT("\nDemodIOCTLFun Oid 0x%08x\n"), IOCTLCode));
            //DBGPRINT(MP_WARN, (TEXT("DemodInformation--->[not suported!!!]\n")));
            printk("NOT IMPLETEMENTED IOCONTROL!\n");
            return ENOTTY;
    }
    
    return error;
}
