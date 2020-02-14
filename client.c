#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include <json-c/json.h>

int main(int argc, char *argv[])
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *response = calloc(BUFLEN, sizeof(char));
    char *token2 = calloc(BUFLEN, sizeof(char));
    char *char_url = calloc(BUFLEN, sizeof(char));
    char *char_method = calloc(BUFLEN, sizeof(char));

    struct json_object *parsed_json;
    struct json_object *method;
    struct json_object *type;
    struct json_object *url;

    struct json_object *data;
    struct json_object *token;

    int server;

    server = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request("185.118.200.35:8081", "/task1/start", NULL, NULL, NULL, NULL);
    send_to_server(server, message);
    response = receive_from_server(server);
    close_connection(server);
    printf("%s\n", response);

    server = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    send_to_server(server, compute_task2(response));
    response = receive_from_server(server);
    close_connection(server);
    printf("%s\n", response);

    parsed_json = json_tokener_parse(strchr(response, '{'));
    json_object_object_get_ex(parsed_json, "data", &data);
    json_object_object_get_ex(data, "token", &token);
    token2 = (char *)json_object_get_string(token);

    server = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    send_to_server(server, compute_task3(response));
    response = receive_from_server(server);
    close_connection(server);
    printf("%s\n", response);

    server = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    send_to_server(server, compute_task4(response, token2));
    response = receive_from_server(server);
    close_connection(server);
    char cookies[BUFLEN] = {0};
    compute_cookies(response, cookies);
    printf("%s\n", response);

    parsed_json = json_tokener_parse(strchr(response, '{'));

    json_object_object_get_ex(parsed_json, "method", &method);
    json_object_object_get_ex(parsed_json, "url", &url);
    json_object_object_get_ex(parsed_json, "type", &type);
    char_url = (char *)json_object_get_string(url);
    char_method = (char *)json_object_get_string(method);

    char *ip = calloc(BUFLEN, sizeof(char));
    char *msg = calloc(BUFLEN, sizeof(char));
    msg = compute_task5_1(response, token2, ip);
    server = open_connection(ip, 80, AF_INET, SOCK_STREAM, 0);

    send_to_server(server, msg);
    response = receive_from_server(server);
    close_connection(server);
    printf("%s\n", response);

    server = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    if (strlen(char_method) == 3)
        message = compute_get_request(IP_SERVER, char_url, strchr(response, '{'), (char *)json_object_get_string(type), cookies, token2);
    else
        message = compute_post_request(IP_SERVER, char_url, strchr(response, '{'), (char *)json_object_get_string(type), cookies, token2);

    send_to_server(server, message);
    response = receive_from_server(server);
    close_connection(server);
    printf("%s\n", response);

    return 0;
}