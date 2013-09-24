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
#define Bus_USB             2
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


/**
 * The type of handle.
 */
typedef void* Handle;

/**
 * The type defination of ValueSet.
 */
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

typedef struct {
    u8 segmentType;           /** 0:Firmware download 1:Rom copy 2:Direct command */
    u32 segmentLength;
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
    bool cache;                 /** True: IP datagram will be cached in device's buffer. Fasle: IP datagram will be transfer to host. */
    u8 address[16];           /** The byte array to store IP address.                                                               */
} Ip;


/**
 * The type defination of Platform.
 * Mostly used is in DVB-H standard
 */
typedef struct {
    u32 platformId;           /** The ID of platform.                                    */
    char iso639LanguageCode[3]; /** The ISO 639 language code for platform name.           */
    u8 platformNameLength;    /** The length of platform name.                           */
    char platformName[32];      /** The char array to store platform name.                 */
    u16 bandwidth;             /** The operating channel bandwith of this platform.       */
    u32 frequency;            /** The operating channel frequency of this platform.      */
    u8* information;          /** The extra information about this platform.             */
    u16 informationLength;     /** The length of information.                             */
    bool hasInformation;        /** The flag to indicate if there exist extra information. */
    IpVersion ipVersion;        /** The IP version of this platform.                       */
} Platform;


/**
 * The type defination of Label.
 */
typedef struct {
    u8 charSet;
    u16 charFlag;
    u8 string[16];
} Label;


/**
 * The type defination of Ensemble.
 */
typedef struct {
    u16 ensembleId;
    Label ensembleLabel;
    u8 totalServices;
} Ensemble;


/**
 * The type defination of Service.
 * Mostly used is in T-DMB standard
 */
typedef struct {
    u8 serviceType;       /** Service Type(P/D): 0x00: Program, 0x80: Data */
    u32 serviceId;
    u32 frequency;
    Label serviceLabel;
    u8 totalComponents;
} Service;


/**
 * The type defination of Service Component.
 */
typedef struct {
    u8 serviceType;           /** Service Type(P/D): 0x00: Program, 0x80: Data         */
    u32 serviceId;            /** Service ID                                           */
    u16 componentId;           /** Stream audio/data is subchid, packet mode is SCId    */
    u8 componentIdService;    /** Component ID within Service                          */
    Label componentLabel;       /** The label of component. See the defination of Label. */
    u8 language;              /** Language code                                        */
    u8 primary;               /** Primary/Secondary                                    */
    u8 conditionalAccess;     /** Conditional Access flag                              */
    u8 componentType;         /** Component Type (A/D)                                 */
    u8 transmissionId;        /** Transmission Mechanism ID                            */
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
    u8 table;                 /** The table ID. Which is used to filter specific SI/PSI table.                                  */
    u8 duration;              /** The maximum burst duration. It can be specify to 0xFF if user don't know the exact value.     */
    FrameRow frameRow;          /** The frame row of MPE-FEC. It means the exact number of rows for each column in MPE-FEC frame. */
    SectionType sectionType;    /** The section type of pid. See the defination of SectionType.                                   */
    Priority priority;          /** The priority of MPE data. Only valid when sectionType is set to SectionType_MPE.              */
    IpVersion version;          /** The IP version of MPE data. Only valid when sectionType is set to SectionType_MPE.            */
    bool cache;                 /** True: MPE data will be cached in device's buffer. Fasle: MPE will be transfer to host.        */
    u16 value;                 /** The 13 bits Packet ID.                                                                        */
} Pid;


/**
 * The type defination of ValueSet.
 */
typedef struct {
    u32 address;      /** The address of target register */
    u8 value;         /** The value of target register   */
} ValueSet;

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
    Architecture_NONE = 0,      /** Inavalid (Null) Architecture.                                    */
    Architecture_DCA,           /** Diversity combine architecture. Only valid when chip number > 1. */
    Architecture_PIP            /** Picture in picture. Only valid when chip number > 1.             */
} Architecture;

/**
 * The type defination of ClockTable.
 */
typedef struct {
    u32 crystalFrequency;     /** The frequency of crystal. */
    u32 adcFrequency;         /** The frequency of ADC.     */
} ClockTable;


/**
 * The type defination of BandTable.
 */
typedef struct {
    u32 minimum;          /** The minimum frequency of this band */
    u32 maximum;          /** The maximum frequency of this band */
} BandTable;


/**
 * The type defination of MeanTable.
 */
typedef struct {
    u32 mean;
    u32 errorCount;
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
    u32 frequency;                    /** Channel frequency in KHz.                                */
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
    u16 abortCount;
    u32 postVitBitCount;
    u32 postVitErrorCount;
    /** float point */
    u32 softBitCount;
    u32 softErrorCount;
    u32 preVitBitCount;
    u32 preVitErrorCount;
    u8 snr;
} ChannelStatistic;

/**
 * The type defination of Statistic.
 */
typedef struct {
    bool signalPresented;       /** Signal is presented.                                                                         */
    bool signalLocked;          /** Signal is locked.                                                                            */
    u8 signalQuality;         /** Signal quality, from 0 (poor) to 100 (good).                                                 */
    u8 signalStrength;        /** Signal strength from 0 (weak) to 100 (strong).                                               */
} Statistic;

typedef struct _TPS{

    u8 highCodeRate;
    u8 lowCodeRate;
    u8 transmissionMode;
    u8 constellation; 
    u8 interval;
    u16 cellid;

} TPS, *pTPS;

//--------------------------

/**
 * The data structure of Eagle
 */
typedef struct {
    /** Basic structure */
    struct usb_device *udev;
	u8 i2cAddr;
    u8* firmwareCodes;
    Segment* firmwareSegments;
    u16*  firmwarePartitions;
    u16* scriptSets;
    ValueSet* scripts;
    u16 bandwidth;
    u32 frequency;    
    bool booted;
	u8 slaveIICAddr;  
	ChannelModulation channelModulation;
	CalibrationInfo calibrationInfo;
} Modulator;


extern const u8 Eagle_bitMask[8];
#define REG_MASK(pos, len)                (Eagle_bitMask[len-1] << pos)
#define REG_CLEAR(temp, pos, len)         (temp & (~REG_MASK(pos, len)))
#define REG_CREATE(val, temp, pos, len)   ((val << pos) | (REG_CLEAR(temp, pos, len)))
#define REG_GET(value, pos, len)          ((value & REG_MASK(pos, len)) >> pos)
#define LOWBYTE(w)      ((u8)((w) & 0xff))
#define HIGHBYTE(w)     ((u8)((w >> 8) & 0xff))
#define OMEGA_NORMAL                    0x00
#define OMEGA_LNA_Config_1              0x01
#define OMEGA_LNA_Config_2              0x02
#define OMEGA_LNA_Config_3              0x03
#endif
