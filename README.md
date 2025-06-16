Server-Restarter
Server-Restarter is a lightweight C++ tool designed to monitor the health of a TCP server by attempting to connect to it every 5 minutes. If the server is offline or unresponsive, the tool will automatically attempt to restart it to minimize downtime and maintain service availability.

Features
Checks server connectivity at regular intervals (default: every 5 minutes)

Automatically restarts the server if it is found to be offline

Logs all events and errors to a local log file (server_monitor.log)

Prevents multiple restarts if the server process is already running

Limits the number of restart attempts to avoid infinite loops

Includes exception handling to prevent the monitor from crashing

Configuration
You can modify the following settings directly in the source code:

cpp
Kopieren
Bearbeiten
const char* SERVER_IP = "127.0.0.1";         // IP address of your server
const int SERVER_PORT = 12345;               // Port your server listens on
const char* SERVER_PATH = "C:\\Server.exe";  // Full path to your server executable
const int RETRY_INTERVAL_SEC = 300;          // Time between checks (in seconds)
const int RESTART_WAIT_SEC = 5;              // Time to wait after restarting (in seconds)
const int MAX_RESTART_ATTEMPTS = 3;          // Max allowed restart attempts before abort


Usage
Open the source file and set your server's IP, port, and executable path.

Compile the program using Visual Studio or a command-line compiler.

Run the compiled executable. It will begin monitoring the server immediately.

Logging
The application logs all activity (status checks, errors, restarts, exceptions) to a file called server_monitor.log. You can use this log to diagnose server issues or confirm uptime.

Notes
The monitor will not start a new instance of the server if it detects that it is already running.

If the server fails to come back online after a limited number of attempts, the restarter will stop retrying until the next interval.

