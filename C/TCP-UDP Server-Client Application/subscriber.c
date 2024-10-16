#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "common.h"
#include "helpers.h"
#include "messages.h"

void run_client(int sockfd)
{
	char buf[MSG_MAXSIZE + 1];
	memset(buf, 0, MSG_MAXSIZE + 1);

	struct pollfd poll_fds[2];
	int num_sockets = 2;
	int rc;

	/*
	poll_fds[0] is the socket that communicates to the server
	poll_fds[1] is the socket that communicates with the user
	*/
	poll_fds[0].fd = sockfd;
	poll_fds[0].events = POLLIN;
	poll_fds[1].fd = fileno(stdin);
	poll_fds[1].events = POLLIN;

	while (1) {
		rc = poll(poll_fds, num_sockets, -1);
		DIE(rc < 0, "poll");

		// Reset the buffer
		memset(buf, 0, MSG_MAXSIZE + 1);

		if (poll_fds[1].revents & POLLIN) {
			if (fgets(buf, sizeof(buf), stdin) == NULL || isspace(buf[0])) {
				break; // Exit loop if EOF or whitespace character is encountered
			}

			buf[strlen(buf) - 1] = 0;
			int len = strlen(buf) + 1;

			if(is_valid(buf) != 0) {
				continue;
			}

			if (strstr(buf, "unsubscribe") > 0) {
				printf("Unsubscribed from topic %s\n", buf + 12);
			} else if (strstr(buf, "subscribe") > 0) {
				printf("Subscribed to topic %s\n", buf + 10);
			}

			// Send user message to the server
			send_buf(sockfd, buf, len);
		}

		if (poll_fds[0].revents & POLLIN)
		{
			char udp_ip[20];
			int udp_port;
			struct udp_packet udp_pack;
			memset(&udp_pack, 0, sizeof(struct udp_packet));

			int rc = recv_info(sockfd, udp_ip, &udp_port, &udp_pack);
			DIE(rc < 0, "recv");
			if (rc == 0) {
				close(poll_fds[1].fd);
				break;
			}

			switch (udp_pack.type) {
			case 0:
				int received_number = parse_int(udp_pack.content);
				printf("%s:%d - %s - INT - %d\n", udp_ip, udp_port, udp_pack.topic, received_number);
				break;
			case 1:
				float received_short_real = parse_short_real(udp_pack.content);
				printf("%s:%d - %s - SHORT_REAL - %.2f\n", udp_ip, udp_port, udp_pack.topic, received_short_real);
				break;
			case 2:
				unsigned int nr_decimals = 0;
				float received_float = parse_float(udp_pack.content, &nr_decimals);
				printf("%s:%d - %s - FLOAT - %.*f\n", udp_ip, udp_port, udp_pack.topic, nr_decimals, received_float);
				break;
			case 3:
				printf("%s:%d - %s - STRING - %s\n", udp_ip, udp_port, udp_pack.topic, udp_pack.content);
				break;
			}
		}
	}
}

int main(int argc, char *argv[])
{

	// Verify if the right number of arguments are given
	if (argc != 4) {
		fprintf(stderr, "\n Usage: %s <id_client> <ip_server> <port>\n", argv[0]);
		return 1;
	}

	// Deactivate stdout buffering
	setvbuf(stdout, NULL, _IONBF, BUFSIZ);

	// Parse the port as a 2 bytes number
	uint16_t port;
	int rc = sscanf(argv[3], "%hu", &port);
	DIE(rc != 1, "Given port is invalid");

	// Get TCP socket for server connection
	const int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	DIE(sockfd < 0, "socket");

	// Write the server address, family, and the port in serv_addr
	struct sockaddr_in serv_addr;
	socklen_t socket_len = sizeof(struct sockaddr_in);

	memset(&serv_addr, 0, socket_len);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	rc = inet_pton(AF_INET, argv[2], &serv_addr.sin_addr.s_addr);
	DIE(rc <= 0, "inet_pton");

	// Connect to the server
	rc = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	DIE(rc < 0, "connect");

	// Send the client ID as the first packet
	send_buf(sockfd, argv[1], strlen(argv[1]) + 1);
	run_client(sockfd);

	// CLose connection
	close(sockfd);

	return 0;
}
