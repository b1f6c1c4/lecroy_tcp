------------------------------------------------------------
TCP/IP control of LeCroy DSO by Steve D. Sharples, May 2003.
------------------------------------------------------------

The programs are free software; you can redistribute them and/or
modify them under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

These programs are distributed in the hope that they will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details, in the file
GNU_General_Public_License.txt

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, 
USA.

The author's email address is steve.sharples@nottingham.ac.uk

THE CODE
--------

The original "net_con.cpp" code, upon which much of the code herein is
based, was Copyright 1998 LeCroy Corporation, and written by Ricardo
Palacio. This code was passed on to me (Steve Sharples) by LeCroy so that
I could write drivers to communicate with my new LT584L DSO from my Linux
machine. LeCroy corporation have agreed to allow the release of the
UNIX/Linux code (since it is based on their code) under the GNU General
Public License in the hope that it will be useful to others. THEY (LECROY)
OBVIOUSLY CANNOT SUPPORT THIS CODE, although if I were them I'd be
starting to think seriously about supporting the Linux/UNIX
architectures.

There are three programs:

lecroy_example.c - simple program that demonstrates communication
lecroy_tcp.c	 - the main library
lecroy_tcp.h	 - the header

Please read the comments in the code for usage instructions.

COMPILING
---------

Hopefully, you should just be able to type "make" and the code will 
compile.

I haven't written a "./configure" script to check architecture 
compatibility etc, and I'm not sure exactly how strictly ANSI C it's 
encoded. I'm afraid I don't know very much about these things! All I know 
is that it compiles ok on all my Linux boxes, which are RedHat Linux 7.3, 
8.0 and 9, including gcc versions 2.96 20000731, 3.2 20020903, and
3.2.2 20030222. 

You'll probably need to edit lecroy_tcp.h to put in your own scope IP
address.

RUNNING
-------

Just run ./lecroy_example

See the source code to see what it does.


FINALLY
-------

I hope this code is of use to people. If it is, then email me. If you have 
problems, then I'll try and answer your questions, but be aware that I've 
little knowledge of other Linux/UNIX architectures other than my own at 
the moment.

I'll set up a website with some other info and utilities etc, which may
have a later version of this code in it. It will probably be at:
http://osam.eee.nottingham.ac.uk/lecroy_tcp/

Cheers!

   Steve D. Sharples. May 2003.
   steve.sharples@nottingham.ac.uk
