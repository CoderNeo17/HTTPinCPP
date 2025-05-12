# HTTP SERVER IN CPP

## Description
This is a simple HTTP server written in C++. It is designed to handle basic HTTP requests and serve static files. The server is built using the C++ standard library and does not require any external dependencies.
## Features
- Fully custom request handlers
- User can define their own request handling functors and pass them to the server
- Supports basic HTTP headers
- Simple and easy to use
- Lightweight and fast
## Requirements
- C++11 or later
- A C++ compiler (g++, clang++, etc.)

## Implementation
- The server is implemented using the C++ standard library
- The server uses the `socket` API to create a TCP socket and listen for incoming connections
- Request and Response classes are implemented to handle HTTP requests and responses easily
- Basic router that matches incoming requests to user-defined routes and passes them to the appropriate route handler
- Each route handler can be given a method (GET, POST, etc.) and a path (e.g. "/hello") to match incoming requests
- Each method in a route can be given a functor (a function or a lambda) that will be called when the route is matched

> The word **user** refers to the person who is building the server.

> **Note:** Check `version_log` for the actual progress of the project.  
