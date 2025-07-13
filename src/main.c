/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/05 21:46:47 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/13 22:39:53 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "options.h"
#include <stdio.h>

int main(int argc, char **argv) {
	t_options options;
	memset(&options, 0, sizeof(t_options));
	parse_options(&options, argc, argv);

	printf("Prueba: %d\n", options.count);
	return (0);
}


// BONUS: -f -l -n -w -W -p -r -s -T --ttl --ip-timestamp flags...

// Usage: ping [OPTION...] HOST ...
// Send ICMP ECHO_REQUEST packets to network hosts.

//  Options controlling ICMP request types:
//       --address              send ICMP_ADDRESS packets (root only)
//       --echo                 send ICMP_ECHO packets (default)
//       --mask                 same as --address
//       --timestamp            send ICMP_TIMESTAMP packets
//   -t, --type=TYPE            send TYPE packets

//  Options valid for all request types:

//   -c, --count=NUMBER         stop after sending NUMBER packets
//   -d, --debug                set the SO_DEBUG option
//   -i, --interval=NUMBER      wait NUMBER seconds between sending each packet
//   -n, --numeric              do not resolve host addresses
//   -r, --ignore-routing       send directly to a host on an attached network
//       --ttl=N                specify N as time-to-live
//   -T, --tos=NUM              set type of service (TOS) to NUM
//   -v, --verbose              verbose output
//   -w, --timeout=N            stop after N seconds
//   -W, --linger=N             number of seconds to wait for response

//  Options valid for --echo requests:

//   -f, --flood                flood ping (root only)
//       --ip-timestamp=FLAG    IP timestamp of type FLAG, which is one of
//                              "tsonly" and "tsaddr"
//   -l, --preload=NUMBER       send NUMBER packets as fast as possible before
//                              falling into normal mode of behavior (root only)
//   -p, --pattern=PATTERN      fill ICMP packet with given pattern (hex)
//   -q, --quiet                quiet output
//   -R, --route                record route
//   -s, --size=NUMBER          send NUMBER data octets

//   -?, --help                 give this help list
//       --usage                give a short usage message
//   -V, --version              print program version

// Mandatory or optional arguments to long options are also mandatory or optional
// for any corresponding short options.

// Options marked with (root only) are available only to superuser.


// Usage: ping [-dnrvfqR?V] [-t TYPE] [-c NUMBER] [-i NUMBER] [-T NUM] [-w N]
//             [-W N] [-l NUMBER] [-p PATTERN] [-s NUMBER] [--address] [--echo]
//             [--mask] [--timestamp] [--type=TYPE] [--count=NUMBER] [--debug]
//             [--interval=NUMBER] [--numeric] [--ignore-routing] [--ttl=N]
//             [--tos=NUM] [--verbose] [--timeout=N] [--linger=N] [--flood]
//             [--ip-timestamp=FLAG] [--preload=NUMBER] [--pattern=PATTERN]
//             [--quiet] [--route] [--size=NUMBER] [--help] [--usage] [--version]
//             HOST ...
