#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <windows.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 49151
#define SERVER_IP "127.0.0.1"

using namespace std;

bool isServerRunning() {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) return false;

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    bool connected = (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) != SOCKET_ERROR);
    closesocket(sock);
    return connected;
}

void startServer() {
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (!CreateProcess(L"Server.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        cout << "Ошибка запуска сервера!" << endl;
    }
    else {
        cout << "Сервер запущен!" << endl;
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

int main() {
    setlocale(LC_ALL, "ru");

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "Ошибка WSAStartup!" << endl;
        return 1;
    }

    if (!isServerRunning()) {
        cout << "Сервер не найден, запускаем..." << endl;
        startServer();
        Sleep(2000); // Ждём 2 сек, чтобы сервер успел запуститься
    }

    SOCKET ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ClientSocket == INVALID_SOCKET) {
        cout << "Ошибка создания сокета!" << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    if (connect(ClientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Ошибка подключения!" << endl;
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    cout << "Подключено к серверу!" << endl;

    string input;
    while (true) {
        cout << "Введите число (или 'exit' для выхода): ";
        cin >> input;

        if (input == "exit") break;

        send(ClientSocket, input.c_str(), input.size(), 0);
        char buffer[512] = { 0 };
        recv(ClientSocket, buffer, sizeof(buffer), 0);
        cout << "Ответ от сервера: " << buffer << endl;
    }

    closesocket(ClientSocket);
    WSACleanup();
    return 0;
}
