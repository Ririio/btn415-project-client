#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <iostream>
#include <thread>
#include <string>

#pragma comment(lib, "ws2_32.lib") // Link Winsock library

#define PORT 8080
#define BUFFER_SIZE 1024



void receiveMessages(SOCKET clientSocket) {
    char buffer[BUFFER_SIZE];

    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytesReceived <= 0) {  // Handle errors and connection closure
        if (bytesReceived == 0) {
            std::cout << "Connection closed by server.\n";
        }
        else {
            std::cerr << "Error receiving data from server.\n";
        }
    }

    buffer[bytesReceived] = '\0'; // Null-terminate safely
    std::cout << "   " << buffer;

}
void printDevice(SOCKET clientSocket, std::string device, int numDevices) {
    char message[BUFFER_SIZE];

    for (int i = 0; i < numDevices; i++) {
        std::string temp = "GET " + device + "/" + std::to_string(i) + "/";
        std::strcpy(message, temp.c_str());
        send(clientSocket, message, strlen(message), 0);
        receiveMessages(clientSocket);
    }
}
int main() {
    int NUM_LIGHTS = 10;
    int NUM_THERMOSTATS = 1;
    int NUM_SPEAKERS = 4;
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }

    char message[BUFFER_SIZE];
    SOCKET clientSocket;
    sockaddr_in serverAddr;

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed.\n";
        WSACleanup();
        return 1;
    }

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(PORT);

    // Connect to server
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed.\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    int hasExited = false;

    std::cout << "Connected to server\n";

    while (!hasExited) {
        std::cout << "Select Device: \n"
            << "    1. Lights\n"
            << "    2. Thermostat\n"
            << "    3. Speakers\n"
            << "    4. Exit\n";
        int choice = 0;
        std::string temp;
        int i = 0;
        while (!(std::cin >> choice || choice < 1 || choice > 4)) {  // Check if input fails
            std::cin.clear();  // Clear error flags
            std::cin.ignore(1000, '\n');  // Ignore invalid input
            std::cout << "Invalid input\n";


        }
        switch (choice) {
        case 1:
            std::cout << "Current status for lights: \n";
            printDevice(clientSocket, "lights", NUM_LIGHTS);
            while (true) {
                std::cout << "Select a light number to switch on/off: \n";
                std::cout << "Enter an out of range number to exit: \n";
                while (!(std::cin >> choice || choice < 0)) {  // Check if input fails
                    std::cin.clear();  // Clear error flags
                    std::cin.ignore(1000, '\n');  // Ignore invalid input
                    std::cout << "Invalid input\n";
                }
                if (choice > NUM_LIGHTS) {
                    break;
                };
                std::string temp = "GET lights/" + std::to_string(choice) + "/switch/";
                std::strcpy(message, temp.c_str());
                send(clientSocket, message, strlen(message), 0);
                receiveMessages(clientSocket);
                std::cout << "Current status for lights: \n";
                printDevice(clientSocket, "lights", NUM_LIGHTS);
            }

            break;
        case 2:
            std::cout << "Current status for thermostats: \n";
            printDevice(clientSocket, "thermostats", NUM_THERMOSTATS);
            while (true) {
                std::cout << "Enter thermostat number to interact with:\n";
                std::cout << "Enter an out of range number to exit: \n";
                while (!(std::cin >> choice || choice < 0)) {  // Check if input fails
                    std::cin.clear();  // Clear error flags
                    std::cin.ignore(1000, '\n');  // Ignore invalid input
                    std::cout << "Invalid input\n";
                }
                if (choice > NUM_THERMOSTATS) {
                    break;
                };
                int choice2;
                std::cout << "Thermostat selected" + std::to_string(choice) + ": \n";
                std::cout << "  1. Switch On/Off\n";
                std::cout << "  2. Set Temp\n";
                while (!(std::cin >> choice2 || choice2 < 0)) {  // Check if input fails
                    std::cin.clear();  // Clear error flags
                    std::cin.ignore(1000, '\n');  // Ignore invalid input
                    std::cout << "Invalid input\n";
                }
                if (choice2 == 1) {
                    std::string temp = "GET thermostats/" + std::to_string(choice) + "/switch/";
                    std::strcpy(message, temp.c_str());
                    send(clientSocket, message, strlen(message), 0);
                    receiveMessages(clientSocket);
                }
                else if (choice2 == 2) {
                    float tempInput;
                    std::cout << "Enter temperature to set:\n";
                    while (!(std::cin >> tempInput || tempInput < 0)) {  // Check if input fails
                        std::cin.clear();  // Clear error flags
                        std::cin.ignore(1000, '\n');  // Ignore invalid input
                        std::cout << "Invalid input\n";
                    }
                    std::string temp = "GET thermostats/" + std::to_string(choice) + "/setTemp/" + std::to_string(tempInput) + "/";
                    std::strcpy(message, temp.c_str());
                    send(clientSocket, message, strlen(message), 0);
                    receiveMessages(clientSocket);
                }
                std::cout << "Current status for thermostats: \n";
                printDevice(clientSocket, "thermostats", NUM_THERMOSTATS);
            }
            break;
        case 3:
            std::cout << "Current status for speakers: \n";
            printDevice(clientSocket, "speakers", NUM_SPEAKERS);
            while (true) {
                std::cout << "Enter speaker number to interact with\n";
                std::cout << "or enter an out of range number to exit: \n";
                while (!(std::cin >> choice || choice < 0)) {  // Check if input fails
                    std::cin.clear();  // Clear error flags
                    std::cin.ignore(1000, '\n');  // Ignore invalid input
                    std::cout << "Invalid input\n";
                }
                if (choice > NUM_SPEAKERS) {
                    break;
                };
                int choice2;
                std::cout << "Speaker selected" + std::to_string(choice) + ": \n";
                std::cout << "  1. Switch On/Off\n";
                std::cout << "  2. Set Volume\n";
                while (!(std::cin >> choice2 || choice2 < 0)) {  // Check if input fails
                    std::cin.clear();  // Clear error flags
                    std::cin.ignore(1000, '\n');  // Ignore invalid input
                    std::cout << "Invalid input\n";
                }
                if (choice2 == 1) {
                    std::string temp = "GET speakers/" + std::to_string(choice) + "/switch/";
                    std::strcpy(message, temp.c_str());
                    send(clientSocket, message, strlen(message), 0);
                    receiveMessages(clientSocket);
                }
                else if (choice2 == 2) {
                    int tempInput;
                    std::cout << "Enter volume to set:\n";
                    while (!(std::cin >> tempInput || tempInput < 0)) {  // Check if input fails
                        std::cin.clear();  // Clear error flags
                        std::cin.ignore(1000, '\n');  // Ignore invalid input
                        std::cout << "Invalid input\n";
                    }
                    std::string temp = "GET speakers/" + std::to_string(choice) + "/setVolume/" + std::to_string(tempInput) + "/";
                    std::strcpy(message, temp.c_str());
                    send(clientSocket, message, strlen(message), 0);
                    receiveMessages(clientSocket);
                }
                std::cout << "Current status for speakers: \n";
                printDevice(clientSocket, "speakers", NUM_SPEAKERS);
            }
            break;
        case 4:
            hasExited = true;
            std::strcpy(message, "exit");
            send(clientSocket, message, strlen(message), 0);
            break;
        }
    }

    // Cleanup
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
