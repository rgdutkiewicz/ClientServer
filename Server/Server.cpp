// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <windows.h>

using namespace std;
#define BUFFER_SIZE 512


int main()
{
	cout << "Creating an instance of a named pipe..." << endl;

	// Create a pipe to send data
	HANDLE outbound_pipe = CreateNamedPipe(
		"\\\\.\\pipe\\server_pipe", // name of the pipe
		PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, // 2-way pipe, read and write
		PIPE_TYPE_MESSAGE, // Pipe is message-type
		1, // only allow 1 instance of this pipe
		0, // no outbound buffer
		0, // no inbound buffer
		0, // use default wait time
		NULL // use default security attributes
		);

	if (outbound_pipe == NULL || outbound_pipe == INVALID_HANDLE_VALUE) {
		wcout << "Failed to create outbound pipe instance.";
		system("pause");
		return 1;
	}

	wcout << "Waiting for a client to connect to the pipe..." << endl;

	// This call blocks until a client process connects to the pipe
	BOOL result = ConnectNamedPipe(outbound_pipe, NULL);
	if (!result) {
		wcout << "Failed to make connection on named pipe." << endl;
		CloseHandle(outbound_pipe); // close the pipe
		system("pause");
		return 1;
	}

	// Wait for a message from the client
	char buff[BUFFER_SIZE];
	DWORD numRead = 1;
	while (1) {
		bool success = ReadFile(outbound_pipe, &buff, BUFFER_SIZE, &numRead, NULL);
		if (success) {
			printf( "%s\n", buff );
			if (buff == NULL) {
				break;
			}
		}
		else
		{
			_tprintf(TEXT("ReadFile failed. GLE=%d\n"), GetLastError());
			break;
		}
	}

	// Close the pipe (automatically disconnects client too)
	CloseHandle(outbound_pipe);

	cout << "Done" << endl;

    return 0;
}

