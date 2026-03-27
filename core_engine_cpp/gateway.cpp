#include <iostream>
#include <winsock2.h>
#include <string>

// Windows socket library link karne ke liye
#pragma comment(lib, "ws2_32.lib") 

using namespace std;

int main() {
    // 1. Winsock Initialize karna
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "[-] Winsock initialization failed!" << endl;
        return 1;
    }

    // 2. Gateway ka Socket Banana (The Bouncer)
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; 
    serverAddr.sin_port = htons(8000); // Gateway port 8000 pe sunega

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, SOMAXCONN);

    cout << "[+] High-Performance API Gateway listening on Port 8000..." << endl;
    cout << "[*] Waiting for traffic..." << endl;

    // 3. Traffic handle karne ka loop
    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        cout << "\n-----------------------------------" << endl;
        cout << "[!] Naya Request Aaya Browser Se!" << endl;

        // Browser ki request ko read karna
        char buffer[4096] = {0};
        recv(clientSocket, buffer, sizeof(buffer), 0);

        // TODO: Next step mein yahan Upstash Redis ka Rate Limiting logic aayega!
        cout << "[*] Rate limit check skipped (Abhi ke liye)..." << endl;
        cout << "[*] Forwarding traffic to Python Backend (Port 8080)..." << endl;

        // 4. Python Backend se connect karna
        SOCKET backendSocket = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in backendAddr;
        backendAddr.sin_family = AF_INET;
        backendAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Localhost
        backendAddr.sin_port = htons(8080); // Python server ka port

        // Agar Python backend on hai toh request aage bhejo
        if (connect(backendSocket, (struct sockaddr*)&backendAddr, sizeof(backendAddr)) == 0) {
            
            // Request Python ko bheji
            send(backendSocket, buffer, strlen(buffer), 0);

            // Python se response wapas liya
            char backendResponse[4096] = {0};
            recv(backendSocket, backendResponse, sizeof(backendResponse), 0);

            // Wohi response wapas user/browser ko de diya
            send(clientSocket, backendResponse, strlen(backendResponse), 0);
            
            cout << "[+] Success! Backend ka response user ko bhej diya." << endl;
        } else {
            // Agar tune Python server on karna miss kar diya toh yeh error aayega
            cout << "[-] Backend down hai bhai! Server chalu kar." << endl;
            string errorResponse = "HTTP/1.1 502 Bad Gateway\r\n\r\nBackend is offline. Start Python server!";
            send(clientSocket, errorResponse.c_str(), errorResponse.length(), 0);
        }

        // Sockets close karna taaki memory leak na ho
        closesocket(backendSocket);
        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}