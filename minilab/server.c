/*
   A simple server in the internet domain using TCP
   Usage:./server port (E.g. ./server 10000
*/
#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <stdlib.h>
#include <strings.h>

int byeCheck(const char*);
int helloCheck(const char*);

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

     char buffer[256];

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
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     if (newsockfd < 0)
          error("ERROR on accept");
     while(1){
       bzero(buffer,256);
       //n=recv(newsockfd,buffer,255,0);
       n = read(newsockfd,buffer,255); //Read is a block function. It will read at most 255 bytes
       if (n < 0) error("ERROR reading from socket");

       if (byeCheck(buffer)) {
          printf("Here is the message: %s\n",buffer);

          if (helloCheck(buffer)) {
              n = write(newsockfd,"I got your message",18); //NOTE: write function returns the number of bytes actually sent out �> this might be less than the number you told it to send
              if (n < 0) error("ERROR writing to socket");
            } else {
              n = write(newsockfd,"Welcome!",8);
              if (n < 0) error("ERROR writing to socket");
            }
        } else {
	  close(sockfd);
     	  close(newsockfd);
          break;
        }
      }

     close(sockfd);
     close(newsockfd);

     return 0;
}

int byeCheck(const char* buffer) {
      int notBye = 1;

      if((strlen(buffer) == 4)&&(buffer[0]=='b'||buffer[0]=='B')&&(buffer[1]=='y'||buffer[1]=='Y')&&
          (buffer[2]=='e'||buffer[2]=='E')){
            notBye = 0;
      }
      return notBye;
}

int helloCheck(const char* buffer) {
      int notHello = 1;

      if((strlen(buffer) == 6)&&(buffer[0]=='h'||buffer[0]=='H')&&(buffer[1]=='e'||buffer[1]=='E')&&
        (buffer[2]=='l'||buffer[2]=='L')&&(buffer[3]=='l'||buffer[3]=='L')&&
        (buffer[4]=='o'||buffer[4]=='O')){

            notHello = 0;
      }
      return notHello;
}
