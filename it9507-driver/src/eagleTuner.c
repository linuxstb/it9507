#include "IT9507.h"


//#define PI 3.141593
#define BYTESHIFT 512

int setIO(const int THETA){
   int reverse;
   int sign;

   int x_in;
   int x_in_reverse;
      
   int mi;
   int xi;
   int yi;
      
   int ph2amp;
//   int ph2amp_scaling;

   if ( ( 0 <= THETA ) && ( THETA < 4096 ) ) // 0 ~ 4096
   {
      reverse = 0;
      sign    = 1;
   }
   else if ( ( 4096 <= THETA ) && ( THETA < 8192 ) ) // 4096 ~ 8192
   {
      reverse = 1;
      sign    = 1;
   }
   else if ( ( 8192 <= THETA ) && ( THETA < 12288 ) ) // 8192 ~ 12288
   {
      reverse = 0;
      sign    = -1;
   }
   else
   {
      reverse = 1;
      sign    = -1;
   }


   //x_in = ( double ) ( ( int ) THETA % 4096 );
   x_in =( ( int ) THETA % 4096 );
   if ( reverse == 0 )
      x_in_reverse = x_in;
   else
      x_in_reverse = 4096 - x_in;
   
   if ( ( 0 <= x_in_reverse ) && ( x_in_reverse < 256 ) ) // section 1 : 0 ~ 256
   {
      mi = 25*16;
      xi = 256 *0;
      yi = 0*16;
   }
   else if ( ( 256 <= x_in_reverse ) && ( x_in_reverse < 512 ) ) // section 2 : 256 ~ 512
   {
      mi = 25*16;
      xi = 256 * 1;
      yi = 400*16;
   }
   else if ( ( 512 <= x_in_reverse ) && ( x_in_reverse < 768 ) ) // section 3 : 512 ~ 768
   {
      mi = 24*16;
      xi = 256 * 2;
      yi = 801*16;
   }
   else if ( ( 768 <= x_in_reverse ) && ( x_in_reverse < 1024 ) ) // section 4 : 768 ~ 1024
   {
      mi = 24*16;
      xi = 256 * 3;
      yi = 1186*16;
   }
   else if ( ( 1024 <= x_in_reverse ) && ( x_in_reverse < 1280 ) ) // section 5 : 1024 ~ 1280
   {
      mi = 23*16;
      xi = 256 * 4;
      yi = 1564*16;
   }
   else if ( ( 1280 <= x_in_reverse ) && ( x_in_reverse < 1536 ) ) // section 6 : 1280 ~ 1536
   {
      mi = 22*16;
      xi = 256 * 5;
      yi = 1927*16;
   }
   else if ( ( 1536 <= x_in_reverse ) && ( x_in_reverse < 1792 ) ) // section 7 : 1536 ~ 1792
   {
      mi = 20*16;
      xi = 256 * 6;
      yi = 2277*16;
   }
   else if ( ( 1792 <= x_in_reverse ) && ( x_in_reverse < 2048 ) ) // section 8 : 1792 ~ 2048
   {
      mi = 19*16;
      xi = 256 * 7;
      yi = 2595*16;
   }
   else if ( ( 2048 <= x_in_reverse ) && ( x_in_reverse < 2304 ) ) // section 9 : 2048 ~ 2304
   {
      mi = 17*16;
      xi = 256 * 8;
      yi = 2894*16;
   }
   else if ( ( 2304 <= x_in_reverse ) && ( x_in_reverse < 2560 ) ) // section 10 : 2304 ~ 2560
   {
      mi = 15*16;
      xi = 256 * 9;
      yi = 3165*16;
   }
   else if ( ( 2560 <= x_in_reverse ) && ( x_in_reverse < 2816 ) ) // section 11 : 2560 ~ 2816
   {
      mi = 13*16;
      xi = 256 * 10;
      yi = 3405*16;
   }
   else if ( ( 2816 <= x_in_reverse ) && ( x_in_reverse < 3072 ) ) // section 12 : 2816 ~ 3072
   {
      mi = 11*16;
      xi = 256 * 11;
      yi = 3609*16;
   }
   else if ( ( 3072 <= x_in_reverse ) && ( x_in_reverse < 3328 ) ) // section 13 : 3072 ~ 3328
   {
      mi = 8*16;
      xi = 256 * 12;
      yi = 3788*16;
   }
   else if ( ( 3328 <= x_in_reverse ) && ( x_in_reverse < 3584 ) ) // section 14 : 3328 ~ 3584
   {
      mi = 6*16;
      xi = 256 * 13;
      yi = 3920*16;
   }    
   else if ( ( 3584 <= x_in_reverse ) && ( x_in_reverse < 3840 ) ) // section 15 : 3584 ~ 3840
   {
      mi = 4*16;
      xi = 256 * 14;
      yi = 4015*16;
   }
   else // section 16 : 3840 ~ 4096
   {
      mi = 1*16;
      xi = 256 * 15;
      yi = 4078*16;
   }

   ph2amp = sign * ( mi * (x_in_reverse - xi)/16 + yi );


   return ph2amp;
}


Dword interpolation(
	Modulator*    modulator,
    int fIn, 
    int *ptrdAmp, 
	int *ptrdPhi
	)
{
  // Using binary search to find the frequency interval in the table
	Word TABLE_NROW = modulator->calibrationInfo.tableGroups;
	Dword   error = ModulatorError_NO_ERROR;
    int idx = TABLE_NROW/2;
    int preIdx = -1;
    int lower = 0;
    int upper = TABLE_NROW;
	int outdAmp;
    int outdPhi;
	int diff;


	int temp1,temp2,temp3,temp4;

    while( ! ( (fIn - (int)(modulator->calibrationInfo.ptrIQtableEx[idx].frequency)) >= 0 && (fIn - (int)(modulator->calibrationInfo.ptrIQtableEx[idx+1].frequency) < 0) ) ){
	  if((fIn - (int)modulator->calibrationInfo.ptrIQtableEx[idx].frequency)==0)
		  break;
      preIdx = idx;
      if(fIn - (int)(modulator->calibrationInfo.ptrIQtableEx[idx+1].frequency) >= 0){
        idx = (preIdx + upper)/2;
        lower = preIdx;
      }else if(fIn - (int)(modulator->calibrationInfo.ptrIQtableEx[idx].frequency) < 0){
        idx = (preIdx + lower)/2;
        upper = preIdx;
      }

	  if(lower == TABLE_NROW - 1)
		  break;
    }

	if((fIn != (int)modulator->calibrationInfo.ptrIQtableEx[idx].frequency) && 
		((modulator->calibrationInfo.ptrIQtableEx[idx+1].frequency - modulator->calibrationInfo.ptrIQtableEx[idx].frequency)>100000)){
		// out of Calibration range
			error = ModulatorError_OUT_OF_CALIBRATION_RANGE;
			goto exit;
	}

    //printf("Found: %f \t %f\n", IQ_table[idx][0], IQ_table[idx+1][0]);
  // Perform linear interpolation

//--------test
	temp1 = modulator->calibrationInfo.ptrIQtableEx[idx].dAmp;
	temp2 = (fIn - (int)modulator->calibrationInfo.ptrIQtableEx[idx].frequency);
	temp3 = (modulator->calibrationInfo.ptrIQtableEx[idx+1].dAmp - modulator->calibrationInfo.ptrIQtableEx[idx].dAmp);
	temp4 = (modulator->calibrationInfo.ptrIQtableEx[idx+1].frequency - modulator->calibrationInfo.ptrIQtableEx[idx].frequency);
	
	outdAmp = temp1 + temp2*temp3/temp4;
	
	//test	

	//outdAmp = modulator->calibrationInfo.ptrIQtableEx[idx].dAmp + ((fIn - (int)modulator->calibrationInfo.ptrIQtableEx[idx].frequency) 
	//	       * (modulator->calibrationInfo.ptrIQtableEx[idx+1].dAmp - modulator->calibrationInfo.ptrIQtableEx[idx].dAmp)) / (modulator->calibrationInfo.ptrIQtableEx[idx+1].frequency - modulator->calibrationInfo.ptrIQtableEx[idx].frequency);
	diff = (modulator->calibrationInfo.ptrIQtableEx[idx+1].dPhi - modulator->calibrationInfo.ptrIQtableEx[idx].dPhi);
	if(diff <= -8192) {
		diff = diff+16384;
	} else if(diff >= 8192) {
		diff = diff-16384;
	}
	//outdPhi = IQ_table[idx][2] + (fIn - IQ_table[idx][0]) * (IQ_table[idx+1][2] - IQ_table[idx][2]) / (IQ_table[idx+1][0] - IQ_table[idx][0]);
			
	//------test---------
	temp1 = modulator->calibrationInfo.ptrIQtableEx[idx].dPhi;
	temp2 = (fIn - (int)modulator->calibrationInfo.ptrIQtableEx[idx].frequency);
	temp3 = diff;
	temp4 = (modulator->calibrationInfo.ptrIQtableEx[idx+1].frequency - modulator->calibrationInfo.ptrIQtableEx[idx].frequency);
	outdPhi = temp1 + temp2*temp3/temp4;
	//----------------

	
	//outdPhi = modulator->calibrationInfo.ptrIQtableEx[idx].dPhi + ((fIn - (int)modulator->calibrationInfo.ptrIQtableEx[idx].frequency) * diff) / (modulator->calibrationInfo.ptrIQtableEx[idx+1].frequency - modulator->calibrationInfo.ptrIQtableEx[idx].frequency);
	
		
	if (outdPhi>=16384) {
		outdPhi = outdPhi%16384;
    }
	while (outdPhi<0) {
		outdPhi = outdPhi+16384;
	}
    (*ptrdAmp) = outdAmp;
    (*ptrdPhi) = outdPhi;
exit:
	return error;
}





Dword EagleTuner_setIQCalibration(
	IN  Modulator*    modulator,
    IN  Dword         frequency	
) {
	Dword   error = ModulatorError_NO_ERROR;
	Dword reg = 0;
	Byte c1_tmp_highbyte;
	Byte c1_tmp_lowbyte;
	Byte c2_tmp_highbyte;
	Byte c2_tmp_lowbyte;
	Byte c3_tmp_highbyte;
	Byte c3_tmp_lowbyte;
	Byte val[6];
	int dAmp = 0;
    int dPhi = 0;
    int* ptrdAmp = &dAmp;
    int* ptrdPhi = &dPhi;
	int alpha;
	int beta;
	int dphi;
	int c1;
	int c2;
	int c3;
	int test;
	int test2;

    error = interpolation(modulator, frequency, ptrdAmp, ptrdPhi);
	if(error == ModulatorError_OUT_OF_CALIBRATION_RANGE){
		//out of range --> set to default
		val[0] = 0x00;
		val[1] = 0x02;
		val[2] = 0x00;
		val[3] = 0x00;
		val[4] = 0x00;
		val[5] = 0x02;
		error = ModulatorError_NO_ERROR;
		goto exit;

	}

	alpha = 512 - dAmp;
	beta = 512 + dAmp;
	dphi = dPhi;

	
   test = setIO(dPhi); 
 
   dPhi = dPhi+4096;
   if (dPhi>=16384) {
	dPhi = dPhi%16384;
   }

   test2 = setIO(dPhi); 

   c1 = (int) (alpha*beta/16*test2/4096/BYTESHIFT);
   c2 = (int) (-1*alpha*beta/16*test/4096/BYTESHIFT);
   c3 = (int) (alpha * alpha/BYTESHIFT);

	if (c1 < 0) {
    c1 = c1+2048;
	}
	if (c2 < 0) {
    c2 = c2+2048;
	}
	if (c3 < 0) {
    c3 = c3+2048;
	}
  
  c1_tmp_highbyte = (unsigned char) (c1>>8);
  c1_tmp_lowbyte = (unsigned char) (c1- c1_tmp_highbyte*256);
  c2_tmp_highbyte = (unsigned char) (c2>>8);
  c2_tmp_lowbyte = (unsigned char) (c2- c2_tmp_highbyte*256);
  c3_tmp_highbyte = (unsigned char) (c3>> 8);
  c3_tmp_lowbyte = (unsigned char) (c3- c3_tmp_highbyte*256);


  reg = 0xF752; //p_eagle_reg_iqik_c1_7_0
  val[0] = (Byte) c1_tmp_lowbyte;
  val[1] = (Byte) c1_tmp_highbyte;
  val[2] = (Byte) c2_tmp_lowbyte;
  val[3] = (Byte) c2_tmp_highbyte;
  val[4] = (Byte) c3_tmp_lowbyte;
  val[5] = (Byte) c3_tmp_highbyte;
exit:
  error = IT9507_writeRegisters(modulator, Processor_OFDM, reg, 6, val);

  return (error);
}


Dword EagleTuner_calIQCalibrationValue(
	IN  Modulator*    modulator,
    IN  Dword         frequency,
	IN  Byte*		  val
) {
	Dword   error = ModulatorError_NO_ERROR;
	
	Byte c1_tmp_highbyte;
	Byte c1_tmp_lowbyte;
	Byte c2_tmp_highbyte;
	Byte c2_tmp_lowbyte;
	Byte c3_tmp_highbyte;
	Byte c3_tmp_lowbyte;
	int dAmp = 0;
    int dPhi = 0;
    int* ptrdAmp = &dAmp;
    int* ptrdPhi = &dPhi;
	int alpha;
	int beta;
	int dphi;
	int c1;
	int c2;
	int c3;
	int test;
	int test2;

    error = interpolation(modulator, frequency, ptrdAmp, ptrdPhi);
	if(error == ModulatorError_OUT_OF_CALIBRATION_RANGE){
		//out of range --> set to default
		val[0] = 0x00;
		val[1] = 0x02;
		val[2] = 0x00;
		val[3] = 0x00;
		val[4] = 0x00;
		val[5] = 0x02;
		error = ModulatorError_NO_ERROR;
		goto exit;

	}

	alpha = 512 - dAmp;
	beta = 512 + dAmp;
	dphi = dPhi;

	
   test = setIO(dPhi); 
 
   dPhi = dPhi+4096;
   if (dPhi>=16384) {
	dPhi = dPhi%16384;
   }
   test2 = setIO(dPhi); 

   c1 = (int) (alpha*beta/16*test2/4096/BYTESHIFT);
   c2 = (int) (-1*alpha*beta/16*test/4096/BYTESHIFT);
   c3 = (int) (alpha * alpha/BYTESHIFT);

	if (c1 < 0) {
    c1 = c1+2048;
	}
	if (c2 < 0) {
    c2 = c2+2048;
	}
	if (c3 < 0) {
    c3 = c3+2048;
	}
  
  c1_tmp_highbyte = (unsigned char) (c1>>8);
  c1_tmp_lowbyte = (unsigned char) (c1- c1_tmp_highbyte*256);
  c2_tmp_highbyte = (unsigned char) (c2>>8);
  c2_tmp_lowbyte = (unsigned char) (c2- c2_tmp_highbyte*256);
  c3_tmp_highbyte = (unsigned char) (c3>> 8);
  c3_tmp_lowbyte = (unsigned char) (c3- c3_tmp_highbyte*256);


  
  val[0] = (Byte) c1_tmp_lowbyte;
  val[1] = (Byte) c1_tmp_highbyte;
  val[2] = (Byte) c2_tmp_lowbyte;
  val[3] = (Byte) c2_tmp_highbyte;
  val[4] = (Byte) c3_tmp_lowbyte;
  val[5] = (Byte) c3_tmp_highbyte;
  //error = Eagle_writeRegisters(modulator, Processor_OFDM, reg, 6, val);
exit:
  return (error);
}


