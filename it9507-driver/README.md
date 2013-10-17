it9507-driver
=============

A driver for the ITE it9507 DVB-T modulator as found in the UT-100C USB
stick.


Installation and usage
======================

To install the driver, just cd into the "src" directory and type
"make".  You will require the kernel headers for the currently running
kernel.

You must also copy the firmware file from src/dvb-mod-it9507.fw to
/lib/firmware/

After building the module, you can either load it manually with
"insmod ./usb-it950x.ko" or run "make install" to install it under
/lib/modules/$(uname -r)

A sample program to send a transport stream to the modulator is in the
examples/ directory.  All modulation parameters are currently
hard-coded in sendts.c and the TS is fed via stdin.

The public API is found in include/dvbmod.h

The input TS must be a valid DVB transport stream and padded to a
constant bitrate lower than or equal to the bitrate of the modulation
parameters.  In the case of a transport stream with lower bitrate than
the modulation parameters allow, the it950x will pad with null
packets, but it MUST be a CBR transport stream.


History
=======

This driver is a heavily modified version of v13.06.27.1 (presumably
released on 27th June 2013) of the Linux driver distributed by ITE,
the manufacturer of the it9507.

ITE's Linux driver uses their very generic cross-platform SDK, and as
such does not follow many Linux kernel conventions and does not
integrate with the DVB sub-system.

The modifications stripped out all code not required for the UT-100C
(including all code for the demodulator found on some UT-100 variants)
as well as support for hardware PSI packet insertion and TS PID
filtering.

All low-level API functions (such as reading/writing registers) were
also removed, and the API was changed to use standard DVB
enums/#defines for the modulation parameters (constellations, code
rate etc).

The firmware that runs on the "link" MCU was extracted from the #include 
file and placed in the dvb-mod-it9507.fw file, loaded via the standard 
Linux firmware loading mechanism.

The device node was also renamed from /dev/usb-it950x%d to
/dev/dvbmod%d.

One significant change/bug-fix compared to ITE's driver is in the
implementation of the write() function.  This has been changed to
block when the internal ringbuffer is full (ITE's driver just returned
immediately with a 0 result code) and also to handle short writes
(e.g. user application attempts to write 32KB, only 16KB is available
in the buffer, so the driver just consumes 16KB.  In that case, the
ITE driver would consume nothing).  This makes the userspace "sendts"
application simpler and consume less CPU.

Non-blocking writes (and the use of poll/select) are not yet
implemented, but are on the to-do list.

The full history of the modifications can be found on github at
https://github.com/linuxstb/it9507 within the it9507-driver directory.


Copyright
=========

The original it9507-driver is (C) 2013 ITE Corporation and was
licensed under the GPL.

Modifications (C) 2013 Dave Chapman <dave@dchapman.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

