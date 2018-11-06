# ClientServer
StreamLabs StreamBase App

The purpose of this project is to create a simple Client/Server interface using NamedPipes to allow sending of data and registration of object from the Client on the Server.

Please note: The binaries for Server and Client can be found under Release in their respective directories.

Requirements:

1.    The client should be able to connect to the server through a NamedPipe
2.    The client should be able to make both sync and async calls to the server
3.    The client should be able to send trivial data (strings, numbers) to the server
4.    The client should be able to create objects on the server (based on req-7 below), retrieve them, their attributes and call methods on them
5.    The server should be able to receive both sync/async connection requests from clients
6.    The server should be able to store data provided by the client via NamedPipe in a reasonable data structure
7.    The server should be able to register a custom class (w/ related functions, attributes) which can be used by the client (see req-4)
8.    The server should be able to store the custom objects created by the client for the custom class created in req-7

Assumptions and implementation details:
The Server must be started before the Client, otherwise the Client will not find an open pipe and will exit.
We do not store trivial data sent to the Server re. req 3.
The Client interacts with the server through a series of Predefined commands.  Use command HELP to view these commands.
The Client utilizes a simple text interface for dynamic, user-driven client/server interaction
I used a very simple Object for demonstration purposes, but the framework could be extended to incorporate more complex objects.
Shared Objects exist as a common library (for simplicity, simply as duplicated classes) between the Client and Server.
Synchronous/Asynchronous I/O has been implemented on a per-message basis, rather than a per-connection basis, to allow greater flexibility for the Client.  An Asynchronous I/O request cannot be initiated while an existing asynchronous request is still pending.
