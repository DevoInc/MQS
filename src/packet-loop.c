#include <stdio.h>
#include <pcap.h>
#include <log.h>
#include <ctype.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ether.h>

#include <tcpip.h>
#include <packet-loop.h>
#include <mysql-decode.h>

int packet_loop(lua_State *luavm, pcap_t *handle, _Bool quiet)
{
	struct pcap_pkthdr *header;
	const u_char *packet;

	const struct sniff_ethernet *ethernet;
	const struct sniff_ethernet_cooked *ethernet_cooked;			
	const struct sniff_ip *ip;
	const struct sniff_tcp *tcp;
	const unsigned char *payload;

	size_t size_ip;
	size_t size_tcp;
	size_t size_payload;

	size_t link_size = 0;

	int datalink;
	size_t datalink_size;

	int retval;

	int pktid = 0;
	
	datalink = pcap_datalink(handle);

	while ((retval = pcap_next_ex(handle, &header, &packet)) >= 0) {
		pktid++;
		
		switch(retval) {
		case 0:
			continue; 	/* Elapsed Timeout */
		case -1:
			log_crit("Error occurred while reading the packet");
			break;
		case -2:
			log_crit("packets are being read from a ``savefile'' and there are no more packets to read from the savefile: %s", pcap_geterr(handle));
			break;
		}
		if (!packet) {
			log_crit("Error fetching our first packet: %s", pcap_geterr(handle));
		}
		
		switch(datalink) {
		case DLT_RAW:
			ip = (struct sniff_ip*)(packet);
			break;
		case DLT_LINUX_SLL:
			ethernet_cooked = (struct sniff_ethernet_cooked*)(packet);
			ip = (struct sniff_ip*)(packet + COOKED_ETHERNET_SIZE);
			datalink_size = COOKED_ETHERNET_SIZE;
			break;
		default:
			log_notice("Defaults to Ethernet.");
			ethernet = (struct sniff_ethernet*)(packet);
			ip = (struct sniff_ip*)(packet + ETHERNET_SIZE);
			datalink_size = ETHERNET_SIZE;
			break;
		}
		
		size_ip = IP_HL(ip)*4;
		if (size_ip < 20) {
			log_warning("Invalid IP header length: %lu\n", size_ip);
			continue;
		}
		tcp = (struct sniff_tcp*)(packet + datalink_size + size_ip);
		size_tcp = TH_OFF(tcp)*4;
		if (size_tcp < 20) {
			log_warning("Invalid TCP header length: %lu\n", size_ip);
			continue;
		}
		payload = (u_char *)(packet + datalink_size + size_ip + size_tcp);
		size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);

		mysql_decode(pktid, luavm, payload, size_payload, quiet);
	}
	
	return 0;
}
