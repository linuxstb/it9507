#ifndef __MODULATORERROR_H__
#define __MODULATORERROR_H__

#define ModulatorError_NO_ERROR							0x00000000
#define ModulatorError_RESET_TIMEOUT					0x00000001
#define ModulatorError_WRITE_REG_TIMEOUT				0x00000002
#define ModulatorError_WRITE_TUNER_TIMEOUT				0x00000003
#define ModulatorError_WRITE_TUNER_FAIL					0x00000004
#define ModulatorError_RSD_COUNTER_NOT_READY			0x00000005
#define ModulatorError_VTB_COUNTER_NOT_READY			0x00000006
#define ModulatorError_FEC_MON_NOT_ENABLED				0x00000007
#define ModulatorError_INVALID_DEV_TYPE					0x00000008
#define ModulatorError_INVALID_TUNER_TYPE				0x00000009
#define ModulatorError_OPEN_FILE_FAIL					0x0000000A
#define ModulatorError_WRITEFILE_FAIL					0x0000000B
#define ModulatorError_READFILE_FAIL					0x0000000C
#define ModulatorError_CREATEFILE_FAIL					0x0000000D
#define ModulatorError_MALLOC_FAIL						0x0000000E
#define ModulatorError_INVALID_FILE_SIZE				0x0000000F
#define ModulatorError_INVALID_READ_SIZE				0x00000010
#define ModulatorError_LOAD_FW_DONE_BUT_FAIL			0x00000011
#define ModulatorError_NOT_IMPLEMENTED					0x00000012
#define ModulatorError_NOT_SUPPORT						0x00000013
#define ModulatorError_WRITE_MBX_TUNER_TIMEOUT			0x00000014
#define ModulatorError_DIV_MORE_THAN_8_CHIPS			0x00000015
#define ModulatorError_DIV_NO_CHIPS						0x00000016
#define ModulatorError_SUPER_FRAME_CNT_0				0x00000017
#define ModulatorError_INVALID_FFT_MODE					0x00000018
#define ModulatorError_INVALID_CONSTELLATION_MODE		0x00000019
#define ModulatorError_RSD_PKT_CNT_0					0x0000001A
#define ModulatorError_FFT_SHIFT_TIMEOUT				0x0000001B
#define ModulatorError_WAIT_TPS_TIMEOUT					0x0000001C
#define ModulatorError_INVALID_BW						0x0000001D
#define ModulatorError_INVALID_BUF_LEN					0x0000001E
#define ModulatorError_NULL_PTR							0x0000001F
#define ModulatorError_INVALID_AGC_VOLT					0x00000020
#define ModulatorError_MT_OPEN_FAIL						0x00000021
#define ModulatorError_MT_TUNE_FAIL						0x00000022
#define ModulatorError_CMD_NOT_SUPPORTED				0x00000023
#define ModulatorError_CE_NOT_READY						0x00000024
#define ModulatorError_EMBX_INT_NOT_CLEARED				0x00000025
#define ModulatorError_INV_PULLUP_VOLT					0x00000026
#define ModulatorError_FREQ_OUT_OF_RANGE				0x00000027
#define ModulatorError_INDEX_OUT_OF_RANGE				0x00000028
#define ModulatorError_NULL_SETTUNER_PTR				0x00000029
#define ModulatorError_NULL_INITSCRIPT_PTR				0x0000002A
#define ModulatorError_INVALID_INITSCRIPT_LEN			0x0000002B
#define ModulatorError_INVALID_POS						0x0000002C
#define ModulatorError_BACK_TO_BOOTCODE_FAIL			0x0000002D
#define ModulatorError_GET_BUFFER_VALUE_FAIL			0x0000002E
#define ModulatorError_INVALID_REG_VALUE				0x0000002F
#define ModulatorError_INVALID_INDEX					0x00000030
#define ModulatorError_READ_TUNER_TIMEOUT				0x00000031
#define ModulatorError_READ_TUNER_FAIL					0x00000032
#define ModulatorError_UNDEFINED_SAW_BW					0x00000033
#define ModulatorError_MT_NOT_AVAILABLE					0x00000034
#define ModulatorError_NO_SUCH_TABLE					0x00000035
#define ModulatorError_WRONG_CHECKSUM					0x00000036
#define ModulatorError_INVALID_XTAL_FREQ				0x00000037
#define ModulatorError_COUNTER_NOT_AVAILABLE			0x00000038
#define ModulatorError_INVALID_DATA_LENGTH				0x00000039
#define ModulatorError_BOOT_FAIL						0x0000003A
#define ModulatorError_INITIALIZE_FAIL					0x0000003A
#define ModulatorError_BUFFER_INSUFFICIENT				0x0000003B
#define ModulatorError_NOT_READY						0x0000003C
#define ModulatorError_DRIVER_INVALID					0x0000003D
#define ModulatorError_INTERFACE_FAIL					0x0000003E
#define ModulatorError_PID_FILTER_FULL					0x0000003F
#define ModulatorError_OPERATION_TIMEOUT				0x00000040
#define ModulatorError_LOADFIRMWARE_SKIPPED				0x00000041
#define ModulatorError_REBOOT_FAIL						0x00000042
#define ModulatorError_PROTOCOL_FORMAT_INVALID			0x00000043
#define ModulatorError_ACTIVESYNC_ERROR					0x00000044
#define ModulatorError_CE_READWRITEBUS_ERROR			0x00000045
#define ModulatorError_CE_NODATA_ERROR					0x00000046
#define ModulatorError_NULL_FW_SCRIPT					0x00000047
#define ModulatorError_NULL_TUNER_SCRIPT				0x00000048
#define ModulatorError_INVALID_CHIP_TYPE				0x00000049
#define ModulatorError_TUNER_TYPE_NOT_COMPATIBLE		0x0000004A
#define ModulatorError_NULL_HANDLE_PTR					0x0000004B
#define ModulatorError_UNDEFINED_FREQ					0x0000004C
#define ModulatorError_INVALID_SYSTEM_CONFIG			0x0000004D
#define ModulatorError_INVALID_FW_TYPE					0x0000004E
#define ModulatorError_INVALID_BUS_TYPE					0x0000004F
#define ModulatorError_BITRATE_OUT_OF_RANGE				0x00000050
#define ModulatorError_OUT_OF_CALIBRATION_RANGE			0x00000051


/** Error Code of System */
#define ModulatorError_INVALID_INDICATOR_TYPE			0x00000101
#define ModulatorError_INVALID_SC_NUMBER				0x00000102
#define ModulatorError_INVALID_SC_INFO					0x00000103
#define ModulatorError_FIGBYPASS_FAIL					0x00000104

/** Error Code of Firmware */
#define ModulatorError_FIRMWARE_STATUS					0x01000000

/** Error Code of I2C Module */
#define ModulatorError_I2C_DATA_HIGH_FAIL				0x02001000
#define ModulatorError_I2C_CLK_HIGH_FAIL				0x02002000
#define ModulatorError_I2C_WRITE_NO_ACK					0x02003000
#define ModulatorError_I2C_DATA_LOW_FAIL				0x02004000

/** Error Code of USB Module */
#define ModulatorError_USB_NULL_HANDLE					0x03010001
#define ModulatorError_USB_WRITEFILE_FAIL				0x03000002
#define ModulatorError_USB_READFILE_FAIL				0x03000003
#define ModulatorError_USB_INVALID_READ_SIZE			0x03000004
#define ModulatorError_USB_INVALID_STATUS				0x03000005
#define ModulatorError_USB_INVALID_SN					0x03000006
#define ModulatorError_USB_INVALID_PKT_SIZE				0x03000007
#define ModulatorError_USB_INVALID_HEADER				0x03000008
#define ModulatorError_USB_NO_IR_PKT					0x03000009
#define ModulatorError_USB_INVALID_IR_PKT				0x0300000A
#define ModulatorError_USB_INVALID_DATA_LEN				0x0300000B
#define ModulatorError_USB_EP4_READFILE_FAIL			0x0300000C
#define ModulatorError_USB_EP$_INVALID_READ_SIZE		0x0300000D
#define ModulatorError_USB_BOOT_INVALID_PKT_TYPE		0x0300000E
#define ModulatorError_USB_BOOT_BAD_CONFIG_HEADER		0x0300000F
#define ModulatorError_USB_BOOT_BAD_CONFIG_SIZE			0x03000010
#define ModulatorError_USB_BOOT_BAD_CONFIG_SN			0x03000011
#define ModulatorError_USB_BOOT_BAD_CONFIG_SUBTYPE		0x03000012
#define ModulatorError_USB_BOOT_BAD_CONFIG_VALUE		0x03000013
#define ModulatorError_USB_BOOT_BAD_CONFIG_CHKSUM		0x03000014
#define ModulatorError_USB_BOOT_BAD_CONFIRM_HEADER		0x03000015
#define ModulatorError_USB_BOOT_BAD_CONFIRM_SIZE		0x03000016
#define ModulatorError_USB_BOOT_BAD_CONFIRM_SN			0x03000017
#define ModulatorError_USB_BOOT_BAD_CONFIRM_SUBTYPE		0x03000018
#define ModulatorError_USB_BOOT_BAD_CONFIRM_VALUE		0x03000019
#define ModulatorError_USB_BOOT_BAD_CONFIRM_CHKSUM		0x03000020
#define ModulatorError_USB_BOOT_BAD_BOOT_HEADER			0x03000021
#define ModulatorError_USB_BOOT_BAD_BOOT_SIZE			0x03000022
#define ModulatorError_USB_BOOT_BAD_BOOT_SN				0x03000023
#define ModulatorError_USB_BOOT_BAD_BOOT_PATTERN_01		0x03000024
#define ModulatorError_USB_BOOT_BAD_BOOT_PATTERN_10		0x03000025
#define ModulatorError_USB_BOOT_BAD_BOOT_CHKSUM			0x03000026
#define ModulatorError_USB_INVALID_BOOT_PKT_TYPE		0x03000027
#define ModulatorError_USB_BOOT_BAD_CONFIG_VAlUE		0x03000028
#define ModulatorError_USB_COINITIALIZEEX_FAIL			0x03000029
#define ModulatorError_USB_COCREATEINSTANCE_FAIL		0x0300003A
#define ModulatorError_USB_COCREATCLSEENUMERATOR_FAIL   0x0300002B
#define ModulatorError_USB_QUERY_INTERFACE_FAIL			0x0300002C
#define ModulatorError_USB_PKSCTRL_NULL					0x0300002D
#define ModulatorError_USB_INVALID_REGMODE				0x0300002E
#define ModulatorError_USB_INVALID_REG_COUNT			0x0300002F
#define ModulatorError_USB_INVALID_HANDLE				0x03000100
#define ModulatorError_USB_WRITE_FAIL					0x03000200
#define ModulatorError_USB_UNEXPECTED_WRITE_LEN			0x03000300
#define ModulatorError_USB_READ_FAIL					0x03000400

/** Error code of 9035U2I bridge*/
#define ModulatorError_AF9035U2I                        0x04000000
#define ModulatorError_EAGLESLAVE_FAIL                  0x04000000

/** Error code of Omega */						
#define ModulatorError_TUNER_INIT_FAIL					0x05000000
#define ModulatorError_OMEGA_TUNER_INIT_FAIL			0x05000000



#endif
