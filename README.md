Multithreaded Echo Server and a simple Client.

Written in C language using TCP socket programming.

Up-to 3 clients can connect to the server at a time, and additional connections are blocked by the server.
Each client is serviced via a separate server thread.

Server echoes whatever message the client sends, back to the client.

A client is disconnected from the server when they send "DISCONNECT" as the message.
