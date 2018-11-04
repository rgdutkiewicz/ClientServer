// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include <string>
#include "SharedObject.h"

using namespace std;
#define BUFFER_SIZE 512

class Client {
private:
	HANDLE pipe = NULL;
	OVERLAPPED overlap;
	DWORD  cbRead, cbWritten, dwMode;

public:
	Client() {
		int attempts = 0;
		while (pipe == NULL && attempts < 10) {
			pipe = create_pipe_handle("sync");
			// Retry every 5 seconds
			if (pipe == NULL) {
				WaitNamedPipe("\\\\.\\pipe\\server_pipe", 5000);
			}
			attempts++;
		}
	}

	Client(string mode) {
		// Open the pipe instance, retry up to 10 times if necessary.
		int attempts = 0;
		while (pipe == NULL && attempts < 10) {
			pipe = create_pipe_handle(mode);
			// Retry every 5 seconds
			if (pipe == NULL) {
				WaitNamedPipe("\\\\.\\pipe\\server_pipe", 5000);
			}
			attempts++;
		}
	}

	// Method to create the pipe in either OVERLAPPED mode for asynchronous, or Normal mode for Synchronous
	HANDLE create_pipe_handle(string sync_attr) {
		HANDLE pipe;
		if (sync_attr == "async") {
			pipe = CreateFile(
				"\\\\.\\pipe\\server_pipe",
				GENERIC_READ | GENERIC_WRITE, // bidirectional access
				0,
				NULL,
				OPEN_EXISTING,
				FILE_FLAG_OVERLAPPED, // ASYNC operation
				NULL
				);
			// TODO Create the OVERLAPPED object for asynchronous I/O
			//overlap = 
		}
		else if (sync_attr == "sync")
		{
			pipe = CreateFile(
				"\\\\.\\pipe\\server_pipe",
				GENERIC_READ | GENERIC_WRITE, // bidirectional access
				0,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL, // Normal, synchronous operation
				NULL
				);
		}
		// Break if the pipe handle is valid.
		if (pipe != INVALID_HANDLE_VALUE)
		{
			return pipe;
		}
		// Exit if an error other than ERROR_PIPE_BUSY occurs. 
		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			_tprintf(TEXT("Could not open pipe. GLE=%d\n"), GetLastError());
			return NULL;
		}
		return NULL;
	}

	bool sendMessage(char message[]) {
		DWORD cbToWrite;
		cbToWrite = (lstrlen(message) + 1)*sizeof(char);
		_tprintf(TEXT("Sending %d byte message: \"%s\"\n"), cbToWrite, message);

		BOOL fSuccess = WriteFile(
			pipe,                   // pipe handle 
			message,			    // message 
			cbToWrite,              // message length 
			&cbWritten,             // bytes written 
			NULL);                  // not overlapped 

		if (!fSuccess)
		{
			DWORD Error = GetLastError();
			_tprintf(TEXT("WriteFile to pipe failed. GLE=%d\n"), GetLastError());
			return fSuccess;
		}
		cout << "Message sent" << endl;
		return fSuccess;
	}

	string readMessage() {
		DWORD numRead = 1;
		char buff[BUFFER_SIZE];
		bool success = ReadFile(pipe, &buff, BUFFER_SIZE, &numRead, NULL);
		if (success) {
			cout << buff << endl;
		}
		else
		{
			_tprintf(TEXT("ReadFile failed. GLE=%d\n"), GetLastError());
		}
		return buff;
	}

	// Registers an object to the server
	bool registerObject(SharedObject object) {

		return false;
	}

	bool isPipeOpen() {
		if (pipe == NULL) {
			return false;
		}
		else
		{
			return true;
		}
	}

	// Close the connection
	void closeConnection() {
		// Close our pipe handle
		CloseHandle(pipe);
		return;
	}
};

int main()
{	
	cout << "Synchronous or Asynchronous? (sync, async)" << endl;
	string sync_attr;
	cin >> sync_attr;
	while (!(sync_attr == "sync" || sync_attr == "async") )
	{
		cout << "invalid connection type, valid conections are sync and async" << endl;
		cin >> sync_attr;
	}
	cout << "Connecting to pipe..." << endl;

	Client client(sync_attr);
	
	if (!client.isPipeOpen())
	{
		_tprintf(TEXT("SetNamedPipeHandleState failed. GLE=%d\n"), GetLastError());
		return -1;
	}

	//while (1) {
		char message[BUFFER_SIZE];
		string buffer;
		/*cout << "Enter a message for the server" << endl;
		cin >> buffer;
		if (buffer == "Close") {
			client.closeConnection();
			return 0;
		}
		else
		{
			strcpy_s(message, buffer.c_str());
			bool messageSuccess = client.sendMessage(message);
			if (!messageSuccess) {
				cout << "Message not sent." << endl;
			}
		}*/
		
		SharedObject sobj("SOBJ 12 Ross");
		strcpy_s(message, sobj.Serialize().c_str());
		bool messageSuccess = client.sendMessage(message);
		if (!messageSuccess) {
			cout << "Message not sent." << endl;
		}
		// Retrieve object, then wait for a response.
		strcpy_s(message, "GET 12");
		messageSuccess = client.sendMessage(message);
		if (!messageSuccess) {
			cout << "Message not sent." << endl;
		}
		string received = client.readMessage();
		cout << received << endl;


	//}
    return 0;
}