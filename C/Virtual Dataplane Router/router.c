#include "queue.h"
#include "lib.h"
#include "protocols.h"
#include <string.h>
#include <arpa/inet.h>

uint8_t broadcast_mac_addr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void ip_address_print(uint32_t ip) {
	for(int i = 0; i < 4; i++) {
		int x = *(((uint8_t *) &ip) + i);
		printf("%d ", x);
	}
	printf("\n");
}

void mac_address_print(uint8_t mac[6]) {
	for(int i = 0; i < 6; i++) {
		int x = mac[i];
		printf("%d ", x);
	}
	printf("\n");
}

void switch_mac_addresses(uint8_t in[6], uint8_t out[6]) {
	for(int i = 0; i < 6; i++) {
		uint8_t temp = in[i];
		in[i] = out[i];
		out[i] = temp;
	}
}

void switch_ip_addresses(uint32_t *in, uint32_t *out) {
	uint32_t temp = 0;
	temp = *in;
	*in = *out;
	*out = temp;
}

void request_mac(int interface, uint32_t ip_dest, uint8_t exit_port_mac[6], uint32_t ip_exit_port) {
	int arp_req_length = sizeof(struct ether_header) + sizeof(struct arp_header);
	char new_buf[arp_req_length];
	struct ether_header *eth_hdr = (struct ether_header *) new_buf;
	struct arp_header *arp_hdr = (struct arp_header *) (new_buf + sizeof(struct ether_header));
	memcpy(eth_hdr->ether_dhost, broadcast_mac_addr, sizeof(uint8_t) * 6);
	memcpy(eth_hdr->ether_shost, exit_port_mac, sizeof(uint8_t) * 6);
	eth_hdr->ether_type = htons(ETHERTYPE_ARP);
	arp_hdr->htype = htons(1);
	arp_hdr->ptype = htons(ETHERTYPE_IP);
	arp_hdr->plen = 4;
	arp_hdr->hlen = 6;
	arp_hdr->op = htons(1); // request
	memcpy(arp_hdr->sha, exit_port_mac, sizeof(uint8_t) * 6);
	arp_hdr->spa = ip_exit_port;
	arp_hdr->tpa = ip_dest;
	memset(arp_hdr->tha, 0, sizeof(uint8_t) * 6);
	send_to_link(interface, new_buf, arp_req_length);
}

void send_icmp(char* buf, int buf_len, int interface, uint8_t type, uint8_t code) {
	struct ether_header *eth_hdr = (struct ether_header *) buf;
	struct iphdr *ip_hdr = (struct iphdr *) (buf + sizeof(struct ether_header));
	struct icmphdr *icmp_hdr = (struct icmphdr *)(buf + sizeof(struct ether_header) + sizeof(struct iphdr));

	char* interface_ip = get_interface_ip(interface);
	int copy_len = sizeof(struct iphdr) + 8;
	char inter_buf[copy_len];

	memcpy(inter_buf, ip_hdr, sizeof(struct iphdr) + 8);
	switch_mac_addresses(eth_hdr->ether_shost, eth_hdr->ether_dhost);
	switch_ip_addresses(&ip_hdr->saddr, &ip_hdr->daddr);
	printf("\nInterface IP converted to int: ");
	convert_to_int(interface_ip, &ip_hdr->saddr);
	ip_address_print(ip_hdr->saddr);
	

	mac_address_print(eth_hdr->ether_dhost);
	mac_address_print(eth_hdr->ether_shost);
	icmp_hdr->code = code;
	icmp_hdr->type = type;
	icmp_hdr->checksum = 0;
	if(type == 0 && code == 0) {
		printf("\nEcho reply for the router!\n");
		ip_hdr->check = 0;
		ip_hdr->ttl--;
		ip_hdr->check = htons(checksum((uint16_t *) ip_hdr, sizeof(struct iphdr)));
		icmp_hdr->checksum = 0;
		icmp_hdr->checksum = htons(checksum((uint16_t *) icmp_hdr, buf + buf_len - (char *) icmp_hdr));
		send_to_link(interface, buf, buf_len);

	} else {
		memset(&icmp_hdr->un, 0, 4); // 4 bytes are unused
		// copy the ip hdr + 64 after the unused bytes	
		memcpy(icmp_hdr + 1, inter_buf, copy_len);
		ip_hdr->ttl = 64; // initial value of ttl
		ip_hdr->tot_len = htons(sizeof(struct iphdr) + sizeof(struct icmphdr) + copy_len);
		ip_hdr->protocol = 1;
		ip_hdr->check = htons(checksum((uint16_t *) ip_hdr, sizeof(struct iphdr)));
		icmp_hdr->checksum = 0;
		icmp_hdr->checksum = htons(checksum((uint16_t *) icmp_hdr, sizeof(struct icmphdr) + copy_len));
		send_to_link(interface, buf, sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct icmphdr) + copy_len);
	}
}

int arp_req_receive(char* buf, int interface) {
	// returns 1 if the requester wants the IP address of this interface
	// returns 0 if the requester wants to know the IP address of another device
	printf("Received an ARP request!\n");

	struct ether_header *eth_hdr = (struct ether_header *) buf;
	struct arp_header *arp_hdr = (struct arp_header *) (buf + sizeof(struct ether_header));

	uint32_t int_ip = 0;
	char* interface_ip = get_interface_ip(interface);
	convert_to_int(interface_ip, &int_ip);
	printf("Receiver Interface IP: \n");
	ip_address_print(int_ip);
	printf("The IP address of the MAC he wants to know: \n");
	ip_address_print(arp_hdr->tpa);
	if(arp_hdr->tpa == int_ip) {
		printf("He wants my MAC addr!\n");
	} else {
		printf("He doesn't want my MAC addr!\n");
		return 0;
	}
	int arp_req_length = sizeof(struct ether_header) + sizeof(struct arp_header);
	memcpy(arp_hdr->tha, arp_hdr->sha, sizeof(uint8_t) * 6);
	get_interface_mac(interface, arp_hdr->sha);
	memcpy(eth_hdr->ether_dhost, eth_hdr->ether_shost, sizeof(uint8_t) * 6);
	memcpy(eth_hdr->ether_shost, arp_hdr->sha, sizeof(uint8_t) * 6);
	arp_hdr->op = htons(2);
	uint32_t temp = arp_hdr->spa;
	arp_hdr->spa = arp_hdr->tpa;
	arp_hdr->tpa = temp;
	printf("Sent back my MAC addr!\n");
	send_to_link(interface, buf, arp_req_length);
	return 1;
}

void arp_reply_receive(char* buf, int interface, struct arp_table_entry *arp_cache, int* arp_cache_len) {
	printf("Received an ARP reply!\n");
	struct ether_header *eth_hdr = (struct ether_header *) buf;
	struct arp_header *arp_hdr = (struct arp_header *) (buf + sizeof(struct ether_header));
	int arp_req_length = sizeof(struct ether_header) + sizeof(struct arp_header);
	uint32_t src_ip_addr = arp_hdr->spa;

	printf("The MAC and IP address received are:\n");
	ip_address_print(src_ip_addr);
	mac_address_print(arp_hdr->sha);

	printf("Cached entries:\n");
	for(int i = 0; i < *arp_cache_len; i++) {
		ip_address_print(arp_cache[i].ip);
		mac_address_print(arp_cache[i].mac);
		if(src_ip_addr == arp_cache[i].ip) {
			printf("MAC addr already cached!\n");
			return;
		}
	}

	arp_cache[*arp_cache_len].ip = src_ip_addr;
	memcpy(arp_cache[*arp_cache_len].mac, arp_hdr->sha,sizeof(uint8_t) * 6);
	*arp_cache_len = *arp_cache_len + 1;
}

uint16_t get_arp_opcode(char* buf) {
	struct ether_header *eth_hdr = (struct ether_header *) buf;
	struct arp_header *arp_hdr = (struct arp_header *) (buf + sizeof(struct ether_header));
	return ntohs(arp_hdr->op);
}

void check_and_send_queue(queue* packet_q_addr, struct arp_table_entry *arp_cache, int arp_cache_len) {
	printf("Now let's chec our q\n");
	queue inter_q = queue_create();
	while(!queue_empty(*packet_q_addr)) {
		char* info = queue_deq(*packet_q_addr);
		int buf_len = *((int *) info);
		int interface = *((int *) info + 1);
		uint32_t next_hop_ip = *((uint32_t *) info + 2);
		printf("Length and the interface of the buffer: %d %d \n", buf_len, interface);
		printf("IP address of next hop:\n");
		ip_address_print(next_hop_ip);
		printf("IP address in the queue:\n");
		char* buf = info + 3 * sizeof(int);
		struct ether_header *eth_hdr = (struct ether_header *) buf;
		struct iphdr *ip_hdr = (struct iphdr *) (buf + sizeof(struct ether_header));
		ip_address_print(ip_hdr->daddr);
		for(int i = 0; i < arp_cache_len; i++) {
			if(arp_cache[i].ip == next_hop_ip) {
				memcpy(eth_hdr->ether_dhost, arp_cache[i].mac, sizeof(uint8_t) * 6);
				printf("Found a match!\n");
				send_to_link(interface, buf, buf_len);
				free(info);
				info = NULL;
				break;
			}
		}
		if(info != NULL) {
			queue_enq(inter_q, info);
		}
	}
	*packet_q_addr = inter_q;
}

typedef struct node {
	struct route_table_entry *entry;
	struct node *zero_bit;
	struct node *one_bit;
} trie, node;

node* intit_node() {
	node *n = malloc(sizeof(trie));
	n->entry = NULL;
	n->zero_bit = NULL;
	n->one_bit = NULL;
	return n;
}

void populate_trie(struct route_table_entry *rtable_table, int rtable_len, trie *rtable_trie) {
	for(int i = 0; i < rtable_len; i++) {
		int offset = 31;
		node *n = rtable_trie;
		//printBits(rtable_table[i].mask);
		//printBits(rtable_table[i].prefix);
		uint32_t prefix = ntohl(rtable_table[i].prefix);
		uint32_t mask = ntohl(rtable_table[i].mask);
		printf("We are at %d entry\n", i);
		printf("The mask looks like:\n");
		printf("The prefix looks like:\n");
		while(offset >= 0 && ((1 << offset) & mask) != 0) {
			int bit = ((prefix >> offset) & 1);
			if(bit == 1) {
				if(n->one_bit == NULL) {
					n->one_bit = intit_node();
				}
				n = n->one_bit;
			} else {
				if(n->zero_bit == NULL) {
					n->zero_bit = intit_node();
				}
				n = n->zero_bit;
			}
			offset--;
		}
		n->entry = &rtable_table[i];
	}
}

struct route_table_entry *get_entry(uint32_t ip_addr, trie *rtable_trie) {
	node *n = rtable_trie;
	ip_addr = ntohl(ip_addr);
	struct route_table_entry *longest_entry  = NULL;
	for(int offset = 0; offset < 32; offset++) {
		int bit = (ip_addr >> (31 - offset)) & 1;
		if(bit == 1) {
			if(n->one_bit == NULL) {
				break;
			}
			n = n->one_bit;
		} else {
			if(n->zero_bit == NULL) {
				break;
			}
			n = n->zero_bit;
		}
		if(n->entry != NULL) {
			longest_entry = n->entry;
		}
	}
	return longest_entry;
}


int main(int argc, char *argv[])
{
	char buf[MAX_PACKET_LEN];
	// Do not modify this line
	init(argc - 2, argv + 2);

	struct route_table_entry *rtable = malloc(sizeof(struct route_table_entry) * 80000);
	int rtable_len = read_rtable(argv[1], rtable);

	trie *rtable_trie = intit_node();
	populate_trie(rtable, rtable_len, rtable_trie);

	struct arp_table_entry *arp_cache= malloc(sizeof(struct arp_table_entry) *10);
	int arpcache_len = 0;

	int packet_number = 0;
	printf("GOT HERE!");

	queue packet_queue = queue_create();
	while (1) {

		int interface;
		size_t len;
		interface = recv_from_any_link(buf, &len);
		DIE(interface < 0, "recv_from_any_links");


		struct ether_header *eth_hdr = (struct ether_header *) buf;
		struct iphdr * ip_hdr = (struct iphdr *) (buf + sizeof(struct ether_header));
		/* Note that packets received are in network order,
		any header field which has more than 1 byte will need to be converted to
		host order. For example, ntohs(eth_hdr->ether_type). The oposite is needed when
		sending a packet on the link, */


		/* Get the MAC address of the interface that received the packet */
		uint8_t mac_recv_addr[6];
		get_interface_mac(interface, mac_recv_addr);

		/* Throw the packet if it doesn't have the right MAC destination address */
		if(memcmp(mac_recv_addr, eth_hdr->ether_dhost, 6 * sizeof(uint8_t)) != 0 && memcmp(eth_hdr->ether_dhost, broadcast_mac_addr, 6 * sizeof(uint8_t)) != 0) {
			continue;
		}


		/* Throw the packet if it doesn't have the right ether type */
		if ((ntohs(eth_hdr->ether_type) != ETHERTYPE_IP) && ntohs(eth_hdr->ether_type) != ETHERTYPE_ARP) {
			printf("Ignored non-IPv4 packet\n");
			continue;
		}

		if(ntohs(eth_hdr->ether_type) == ETHERTYPE_ARP) {
			uint16_t op_code = get_arp_opcode(buf);
			if(op_code == 1) {
				if(arp_req_receive(buf, interface) == 1) {
					continue;
				}
			} else if(op_code == 2) {
				arp_reply_receive(buf, interface, arp_cache, &arpcache_len);
				check_and_send_queue(&packet_queue, arp_cache, arpcache_len);
				continue;
			}	
		}


		char* interface_ip = get_interface_ip(interface);
		uint32_t int_ip = 0;
		convert_to_int(interface_ip, &int_ip);

		printf("%d\n", ip_hdr->check);
		uint16_t temp_checksum = ip_hdr->check;

		// make check equal to 0 before checking
		ip_hdr->check = 0;

		printf("%d\n", htons(checksum((uint16_t *) ip_hdr, sizeof(struct iphdr))));
		if(htons(checksum((uint16_t *) ip_hdr, sizeof(struct iphdr))) != temp_checksum) {
			printf("Corrupted package!\n");
			continue;
		}

		if(memcmp(&ip_hdr->daddr, &int_ip, sizeof(uint32_t)) == 0) {
			/* Process the message, because it is destined for the router */
			printf("Packet destined for the router!\n");
			send_icmp(buf, len, interface, 0, 0);
			continue;
		}

		mac_address_print(eth_hdr->ether_dhost);
		mac_address_print(eth_hdr->ether_shost);

		if(ip_hdr->ttl <= 1) {
			printf("TTL is 0!\n");
			send_icmp(buf, len, interface, 11, 0);
			continue;
		} else {
			ip_hdr->ttl--;
			ip_hdr->check = htons(checksum((uint16_t *) ip_hdr, sizeof(struct iphdr)));
		}

		struct route_table_entry *best_route = get_entry(ip_hdr->daddr, rtable_trie);
		if(best_route == NULL) {
			printf("NO MATCH FOUND, sending ICMP answer to the source");
			send_icmp(buf, len, interface, 3, 0);
			continue;
		}


		// printf("MAC address of the exit port of the router: ");
		uint8_t mac_exit_rinterface[6];
		get_interface_mac(best_route->interface, mac_exit_rinterface);


		printf("Source IP address:\n");
		ip_address_print(ip_hdr->saddr);
		printf("Destination IP address:\n");
		ip_address_print(ip_hdr->daddr);

		struct arp_table_entry *next_mac;
		next_mac = get_mac_entry(best_route->next_hop, arp_cache, arpcache_len);
		
		memcpy(eth_hdr->ether_shost, mac_exit_rinterface, sizeof(uint8_t) * 6);
		if(next_mac == NULL) {
			char* exit_port_ip = get_interface_ip(best_route->interface);
			uint32_t exit_port_ip_int;
			convert_to_int(exit_port_ip, &exit_port_ip_int);
			request_mac(best_route->interface, best_route->next_hop, mac_exit_rinterface, exit_port_ip_int);
			char* enqued_info = calloc(sizeof(char), 12 + MAX_PACKET_LEN);
			*((int *) enqued_info) = len;
			*((int *) enqued_info + 1) = best_route->interface;
			*((uint32_t *) enqued_info + 2) = best_route->next_hop;
			memcpy(enqued_info + 12, buf, MAX_PACKET_LEN);
			queue_enq(packet_queue, enqued_info);
			continue;
		}

		memcpy(eth_hdr->ether_dhost, next_mac->mac, sizeof(uint8_t) * 6);

		send_to_link(best_route->interface, buf, len);

	}
}

