#ifndef __MODULATORERROR_H__
#define __MODULATORERROR_H__

#define ModulatorError_NO_ERROR							0x00000000ul
#define ModulatorError_RESET_TIMEOUT					0x00000001ul
#define ModulatorError_WRITE_REG_TIMEOUT				0x00000002ul
#define ModulatorError_WRITE_TUNER_TIMEOUT				0x00000003ul
#define ModulatorError_WRITE_TUNER_FAIL					0x00000004ul
#define ModulatorError_RSD_COUNTER_NOT_READY			0x00000005ul
#define ModulatorError_VTB_COUNTER_NOT_READY			0x00000006ul
#define ModulatorError_FEC_MON_NOT_ENABLED				0x00000007ul
#define ModulatorError_INVALID_DEV_TYPE					0x00000008ul
#define ModulatorError_INVALID_TUNER_TYPE				0x00000009ul
#define ModulatorError_OPEN_FILE_FAIL					0x0000000Aul
#define ModulatorError_WRITEFILE_FAIL					0x0000000Bul
#define ModulatorError_READFILE_FAIL					0x0000000Cul
#define ModulatorError_CREATEFILE_FAIL					0x0000000Dul
#define ModulatorError_MALLOC_FAIL						0x0000000Eul
#define ModulatorError_INVALID_FILE_SIZE				0x0000000Ful
#define ModulatorError_INVALID_READ_SIZE				0x00000010ul
#define ModulatorError_LOAD_FW_DONE_BUT_FAIL			0x00000011ul
#define ModulatorError_NOT_IMPLEMENTED					0x00000012ul
#define ModulatorError_NOT_SUPPORT						0x00000013ul
#define ModulatorError_WRITE_MBX_TUNER_TIMEOUT			0x00000014ul
#define ModulatorError_DIV_MORE_THAN_8_CHIPS			0x00000015ul
#define ModulatorError_DIV_NO_CHIPS						0x00000016ul
#define ModulatorError_SUPER_FRAME_CNT_0				0x00000017ul
#define ModulatorError_INVALID_FFT_MODE					0x00000018ul
#define ModulatorError_INVALID_CONSTELLATION_MODE		0x00000019ul
#define ModulatorError_RSD_PKT_CNT_0					0x0000001Aul
#define ModulatorError_FFT_SHIFT_TIMEOUT				0x0000001Bul
#define ModulatorError_WAIT_TPS_TIMEOUT					0x0000001Cul
#define ModulatorError_INVALID_BW						0x0000001Dul
#define ModulatorError_INVALID_BUF_LEN					0x0000001Eul
#define ModulatorError_NULL_PTR							0x0000001Ful
#define ModulatorError_INVALID_AGC_VOLT					0x00000020ul
#define ModulatorError_MT_OPEN_FAIL						0x00000021ul
#define ModulatorError_MT_TUNE_FAIL						0x00000022ul
#define ModulatorError_CMD_NOT_SUPPORTED				0x00000023ul
#define ModulatorError_CE_NOT_READY						0x00000024ul
#define ModulatorError_EMBX_INT_NOT_CLEARED				0x00000025ul
#define ModulatorError_INV_PULLUP_VOLT					0x00000026ul
#define ModulatorError_FREQ_OUT_OF_RANGE				0x00000027ul
#define ModulatorError_INDEX_OUT_OF_RANGE				0x00000028ul
#define ModulatorError_NULL_SETTUNER_PTR				0x00000029ul
#define ModulatorError_NULL_INITSCRIPT_PTR				0x0000002Aul
#define ModulatorError_INVALID_INITSCRIPT_LEN			0x0000002Bul
#define ModulatorError_INVALID_POS						0x0000002Cul
#define ModulatorError_BACK_TO_BOOTCODE_FAIL			0x0000002Dul
#define ModulatorError_GET_BUFFER_VALUE_FAIL			0x0000002Eul
#define ModulatorError_INVALID_REG_VALUE				0x0000002Ful
#define ModulatorError_INVALID_INDEX					0x00000030ul
#define ModulatorError_READ_TUNER_TIMEOUT				0x00000031ul
#define ModulatorError_READ_TUNER_FAIL					0x00000032ul
#define ModulatorError_UNDEFINED_SAW_BW					0x00000033ul
#define ModulatorError_MT_NOT_AVAILABLE					0x00000034ul
#define ModulatorError_NO_SUCH_TABLE					0x00000035ul
#define ModulatorError_WRONG_CHECKSUM					0x00000036ul
#define ModulatorError_INVALID_XTAL_FREQ				0x00000037ul
#define ModulatorError_COUNTER_NOT_AVAILABLE			0x00000038ul
#define ModulatorError_INVALID_DATA_LENGTH				0x00000039ul
#define ModulatorError_BOOT_FAIL						0x0000003Aul
#define ModulatorError_INITIALIZE_FAIL					0x0000003Aul
#define ModulatorError_BUFFER_INSUFFICIENT				0x0000003Bul
#define ModulatorError_NOT_READY						0x0000003Cul
#define ModulatorError_DRIVER_INVALID					0x0000003Dul
#define ModulatorError_INTERFACE_FAIL					0x0000003Eul
#define ModulatorError_PID_FILTER_FULL					0x0000003Ful
#define ModulatorError_OPERATION_TIMEOUT				0x00000040ul
#define ModulatorError_LOADFIRMWARE_SKIPPED				0x00000041ul
#define ModulatorError_REBOOT_FAIL						0x00000042ul
#define ModulatorError_PROTOCOL_FORMAT_INVALID			0x00000043ul
#define ModulatorError_ACTIVESYNC_ERROR					0x00000044ul
#define ModulatorError_CE_READWRITEBUS_ERROR			0x00000045ul
#define ModulatorError_CE_NODATA_ERROR					0x00000046ul
#define ModulatorError_NULL_FW_SCRIPT					0x00000047ul
#define ModulatorError_NULL_TUNER_SCRIPT				0x00000048ul
#define ModulatorError_INVALID_CHIP_TYPE				0x00000049ul
#define ModulatorError_TUNER_TYPE_NOT_COMPATIBLE		0x0000004Aul
#define ModulatorError_NULL_HANDLE_PTR					0x0000004Bul
#define ModulatorError_UNDEFINED_FREQ					0x0000004Cul
#define ModulatorError_INVALID_SYSTEM_CONFIG			0x0000004Dul
#define ModulatorError_INVALID_FW_TYPE					0x0000004Eul
#define ModulatorError_INVALID_BUS_TYPE					0x0000004Ful
#define ModulatorError_BITRATE_OUT_OF_RANGE				0x00000050ul
#define ModulatorError_OUT_OF_CALIBRATION_RANGE			0x00000051ul


/** Error Code of System */
#define ModulatorError_INVALID_INDICATOR_TYPE			0x00000101ul
#define ModulatorError_INVALID_SC_NUMBER				0x00000102ul
#define ModulatorError_INVALID_SC_INFO					0x00000103ul
#define ModulatorError_FIGBYPASS_FAIL					0x00000104ul

/** Error Code of Firmware */
#define ModulatorError_FIRMWARE_STATUS					0x01000000ul

/** Error Code of I2C Module */
#define ModulatorError_I2C_DATA_HIGH_FAIL				0x02001000ul
#define ModulatorError_I2C_CLK_HIGH_FAIL				0x02002000ul
#define ModulatorError_I2C_WRITE_NO_ACK					0x02003000ul
#define ModulatorError_I2C_DATA_LOW_FAIL				0x02004000ul

/** Error Code of USB Module */
#define ModulatorError_USB_NULL_HANDLE					0x03010001ul
#define ModulatorError_USB_WRITEFILE_FAIL				0x03000002ul
#define ModulatorError_USB_READFILE_FAIL				0x03000003ul
#define ModulatorError_USB_INVALID_READ_SIZE			0x03000004ul
#define ModulatorError_USB_INVALID_STATUS				0x03000005ul
#define ModulatorError_USB_INVALID_SN					0x03000006ul
#define ModulatorError_USB_INVALID_PKT_SIZE				0x03000007ul
#define ModulatorError_USB_INVALID_HEADER				0x03000008ul
#define ModulatorError_USB_NO_IR_PKT					0x03000009ul
#define ModulatorError_USB_INVALID_IR_PKT				0x0300000Aul
#define ModulatorError_USB_INVALID_DATA_LEN				0x0300000Bul
#define ModulatorError_USB_EP4_READFILE_FAIL			0x0300000Cul
#define ModulatorError_USB_EP$_INVALID_READ_SIZE		0x0300000Dul
#define ModulatorError_USB_BOOT_INVALID_PKT_TYPE		0x0300000Eul
#define ModulatorError_USB_BOOT_BAD_CONFIG_HEADER		0x0300000Ful
#define ModulatorError_USB_BOOT_BAD_CONFIG_SIZE			0x03000010ul
#define ModulatorError_USB_BOOT_BAD_CONFIG_SN			0x03000011ul
#define ModulatorError_USB_BOOT_BAD_CONFIG_SUBTYPE		0x03000012ul
#define ModulatorError_USB_BOOT_BAD_CONFIG_VALUE		0x03000013ul
#define ModulatorError_USB_BOOT_BAD_CONFIG_CHKSUM		0x03000014ul
#define ModulatorError_USB_BOOT_BAD_CONFIRM_HEADER		0x03000015ul
#define ModulatorError_USB_BOOT_BAD_CONFIRM_SIZE		0x03000016ul
#define ModulatorError_USB_BOOT_BAD_CONFIRM_SN			0x03000017ul
#define ModulatorError_USB_BOOT_BAD_CONFIRM_SUBTYPE		0x03000018ul
#define ModulatorError_USB_BOOT_BAD_CONFIRM_VALUE		0x03000019ul
#define ModulatorError_USB_BOOT_BAD_CONFIRM_CHKSUM		0x03000020ul
#define ModulatorError_USB_BOOT_BAD_BOOT_HEADER			0x03000021ul
#define ModulatorError_USB_BOOT_BAD_BOOT_SIZE			0x03000022ul
#define ModulatorError_USB_BOOT_BAD_BOOT_SN				0x03000023ul
#define ModulatorError_USB_BOOT_BAD_BOOT_PATTERN_01		0x03000024ul
#define ModulatorError_USB_BOOT_BAD_BOOT_PATTERN_10		0x03000025ul
#define ModulatorError_USB_BOOT_BAD_BOOT_CHKSUM			0x03000026ul
#define ModulatorError_USB_INVALID_BOOT_PKT_TYPE		0x03000027ul
#define ModulatorError_USB_BOOT_BAD_CONFIG_VAlUE		0x03000028ul
#define ModulatorError_USB_COINITIALIZEEX_FAIL			0x03000029ul
#define ModulatorError_USB_COCREATEINSTANCE_FAIL		0x0300003Aul
#define ModulatorError_USB_COCREATCLSEENUMERATOR_FAIL   0x0300002Bul
#define ModulatorError_USB_QUERY_INTERFACE_FAIL			0x0300002Cul
#define ModulatorError_USB_PKSCTRL_NULL					0x0300002Dul
#define ModulatorError_USB_INVALID_REGMODE				0x0300002Eul
#define ModulatorError_USB_INVALID_REG_COUNT			0x0300002Ful
#define ModulatorError_USB_INVALID_HANDLE				0x03000100ul
#define ModulatorError_USB_WRITE_FAIL					0x03000200ul
#define ModulatorError_USB_UNEXPECTED_WRITE_LEN			0x03000300ul
#define ModulatorError_USB_READ_FAIL					0x03000400ul

/** Error code of 9035U2I bridge*/
#define ModulatorError_AF9035U2I                        0x04000000ul
#define ModulatorError_EAGLESLAVE_FAIL                  0x04000000ul

/** Error code of Omega */						
#define ModulatorError_TUNER_INIT_FAIL					0x05000000ul
#define ModulatorError_OMEGA_TUNER_INIT_FAIL			0x05000000ul



#endif