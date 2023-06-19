#include <stdio.h>
#include <string.h>
#include <sys/socket.h> //socket
#include <arpa/inet.h> //inet_addr
#include <pthread.h>

pthread_t threads[3];

//number of client currently connected using the threads
int clientCount = 0;

//here we build a thread function for client
void *server_thread (void *args)
{
	char server_message[2000], client_message[2000];	// Sending values from the server and receive from the server we need this

	memset(server_message,'\0',sizeof(server_message));
	memset(client_message,'\0',sizeof(client_message));

	int newSocket = *((int *)args);

	int clientNum = clientCount;

	clientCount++;

	int clientSockFlag = 0;

	while(clientSockFlag == 0)
	{

		//Receive the message from the client

		if (recv(newSocket, client_message, sizeof(client_message),0) < 0)
		{
			printf("Receive Failed. Error!!!!!\n");
			return NULL;
		}

		printf("\nClient %d connected!\n", clientNum);

		printf("Client Message: %s\n",client_message);


		//Send the message back to client

		strcpy(server_message, client_message);

		if (send(newSocket, server_message, strlen(server_message),0)<0)
		{
			printf("Send Failed. Error!!!!!\n");
			return NULL;
		}

		if((strcmp(client_message,"DISCONNECT") == 0) || (strcmp(client_message,"disconnect") == 0))
		{
			clientSockFlag = 1;
		}

		memset(server_message,'\0',sizeof(server_message));
		memset(client_message,'\0',sizeof(client_message));

	}

	close(newSocket);

	clientCount--;

	pthread_exit(NULL);
}

int main(void)
{
	char server_message[2000], client_message[2000];
	int socket_desc, client_sock, client_size;
	struct sockaddr_in server_addr, client_addr;		//SERVER ADDR will have all the server address

	//Cleaning the Buffers
	memset(server_message,'\0',sizeof(server_message));
	memset(client_message,'\0',sizeof(client_message));	// Set all bits of the padding field//

	//Creating Socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);

	if(socket_desc < 0)
	{
					printf("Could Not Create Socket. Error!!!!!\n");
					return -1;
	}

	printf("Socket Created\n");

	//Binding IP and Port to socket

	server_addr.sin_family = AF_INET;		/* Address family = Internet */
	server_addr.sin_port = htons(2000);		// Set port number, using htons function to use proper byte order */
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");    /* Set IP address to localhost */


	//bind() passes file descriptor, the address structure,and the length of the address structure
	if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr))<0)    // Bind the address struct to the socket.  /
					{
					printf("Bind Failed. Error!!!!!\n");
					return -1;
	}

	printf("Bind Done\n");


	while(1 == 1)
	{
		//Put the socket into Listening State

		//This listen() call tells the socket to listen to the incoming connections.
		// The listen() function places all incoming connection into a "backlog queue" until () call accepts the connection.
		if(listen(socket_desc, 1) < 0)
		{
			printf("Listening Failed. Error!!!!!\n");
			return -1;
		}

		printf("Listening for Incoming Connections.....\n");


		//Accept the incoming Connections
		client_size = sizeof(client_addr);
		client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);          // heree particular client k liye new socket create kr rhaa ha

		if (client_sock < 0)
		{
			printf("Accept Failed. Error!!!!!!\n");
			return -1;
		}

		//inet_ntoa() function converts the Internet host address in, given in network byte order, to a string in IPv4 dotted-decimal notation
		printf("Client Connected with IP: %s and Port No: %i\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

		//we only accept upto 3 connections at a time
		if(clientCount < 3)
		{
			if( pthread_create(&threads[clientCount], NULL, server_thread, &client_sock) != 0 ){
				printf("Failed to create thread\n");
			}

			memset(server_message,'\0',sizeof(server_message));
			memset(client_message,'\0',sizeof(client_message));

		}

		else
		{
			//Receive the message from the client

			if (recv(client_sock, client_message, sizeof(client_message),0) < 0)
			{
				printf("Receive Failed. Error!!!!!\n");
				return -1;
			}

			printf("Client Message: %s\n",client_message);

			//Send the message back to client

			strcpy(server_message, "Server Full!");

			if (send(client_sock, server_message, strlen(server_message),0)<0)
			{
				printf("Send Failed. Error!!!!!\n");
				return -1;
			}

			//Closing the Socket
			close(client_sock);
		}

		memset(server_message,'\0',sizeof(server_message));
		memset(client_message,'\0',sizeof(client_message));
	}

	close(socket_desc);

	return 0;
}
