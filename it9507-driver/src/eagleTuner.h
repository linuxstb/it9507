#ifndef _EAGLETUNER_H_
#define _EAGLETUNER_H_


u32 EagleTuner_setIQCalibration(
	IN  Modulator*    modulator,
    IN  u32         frequency	
);

u32 EagleTuner_calIQCalibrationValue(
	IN  Modulator*    modulator,
    IN  u32         frequency,
	IN  u8*		  val
);


#endif
