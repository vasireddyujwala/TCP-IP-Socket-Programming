#include <stdio.h>            // Standard Input/Output library
#include <stdlib.h>           // Standard Library
#include <string.h>           // String operations library
#include <unistd.h>           // Unix Standard library

#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>     // Windows Socket library
    #pragma comment(lib, "ws2_32.lib")  // Link Windows Socket library
#else
    #include <sys/socket.h>   // Socket library for Unix-like systems
    #include <arpa/inet.h>    // Definitions for internet operations
#endif

#define BUFFER_SIZE 1024       // Maximum buffer size for messages

// Structure to hold user information
struct UserRecord {
    char userId[20];
    char password[20];
    int accountNumber;
    char balance[100];
};

// Function to authenticate user based on username and password
struct UserRecord *authenticateUser(const char *username, const char *password, struct UserRecord users[], int length) {
    for (int i = 0; i < length; i++) {
        if (strcmp(users[i].userId, username) == 0 && strcmp(users[i].password, password) == 0) {
            return &users[i]; // Return pointer to user record
        }
    }
    return NULL; // Return NULL if user not found
}

// Function to create a server socket
int createServerSocket() {
    int serverSocket;  // Variable to hold the server socket file descriptor

    // Conditional compilation for Windows
    #if defined(_WIN32) || defined(_WIN64)
        WSADATA wsa;  // Windows Socket Data structure
        // Initialize Windows Socket subsystem
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
            printf("[ - ] Windows socket subsystem could not be initialized. Error Code: %d. Exiting...\n", WSAGetLastError());
            exit(EXIT_FAILURE);
        }
    #endif

    // Create a socket (IPv4, TCP)
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("[ - ] Socket creation failed");  // Print error message if creation fails
        exit(EXIT_FAILURE);  // Exit with failure status
    }
    printf("[ + ] Socket successfully created.\n");  // Print success message
    return serverSocket;  // Return the server socket file descriptor
}


// Function to set socket options
void setSocketOptions(int serverSocket) {
    int opt = 1; // Define an integer variable 'opt' and set it to 1. This will be used as an option for the socket.

    // Attempt to set the socket option to reuse the address. 
    // This allows the socket to bind to an address even if it is already in use.
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt))) {
        perror("setsockopt failed"); // Print an error message if setting the socket option fails.
        exit(EXIT_FAILURE); // Exit the program with a failure status.
    }
}

// Function to bind socket to a specific port
void bindSocket(int serverSocket, int port) {
    struct sockaddr_in serverAddress;  // Declare a structure to hold server address details.
    serverAddress.sin_family = AF_INET;  // Set the address family to IPv4.
    serverAddress.sin_addr.s_addr = INADDR_ANY;  // Bind the socket to any available local address.
    serverAddress.sin_port = htons(port);  // Set the port number and convert it to network byte order.

    // Attempt to bind the server socket to the specified address and port.
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("[ - ] Bind failed");  // If binding fails, print an error message.
        exit(EXIT_FAILURE);  // Exit the program with a failure status.
    }
    printf("[ + ] Socket successfully binded.\n");  // Print a success message after successful binding.
}
// Function to start listening for incoming connections on the server socket
int listenForConnections(int serverSocket) {
    // Attempt to start listening for incoming connections
    if (listen(serverSocket, 3) < 0) {
        perror("Listen failed"); // Print an error message if listening fails
        exit(EXIT_FAILURE); // Exit the program with a failure status
    }
    printf("[ + ] Server listening...\n"); // Print a success message if listening succeeds
}

// Function to accept incoming connection
int acceptConnection(int serverSocket) {
    struct sockaddr_in clientAddress;  // Declare a structure to hold client address details.
    
    #if defined(_WIN32) || defined(_WIN64)
        int addressLength = sizeof(clientAddress);  // Define the length of the client address for Windows.
    #else
        socklen_t addressLength = sizeof(clientAddress);  // Define the length of the client address for Unix-like systems.
    #endif
    
    int newSocket;  // Declare a variable to hold the new socket for the accepted connection.

    // Attempt to accept an incoming connection and create a new socket for it.
    if ((newSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &addressLength)) < 0) {
        perror("Accept failed");  // If accepting the connection fails, print an error message.
        exit(EXIT_FAILURE);  // Exit the program with a failure status.
    }
    printf("[ + ] Server accepted the client...\n");  // Print a message indicating that the server has accepted the client.
    return newSocket;  // Return the new socket for the accepted connection.
}


void handleClientCommunication(int newSocket) {
    // Define variables
    int valRead; // Variable to store the return value of recv (number of bytes received)
    char buffer[BUFFER_SIZE] = {0}; // Buffer to store received data
    char message[BUFFER_SIZE] = {0}; // Buffer to store outgoing messages
    char userId[BUFFER_SIZE] = {0}; // Buffer to store user ID
    char password[BUFFER_SIZE] = {0}; // Buffer to store password
    int timeoutSeconds = 30; // Time to wait for user input
    struct timeval timeout; // Struct to define timeout for select()
    fd_set readfds; // Set of file descriptors for select()
    timeout.tv_sec = timeoutSeconds; // Set timeout to 30 seconds
    timeout.tv_usec = 0; // Set microseconds to 0
    int result; // Variable to store result of select()
    struct UserRecord users[5]; // Array of UserRecords to store user data
    // End


    // Add data in user
    // Initialize user data for testing
    // Each user has a user ID, password, account number, and balance
    strcpy(users[0].userId, "davi0027");
    strcpy(users[0].password, "Crc51RqV");
    users[0].accountNumber = 14632873;
    strcpy(users[0].balance, "70,000");

    strcpy(users[1].userId, "jack0046");
    strcpy(users[1].password, "Cfw61RqV");
    users[1].accountNumber = 14646987;
    strcpy(users[1].balance, "74,000");

    strcpy(users[2].userId, "bhat0092");
    strcpy(users[2].password, "G6M7p8az");
    users[2].accountNumber = 14666234;
    strcpy(users[2].balance, "77,000");

    strcpy(users[3].userId, "yogi0067");
    strcpy(users[3].password, "uCh781fY");
    users[3].accountNumber = 14693456;
    strcpy(users[3].balance, "80,000");

    strcpy(users[4].userId, "anni0083");
    strcpy(users[4].password, "Pd82bG57");
    users[4].accountNumber = 14677213;
    strcpy(users[4].balance, "60,000");
    // End
    
    // Send an initial message to the client
    strcpy(message, "Please join the server with valid credentials to retrieve the details");
    send(newSocket, message, strlen(message), 0);

     // Reset buffers for re-use
    memset(buffer, 0, BUFFER_SIZE);
    memset(message, 0, BUFFER_SIZE);
    // End

    // Handle user id input
    printf("[ + ] Waiting for %d seconds to receive User Id...\n", timeoutSeconds);

    FD_ZERO(&readfds);  // Clear the set of file descriptors
    FD_SET(newSocket, &readfds);  // Add the new socket to the set

    result = select(newSocket + 1, &readfds, NULL, NULL, &timeout);  // Wait for input on the socket

    if (result == 0) {
        printf("[ + ] No connection occurred within %d seconds.\n", timeoutSeconds);
        send(newSocket, "timeout_exit\n", strlen(message), 0);  // Send timeout message to client
        return;  
    }
    if (FD_ISSET(newSocket, &readfds)){
        valRead = recv(newSocket, buffer, BUFFER_SIZE, 0); // Receive user ID
        if (valRead < 0) {
            perror("[ - ] Read error while reading User Id");
            memset(buffer, 0, sizeof(buffer));
            exit(EXIT_FAILURE);
        } else if (valRead == 0) {
            printf("[ - ] Client disconnected.\n");
            exit(EXIT_FAILURE);
        } else {
            printf("[ + ] Received User Id: %s", buffer);
            strcpy(userId, buffer); // Copy user ID to userId buffer
            memset(buffer, 0, sizeof(buffer)); // Reset buffer for re-use
        }
    }
    // End
    
    // Send acknowledgement after receiving user id
    strcpy(message, "Ack"); // Prepare an acknowledgment message
    send(newSocket, message, strlen(message), 0); // Send acknowledgment to client
    memset(message, 0, BUFFER_SIZE); // Reset message buffer for re-use
    // End

    // Handle password input
    printf("[ + ] Waiting for %d seconds to receive Password...\n", timeoutSeconds);

    FD_ZERO(&readfds); // Clear the set of file descriptors
    FD_SET(newSocket, &readfds); // Add the new socket to the set

    result = select(newSocket + 1, &readfds, NULL, NULL, &timeout); // Wait for input on the socket


   if (result == 0) {
        printf("[ - ] No connection occurred within %d seconds.\n", timeoutSeconds);
        send(newSocket, "timeout_exit\n", strlen(message), 0); // Send timeout message to client
        return;  
    }

    if (FD_ISSET(newSocket, &readfds)){
        valRead = recv(newSocket, buffer, BUFFER_SIZE, 0); // Receive password
        if (valRead < 0) {
            perror("[ - ] Read error while reading Password");
            memset(buffer, 0, sizeof(buffer));
            exit(EXIT_FAILURE);
        } else if (valRead == 0) {
            printf("[ - ] Client disconnected.\n");
            exit(EXIT_FAILURE);
        } else {
            printf("[ + ] Received Password: %s", buffer);
            strcpy(password, buffer); // Copy password to password buffer
            memset(buffer, 0, sizeof(buffer)); // Reset buffer for re-use
        }
    }
    // End
    
   // Handle user authentication
    userId[strcspn(userId, "\n")] = '\0'; // Remove newline character from user ID
    password[strcspn(password, "\n")] = '\0'; // Remove newline character from password
    
    struct UserRecord *authenticatedUser = authenticateUser(userId, password, users, 5); // Authenticate user

    if(authenticatedUser != NULL){ // If authentication is successful
        memset(message, 0, BUFFER_SIZE); // Reset message buffer
        strcpy(message, "User Validation Successful\n"); // Prepare success message
        int valSend = send(newSocket, message, strlen(message), 0); // Send success message
        memset(message, 0, BUFFER_SIZE); // Reset message buffer

        // Prepare and send user account information
        sprintf(message, "[ INFORMATION ] Account Number: %d\n[ INFORMATION ] Balance: %s", 
                authenticatedUser->accountNumber, authenticatedUser->balance);
        valSend = send(newSocket, message, strlen(message), 0);

        // Reset message buffer for re-use (commented out code is redundant)
        // memset(message, 0, BUFFER_SIZE);
        // sprintf(message, "Balance: %d\n", authenticatedUser->balance);
        // send(newSocket, message, strlen(message), 0);
        memset(message, 0, BUFFER_SIZE); // Reset message buffer
    }
   else{ // If authentication fails
        printf("[ - ] Credentials mismatch \n"); // Print failure message
        strcpy(message, "User Validation Unsuccessful"); // Prepare failure message
        int x = send(newSocket, message, strlen(message), 0); // Send failure message
    }
    // End

    // Handle quit prompt
    while(1){ // Loop indefinitely
        memset(buffer, 0, sizeof(buffer)); // Reset buffer for re-use
        valRead = recv(newSocket, buffer, BUFFER_SIZE, 0); // Receive client input
        
        if (valRead < 0) {
            perror("[ - ] Read error while reading input");
            memset(buffer, 0, sizeof(buffer));
            exit(EXIT_FAILURE);
        } else if (valRead == 0) {
            printf("[ - ] Client disconnected.\n");
            memset(buffer, 0, sizeof(buffer));
            exit(EXIT_FAILURE);
        } else if(strcmp(buffer, "QUIT\n") != 0){ // If input is not 'QUIT'
            printf("[ - ] Received : %s", buffer); // Print received input
            memset(message, 0, sizeof(buffer)); // Reset message buffer
            memset(buffer, 0, sizeof(buffer)); // Reset buffer
            strcpy(message, "Ok"); // Prepare response message
            send(newSocket, message, BUFFER_SIZE, 0); // Send response
        }
        else if(strcmp(buffer, "QUIT\n") == 0){ // If input is 'QUIT'
            printf("[ + ] Received : %s", buffer); // Print received input
            memset(buffer, 0, sizeof(buffer)); // Reset buffer
            send(newSocket, "exit\n", BUFFER_SIZE, 0); // Send exit message to client
            break; // Exit loop
        }
    }
    // End
    
}


int main(int argc, char *argv[]) {
    int port; // Declare an integer variable to hold the port number.

    // Check if the number of command-line arguments is not equal to 2.
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\nUsing default port : 9633\n", argv[0]); // Print usage message with default port.
        port = 9633; // Set port to default value (9633).
    } else {
        port = atoi(argv[1]); // Convert the second command-line argument to an integer (port number).
    }

    int serverSocket, newSocket; // Declare variables for server socket and new socket for client connection.

    serverSocket = createServerSocket(); // Create a server socket and store the file descriptor in serverSocket.
    setSocketOptions(serverSocket); // Set socket options (SO_REUSEADDR) for serverSocket.
    bindSocket(serverSocket, port); // Bind the server socket to a specific port.

    listenForConnections(serverSocket); // Start listening for incoming connections on the serverSocket.

    while (1) {
        printf("\n***********************************\n[ + ] Waiting for a client to connect.\n"); // Print a message indicating waiting for a client.
        newSocket = acceptConnection(serverSocket); // Accept an incoming connection and store the new socket in newSocket.
        printf("[ + ] A client got connected.\n"); // Print a message indicating successful connection to a client.
        handleClientCommunication(newSocket); // Handle communication with the connected client.
        printf("[ - ] Closing the connection to the client.\n"); // Print a message indicating closing the connection.

        // Close the socket for the connected client based on the operating system.
        #if defined(_WIN32) || defined(_WIN64)
            closesocket(newSocket);
        #else
            close(newSocket);
        #endif
    }

    // Close the server socket and clean up the Windows socket subsystem (if applicable).
    #if defined(_WIN32) || defined(_WIN64)
        closesocket(serverSocket);
        WSACleanup();
    #else
        close(serverSocket);
    #endif

    return 0; // Indicate successful program execution.
}

