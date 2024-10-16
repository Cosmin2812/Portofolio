#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "common.h"
#include "helpers.h"
#include "messages.h"

#define MAX_CONNECTIONS 100

void run_chat_multi_server(int listenfd, int udp_listenfd, struct topic_entry *topics, int *nr_topics)
{
    int num_sockets = 3;
    int poll_max_capacity = num_sockets;
    struct pollfd *poll_fds = calloc(num_sockets, sizeof(struct pollfd));
    int rc;

    // Set socket TCP soket on listen
    rc = listen(listenfd, MAX_CONNECTIONS);
    DIE(rc < 0, "listen");

    poll_fds[0].fd = listenfd;
    poll_fds[0].events = POLLIN;

    poll_fds[1].fd = udp_listenfd;
    poll_fds[1].events = POLLIN;

    poll_fds[2].fd = fileno(stdin);
    poll_fds[2].events = POLLIN;

    struct client_info *clients_info = NULL;
    int nr_clients = 0;

    while (1) {
        // Wait to receive something on one of the sockets
        rc = poll(poll_fds, num_sockets, -1);
        DIE(rc < 0, "poll");

        for (int i = 0; i < num_sockets; i++) {
            if (poll_fds[i].revents & POLLIN) {

                /*
                Received a connection request from the TCP listen socket 
                */
                if (poll_fds[i].fd == listenfd) {
                    struct sockaddr_in cli_addr;
                    socklen_t cli_len = sizeof(cli_addr);
                    const int newsockfd =
                        accept(listenfd, (struct sockaddr *)&cli_addr, &cli_len);
                    DIE(newsockfd < 0, "accept");

                    char *client_ip = inet_ntoa(cli_addr.sin_addr);
                    int client_port = ntohs(cli_addr.sin_port);

                    if(num_sockets == poll_max_capacity) {
                        poll_max_capacity *= 2;
                        poll_fds = realloc(poll_fds, poll_max_capacity * sizeof(struct pollfd));
                        DIE(poll_fds == NULL, "realloc");
                    }

                    poll_fds[num_sockets].fd = newsockfd;
                    poll_fds[num_sockets].events = POLLIN;
                    num_sockets++;

                    nr_clients++;
                    clients_info = realloc(clients_info, nr_clients * sizeof(struct client_info));

                    clients_info[nr_clients - 1].socketFD = newsockfd;
                    clients_info[nr_clients - 1].ID = NULL;
                    clients_info[nr_clients - 1].port = client_port;
                    clients_info[nr_clients - 1].IP = client_ip;

                /*
                Received data on the socket connected to stdin
                */
                } else if (poll_fds[i].fd == fileno(stdin)) {
                    char buf[MSG_MAXSIZE + 1];
                    memset(buf, 0, MSG_MAXSIZE + 1);
                    if (fgets(buf, sizeof(buf), stdin) == NULL || isspace(buf[0])) {
                        break; // Exit loop if EOF or whitespace character is encountered
                    }

                    // Remove the newline from fgets
                    buf[strlen(buf) - 1] = 0;


                    if(strcmp(buf, "exit") == 0) { 
                        for(int j = 1; j < num_sockets; j++) {
                            close(poll_fds[i].fd);
                        }
                        free(poll_fds);
                        delete_clients(&clients_info, nr_clients);
                    } else {
                        fprintf(stderr, "Bad message!\n");
                    }
                }

                /*
                    Received a packet from the UDP client
                */
                else if (poll_fds[i].fd == udp_listenfd) {
                    struct sockaddr_in client_addr;
                    struct udp_packet packet;
                    memset(&packet, 0, sizeof(struct udp_packet));
                    socklen_t clen = sizeof(client_addr);
                    int rc = recvfrom(udp_listenfd, &packet, sizeof(packet), 0,
                                      (struct sockaddr *)&client_addr, &clen);
                    DIE(rc < 0, "udp error");
                    if(packet.type != 0 || packet.type != 1 || packet.type != 2 || packet.type != 3) {
                        fprintf(stderr, "Corrupted packet!\n");
                    }

                    char *client_ip = inet_ntoa(client_addr.sin_addr);
                    int client_port = ntohs(client_addr.sin_port);



                    for(int k = 0; k < nr_clients; k++) {
                        clients_info[k].got_packet = 0;
                    }

                    for(int k = 0; k < *nr_topics; k++) {
                        if(cmp_topics(packet.topic, topics[k].topic) == 0) {
                            char** subs = topics[k].subscribed_clients;
                            int nr_subs = topics[k].nr_subscribers;
                            int fd;

                            for(int j = 0; j < nr_subs; j++) {
                                for(int p = 0; p < nr_clients; p++) {
                                    if(strcmp(subs[j], clients_info[p].ID) == 0) {
                                        if(clients_info[p].got_packet == 0) {
                                            int rc;
                                            fd = clients_info[p].socketFD;
                                            rc = send_info(fd, client_ip, client_port, packet);
                                            DIE(rc < 0, "send failed");
                                            clients_info[p].got_packet = 1;
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }

                /*
                Received a packet on one of the TCP clients sockets
                */
                else {
                    int packet_len = 0;
                    int rc = recv_all(poll_fds[i].fd, &packet_len,
                                      sizeof(int));
                    char *buf = malloc(packet_len);
                    rc = recv_all(poll_fds[i].fd, buf, packet_len);
                    DIE(rc < 0, "recv");

                    if (rc == 0 || strcmp(buf, "exit") == 0) {
                        for(int k = 0; k < nr_clients; k++) {
                            if(poll_fds[i].fd == clients_info[k].socketFD) {
                                printf("Client %s disconnected.\n", clients_info[k].ID);
                                for(int p = k; p < nr_clients - 1; p++) {
                                    memcpy(&clients_info[k], &clients_info[k + 1], sizeof(struct client_info));
                                }
                                nr_clients--;
                                clients_info = realloc(clients_info, nr_clients * sizeof(struct client_info));
                                if(nr_clients != 0) {
                                    DIE(clients_info == NULL, "realloc");
                                }
                                break;
                            }
                        }
                        close(poll_fds[i].fd);

                        // Remove closed socket from the list
                        for (int j = i; j < num_sockets - 1; j++)
                        {
                            poll_fds[j] = poll_fds[j + 1];
                        }
                        num_sockets--;
                    }
                    else
                    {
                        int client_index = -1;
                        int hasID = 1;
                        for(int k = 0; k < nr_clients; k++) {
                            if(clients_info[k].socketFD == poll_fds[i].fd) {
                                client_index = k;
                                if(clients_info[k].ID == NULL) {
                                    hasID = 0;
                                    break;
                                }
                            }
                        }

                        DIE(client_index == -1, "mem corr");

                        if(hasID == 1) {
                            char* topic_name = get_topic(buf);
                            execute_command(buf, topic_name, &topics, nr_topics, clients_info[client_index].ID);
                        } else {
                            char* new_ID = strdup(buf);
                            //Verify if there is another client with the same ID
                            int same_ID = 0;
                            for(int k = 0; k < nr_clients; k++) {
                                if(clients_info[k].ID != NULL && strcmp(clients_info[k].ID, new_ID) == 0) {
                                    printf("Client %s already connected.\n", new_ID);
                                    same_ID = 1;
                                    for(int p = client_index; p < nr_clients - 1; p++) {
                                        clients_info[p].ID = clients_info[p + 1].ID;
                                        clients_info[p].socketFD = clients_info[p + 1].socketFD;
                                    }
                                    nr_clients--;
                                    clients_info = realloc(clients_info, nr_clients * sizeof(struct client_info));
                                    close(poll_fds[i].fd);
                                    for (int j = i; j < num_sockets - 1; j++)
                                    {
                                        poll_fds[j] = poll_fds[j + 1];
                                    }

                                    num_sockets--;
                                    break;
                                }
                            }

                            if(same_ID == 0 && client_index != -1) {
                                clients_info[client_index].ID = new_ID;
                                char *IP = clients_info[client_index].IP;
                                int port = clients_info[client_index].port;
                                printf("New client %s connected from %s:%d\n", new_ID, IP, port);
                            }
                        }
                    }
                    free(buf);
                }
            }
        }
    }
    free(poll_fds);
    free(clients_info);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "\n Usage: %s <port>\n", argv[0]);
        return 1;
    }

    setvbuf(stdout, NULL, _IONBF, BUFSIZ);
    // Parse the port as a number
    uint16_t port;
    int rc = sscanf(argv[1], "%hu", &port);
    DIE(rc != 1, "Given port is invalid");

    // TCP socket
    const int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    DIE(listenfd < 0, "socket");

    // UDP socket
    const int udp_listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    DIE(udp_listenfd < 0, "socket");

    struct sockaddr_in serv_addr;
    socklen_t socket_len = sizeof(struct sockaddr_in);


    int enable = 1;
    // Set the TCP socket reusable
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
    }

    // Set the UDP socket reusable
    enable = 1;
    if (setsockopt(udp_listenfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
    }

    // Disable Nagle's algo
    enable = 1;
    if (setsockopt(listenfd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(int)) < 0) {
        perror("Setting TCP_NODELAY failed");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, socket_len);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind sockets to the server address
    rc = bind(listenfd, (const struct sockaddr *)&serv_addr, sizeof(serv_addr));
    DIE(rc < 0, "bind");

    rc = bind(udp_listenfd, (const struct sockaddr *)&serv_addr, sizeof(serv_addr));
    DIE(rc < 0, "bind");

    struct topic_entry *topics = NULL;
    int nr_topics = 0;
    run_chat_multi_server(listenfd, udp_listenfd, topics, &nr_topics);
    destroy_topics(&topics, nr_topics);

    // Close listenfd
    close(listenfd);

    return 0;
}
