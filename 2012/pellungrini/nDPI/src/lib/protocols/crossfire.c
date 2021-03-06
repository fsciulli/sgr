/*
 * crossfire.c
 * Copyright (C) 2009-2011 by ipoque GmbH
 * 
 * This file is part of OpenDPI, an open source deep packet inspection
 * library based on the PACE technology by ipoque GmbH
 * 
 * OpenDPI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * OpenDPI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with OpenDPI.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */



/* include files */
#include "ndpi_protocols.h"
#ifdef NDPI_PROTOCOL_CROSSFIRE


static void ndpi_int_crossfire_add_connection(struct ndpi_detection_module_struct *ndpi_struct, 
					      struct ndpi_flow_struct *flow, ndpi_protocol_type_t protocol_type)
{

	ndpi_int_add_connection(ndpi_struct, flow, NDPI_PROTOCOL_CROSSFIRE, protocol_type);
}

void ndpi_search_crossfire_tcp_udp(struct ndpi_detection_module_struct *ndpi_struct, struct ndpi_flow_struct *flow)
{
	struct ndpi_packet_struct *packet = &flow->packet;
	
//      struct ndpi_id_struct         *src=ndpi_struct->src;
//      struct ndpi_id_struct         *dst=ndpi_struct->dst;

	NDPI_LOG(NDPI_PROTOCOL_CROSSFIRE, ndpi_struct, NDPI_LOG_DEBUG, "search crossfire.\n");


	if (packet->udp != 0) {
		if (packet->payload_packet_len == 25 && get_u_int32_t(packet->payload, 0) == ntohl(0xc7d91999)
			&& get_u_int16_t(packet->payload, 4) == ntohs(0x0200)
			&& get_u_int16_t(packet->payload, 22) == ntohs(0x7d00)
			) {
			NDPI_LOG(NDPI_PROTOCOL_CROSSFIRE, ndpi_struct, NDPI_LOG_DEBUG, "Crossfire: found udp packet.\n");
			ndpi_int_crossfire_add_connection(ndpi_struct, flow, NDPI_REAL_PROTOCOL);
			return;
		}

	} else if (packet->tcp != 0) {

		if (packet->payload_packet_len > 4 && memcmp(packet->payload, "GET /", 5) == 0) {
			ndpi_parse_packet_line_info(ndpi_struct, flow);
			if (packet->parsed_lines == 8
				&& (packet->line[0].ptr != NULL && packet->line[0].len >= 30
					&& (memcmp(&packet->payload[5], "notice/login_big", 16) == 0
						|| memcmp(&packet->payload[5], "notice/login_small", 18) == 0))
				&& memcmp(&packet->payload[packet->line[0].len - 19], "/index.asp HTTP/1.", 18) == 0
				&& (packet->host_line.ptr != NULL && packet->host_line.len >= 13
					&& (memcmp(packet->host_line.ptr, "crossfire", 9) == 0
						|| memcmp(packet->host_line.ptr, "www.crossfire", 13) == 0))
				) {
				NDPI_LOG(NDPI_PROTOCOL_CROSSFIRE, ndpi_struct, NDPI_LOG_DEBUG, "Crossfire: found HTTP request.\n");
				ndpi_int_crossfire_add_connection(ndpi_struct, flow, NDPI_CORRELATED_PROTOCOL);
				return;
			}
		}

	}

	NDPI_LOG(NDPI_PROTOCOL_CROSSFIRE, ndpi_struct, NDPI_LOG_DEBUG, "exclude crossfire.\n");
	NDPI_ADD_PROTOCOL_TO_BITMASK(flow->excluded_protocol_bitmask, NDPI_PROTOCOL_CROSSFIRE);
}



#endif
