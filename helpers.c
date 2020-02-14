#include <stdlib.h> /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include <json-c/json.h>

char *compute_get_request(char *host, char *url, char *params, char *type, char *cookie, char *token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    if (params == NULL)
        sprintf(line, "GET %s HTTP/1.1", url);
    else
        sprintf(line, "GET %s?%s HTTP/1.1", url, params);

    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    if (cookie != NULL)
    {
        sprintf(line, "%s", cookie);
        compute_message(message, line);
    }
    if (token != NULL)
    {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    memset(line, 0, LINELEN);
    compute_message(message, line);
    return message;
}

char *compute_post_request(char *host, char *url, char *params, char *type, char *cookie, char *token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if (token != NULL)
    {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }
    if (cookie != NULL)
    {
        strcat(message, cookie);
        strcat(message, "\n");
    }
    sprintf(line, "Content-Type: %s", type);
    compute_message(message, line);

    sprintf(line, "Content-Length: %d", (int)strlen(params));
    compute_message(message, line);

    memset(line, 0, LINELEN);
    compute_message(message, line);

    sprintf(line, "%s", params);
    compute_message(message, params);
    return message;
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}
void compute_message(char *message, const char *line)
{
    strcat(message, line);
    strcat(message, "\r\n");
}

int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag)
{
    struct sockaddr_in serv_addr;
    int sockfd = socket(ip_type, socket_type, flag);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = ip_type;
    serv_addr.sin_port = htons(portno);
    inet_aton(host_ip, &serv_addr.sin_addr);
    if (sockfd < 0)
        error("ERROR opening socket");

    /* connect the socket */
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    return sockfd;
}

void close_connection(int sockfd)
{
    close(sockfd);
}
void send_to_server(int sockfd, char *message)
{
    int bytes, sent = 0;
    int total = strlen(message);
    do
    {
        bytes = write(sockfd, message + sent, total - sent);
        if (bytes < 0)
            error("ERROR writing message to socket");
        if (bytes == 0)
            break;
        sent += bytes;
    } while (sent < total);
}

char *receive_from_server(int sockfd)
{
    char *response = calloc(BUFLEN, sizeof(char));
    int total = BUFLEN;
    int received = 0;
    do
    {
        int bytes = read(sockfd, response + received, total - received);
        if (bytes < 0)
            error("ERROR reading response from socket");
        if (bytes == 0)
        {
            break;
        }
        received += bytes;
    } while (received < total);

    if (received == total)
        error("ERROR storing complete response from socket");

    return response;
}

void compute_cookies(char *in, char *cookie)
{
    char *cookie1 = calloc(BUFLEN, sizeof(char));
    char *cookie2 = calloc(BUFLEN, sizeof(char));
    char *p1, *p2;
    p1 = strstr(in, "prajiturica");
    p2 = strstr(in, "prajiturica.sig");

    int k = 0;
    while (p1[k + 1] != '\n')
    {
        k++;
    }
    strncpy(cookie1, p1, k);

    int k2 = 0;
    while (p2[k2 + 1] != '\n')
    {
        k2++;
    }
    strncpy(cookie2, p2, k2);
    sprintf(cookie, "Cookie: %s; %s", cookie1, cookie2);
}

int hostname_to_ip(char *hostname, char *ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    if ((he = gethostbyname(hostname)) == NULL)
    {
        return 1;
    }

    addr_list = (struct in_addr **)he->h_addr_list;

    for (i = 0; addr_list[i] != NULL; i++)
    {
        strcpy(ip, inet_ntoa(*addr_list[i]));
        return 0;
    }

    return 1;
}

char *compute_request(char *method, char *host, char *url, char *params, char *type, char *cookie, char *token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    if (method == NULL || host == NULL)
        return NULL;
    if (strcmp(method, "GET") == 0)
    {
        message = compute_get_request(host, url, params, type, cookie, token);
        return message;
    }
    else if (strcmp(method, "POST") == 0)
    {
        return compute_post_request(host, url, params, type, cookie, token);
    }
    return NULL;
}

char *compute_task2(char *response)
{
    struct json_object *response_to_json;
    struct json_object *method;
    struct json_object *type;
    struct json_object *url;
    struct json_object *data;
    struct json_object *username;
    struct json_object *password;

    char *message = calloc(BUFLEN, sizeof(char));
    char *form_data = calloc(BUFLEN, sizeof(char));
    char *char_url = calloc(BUFLEN, sizeof(char));
    char *char_type = calloc(BUFLEN, sizeof(char));
    char *char_method = calloc(BUFLEN, sizeof(char));
    char *char_username = calloc(BUFLEN, sizeof(char));
    char *char_password = calloc(BUFLEN, sizeof(char));
    char *cookies = calloc(BUFLEN, sizeof(char));

    response_to_json = json_tokener_parse(strchr(response, '{'));

    json_object_object_get_ex(response_to_json, "method", &method);
    json_object_object_get_ex(response_to_json, "url", &url);
    json_object_object_get_ex(response_to_json, "type", &type);
    json_object_object_get_ex(response_to_json, "data", &data);
    json_object_object_get_ex(data, "username", &username);
    json_object_object_get_ex(data, "password", &password);

    char_url = (char *)json_object_get_string(url);
    char_method = (char *)json_object_get_string(method);
    char_type = (char *)json_object_get_string(type);
    char_username = (char *)json_object_get_string(username);
    char_password = (char *)json_object_get_string(password);

    compute_cookies(response, cookies);
    sprintf(form_data, "username=%s&password=%s", char_username, char_password);
    message = compute_request(char_method, "185.118.200.35", char_url, form_data, char_type, cookies, NULL);
    return message;
}

char *compute_task3(char *response)
{
    struct json_object *response_to_json;
    struct json_object *method;
    struct json_object *type;
    struct json_object *url;
    struct json_object *id;
    struct json_object *data;
    struct json_object *token;
    struct json_object *queryParams;

    char *message = calloc(BUFLEN, sizeof(char));
    char *form_data = calloc(BUFLEN, sizeof(char));
    char *char_url = calloc(BUFLEN, sizeof(char));
    char *char_type = calloc(BUFLEN, sizeof(char));
    char *char_method = calloc(BUFLEN, sizeof(char));
    char *cookies = calloc(BUFLEN, sizeof(char));
    char *char_id = calloc(BUFLEN, sizeof(char));
    char *char_token = calloc(BUFLEN, sizeof(char));

    response_to_json = json_tokener_parse(strchr(response, '{'));

    json_object_object_get_ex(response_to_json, "method", &method);
    json_object_object_get_ex(response_to_json, "url", &url);
    json_object_object_get_ex(response_to_json, "type", &type);
    json_object_object_get_ex(response_to_json, "data", &data);
    json_object_object_get_ex(data, "queryParams", &queryParams);
    json_object_object_get_ex(data, "token", &token);
    json_object_object_get_ex(queryParams, "id", &id);

    char_url = (char *)json_object_get_string(url);
    char_method = (char *)json_object_get_string(method);
    char_type = (char *)json_object_get_string(type);
    char_id = (char *)json_object_get_string(id);
    char_token = (char *)json_object_get_string(token);
    compute_cookies(response, cookies);
    sprintf(form_data, "raspuns1=omul&raspuns2=numele&id=%s", char_id);
    message = compute_request(char_method, "185.118.200.35", char_url, form_data, char_type, cookies, char_token);
    return message;
}

char *compute_task4(char *response, char *token)
{
    struct json_object *response_to_json;
    struct json_object *method;
    struct json_object *type;
    struct json_object *url;
    char *message = calloc(BUFLEN, sizeof(char));
    char *char_url = calloc(BUFLEN, sizeof(char));
    char *char_type = calloc(BUFLEN, sizeof(char));
    char *char_method = calloc(BUFLEN, sizeof(char));
    char *cookies = calloc(BUFLEN, sizeof(char));

    response_to_json = json_tokener_parse(strchr(response, '{'));
    json_object_object_get_ex(response_to_json, "method", &method);
    json_object_object_get_ex(response_to_json, "url", &url);
    json_object_object_get_ex(response_to_json, "type", &type);

    char_url = (char *)json_object_get_string(url);
    char_method = (char *)json_object_get_string(method);
    char_type = (char *)json_object_get_string(type);

    compute_cookies(response, cookies);
    message = compute_request(char_method, "185.118.200.35", char_url, NULL, char_type, cookies, token);
    return message;
}

char *compute_task5_1(char *response, char *token, char *ip)
{
    struct json_object *response_to_json;
    struct json_object *method;
    struct json_object *method_api;
    struct json_object *type;
    struct json_object *url;
    struct json_object *data;

    struct json_object *queryParams;
    struct json_object *city;
    struct json_object *url_api;
    struct json_object *appid;

    char *message = calloc(BUFLEN, sizeof(char));
    char *form_data = calloc(BUFLEN, sizeof(char));
    char *char_type = calloc(BUFLEN, sizeof(char));
    char *char_method_api = calloc(BUFLEN, sizeof(char));
    char *cookies = calloc(BUFLEN, sizeof(char));
    char *char_url_api = calloc(BUFLEN, sizeof(char));
    char *char_city = calloc(BUFLEN, sizeof(char));
    char *char_appid = calloc(BUFLEN, sizeof(char));
    char *hostname = calloc(BUFLEN, sizeof(char));

    response_to_json = json_tokener_parse(strchr(response, '{'));

    json_object_object_get_ex(response_to_json, "method", &method);
    json_object_object_get_ex(response_to_json, "url", &url);
    json_object_object_get_ex(response_to_json, "type", &type);
    json_object_object_get_ex(response_to_json, "data", &data);
    json_object_object_get_ex(data, "queryParams", &queryParams);
    json_object_object_get_ex(data, "url", &url_api);
    json_object_object_get_ex(data, "method", &method_api);
    json_object_object_get_ex(queryParams, "q", &city);
    json_object_object_get_ex(queryParams, "APPID", &appid);

    char_type = (char *)json_object_get_string(type);

    char_url_api = (char *)json_object_get_string(url_api);
    char_city = (char *)json_object_get_string(city);
    char_appid = (char *)json_object_get_string(appid);
    char_method_api = (char *)json_object_get_string(method_api);
    char *urlf = strchr(char_url_api, '/');

    for (int i = 0; char_url_api[i] != '/'; i++)
    {
        hostname[i] = char_url_api[i];
    }

    hostname_to_ip(hostname, ip);
    compute_cookies(response, cookies);

    sprintf(form_data, "q=%s&appid=%s", char_city, char_appid);

    message = compute_request(char_method_api, "185.118.200.35", urlf, form_data, char_type, cookies, token);

    return message;
}
