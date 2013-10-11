#ifndef __MODULATOR_VARIABLE_H__
#define __MODULATOR_VARIABLE_H__

#define OVA_BASE							0x4C00							// omega variable address base
#define OVA_EEPROM_CFG						(OVA_BASE-620)					// 256bytes
#define OVA_SECOND_DEMOD_I2C_ADDR		(OVA_BASE-5)

#define second_i2c_address                             OVA_SECOND_DEMOD_I2C_ADDR       //0x417F
#define chip_version_7_0					0x1222

#endif

