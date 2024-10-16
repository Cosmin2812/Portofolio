// SPDX-License-Identifier: BSD-3-Clause

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/sendfile.h>
#include <sys/eventfd.h>
#include <libaio.h>
#include <errno.h>

#include "aws.h"
#include "utils/util.h"
#include "utils/debug.h"
#include "utils/sock_util.h"
#include "utils/w_epoll.h"

/* server socket file descriptor */
static int listenfd;

/* epoll file descriptor */
static int epollfd;

static char request_path[BUFSIZ];

#define OK_REPLY "HTTP/1.1 200 OK\r\n\r\n"
#define ERROR_REPLY "HTTP/1.1 404 Not Found\r\n\r\n"

char bufferOK[BUFSIZ];
char bufferERROR[BUFSIZ];

static int on_path_cb(http_parser *p, const char *buf, size_t len)
{
	memcpy(request_path, buf, len);
	return 0;
}

/* this function wasn't used due to wrong cast,
 * used the one in the samples instead (the one above)
 */
// static int aws_on_path_cb(http_parser *p, const char *buf, size_t len)
// {
// struct connection *conn = (struct connection *)p->data;

// memcpy(conn->request_path, buf, len);
// conn->request_path[len] = '\0';
// conn->have_path = 1;

// return 0;
// }

static void connection_prepare_send_reply_header(struct connection *conn)
{
	/* TODO: Prepare the connection buffer to send the reply header. */
	memset(conn->send_buffer, 0, BUFSIZ);
	sprintf(conn->send_buffer, "HTTP/1.1 200 OK\r\n"
						"Content-Length: %ld\r\n"
						"Connection: close\r\n", conn->file_size);
	conn->send_len = strlen(bufferOK);
}

static void connection_prepare_send_404(struct connection *conn)
{
	/* TODO: Prepare the connection buffer to send the 404 header. */
	memset(conn->send_buffer, 0, BUFSIZ);
	sprintf(conn->send_buffer, "HTTP/1.1 404 Not Found\r\n"
								"Content-Length: %ld\r\n"
								"Connection: close\r\n", conn->file_size);
	conn->send_len = strlen(bufferERROR);
}

static enum resource_type connection_get_resource_type(struct connection *conn)
{
	/* TODO: Get resource type depending on request path/filename. Filename should
	 * point to the static or dynamic folder.
	 */

	if (strstr(conn->request_path, "static") != NULL)
		return RESOURCE_TYPE_STATIC;
	else if (strstr(conn->request_path, "dynamic") != NULL)
		return RESOURCE_TYPE_DYNAMIC;
	return RESOURCE_TYPE_NONE;
}

struct connection *connection_create(int sockfd)
{
	/* TODO: Initialize connection structure on given socket. */
	struct connection *conn = malloc(sizeof(*conn));

	DIE(conn == NULL, "malloc");

	conn->sockfd = sockfd;
	conn->recv_len = 0;
	conn->send_len = 0;
	conn->fd = -1;
	conn->file_size = 0;
	conn->dynamic_ready = 0;
	conn->prepare_send = 0;

	memset(conn->recv_buffer, 0, BUFSIZ);
	memset(conn->send_buffer, 0, BUFSIZ);
	memset(conn->request_path, 0, BUFSIZ);
	memset(&(conn->iocb), 0, sizeof(struct iocb));

	return conn;
}

void connection_start_async_io(struct connection *conn)
{
	/* TODO: Start asynchronous operation (read from file).
	 * Use io_submit(2) & friends for reading data asynchronously.
	 */
}

void connection_remove(struct connection *conn)
{
	/* TODO: Remove connection handler. */
	close(conn->sockfd);
	conn->state = STATE_CONNECTION_CLOSED;
	free(conn);
}

void handle_new_connection(void)
{
	/* TODO: Handle a new connection request on the server socket. */
	static int sockfd;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	struct sockaddr_in addr;
	struct connection *conn;
	int rc;

	/* TODO: Accept new connection. */
	sockfd = accept(listenfd, (SSA *)&addr, &addrlen);
	DIE(sockfd < 0, "accept");
	dlog(LOG_DEBUG, "New connection from %s:%d.\n",
		 inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

	/* TODO: Set socket to be non-blocking. */
	int flags = fcntl(sockfd, F_GETFL, 0);

	DIE(flags < 0, "fcntl");
	rc = fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
	DIE(rc < 0, "fcntl");

	/* TODO: Instantiate new connection handler. */
	conn = connection_create(sockfd);

	/* TODO: Add socket to epoll. */
	rc = w_epoll_add_ptr_in(epollfd, sockfd, conn);
	DIE(rc < 0, "w_epoll_add_fd_in");

	/* TODO: Initialize HTTP_REQUEST parser. */
	http_parser_init(&conn->request_parser, HTTP_REQUEST);
}

void receive_data(struct connection *conn)
{
	ssize_t bytes_recv;
	int rc;
	char abuffer[64];

	rc = get_peer_address(conn->sockfd, abuffer, 64);
	if (rc < 0) {
		ERR("get_peer_address");
		goto remove_connection;
	}

	bytes_recv = recv(conn->sockfd, conn->recv_buffer + conn->recv_len, BUFSIZ, 0);
	if (bytes_recv < 0) {
		ERR("recv");
		goto remove_connection;
	}

	if (bytes_recv == 0) {
		dlog(LOG_INFO, "Connection closed from: %s\n", abuffer);
		goto remove_connection;
	}

	dlog(LOG_DEBUG, "HEEEY\n");
	dlog(LOG_DEBUG, "Received message from: %s\n", abuffer);

	conn->recv_len += bytes_recv;

	while (bytes_recv > 0) {
		bytes_recv = recv(conn->sockfd, conn->recv_buffer + conn->recv_len, BUFSIZ, 0);
		conn->recv_len += bytes_recv;
	}
	conn->state = STATE_REQUEST_RECEIVED;
	dlog(LOG_DEBUG, "Received message: %s\n", conn->recv_buffer);
	return;

remove_connection:
	rc = w_epoll_remove_ptr(epollfd, conn->sockfd, conn);
	DIE(rc < 0, "w_epoll_remove_fd");

	/* remove current connection */
	connection_remove(conn);
}

int connection_open_file(struct connection *conn)
{
	/* TODO: Open file and update connection fields. */
	conn->fd = open(conn->request_path, O_RDWR);
	if (conn->fd == -1) {
		connection_prepare_send_404(conn);
		connection_send_data(conn);
		return -1;
	}

	struct stat file_stat;

	fstat(conn->fd, &file_stat);
	conn->file_size = file_stat.st_size;
	connection_prepare_send_reply_header(conn);
	return 1;
}

void connection_complete_async_io(struct connection *conn)
{
	/* TODO: Complete asynchronous operation; operation returns successfully.
	 * Prepare socket for sending.
	 */
}

int parse_header(struct connection *conn)
{
	/* TODO: Parse the HTTP header and extract the file path. */
	/* Use mostly null settings except for on_path callback. */
	http_parser_settings settings_on_path = {
		.on_message_begin = 0,
		.on_header_field = 0,
		.on_header_value = 0,
		.on_path = on_path_cb,
		.on_url = 0,
		.on_fragment = 0,
		.on_query_string = 0,
		.on_body = 0,
		.on_headers_complete = 0,
		.on_message_complete = 0};

	http_parser_init(&(conn->request_parser), HTTP_REQUEST);
	size_t parsed_bytes = http_parser_execute(&(conn->request_parser), &settings_on_path,
												conn->recv_buffer, conn->recv_len);

	if (parsed_bytes != conn->recv_len) {
		ERR("http_parser_execute");
		return -1;
	}

	/* complete the request_path */
	memset(conn->request_path, 0, BUFSIZ);
	memcpy(conn->request_path, AWS_DOCUMENT_ROOT, strlen(AWS_DOCUMENT_ROOT));
	memcpy(conn->request_path + strlen(AWS_DOCUMENT_ROOT), request_path + 1, strlen(request_path) - 1);
	return 0;
}

enum connection_state connection_send_static(struct connection *conn)
{
	/* TODO: Send static data using sendfile(2). */
	ssize_t bytes_sent;
	int rc;
	char abuffer[64];

	rc = get_peer_address(conn->sockfd, abuffer, 64);

	if (rc < 0) {
		ERR("get_peer_address");
		return -1;
	}

	bytes_sent = sendfile(conn->sockfd, conn->fd, NULL, conn->file_size);
	printf("bytes sent: %ld\n", bytes_sent);
	conn->file_size -= bytes_sent;

	if (bytes_sent < 0) { /* error in communication */
		dlog(LOG_ERR, "Error in communication to %s\n", abuffer);
		return -1;
	}

	if (bytes_sent == 0) { /* connection closed */
		dlog(LOG_INFO, "Connection closed to %s\n", abuffer);
		return -1;
	}

	if (conn->file_size == 0)
		return 1;

	return 0;
}

int connection_send_data(struct connection *conn)
{
	/* May be used as a helper function. */
	/* TODO: Send as much data as possible from the connection send buffer.
	 * Returns the number of bytes sent or -1 if an error occurred
	 */
	ssize_t bytes_sent;
	int rc;
	char abuffer[64];

	rc = get_peer_address(conn->sockfd, abuffer, 64);
	if (rc < 0) {
		ERR("get_peer_address");
		goto remove_connection;
	}

	conn->res_type = connection_get_resource_type(conn);

	/* contents from the buffer are sent as long as there are bytes remaining */
	if (conn->send_len > 0) {
		bytes_sent = send(conn->sockfd, conn->send_buffer, conn->send_len, 0);

		if (bytes_sent < 0) { /* error in communication */
			dlog(LOG_ERR, "Error in communication to %s\n", abuffer);
			goto remove_connection;
		}

		if (bytes_sent == 0) { /* connection closed */
			dlog(LOG_INFO, "Connection closed to %s\n", abuffer);
			goto remove_connection;
		}

		conn->send_len -= bytes_sent;
		memmove(conn->send_buffer, conn->send_buffer + bytes_sent, conn->send_len);
		return 0;
	}

	/* send file */

	/* the operation can be executed multiple times,
	 * depending on the size of the file to be sent
	 */
	if (conn->res_type == RESOURCE_TYPE_STATIC) {
		rc = connection_send_static(conn);
		if (rc == 0)
			return 0;
		else if (rc < 0)
			goto remove_connection;

	} else if (conn->res_type == RESOURCE_TYPE_DYNAMIC) {
		if (conn->state != STATE_ASYNC_ONGOING && conn->state != STATE_ASYNC_DONE) {
			connection_send_dynamic(conn);
			return 0;
		} else if (conn->state == STATE_ASYNC_DONE) {
			int bytes_sent = send(conn->sockfd, conn->recv_file, conn->file_size, 0);

			if (bytes_sent < 0) {
				/* error in communication */
				dlog(LOG_ERR, "Error in communication to %s\n", abuffer);
				goto remove_connection;
			}
			if (bytes_sent == 0) {
				/* connection closed */
				dlog(LOG_INFO, "Connection closed to %s\n", abuffer);
				goto remove_connection;
			}
			dlog(LOG_DEBUG, "bytes sent: %d\n", bytes_sent);
			printf("bytes sent: %d\n", bytes_sent);
			conn->file_size -= bytes_sent;
			//memmove(conn->recv_file, conn->recv_file + bytes_sent, conn->file_size);
			conn->recv_file += bytes_sent;
			if (conn->file_size == 0) {
				io_destroy(conn->ctx);
				free(conn->recv_file);
				close(conn->eventfd);
			} else {
				// strncpy(conn->recv_file, conn->recv_file + bytes_sent, conn->file_size);
				return 0;
			}
		}
	}

	rc = w_epoll_update_ptr_in(epollfd, conn->sockfd, conn);
	DIE(rc < 0, "w_epoll_update_ptr_in");

	conn->state = STATE_DATA_SENT;

	return STATE_DATA_SENT;

remove_connection:
	rc = w_epoll_remove_ptr(epollfd, conn->sockfd, conn);
	DIE(rc < 0, "w_epoll_remove_ptr");

	/* remove current connection */
	connection_remove(conn);

	return STATE_CONNECTION_CLOSED;
}

int connection_send_dynamic(struct connection *conn)
{
	/* TODO: Read data asynchronously.
	 * Returns 0 on success and -1 on error.
	 */

	/* create 0 valued event */
	conn->recv_file = malloc(conn->file_size);
	conn->eventfd = eventfd(0, 0);
	io_prep_pread(&(conn->iocb), conn->fd, conn->recv_file, conn->file_size, 0);
	conn->piocb[0] = &(conn->iocb);
	io_set_eventfd(&(conn->iocb), conn->eventfd);
	int rc = w_epoll_add_ptr_in(epollfd, conn->eventfd, conn);

	DIE(rc < 0, "w_epoll_add_ptr_in");
	if (io_setup(1, &(conn->ctx)) < 0) {
		/* BAD SETUP */

		return -1;
	}
	if (io_submit(conn->ctx, 1, conn->piocb) < 0) {
		/* BAD SUBMIT */
		return -1;
	}
	conn->state = STATE_ASYNC_ONGOING;
	return 0;
}

void handle_input(struct connection *conn)
{
	/* TODO: Handle input information: may be a new message or notification of
	 * completion of an asynchronous I/O operation.
	 */

	switch (conn->state) {
	default:
		printf("shouldn't get here %d\n", conn->state);
	}
}

void handle_output(struct connection *conn)
{
	/* TODO: Handle output information: may be a new valid requests or notification of
	 * completion of an asynchronous I/O operation or invalid requests.
	 */

	switch (conn->state) {
	default:
		ERR("Unexpected state\n");
		exit(1);
	}
}

void handle_client(uint32_t event, struct connection *conn)
{
	/* TODO: Handle new client. There can be input and output connections.
	 * Take care of what happened at the end of a connection.
	 */
	int rc;

	receive_data(conn);
	if (conn->state == STATE_REQUEST_RECEIVED) {
		rc = parse_header(conn);
		if (rc < 0) {
			ERR("parse_header");
			goto remove_connection;
		}

		dlog(LOG_DEBUG, "Request path: %s\n", conn->request_path);
		if (connection_open_file(conn) < 0)
			goto remove_connection;

		/* print cwd for debug */
		char cwd[BUFSIZ];

		getcwd(cwd, BUFSIZ);
		printf("cwd: %s\n", cwd);
		printf("fd: %d\n", conn->fd);
	}
	rc = w_epoll_update_ptr_inout(epollfd, conn->sockfd, conn);
	DIE(rc < 0, "w_epoll_add_ptr_inout");
	return;

remove_connection:
	rc = w_epoll_remove_ptr(epollfd, conn->sockfd, conn);
	DIE(rc < 0, "w_epoll_remove_ptr");

	/* remove current connection */
	connection_remove(conn);
}

int main(void)
{
	int rc;

	/* TODO: Initialize asynchronous operations. */

	/* TODO: Initialize multiplexing. */
	epollfd = w_epoll_create();
	DIE(epollfd < 0, "epoll_create");

	/* TODO: Create server socket. */
	listenfd = tcp_create_listener(AWS_LISTEN_PORT, DEFAULT_LISTEN_BACKLOG);
	DIE(listenfd < 0, "tcp_create_listener");

	/* TODO: Add server socket to epoll object*/
	rc = w_epoll_add_fd_in(epollfd, listenfd);
	DIE(rc < 0, "w_epoll_add_fd_in");

	/* Uncomment the following line for debugging. */
	dlog(LOG_INFO, "Server waiting for connections on port %d\n", AWS_LISTEN_PORT);

	/* server main loop */
	while (1) {
		struct epoll_event rev;

		/* TODO: Wait for events. */
		rc = w_epoll_wait_infinite(epollfd, &rev);
		DIE(rc < 0, "w_epoll_wait_infinite");

		/* TODO: Switch event types; consider
		 *   - new connection requests (on server socket)
		 *   - socket communication (on connection sockets)
		 */

		if (rev.data.fd == listenfd) {
			dlog(LOG_INFO, "New connection request\n");
			if (rev.events & EPOLLIN)
				handle_new_connection();
		} else {
			struct connection *conn = rev.data.ptr;

			if (conn->state == STATE_ASYNC_ONGOING) {
				ssize_t bytesRead = io_getevents(conn->ctx, 1, 1, NULL, NULL);

				if (bytesRead != 0) {
					rc = w_epoll_remove_ptr(epollfd, conn->eventfd, conn);
					conn->state = STATE_ASYNC_DONE;
				}
			} else {
				if (rev.events & EPOLLIN)
					handle_client(rev.events, conn);
				if (rev.events & EPOLLOUT) {
					if (connection_send_data(conn) == STATE_DATA_SENT) {
						w_epoll_remove_ptr(epollfd, conn->sockfd, conn);
						connection_remove(conn);
					}
				}
			}
		}
	}
	return 0;
}
