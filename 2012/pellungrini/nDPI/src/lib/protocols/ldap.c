/*
 * ldap.c
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
#ifdef NDPI_PROTOCOL_LDAP

static void ndpi_int_ldap_add_connection(struct ndpi_detection_module_struct *ndpi_struct,
					 struct ndpi_flow_struct *flow)
{
  ndpi_int_add_connection(ndpi_struct, flow, NDPI_PROTOCOL_LDAP, NDPI_REAL_PROTOCOL);
}

void ndpi_search_ldap(struct ndpi_detection_module_struct *ndpi_struct, struct ndpi_flow_struct *flow)
{
	struct ndpi_packet_struct *packet = &flow->packet;
	
//      struct ndpi_id_struct         *src=ndpi_struct->src;
//      struct ndpi_id_struct         *dst=ndpi_struct->dst;

//  u_int16_t dport;



	NDPI_LOG(NDPI_PROTOCOL_LDAP, ndpi_struct, NDPI_LOG_DEBUG, "search ldap\n");


	if (packet->payload_packet_len >= 14 && packet->payload[0] == 0x30) {

		// simple type
		if (packet->payload[1] == 0x0c && packet->payload_packet_len == 14 &&
			packet->payload[packet->payload_packet_len - 1] == 0x00 && packet->payload[2] == 0x02) {

			if (packet->payload[3] == 0x01 &&
				(packet->payload[5] == 0x60 || packet->payload[5] == 0x61) && packet->payload[6] == 0x07) {
				NDPI_LOG(NDPI_PROTOCOL_LDAP, ndpi_struct, NDPI_LOG_DEBUG, "found ldap simple type 1\n");
				ndpi_int_ldap_add_connection(ndpi_struct, flow);
				return;
			}

			if (packet->payload[3] == 0x02 &&
				(packet->payload[6] == 0x60 || packet->payload[6] == 0x61) && packet->payload[7] == 0x07) {
				NDPI_LOG(NDPI_PROTOCOL_LDAP, ndpi_struct, NDPI_LOG_DEBUG, "found ldap simple type 2\n");
				ndpi_int_ldap_add_connection(ndpi_struct, flow);
				return;
			}
		}
		// normal type
		if (packet->payload[1] == 0x84 && packet->payload_packet_len >= 0x84 &&
			packet->payload[2] == 0x00 && packet->payload[3] == 0x00 && packet->payload[6] == 0x02) {

			if (packet->payload[7] == 0x01 &&
				(packet->payload[9] == 0x60 || packet->payload[9] == 0x61 || packet->payload[9] == 0x63 ||
				 packet->payload[9] == 0x64) && packet->payload[10] == 0x84) {

				NDPI_LOG(NDPI_PROTOCOL_LDAP, ndpi_struct, NDPI_LOG_DEBUG, "found ldap type 1\n");
				ndpi_int_ldap_add_connection(ndpi_struct, flow);
				return;
			}

			if (packet->payload[7] == 0x02 &&
				(packet->payload[10] == 0x60 || packet->payload[10] == 0x61 || packet->payload[10] == 0x63 ||
				 packet->payload[10] == 0x64) && packet->payload[11] == 0x84) {

				NDPI_LOG(NDPI_PROTOCOL_LDAP, ndpi_struct, NDPI_LOG_DEBUG, "found ldap type 2\n");
				ndpi_int_ldap_add_connection(ndpi_struct, flow);
				return;
			}
		}
	}


	NDPI_LOG(NDPI_PROTOCOL_LDAP, ndpi_struct, NDPI_LOG_DEBUG, "ldap excluded.\n");
	NDPI_ADD_PROTOCOL_TO_BITMASK(flow->excluded_protocol_bitmask, NDPI_PROTOCOL_LDAP);
}

#endif
