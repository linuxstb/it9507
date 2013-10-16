/* 

gain_range.c - query the full gain range from the device for all frequencies

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

int main(int argc, char* argv[])
{
  int mod_fd;
  int freq;
  int result;
  struct dvb_modulator_gain_range gain_range;

  /* Open Device - hard-coded to device #1 */
  if ((mod_fd = open("/dev/dvbmod0", O_RDWR)) < 0) {
    fprintf(stderr,"Failed to open device.\n");
    return 1;
  }

  int min = 9999;   /* The minimum value of max_gain */
  int max = -9999;  /* The maximum value of min_gain */
  for (freq=50000;freq<=1500000;freq+=10000) {
    gain_range.frequency_khz = freq;
    result = ioctl(mod_fd, DVBMOD_GET_RF_GAIN_RANGE, &gain_range);
    fprintf(stderr,"%dKHz: %d to %d\n",freq,gain_range.min_gain,gain_range.max_gain);
    if (gain_range.min_gain > max) max = gain_range.min_gain;
    if (gain_range.max_gain < min) min = gain_range.max_gain;
  }

  fprintf(stderr,"Common gain range: %d to %d\n",max,min);

  close(mod_fd);
  return 0;
}
