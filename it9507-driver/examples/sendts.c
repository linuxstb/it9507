/* 

sendts.c - write a TS to the UT100-C DVB-T modulator

(C) Dave Chapman <dave@dchapman.com> 2013

Copyright notice:

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
    
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <linux/types.h>

#include "../include/dvbmod.h"

/* Function based on code in the tsrfsend.c application by Avalpa
   Digital Engineering srl */
static int calc_channel_capacity(struct dvb_modulator_parameters *params)
{
  uint64_t channel_capacity;
  int temp;

  switch (params->constellation) {
    case QPSK:
      temp = 0;
      break;
    case QAM_16:
      temp = 1;
      break;
    case QAM_64:
      temp = 2;
      break;
    default:
      fprintf(stderr,"Invalid constellation, aborting\n");
      exit(1);
  }
  channel_capacity = params->bandwidth_hz * 1000;
  channel_capacity = channel_capacity * ( temp * 2 + 2);

  switch (params->guard_interval) {
    case GUARD_INTERVAL_1_32:
      channel_capacity = channel_capacity*32/33;
      break;
    case GUARD_INTERVAL_1_16:
      channel_capacity = channel_capacity*16/17;
      break;
    case GUARD_INTERVAL_1_8:
      channel_capacity = channel_capacity*8/9;
      break;
    case GUARD_INTERVAL_1_4:
      channel_capacity = channel_capacity*4/5;
      break;
    default:
      fprintf(stderr,"Invalid guard interval, aborting\n");
      exit(1);
  }
  switch (params->code_rate_HP) {
    case FEC_1_2:
      channel_capacity = channel_capacity*1/2;
      break;
    case FEC_2_3:
      channel_capacity = channel_capacity*2/3;
      break;
    case FEC_3_4:
      channel_capacity = channel_capacity*3/4;
      break;
    case FEC_5_6:
      channel_capacity = channel_capacity*5/6;
      break;
    case FEC_7_8:
      channel_capacity = channel_capacity*7/8;
      break;
    default:
      fprintf(stderr,"Invalid coderate, aborting\n");
      exit(1);
  }

  return channel_capacity/544*423;
}


int main(int argc, char* argv[])
{
  int mod_fd;
  int result;
  int channel_capacity;
  int gain;
  struct dvb_modulator_parameters params;

  /* Open Device - hard-coded to device #1 */
  if ((mod_fd = open("/dev/dvbmod0", O_RDWR)) < 0) {
    fprintf(stderr,"Failed to open device.\n");
    return 1;
  }

  /* Set modulation parameters */
  params.frequency_khz = 794000;
  params.bandwidth_hz = 8000;
  params.transmission_mode = TRANSMISSION_MODE_8K;
  params.constellation = QAM_64;
  params.guard_interval = GUARD_INTERVAL_1_4;
  params.code_rate_HP = FEC_2_3;
  params.cell_id = 0;
  result = ioctl(mod_fd, DVBMOD_SET_PARAMETERS, &params);

  struct dvb_modulator_gain_range gain_range;
  gain_range.frequency_khz = 794000;
  result = ioctl(mod_fd, DVBMOD_GET_RF_GAIN_RANGE, &gain_range);
  fprintf(stderr,"Gain range: %d to %d\n",gain_range.min_gain,gain_range.max_gain);

  gain = -10;
  result = ioctl(mod_fd, DVBMOD_SET_RF_GAIN, &gain);
  fprintf(stderr,"Gain set to %d\n",gain);

  /* Calculate and display the channel capacity based on the modulation/channel parameters */
  channel_capacity = calc_channel_capacity(&params);
  fprintf(stderr,"Channel capacity = %dbps\n",channel_capacity);

  /* The main transfer loop */
  unsigned char buf[188*1000];
  int n;
  unsigned long long bytes_sent = 0;
  while(1) {
    n = read(0,buf,sizeof(buf));
    if (n == 0) { break; }

    int to_write = n;
    int bytes_done = 0;
    while (bytes_done < to_write) {
      n = write(mod_fd,buf+bytes_done,to_write-bytes_done);
      if (n == 0) {
        /* This shouldn't happen */
        fprintf(stderr,"Zero write\n");
        usleep(500);
      } else if (n <= 0) {
	fprintf(stderr,"Write error %d: ",n);
        perror("Write error: ");
      } else {
        //if (n < sizeof(buf)) { fprintf(stderr,"Short write - %d bytes\n",n); }
        //fprintf(stderr,"Wrote %d\n",n);
        bytes_sent += n;
        bytes_done += n;
        //fprintf(stderr,"Bytes sent: %llu\r",bytes_sent);
      }
    }
  }  

  close(mod_fd);
  return 0;
}
