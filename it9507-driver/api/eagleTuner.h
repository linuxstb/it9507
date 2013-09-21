#ifndef _EAGLETUNER_H_
#define _EAGLETUNER_H_


Dword EagleTuner_setIQCalibration(
	IN  Modulator*    modulator,
    IN  Dword         frequency	
);

Dword EagleTuner_calIQCalibrationValue(
	IN  Modulator*    modulator,
    IN  Dword         frequency,
	IN  Byte*		  val
);


#endif
