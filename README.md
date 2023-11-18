# Project "MultyChat"

Simple multi-user chat application using sockets.

## Compilation and Execution

### Server

1. **Compile the server:**
   - Open the command prompt or terminal in the directory with the server code.
   - Use a compiler, for example, g++, to compile the code. Example for g++ in the command prompt:
     ```bash
     g++ -o server server.cpp -lws2_32
     ```
   - This command will create the executable file `server.exe` (or `server` in the case of Linux).

2. **Run the server:**
   - Execute the compiled server, specifying the port to listen on. Example:
     ```bash
     ./server.exe 1234
     ```

### Client

1. **Compile the client:**
   - Open the command prompt or terminal in the directory with the client code.
   - Use a compiler, for example, g++, to compile the code. Example for g++ in the command prompt:
     ```bash
     g++ -o client client.cpp -lws2_32
     ```
   - This command will create the executable file `client.exe` (or `client` in the case of Linux).

2. **Run the client:**
   - Execute the compiled client, specifying the server's IP address and port (default is "127.0.0.1:1234"), and enter a unique username. Example:
     ```bash
     ./client.exe 127.0.0.1 1234
     ```

**Note:** Please ensure that you have a compiler installed and the ws2_32.lib library for compilation and linking.
