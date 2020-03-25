#include <iostream>
//#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <thread>

#pragma comment (lib, "ws2_32.lib")

bool wsSetup()
{
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 0);
	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		std::cout << "cant";
		return true;
	}
	return false;
}



sockaddr_in setupAdress(int port)
{
	sockaddr_in adress;
	adress.sin_family = AF_INET;
	adress.sin_port = htons(port);
	std::string ipAdress = "127.0.0.1";
	inet_pton(AF_INET, ipAdress.c_str(), &adress.sin_addr);
	return adress;
}

void Receiver(SOCKET clientReceiveSocket, int* o)
{
	char rMessage[100];

	while (*o == 1)
	{
		recv(clientReceiveSocket, rMessage, sizeof(rMessage), 0);

		std::cout << "Gavau " << rMessage << std::endl;
	}
}




int main()
{
	// inicializuojuam winsock
	if (wsSetup()) return 1;

	SOCKET clientReceiveSocket;
	clientReceiveSocket = socket(AF_INET, SOCK_STREAM, 0);


	struct sockaddr_in clientReceiveAdress = setupAdress(9001);
	if (clientReceiveSocket == INVALID_SOCKET) std::cout << "blogas socketas RC";

	connect(clientReceiveSocket, (sockaddr*)&clientReceiveAdress, sizeof(clientReceiveAdress));


	char buff[100];
	int x = 0;
	int o = 1;

	std::thread tS(Receiver, clientReceiveSocket, &o);



	
	while (o == 1)
	{
		std::cin.getline(buff, sizeof(buff));
		//strcpy_s(buff, names[x].c_str());

		if (buff[0] == '0') o=0;

		send(clientReceiveSocket, buff, sizeof(buff), 0);

		Sleep(100);
		x++;
	}
	std::cout << "dis";
	tS.join();
	closesocket(clientReceiveSocket);

	return 0;
}