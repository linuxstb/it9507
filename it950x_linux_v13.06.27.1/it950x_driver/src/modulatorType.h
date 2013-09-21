#ifndef __MODULATOR_TYPE_H__
#define __MODULATOR_TYPE_H__

#define EagleUser_INTERNAL	 1
/* *

 * for Linux 

 */

/* define NULL, true, flase */

#include <linux/stddef.h>

/* define bool */

#include <linux/types.h>

/* include Linux delay */

#include <linux/delay.h>
/* define BYTE ,WORD....*/
//#include "Common.h"





#ifndef UNDER_CE
#endif


#ifdef __cplusplus
#define EXPORT extern "C" __declspec(dllexport)
#else
#define EXPORT __declspec(dllexport)
#endif
//#define ModulatorStatus extern "C" DllExport Dword

#define Tuner_Afatech_OMEGA                 0x38
#define Tuner_Afatech_OMEGA_LNA_Config_1	0x51 // 0x50~0x5f reserved for OMEGA use
#define Tuner_Afatech_OMEGA_LNA_Config_2    0x52
#define Tuner_Afatech_OMEGA_V2              0x60 // 0x60~0x6f reserved for OMEGA V2 use
#define Tuner_Afatech_OMEGA_V2_LNA_Config_1	0x61 
#define Tuner_Afatech_OMEGA_V2_LNA_Config_2 0x62
#define Tuner_Afatech_AF9007                0xFF
#define IN
#define OUT
#define INOUT
#define Bus_I2C             1
#define Bus_USB             2
#define Bus_9035U2I         3  /** I2C bus for Ganymede USB */
#define IT9507Cmd_buildCommand(command, processor)  (command + (Word) (processor << 12))
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


/**
 * The type of handle.
 */
typedef void* Handle;


/**
 * The type defination of 8-bits unsigned type.
 */
typedef unsigned char Byte;


/**
 * The type defination of 16-bits unsigned type.
 */
typedef unsigned short Word;


/**
 * The type defination of 32-bits unsigned type.
 */
typedef unsigned long Dword;

/**
 * The type defination of 64-bits unsigned type.
 */
typedef long long LONGLONG;

/**
 * The type defination of 16-bits signed type.
 */
typedef short Short;


/**
 * The type defination of 32-bits signed type.
 */
typedef long Long;


/**
 * The type defination of ValueSet.
 */
typedef struct {
    Dword frequency;      /**  */
    int  dAmp;			  /**  */
	int  dPhi;	
} IQtable;

typedef struct {
    IQtable *ptrIQtableEx;
	Word tableGroups;		//Number of IQtable groups;
	int	outputGain;	
    Word c1DefaultValue;
	Word c2DefaultValue;
	Word c3DefaultValue;
} CalibrationInfo;

/**
 * The type defination of Bool
 */

typedef enum {
    False = 0,
    True = 1
} Bool
#ifndef __cplusplus
//,bool
#endif
;

//typedef enum {False=0, True} bool;


typedef struct {
    Byte segmentType;           /** 0:Firmware download 1:Rom copy 2:Direct command */
    Dword segmentLength;
} Segment;


/**
 * The type defination of Bandwidth.
 */
typedef enum {
    Bandwidth_6M = 0,           /** Signal bandwidth is 6MHz */
    Bandwidth_7M,               /** Signal bandwidth is 7MHz */
    Bandwidth_8M,               /** Signal bandwidth is 8MHz */
    Bandwidth_5M,               /** Signal bandwidth is 5MHz */
	Bandwidth_4M,               /** Signal bandwidth is 4MHz */
	Bandwidth_3M,               /** Signal bandwidth is 3MHz */
	Bandwidth_2_5M ,            /** Signal bandwidth is 2.5MHz */
	Bandwidth_2M                /** Signal bandwidth is 2MHz */
} Bandwidth;

/**
 * The type defination of TransmissionMode.
 */
typedef enum {
    TransmissionMode_2K = 0,    /** OFDM frame consists of 2048 different carriers (2K FFT mode) */
    TransmissionMode_8K = 1,    /** OFDM frame consists of 8192 different carriers (8K FFT mode) */
    TransmissionMode_4K = 2     /** OFDM frame consists of 4096 different carriers (4K FFT mode) */
} TransmissionModes;


/**
 * The type defination of Interval.
 */
typedef enum {
    Interval_1_OVER_32 = 0,     /** Guard interval is 1/32 of symbol length */
    Interval_1_OVER_16,         /** Guard interval is 1/16 of symbol length */
    Interval_1_OVER_8,          /** Guard interval is 1/8 of symbol length  */
    Interval_1_OVER_4           /** Guard interval is 1/4 of symbol length  */
} Interval;


/**
 * The type defination of Priority.
 */
typedef enum {
    Priority_HIGH = 0,          /** DVB-T and DVB-H - identifies high-priority stream */
    Priority_LOW                /** DVB-T and DVB-H - identifies low-priority stream  */
} Priority;


/**
 * The type defination of CodeRate.
 */
typedef enum {
    CodeRate_1_OVER_2 = 0,      /** Signal uses FEC coding ratio of 1/2 */
    CodeRate_2_OVER_3,          /** Signal uses FEC coding ratio of 2/3 */
    CodeRate_3_OVER_4,          /** Signal uses FEC coding ratio of 3/4 */
    CodeRate_5_OVER_6,          /** Signal uses FEC coding ratio of 5/6 */
    CodeRate_7_OVER_8,          /** Signal uses FEC coding ratio of 7/8 */
    CodeRate_NONE               /** None, NXT doesn't have this one     */
} CodeRate;


/**
 * TPS Hierarchy and Alpha value.
 */
typedef enum {
    Hierarchy_NONE = 0,         /** Signal is non-hierarchical        */
    Hierarchy_ALPHA_1,          /** Signalling format uses alpha of 1 */
    Hierarchy_ALPHA_2,          /** Signalling format uses alpha of 2 */
    Hierarchy_ALPHA_4           /** Signalling format uses alpha of 4 */
} Hierarchy;

/**
 * The defination of SubchannelType.
 */
typedef enum {
    SubchannelType_AUDIO = 0,           /** Signal in subchannel is audio format          */
    SubchannelType_VIDEO = 1,           /** Signal in subchannel is video format          */
    SubchannelType_PACKET = 3,          /** Signal in subchannel is packet format         */
    SubchannelType_ENHANCEPACKET = 4    /** Signal in subchannel is enhance packet format */
} SubchannelType;


/**
 * The defination of ProtectionLevel.
 */
typedef enum {
    ProtectionLevel_NONE = 0x00,    /** The protection level of subchannel is none     */
    ProtectionLevel_PL1 = 0x01,     /** The protection level of subchannel is level 1  */
    ProtectionLevel_PL2 = 0x02,     /** The protection level of subchannel is level 2  */
    ProtectionLevel_PL3 = 0x03,     /** The protection level of subchannel is level 3  */
    ProtectionLevel_PL4 = 0x04,     /** The protection level of subchannel is level 4  */
    ProtectionLevel_PL5 = 0x05,     /** The protection level of subchannel is level 5  */
    ProtectionLevel_PL1A = 0x1A,    /** The protection level of subchannel is level 1A */
    ProtectionLevel_PL2A = 0x2A,    /** The protection level of subchannel is level 2A */
    ProtectionLevel_PL3A = 0x3A,    /** The protection level of subchannel is level 3A */
    ProtectionLevel_PL4A = 0x4A,    /** The protection level of subchannel is level 4A */
    ProtectionLevel_PL1B = 0x1B,    /** The protection level of subchannel is level 1B */
    ProtectionLevel_PL2B = 0x2B,    /** The protection level of subchannel is level 2B */
    ProtectionLevel_PL3B = 0x3B,    /** The protection level of subchannel is level 3B */
    ProtectionLevel_PL4B = 0x4B     /** The protection level of subchannel is level 4B */
} ProtectionLevel;


/**
 * The defination of SubchannelModulation. This structure is used to
 * represent subchannel modulation when device is operate in T-DMB/DAB mode.
 *
 */
typedef struct {
    Byte subchannelId;                  /** The ID of subchannel.                                                 */
    Word subchannelSize;                /** The size of subchannel.                                               */
    Word bitRate;                       /** The bit rate of subchannel.                                           */
    Byte transmissionMode;              /** The transmission mode of subchannel, possible values are: 1, 2, 3, 4. */
    ProtectionLevel protectionLevel;    /** The protection level of subchannel.                                   */
    SubchannelType subchannelType;      /** The type of subchannel                                                */
    Byte conditionalAccess;             /** If a conditional access exist                                         */
    Byte tiiPrimary;                    /** TII primary                                                           */
    Byte tiiCombination;                /** TII combination                                                       */
} SubchannelModulation;

/**
 * The type defination of IpVersion.
 */
typedef enum {
    IpVersion_IPV4 = 0,         /** The IP version if IPv4 */
    IpVersion_IPV6 = 1          /** The IP version if IPv6 */
} IpVersion;

/**
 * The type defination of Ip.
 */
typedef struct {
    IpVersion version;          /** The version of IP. See the defination of IpVersion.                                               */
    Priority priority;          /** The priority of IP. See the defination of Priority.                                               */
    Bool cache;                 /** True: IP datagram will be cached in device's buffer. Fasle: IP datagram will be transfer to host. */
    Byte address[16];           /** The byte array to store IP address.                                                               */
} Ip;


/**
 * The type defination of Platform.
 * Mostly used is in DVB-H standard
 */
typedef struct {
    Dword platformId;           /** The ID of platform.                                    */
    char iso639LanguageCode[3]; /** The ISO 639 language code for platform name.           */
    Byte platformNameLength;    /** The length of platform name.                           */
    char platformName[32];      /** The char array to store platform name.                 */
    Word bandwidth;             /** The operating channel bandwith of this platform.       */
    Dword frequency;            /** The operating channel frequency of this platform.      */
    Byte* information;          /** The extra information about this platform.             */
    Word informationLength;     /** The length of information.                             */
    Bool hasInformation;        /** The flag to indicate if there exist extra information. */
    IpVersion ipVersion;        /** The IP version of this platform.                       */
} Platform;


/**
 * The type defination of Label.
 */
typedef struct {
    Byte charSet;
    Word charFlag;
    Byte string[16];
} Label;


/**
 * The type defination of Ensemble.
 */
typedef struct {
    Word ensembleId;
    Label ensembleLabel;
    Byte totalServices;
} Ensemble;


/**
 * The type defination of Service.
 * Mostly used is in T-DMB standard
 */
typedef struct {
    Byte serviceType;       /** Service Type(P/D): 0x00: Program, 0x80: Data */
    Dword serviceId;
    Dword frequency;
    Label serviceLabel;
    Byte totalComponents;
} Service;


/**
 * The type defination of Service Component.
 */
typedef struct {
    Byte serviceType;           /** Service Type(P/D): 0x00: Program, 0x80: Data         */
    Dword serviceId;            /** Service ID                                           */
    Word componentId;           /** Stream audio/data is subchid, packet mode is SCId    */
    Byte componentIdService;    /** Component ID within Service                          */
    Label componentLabel;       /** The label of component. See the defination of Label. */
    Byte language;              /** Language code                                        */
    Byte primary;               /** Primary/Secondary                                    */
    Byte conditionalAccess;     /** Conditional Access flag                              */
    Byte componentType;         /** Component Type (A/D)                                 */
    Byte transmissionId;        /** Transmission Mechanism ID                            */
} Component;

/**
 * The type defination of Target.
 */
typedef enum {
    SectionType_MPE = 0,        /** Stands for MPE data.                                         */
    SectionType_SIPSI,          /** Stands for SI/PSI table, but don't have to specify table ID. */
    SectionType_TABLE           /** Stands for SI/PSI table.                                     */
} SectionType;


/**
 * The type defination of FrameRow.
 */
typedef enum {
    FrameRow_256 = 0,           /** There should be 256 rows for each column in MPE-FEC frame.  */
    FrameRow_512,               /** There should be 512 rows for each column in MPE-FEC frame.  */
    FrameRow_768,               /** There should be 768 rows for each column in MPE-FEC frame.  */
    FrameRow_1024               /** There should be 1024 rows for each column in MPE-FEC frame. */
} FrameRow;


/**
 * The type defination of Pid.
 *
 * In DVB-T mode, only value is valid. In DVB-H mode,
 * as sectionType = SectionType_SIPSI: only value is valid.
 * as sectionType = SectionType_TABLE: both value and table is valid.
 * as sectionType = SectionType_MPE: except table all other fields is valid.
 */
typedef struct {
    Byte table;                 /** The table ID. Which is used to filter specific SI/PSI table.                                  */
    Byte duration;              /** The maximum burst duration. It can be specify to 0xFF if user don't know the exact value.     */
    FrameRow frameRow;          /** The frame row of MPE-FEC. It means the exact number of rows for each column in MPE-FEC frame. */
    SectionType sectionType;    /** The section type of pid. See the defination of SectionType.                                   */
    Priority priority;          /** The priority of MPE data. Only valid when sectionType is set to SectionType_MPE.              */
    IpVersion version;          /** The IP version of MPE data. Only valid when sectionType is set to SectionType_MPE.            */
    Bool cache;                 /** True: MPE data will be cached in device's buffer. Fasle: MPE will be transfer to host.        */
    Word value;                 /** The 13 bits Packet ID.                                                                        */
} Pid;


/**
 * The type defination of ValueSet.
 */
typedef struct {
    Dword address;      /** The address of target register */
    Byte value;         /** The value of target register   */
} ValueSet;

/**
 * The type defination of tuner group.
 */
typedef struct {
    Word tunerId;       /** The id of tuner */
    Byte groupIndex;    /** The index of group */
} TunerGroup;


/**
 * The type defination of Datetime.
 */
typedef struct {
    Dword mjd;              /** The mjd of datetime           */
    Byte configuration;     /** The configuration of datetime */
    Byte hours;             /** The hours of datetime         */
    Byte minutes;           /** The minutes of datetime       */
    Byte seconds;           /** The seconds of datetime       */
    Word milliseconds;      /** The milli seconds of datetime */
} Datetime;


/**
 * The type defination of Interrupt.
 */
typedef enum {
    Interrupt_NONE      = 0x0000,   /** No interrupt. */
    Interrupt_SIPSI     = 0x0001,
    Interrupt_DVBH      = 0x0002,
    Interrupt_DVBT      = 0x0004,
    Interrupt_PLATFORM  = 0x0008,
    Interrupt_VERSION   = 0x0010,
    Interrupt_FREQUENCY = 0x0020,
    Interrupt_SOFTWARE1 = 0x0040,
    Interrupt_SOFTWARE2 = 0x0080,
    Interrupt_FIC       = 0x0100,
    Interrupt_MSC       = 0x0200,
    Interrupt_MCISI     = 0x0400
} Interrupt;


/**
 * The type defination of Multiplier.
 */
typedef enum {
    Multiplier_1X = 0,
    Multiplier_2X
} Multiplier;


/**
 * The type defination of StreamType.
 */
typedef enum {
    StreamType_NONE = 0,            /** Invalid (Null) StreamType                */
    StreamType_DVBT_DATAGRAM = 3,   /** DVB-T mode, store data in device buffer  */
    StreamType_DVBT_PARALLEL,       /** DVB-T mode, output via paralle interface */
    StreamType_DVBT_SERIAL,         /** DVB-T mode, output via serial interface  */
} StreamType;
/**
 * The type defination of StreamType.
 */
typedef enum {
	INTERFACE_UNUSE = 0, 
    PARALLEL_TS_INPUT=4,		/** input via paralle interface */
    SERIAL_TS_INPUT,			/** input via serial interface  */
} TsInterface;

/**
 * The type defination of StreamType.
 */
typedef enum {
    Architecture_NONE = 0,      /** Inavalid (Null) Architecture.                                    */
    Architecture_DCA,           /** Diversity combine architecture. Only valid when chip number > 1. */
    Architecture_PIP            /** Picture in picture. Only valid when chip number > 1.             */
} Architecture;

/**
 * The type defination of ClockTable.
 */
typedef struct {
    Dword crystalFrequency;     /** The frequency of crystal. */
    Dword adcFrequency;         /** The frequency of ADC.     */
} ClockTable;


/**
 * The type defination of BandTable.
 */
typedef struct {
    Dword minimum;          /** The minimum frequency of this band */
    Dword maximum;          /** The maximum frequency of this band */
} BandTable;


/**
 * The type defination of MeanTable.
 */
typedef struct {
    Dword mean;
    Dword errorCount;
} MeanTable;


/**
 * The type defination of Polarity.
 */
typedef enum {
    Polarity_NORMAL = 0,
    Polarity_INVERSE
} Polarity;

/**
 * The type defination of Processor.
 */
typedef enum {
    Processor_LINK = 0,
    Processor_OFDM = 8
} Processor;



/**
 * The type defination of Constellation.
 */
typedef enum {

    Constellation_QPSK = 0,     /** Signal uses QPSK constellation  */
    Constellation_16QAM,        /** Signal uses 16QAM constellation */
    Constellation_64QAM         /** Signal uses 64QAM constellation */
} Constellation;

/**
 * The defination of ChannelInformation.
 */
typedef struct {
    Dword frequency;                    /** Channel frequency in KHz.                                */
    TransmissionModes transmissionMode; /** Number of carriers used for OFDM signal                  */
    Constellation constellation;        /** Constellation scheme (FFT mode) in use                   */
    Interval interval;                  /** Fraction of symbol length used as guard (Guard Interval) */
    Priority priority;                  /** The priority of stream                                   */
    CodeRate highCodeRate;              /** FEC coding ratio of high-priority stream                 */
    CodeRate lowCodeRate;               /** FEC coding ratio of low-priority stream                  */
    Hierarchy hierarchy;                /** Hierarchy levels of OFDM signal                          */
    Bandwidth bandwidth;
} ChannelModulation;

/**
 * The type defination of Statistic.
 */
typedef struct {
    Word abortCount;
    Dword postVitBitCount;
    Dword postVitErrorCount;
    /** float point */
    Dword softBitCount;
    Dword softErrorCount;
    Dword preVitBitCount;
    Dword preVitErrorCount;
    Byte snr;
} ChannelStatistic;

/**
 * The type defination of Statistic.
 */
typedef struct {
    Bool signalPresented;       /** Signal is presented.                                                                         */
    Bool signalLocked;          /** Signal is locked.                                                                            */
    Byte signalQuality;         /** Signal quality, from 0 (poor) to 100 (good).                                                 */
    Byte signalStrength;        /** Signal strength from 0 (weak) to 100 (strong).                                               */
} Statistic;

typedef struct _TPS{

    Byte highCodeRate;
    Byte lowCodeRate;
    Byte transmissionMode;
    Byte constellation; 
    Byte interval;
    Word cellid;

} TPS, *pTPS;

//------------------------
/**
 * The type defination of Demodulator.
 */
typedef struct {
    Handle userData;
	Handle driver;
} Demodulator;

/**
 * General tuner opening function
 *
 * @param demodulator the handle of demodulator.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
typedef Dword (*OpenTuner) (
    IN  Demodulator*    demodulator
);


/**
 * General tuner closing function
 *
 * @param demodulator the handle of demodulator.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
typedef Dword (*CloseTuner) (
    IN  Demodulator*    demodulator
);


/**
 * General tuner setting function
 *
 * @param demodulator the handle of demodulator.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
typedef Dword (*SetTuner) (
    IN  Demodulator*    demodulator,
    IN  Word            bandwidth,
    IN  Dword           frequency
);
/**
 * The type defination of TunerDescription
 */
typedef struct {
    OpenTuner       openTunerFunc;
    CloseTuner      closeTunerFunc;
    SetTuner        setTunerFunc;
    ValueSet*       tunerScriptTable;
    Word*           tunerScriptSetsTable;
    Byte            tunerAddress;
    Byte            registerAddressLength;
    Dword           ifFrequency;
    Bool            inversion;
    Word            tunerId;
} TunerDescription;


/**
 * The data structure of Ganymede
 */
typedef struct {
    /** Basic structure */
    Handle userData;
	Handle driver;
    Byte* firmwareCodes;
    Segment* firmwareSegments;
    Byte* firmwarePartitions;
    Word* scriptSets;
    ValueSet* scripts;
    Dword crystalFrequency;
    Dword adcFrequency;
    StreamType streamType;
    Word bandwidth;
    Dword frequency;
    Dword fcw;
    Statistic statistic;
    Byte hostInterface;
    Bool booted;
    Bool initialized;
    Byte pidCounter;
    Byte demodAddr;       /** Demodulator I2C Address */
    Bool Clkout_en;       /** Clock output enable */
	ChannelStatistic channelStatistic;
} DefaultDemodulator;
//--------------------------

typedef enum {
	UNUSED = 0,
    GPIOH1 = 0xD8AE,           //r_eagle_reg_top_gpioh1_i
    GPIOH2 = 0xD8B6,           //r_eagle_reg_top_gpioh2_i
	GPIOH3 = 0xD8B2,           //r_eagle_reg_top_gpioh3_i
    GPIOH4 = 0xD8BE,           //r_eagle_reg_top_gpioh4_i
	GPIOH5 = 0xD8BA,           //r_eagle_reg_top_gpioh5_i
    GPIOH6 = 0xD8C6,           //r_eagle_reg_top_gpioh6_i
	GPIOH7 = 0xD8C2,           //r_eagle_reg_top_gpioh7_i
    GPIOH8 = 0xD8CE            //r_eagle_reg_top_gpioh8_i
          
} GPIO;


typedef struct _SystemConfig{
	GPIO	restSlave;
	GPIO	rfEnable;
	GPIO	loClk;
	GPIO	loData;
	GPIO	loLe;
	GPIO	lnaPowerDown;
	GPIO	irDa;
	GPIO	uvFilter;
	GPIO	chSelect3;
	GPIO	chSelect2;
	GPIO	chSelect1;
	GPIO	chSelect0;
	GPIO	powerDownSlave;
	GPIO	uartTxd;
	GPIO	muxSelect;
	GPIO	lnaGain;
	GPIO	intrEnable;
} SystemConfig, *pSystemConfig;
/**
 * The data structure of Eagle
 */
typedef struct {
    /** Basic structure */
    Handle userData;
    Byte busId;
	Byte i2cAddr;
    Byte* firmwareCodes;
    Segment* firmwareSegments;
    Word*  firmwarePartitions;
    Word* scriptSets;
    ValueSet* scripts;
    TsInterface tsInterfaceType;
    Word bandwidth;
    Dword frequency;    
    Bool booted;
	Byte slaveIICAddr;  
	ChannelModulation channelModulation;
	CalibrationInfo calibrationInfo;
	SystemConfig systemConfig;
} Modulator;


extern const Byte Eagle_bitMask[8];
#define REG_MASK(pos, len)                (Eagle_bitMask[len-1] << pos)
#define REG_CLEAR(temp, pos, len)         (temp & (~REG_MASK(pos, len)))
#define REG_CREATE(val, temp, pos, len)   ((val << pos) | (REG_CLEAR(temp, pos, len)))
#define REG_GET(value, pos, len)          ((value & REG_MASK(pos, len)) >> pos)
#define LOWBYTE(w)      ((Byte)((w) & 0xff))
#define HIGHBYTE(w)     ((Byte)((w >> 8) & 0xff))
#define OMEGA_NORMAL                    0x00
#define OMEGA_LNA_Config_1              0x01
#define OMEGA_LNA_Config_2              0x02
#define OMEGA_LNA_Config_3              0x03
#endif
