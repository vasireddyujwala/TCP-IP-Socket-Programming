# TCP-IP-Socket-Programming
## Objective
The objective of this first programming project is to learn TCP iterative client-server interaction using socket interface in C programming language. After completing the project, you will have a basic understanding of the steps required to develop a networking application.
## Project Specification
In this project, you are required to do socket programming in C programming to implement a pair of client and server that can achieve simple password verification to indicate the account number and balance. Your client will send a pair of user id and password to your server and your server will verify whether the pair of user id and password is legitimate or not for retrieving the account number and balance. Assume the only legitimate pairs of user id and passwords that will be accepted by your server are as follows:
![table](https://github.com/vasireddyujwala/TCP-IP-Socket-Programming/assets/92040231/0570957e-42e6-4358-8637-6e9cdf11cef9)

For example. If the Client sends a pair of user id and password values as: "davi0027" and "Crc51RqV ", the server retrieves the Account number as 14632873 and Balance as 70,000
![client_server](https://github.com/vasireddyujwala/TCP-IP-Socket-Programming/assets/92040231/9f2806de-c361-4550-9015-cd0eb138b655)

Your client and server programs entail to achieve the following requirements:
1. Your client program needs to take two arguments that specify the name of server and the port that it is trying to connect to. Your program for server needs to take an argument that specifies the port that it is listening to. You can use (5000+last 4 digits of your student-id number) to avoid requesting same port by multiple students.
2. The server program will start first and keep listening to the specified port. Your client will connect to the port that your server is listening to, and a socket between your client and server is constructed.
3. After successful startup, the server program will ask the clients to join the server for retrieving the Account number and Balance.
4. Your client program will first prompt a welcome message that asks the user to enter a user id using the keyboard. This user id will then be sent to the server. Then, your server, after receiving the user id from your client, will send an acknowledgment message to the client.
5. Your client, after receiving the acknowledgment message from your server, will prompt a message that asks the user to enter the corresponding password. This password will then be sent to the server. Then, your server, after receiving the password from your client, will verify the received pair of user id and password against the list of legitimate pairs. If the result is positive, the server will send a success message along with the account number and balance for the user id to the client. If the result is negative, the server will send a failure message to the client.
6. Your client, after receiving the result message (i.e., account number and balance), will print out the result. Then, the client sends “QUIT” message to close the socket. Your server will close the socket following the client and keep listening for the next client request.
7. Your server will close the socket after waiting for the user id or the password for 30 seconds.
