#include <stdio.h>   // Standard Input/Output library
#include <stdlib.h>  // Standard Library
#include <string.h>  // String operations library
#include <unistd.h>  // Unix Standard library
#include <time.h>    // Time library

#if defined(_WIN32) || defined(_WIN64)  // Conditional compilation for Windows
    #include <winsock2.h>         // Windows Socket library
    #pragma comment(lib, "ws2_32.lib")  // Link Windows Socket library
#else
    #include <sys/socket.h>    // Socket library for Unix-like systems
    #include <arpa/inet.h>     // Definitions for internet operations
    #include <netinet/in.h>    // Internet address family
    #include <netdb.h>         // Definitions for network database operations
#endif

#define BUFFER_SIZE 1024       // Maximum buffer size for messages

// Function to create a client socket
int createClientSocket() {     // File descriptor for client socket
    int clientSocket;

    #if defined(_WIN32) || defined(_WIN64)
        WSADATA wsa;           // Windows Socket Data structure
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {   // Initialize Windows Socket subsystem
            printf("[ - ] Error: Windows socket subsystem could not be initialized. Error Code: %d. Exiting...\n", WSAGetLastError());
            exit(EXIT_FAILURE);
        }
    #endif

    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {   // Create socket
        perror("[ - ] Socket creation failed");   // Print error message if creation fails
        exit(EXIT_FAILURE);
    }
    printf("Socket successfully created.\n");     // Print success message
    return clientSocket;     // Return socket file descriptor
}
// Function to connect to a server
int connectToServer(int clientSocket, const char *serverIP, int port) {
    struct sockaddr_in serverAddress;      // Structure to hold server address details
    serverAddress.sin_family = AF_INET;    // Address family (IPv4)

    serverAddress.sin_port = htons(port);  // Set port (host to network byte order)

    #if defined(_WIN32) || defined(_WIN64)
        serverAddress.sin_addr.s_addr = inet_addr(serverIP);   // Convert IP to binary format
    #else
        struct hostent *host = gethostbyname(serverIP);    // Get host details from IP
        if (host == NULL) {
            perror("[ - ] Host not found");     // Print error if host details not found
            exit(EXIT_FAILURE);
        }
        serverAddress.sin_addr = *((struct in_addr *)host->h_addr);   // Set server address
    #endif

    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {   // Connect to server
        perror("[ - ] Connection failed");     // Print error message if connection fails
        exit(EXIT_FAILURE);  
    }
    printf("[ + ] Connected to the server.\n"); // Print success message
    return clientSocket;   // Return socket file descriptor
}

// Function to handle communication with the server
void communicateWithServer(int clientSocket) {
    // Define variables
    int valRead;  // Variable to hold received message length
    int valSend;  // Variable to hold sent message length
    char buffer[BUFFER_SIZE] = {0};  // Buffer to store received message
    char message[BUFFER_SIZE] = {0}; // Buffer to store user input
    time_t start_time, end_time;     // Variables to hold start and end time
    double timeoutSeconds = 30.00;   // Timeout duration in seconds
    // End
    
    // Start loop for communication
    while (1) {
       
        // Receive initial message from server
        memset(buffer, 0, BUFFER_SIZE);  // Clear buffer
        memset(message, 0, BUFFER_SIZE); // Clear message buffer

        valRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);  // Receive message from server
        if (valRead < 0) {  // If receive error
            perror("[ - ] Read error");  // Print error message

            memset(buffer, 0, sizeof(buffer));  // Clear buffer
            break;   // Break loop
        } else if (valRead == 0) {  // If client disconnected
            
            printf("[ - ] Client disconnected.\n");   // Print message
            break;   // Break loop
        } else {     // If message received
            
            printf("[ + ] Received: %s\n", buffer);  // Print received message
            memset(buffer, 0, sizeof(buffer));   // Clear buffer
        }
        // End

        // Handle sending user id
        start_time = time(NULL);  // Record start time
        printf("[ + ] User Id: ");   // Prompt for user ID
        fgets(message, BUFFER_SIZE, stdin);  // Get user input
        end_time = time(NULL);  // Record end time
        double time_difference = difftime(end_time, start_time);  // Calculate time taken

        // Check if input was provided within timeout duration
        if(time_difference<timeoutSeconds){

            valSend = send(clientSocket, message, strlen(message), 0);  // Send user ID to server
            if (valSend < 0) {  // If send error
                perror("[ - ] Send error");  // Print error message
                memset(message, 0, sizeof(message));  // Clear message buffer
                break;  // Break loop
            }
            else if (valSend == 0) {  // If server disconnected
                printf("[ - ] Send error server disconnected.\n");  // Print message
                break;  // Break loop
            }

        }

        // Receive response from server after sending user ID
        valRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);  // Receive message from server
        if (valRead < 0) {  // If receive error
            perror("[ - ] Read error");   // Print error message
            memset(buffer, 0, sizeof(buffer));  // Clear buffer
            break;  // Break loop
        } 
        else if (valRead == 0) {   // If client disconnected
            printf("[ - ] Client disconnected.\n[ - ] Username was not provided within %.0f seconds\n", timeoutSeconds);  // Print message
            break;  // Break loop
        }
        else if (strcmp(buffer, "timeout_exit\n") == 0) {   // If server requests exit
            printf("[ - ] Server requested to close the connection.\n");  // Print message
            break;  // Break loop
        }
        else if(strcmp(buffer, "Ack") == 0){  // If acknowledgement received
            printf("[ + ] Acknowledgement Received\n");  // Print message
            memset(buffer, 0, sizeof(buffer));   // Clear buffer
        }
        else{   // If acknowledgement not received
            printf("[ - ] Acknowledgement Was Not Received %s\n", buffer);
            break;  // Break loop
        }
        // End

        // Handle sending password
        memset(message, 0, sizeof(message));  // Clear message buffer
        start_time = time(NULL);  // Record start time
        printf("[ + ] Password: ");  // Prompt for password
        fgets(message, BUFFER_SIZE, stdin);  // Get user input
        end_time = time(NULL);  // Record end time
        time_difference = difftime(end_time, start_time);  // Calculate time taken

        // Check if input was provided within timeout duration
        if (time_difference < timeoutSeconds) {
            valSend = send(clientSocket, message, strlen(message), 0);  // Send password to server
            if (valSend < 0) {  // If send error
                perror("[ - ] Send error");  // Print error message
                memset(message, 0, sizeof(message));  // Clear message buffer
                break;  // Break loop
            } else if (valSend == 0) {  // If server disconnected
                printf("[ - ] Send error server disconnected.\n");  // Print message
                break;  // Break loop
            }
        }

        // Receive response from server after sending password
        valRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);  // Receive message from server
        if (valRead < 0) {  // If receive error
            perror("[ - ] Read error");  // Print error message
            memset(buffer, 0, sizeof(buffer));  // Clear buffer
            break;  // Break loop
        } else if (valRead == 0) {  // If client disconnected
            printf("[ - ] Client disconnected.\n[ - ] Password was not provided within %.0f seconds\n", timeoutSeconds);  // Print message
            break;  // Break loop
        } else if (strcmp(buffer, "timeout_exit\n") == 0) {  // If server requests exit
            printf("[ - ] Server requested to close the connection.\n");  // Print message
            break;  // Break loop
        }
        // End

         // User validation
        printf("[ + ] Server : %s\n", buffer);  // Print server message
        if (strcmp(buffer, "User Validation Unsuccessful") != 0) {  // If user validation successful
            memset(buffer, 0, BUFFER_SIZE);  // Clear buffer
        }
        // End

        // Handle quit
        while (1) {
            printf("Type `QUIT` to close the connection : ");  // Prompt for quit command
            fgets(message, BUFFER_SIZE, stdin);  // Get user input

            valSend = send(clientSocket, message, strlen(message), 0);  // Send quit command to server
            memset(message, 0, BUFFER_SIZE);  // Clear message buffer
            if (valSend < 0) {  // If send error
                perror("[ - ] Send error");  // Print error message
                break;
            }

            memset(buffer, 0, BUFFER_SIZE);  // Clear buffer
            valRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);  // Receive message from server
            if (valRead == 0) {  // If server closed the connection
                printf("[ - ] Server closed the connection.\n");  // Print message
                break;  // Break loop
            } else if (valRead < 0) {  // If receive error
                perror("[ - ] Received error");  // Print error message
                break;
            }

            if (strcmp(buffer, "exit\n") == 0) {  // If server requests exit
                printf("[ - ] Server requested to close the connection.\n");  // Print message
                break;
            }
        }
        // End
    }
}


// Main function
int main(int argc, char *argv[]) {
    const char *serverIP;  // Variable to hold server IP address
    int port;  // Variable to hold server port number
    if (argc != 3) {  // If insufficient command line arguments provided
        fprintf(stderr, "Usage: %s <server_ip> <port>\nUsing default ip : port [ 127.0.0.1 : 9633 ]\n", argv[0]);  // Print usage message
        serverIP = "127.0.0.1";  // Set default server IP
        port = 9633;  // Set default server port
    } else {  // If command line arguments provided
        serverIP = argv[1];  // Get server IP from command line
        port = atoi(argv[2]);  // Get server port from command line
    }
    
    int clientSocket;  // File descriptor for client socket

    clientSocket = createClientSocket();  // Create client socket
    clientSocket = connectToServer(clientSocket, serverIP, port);  // Connect to server
    communicateWithServer(clientSocket);  // Start communication with server

    #if defined(_WIN32) || defined(_WIN64)
        closesocket(clientSocket);  // Close client socket for Windows
        WSACleanup();  // Clean up Windows Socket subsystem
    #else
        close(clientSocket);  // Close client socket for Unix-like systems
    #endif

    return 0;  // Return 0 to indicate successful execution
}
