#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "fileio.h"

#define PORT 60001
#define REQ_BUFFER 30000

#define STATUS_OK "HTTP/1.1 200 OK\n"
#define STATUS_BAD "HTTP/1.1 400 Bad Request\n"
#define STATUS_NOT_FOUND "HTTP/1.1 404 Not Found\n"

#define CONTENT_TEXT "Content-Type: text/plain\n"
#define CONTENT_HTML "Content-Type: text/html\n"
#define CONTENT_JPEG "Content-Type: image/jpeg\n"
#define CONTENT_PNG "Content-Type: image/png\n"

#define CONTENT_LENGTH "Content-Length: "

#define BLANK_LINE "\r\n"

#define GET "GET"

#define ROOT "/"

#define HTML "html"
#define JPEG "jpeg"
#define PNG "png"

#define REQUEST_ERROR "Invalid GET Request\n"
#define REQUEST_ERROR_SIZE "20\n"

#define NOT_FOUND_ERROR "File Not Found\n"
#define NOT_FOUND_ERROR_SIZE "15\n"

void handleRequest(char *request, char *response) {
    if (!strlen(request)) return;

    char *token = strtok(request, " ");

    // make sure request method is GET
    if (strcmp(token, GET)) {
        strcat(response, STATUS_BAD);
        strcat(response, CONTENT_TEXT);
        strcat(response, CONTENT_LENGTH);
        strcat(response, REQUEST_ERROR_SIZE);
        strcat(response, BLANK_LINE);
        strcat(response, REQUEST_ERROR);
        return;
    } else {
        token = strtok(NULL, " ");
        char route[FILE_BUFFER_SIZE] = "\0";
        char routeTemp[FILE_BUFFER_SIZE] = "\0";
        char content[FILE_BUFFER_SIZE] = "\0";
        
        // if route is root serve index.html
        if(!strcmp(token, ROOT)) strcpy(route, "/index.html");
        else strcpy(route, token);
        strcpy(routeTemp, route);

        // get file type
        char *type = strtok(routeTemp, ".");
        type = strtok(NULL, ".");

        char fp[REQ_BUFFER] = ".";
        FILE *file;
        file = fopen(fp, "r");
        strcat(fp, route);
        if (!strcmp(type, HTML) && file != NULL) {
            fclose(file);
            setFile(fp);
            strcpy(content, CONTENT_HTML);
            // get file length
            int fileLength = getFileLength();
            char size[FILE_BUFFER_SIZE] = "\0";
            sprintf(size, "%i", getFileLength());

            // get file text
            char *buffer = malloc(fileLength * sizeof(char));
            buffer[0] = '\0';
            getText(buffer);

            // build response
            strcat(response, STATUS_OK);
            strcat(response, content);
            strcat(response, CONTENT_LENGTH);
            strcat(response, size);
            strcat(response, "\n");
            strcat(response, BLANK_LINE);
            strcat(response, buffer);
            strcat(response, "\n");
            free(buffer);
            closeFile();
        } else if (!strcmp(type, JPEG)) {
            strcpy(content, CONTENT_JPEG);
        } else if (!strcmp(type, PNG) && file != NULL) {
            char buffer[REQ_BUFFER];
            fseek(file, 0L, SEEK_END);
            int sz = ftell(file);
            rewind(file);
            char size[FILE_BUFFER_SIZE] = "\0";
            sprintf(size, "%i", sz);
            fread(buffer, sizeof(char), sz, file);
            strcpy(content, CONTENT_PNG);
            strcat(response, STATUS_OK);
            strcat(response, content);
            strcat(response, CONTENT_LENGTH);
            strcat(response, size);
            strcat(response, "\n");
            strcat(response, BLANK_LINE);
            strcat(response, buffer);
            strcat(response, "\n");
            fclose(file);
        } else {
            // build 404 response
            strcat(response, STATUS_NOT_FOUND);
            strcat(response, CONTENT_TEXT);
            strcat(response, CONTENT_LENGTH);
            strcat(response, NOT_FOUND_ERROR_SIZE);
            strcat(response, BLANK_LINE);
            strcat(response, NOT_FOUND_ERROR);
            fclose(file);
        }
    }
}

int main() {
    // create the socket
    int server;
    if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("cannot create socket"); 
        return 0; 
    }

    // construct socket address
    struct sockaddr_in localhost;
    int addrlen = sizeof(localhost);

    memset((char *) &localhost, 0, sizeof(localhost));
    localhost.sin_family = AF_INET;
    localhost.sin_addr.s_addr = htonl(INADDR_ANY);
    localhost.sin_port = htons(PORT);

    // bind address to socket
    if (bind(server, (struct sockaddr *)&localhost, sizeof(localhost)) < 0) {
        perror("failed to bind to the socket");
        return 0;
    }

    // start listening
    if (listen(server, 3) < 0) 
    { 
        perror("Error listening"); 
        return 0;
    }

    printf("\nServer listening on port %i\n\n", PORT);
    while(1)
    {
        int newSocket;

        // accept connections for requests
        if ((newSocket = accept(server, (struct sockaddr *)&localhost, (socklen_t*)&addrlen))<0)
        {
            perror("Error accepting connection");
            return 0;
        }
        
        // create buffers
        char request[REQ_BUFFER] = {0};
        char response[REQ_BUFFER] = "\0";

        // read request
        read(newSocket, request, REQ_BUFFER);
        printf("REQUEST:\n%s\n", request);

        // handle request and respond
        handleRequest(request, response);
        write(newSocket, response , strlen(response));
        printf("RESPONSE:\n%s\n", response);

        // close connection
        close(newSocket);
    }
    return 0;
}