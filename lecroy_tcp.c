/**************************************************************************
lecroy_tcp.c
Version 1.00
Copyright (C) 2003  Steve D. Sharples

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

***************************************************************************

Libraries for communicating with a LeCroy scope (and perhaps other things)
via TCP/IP. Based on "net_con.cpp" by LeCroy, also using bits of code from
an ariticle in October 2001 issue of "Linux Magazine" see:
http://www.linux-mag.com/2001-10/compile_01.html

The original "net_con.cpp" code was Copyright 1998 LeCroy Corporation, and
written by Ricardo Palacio. This code was passed on to me (Steve
Sharples) by LeCroy so that I could write drivers to communicate with my
new LT584L DSO from my Linux machine. LeCroy corporation have agreed to 
allow the release of the UNIX/Linux code (since it is based on their code) 
under the GNU General Public License in the hope that it will be useful
to others. THEY (LECROY) OBVIOUSLY CANNOT SUPPORT THIS CODE, although if
I were them I'd be starting to think seriously about supporting the Linux/
UNIX architectures. 

See "lecroy_tcp.h" for additional comments and usage

**************************************************************************/



#include "lecroy_tcp.h"

#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
using namespace std;

static BOOL	LECROY_TCP_connected_flag = FALSE;

typedef struct {	/* defines LeCroy VICP protocol (TCP header) */
	unsigned char	bEOI_Flag;
	unsigned char	reserved[3];
	int		iLength;
} LECROY_TCP_HEADER;


int LECROY_TCP_write(int sockfd, char *buf)
{
	LECROY_TCP_HEADER header;
	int result, bytes_more, bytes_xferd,tmp;
	char *idxPtr;

	BOOL eoi_flag	= TRUE;
	int len		= strlen(buf);

	if (LECROY_TCP_connected_flag != TRUE) return -1;

	/* set the header info */
	header.bEOI_Flag = LECROY_DATA_FLAG;
	/* following line retained because the original "write" function
	   contained an extra argument of eoi_flag, which I've set to
	   be always TRUE */
	header.bEOI_Flag |= (eoi_flag)? LECROY_EOI_FLAG:0;
	header.reserved[0] = 1; /* see LeCroy documentation about these */
	header.reserved[1] = 0;
	header.reserved[2] = 0;
	header.iLength = htonl(len);


	/* write the header first */
	tmp=write(sockfd, (char *) &header, sizeof(LECROY_TCP_HEADER));
	if (tmp!=sizeof(LECROY_TCP_HEADER)) {
		printf("Could not write the header successfully, returned: %d\n",tmp);
		return -1;
		}

	bytes_more = len;
	idxPtr = buf;
	bytes_xferd = 0;
	while (1)
	{
                /* then write the rest of the block */
                idxPtr = buf + bytes_xferd;
		result=write (sockfd, (char *) idxPtr, bytes_more);
		if (result<0) {
			printf("Could not write the rest of the block successfully, returned: %d\n",tmp);
			return -1;
			}
                bytes_xferd += result;
                bytes_more -= result;
                if (bytes_more <= 0)
                        break;
        }

        return 0;
}

int LECROY_TCP_read(int sockfd, char *buf, int len, int allowable_time)
{
        LECROY_TCP_HEADER header;
        char tmpStr[512];
        int result, accum, space_left, bytes_more, buf_count;
        char *idxPtr;

	fd_set rfds;
        struct timeval tval;

        tval.tv_sec = allowable_time;
        tval.tv_usec = 0;

	if (LECROY_TCP_connected_flag != TRUE) return -1;

	FD_ZERO(&rfds);
	FD_SET(sockfd, &rfds);

        if (buf==NULL)
                return -1;


        memset(buf, 0, len);
        buf_count = 0;
        space_left = len;

        while (1)
        {
                /* block here until data is received of timeout expires */
                result = select((sockfd+1), &rfds, NULL, NULL, &tval);
                if (result < 0)
                {
			LECROY_TCP_disconnect(sockfd);			
                        printf("Read timeout\n");
                        return -1;
                }
                /* get the header info first */
                accum = 0;
                while (1)
                {
                        memset(&header, 0, sizeof(LECROY_TCP_HEADER));

                        if ((result = read(sockfd, (char *) &header + accum, sizeof(header) - accum)) < 0)
                        {
				LECROY_TCP_disconnect(sockfd);			
                                printf("Unable to receive header info from the server.\n");
                                return -1;
                        }

                        accum += result;
                        if (accum>=sizeof(header))
                                break;
                }

                header.iLength = ntohl(header.iLength);
                if (header.iLength < 1)
                        return 0;

                /* only read to len amount */
                if (header.iLength > space_left)
                {
                        header.iLength = space_left;
                        sprintf(tmpStr, "Read buffer size (%d bytes) is too small\n", len);
                        printf(tmpStr);
                }

                /* read the rest of the block */
                accum = 0;
                while (1)
                {
                        idxPtr = buf + (buf_count + accum);
                        bytes_more = header.iLength - accum;
                        if ((space_left-accum) < LECROY_TCP_MINIMUM_PACKET_SIZE)
                        {
				LECROY_TCP_disconnect(sockfd);			

                                printf("Read buffer needs to be adjusted, must be minimum of %d bytes\n", LECROY_TCP_MINIMUM_PACKET_SIZE);
                                return -1;
                        }

                        if ((result = read(sockfd, (char *) idxPtr, (bytes_more>2048)?2048:bytes_more)) < 0)
                        {
				LECROY_TCP_disconnect(sockfd);			
                                printf("Unable to receive data from the server.\n");
                                return -1;
                        }


                        accum += result;
                        if (accum >= header.iLength)
                                break;
                        if ((accum + buf_count) >= len)
                                break;
                }
                buf_count += accum;
                space_left -= accum;

                if (header.bEOI_Flag & LECROY_EOI_FLAG)
                        break;
                if (space_left <= 0)
                        break;
        }

        return 0;
}



int LECROY_TCP_connect(char *ip_address, int allowable_delay)
{

	int sockfd;
	int tmp;
	const int disable=1;

	struct sockaddr_in addr;

/* bomb out if we've already connected. At time of coding, many LeCroy
   instruments (including my own scope) can support only one client at
   a time */
	if (LECROY_TCP_connected_flag==TRUE){
		printf("ERROR! lecroy_tcp: LECROY_TCP_connected_flag==TRUE!\n");
		return -2;
		}
	sockfd = socket (PF_INET, SOCK_STREAM, 0);

	/* Clear out the address struct. */
	/* not sure this is entirely necessary.... */
	bzero (&addr, sizeof (addr));

	/* Initialize the values of the address struct including
	   port number and IP address. */
	addr.sin_family = AF_INET;
	addr.sin_port = htons (LECROY_SERVER_PORT);
	addr.sin_addr.s_addr = inet_addr(ip_address);


	/* The following two lines are necessary so that the program doesn't
	   hang around waiting forever if the scope is turned off. You'd
	   think there would be something you could set in the socket options,
	   wouldn't you? Well, there is, but to quote the socket(7) man pages:

------------------------------------------------------------------------------
SOCKET OPTIONS
       SO_RCVTIMEO and SO_SNDTIMEO
              Specify  the  sending  or  receiving timeouts until reporting an
              error.  They are fixed to a protocol specific setting  in  Linux
              and  cannot  be read or written. Their functionality can be emu-
              lated using alarm(2) or setitimer(2).
------------------------------------------------------------------------------

	   So, a timer is set with alarm(), and SIGALRM is caught by the
	   "LECROY_TCP_bored_now(int sig)" function. Upon successful 
           connection, however, the alarm is cancelled with "alarm(0)" */

	signal(SIGALRM,LECROY_TCP_bored_now);
	alarm(allowable_delay);

	/* Connect to the server. */
	tmp=connect(sockfd, (struct sockaddr *)&addr, sizeof (addr));
	alarm(0);
	if (tmp<0){
		printf("ERROR! lecroy_tcp: could not connect to the scope!\n");
		return -1;
		}
	LECROY_TCP_connected_flag=TRUE;

	/* Ok, now we're all connected */

	/* THE FOLLOWING LINE IS ARCHITECTURE DEPENDENT 
	   and is not at all necessary to make a connection and may be
	   safely removed */

	LECROY_TCP_write(sockfd,"CORD LO\n");

	/* It sets the 16-bit word transfer byte order to LSB,MSB (ie native
	   Intel format). This makes things easier to interpret, so it's a
	   sufficiently-useful thing to add in the connect function. Can be 
	   commented out obviously. For motorola-based systems, use:
	   LECROY_TCP_write(sockfd,"CORD HI\n");
	   Obviously there'll be some way of working out at compile-time, but
	   I don't know how! --- sds, 6/5/03 */

	return sockfd;
}

void LECROY_TCP_bored_now(int sig) {
	printf("Well, I got bored waiting to connect, so I'm going to quit. Bye!\n");
	exit(1);
	}

int LECROY_TCP_disconnect(int sockfd) {
	if (LECROY_TCP_connected_flag !=TRUE) return -1;
	close (sockfd);
	LECROY_TCP_connected_flag=FALSE;
	return 0;
	}

