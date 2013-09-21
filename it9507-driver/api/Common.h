#pragma once

typedef unsigned char		BYTE;						/** At least 1 Byte				*/
typedef unsigned short		WORD;						/** At least 2 Bytes				*/
typedef signed short		_s16;
typedef unsigned long		DWORD;						/** At least 4 Bytes				*/
typedef void *				HANDLE;						/** Pointer to memory location	*/

#define TUNER_REGS_NUM		104
#define INITCTRL_NUM		40
#ifdef _MXL_PRODUCTION
#define CHCTRL_NUM			39
#else
#define CHCTRL_NUM			36
#endif

#define MXLCTRL_NUM			189

#define MASTER_CONTROL_ADDR		9

/** Enumeration of AGC Mode */
typedef enum
{
	MXL_DUAL_AGC = 0 ,
	MXL_SINGLE_AGC
} AGC_Mode ;

/**
 * Enumeration of Master Control Register State
 */
typedef enum
{
	MC_LOAD_START = 1 ,
	MC_POWER_DOWN ,
	MC_SYNTH_RESET ,
	MC_SEQ_OFF
} Master_Control_State ;

/**
 * Enumeration of MXL5005 Tuner Mode
 */
typedef enum
{
	MXL_ANALOG_MODE = 0 ,
	MXL_DIGITAL_MODE

} Tuner_Mode ;

/**
 * Enumeration of MXL5005 Tuner IF Mode
 */
typedef enum
{
	MXL_ZERO_IF = 0 ,
	MXL_LOW_IF

} Tuner_IF_Mode ;

/**
 * Enumeration of MXL5005 Tuner Clock Out Mode
 */
typedef enum
{
	MXL_CLOCK_OUT_DISABLE = 0 ,
	MXL_CLOCK_OUT_ENABLE
} Tuner_Clock_Out ;

/**
 * Enumeration of MXL5005 Tuner Div Out Mode
 */
typedef enum
{
	MXL_DIV_OUT_1 = 0 ,
	MXL_DIV_OUT_4

} Tuner_Div_Out ;

/**
 * Enumeration of MXL5005 Tuner Pull-up Cap Select Mode
 */
typedef enum
{
	MXL_CAP_SEL_DISABLE = 0 ,
	MXL_CAP_SEL_ENABLE

} Tuner_Cap_Select ;

/**
 * Enumeration of MXL5005 Tuner RSSI Mode
 */
typedef enum
{
	MXL_RSSI_DISABLE = 0 ,
	MXL_RSSI_ENABLE

} Tuner_RSSI ;

/**
 * Enumeration of MXL5005 Tuner Modulation Type
 */
typedef enum
{
	MXL_DEFAULT_MODULATION = 0 ,
	MXL_DVBT,
	MXL_ATSC,
	MXL_QAM,
	MXL_ANALOG_CABLE,
	MXL_ANALOG_OTA

} Tuner_Modu_Type ;

/**
 * Enumeration of MXL5005 Tuner Tracking Filter Type
 */
typedef enum
{
	MXL_TF_DEFAULT = 0 ,
	MXL_TF_OFF,
	MXL_TF_C,
	MXL_TF_C_H,
	MXL_TF_D,
	MXL_TF_D_L,
	MXL_TF_E, 
	MXL_TF_F,
	MXL_TF_E_2,
	MXL_TF_E_NA,
	MXL_TF_G
	

} Tuner_TF_Type ;

/**
 * MXL5005 Tuner Register Struct
 */
typedef struct _TunerReg_struct
{
	WORD 	Reg_Num ;							// Tuner Register Address
	WORD	Reg_Val ;							// Current sofware programmed value waiting to be writen
} TunerReg_struct ;

/** 
 * MXL5005 Tuner Control Struct
 */
typedef struct _TunerControl_struct {
	WORD	Ctrl_Num ;							// Control Number
	WORD	size ;								// Number of bits to represent Value
	WORD 	addr[25] ;							// Array of Tuner Register Address for each bit position
	WORD 	bit[25] ;							// Array of bit position in Register Address for each bit position
	WORD 	val[25] ;							// Binary representation of Value
} TunerControl_struct ;

/**
 * MXL5005 Tuner Struct
 */
typedef struct _Tuner_struct
{
	BYTE			Mode ;				/** 0: Analog Mode ; 1: Digital Mode */
	BYTE			IF_Mode ;			/** for Analog Mode, 0: zero IF; 1: low IF */
	DWORD			Chan_Bandwidth ;	/** filter  channel bandwidth (6, 7, 8) */
	DWORD			IF_OUT ;			/** Desired IF Out Frequency */
	WORD			IF_OUT_LOAD ;		/** IF Out Load Resistor (200/300 Ohms) */
	DWORD			RF_IN ;				/** RF Input Frequency */
	DWORD			Fxtal ;				/** XTAL Frequency */
	BYTE			AGC_Mode ;			/** AGC Mode 0: Dual AGC; 1: Single AGC */
	WORD			TOP ;				/** Value: take over point */
	BYTE			CLOCK_OUT ;			/** 0: turn off clock out; 1: turn on clock out */
	BYTE			DIV_OUT ;			/** 4MHz or 16MHz */
	BYTE			CAPSELECT ;			/** 0: disable On-Chip pulling cap; 1: enable */
	BYTE			EN_RSSI ;			/** 0: disable RSSI; 1: enable RSSI */
	BYTE			Mod_Type ;			/** Modulation Type; */
										/** 0 - Default;	1 - DVB-T; 2 - ATSC; 3 - QAM; 4 - Analog Cable */
	BYTE			TF_Type ;			// Tracking Filter Type
										// 0 - Default; 1 - Off; 2 - Type C; 3 - Type C-H

	/** Calculated Settings */
	DWORD			RF_LO ;				/** Synth RF LO Frequency */
	DWORD			IF_LO ;				/** Synth IF LO Frequency */
	DWORD			TG_LO ;				/** Synth TG_LO Frequency */

	/** Pointers to ControlName Arrays */
	WORD					Init_Ctrl_Num ;					/** Number of INIT Control Names */
	TunerControl_struct		Init_Ctrl[INITCTRL_NUM] ;		/** INIT Control Names Array Pointer */
	WORD					CH_Ctrl_Num ;					/** Number of CH Control Names */
	TunerControl_struct		CH_Ctrl[CHCTRL_NUM] ;			/** CH Control Name Array Pointer */
	WORD					MXL_Ctrl_Num ;					/** Number of MXL Control Names */
	TunerControl_struct		MXL_Ctrl[MXLCTRL_NUM] ;			/** MXL Control Name Array Pointer */

	/** Pointer to Tuner Register Array */
	WORD					TunerRegs_Num ;		/** Number of Tuner Registers */
	TunerReg_struct			TunerRegs[TUNER_REGS_NUM] ;			/** Tuner Register Array Pointer */
} Tuner_struct ;



typedef enum
{
	/**
	 * Initialization Control Names
	 */
	DN_IQTN_AMP_CUT = 1 ,       /** 1  */
	BB_MODE ,                   /** 2  */
	BB_BUF ,                    /** 3  */
	BB_BUF_OA ,	                /** 4  */
	BB_ALPF_BANDSELECT ,        /** 5  */
	BB_IQSWAP ,                 /** 6  */
	BB_DLPF_BANDSEL ,           /** 7  */
	RFSYN_CHP_GAIN ,            /** 8  */
	RFSYN_EN_CHP_HIGAIN ,       /** 9  */
	AGC_IF ,                    /** 10 */
	AGC_RF ,                    /** 11 */
	IF_DIVVAL ,                 /** 12 */
	IF_VCO_BIAS ,               /** 13 */
	CHCAL_INT_MOD_IF ,          /** 14 */
	CHCAL_FRAC_MOD_IF ,         /** 15 */
	DRV_RES_SEL ,               /** 16 */
	I_DRIVER ,                  /** 17 */
	EN_AAF ,                    /** 18 */
	EN_3P ,                     /** 19 */
	EN_AUX_3P ,                 /** 20 */
	SEL_AAF_BAND ,              /** 21 */
	SEQ_ENCLK16_CLK_OUT ,       /** 22 */
	SEQ_SEL4_16B ,              /** 23 */
	XTAL_CAPSELECT ,            /** 24 */
	IF_SEL_DBL ,                /** 25 */
	RFSYN_R_DIV ,               /** 26 */
	SEQ_EXTSYNTHCALIF ,         /** 27 */
	SEQ_EXTDCCAL ,              /** 28 */
	AGC_EN_RSSI ,               /** 29 */
	RFA_ENCLKRFAGC ,            /** 30 */
	RFA_RSSI_REFH ,             /** 31 */
	RFA_RSSI_REF ,              /** 32 */
	RFA_RSSI_REFL ,             /** 33 */
	RFA_FLR ,                   /** 34 */
	RFA_CEIL ,                  /** 35 */
	SEQ_EXTIQFSMPULSE ,         /** 36 */
	OVERRIDE_1 ,                /** 37 */
	BB_INITSTATE_DLPF_TUNE,     /** 38 */
	TG_R_DIV,					/** 39 */
	EN_CHP_LIN_B ,				/** 40 */

	/**
	 * Channel Change Control Names
	 */
	DN_POLY = 51 ,              /** 51 */
	DN_RFGAIN ,                 /** 52 */
	DN_CAP_RFLPF ,              /** 53 */
	DN_EN_VHFUHFBAR ,           /** 54 */
	DN_GAIN_ADJUST ,            /** 55 */
	DN_IQTNBUF_AMP ,            /** 56 */
	DN_IQTNGNBFBIAS_BST ,       /** 57 */
	RFSYN_EN_OUTMUX ,           /** 58 */
	RFSYN_SEL_VCO_OUT ,         /** 59 */
	RFSYN_SEL_VCO_HI ,          /** 60 */
	RFSYN_SEL_DIVM ,            /** 61 */
	RFSYN_RF_DIV_BIAS ,         /** 62 */
	DN_SEL_FREQ ,               /** 63 */
	RFSYN_VCO_BIAS ,            /** 64 */
	CHCAL_INT_MOD_RF ,          /** 65 */
	CHCAL_FRAC_MOD_RF ,         /** 66 */
	RFSYN_LPF_R ,               /** 67 */
	CHCAL_EN_INT_RF ,           /** 68 */
	TG_LO_DIVVAL ,              /** 69 */
	TG_LO_SELVAL ,              /** 70 */
	TG_DIV_VAL ,                /** 71 */
	TG_VCO_BIAS ,               /** 72 */
	SEQ_EXTPOWERUP ,            /** 73 */
	OVERRIDE_2 ,                /** 74 */
	OVERRIDE_3 ,                /** 75 */
	OVERRIDE_4 ,                /** 76 */
	SEQ_FSM_PULSE ,             /** 77 */
	GPIO_4B,                    /** 78 */
	GPIO_3B,                    /** 79 */
	GPIO_4,                     /** 80 */
	GPIO_3,                     /** 81 */
	GPIO_1B,                    /** 82 */
	DAC_A_ENABLE ,              /** 83 */
	DAC_B_ENABLE ,              /** 84 */
	DAC_DIN_A ,	                /** 85 */
	DAC_DIN_B ,                 /** 86 */
#ifdef _MXL_PRODUCTION
	RFSYN_EN_DIV,				/** 87 */
	RFSYN_DIVM,					/** 88 */
	DN_BYPASS_AGC_I2C			/** 89 */
#endif

} MXL5005_ControlName ;
