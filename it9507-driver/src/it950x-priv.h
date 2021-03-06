#ifndef IT9507_PRIV_H
#define IT9507_PRIV_H

/* GPIO Registers */
#define GPIO_I(x) ((x))
#define GPIO_O(x) ((x)+1)
#define GPIO_EN(x) ((x)+2)
#define GPIO_ON(x) ((x)+3)
#define GPIOH1 0xd8ae
#define GPIOH2 0xd8b6
#define GPIOH3 0xd8b2
#define GPIOH4 0xd8be
#define GPIOH5 0xd8ba
#define GPIOH6 0xd8c6
#define GPIOH7 0xd8c2
#define GPIOH8 0xd8ce

#define SLAVE_DEMOD_2WIREADDR  0x3A

// biu_reg.h 8-30-2011
// gen_biu Ver 1.0 generated by  
#define    p_eagle_reg_lnk2ofdm_data_63_56	0xF6A7 

#define    p_eagle_reg_fec_sw_rst	0xF702 
#define    p_eagle_reg_tps_gi	0xF725 
#define    p_eagle_reg_iqik_c1_7_0	0xF752 
#define    p_eagle_reg_iqik_c1_10_8	0xF753 
#define    p_eagle_reg_iqik_c2_7_0	0xF754 
#define    p_eagle_reg_iqik_c2_10_8	0xF755 
#define    p_eagle_reg_iqik_c3_7_0	0xF756 
#define    p_eagle_reg_iqik_c3_10_8	0xF757 
#define    p_mp2if_mpeg_ser_mode	0xF985 
#define	mp2if_mpeg_ser_mode_pos 0
#define	mp2if_mpeg_ser_mode_len 1
#define	mp2if_mpeg_ser_mode_lsb 0
#define    p_mp2if_mpeg_par_mode	0xF986 
#define	mp2if_mpeg_par_mode_pos 0
#define	mp2if_mpeg_par_mode_len 1
#define	mp2if_mpeg_par_mode_lsb 0
#define    p_eagle_reg_mpeg_full_speed	0xF990 
#define	eagle_reg_mpeg_full_speed_pos 0
#define	eagle_reg_mpeg_full_speed_len 1
#define	eagle_reg_mpeg_full_speed_lsb 0
#define    p_eagle_reg_mp2_sw_rst	0xF99D 
#define	eagle_reg_mp2_sw_rst_pos 0
#define	eagle_reg_mp2_sw_rst_len 1
#define	eagle_reg_mp2_sw_rst_lsb 0
#define    p_eagle_reg_mp2if2_en	0xF9A3 
#define	eagle_reg_mp2if2_en_pos 0
#define	eagle_reg_mp2if2_en_len 1
#define	eagle_reg_mp2if2_en_lsb 0
#define    p_eagle_reg_mp2if2_sw_rst	0xF9A4 
#define	eagle_reg_mp2if2_sw_rst_pos 0
#define	eagle_reg_mp2if2_sw_rst_len 1
#define	eagle_reg_mp2if2_sw_rst_lsb 0
#define    p_eagle_reg_mp2if2_half_psb	0xF9A5 
#define	eagle_reg_mp2if2_half_psb_pos 0
#define	eagle_reg_mp2if2_half_psb_len 1
#define	eagle_reg_mp2if2_half_psb_lsb 0
#define    p_eagle_reg_mp2if_stop_en	0xF9B5 
#define	eagle_reg_mp2if_stop_en_pos 0
#define	eagle_reg_mp2if_stop_en_len 1
#define	eagle_reg_mp2if_stop_en_lsb 0
#define    p_eagle_reg_sys_buf_overflow	0xF9B9 
#define	eagle_reg_sys_buf_overflow_pos 0
#define	eagle_reg_sys_buf_overflow_len 1
#define	eagle_reg_sys_buf_overflow_lsb 0
#define    p_eagle_reg_tsip_en	0xF9CC 
#define	eagle_reg_tsip_en_pos 0
#define	eagle_reg_tsip_en_len 1
#define	eagle_reg_tsip_en_lsb 0
#define    p_eagle_reg_tsis_en	0xF9CD 
#define	eagle_reg_tsis_en_pos 0
#define	eagle_reg_tsis_en_len 1
#define	eagle_reg_tsis_en_lsb 0
#define    p_eagle_reg_ts_in_src	0xF9D8 
#define eagle_reg_tx_fifo_overflow	0xF9E7
#define    p_eagle_reg_afe_mem0	0xFB24 
#define    p_eagle_reg_afe_mem1	0xFB25 
#define    p_eagle_reg_dyn0_clk	0xFBA8 

// biu_reg.h 6-29-2010
// gen_biu Ver 1.0 generated by weili.su 
#define    p_eagle_reg_top_padmiscdr2	0xD830 
#define    p_eagle_reg_top_padmiscdr4	0xD831 
#define    p_eagle_reg_top_padmiscdr8	0xD832 
#define    p_eagle_reg_top_padmiscdrsr	0xD833 
#define    p_eagle_reg_top_lock3_out	0xD8FD 
#define    p_eagle_reg_top_hostb_mpeg_par_mode	0xD91B 
#define	eagle_reg_top_hostb_mpeg_par_mode_pos 0
#define	eagle_reg_top_hostb_mpeg_par_mode_len 1
#define	eagle_reg_top_hostb_mpeg_par_mode_lsb 0
#define    p_eagle_reg_top_hostb_mpeg_ser_mode	0xD91C 
#define	eagle_reg_top_hostb_mpeg_ser_mode_pos 0
#define	eagle_reg_top_hostb_mpeg_ser_mode_len 1
#define	eagle_reg_top_hostb_mpeg_ser_mode_lsb 0
#define    p_eagle_reg_top_host_reverse	0xD920 

#define    p_eagle_reg_dvbt_en	0xF41A 
#define	eagle_reg_dvbt_en_pos 0
#define	eagle_reg_dvbt_en_len 1
#define	eagle_reg_dvbt_en_lsb 0
#define    p_eagle_reg_dvbt_inten	0xF41F 
#define	eagle_reg_dvbt_inten_pos 2
#define	eagle_reg_dvbt_inten_len 1
#define	eagle_reg_dvbt_inten_lsb 0

// usb_biu_reg.h 8-1-2011
// gen_biu Ver 1.0 generated by cphsu 
#define    p_eagle_reg_ep5_max_pkt	0xDD0D 
#define    p_eagle_reg_ep6_max_pkt	0xDD0E 
#define    p_eagle_reg_ep5_tx_en	0xDD11 
#define	eagle_reg_ep5_tx_en_pos 6
#define	eagle_reg_ep5_tx_en_len 1
#define	eagle_reg_ep5_tx_en_lsb 0
#define    p_eagle_reg_ep6_rx_en	0xDD11 
#define	eagle_reg_ep6_rx_en_pos 7
#define	eagle_reg_ep6_rx_en_len 1
#define	eagle_reg_ep6_rx_en_lsb 0
#define    p_eagle_reg_ep5_tx_nak	0xDD13 
#define	eagle_reg_ep5_tx_nak_pos 6
#define	eagle_reg_ep5_tx_nak_len 1
#define	eagle_reg_ep5_tx_nak_lsb 0
#define    p_eagle_reg_ep6_rx_nak	0xDD13 
#define	eagle_reg_ep6_rx_nak_pos 7
#define	eagle_reg_ep6_rx_nak_len 1
#define	eagle_reg_ep6_rx_nak_lsb 0
#define    p_eagle_reg_ep5_tx_len_7_0	0xDD8A 
#define    p_eagle_reg_ep5_tx_len_15_8	0xDD8B 
#define    p_eagle_reg_ep6_cnt_num_7_0	0xDDA9 


//***************** from afdrv.h *****************//

#define OVA_BASE							0x4C00							// omega variable address base
#define OVA_EEPROM_CFG						(OVA_BASE-620)					// 256bytes
#define OVA_SECOND_DEMOD_I2C_ADDR		(OVA_BASE-5)

#define second_i2c_address                             OVA_SECOND_DEMOD_I2C_ADDR       //0x417F
#define chip_version_7_0					0x1222

#define EEPROM_FLB_OFS  8

#define EEPROM_IRMODE      (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x10)   //00:disabled, 01:HID
#define EEPROM_SELSUSPEND  (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x28)   //Selective Suspend Mode
#define EEPROM_TSMODE      (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x28+1) //0:one ts, 1:dual ts
#define EEPROM_2WIREADDR   (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x28+2) //MPEG2 2WireAddr
#define EEPROM_SUSPEND     (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x28+3) //Suspend Mode
#define EEPROM_IRTYPE      (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x28+4) //0:NEC, 1:RC6
#define EEPROM_SAWBW1      (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x28+5)
#define EEPROM_XTAL1       (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x28+6) //0:28800, 1:20480
#define EEPROM_SPECINV1    (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x28+7)
#define EEPROM_TUNERID     (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x30+4) //
#define EEPROM_IFFREQL     (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x30) 
#define EEPROM_IFFREQH     (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x30+1)   
#define EEPROM_IF1L        (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x30+2)   
#define EEPROM_IF1H        (OVA_EEPROM_CFG+EEPROM_FLB_OFS+0x30+3)
#define EEPROM_SHIFT       (0x10)                 //EEPROM Addr Shift for slave front end

/* From modulatorError.h */
#define ModulatorError_NO_ERROR							0x00000000
#define ModulatorError_INVALID_FFT_MODE					0x00000018
#define ModulatorError_INVALID_CONSTELLATION_MODE		0x00000019
#define ModulatorError_INVALID_BW						0x0000001D
#define ModulatorError_FREQ_OUT_OF_RANGE				0x00000027
#define ModulatorError_WRONG_CHECKSUM					0x00000036
#define ModulatorError_INVALID_DATA_LENGTH				0x00000039
#define ModulatorError_BOOT_FAIL						0x0000003A
#define ModulatorError_PROTOCOL_FORMAT_INVALID			0x00000043
#define ModulatorError_NULL_HANDLE_PTR					0x0000004B
#define ModulatorError_INVALID_FW_TYPE					0x0000004E
#define ModulatorError_OUT_OF_CALIBRATION_RANGE			0x00000051
#define ModulatorError_FIRMWARE_STATUS					0x01000000

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
 * The type defination of Processor.
 */
typedef enum {
    Processor_LINK = 0,
    Processor_OFDM = 8
} Processor;



#endif
