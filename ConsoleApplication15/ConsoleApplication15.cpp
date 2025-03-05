#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 49151
#define DEFAULT_BUFLEN 512

using namespace std;

int main() {
    setlocale(LC_ALL, "ru");

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "Ошибка WSAStartup!" << endl;
        return 1;
    }

    SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == INVALID_SOCKET) {
        cout << "Ошибка создания сокета: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(ListenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Ошибка привязки: " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        cout << "Ошибка прослушивания: " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    cout << "Сервер запущен! Ожидание подключения..." << endl;

    SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        cout << "Ошибка accept: " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    cout << "Клиент подключен." << endl;

    char recvBuffer[DEFAULT_BUFLEN];
    while (true) {
        memset(recvBuffer, 0, DEFAULT_BUFLEN);
        int recvResult = recv(ClientSocket, recvBuffer, DEFAULT_BUFLEN, 0);

        if (recvResult > 0) {
            int number = atoi(recvBuffer);
            number++;
            string response = to_string(number);
            send(ClientSocket, response.c_str(), response.size(), 0);
        }
        else {
            cout << "Клиент отключился." << endl;
            break;
        }
    }

    closesocket(ClientSocket);
    closesocket(ListenSocket);
    WSACleanup();
    return 0;
}
