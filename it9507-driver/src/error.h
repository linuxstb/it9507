/********************************************************************************************/
/**                                                                                         */
/**       These value in this file are automatically generated and used by API.             */
/**       Please not modify the content.                                                    */
/**                                                                                         */
/********************************************************************************************/
#ifndef __ERROR_H__
#define __ERROR_H__

#define Error_NO_ERROR							0x00000000
#define Error_RESET_TIMEOUT						0x00000001
#define Error_WRITE_REG_TIMEOUT					0x00000002
#define Error_WRITE_TUNER_TIMEOUT				0x00000003
#define Error_WRITE_TUNER_FAIL					0x00000004
#define Error_RSD_COUNTER_NOT_READY				0x00000005
#define Error_VTB_COUNTER_NOT_READY				0x00000006
#define Error_FEC_MON_NOT_ENABLED				0x00000007
#define Error_INVALID_DEV_TYPE					0x00000008
#define Error_INVALID_TUNER_TYPE				0x00000009
#define Error_OPEN_FILE_FAIL					0x0000000A
#define Error_WRITEFILE_FAIL					0x0000000B
#define Error_READFILE_FAIL						0x0000000C
#define Error_CREATEFILE_FAIL					0x0000000D
#define Error_MALLOC_FAIL						0x0000000E
#define Error_INVALID_FILE_SIZE					0x0000000F
#define Error_INVALID_READ_SIZE					0x00000010
#define Error_LOAD_FW_DONE_BUT_FAIL				0x00000011
#define Error_NOT_IMPLEMENTED					0x00000012
#define Error_NOT_SUPPORT						0x00000013
#define Error_WRITE_MBX_TUNER_TIMEOUT			0x00000014
#define Error_DIV_MORE_THAN_8_CHIPS				0x00000015
#define Error_DIV_NO_CHIPS						0x00000016
#define Error_SUPER_FRAME_CNT_0					0x00000017
#define Error_INVALID_FFT_MODE					0x00000018
#define Error_INVALID_CONSTELLATION_MODE		0x00000019
#define Error_RSD_PKT_CNT_0						0x0000001A
#define Error_FFT_SHIFT_TIMEOUT					0x0000001B
#define Error_WAIT_TPS_TIMEOUT					0x0000001C
#define Error_INVALID_BW						0x0000001D
#define Error_INVALID_BUF_LEN					0x0000001E
#define Error_NULL_PTR							0x0000001F
#define Error_INVALID_AGC_VOLT					0x00000020
#define Error_MT_OPEN_FAIL						0x00000021
#define Error_MT_TUNE_FAIL						0x00000022
#define Error_CMD_NOT_SUPPORTED					0x00000023
#define Error_CE_NOT_READY						0x00000024
#define Error_EMBX_INT_NOT_CLEARED				0x00000025
#define Error_INV_PULLUP_VOLT					0x00000026
#define Error_FREQ_OUT_OF_RANGE					0x00000027
#define Error_INDEX_OUT_OF_RANGE				0x00000028
#define Error_NULL_SETTUNER_PTR					0x00000029
#define Error_NULL_INITSCRIPT_PTR				0x0000002A
#define Error_INVALID_INITSCRIPT_LEN			0x0000002B
#define Error_INVALID_POS						0x0000002C
#define Error_BACK_TO_BOOTCODE_FAIL				0x0000002D
#define Error_GET_BUFFER_VALUE_FAIL				0x0000002E
#define Error_INVALID_REG_VALUE					0x0000002F
#define Error_INVALID_INDEX						0x00000030
#define Error_READ_TUNER_TIMEOUT				0x00000031
#define Error_READ_TUNER_FAIL					0x00000032
#define Error_UNDEFINED_SAW_BW					0x00000033
#define Error_MT_NOT_AVAILABLE					0x00000034
#define Error_NO_SUCH_TABLE						0x00000035
#define Error_WRONG_CHECKSUM					0x00000036
#define Error_INVALID_XTAL_FREQ					0x00000037
#define Error_COUNTER_NOT_AVAILABLE				0x00000038
#define Error_INVALID_DATA_LENGTH				0x00000039
#define Error_BOOT_FAIL							0x0000003A
#define Error_BUFFER_INSUFFICIENT				0x0000003B
#define Error_NOT_READY							0x0000003C
#define Error_DRIVER_INVALID					0x0000003D
#define Error_INTERFACE_FAIL					0x0000003E
#define Error_PID_FILTER_FULL					0x0000003F
#define Error_OPERATION_TIMEOUT					0x00000040
#define Error_LOADFIRMWARE_SKIPPED				0x00000041
#define Error_REBOOT_FAIL						0x00000042
#define Error_PROTOCOL_FORMAT_INVALID			0x00000043
#define Error_ACTIVESYNC_ERROR					0x00000044
#define Error_CE_READWRITEBUS_ERROR				0x00000045
#define Error_CE_NODATA_ERROR					0x00000046
#define Error_NULL_FW_SCRIPT					0x00000047
#define Error_NULL_TUNER_SCRIPT					0x00000048
#define Error_NULL_HANDLE_PTR					0x0000004B
#define Error_CLK_STRAP0_INVALID_BW				0x00000050
#define Error_CLK_STRAP1_INVALID_BW				0x00000051

/** Error Code of Gemini System */
#define Error_INVALID_INDICATOR_TYPE			0x00000101
#define Error_INVALID_SC_NUMBER					0x00000102
#define Error_INVALID_SC_INFO					0x00000103
#define Error_FIGBYPASS_FAIL					0x00000104

/** Error Code of Firmware */
#define Error_FIRMWARE_STATUS					0x01000000

/** Error Code of I2C Module */
#define Error_I2C_DATA_HIGH_FAIL				0x02001000
#define Error_I2C_CLK_HIGH_FAIL					0x02002000
#define Error_I2C_WRITE_NO_ACK					0x02003000
#define Error_I2C_DATA_LOW_FAIL					0x02004000

/** Error Code of USB Module */
#define Error_USB_NULL_HANDLE					0x03010001
#define Error_USB_WRITEFILE_FAIL				0x03000002
#define Error_USB_READFILE_FAIL					0x03000003
#define Error_USB_INVALID_READ_SIZE				0x03000004
#define Error_USB_INVALID_STATUS				0x03000005
#define Error_USB_INVALID_SN					0x03000006
#define Error_USB_INVALID_PKT_SIZE				0x03000007
#define Error_USB_INVALID_HEADER				0x03000008
#define Error_USB_NO_IR_PKT						0x03000009
#define Error_USB_INVALID_IR_PKT				0x0300000A
#define Error_USB_INVALID_DATA_LEN				0x0300000B
#define Error_USB_EP4_READFILE_FAIL				0x0300000C
#define Error_USB_EP4_INVALID_READ_SIZE			0x0300000D
#define Error_USB_BOOT_INVALID_PKT_TYPE			0x0300000E
#define Error_USB_BOOT_BAD_CONFIG_HEADER		0x0300000F
#define Error_USB_BOOT_BAD_CONFIG_SIZE			0x03000010
#define Error_USB_BOOT_BAD_CONFIG_SN			0x03000011
#define Error_USB_BOOT_BAD_CONFIG_SUBTYPE		0x03000012
#define Error_USB_BOOT_BAD_CONFIG_VALUE			0x03000013
#define Error_USB_BOOT_BAD_CONFIG_CHKSUM		0x03000014
#define Error_USB_BOOT_BAD_CONFIRM_HEADER		0x03000015
#define Error_USB_BOOT_BAD_CONFIRM_SIZE			0x03000016
#define Error_USB_BOOT_BAD_CONFIRM_SN			0x03000017
#define Error_USB_BOOT_BAD_CONFIRM_SUBTYPE		0x03000018
#define Error_USB_BOOT_BAD_CONFIRM_VALUE		0x03000019
#define Error_USB_BOOT_BAD_CONFIRM_CHKSUM		0x03000020
#define Error_USB_BOOT_BAD_BOOT_HEADER			0x03000021
#define Error_USB_BOOT_BAD_BOOT_SIZE			0x03000022
#define Error_USB_BOOT_BAD_BOOT_SN				0x03000023
#define Error_USB_BOOT_BAD_BOOT_PATTERN_01		0x03000024
#define Error_USB_BOOT_BAD_BOOT_PATTERN_10		0x03000025
#define Error_USB_BOOT_BAD_BOOT_CHKSUM			0x03000026
#define Error_USB_INVALID_BOOT_PKT_TYPE			0x03000027
#define Error_USB_BOOT_BAD_CONFIG_VAlUE			0x03000028
#define Error_USB_COINITIALIZEEX_FAIL			0x03000029
#define Error_USB_COCREATEINSTANCE_FAIL			0x0300003A
#define Error_USB_COCREATCLSEENUMERATOR_FAIL    0x0300002B
#define Error_USB_QUERY_INTERFACE_FAIL			0x0300002C
#define Error_USB_PKSCTRL_NULL					0x0300002D
#define Error_USB_INVALID_REGMODE				0x0300002E
#define Error_USB_INVALID_REG_COUNT				0x0300002F
#define Error_USB_INVALID_HANDLE				0x03000100
#define Error_USB_WRITE_FAIL					0x03000200
#define Error_USB_UNEXPECTED_WRITE_LEN			0x03000300
#define Error_USB_READ_FAIL						0x03000400

/** Error code of 9035U2I bridge*/
#define Error_AF9035U2I                         0x04000000

/** Error code of Omega */						
#define Error_OMEGA_TUNER_INIT_FAIL				0x05000000

/** Error code of Castor*/
#define Error_CASTOR                            0x70000000
#define Error_CASTOR_BUS_NO_SUPPORT             0x00001000

#endif
