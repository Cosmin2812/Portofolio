#include "common.h"
#include "messages.h"
#include "helpers.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>


char *get_topic(char message[MSG_MAXSIZE + 1])
{
	char *topic;
	char *p = strtok(message, " ");
	p = strtok(NULL, " ");
	topic = malloc(strlen(p) + 1);
	DIE(topic == NULL, "malloc");
	memcpy(topic, p, strlen(p) + 1);
	return topic;
}

void execute_command(char *command, char *topic_name, struct topic_entry **topics, int *nr_topics, char *clientID)
{
	struct topic_entry *topic = NULL;

	// Get a pointer to the topic info we are interested in
	for (int i = 0; i < *nr_topics; i++) {
		if (strlen((*topics)[i].topic) == strlen(topic_name) && memcmp((*topics)[i].topic, topic_name, strlen((*topics)[i].topic)) == 0) {
			topic = &((*topics)[i]);
		}
	}

	// If the topic doesn't exist, add it to the array
	if (topic == NULL) {
		*nr_topics = *nr_topics + 1;
		*topics = realloc(*topics, *nr_topics * sizeof(struct topic_entry));
		DIE(*topics == NULL, "realloc");

		topic = &((*topics)[*nr_topics - 1]);
		memset(topic, 0, sizeof(struct topic_entry));
		int length = (strlen(topic_name) > 50) ? 50 : strlen(topic_name);
		memcpy(topic->topic, topic_name, length);
		(*topics)[*nr_topics - 1].nr_subscribers = 0;
	}

	char **clients = topic->subscribed_clients;
	int nr_subs = topic->nr_subscribers;

	// Add the client to the subscribed_clients array, if the command is "subscribe"
	if (memcmp(command, "subscribe", strlen("subscribe")) == 0)
	{
		for (int j = 0; j < nr_subs; j++)
		{
			if (strcmp(clients[j], clientID) == 0)
			{
				return;
			}
		}
		topic->nr_subscribers++;
		nr_subs++;
		topic->subscribed_clients = realloc(topic->subscribed_clients, topic->nr_subscribers * sizeof(char *));
		topic->subscribed_clients[nr_subs - 1] = strdup(clientID);
	}

	// Remove the client from the subscribed_clients array, if the command is "unsubscribe"
	else if (memcmp(command, "unsubscribe", strlen("unsubscribe")) == 0)
	{
		for (int j = 0; j < nr_subs; j++)
		{
			if (strcmp(clients[j], clientID) == 0)
			{
				free(clients[j]);
				for (int k = j + 1; k < nr_subs; k++)
				{
					clients[k - 1] = clients[k];
				}
				topic->nr_subscribers--;
				topic->subscribed_clients = realloc(topic->subscribed_clients, topic->nr_subscribers * sizeof(int));
				return;
			}
		}
	}
}

// Send buffer of length len with length prefixation
int send_buf(int sockfd, void *buffer, size_t len)
{
	char *new_buff = malloc(sizeof(int) + len);
	int *p = (int *)new_buff;
	memcpy(new_buff + sizeof(int), buffer, len);
	*p = len;
	return send_all(sockfd, new_buff, sizeof(int) + len);
}

// Send UDP packet message using length prefixation
int send_info(int sockfd, char *udp_ip, int udp_port, struct udp_packet udp_pack)
{
	int lengths[5];
	lengths[0] = strlen(udp_ip) + 1;
	lengths[1] = sizeof(int);
	lengths[2] = strlen(udp_pack.topic) + 1;
	lengths[3] = sizeof(int);
	switch (udp_pack.type)
	{
	case 0:
		lengths[4] = 5;
		break;
	case 1:
		lengths[4] = 2;
		break;
	case 2:
		lengths[4] = 6;
		break;
	default:
		if (strlen(udp_pack.content) == 1500)
		{
			lengths[4] = 1500;
		}
		else
		{
			lengths[4] = strlen(udp_pack.content) + 1;
		}
	}
	int total_length = 0;
	for (int i = 0; i <= 4; i++)
	{
		total_length += lengths[i];
	}
	char *buf = malloc(total_length);
	char *p = buf;
	memcpy(p, udp_ip, lengths[0]);
	p += lengths[0];
	memcpy(p, &udp_port, lengths[1]);
	p += lengths[1];
	memcpy(p, udp_pack.topic, lengths[2]);
	p += lengths[2];
	memcpy(p, &udp_pack.type, lengths[3]);
	p += lengths[3];
	memcpy(p, udp_pack.content, lengths[4]);
	char *new_buf = malloc(5 * sizeof(int) + total_length);
	memcpy(new_buf, lengths, 5 * sizeof(int));
	memcpy(new_buf + 5 * sizeof(int), buf, total_length);
	int rc = send_all(sockfd, new_buf, total_length + 5 * sizeof(int));
	free(buf);
	free(new_buf);
	return rc;
}

// Function for receving UDP packet, used by TCP clients
int recv_info(int sockfd, char *udp_ip, int *udp_port, struct udp_packet *udp_pack)
{
	int lengths[5];
	int rc = recv_all(sockfd, lengths, sizeof(int) * 5);
	if (rc <= 0)  {
		return rc;
	}

	int total_length = 0;
	for (int i = 0; i <= 4; i++) {
		total_length += lengths[i];
	}
	char *buf = malloc(total_length);
	rc = recv_all(sockfd, buf, total_length);
	if (rc <= 0) {
		return rc;
	}

	char *p = buf;
	memcpy(udp_ip, p, lengths[0]);
	p += lengths[0];
	memcpy(udp_port, p, lengths[1]);
	p += lengths[1];
	memcpy(udp_pack->topic, p, lengths[2]);
	p += lengths[2];
	memcpy(&udp_pack->type, p, lengths[3]);
	p += lengths[3];
	memcpy(udp_pack->content, p, lengths[4]);

	free(buf);
	return rc;
}

int parse_int(char *content)
{
	char first_byte = *content;
	int multiplier = 1;
	if (first_byte == 1) {
		multiplier = -1;
	}

	content = content + 1;
	unsigned int number = *((unsigned int *)content);
	return multiplier * ntohl(number);
}

float parse_short_real(char *content)
{
	unsigned short nr = *((unsigned short *)content);
	nr = ntohs(nr);
	return (float)nr / 100;
}

float parse_float(char *content, unsigned int *decimals)
{
	char first_byte = *content;
	int multiplier = 1;
	if (first_byte == 1) {
		multiplier = -1;
	}
	content = content + 1;
	unsigned int glued_nr = *((unsigned int *)content);
	glued_nr = ntohl(glued_nr);

	content = content + 4;
	int power = *content;
	*decimals = power;

	return (float)multiplier * glued_nr * pow(10, -power);
}

void delete_clients(struct client_info **clients, int nr_clients)
{
	for (int i = 0; i < nr_clients; i++) {
		free((*clients)[i].IP);
		free((*clients)[i].ID);
	}
	free(*clients);
	*clients = NULL;
}

// Topics matching function
int cmp_topics(char packet_topic[50], char existing_topic[50])
{
	// packet_topic is the topic that cannot contain wildcards
	// existing_topic is a topic contained in the topics list and can contain wildcards
	// strtok alters the initial string, so we work with copies
	if (strncmp(packet_topic, existing_topic, 50) == 0) {
		return 0;
	}
	char *ptopic_copy = malloc(50);
	char *ltopic_copy = malloc(50);
	char *in1 = ptopic_copy;
	char *in2 = ltopic_copy;
	memcpy(ptopic_copy, packet_topic, 50);
	memcpy(ltopic_copy, existing_topic, 50);
	char *p, *l;
	int pmax_size = 20, lmax_size = 20;
	char **pwords = malloc(pmax_size * sizeof(char *));
	char **lwords = malloc(lmax_size * sizeof(char *));
	int pindex = 0, lindex = 0;

	p = strtok(ptopic_copy, "/");
	while (p != NULL) {
		if (pindex == pmax_size) {
			pwords = realloc(pwords, 2 * pmax_size);
		}
		pwords[pindex++] = strdup(p);
		p = strtok(NULL, "/");
	}

	l = strtok(ltopic_copy, "/");
	while (l != NULL) {
		if (lindex == lmax_size) {
			lwords = realloc(lwords, 2 * lmax_size);
		}
		lwords[lindex++] = strdup(l);
		l = strtok(NULL, "/");
	}

	int nr_words_p = pindex;
	int nr_words_l = lindex;
	pindex = 0, lindex = 0;
	while (pindex < nr_words_p && lindex < nr_words_l) {
		if (strcmp(lwords[lindex], "+") != 0 && strcmp(lwords[lindex], "*") != 0) {
			if (strlen(pwords[pindex]) != strlen(lwords[lindex]) || memcmp(pwords[pindex], lwords[lindex], strlen(pwords[pindex])) != 0) {
				free(in1);
				free(in2);
				free(pwords);
				free(lwords);
				return -1;
			}
			pindex++;
			lindex++;
		} else {
			if (strcmp(lwords[lindex], "+") == 0) {
				pindex++;
				lindex++;
				continue;
			}

			pindex++;
			lindex++;
			if (lindex >= nr_words_l) {
				free(in1);
				free(in2);
				free(pwords);
				free(lwords);
				return 0;
			}

			while (pindex < nr_words_p && strcmp(pwords[pindex], lwords[lindex]) != 0) {
				pindex++;
			}
			if (pindex == nr_words_p) {
				free(in1);
				free(in2);
				free(pwords);
				free(lwords);
				return -1;
			}
			pindex++;
			lindex++;
		}
	}

	free(in1);
	free(in2);

	if (pindex < nr_words_p || lindex < nr_words_l) {
		return -1;
	}

	return 0;
}

int is_valid(char *buf)
{
	char *dup = strdup(buf);
	if (strcmp(dup, "exit") == 0) {
		return 0;
	}
	char *p = strtok(dup, " ");

	if (strcmp(p, "subscribe") != 0 && strcmp(p, "unsubscribe") != 0) {
		fprintf(stderr, "Invalid message, send again!\n");
		return -1;
	}

	p = strtok(NULL, " ");
	if (p == NULL) {
		fprintf(stderr, "Invalid message, send again!\n");
		return -1;
	}

	free(dup);
	return 0;
}

void destroy_topics(struct topic_entry **topics_addr, int nr_topics)
{
	for (int i = 0; i < nr_topics; i++) {
		for (int j = 0; j < (*topics_addr)[i].nr_subscribers; j++) {
			free((*topics_addr)[i].subscribed_clients[j]);
		}
	}
	free(*topics_addr);
	*topics_addr = NULL;
}