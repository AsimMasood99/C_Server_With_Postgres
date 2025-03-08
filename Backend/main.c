// Utility Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

// Postgres library
#include <libpq-fe.h>

// Socket related libraries
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

// CJSON
#include <cjson/cJSON.h>

#define PORT 8080

int varify_credentials(PGconn *conn, cJSON *body) {
    char *query = malloc(256);
    cJSON *username = cJSON_GetObjectItem(body, "username");
    cJSON *password = cJSON_GetObjectItem(body, "password");

    sprintf(query, "SELECT * FROM Users WHERE Username = '%s'",
            username->valuestring);

    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("No data retrieved\n");
        PQclear(res);
        free(query);
        cJSON_free(username);
        cJSON_free(password);
        return 0;
    }

    int rows = PQntuples(res);
    if (rows > 0) {
        for (int i = 0; i < rows; i++) {
            if (strcmp(password->valuestring, PQgetvalue(res, i, 2)) == 0) {
                free(query);
                cJSON_free(username);
                cJSON_free(password);
                return 1;
            }
        }
    }

    free(query);
    cJSON_free(username);
    cJSON_free(password);
    return 0;
}

int main() {
    int server_sockfd, client_sockfd, len;
    struct sockaddr_in server_addr, client_addr;

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_sockfd < 0) {
        perror("Error creating socket\n");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sockfd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) > 0) {
        perror("Error binding socket\n");
        exit(1);
    }

    if (listen(server_sockfd, 10) < 0) {
        perror("Couldn't listen on socket\n");
        exit(1);
    }

    printf("Listning on port %d ...\n", PORT);

    PGconn *conn = PQconnectdb("user=postgres dbname=testdb");

    if (PQstatus(conn) == CONNECTION_BAD) {
        fprintf(stderr, "Connection to database failed: %s\n",
                PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    } else {
        printf("Connected to Postgres ... \n");
    }

    int buffer_len = 1024;
    char *buffer = malloc(buffer_len);
    int bytes_received = 0;

    while (1) {
        len = sizeof(client_addr);
        client_sockfd =
            accept(server_sockfd, (struct sockaddr *)&client_addr, &len);
        if (client_sockfd < 0) {
            perror("Error accepting incoming connection\n");
            continue;  // should there be exit(1) here. ??
        }

        printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));

        while (1) {
            memset(buffer, 0, buffer_len);
            bytes_received = recv(client_sockfd, buffer, buffer_len - 1, 0);

            if (bytes_received <= 0) {
                if (bytes_received == 0) {
                    printf("Client disconnected\n");
                } else {
                    perror("Error receiving data");
                }

                close(client_sockfd);
                printf("Client disconnected  %d\n", client_sockfd);
                break;
            }
            buffer[bytes_received] = '\0';
            printf("Client said: \n%s\n", buffer);

            char *request_body = strstr(buffer, "\r\n\r\n");
            if (request_body) {
                request_body += 4;
            } else {
                printf("Error: Could not find JSON data in the request.\n");
                close(client_sockfd);
                break;
            }

            printf("Extracted JSON: %s\n", request_body);
            cJSON *request_body_JSON = cJSON_Parse(request_body);

            int varification_res = varify_credentials(conn, request_body_JSON);
            char varification_res_c[10];
            snprintf(varification_res_c, 10, "%d", varification_res);
            // Http request and responce formats (REF)
            // https://developer.mozilla.org/en-US/docs/Web/HTTP/Messages

            // Http Responce with headers.
            send(client_sockfd, "HTTP/1.1 200 OK\r\n", 17, 0);
            send(client_sockfd, "Access-Control-Allow-Origin: *\r\n", 32, 0);
            send(client_sockfd, "Content-Length: 1\r\n", 19, 0);
            send(client_sockfd, "Content-Type: text/plain\r\n\r\n", 28, 0);
            // Responce body.
            send(client_sockfd, varification_res_c, 1, 0);
            printf("\n%s\n", varification_res_c);
            printf("Http Responce Sent ...\n");
        }
    }
    PQfinish(conn);
    return 0;
}
