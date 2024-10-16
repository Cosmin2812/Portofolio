
#include <stddef.h>
#include <stdint.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <math.h>

// Maximum size of the message sent by user
#define MSG_MAXSIZE 1024

char* get_topic(char message[MSG_MAXSIZE + 1]);
int send_buf(int sockfd, void *buffer, size_t len);
struct chat_packet {
  uint16_t len;
  char message[MSG_MAXSIZE + 1];
};

struct udp_packet {
  char topic[50];
  char type;
  char content[1500];
};

struct topic_entry {
  char topic[50];

  // array of all the clients IDs that are subscribed to the topic
  char** subscribed_clients;
  int nr_subscribers;
};

struct client_info {
  int socketFD;
  char* ID;
  char* IP;
  int port;
  int got_packet;
};

void execute_command(char *command, char *topic_name, struct topic_entry **topics, int *nr_topics, char * client_ID);
int send_info(int sockfd, char* udp_ip, int udp_port, struct udp_packet udp_pack);
int recv_info(int sockfd, char* udp_ip, int *udp_port, struct udp_packet *udp_pack);
int parse_int(char *content);
float parse_float(char *content, unsigned int *nr_decimals);
float parse_short_real(char *content);
int cmp_topics(char packet_topic[50], char existing_topic[50]);
void delete_clients(struct client_info **clients, int nr_clients);
int is_valid(char* buf);
void destroy_topics(struct topic_entry **topics_addr, int nr_topics);