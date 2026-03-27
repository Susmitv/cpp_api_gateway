#include <iostream>
#include <winsock2.h>
#include <string>


#pragma comment(lib, "ws2_32.lib") 

using namespace std;

int main() {
    
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "[-] Winsock initialization failed!" << endl;
        return 1;
    }

    
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; 
    serverAddr.sin_port = htons(8000); 

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, SOMAXCONN);

    cout << "[+] High-Performance API Gateway listening on Port 8000..." << endl;
    cout << "[*] Waiting for traffic..." << endl;

    
    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        cout << "\n-----------------------------------" << endl;
        cout << "[!] Naya Request Aaya Browser Se!" << endl;

        
        char buffer[4096] = {0};
        recv(clientSocket, buffer, sizeof(buffer), 0);

     
        cout << "[*] Rate limit check skipped (Abhi ke liye)..." << endl;
        cout << "[*] Forwarding traffic to Python Backend (Port 8080)..." << endl;

        SOCKET backendSocket = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in backendAddr;
        backendAddr.sin_family = AF_INET;
        backendAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
        backendAddr.sin_port = htons(8080); 

      
        if (connect(backendSocket, (struct sockaddr*)&backendAddr, sizeof(backendAddr)) == 0) {
            
           
            send(backendSocket, buffer, strlen(buffer), 0);

          
            char backendResponse[4096] = {0};
            recv(backendSocket, backendResponse, sizeof(backendResponse), 0);
            send(clientSocket, backendResponse, strlen(backendResponse), 0);
            cout << "[+] Success! Backend ka response user ko bhej diya." << endl;
        } else {  
            cout << "[-] Backend down hai bhai! Server chalu kar." << endl;
            string errorResponse = "HTTP/1.1 502 Bad Gateway\r\n\r\nBackend is offline. Start Python server!";
            send(clientSocket, errorResponse.c_str(), errorResponse.length(), 0);
        }
        closesocket(backendSocket);
        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
