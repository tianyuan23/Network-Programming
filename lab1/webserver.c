/*
 develop a Web server in C, get to know HTTP request and respone work
*/
#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void parseAndRespond(char*, int);
void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd; //descriptors rturn from socket and accept system calls
     int portno; // port number
     socklen_t clilen;

     char buffer[1000000];

     /*sockaddr_in: Structure Containing an Internet Address*/
     struct sockaddr_in serv_addr, cli_addr;

     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }

     /*Create a new socket
       AF_INET: Address Domain is Internet
       SOCK_STREAM: Socket Type is STREAM Socket */
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");

     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]); //atoi converts from String to Integer
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY; //for the server the IP address is always the address that the server is running on
     serv_addr.sin_port = htons(portno); //convert from host to network byte order

     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //Bind the socket to the server address
              error("ERROR on binding");

     listen(sockfd,5); // Listen for socket connections. Backlog queue (connections to wait) is 5

     clilen = sizeof(cli_addr);
     /*accept function:
       1) Block until a new connection is established
       2) the new socket descriptor will be used for subsequent communication with the newly connected client.
     */
     while (1) {

         newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0)
              error("ERROR on accept");


         bzero(buffer,1000000);
         //n=recv(newsockfd,buffer,1000000,0);
         n = read(newsockfd,buffer,999999); //Read is a block function. It will read at most 999999 bytes
         if (n < 0) error("ERROR reading from socket");

         printf("\n%s\n", buffer);

         parseAndRespond(buffer, newsockfd);
         bzero(buffer, 999999);
         close(newsockfd);
     }

     close(sockfd);
     close(newsockfd);

     return 0;
}

void parseAndRespond(char* buffer, int newsockfd) {

	int i = 0;

	char* strArray[100];
	char responseHTTP[5000] = "HTTP/1.1";
    char fileName[50] = "."; //add . in front of file, because fileName from strArray[i] begins with "/"
	char *tokens = strtok(buffer, " \n\r");
    FILE* file;

	// parse the message
	while(tokens != NULL){
		strArray[i] = malloc(strlen(tokens) + 1);
		strcpy(strArray[i], tokens);
		i++;
		tokens = strtok(NULL, " \n\r");
	}
	int numOfElements = i;

	// check the file the client is trying to GET. Checks to see if it has proper extension
    i = 0;
	while(i < numOfElements){
		if(strstr(strArray[i], ".html") || strstr(strArray[i], ".jpg") ||
        strstr(strArray[i], ".gif") || strstr(strArray[i], ".jpeg")){
			strcat(fileName, strArray[i]);
    		strcat(responseHTTP, " 200 OK");
            break;
		}
		i++;
	}

    if(i == numOfElements){
        strcat(responseHTTP, " 404 Not Found");
    }


	strcat(responseHTTP, "\r\n");


	// add content type to response based on what file is
	i = 0;
	while(i < numOfElements){
		if(strstr(strArray[i], ".html")){
			strcat(responseHTTP, "Content-Type: text/html");
			strcat(responseHTTP, "\r\n");
			break;
		}else if(strstr(strArray[i], ".jpg")){
			strcat(responseHTTP, "Content-Type: image/jpeg");
			strcat(responseHTTP, "\r\n");
			break;
		}else if(strstr(strArray[i], ".gif")){
			strcat(responseHTTP, "Content-Type: image/gif");
			strcat(responseHTTP, "\r\n");
			break;
		}
	i++;
	}

	strcat(responseHTTP, "\r\n");
    write(newsockfd, responseHTTP, strlen(responseHTTP));


	printf("The HTTP response is: \n");
	printf("%s", responseHTTP);

	// open file
	if (file = fopen(fileName, "rb")){
    	char data[256];
    	int sent = fread(data, 1, 255, file);

    	// send file to web brower
    	while(sent > 0){
            write(newsockfd, data, sent);
    		sent = fread(data, 1, 255, file);
    	}
    } else {
        write(newsockfd, "404 File Not Found", 18);
    }

	fclose(file);

}
