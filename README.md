# Server-Restarter

**Server-Restarter** is a lightweight C++ tool that monitors a TCP server's availability. It attempts to connect every 5 minutes and restarts the server automatically if it becomes unresponsive, helping reduce downtime and keep your server online.

## Features

- Periodic connectivity checks (default: every 5 minutes)
- Automatic restart if the server is offline
- Logs events and errors to `server_monitor.log`
- Avoids multiple restarts if the server is already running
- Limits restart attempts to prevent infinite loops
- Handles exceptions to ensure the monitor stays running

## Configuration

You can customize behavior by editing the following constants in the source code:

```cpp
const char* SERVER_IP = "127.0.0.1";         // IP address of the server
const int SERVER_PORT = 12345;               // Port the server listens on
const char* SERVER_PATH = "C:\\Server.exe";  // Full path to the server executable
const int RETRY_INTERVAL_SEC = 300;          // Time between checks (in seconds)
const int RESTART_WAIT_SEC = 5;              // Wait time after restart (in seconds)
const int MAX_RESTART_ATTEMPTS = 3;          // Max restart attempts before abort
