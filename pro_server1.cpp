#include <pthread.h>
#include "config.h"
#include "unp.h"
//#include "dijkstra.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <cstring>
#include <sys/types.h>
#include <dirent.h> 
#include <stdio.h> 
#include <sys/socket.h>
#include <netinet/in.h>	// Internet Add
#include <arpa/inet.h>	// inet_aton
#include <stdlib.h>
#include <errno.h>		// for error mechanism system calls
#include <string.h>
#include <time.h>
#define INFINITY 999;

using namespace std;
class Dijkstra{
    private:
        int adjMatrix[6][6];
        int predecessor[6],distance[6];
        bool mark[6]; //keep track of visited node
        int source;
        int numOfVertices;
    public:
		  Dijkstra(); 
        int ** read();
        void initialize();
        int getClosestUnmarkedNode();
        void calculateDistance();
        char * getShortestPath(char);
        int getNoOfVertices();
};

//########################################

//===========  Router Structure....
struct node{
	
	char name;
	int port;
	string IP;
	int * cost;	//static no of routers in the network.......		
};
vector<node> RoutingTable;

Dijkstra :: Dijkstra(){
		//adjMatrix[]
}

int Dijkstra :: getNoOfVertices(){	
		return numOfVertices;
}
int ** Dijkstra :: read(){
	 
	int matrix[6][6] = {{0,1,3,5,0,0},{1,0,0,2,4,0},{3,0,0,1,0,2},{5,2,1,0,1,0}, {0,4,0,1,0,0},{0,0,2,0,0,0}};
   // cout<<"Enter the number of routers of your NETWORK(should be > 0)\n";
    //cin>>numOfVertices;
    numOfVertices = 6;
    while(numOfVertices <= 0) {
        cout<<"Enter the number of routers of your NETWORK(should be > 0)\n";
        cin>>numOfVertices;
    }
    int ** cost = new int*[numOfVertices];
    for(int i=0; i<numOfVertices; i++){
		cost[i] = new int[numOfVertices];
	 }
    
    //cout<<"Enter the adjacency matrix for the graph\n";
    //cout<<"To enter infinity enter 999"<<endl;
    for(int i=0;i<numOfVertices;i++) {
       // cout<<"Enter the (+ve)costs for the router "<<char(i+97)<<endl;
        for(int j=0;j<numOfVertices;j++) {
            //cin>>adjMatrix[i][j];
            adjMatrix[i][j] = matrix[i][j];
            cost[i][j] = adjMatrix[i][j];
            while(adjMatrix[i][j]<0) {
                cout<<"Costs should be +ve. Enter the cost again\n";
                cin>>adjMatrix[i][j];
					 adjMatrix[i][j] = matrix[i][j];
					 cost[i][j] = adjMatrix[i][j];
            
            }
        }
    }
    char src;
    cout<<"Enter the source router(router connected with the source!)\n";
    cin>>src;
    source  = int(src)-97;
    cout<<"source : "<<source;
    while((source<0) && (source>numOfVertices-1)) {
        cout<<"Source router should be between a and"<<char( (numOfVertices-1)+97 )<<endl;
        cout<<"Enter the source router again\n";
        cin>>src;
        source  = int(src)-97;
    }
   
    return cost;
}

 
void Dijkstra::initialize(){
    for(int i=0;i<numOfVertices;i++) {
        mark[i] = false;
        predecessor[i] = -1;
        distance[i] = INFINITY;
    }
    distance[source]= 0;
}
 
 
int Dijkstra::getClosestUnmarkedNode(){
    int minDistance = INFINITY;
    int closestUnmarkedNode;
    for(int i=0;i<numOfVertices;i++) {
        if((!mark[i]) && ( minDistance >= distance[i])) {
            minDistance = distance[i];
            closestUnmarkedNode = i;
        }
    }
    return closestUnmarkedNode;
}
 
 
void Dijkstra::calculateDistance(){
    initialize();
    //int minDistance = INFINITY;
    int closestUnmarkedNode;
    int count = 0;
    while(count < numOfVertices) {
        closestUnmarkedNode = getClosestUnmarkedNode();
        mark[closestUnmarkedNode] = true;
        for(int i=0;i<numOfVertices;i++) {
            if((!mark[i]) && (adjMatrix[closestUnmarkedNode][i]>0) ) {
                if(distance[i] > distance[closestUnmarkedNode]+adjMatrix[closestUnmarkedNode][i]) {
                    distance[i] = distance[closestUnmarkedNode]+adjMatrix[closestUnmarkedNode][i];
                    predecessor[i] = closestUnmarkedNode;
                }
            }
        }
        count++;
    }
} 
 
char * Dijkstra::getShortestPath(char dest){
	char * arr = new char[6];
	int in=0;
    for(int i=0;i<numOfVertices;i++) {
        if(i == source){}
           // cout<<(char)(source + 97)<<".."<<source;
        else if( i+97 == int(dest) ){
			  cout<<"dest: "<<char(i+97)<<endl;
           int j=i;
            
            while(1){
					if( j == source ){
						break;
					}else{
						cout<<char(predecessor[j]+97)<<endl;
						arr[in++] = char(predecessor[j]+97);
						j=predecessor[j];
					}
				}
			//cout<<"->"<<distance[i]<<endl;
			break;
		}        
   }
   cout<<"value of in: "<<in<<endl;
   char * path = new char[in+1];
   int i=0;
   for(;i<in; i++)
		path[i] = arr[in-(i+1)];
	path[i] = dest;
	
	for(int i=0;i<in+1; i++)
		cout<<path[i]<<"..";
	path[in+1] = '\0';
	
	cout<<"\nret-path: "<<path<<endl;
	return path;
}
//########################################
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

//===== Network layer Method [unreliable data transfer] ====
int udt_Send(int server_port,struct packet pkt, char * path, char * sIP, char * dIP, vector<node> RoutingTable, int conn_sock ){
		
		cout<<"~~~~~~~~~~~~~~~~~~~~\nPath recv in udt_Send(): "<<path<<endl;
		struct Datagram datagram;
		datagram.payload = pkt;
		int i=0;
		
		for(i=0; path[i] != '\0'; i++)
			datagram.path[i]    = path[i];
		
		datagram.path[i]    = '\0';
		cout<<"~~~~~~~~~~~~~~~~~~~~\nPath recv in udt_Send(): "<<datagram.path<<endl;
		//Source IP	
		for(i=0; sIP[i] != '\0'; i++)
			datagram.srcIP[i]   = sIP[i];
		
		datagram.srcIP[i] = '\0';
		
		//Destination IP
		for(int i=0; dIP[i] != '\0'; i++)
			datagram.destIP[i]  = dIP[i];
		
		datagram.destIP[i] = '\0';
		
		//Send datagram in the network...
		int port;string ip;
		
		if( server_port == 0 ){
			for(unsigned int i=0; i < RoutingTable.size(); i++){
				if(RoutingTable.at(i).name == path[0]){
					port = RoutingTable.at(i).port;
					ip   = RoutingTable.at(i).IP;
				}
			}
		}
		else{
			
			ip = "127.0.0.1";
			port = server_port;
			cout<<"udt-Send: port: "<<port<<endl;
		}
		
	cout<<"\nSending to ROUTER with port: "<<port<<" and IP: "<<ip<<endl;	
	struct sockaddr_in next_router;
	next_router.sin_family = AF_INET;
	next_router.sin_port   = htons(port);
	next_router.sin_addr.s_addr = inet_addr(ip.c_str());
		
	return sendto(conn_sock,  &datagram, sizeof(struct Datagram), 0, (struct sockaddr *)&next_router, sizeof(next_router));
}

//=========== Global variabl es
struct sockaddr_in server, client, server_addr;
int conn_sock;

//==========	send cwd Files...
vector<string> sendCwdFiles(){
	vector<string>Vector; 
	char path[256];
	getcwd(path,256);
	DIR* dirFile=opendir(path);
	if(dirFile)
	{
		struct dirent* hFile;
		while((hFile=readdir(dirFile))!=NULL)
		{
			if(!strcmp(hFile->d_name,".")) continue;
			if(!strcmp(hFile->d_name,".."))continue;
			if((hFile->d_name[0]=='.'))    continue;
			if(strstr(hFile->d_name,".txt"))
			{
				string s=hFile->d_name;
				Vector.push_back(s);
			}
		}
		closedir(dirFile);
	}
	return Vector;
}

//			Check Code Method...
string checkCode(struct packet arr){
	string ret;
	if( arr.header.app.type[0]=='0' && arr.header.app.type[1]=='0'      && arr.header.app.code[0]=='0' && arr.header.app.code[1]=='0' && arr.header.app.code[2]=='0' ){
		ret =  "FILE_LIST_REQUEST";
	}
	else if( arr.header.app.type[0]=='0' && arr.header.app.type[1]=='1' && arr.header.app.code[0]=='0' && arr.header.app.code[1]=='0' && arr.header.app.code[2]=='0' ){
		ret =  "FILE_DOWNLOAD_REQUEST";
	}
	else if( arr.header.app.type[0]=='0' && arr.header.app.type[1]=='1' && arr.header.app.code[0]=='0' && arr.header.app.code[1]=='1' && arr.header.app.code[2]=='0' ){
		ret =  "FILE_DOWNLOAD_START";
	}
	return ret;
}

//===========  make Packet...
struct appHeader makeAppHeader( string arr ) {
	struct appHeader app;
	app.type[0] = arr[0];
	app.type[1] = arr[1];
	app.code[0] = arr[2];
	app.code[1] = arr[3];
	app.code[2] = arr[4];
	return app;
}

//===========	create Socket...
int createUdpSocket(){
		return socket(AF_INET, SOCK_DGRAM, 0);
	}
	
//			close Socket...	
void closeSocket(int sock){
		close(sock);
}

//			bind Server...
int bindServer(int sock, struct sockaddr_in server){
		return bind( sock, ( struct sockaddr *)& server, sizeof( struct sockaddr) );
	}	
	
//			read Configuration File...	
vector<string> readConfigurationFile(string fileName){
			vector<string> filevector;
			string readstr;
			ifstream in(fileName.c_str());
			if(!in.is_open()){
				cout<<"failed to open the file....\n";
			}
			else{
				while( getline(in,readstr)){
					//cout<<"readstr from file: "<<readstr<<endl;
					filevector.push_back(readstr);
				}
			}
			return filevector;
	}
	
// 		bind Method.....
struct sockaddr_in bindserver(int port, string ip){
		struct sockaddr_in server;
		//				Server Socket...
		server.sin_family = AF_INET;
		server.sin_port   = htons( port );
		server.sin_addr.s_addr = inet_addr(ip.c_str());
		memset( &(server.sin_zero), '\0', 8);
		return server;
	}

//			file Size...
int fileSize(string filename){
		FILE *p_file=NULL;
      p_file=fopen(filename.c_str(),"rb");
      fseek(p_file,0,SEEK_END);
      int size=ftell(p_file);
      fclose(p_file);
      return size;
}

//			file Name...
string fileName( packet recvpkt ){
	string filename = "";
	for(int i=0; recvpkt.header.data[i] != '\0'; i++){
		filename.append(1u, recvpkt.header.data[i]);
	}
	return filename;
}

//			Generate Random Number
int generateRandomNo(){
	
	srand(time(NULL));
	int no = rand();
	return no;
}

//============== ShortestPathAlgo
char * shortestPathAlgorithm(Dijkstra * G,char dest){//-------------
	//Dijkstra G;									//--------------
   //G.read();
   G->calculateDistance();
   char * path = G->getShortestPath(dest);
   cout<<"PATH in shortestPathAlgorithm: "<<G->getShortestPath(dest)<<endl;
   return path;
}
//==============


//			sendPacket Method....
int sendPacket(int sock, struct packet sendpkt, struct sockaddr_in * receiver){
	int n = sendto(sock,  &sendpkt, sizeof(struct packet), 0, (struct sockaddr *)receiver, sizeof(*receiver));
	return n;
}

//			recvPacket Method....
packet * recvPacket(struct sockaddr_in * sender ){
	
	struct packet recvpkt;
	struct packet *p;
	int i = sizeof(*sender);
	int n = recvfrom( conn_sock, (struct packet *)& recvpkt, sizeof(recvpkt), 0, (struct sockaddr *) sender, (socklen_t *)&i );
	cout<<"\nDATA LENGTH: "<<strlen(recvpkt.header.data)<<endl;
	if( n == -1 ){
		perror("\nError in receiving packet from the client.....\n");
	}
	else{
		p = &recvpkt;
		}
	return p;
}

//			Three Way Handshaking... server method
int * threeWayHandshaking(int size, int MAX){
	cout<<"\n========================3-way Handshaking========================\n";
	int arr[3];	arr[0] = -1; 	arr[1] = -1; arr[2] = -1;
	int seqNo = generateRandomNo()%1000;
	while(seqNo <= 0 ){
		seqNo = generateRandomNo()%1000;
	}
	arr[0] = seqNo;
	cout<<"@server: initial seq#: "<<seqNo<<endl;
	if( size < MAX ){		//size starts from 0
		while( 1 ){
			struct packet sendpkt;
			cout<<"@server: server waiting.......................\n";
			struct packet * recvpkt = recvPacket(&client) ;	//recv pkt from the client....
			if( recvpkt == NULL ){	//connection failed...
				perror("\nError in receiving packet from the client.....\n");return NULL;	
			}
			else{
				if( recvpkt->header.controlBits[1] == '1' ){	// SYNbit = 1
					arr[1] = recvpkt->header.seqNo;
					//cout<<"SYNbit: 1 "<<" seq#: "<<seq<<endl;
					sendpkt.header.controlBits[0]  = '1';			//Ackbit = 1 
					sendpkt.header.controlBits[1]  = '1';			//SYNbit = 1
					sendpkt.header.seqNo = seqNo;						//seq#
					sendpkt.header.AckNo = arr[1] + 1;//AckNo  = seq# + 1
					int status = sendPacket(conn_sock,sendpkt, &client);
					if( status == -1){
						cout<<"Error! packet not sent...\n";return NULL;
					}
					//cout<<"1 pkt recv: -> client addr: "<<client.sin_port<<endl;
				}
				else if( recvpkt->header.controlBits[0] == '1' ){	//Ackbit = 1
					if( recvpkt->header.AckNo == seqNo + 1 ){
						cout<<"@server: client is alive!\n";
						int port = client.sin_port;
				//		cout<<"Client PORT: "<<port<<endl;
				//		cout<<"Starting Seq#: "<<seqNo<<endl;
						arr[2] = port;
						break;
					}
					else{
						break;
					}
				}
			}
		}//while
	}
	else{
		struct packet sendpkt;
		sendpkt.header.controlBits[0]  = '1';			//Ackbit = 1 
		sendpkt.header.controlBits[1]  = '0';			//SYNbit = 1
		sendpkt.header.controlBits[2]  = '1';			//FINbit = 1
		int status = sendPacket(conn_sock,sendpkt, &client);
		if( status == -1){
			cout<<"Error! packet not sent...\n";return NULL;
		}			
		return NULL;
	}
	cout<<"\n========================New client connected========================\n";
	int * ptr = arr;		//???????????????????????????????????
	return ptr;
}

//			establish Connection... client method
int *	establishConnection(int conn_sock){

	cout<<"\n========================3-way Handshaking========================\n";
	int arr[2];
	struct packet sendpkt;
	int x = generateRandomNo()%1000;
	cout<<"@client: initial seq#: "<<x<<endl;
	sendpkt.header.seqNo = x;						//starting from random seq# 
	sendpkt.header.controlBits[1] = '1';	 	// SYNbit = 1				
	arr[0] = x;
	//cout<<"seqNo: "<<sendpkt.header.seqNo<<endl;
	//cout<<"SYNbit: "<<sendpkt.header.controlBits[1]<<endl;
	int n = sendPacket(conn_sock,sendpkt, &server);
	if( n == -1 ){
		cout<<"Error in sending.....\n\n";exit(0);
	}
	cout<<"@client: connection request sent...!\n";
	cout<<"@client: client waiting for ACK..................\n";
	struct packet * recvpkt = recvPacket(&server) ;	//recv pkt from the client....
	if( recvpkt == NULL ){	//connection failed...
		perror("\nError in receiving packet from the server.....\n");return NULL;	
	}
	else{
		if(recvpkt->header.controlBits[2] == '1'){
			//Server denied to connect client...
			return NULL;
		}
		char syn =  recvpkt->header.controlBits[1];
		char ack =  recvpkt->header.controlBits[0];
		int  ano =  recvpkt->header.AckNo;
		int  seq =  recvpkt->header.seqNo;
		arr[1] = seq;
		cout<<"SERVER seq#: "<<arr[1]<<endl;
		//cout<<"@client: SYNbit: "<<syn<<" Ackbit: "<<ack<<" ACK: "<<ano<<" seq#: "<<seq<<endl;
		if( syn == '1' &&  ack == '1' &&  ano == x+1){
			cout<<"@client: server Ack is received!\n";
			cout<<"@client: sending ack to server.................\n";
			int y = seq + 1;
			sendpkt.header.AckNo = y;						//next seq# 
			sendpkt.header.controlBits[0] = '1';	 	// Ackbit = 1				
			sendpkt.header.controlBits[1] = '0';	 	// SYNbit = 1				
			//cout<<"AckNum: "<<sendpkt.header.AckNo<<endl;
			int n = sendPacket(conn_sock,sendpkt, &server);
			if( n == -1 ){
				cout<<"@client: Error in sending.....\n\n";exit(0);
			}
		}
	}
	cout<<"\n========================client connected successfully========================\n";
	int * ptr = arr;
	return ptr;
}


//			Connection Closing Method... server method
int connectionCloseByServer(){
	cout<<"\n========================Connection Close By Server========================\n";
	
	int seqNo = generateRandomNo()%1000;
	while(seqNo <= 0 ){
		seqNo = generateRandomNo()%1000;
	}
	cout<<"@server: initial seq#: "<<seqNo<<endl;
	while(1){
	struct packet sendpkt;
	cout<<"@server: server waiting.......................\n";
	struct packet * recvpkt = recvPacket(&client) ;	//recv pkt from the client....
	if( recvpkt == NULL ){	//connection failed...
		perror("\nError in receiving packet from the client.....\n");return 0;	
	}
	else{
		if( recvpkt->header.controlBits[2] == '1' ){		// FINbit = 1
			int seq = recvpkt->header.seqNo;
			sendpkt.header.controlBits[0]  = '1';			//Ackbit = 1 
			sendpkt.header.controlBits[2]  = '1';			//FINbit = 1
			sendpkt.header.seqNo = seqNo;						//seq#
			sendpkt.header.AckNo = seq + 1;					//AckNo  = seq# + 1
			int status = sendPacket(conn_sock,sendpkt, &client);
			if( status == -1){
			cout<<"Error! packet not sent...\n";return 0;
			}
					//cout<<"1 pkt recv: -> client addr: "<<client.sin_port<<endl;
		}
		else if( recvpkt->header.controlBits[0] == '1' ){	//Ackbit = 1
			if( recvpkt->header.AckNo == seqNo + 1 ){
				cout<<"@server: client is dead!\n";
				break;
			}
			else{
				break;
			}
		}
	}
	}
	cout<<"\n========================One client closed successfully========================\n";
	return 1;
	sleep(10);
}

//				Connection close Method... client Method
bool  connectionCloseByClient(){
	cout<<"\n========================Connection Close By Client========================\n";
	bool closed = false;
	struct packet sendpkt;
	int x = generateRandomNo()%1000;
	cout<<"@client: initial seq#: "<<x<<endl;
	sendpkt.header.seqNo = x;						//starting from random seq# 
	sendpkt.header.controlBits[2] = '1';	 	// FINbit = 1				
	//cout<<"seqNo: "<<sendpkt.header.seqNo<<endl;
	//cout<<"SYNbit: "<<sendpkt.header.controlBits[1]<<endl;
	int n = sendPacket(conn_sock,sendpkt, &server);
	if( n == -1 ){
		cout<<"Error in sending.....\n\n";exit(0);
	}
	cout<<"@client: connection close request sent...!\n";
	cout<<"@client: client waiting for ACK..................\n";
	struct packet * recvpkt = recvPacket(&server) ;	//recv pkt from the client....
	if( recvpkt == NULL ){	//connection failed...
		perror("\nError in receiving packet from the server.....\n");return false;	
	}
	else{
		
		char fin  =  recvpkt->header.controlBits[2];
		char ack  =  recvpkt->header.controlBits[0];
		int  ano  =  recvpkt->header.AckNo;
		int  seq  =  recvpkt->header.seqNo;
		cout<<"@client: FINbit: "<<fin<<" Ackbit: "<<ack<<" ACK: "<<ano<<" seq#: "<<seq<<endl;
		if( fin == '1' &&  ack == '1' &&  ano == x+1){
			cout<<"@client: server Ack is received!\n";
			cout<<"@client: sending ack to server.................\n";
			int y = seq + 1;
			sendpkt.header.AckNo = y;						//next seq# 
			sendpkt.header.controlBits[0] = '1';	 	// Ackbit = 1				
			sendpkt.header.controlBits[2] = '0';	 	// FINbit = 1				
			//cout<<"AckNum: "<<sendpkt.header.AckNo<<endl;
			int n = sendPacket(conn_sock,sendpkt, &server);
			if( n == -1 ){
				cout<<"@client: Error in sending.....\n\n";exit(0);
			}
		}
	}
	cout<<"\n========================client connected successfully========================\n";
	return closed;
}

vector<packet> makePacket(char * data,char * srcPort,char * destPort,int seqNo,struct appHeader header)
{		seqNo += 1;				//????????????????? in client ??????????????????????????????
	
		/*if( (pktSize-1) <= 0 ){
		cout<<"Unable to make packet with this SIZE.... !\n";
		return NULL;
		}*/
		
      int sizeofData=strlen(data);
      int noOfPackets=0;
      vector<packet>Vector;
      if (sizeofData % 127==0)
      {
      	  noOfPackets=(sizeofData/127);
      }
      else
			noOfPackets=(sizeofData/127)+1;
       	
     cout<<"the number of packets made by transport layer is "<< noOfPackets<<endl;
     struct packet array[noOfPackets];
       for(int i=0;i<noOfPackets;i++)
       {   array[i].header.app=header;
       	    for(int j=0;srcPort[j]!='\0';j++)
       	    {
       	       array[i].header.srcPort[j]=srcPort[j];	
       	    }
       		for(int j=0;destPort[j]!='\0';j++)
       	    {
       	       array[i].header.destPort[j]=destPort[j];	
       	    }
       	    for(int m=0;m<3;m++)
       	    {
       	    	array[i].header.controlBits[m]='0';
       	    }
       		array[i].header.seqNo=seqNo;
       		seqNo++;
       		array[i].header.AckNo=-1;
       		//we donot need specify control bits
       		//here we specify checksum and window size later
       }
       int counter=0;
       int mm=0;
        for(int i=0;i<sizeofData;i++)
        {
             array[counter].header.data[mm]=data[i];
             if(mm==126)
             { if( counter == noOfPackets){
						array[counter].header.data[mm+1]='#';	//mm+1???????????????????
					}else{
             	array[counter].header.data[mm+1]='\0';
					}
                 Vector.push_back(array[counter]);
                 mm=-1;
                 counter++;
             }
             mm++;
        }
        if(mm!=0)
        { 	array[counter].header.data[mm]='#';	//mm
				Vector.push_back(array[counter]);
        }
        //testring purpose code
        for(int i=0;i<noOfPackets;i++)
        {
			  cout<<"\n###############################################\n";
			  cout<<"Packet : "<<i<<endl;
        	for(int j=0;Vector.at(i).header.data[j]!='\0';j++)
        	{
        		cout<<Vector.at(i).header.data[j];
        	}
        	cout<<"\n###############################################\n";
        	cout<<"the length of segment is "<<strlen(Vector.at(i).header.data)<<endl;
        	cout<<i<<" round completed"<<endl;
        }
        cout<<"the size of vector is "<<Vector.size()<<endl;
        //sleep(10);
        return Vector;
}

//==============================
struct clientInfo{
	int c_port;
	int s_seqNo;
	int c_seq;
	int pktSize;
	int pktNoToDrop;
	int N;		//Window Size...
	char path[20];
};

char * intTochar(int arg){
	stringstream ss;
	ss<<arg;
	string str=ss.str();
	unsigned int i=0;
	char * ret = new char [str.length() +1];
	for(;i<str.length();i++)
		ret[i]=str[i];
		ret[i] = '\0';
	return ret;
}

// 			GBN.....
int GBNImplementation(int server_port,char * path,vector<packet>Vector,int seqNo, int new_sock, struct sockaddr_in client, int N, int pktToDrop)
{  
	//recvfrom( new_sock, (struct Datagram *)& datagram, sizeof(datagram), 0, (struct sockaddr *)& client, (socklen_t *)&i );
	//udt_Send(sendpkt, path, ip, ip, RoutingTable, new_sock);

	char ip[20] = "127.0.0.1";	//? Need to update after reading file
	cout<<"\nGBN HAS TO FOLLOW PATH:----------> "<<path<<endl;
	cout<<"\nGBN>>> s_seqNo: "<<seqNo<<endl;
	cout<<"\nGBN>>> windSize: "<<N<<endl;
	
	//used variable in GBN Protocol
	int LastPktSeq=-1;
	int windsize = N;
	int counter=0;
	int n, sent;  
	int size=0;
	int c=0;
	int totalSize=Vector.size();
	int required_index=0;
	int AckPackets = 0;
	int required_seqNO=seqNo;
	
	//timeout
	struct timeval tv;
	tv.tv_sec = 10;				// RRT = 20 Seconds...
	tv.tv_usec = 0;
	
	//LOOP
	if( totalSize >= windsize){
		sent = windsize;
	}else{	
		sent = totalSize; 
	}
	
	for(int i=0; i<sent; i++){
		cout<<"Counter: "<<counter<<endl;
		cout<<"pktToDrop is: "<<pktToDrop-1<<endl;
		if( pktToDrop-1 != counter){
			size++;
			cout<<"Sequence No of packet: "<<i<<" : "<<Vector.at(counter).header.seqNo<<endl;
			int n = udt_Send(server_port,Vector.at(counter++), path, ip, ip, RoutingTable, new_sock); //sendPacket(new_sock,Vector.at(counter++), &client);
			if( n == -1 ){
			cout<<"Error in 1st packet sending.....\n\n";exit(0);
			}			
		}
		else{
			size++;
			counter++;
			cout<<"Pkt: "<<pktToDrop<<" has been dropped....\n";
			pktToDrop += pktToDrop;
			cout<<"\nNext Pkt to Drop is: "<<pktToDrop<<endl;
		}		
	}
		cout<<"\ntotal No of pkts: "<<totalSize<<endl;
		while(AckPackets<totalSize)
		{	
			struct packet recvpkt1;
			cout<<"\nGBN: entered in while\n";
			if( setsockopt(new_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0 ){
			cout<<"ERROR!\n";}
			else{ 
				
			int i = sizeof(client);
			time_t time1 = time(0);
			cout<<"value of tv.sec: "<<tv.tv_sec<<endl;
			cout<<"\nGBN: Waiting for Ack of Sent...\n";
			struct Datagram datagram;
			if( /*recvfrom( new_sock, (struct packet *)& recvpkt1, sizeof(recvpkt1), 0, (struct sockaddr *) &client, (socklen_t *)&i )*/recvfrom( new_sock, (struct Datagram *)& datagram, sizeof(datagram), 0, (struct sockaddr *)& client, (socklen_t *)&i ) < 0 )
			{		//TIMEOUT
				cout<<"\n\tTIMEOUT:\n";
				int k=counter-AckPackets;
				int index=required_index;
				cout<<"req_index: "<<index<<endl;
				cout<<"k: "<<k<<endl;
				cout<<"counter: "<<counter<<endl;
				cout<<"size: "<<size<<endl;
					
				while(index<size)
				{	
					/*cout<<"req_index: "<<index<<endl;
					cout<<"k: "<<k<<endl;
					cout<<"counter: "<<counter<<endl;
					cout<<"size: "<<size<<endl;*/
					
					index++;
					int n = udt_Send(server_port,Vector.at(counter-k), path, ip, ip, RoutingTable, new_sock); //sendPacket(new_sock,Vector.at(counter-k), &client);
					//cout<<"\nVector.at(counter-k).seqNo: "<<Vector.at(counter-k).header.seqNo<<endl;
					k--;
					if( n == -1 ){
					cout<<"Error in 1st packet sending.....\n\n";exit(0);
					}
					//cout<<"pkt with seqNo: "<<Vector.at(counter-k).header.seqNo<<" is sent..\n";
				}//while
				
				
			 }//if timeout
		else
			{	// ack recvd..........
				recvpkt1 = datagram.payload;
			if(required_seqNO==recvpkt1.header.AckNo)
			{	 LastPktSeq=recvpkt1.header.seqNo;
				cout<<"\n\tACK Received! ( "<< recvpkt1.header.AckNo <<" )with seq No of pkt is: "<<LastPktSeq;
				AckPackets++;  
				cout<<"\nNo of ACKed Packets: "<<AckPackets<<endl;
				required_seqNO++;
				tv.tv_sec = 10;
				//++counter;
				required_index++;
				cout<<"req_index: "<<required_index<<endl;
				cout<<"required_seqNO: "<<required_seqNO<<endl;
				cout<<"counter: "<<counter<<endl;
				cout<<"totalSize: "<<totalSize<<endl;
				if(counter<totalSize)
				{	cout<<" Demainding packet in going to sent....!";
					size++;
					//++counter;
					if( pktToDrop-1 != counter){
					n = udt_Send(server_port,Vector.at(counter++), path, ip, ip, RoutingTable, new_sock);//sendPacket(new_sock,Vector.at(counter++), &client);
					if( n == -1 ){
						cout<<"Error in  packet sending.....\n\n";exit(0);
						}	
					}
					else{					
					cout<<"Pkt: "<<pktToDrop<<" has been dropped....\n";
					pktToDrop += pktToDrop;
					cout<<"\nNext Pkt to Drop is: "<<pktToDrop<<endl;
					}
				}
				else{
					cout<<"NOTNOTNOT\n";
				} 
		} 
		else if (recvpkt1.header.AckNo>required_seqNO)		// Commulative ack....
		{	cout<<"\n\tCommulative Ack\n";
			
			tv.tv_sec = 10;
			 LastPktSeq=recvpkt1.header.seqNo;
			cout<<"\n\tACK Received! with seq No of pkt is: "<<LastPktSeq;
			int AckNo=recvpkt1.header.AckNo;
			int count=0;
			while(required_seqNO<AckNo+1)
			{	
			count++;
			AckNo--;
			}
			required_seqNO=recvpkt1.header.AckNo+1;
			AckPackets=count+AckPackets;
			for(int i=0;i<count;i++)
			{    ++counter;
			if(counter<totalSize)
			{  required_index++;
				size++;
				if( pktToDrop-1 != counter){
					n = udt_Send(server_port,Vector.at(counter), path, ip, ip, RoutingTable, new_sock); //sendPacket(new_sock,Vector.at(counter), &client);
					if( n == -1 ){
					cout<<"Error in  packet sending.....\n\n";exit(0);
					}
				}
				else{
					cout<<"Pkt: "<<pktToDrop<<" has been dropped....\n";
					pktToDrop += pktToDrop;
					cout<<"\nNext Pkt to Drop is: "<<pktToDrop<<endl;
				}
			}//if
			}//for
		}//else if
		else //Need to wait....
		{	cout<<"\n\tNEED TO WAIT\n";
			c++;
			if(c == 5){
				
			}
			else{
				time_t time2 = time(0);
				time_t time3 = time2 - time1;
				if( time3 == 0){
					cout<<"\n\nOOOOOOOOOOOOOOOOOOOOOOOOOOO\n";
					tv.tv_sec = 10;
				}else{
					tv.tv_sec = time3;
				}
				
				tv.tv_usec = 0;
			}
	
		}//else
	
	}//else
	
	}//setsockopt else
	cout<<"\nNo of ACKed Packets: "<<AckPackets<<endl;		
	}//while
	return LastPktSeq;
}//function end

//============ Make Routing Table
vector<node> makeRoutingTable(	Dijkstra * G
/* Here we will pass a vector<string>ip containing ip addresses for all nodes*/){
		int ** arr = G->read();
		vector<node> nodeVector;
		int port = 2000;
		for(int i=0; i<6; i++, port+=1000){
			struct node node1;
			node1.name = char(i+97);		//i=0, node.name = 'a'
			node1.port = port;
			node1.IP = "127.0.0.1";			// till now, this is static but filled with ip address from the file
			node1.cost = arr[i];
			nodeVector.push_back(node1);	// pushing in vector...
		}	
	return nodeVector;
}
//====== THREAD SUBROUTINE
void * handleClient( void * arg){
		
		cout<<"\n***************************\nhandleClient Thread is Called!\n*********************************\n";
		int n;
		char ip[20] = "127.0.0.1";
		struct sockaddr_in server,client;
		struct clientInfo *obj  = (struct clientInfo *)arg;
		cout<<"\n###########\nSERVER PORT: "<<obj->c_port<<"\n###########\n";
		cout<<"\nPATH to FOLLOW for Routing: "<<obj->path<<endl;
		cout<<"clientInfo: s_seqNo-> "<<obj->s_seqNo<<endl;
		cout<<"clientInfo: c_seq-> "<<obj->c_seq<<endl;
		cout<<"clientInfo: pktNoToDrop-> "<<obj->pktNoToDrop<<endl;
		cout<<"clientInfo: pktSize-> "<<obj->pktSize<<endl;
		cout<<"clientInfo: Window Size-> "<<obj->N<<endl;
		
		int N = obj->N;
		int clientSeqNo;
		int totalPkts = 0;//int pktNo = obj->pktNoToDrop;
		int prt = obj->c_port;
		
		//?????????????????????????????????????????
		int server_port = 0;
		if( strcmp(obj->path, "no") == 0){
			server_port = prt;
		}
		//===== Creating new Socket for Client Server Communication =====
		int new_sock = socket(AF_INET, SOCK_DGRAM, 0);
		if( new_sock == -1 )
		{	
			cout<<"failed to create new socket in handleClient...\n";
		}	
		server.sin_family = AF_INET;
		server.sin_port   =htons(prt);
		cout<<"server sin port is "<<server.sin_port<<endl;
		server.sin_addr.s_addr = inet_addr("127.0.0.1");
		memset( &(server.sin_zero), '\0', 8);
		n = bind( new_sock, ( struct sockaddr *)& server, sizeof( struct sockaddr) );
		if(n==-1)
		{	
			cout<<"failed to bind server with new port\n";
		}
		
		vector<string>Vector;
		//====> Receiving Packet from Client... 
		while(1){
			
		struct Datagram datagram;		//Network layer Packet
		struct packet recvpkt1;			//Segment
		struct packet *recvpkt;
		int i = sizeof(client);
		cout<<"\n\n======================\nOn receiving packet....\n\n";
		
		//Here our 'recv method' is not used because it was not working well in some situations, 
		// [ it returns a pointer to the packet structure ]
		
		n = recvfrom( new_sock, (struct Datagram *)& datagram, sizeof(datagram), 0, (struct sockaddr *)& client, (socklen_t *)&i );
		if( n == -1 ){
			perror("\nError in receiving packet from the client.....\n");continue;
		}
		
		recvpkt1 = datagram.payload;		//actual data
		recvpkt = & recvpkt1;
		cout<<"\n====> Packet received, identifying packet type and code";
		if(recvpkt->header.app.type[0] == '1' && recvpkt->header.app.type[1] == '1' ){
				//first phase requirement 
			cout<<"\n--------------------------\n\tSimulation Starts....\n--------------------------\n";
		}
		else if( recvpkt->header.app.type[0] == '0' && recvpkt->header.app.type[1] == '0' ){	//list files	
			cout<<"\nList files...\n";
			
			//	   Check packet Code
			if( checkCode(*recvpkt) == "FILE_LIST_REQUEST" ){
				cout<<"\t\t====> FILE_LIST_REQUEST\n";
				//		Make a Response Packet	
				struct appHeader app = makeAppHeader("00001");	// app header...
				Vector = sendCwdFiles();		//This method returns Current working directory files
				stringstream ss;
				ss<<Vector.size();
				string str=ss.str();
				
				unsigned int p=0;
				for(unsigned int i=0;i<str.length();i++,p++)
					app.option[p]=str[i];	
				app.option[p] = '$';				//indication for end of Option Field...
				
				//		Finding data length to be send to receiver...
				int datalength = 0;p=0;
				for(; p<Vector.size(); p++){	
				  datalength += Vector.at(p).length();
				}
				datalength += p;
				char * datapkt = new char[datalength+1];	
				
				//		Coping data
				p = 0; 
				for(unsigned int k=0;k<Vector.size();k++){	
				  string s=Vector.at(k);
				  for(unsigned int i=0; i<s.length(); i++,p++)
					 datapkt[p]=s[i];
				  datapkt[p++] = '$';
				}datapkt[p] = '\0';
				
				cout<<"\ndata is: \n";
				for(int i=0; datapkt[i] != '\0'; i++){
					cout<<datapkt[i];
				}cout<<endl;
				
				//		Placing source and destination ports for Multiplexing and Demultiplexing
				char * srcport = intTochar(server.sin_port);
				char * destPort = intTochar(obj->c_port);
				int seq  = obj->s_seqNo;
				cout<<"Data length is: "<<datalength<<endl;
				cout<<"srcport: "<<srcport<<endl;
				cout<<"destport: "<<destPort<<endl;
				cout<<"Sequence No: "<<seq<<endl;
				
				//		Making Data packets [Transport Layer Responsibility]
				vector<packet> packets = makePacket( datapkt, srcport, destPort, seq, app );
				totalPkts = packets.size();cout<<"total pkts: "<<totalPkts<<endl;
				
				//    Calling Go Back N to deliver packets in parallel
				clientSeqNo = GBNImplementation(server_port,obj->path ,packets, (obj->s_seqNo)+1, new_sock, client, N , 2);//pktNo);
				cout<<"Server Next Sequence Number is: "<<obj->s_seqNo+totalPkts<<endl;
				cout<<"Next Seq No after GBN is: "<<clientSeqNo<<endl;clientSeqNo++;
				obj->s_seqNo = totalPkts + obj->s_seqNo;
				
				}//FILE_LIST_REQUEST...
			
			}//FILE_LIST END...
			
			//FILE_DOWNLOAD REQUEST...
			else if( recvpkt->header.app.type[0] == '0' && recvpkt->header.app.type[1] == '1' ){	
				
				cout<<"DOWNLOADING: \trecv pkt SeqNO: "<<recvpkt->header.seqNo<<endl;
				cout<<"SERVER is expecting this Sequence No: "<<clientSeqNo<<endl;
				
				int serverSeqNo = obj->s_seqNo + 1;
				cout<<"Client Next send pkt with this seq No: "<<clientSeqNo<<endl;
				cout<<"Server Next send pkt with this seq No: "<<serverSeqNo<<endl;
			
				if( checkCode(*recvpkt) == "FILE_DOWNLOAD_REQUEST" ){
					cout<<"\n=====> FILE_DOWNLOAD_REQUEST!\n";
					
					//			FILE_DOWNLOAD_RESPONSE...
					struct appHeader aapp = makeAppHeader("01001");
					struct packet sendpkt;
					sendpkt.header.app = aapp;				//app-layer header is placed
					
					int size = fileSize( fileName(*recvpkt) );		//getting size of requested file...
					stringstream ss;
					ss<<size;
					string str=ss.str();
					unsigned int p=0;
					
					for(unsigned int i=0;i<str.length();i++,p++)
						sendpkt.header.app.option[p]=str[i];
					sendpkt.header.app.option[p] = '$';		//Indication for end of Option Field...
					
					char * array=intTochar(server.sin_port);
					unsigned int i=0;
					for(;array[i]!='\0';i++)
					{
						sendpkt.header.srcPort[i]=array[i];
					}
					sendpkt.header.srcPort[i] = '\0';
					
					char * array1=intTochar(obj->c_port);
					i=0;
					for(; array1[i] != '\0'; i++)
					{		sendpkt.header.destPort[i]=array1[i];
					}
					sendpkt.header.destPort[i] = '\0';
					cout<<"Destination Port: "<<sendpkt.header.destPort<<endl;
					
					// set seq number in header.. 
					sendpkt.header.seqNo=serverSeqNo++; 
					// set control bits
					sendpkt.header.controlBits[0]='0';
					sendpkt.header.controlBits[1]='0';
					sendpkt.header.controlBits[2]='0';
					
					
					//				Send FILE_DOWNLOAD_RESPONSE...
					n = udt_Send(server_port,sendpkt, obj->path, ip, ip, RoutingTable, new_sock);//sendto(new_sock, &sendpkt, sizeof(struct packet), 0, (struct sockaddr *)&client, sizeof(client));
					if( n == -1 ){	
						cout<<"Error in sending packet to receiver.....\n\n";
				   }
				}
				else if( checkCode(*recvpkt) == "FILE_DOWNLOAD_START" ){
					//			FILE_DOWNLOAD_TRANSFER...
					struct packet sendpkt;
					struct appHeader app1 = makeAppHeader("01011");
					sendpkt.header.app = app1;
				   string filename = fileName(*recvpkt);
				   cout<<"File Name: "<<filename<<endl;
				   int size = fileSize(filename);		//returns size of given file...
				   cout<<"Size of requested File is: "<<size<<endl;
				   char * datapkt = new char[size+1]; 
				   int data=0;
      			char ch;
      			fstream fin(filename.c_str(), fstream::in);
               // int pkt = 1;           
   				while (fin >> noskipws >> ch) 	// FILE_TRANSFER...
   				{	
					   datapkt[data++] = ch;
					}
					datapkt[data] = '\0';
					cout<<"\nRequested FILE DATA: "<<datapkt<<endl;
					char * srcport = intTochar(server.sin_port);
					char * destPort = intTochar(obj->c_port);
					vector<packet> packets = makePacket(datapkt, srcport, destPort, serverSeqNo-1, app1);
					cout<<"Total pkts formed: "<<packets.size();
					
					clientSeqNo = GBNImplementation(server_port,obj->path,packets, serverSeqNo, new_sock, client, N, 6  );
					cout<<"\n\n\nAfter GBN has completed Sending File, client seq No: "<<clientSeqNo<<endl;
					//			FILE_DOWNLOAD_COMPLETED...
					
					obj->s_seqNo = obj->s_seqNo + totalPkts;
					clientSeqNo++;
					char * srcport1 = intTochar(server.sin_port);
					char * destPort1 = intTochar(obj->c_port);
					struct appHeader app = makeAppHeader("01100");
					struct packet sendpkt1;
					sendpkt1.header.app = app;
					for(int j=0;srcport1[j]!='\0';j++)
					 {
						 sendpkt1.header.srcPort[j]=srcport1[j];	
					 }
					for(int j=0;destPort1[j]!='\0';j++)
					 {
						 sendpkt1.header.destPort[j]=destPort1[j];	
					 }
					
					cout<<"\n**************\nCOMPLETING MSG: "<<serverSeqNo+packets.size()<<endl;
					sendpkt1.header.seqNo = serverSeqNo + packets.size();
					n = udt_Send(server_port,sendpkt1, obj->path, ip, ip, RoutingTable, new_sock);//sendto(new_sock, &sendpkt1, sizeof(struct packet), 0, (struct sockaddr *)&client, sizeof(client));
					if( n == -1 ){	
						cout<<"Error in sending packet to receiver.....\n\n";
					}
					sleep(10);
					cout<<"\nGOING BACK TO WHILE LOOP\n";
					struct Datagram datagram1;
					int i = sizeof(client);
					cout<<"\n\n======================\nOn receiving packet....\n\n";
					struct packet recvp2;
					n = recvfrom( new_sock, (struct Datagram *)& datagram1, sizeof(datagram1), 0, (struct sockaddr *)& client, (socklen_t *)&i );//recvfrom( new_sock, (struct packet *)& recvp2, sizeof(recvp2), 0, (struct sockaddr *)& client, (socklen_t *)&i );
					if( n == -1 ){
						perror("\nError in receiving packet from the client.....\n");continue;
					}
					recvp2 = datagram1.payload;
					char c = recvp2.header.app.type[0];
					cout<<"Type[0]: "<<c<<endl;
					cout<<"Type[1]: "<<recvp2.header.app.type[1]<<endl;
					//recvpkt1.header.app.type[1] = '9';
					recvpkt1 = recvp2;
						
				}
			}
		}
pthread_exit(0);
}

//============== Main Method...
int main(int argc, char * argv[]){
   

	vector<string>Vector;
	vector<string>filevector;
	int result , port_no ;
	string ip ;
	//-------Socket Creation...
	conn_sock = createUdpSocket();
	if( conn_sock == -1 ){
		perror("\nError in making socket");
		exit(0);
	}
	//====== Server Code ========
	if( strcmp(argv[1], "s") == 0 ){	
		
		int initialusers = 0;
		int MAX_CLIENTS  = 4;
		//int * client_port = new int [MAX_CLIENTS];
		cout<<"\n\t^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\t  This is UDP SERVER...!\n\t^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n";
		Dijkstra G;
		RoutingTable = makeRoutingTable(&G);
		//for(int i=0; i<6; i++){
		//cout<<"\n\n"<<RoutingTable.at(i).name<<"\t"<<RoutingTable.at(i).port<<"\t"<<RoutingTable.at(i).IP<<"\nMetric: ";
			//for(int j=0; j<6; j++){
			//	cout<<RoutingTable.at(i).cost[j]<<' ';
			//}
		//}
		// char dest = jis node sy end_host data packet receive kry ga...!
		char * path = shortestPathAlgorithm(&G,'a');	// Here 'a' is the destination node where end host would recv pkts...
		cout<<"Shortest Path is: "<<path<<endl;
		//int i=0;//for(;path[i]!='\0'; i++);//cout<<"Size of path: "<<i<<endl;
		
		filevector = readConfigurationFile("ServerConfiguration.txt");
		port_no = atoi(filevector.at(0).c_str());
		int pktsize = atoi(filevector.at(1).c_str()); 
		int pktToDrop = atoi(filevector.at(2).c_str());
		int windsize = atoi(filevector.at(3).c_str());
		//--------binding server...
		server = bindserver(port_no, "127.0.0.1");
		result  = bindServer(conn_sock, server);
		if( result == -1 ){
			cout<<"Error in Binding server... \n\n";return 0;
		}
	
		//--------Main Functionality...
   	while(1){
			
		   //-----Three Way Handshaking...
			int * sarr 	= threeWayHandshaking( initialusers , MAX_CLIENTS );
			if( sarr != NULL ){

				initialusers++;
				int s_seq = sarr[0];
				int c_seq = sarr[1];
				int c_port = sarr[2];
			
			cout<<"\n\n(In Main) seq#: "<<s_seq<<"   port# "<<c_port<<endl;
			//	cout<<"\n\n(In Main) seq#: "<<port[0]<<"   port# "<<port[1]<<endl;
			cout<<"\nServer is waiting....\n";
			int prt = c_port+ 100;
			stringstream ss;
			ss<<prt;
			string newport = ss.str();
			char prt2[10];
			unsigned int j=0;
			for(; j<newport.length(); j++){
			prt2[j] = newport[j];
			//cout<<prt2[i];
			}
			prt2[j]='\0';
			cout<<"\n##############\nPORT Sending to Client:"<<prt2<<endl;
			int ret = sendto(conn_sock, prt2, 10,0, (struct sockaddr *)&client, sizeof(client) );
			if( ret==-1){cout<<"fail to send new port to client in MAIN()!\n";}
			
			struct clientInfo *ci = new clientInfo;			//used to pass multiple arguments to THREAD Subroutine
			
			//Placing Path to follow for Routing....
			int i=0;
			for(; path[i] != '\0'; i++){
				ci->path[i] = path[i];
				cout<<ci->path[i];
			}	
			ci->path[i] = '\0';cout<<endl;
			
			//Placing some other Info...
			ci->s_seqNo = s_seq;
			ci->c_port  = prt;//c_port+100;
			ci->c_seq   = c_seq;
			ci->pktNoToDrop = pktToDrop;
			ci->pktSize = pktsize;
			ci->N = windsize;
			cout<<"\n\n===========> ci->c_port : "<<ci->c_port<<endl;
			//Creating an independent Thread
			pthread_t tid;							//Thread id
			pthread_attr_t att;					//Thread Attribute
			pthread_attr_init(&att);			//Initialization
			pthread_attr_setdetachstate(&att, PTHREAD_CREATE_DETACHED);
			pthread_create(&tid, &att, handleClient, (void *) ci );
			cout<<"MAIN HAS CREATED NEW THREAD TO FOR CLIENT REQUESTS...\n";
			}else{
				cout<<"\n******************************\n\tSERVER HAS DENIED TO CONNECT CLIENT\n\t\t CONNECTION FAILED! \n******************************\n";
			}
		}		
		//SERVER Side end
	}//if statement
	
	//====== Client Code =======
	else if( strcmp(argv[1],"c") == 0 )
	{	
		cout<<"\n\t^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\t  This is UDP CLIENT...!\n\t^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n";
		int n;
		vector<string> list;
		//char buff[128];
		filevector = readConfigurationFile("ClientConfiguration.txt");
		ip = filevector.at(0);
		int ackToDrop = atoi(filevector.at(4).c_str());
		string fileName = filevector.at(2);
		cout<<"ackToDrop: "<<ackToDrop<<endl;
		cout<<"File Name: "<<fileName<<endl;
		port_no = atoi(filevector.at(1).c_str());
		server = bindserver(port_no, ip);
		
		int *seqA = establishConnection(conn_sock);	//doing three way handShaking	
		if( seqA == NULL){
			cout<<"\n******************************\n\tSERVER HAS DENIED TO CONNECT CLIENT\n\t\t CONNECTION FAILED! \n******************************\n";
			return 0;
		}
		int seqArr[2];
		seqArr[0] = seqA[0];
		seqArr[1] = seqA[1]+1;
		cout<<"\n FROM EstabConn: client seq#: "<<seqA[0]<<"  server seq#: "<<seqA[1]<<endl;
		
		char arr[10];
		cout<<"\n^^^^^^^^^^^^^^^^^^^^^^^^\nCLIENT CONNECTED SUCCESSFULLY\n^^^^^^^^^^^^^^^^^^^^^^^^\n";
		int i1 = sizeof(server);
		n = recvfrom(conn_sock, arr, 10, 0, (struct sockaddr *)&server, (socklen_t *) &i1);
		if( n == -1 )
		{	cout<<"Error in Recv New PORT.....\n\n";
		}
		
		
		int newport = atoi(arr);
		cout<<"\n*******************************\nClient has recv new PORT = << "<<newport<<" >>"<<endl;
		//s.sin_port = newport;
		int t = socket(AF_INET, SOCK_DGRAM,0);
		if(t==-1)
		{cout<<"fail to create new socket for client!\n";}
		
		struct sockaddr_in myaddr;
		int np = newport-10;//for routing
		myaddr.sin_port =htons(np);
		myaddr.sin_family = AF_INET;
		myaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
		int o = bindServer(t,myaddr);
		if(o==-1){								//for routing
			cout<<"fail to BIND...\n";}
		//server.sin_port = htons(np);
		cout<<"myaddr-port: "<<myaddr.sin_port<<endl;
		//cout<<"\n*******************************\nClient has recv new PORT = << "<<htons (newport)<<" >><< "<<server.sin_port<<" >>"<<endl;
		
		Dijkstra G;
		vector<node> RoutingTable = makeRoutingTable(&G);
		
		//############ Change Path Here
		
		char * path = shortestPathAlgorithm(&G,'e');	// Here 'e' is the destination node where end host would recv pkts...
		cout<<"Client has found shortest Path : "<<path<<endl;
		
		int server_port = 0;
		if( strcmp(path, "no") == 0 ){
			server_port = np;			//No routing
		}
		
		while(1){
		//struct packet sendmsg;
		char buff[128];
		memset(buff,0,128);
		char  ip[20] = "127.0.0.1";
		cout<<"<<Please Enter: >>\n";	
		cout<<"\t 1 to view files"<<endl;
		cout<<"\t 2 to download file"<<endl;
		cout<<"\t 3 to close client connection"<<endl;
		cin>>result;
		int ACK=0;
		int ackCounter = 0;
		if (result == 1)
		{	
			/*
			###############################
			ERROR in DestPORT so correct it!
			###############################
			*/
			
			struct appHeader app1=makeAppHeader("00000");
			struct packet sendmsg;
			sendmsg.header.app = app1;
		    char * array=intTochar(client.sin_port);
		    cout<<"SOURCE PORT->array = << "<<array<<" >>"<<endl;
		
		    for(unsigned int i=0;i<strlen(array);i++)
		     {
				    sendmsg.header.srcPort[i]=array[i];
			 }
			 //char * array1=intTochar(server.sin_port);
			 
			 cout<<"DESTINATION PORT->array = << "<<arr<<" >>"<<endl;
		
		     for(unsigned i=0;arr[i] != '\0';i++)
		     {
				    sendmsg.header.destPort[i]=arr[i];
				    //cout<<sendmsg.header.destPort<<" ";
			 }
			//cout<<"SERVER new PORT = << "<<server.sin_port<<" >>"<<endl;
			cout<<"DESTINATION PORT = << "<<sendmsg.header.destPort<<" >>"<<endl;
		
			// set seq number in header.. 
			sendmsg.header.seqNo=seqArr[0]; 
			// set control bits
			sendmsg.header.controlBits[0]='0';
			sendmsg.header.controlBits[1]='0';
			sendmsg.header.controlBits[2]='0';
			++seqArr[0];
			//struct Datagram datagram;
			
			cout<<"Path to sent: "<<path<<endl;
			n = udt_Send(server_port,sendmsg, path, ip, ip, RoutingTable, t);
			
			/*n = sendPacket(conn_sock,sendmsg, &server);*/
			if( n == -1 )
			{
				cout<<"Error in sending DATAGRAM in main line#1463.....\n\n";
				exit(0);
			}
			cout<<"JK.Packet is sent from client...\n";
			//ack bit on file receiving purpose
			sendmsg.header.controlBits[0]='1';
			// client recieving response msg from the server..
			string data_str = "";
			while (true)
			{	
				cout<<"In while\n";	
				bool flag = false;
				bool endData = false;
				struct packet  recvmsg;
				struct Datagram d;
				
				/*
				cout<<"myaddr-port: "<<myaddr.sin_port<<endl;
				myaddr.sin_port = np; //htons(np);
				cout<<"value of np: "<<np<<endl;
				cout<<"assign myaddr-port: "<<myaddr.sin_port<<endl; 
				cout<<"BELOW, receiving on new port: "<<myaddr.sin_port<<"\n";
				*/
				
				int i = sizeof(myaddr);
				n = recvfrom( t, (struct Datagram *)& d, sizeof(d), 0, (struct sockaddr *)& myaddr, (socklen_t *)&i );//recvfrom(conn_sock, (struct packet *)&recvmsg, sizeof(recvmsg),0,(struct sockaddr *)&server, (socklen_t *)&i);
				if( n==-1){
					cout<<"ERROR in RECV!\n";
				}
				cout<<"PACKET RECEIVED!!!!!!!!!!!!!!!!!!!!!!!\n";	
				recvmsg = d.payload;	
				vector<packet> recvBuffer;
				
				// Checking duplicate..
				cout<<"the required sequence number of client is "<<seqArr[1]<<endl;
				cout<<"the server sent seq number is "<<recvmsg.header.seqNo<<endl;
				if (seqArr[1] == recvmsg.header.seqNo)
				{
					cout<<"entered in ist if"<<endl;
					sendmsg.header.AckNo =recvmsg.header.seqNo;
					recvBuffer.push_back(recvmsg);
					ackCounter++;
					++seqArr[1];
				}
				else if (seqArr[1] < recvmsg.header.seqNo)
				{
					sendmsg.header.AckNo =seqArr[1];
				} 
				else if (seqArr[1] > recvmsg.header.seqNo)
				{
					//sendmsg.header.AckNo =seqArr[1];
					sendmsg.header.AckNo = recvmsg.header.seqNo;
				}
			
				sendmsg.header.seqNo=seqArr[0]++;
				if( ackToDrop != ackCounter ){
					cout<<"Ack sent "<<sendmsg.header.AckNo<<endl;
					n = udt_Send(server_port,sendmsg, path, ip, ip, RoutingTable, t);//sendPacket(conn_sock,sendmsg, &server);
					if( n == -1 )
					{
						cout<<"Error in sending.....\n\n";
						exit(0);
					}
					cout<<"\nACK Packet with AckNum: "<<ACK<<" is sent from client...\n";
					flag = false;
				}
				else{
					flag = true;
					cout<<"ACK of pkt: "<<ackCounter<<" has been DROPPED!\n";
					cout<<"We have received pkt with seq: "<<(seqArr[1]-1)<<" but its ack has been Dropped....\n";
					ackToDrop += ackToDrop;
					cout<<"Next Ack to DROP: "<<ackToDrop<<endl;
					
				}
				if( !flag){				
				cout<<"\n\t\tServer recvmsg seqNo: "<<recvmsg.header.seqNo<<endl;
				cout<<"\nData Length: "<<strlen(recvmsg.header.data)<<endl;
				for(int i=0;recvmsg.header.data[i] != '\0';i++){
					cout<<recvmsg.header.data[i];
				 }
				 
				// now we open the header and display response msg..
				if (recvmsg.header.app.type[0] =='0' && recvmsg.header.app.type[1] == '0' && recvmsg.header.app.code[0] == '0' && recvmsg.header.app.code[1] == '0' && recvmsg.header.app.code[2] == '1')
				{
						string str ="";
						for (int i=0; i<16; i++)
						{
							if (recvmsg.header.app.option[i] != '$')
							{
								str.append(1u,recvmsg.header.app.option[i]);
							}
						}
						for (int i=0; i<128; i++)
						{
							if (recvmsg.header.data[i] == '#')
							{	if( !flag ){	// we have not sent ACK so don't terminate and wait to recv pkt again....
								endData = true;
								//flag2 = false;
								break;
								}
								else{
									// data has been received!
									//flag2 = true;
									//data_str = "";
									break;
								}
							}
							else if(recvmsg.header.data[i] != '$')
							{
								data_str.append(1u,recvmsg.header.data[i]);
							}
							else 
							{	//cout<<"\npushing str: "<<data_str<<"\n\n";
								bool found = false;
								for(unsigned int k=0; k<list.size(); k++){
								//	cout<<list.at(k)<<" ";
									if( list.at(k) == data_str ){
										//cout<<"\n\nstring = "<<data_str<<" already exists..!";
										found = true;
									}
								}
								if( !found ){
									char c = data_str[data_str.length()-1];
									if( c != '~')
									list.push_back(data_str);
								}
								data_str="";
							}
						}//for...
						if (endData)
							break;
				}
			}//if ack in not dropped
		}//while
			
			// now we print the list of availble files....
			cout<<"\n\n================================================\n";
			cout<<"			Following files are availble for download\n\n";
			for (unsigned int i = 0; i<list.size(); i++)
			{
				cout<<"Length: "<<list.at(i).length()<<" Name:"<<list.at(i)<<endl;
			}
		}
		else if (result == 2)
		{	//++seqArr[1];
			ackCounter = 0;
			vector<packet> recvBuffer;	
			do{
				cout<<"\nEnter file name: ";
				cin.ignore();
				cin.getline(buff, 128);
				bool fileExists = false;
				for(unsigned int i=0; i<list.size(); i++){
					
					string str = list.at(i);
					if( strcmp(str.c_str(), buff) == 0 ){
						fileExists = true;
						break;
					}
				}
				if(fileExists){
					break;
				}
				else{
					cout<<"File not Exists....\n";
				}
				
			}while(true);
			//			FILE_DOWNLOAD_REQUEST...
			struct appHeader app;
			struct packet sendmsg;
			app=makeAppHeader("01000");
			int data=0;
			//cout<<"\nType & Code fields in packet: "<<sendpkt<<endl;
			for(; buff[data] != '\0'; data++){
				sendmsg.header.data[data] = buff[data];
			}
			sendmsg.header.data[data]='\0';
			// appheader.. 
			sendmsg.header.app = app;
			
			// set source port in header.
			char * array=intTochar(client.sin_port);
		    for(unsigned int i=0;array[i] != '\0';i++)
		     {
				    sendmsg.header.srcPort[i]=array[i];
			 }
			 // set destination port in header.
			 //char * array1=intTochar(server.sin_port);
		    unsigned int k=0;
		    for(;arr[k] !='\0';k++)
		    {
				  sendmsg.header.destPort[k]=arr[k];
			 }
			 sendmsg.header.destPort[k] = '\0';
			 cout<<"Destination PORT: "<<sendmsg.header.destPort<<endl;
			 // set seq number in header.. 
			sendmsg.header.seqNo=seqArr[0]++; 
			// set control bits
			sendmsg.header.controlBits[0]='0';
			sendmsg.header.controlBits[1]='0';
			sendmsg.header.controlBits[2]='0';
			
			//++seqArr[0];
			n = udt_Send(server_port,sendmsg, path, ip, ip, RoutingTable, t);//sendPacket(conn_sock,sendmsg, &server);
			if( n == -1 )
			{
				cout<<"Error in sending.....\n\n";
				exit(0);
			}
			cout<<"Filename is sent from client...\n";
			 
			struct packet recvmsg1;
			struct packet * recvmsg;
			struct Datagram datagram;
			
			int i = sizeof(myaddr);
			n = recvfrom( t, (struct Datagram *)& datagram, sizeof(datagram), 0, (struct sockaddr *)&myaddr, (socklen_t *)&i ); //recvfrom( conn_sock,(struct packet *)& recvmsg1, sizeof(recvmsg1), 0, (struct sockaddr *)& server, (socklen_t *)&i );
			if( n == -1 )
			{
				perror("\nError in receiving");
				exit(1);
			}
			recvmsg1 = datagram.payload;
			recvmsg = &recvmsg1;
			if (recvmsg->header.app.type[0] =='0' && recvmsg->header.app.type[1] == '1' && recvmsg->header.app.code[0] == '0' && recvmsg->header.app.code[1] == '0' && recvmsg->header.app.code[2] == '1')
			{		cout<<"\n FILE_DOWNLOAD_RESPONSE is received...\n";
					//			Get file size from Option field...
					string str ="";
					for (int j=0; j<16; j++)
					{
						if (recvmsg->header.app.option[j] != '$')
						{
							str.append(1u,recvmsg->header.app.option[j]);
						}
					}
				//			FILE_DOWNLOAD_START...
				struct appHeader app1 = makeAppHeader("01010");
				sendmsg.header.app = app1;
				int data=0;
				for(; buff[data] != '\0'; ++data){
					sendmsg.header.data[data] = buff[data];		//placing file name to download....
				}
				sendmsg.header.data[data]='\0';
				n = udt_Send(server_port,sendmsg, path, ip, ip, RoutingTable, t);//sendto(conn_sock, &sendmsg, sizeof(struct packet), 0, (struct sockaddr *)&server, sizeof(server));
				if( n == -1 )
				{
					cout<<"Error in sending.....\n\n";
					exit(0);
				}
				cout<<"\nFILE_DOWNLOAD_START is sent...\n";
				//		FILE_DOWNLOAD_TRANSFER...
				ofstream out;
				cout<<"\nFILENAME: "<<fileName<<endl;
				out.open(fileName.c_str(),ios::out);
				bool isfileCompleted = false;
				while(!isfileCompleted){
					
					int i = sizeof(myaddr);
					cout<<"\nDemanding for: "<<seqArr[1]<<endl;
					//memset(recvmsg, 0, 128);
					struct packet recvmsg1;
					struct packet *recvmsg;
					struct Datagram datagram;
					cout<<"\nReceiving file...\n";
					n = recvfrom( t, (struct Datagram *)& datagram, sizeof(datagram), 0, (struct sockaddr *)&myaddr , (socklen_t *)&i );//recvfrom( conn_sock, (struct packet *)&recvmsg1, sizeof(recvmsg1), 0, (struct sockaddr *)& server, (socklen_t *)&i );
					if( n == -1 )
					{
						perror("\nError in receiving");
						exit(1);
					}
					cout<<"\nfile recvd...\n";
					recvmsg1 = datagram.payload;
					recvmsg = &recvmsg1;
					if (seqArr[1] == recvmsg->header.seqNo)
					{	cout<<recvmsg->header.app.type[0]<<recvmsg->header.app.type[1]<<recvmsg->header.app.code[0]<<recvmsg->header.app.code[1]<<recvmsg->header.app.code[2]<<"\n";
					//	sleep(5);
						//recvmsg = &recvmsg1;
						if (recvmsg->header.app.type[0] =='0' && recvmsg->header.app.type[1] == '1' && recvmsg->header.app.code[0] == '1' && recvmsg->header.app.code[1] == '0' && recvmsg->header.app.code[2] == '0')
						{	cout<<"\n FILE_DOWNLOAD_COMPLETED....\n";
							struct appHeader app = makeAppHeader("99999");
							struct packet sendmsg;
							sendmsg.header.app = app;
							n = udt_Send(server_port,sendmsg, path, ip, ip, RoutingTable, t);//sendto(conn_sock, &sendmsg, sizeof(struct packet), 0, (struct sockaddr *)&server, sizeof(server));
								if( n == -1 )
								{
									cout<<"Error in sending.....\n\n";
									exit(0);
								}
							out.close();
							break;
						}
						else if (recvmsg->header.app.type[0] =='0' && recvmsg->header.app.type[1] == '1' && recvmsg->header.app.code[0] == '0' && recvmsg->header.app.code[1] == '1' && recvmsg->header.app.code[2] == '1')
						{	cout<<"\nData recieved...\n";
							
							string str ="";
							int j=0;
							for (; recvmsg->header.data[j] != '\0'; j++)
							{ //cout<<recvmsg->header.data[j];
								if( recvmsg->header.data[j] == '#' ){
									cout<<"\n\n################################\n\n";
									recvmsg->header.data[j+1] = '\0';
								}
								else
								str.append(1u,recvmsg->header.data[j]);
							}
						
							out<<str;	
						}
					sendmsg.header.AckNo =recvmsg->header.seqNo;			
					++seqArr[1];
					recvBuffer.push_back(*recvmsg);
					ackCounter++;
				}
				else if (seqArr[1] < recvmsg->header.seqNo)
				{
					sendmsg.header.AckNo =seqArr[1];
				} 
				else if (seqArr[1] > recvmsg->header.seqNo)
				{
					//sendmsg.header.AckNo =seqArr[1];
					sendmsg.header.AckNo = recvmsg->header.seqNo;
				}	
				//sendmsg.header.app = app1;
				sendmsg.header.seqNo=seqArr[0]++;
				if( ackToDrop != ackCounter ){
					cout<<"Ack sent "<<sendmsg.header.AckNo<<endl;
					n = udt_Send(server_port,sendmsg, path, ip, ip, RoutingTable, t);//sendPacket(conn_sock,sendmsg, &server);
					if( n == -1 )
					{
						cout<<"Error in sending.....\n\n";
						exit(0);
					}
					cout<<"\nACK Packet with AckNum: "<<ACK<<" is sent from client...\n";
				}
				else{	
					cout<<"ACK of pkt: "<<ackCounter<<" has been DROPPED!\n";
					cout<<"We have received pkt with seq: "<<(seqArr[1]-1)<<" but its ack has been Dropped....\n";
					ackToDrop += ackToDrop;
					cout<<"Next Ack to DROP: "<<ackToDrop<<endl;
				}
				
				n = udt_Send(server_port,sendmsg, path, ip, ip, RoutingTable, t);//sendPacket(conn_sock,sendmsg, &server);
				if( n == -1 )
				{
					cout<<"Error in sending.....\n\n";
					exit(0);
				}
				//cout<<"\nACK Packet with AckNum: "<<ACK<<" is sent from client...\n";
			
				cout<<"\n\t\tServer recvmsg seqNo: "<<recvmsg->header.seqNo<<endl;
				cout<<"\nData Length: "<<strlen(recvmsg->header.data)<<endl;
				for(int i=0;recvmsg->header.data[i] != '\0';i++){
					if(recvmsg->header.data[i] == '#'){
							cout<<"\nSHANI file is completed! Now close File.....\n";
						//	isfileCompleted = true;
					}
					cout<<recvmsg->header.data[i];
				 }	
				}	
			}
		}//result 2
		else if( result == 3 ){
			closeSocket(conn_sock);
			cout<<"\n\tClient is Terminated...\n";
			break;
		}
	}//while
	
}//client else if
}	//END OF PROGRAM.........
