# ECE463 Intro to Computer Communication Networks project
# Phase 3: Decentralized P2P Network

## Directories/files
src:        Directory containing all source code
.gitignore: File listings all files and directories to ignore when committing changes via Git
makefile:   See Compilation section below
Flowcharts: Directory containing flowcharts that illustrate the logical flow of our project.  
The flowcharts were made and can be modified with [draw.io](https://app.diagrams.net/).

## Usage (May be different for this phase)
$ %put <file name>
Send a file to the server.

$ %get <file name>
Request a file stored on the remote server.

$ <any plain text message>
Will transmit message to any other clients connected to the server.

## Compilation
This project uses make for compilation. Enter "make" to compile the program, "make clean" to remove all object  
files and executables. Any files/directories that are created when compiling are listed in the gitignore.

## Requirements
- Server provides user directory. Contains all user usernames, IP addresses, and statuses.
- Server provides resource directory. Contains resource names and owners.
- Server sends hello message to client to check its liveness. Done periodically to check "liveness"
- All communication between client and server is done through UDP.
- Do not implement communication between clients. Will be implemented in the next stage.
- Optional: Clients can take a script for testing purposes.

