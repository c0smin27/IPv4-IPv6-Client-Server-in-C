# IPv4 Server & IPv6 Client Project

## Description

This project implements a hybrid **IPv4 server / IPv6 client** application written in C, designed for a networking lab. <br>
The Linux-based server listens for commands from a Windows client over IPv4 sockets. Upon receiving a valid command (`xy#`), the server connects to a public IPv6 web server and retrieves a webpage, saving the content locally and sending it back to the original client.

## Functionalities

- Listens for TCP connections on port `22214`
- Accepts commands in the format `xy#` from clients
- If the command is known (e.g., `12#`), it:
  - Establishes an IPv6 TCP connection to the corresponding server
  - Sends an HTTP GET request
  - Saves the response as `index.html`
  - Sends the response back to the IPv4 client
- If the command is unknown -> returns: `"Comanda neimplementata."`
- Logs each client’s IP and port in `log.txt`

## Requirements
- Linux with IPv6 support
- GCC compiler

## Disclaimer

This project was created as part of an academic assignment. Feel free to use it for learning purposes, but please do not submit it as your own work in educational settings.
