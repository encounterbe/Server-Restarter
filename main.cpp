#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>

#pragma comment(lib, "ws2_32.lib")

// === CONFIGURATION ===
const char* SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 12345;
const char* SERVER_PATH = "C:\\Server.exe";
const int RETRY_INTERVAL_SEC = 300;
const int RESTART_WAIT_SEC = 5;
const int MAX_RESTART_ATTEMPTS = 3;
const char* LOG_FILE = "server_monitor.log";

// === GLOBALS ===
std::mutex logMutex;
std::atomic<int> restartAttempts(0);

// === LOGGING ===
void log(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    std::ofstream logFile(LOG_FILE, std::ios::app);
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    logFile << "[" << std::ctime(&now) << "] " << message << std::endl;
}

// === SERVER CHECK ===
bool isServerOnline() {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    bool online = false;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0) {
        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock != INVALID_SOCKET) {
            sockaddr_in serverAddr = {};
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(SERVER_PORT);
            inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

            if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) != SOCKET_ERROR) {
                online = true;
            }
        }
    }

    if (sock != INVALID_SOCKET) closesocket(sock);
    WSACleanup();
    return online;
}

// === PROCESS DETECTION ===
bool isProcessRunning(const char* processName) {
    bool found = false;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 pe32 = {};
        pe32.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hSnapshot, &pe32)) {
            do {
                if (_stricmp(pe32.szExeFile, processName) == 0) {
                    found = true;
                    break;
                }
            } while (Process32Next(hSnapshot, &pe32));
        }
        CloseHandle(hSnapshot);
    }
    return found;
}

// === RESTART SERVER ===
bool restartServer() {
    if (restartAttempts >= MAX_RESTART_ATTEMPTS) {
        log("[ABORT] Max restart attempts reached.");
        return false;
    }

    log("[INFO] Attempting to restart the server...");

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (!CreateProcessA(SERVER_PATH, nullptr, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
        log("[ERROR] Failed to start server. Error: " + std::to_string(GetLastError()));
        return false;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    restartAttempts++;
    log("[INFO] Server process launched.");
    return true;
}

// === MONITOR LOOP ===
void ServerMonitor() {
    while (true) {
        try {
            if (isServerOnline()) {
                log("[OK] Server is online.");
                restartAttempts = 0;
            } else {
                log("[WARN] Server is offline.");
                if (!isProcessRunning("Server.exe")) {
                    if (!restartServer()) {
                        log("[FAIL] Could not restart server.");
                    }
                } else {
                    log("[INFO] Server process is already running.");
                }
                std::this_thread::sleep_for(std::chrono::seconds(RESTART_WAIT_SEC));
                if (isServerOnline()) {
                    log("[INFO] Server restarted successfully.");
                } else {
                    log("[ERROR] Server still offline after restart.");
                }
            }

            std::this_thread::sleep_for(std::chrono::seconds(RETRY_INTERVAL_SEC));
        }
        catch (const std::exception& ex) {
            log(std::string("[EXCEPTION] ") + ex.what());
        }
        catch (...) {
            log("[EXCEPTION] Unknown crash caught.");
        }
    }
}

int main() {
    log("=== Server Monitor Started ===");
    try {
        ServerMonitor();
    } catch (const std::exception& ex) {
        log(std::string("[FATAL] ") + ex.what());
    } catch (...) {
        log("[FATAL] Unknown error occurred.");
    }
    return 0;
}
