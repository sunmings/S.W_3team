#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <winsock2.h>
#include <fstream>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")

#define BUFSIZE 1024

using namespace std;

class TcpClientSocket
{
private :
	char servIp[50];
	char buf[BUFSIZE + 5];
	
	int port;
	int servPort;
	int peerLen;
	
	WSADATA wsaData;

	sockaddr_in servAddr;
	sockaddr_in peerAddr;

	SOCKET sock;

public :
	TcpClientSocket(int port, char* ip, int serv_port);
	void createSocket();
	void connectSocket();
	void sendMessage(char* message);
	char* receiveMessage();
	void receiveFile(char* file_name);
};

TcpClientSocket::TcpClientSocket(int port, char* ip, int serv_port)
{
	this->port = port;
	strcpy(servIp, ip);
	servPort = serv_port;
}

void TcpClientSocket::createSocket()
{
	if ((WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		perror("WSA : ");
		exit(1);
	}

	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		perror("sock : ");
		exit(1);
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(servIp);
	servAddr.sin_port = htons(servPort);
}

void TcpClientSocket::connectSocket()
{
	if( (connect(sock, (struct sockaddr *)&servAddr, sizeof(servAddr))) == INVALID_SOCKET)
	{
		perror("connect : ");
		exit(1);
	}
}

char* TcpClientSocket::receiveMessage()
{
	int mLen = recv(sock, buf, BUFSIZE, 0);

	buf[mLen] = 0;
	return buf;
}

void TcpClientSocket::sendMessage(char* message)
{
	strcpy(buf, message);
	send(sock, buf, strlen(buf), 0);
	Sleep(100);
}

void TcpClientSocket::receiveFile(char* file_name)
{
	ofstream outFile(file_name);

	strcpy(buf, receiveMessage());
	while(strcmp(buf, "EOF"))
	{
		if(outFile.is_open())
		{
			printf("%s\n", buf);
			outFile << buf;
			strcpy(buf, receiveMessage());
		}
		else
		{
			perror("outFile : ");
			exit(1);
		}
	}
	outFile.close();
}