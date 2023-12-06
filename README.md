# FTP Server and Client

## Introduction
This repository contains the code for a File Transfer Protocol (FTP) server and client with enhanced features. The FTP server (`ftp_mt.c`) supports **multithreading** for handling multiple client connections simultaneously, and the FTP client (`ftp_ssh.c`) supports **SSH (Secure Shell)** for secure file transfer.

## Features
- **Multithreading**: The FTP server can handle multiple client connections at the same time, improving performance and scalability.
- **SSH Support**: The FTP client utilizes SSH for secure communication, ensuring that all file transfers are encrypted and protected from eavesdropping.

## Getting Started
To get started with this FTP server and client, follow the instructions below:

### Prerequisites
- A Linux-based system with GCC compiler and SSH installed.
- Basic knowledge of FTP operations and SSH configuration.

### Installation
1. Clone the repository:

    git clone https://github.com/mvali99/ftp-server-and-client.git

2. Navigate to the project directory:

    cd ftp-server-and-client

3. Compile the server and client code:

    gcc -o ftp_server ftp_mt.c -lpthread gcc -o ftp_client ftp_ssh.c -lssh


### Usage
To start the FTP server, run:

    ./ftp_server


To connect to the FTP server using the client with SSH, run:

    ./ftp_client


## Contributing
Contributions are welcome! If you have any suggestions or improvements, please fork the repository and submit a pull request, or open an issue with your ideas.

## License
This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments
- Thank you to everyone who has contributed to the development and testing of this project.
- Special thanks to those who provided feedback and suggestions for improvement.
