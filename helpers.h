#ifndef _HELPERS_
#define _HELPERS_

#define BUFLEN 4096
#define LINELEN 1000
#define IP_SERVER "185.118.200.35"
#define PORT_SERVER 8081

void error(const char *msg);
void compute_message(char *message, const char *line);
int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag);
void close_connection(int sockfd);
void send_to_server(int sockfd, char *message);
char *receive_from_server(int sockfd);
char *compute_get_request(char *host, char *url, char *params, char *type, char *cookie, char *token);
char *compute_post_request(char *host, char *url, char *params, char *type, char *cookie, char *token);
int hostname_to_ip(char *hostname, char *ip);

char *compute_task2(char *response);
char *compute_task3(char *response);
char *compute_task4(char *response, char *token);
char *compute_task5_1(char *response, char *token, char *ip);
void compute_cookies(char *in, char *cookie);

#endif