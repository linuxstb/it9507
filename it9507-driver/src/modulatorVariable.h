#ifndef __MODULATOR_VARIABLE_H__
#define __MODULATOR_VARIABLE_H__

// ----- LL variables -----



#define OVA_BASE							0x4C00							// omega variable address base

//#define OVA_PRECHIP_VERSION_7_0
#define OVA_LINK_VERSION					(OVA_BASE-4)					// 4 byte
#define OVA_LINK_VERSION_31_24			(OVA_LINK_VERSION+0)
#define OVA_LINK_VERSION_23_16			(OVA_LINK_VERSION+1)
#define OVA_LINK_VERSION_15_8				(OVA_LINK_VERSION+2)
#define OVA_LINK_VERSION_7_0				(OVA_LINK_VERSION+3)
#define OVA_SECOND_DEMOD_I2C_ADDR		(OVA_BASE-5)

#define OVA_IR_TABLE                    (OVA_BASE-361)                   // 7 * 50 + 6 bytes
#define OVA_HID_TABLE					OVA_IR_TABLE                     // 7 * 50 bytes
#define OVA_IR_TOGGLE_MASK				(OVA_HID_TABLE+7*50)             // 2 bytes
#define OVA_IR_nKEYS					(OVA_IR_TOGGLE_MASK+2)           // 1 byte
#define OVA_IR_FN_EXPIRE_TIME			(OVA_IR_nKEYS+1)                 // 1 byte
#define OVA_IR_REPEAT_PERIOD			(OVA_IR_FN_EXPIRE_TIME+1)        // 1 byte
#define OVA_IR_RESERVED_PARAM			(OVA_IR_REPEAT_PERIOD+1)         // 1 byte

#define OVA_IR_TABLE_ADDR					(OVA_BASE-363)					// 2 bytes pointer point to IR_TABLE
#define OVA_IR_TABLE_ADDR_15_18			(OVA_IR_TABLE_ADDR+0)
#define OVA_IR_TABLE_ADDR_7_0				(OVA_IR_TABLE_ADDR+1)
#define OVA_HOST_REQ_IR_MODE				(OVA_BASE-364)
#define OVA_EEPROM_CFG						(OVA_BASE-620)					// 256bytes
#define OVA_XC4000_PKTCNT					(OVA_BASE-621)
#define OVA_XC4000_CLKCNT1				(OVA_BASE-623)			// 2 bytes
#define OVA_XC4000_CLKCNT2				(OVA_BASE-625)			// 2 bytes
#define OVA_I2C_NO_STOPBIT_PKTCNT	(OVA_BASE-626)
#define OVA_CLK_STRETCH						(OVA_BASE-643)
#define OVA_DUMMY0XX							(OVA_BASE-644)
#define OVA_HW_VERSION						(OVA_BASE-645)
#define OVA_TMP_HW_VERSION			(OVA_BASE-646)

// for API variable name, not use in firmware

#define PSI_table1                      0x5800
#define PSI_table2                      0x58bc
#define PSI_table3                      0x5978
#define PSI_table4                      0x5a34
#define PSI_table5                      0x5af0

#define psi_table1_timer_H                  0x0007
#define psi_table1_timer_L                  0x0008
#define psi_table2_timer_H                  0x0009
#define psi_table2_timer_L                  0x000a
#define psi_table3_timer_H                  0x000b
#define psi_table3_timer_L                  0x000c
#define psi_table4_timer_H                  0x000d
#define psi_table4_timer_L                  0x000e
#define psi_table5_timer_H                  0x000f
#define psi_table5_timer_L                  0x0010

#define modulator_var_end                             0x0011

#define second_i2c_address				OVA_SECOND_DEMOD_I2C_ADDR	//0x417F
#define ir_table_start_15_8				OVA_IR_TABLE_ADDR_15_18		//0x417F
#define ir_table_start_7_0				OVA_IR_TABLE_ADDR_7_0			//0x4180
#define prechip_version_7_0				0x384F
#define chip_version_7_0					0x1222
#define link_version_31_24				OVA_LINK_VERSION_31_24			//0x83E9
#define link_version_23_16				OVA_LINK_VERSION_23_16			//0x83EA
#define link_version_15_8				OVA_LINK_VERSION_15_8			//0x83EB
#define link_version_7_0					OVA_LINK_VERSION_7_0			//0x83EC



#endif

