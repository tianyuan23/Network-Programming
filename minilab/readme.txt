Name: Tianyuan Li
SID: 200450430
Course: CSE3461
Project: Building echo client-server communication on TCP with sockets.

There are 4 files in the zip file: server.c, client.c, Makefile and readme.txt.

Steps:
1. open a ternimal and change to the directory which contains these four files.
2. type 'make' in the command line.
3. open another terminal and change to the same directory.
4. type './server 3000' in a terminal. If the port number is used, change another one greater than 1024.
5. type './client 127.0.0.1 3000' in the other terminal (same port number as server).
6. send messages from client to server.
