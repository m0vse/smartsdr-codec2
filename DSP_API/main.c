/* *****************************************************************************
 *	main.c															2014 SEP 01
 *
 *  Author: Graham / KE9H
 *  Date created: August 5, 2014
 *
 *  Wrapper program for "Embedded FreeDV" including CODEC2.
 *
 *		Derived, in part from code provided by David Rowe under LGPL in:
 *			freedv_rx.c
 *			freedv_tx.c
 *
 *		Calls and API defined by David Rowe in
 *			freedv_api.c
 *			freedv_api.h
 *
 *  Portions of this file are Copyright (C) 2014 David Rowe
 *
 * *****************************************************************************
 *
 *	Copyright (C) 2014 FlexRadio Systems.
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * *****************************************************************************
 * TODO
 * 		distinguish between rx data and tx data packets?
 * 		discard rx data and tx data packet if wrong type?
 * 		get call for S/N
 *
 *
 *
 **************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "discovery.h"
#include "api-io.h"
#include "api.h"

#include "common.h"

const char* APP_NAME = "FreeDV";            // Name of Application

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 	main()

int main(int argc, char **argv)
{
	struct sockaddr_in radio_address;
	int response_code;
	char *response_string;

	// XXX TODO: Loop around discovery/initiate?

	radio_address.sin_family = AF_INET;

	if (discover_radio(&radio_address) == -1) {
		output("Failed to find radio\n");
		exit(1);
	}
	output("Found radio at %s:%d\n", inet_ntoa(radio_address.sin_addr), ntohs(radio_address.sin_port));

	if (api_io_init(&radio_address) == -1) {
		output("Couldn't connect to radio\n");
		exit(1);
	}

	output("Radio connected\n");
	send_api_command("sub slice all");

	//  XXX These commands should be encapsulated in freedv handling code in
	//  XXX separate file.
	response_code = send_api_command_and_wait("waveform create name=FreeDV-USB mode=FDVU underlying_mode=USB version=2.0.0", &response_string);
	output("Received response code 0x%08x as '%s'\n", response_code, response_string);
	free(response_string);
// 	send_api_command("waveform set FreeDV-USB rx_filter low_cut=180");
// 	send_api_command("waveform set FreeDV-USB rx_filter high_cut=2900");
// 	send_api_command("waveform set FreeDV-USB rx_filter depth=8");
	send_api_command("waveform create name=FreeDV-LSB mode=FDVL underlying_mode=LSB version=2.0.0");
// 	send_api_command("waveform set FreeDV-LSB rx_filter low_cut=180");
// 	send_api_command("waveform set FreeDV-LSB rx_filter high_cut=2900");
// 	send_api_command("waveform set FreeDV-LSB rx_filter depth=8");

	wait_for_api_io();
	output("Complete");

    exit(0);
}



