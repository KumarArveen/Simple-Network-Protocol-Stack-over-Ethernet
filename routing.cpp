#include "unp.h"
#include "config.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>	// Internet Add
#include <arpa/inet.h>	// inet_aton
#include <stdlib.h>
#include <errno.h>		// for error mechanism system calls
#include <string.h>
#include <iostream>
#include <vector>
using namespace std;
//			App Header...
struct appHeader{
	char type[2];
	char code[3];
	char option[16];
	
};

//			TL  Header...
struct TLHeader{
	struct appHeader app;
	char   srcPort[16];
	char   destPort[16];
	int    seqNo;
	int    AckNo;
	char   controlBits[3];// ack, syn, fin 
	char   windowSize[16];
	char   checkSum[16];
	char   data[128];
};

//===========	Packet Structure...
struct packet{
	struct TLHeader header;
	
};

//========== Network Layer Header
struct Datagram{
	struct packet payload;	//payload
	char destIP[16];
	char srcIP[16];
	char path[16];
};

struct router
{
	char name;	//a, b, c,... prompts for user input
	string IP;	//read from configuration file...
	int port;	//prompts for user input 	
};

vector<router> makeForwardingTable(){
		vector<router> routerVector;
		int port = 2000;
		for(int i=0; i<6; i++, port+=1000){
			struct router Router;
			Router.name = char(i+97);		//i=0, Router.name = 'a'
			Router.port = port;
			Router.IP = "127.0.0.1";
			
			routerVector.push_back(Router);
		}
		
		return routerVector;
}

int main(){
	
	char router_name;
	cout<<"Enter router name (should be(a,b,...,f) ): ";
	cin>>router_name;
	int port,ret;
	string ip;
	vector<router> ForwardingTable = makeForwardingTable();
   for(int i=0; i<6; i++){	//cout<<"\t"<<ForwardingTable.at(i).name<<"\t"<<ForwardingTable.at(i).port<<"\t"<<ForwardingTable.at(i).IP<<"\n";
		char rname = ForwardingTable.at(i).name;
		if( rname == router_name ){				//This is current router information
			port = ForwardingTable.at(i).port;
			ip   = ForwardingTable.at(i).IP;
			break;
		}
	}
	cout<<"router port: "<<port<<endl;
	cout<<"router ip: "<<ip<<endl;
					
	struct sockaddr_in Router, client;
	int conn_sock = socket(AF_INET, SOCK_DGRAM,0);
	if( conn_sock == -1 ){
		cout<<"Error in Socket creation...!\n";
	}
	
	Router.sin_port = htons(port);		// port on which it recv data from client...
	Router.sin_family = AF_INET;
	Router.sin_addr.s_addr = inet_addr(ip.c_str());
	ret = bind(conn_sock, (struct sockaddr*)&Router, sizeof(struct sockaddr));
	if( ret == -1 ){
		cout<<"Error in Binding Router...\n";
	}
	
	//char sendmsg[200];
	//char recvmsg[200];
	
	while(1){

		cout<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n";
		cout<<"Router: \""<<router_name<<"\" waiting to receive packets...!\n"; 
		cout<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n";
		//======================================
		//;;;; Receiving Data from client/router
		//======================================	
		struct Datagram datagram;
		int i = sizeof(client);
		ret = recvfrom( conn_sock, (struct Datagram *)& datagram, sizeof(datagram), 0, (struct sockaddr *)& client, (socklen_t *)&i );
		if( ret == -1 ){
				cout<<"Unable to receive data from the router...!\n";
				return -1;
		}
		char * recvmsg = datagram.path;
		cout<<"Path Received from router side: "<<recvmsg<<endl;
		cout<<"DATA Received from router side: "<<datagram.payload.header.data<<endl;
		
		//=========================================
		//;;;; Sending data to next hop/destination
		//=========================================
		// finding next router
		for(int k=0; recvmsg[k] != '\0'; k++){
			if( recvmsg[k] == router_name ){
				if(recvmsg[k+1] == '\0'){
					cout<<"\nI am the Last router towards destination!\n";
					//Now we need to send data to server
					//bind with server ip and port and then send....
					int new_sock = socket(AF_INET, SOCK_DGRAM, 0);
					if( new_sock == -1 ){
						cout<<"Error in creating new Socket!\n";}
						
					int port1 = atoi(datagram.payload.header.destPort);	
					cout<<"\n#########\nDESTINATION PORT: "<<port1<<"\n##########";
					cout<<"\n#########\nDESTINATION PORT: "<<htons(port1)<<"\n##########\n";
					
					if( router_name == 'a' ){
						cout<<"I am a,  my task is to send to Client!!\n";
					//	struct sockaddr_in o;
						client.sin_family= AF_INET;
						int p = port1-10;
						client.sin_port = htons(p) ;
						//cout<<"Router a sending on Changed port: "<<client.sin_port<<endl;
						client.sin_addr.s_addr = inet_addr("127.0.0.1");
						cout<<"client_port: "<<client . sin_port<<endl;
					//	char ppp[200] = "Hello Client!";
						//ret = sendto(conn_sock, ppp, 200, 0, (struct sockaddr *)&client, sizeof(client) );
						ret = sendto(conn_sock, &datagram, sizeof(struct Datagram), 0, (struct sockaddr *)&client, sizeof(client));
						if( ret == -1 ){
							cout<<"\n if (a): Error in sending message to next router...!\n";
						}
					}else{
					
					client.sin_family= AF_INET;
					client.sin_port = htons(port1);
					client.sin_addr.s_addr = inet_addr("127.0.0.1");
					ret = sendto(new_sock, &datagram, sizeof(struct Datagram), 0, (struct sockaddr *)&client, sizeof(client));
					if( ret == -1 ){
						cout<<"\nError in sending message to next router...!\n";
					}}
					cout<<"..........................................\n";
					cout<<"Message towards final destination is sent!\n";
					cout<<"..........................................\n";
				}
				else{
					char next = recvmsg[k+1];
					//search next router port and ip to forward data...
					for(int i=0; i<6; i++){	
						char rname = ForwardingTable.at(i).name;
						if( rname == next ){				//This is current router information
							port = ForwardingTable.at(i).port;
							ip   = ForwardingTable.at(i).IP;
							break;
						}
					}
					cout<<"next router port: "<<port<<endl;
					cout<<"next router ip: "<<ip<<endl;
					int new_sock = socket(AF_INET, SOCK_DGRAM, 0);
					if( new_sock == -1 )
						cout<<"Error in creating new Socket!\n";
					client.sin_port = htons(port);
					client.sin_family= AF_INET;
					client.sin_addr.s_addr = inet_addr(ip.c_str());
					//======================
					//Sending to next router
					//======================
				
					//cout<<"Add a char to send to next router: ";
					//cin>>recvmsg[4];
					ret = sendto(new_sock, &datagram, sizeof(struct Datagram), 0, (struct sockaddr *)&client, sizeof(client));
					if( ret == -1 ){
						cout<<"\nError in sending message to next router...!\n";
					}
					cout<<"Message sent!\n";
				}
			}
		}
		//ret = sendto(conn_sock, recvmsg, 200, 0, (struct sockaddr *)&)
	}
}
