/**************************************************************************
lecroy_tcp.h
Version 1.00
Copyright (C) 2003  Steve D. Sharples
Based on "net_con.h" by LeCroy
See "lecroy_tcp.c" for additional comments and usage

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

The author's email address is steve.sharples@nottingham.ac.uk
**************************************************************************/



/* stick your own I.P. address here. Of course, you don't have to use
   this constant, it's just handy */
#define OUR_SCOPE_IP            "172.25.1.2"

#define LECROY_SERVER_PORT      1861	/* as defined by LeCroy */

#define CMD_BUF_LEN             8192
#define MAX_TCP_CONNECT		5	/* time in secs to get a connection */
#define MAX_TCP_READ		3	/* time in secs to wait for the DSO
					   to respond to a read request */
#define BOOL                    int
#define TRUE                    1
#define FALSE                   0

void LECROY_TCP_bored_now(int);		/* catch signal routine */
int LECROY_TCP_write(int, char *);
int LECROY_TCP_read(int, char *, int, int);
int LECROY_TCP_connect(char *, int);
int LECROY_TCP_disconnect(int);

#define LECROY_EOI_FLAG				0x01
#define LECROY_SRQ_FLAG				0x08
#define LECROY_CLEAR_FLAG			0x10
#define LECROY_LOCKOUT_FLAG			0x20
#define LECROY_REMOTE_FLAG			0x40
#define LECROY_DATA_FLAG			0x80

#define LECROY_READ_TIME_OUT			10

#define LECROY_TCP_MINIMUM_PACKET_SIZE		64
