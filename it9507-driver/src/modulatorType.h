#ifndef __MODULATOR_TYPE_H__
#define __MODULATOR_TYPE_H__

#define EagleUser_INTERNAL	 1

#include <linux/stddef.h>
#include <linux/types.h>


#ifdef __cplusplus
#define EXPORT extern "C" __declspec(dllexport)
#else
#define EXPORT __declspec(dllexport)
#endif
//#define ModulatorStatus extern "C" DllExport Dword

#define IN
#define OUT
#define IT9507Cmd_buildCommand(command, processor)  (command + (u16) (processor << 12))
#define Eagle_MAX_BIT               8
#define IQ_TABLE_NROW 92
#define Command_REG_DEMOD_READ          0x0000
#define Command_REG_DEMOD_WRITE         0x0001
#define Command_REG_EEPROM_READ         0x0004
#define Command_REG_EEPROM_WRITE        0x0005
#define Command_IR_GET                  0x0018
#define Command_QUERYINFO               0x0022
#define Command_BOOT                    0x0023
#define Command_REBOOT                  0x0023
#define Command_SCATTER_WRITE           0x0029
#define Command_GENERIC_READ            0x002A
#define Command_GENERIC_WRITE           0x002B


typedef struct {
    u32 frequency;      /**  */
    int  dAmp;			  /**  */
	int  dPhi;	
} IQtable;

typedef struct {
    IQtable *ptrIQtableEx;
	u16 tableGroups;		//Number of IQtable groups;
	int	outputGain;	
    u16 c1DefaultValue;
	u16 c2DefaultValue;
	u16 c3DefaultValue;
} CalibrationInfo;

/**
 * The type defination of Processor.
 */
typedef enum {
    Processor_LINK = 0,
    Processor_OFDM = 8
} Processor;

typedef struct {
    u32*                  len;
    u8*                       cmd;
    u8                        reserved[16];
} CmdRequest, *PCmdRequest;

//--------------------------

extern const u8 Eagle_bitMask[8];
#define REG_MASK(pos, len)                (Eagle_bitMask[len-1] << pos)
#define REG_CLEAR(temp, pos, len)         (temp & (~REG_MASK(pos, len)))
#define REG_CREATE(val, temp, pos, len)   ((val << pos) | (REG_CLEAR(temp, pos, len)))
#define REG_GET(value, pos, len)          ((value & REG_MASK(pos, len)) >> pos)
#endif
