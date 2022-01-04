/**************************************************************************
lecroy_example.c
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

Example program that demonstrates communication via TCP/IP with an ethernet-
enabled LeCroy DSO. Based on the main() function of "net_con.cpp" written
by LeCroy.

See "lecroy_tcp.c" and "lecroy_tcp.h" for additional comments and usage

**************************************************************************/


#include "lecroy_tcp.h"
#include <stdio.h>
#include <string.h>

#define	MAX_ST	512	/* maximum message string. no picked from thin air */

int main ()
{
	char ip_address[50];
	int sockfd;
	char outbuf[MAX_ST];
	char inbuf[MAX_ST];
	float fvalue;
	int ivalue;


	/* OUR_SCOPE_IP is #define'd in lecroy_tcp.h, so is MAX_TCP_CONNECT */
	sprintf(ip_address, OUR_SCOPE_IP);

	fprintf(stderr, "\nlecroy_example: about to connect to DSO...\n");

	/* how to "set up the scope for ethernet communication" */
	sockfd=LECROY_TCP_connect(ip_address, MAX_TCP_CONNECT);
	if (sockfd<0) {
		fprintf(stderr, "\nCould not connect to the scope on IP: %s\n",ip_address);
		return 1;
		}

	fprintf(stderr, "....connected.\n");

	/* example of writing to the scope) */
	strcpy(outbuf,"C1:PAVA? FREQ\n");
	LECROY_TCP_write(sockfd, outbuf);
	fprintf(stderr, "Request to scope:\n%s\n", outbuf);

	/* example of getting the raw data back from the scope */
	LECROY_TCP_read(sockfd, inbuf, sizeof(inbuf), MAX_TCP_READ);
	fprintf(stderr, "Scope returned:\n%s\n", inbuf);
	{
		char *id = strchr(inbuf, ',');
		*strchr(id, ',') = '\0';
		puts(id);
	}

	LECROY_TCP_disconnect(sockfd);
	fprintf(stderr, "Successfully disconnected... bye!\n");
	return 0;
}
