#include <iostream>
//#include <WinSock2.h>
//#include <stdio.h>
#include <WS2tcpip.h>
#include <thread>
#include <stdlib.h> 

#pragma comment (lib, "ws2_32.lib")
#pragma warning(disable:4996)


struct UserProfile
{
	SOCKET userSocket;
	char name[100];
	int brokenMessagesCount;
	int threadQ;
	bool broken, working;
	UserProfile* nextProfile;
};



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




void stringPass(char from[], char to[])
{
	for (int x = 0; ; x++)
	{
		if (from[x] <= 30)
			break;
		
		to[x] = from[x];
	}
}

void Se(UserProfile* root, char message[], int size, int* count)
{
	UserProfile* prof = root;
	while (prof != NULL)
	{
		//std::cout << '\t ' << "Siunc  " << (prof) << " " << (*prof).threadQ << " "<< (*prof).userSocket <<std::endl;
		send((*prof).userSocket, message, size, 0);
		prof = (*prof).nextProfile;
	}

}





void Re(UserProfile* user, int *count, UserProfile *root, bool* disconnection)
{
	char buf[100];

	SOCKET ssse = (*user).userSocket;


	recv(ssse, buf, sizeof(buf), 0); // strange bug, changes socket to 0 ???
	 (*user).userSocket =  ssse;

	 stringPass(buf, (*user).name);

	 int del;


	while (1)
	{
	

		std::cout << "klausom " << " " << (*user).userSocket  << "\t"; 
		del = recv((*user).userSocket, buf, sizeof(buf), 0);

		if (del > 0)
		{
			(*user).brokenMessagesCount = 0;
			printf("%s \t \t %s   \n", (*user).name, buf);
			Se(root, buf, sizeof(buf), count);
		}
		else
		{
			std::cout << '\r';
			(*user).brokenMessagesCount += 1;
		}

		if ((*user).brokenMessagesCount >= 5)
		{
			(*user).broken = true;
			(*user).working = false;
			std::cout << (*user).userSocket << " Disconnected" << std::endl;
			*disconnection = true;
			break;
		}

		

		Sleep(100);
	}
}



UserProfile setupProfile(UserProfile* profile)
{
	//UserProfile profile;
	memset((*profile).name, 0, sizeof((*profile).name));
	(*profile).userSocket = NULL;
	(*profile).nextProfile = NULL;
	(*profile).threadQ = 0;
	(*profile).broken = false;
	(*profile).working = false;
	(*profile).brokenMessagesCount = 0;
	return (*profile);
}

SOCKET setupClient(SOCKET ss)
{
	SOCKET clientSocket;
	clientSocket = accept(ss, NULL, NULL);



	if (clientSocket == INVALID_SOCKET)
	{
		printf("blogas soketas clS");
		return -1;
	}

	return clientSocket;
}

int Fi(UserProfile* root)
{
	UserProfile* next = root;
	for (int x = 0; ; x++)
	{
		if (next == NULL) return -1;

		if ((*next).broken == true)
		{
			std::cout << "Fi    "<< x << std::endl;
			return x;
		}
		next = (*next).nextProfile;

	}
	std::cout << " ??? " << std::endl;
	return -1;
}


void GetConnections(SOCKET ServerSocket, UserProfile **top , int* count, bool* connected, UserProfile* root)
{
	
	while (1)
	{
		if (!(*connected))
		{
			
			UserProfile* user = (UserProfile*)malloc(sizeof(UserProfile));
			setupProfile(user);
			
				int threadN = -1;
				

			
			SOCKET newSocket = setupClient(ServerSocket);

			if (*count > 0)
			{
				threadN = Fi(root);
			}

			if (threadN != -1)
			{
				std::cout << "suveikeu  " << std::endl;
				user = root;
				for (int x = 0; x < threadN; x++)
				{
					user = (*user).nextProfile;
				}
				(*user).userSocket = newSocket;
				(*user).broken = false;
			}
			else
			{

				(*user).userSocket = newSocket;
				(*user).threadQ = *count;

				if (*count == 0)
				{
					**top = (*user);
				}

				else
				{
					(**top).nextProfile = user;
					*top = user;
					(**top) = *user;
				}
				*count += 1;
			}

				std::cout << "Connected  " << *top << " " << (**top).userSocket << std::endl;
				*connected = true;
			
		}
		Sleep(1000);
	}
			
	
}

std::thread spawn_receiver(UserProfile* user, int* count, UserProfile* root, bool* disconnection)
{
	(*user).working = true;
	return std::thread(Re, user,  count,  root, disconnection);
}





int main()
{

	//SOCKET clientSendSockets[4];
	UserProfile* userRoot = (UserProfile*) malloc(sizeof(UserProfile));
	UserProfile* userCheck;
	UserProfile** userTop = (UserProfile**) malloc(sizeof(UserProfile*));;
	*userTop = userRoot;


	int count = 0;

	bool gotConnection = false, gotDisconnection = false;

	if (wsSetup()) return 1;

	SOCKET serverSendSocket = setupServer(9001); // is antro siuncia

	listen(serverSendSocket, 4);

	


	std::thread t1(GetConnections, serverSendSocket, userTop , &count, &gotConnection, userRoot);
	std::thread t4[4];

	
	
	while (1)
	{
		if (gotConnection)
		{

			userCheck = userRoot;

			
				for (int x = 0; x < count; x++)
				{
					if (userCheck == NULL) break;


					if (x+1 == count || (*userCheck).threadQ < 0)
					{
						std::cout <<std::endl<<"count  "<< count - 1 << std::endl;
						t4[count - 1] = spawn_receiver(*userTop, &count, userRoot, &gotDisconnection);
					}
					userCheck = (*userCheck).nextProfile;
				}
			


			


			//t4[count-1] = spawn_receiver(*userTop , &count, userRoot, &gotDisconnection);

			gotConnection = false;
			Sleep(100);
		}

		if (gotDisconnection)
		{
			userCheck = userRoot;


			for (int x = 0; x < count; x++)
			{
				if (userCheck == NULL) break;


				if ((*userCheck).broken && (*userCheck).threadQ >=0)
				{
					std::cout << "TT  "<< (*userCheck).threadQ << std::endl;
					t4[(*userCheck).threadQ].join();
				}
				userCheck = (*userCheck).nextProfile;
			}

			gotDisconnection = false;
			Sleep(100);
		}

	}



	//fclose(fp);
	//closesocket(clientSendSockets[0]);
	closesocket(serverSendSocket);

	return 0;
}