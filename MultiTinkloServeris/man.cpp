//#include <iostream>
//#include <WinSock2.h>
#include <stdio.h>
#include <WS2tcpip.h>


#pragma comment (lib, "ws2_32.lib")
#pragma warning(disable:4996)

boolean wsSetup()
{
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 0);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		printf("cant");
		return 1;
	}
	return 0;
}


SOCKET setupServer(int port)
{
	SOCKET socketer = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serverAdress;
	serverAdress.sin_family = AF_INET;
	serverAdress.sin_port = htons(port);
	serverAdress.sin_addr.s_addr = INADDR_ANY;


	if (socketer == INVALID_SOCKET)
	{
		printf("blogas soketas seS");
		return -1;
	}

	bind(socketer, (struct sockaddr*) & serverAdress, sizeof(serverAdress));

	return socketer;
}


int setupClient(SOCKET ss)
{
	listen(ss, 4);

	int clientSocket;
	clientSocket = accept(ss, NULL, NULL);

	if (clientSocket == INVALID_SOCKET)
	{
		printf("blogas soketas clS");
		return -1;
	}

	return clientSocket;
}




int main()
{
	//std::cout << "Heloow";


	if (wsSetup()) return 1;

	//SOCKET serverReceiveSocket = setupServer(9002); // is pirmo gauna
	SOCKET serverSendSocket = setupServer(9001); // is antro siuncia



	//int clientSendSocket;
	//clientSendSocket = accept(serverSocket, NULL, NULL);
	//int clientReceiveSocket = setupClient(serverReceiveSocket);
	int clientSendSocket = setupClient(serverSendSocket);

	char buff[100];
	char mes[3] = "OK";
	char mes2[3] = "NO";
	//char addit = '1';
	//recv(clientReceiveSocket, buff, sizeof(buff), 0);

	FILE* fp;
	fp = fopen("test.txt", "w+");
	//fprintf(fp, "This is testing for fprintf...\n");

	//std::string word = "OK";
	int o = 1;
	while (o == 1)
	{

		recv(clientSendSocket, buff, sizeof(buff), 0);

		fprintf(fp, buff);
		fprintf(fp, "\n");

		printf(" %s \n", buff);
		//add1(buff, sizeof(buff) / sizeof(char));

		int num = findEND(buff, sizeof(buff) / sizeof(char));
		if (num == 1)
			send(clientSendSocket, mes, sizeof(mes), 0);
		else
			send(clientSendSocket, mes2, sizeof(mes2), 0);


		if (buff[0] == 'E' && buff[1] == 'N' && buff[2] == 'D')
		{

			o = 0;
		}

		clearString(buff, sizeof(buff) / sizeof(char));


	}

	fclose(fp);
	closesocket(clientSendSocket);
	//closesocket(clientReceiveSocket);
	//closesocket(serverReceiveSocket);
	closesocket(serverSendSocket);

	return 0;
}