it9507-driver
=============

A driver for the ITE it9507 DVB modulator as found in the UT-100C USB
stick.

This driver is a heavily modified version of v13.06.27.1 (presumably
released on 27th June 2013) of the Linux driver distributed by ITE,
the manufacturer of the it9507.

The modifications are intended to fix bugs and to integrate it more
within the Linux kernel and specifically the DVB sub-system.

The full history of the modifications can be found on github at
https://github.com/linuxstb/it9507 within the it9507-driver directory.

The modifications also removed all support for the demodulator found
on some versions of the UT-100 - for simplicity this driver is
exclusively for the modulator.


Copyright
=========

it9507-driver is (C) 2013 ITE Corporation
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

