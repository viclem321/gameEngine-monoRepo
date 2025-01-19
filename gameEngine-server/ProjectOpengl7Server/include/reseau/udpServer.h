#include <winsock2.h>
#include <ws2tcpip.h>
#include<limits>

const int N_MAX_CLIENTS = 100;
const uint16_t NPORT = 5001;

struct DatagramByServer
{
    uint16_t nClient;  // 65535 mean dont have yet num
    uint16_t id;
    uint16_t ack;
    uint64_t previousAcks;
    uint8_t type;  // 1 mean handshake packet, 2 mean data, transform into 20 by the class if late packet, 3 mean disconnect paccket
    unsigned char data[1360];
};

struct DatagramByClient
{
    uint16_t nClient;  // 65535 mean dont have yet num
    uint16_t id;
    uint16_t ack;
    uint64_t previousAcks;
    uint8_t type;  // 1 mean handshake packet, 2 mean data, transform into 20 by the class if late packet, 3 mean disconnect paccket
    unsigned char data[900];
};

struct Client
{
    unsigned char taken; // 't' mean taken, 'f' mean free, 'i' mean in handshake
    bool clock1Tick;
    uint16_t nClient;
    sockaddr_in address;
    uint64_t previousAcks, oldPreviousAcks; uint16_t lastIdReceiv;
    uint64_t lastPreviousAckReceived; uint16_t lastAckReceived;
    DatagramByServer allDatagramsSent[256]; uint8_t actualPosInArrayDatagramsSent; uint16_t actualIdSend;
};




class Udp
{
    public:

        WSADATA wsaData; SOCKET mySocket; sockaddr_in myAddress, distantAddress;
        Client allClients[N_MAX_CLIENTS]; uint16_t nClientsConnected;
        uint8_t buffer[1420]; DatagramByClient *bufferDatagram;
        float clock1;

        Udp(uint16_t pPort)
        {
            bufferDatagram = (DatagramByClient*)buffer;
            nClientsConnected = 0;
            InitUdp(pPort);
            for(int i=0; i<N_MAX_CLIENTS; i++) allClients[i].taken = 'f';
            clock1 = 0;
        }

        ~Udp() {CloseUdp();}




        inline unsigned char ReceivDatagram()  // 't' for true, 'f' for false, 'u' for unblock call
        {
            socklen_t fromLen = sizeof(distantAddress);
            int ret = recvfrom(mySocket, reinterpret_cast<char*>(buffer), 1420, 0, reinterpret_cast<sockaddr*>(&distantAddress), &fromLen);
            if (ret <= 500)  // a modifier pour contrer les attaques ddos
            {
                if(ret == -1 && WSAGetLastError() == WSAEWOULDBLOCK) return 'u';
                else return 'f';
            }           
            return 't';
        }

        inline bool SendDatagram(DatagramByServer &datagram, sockaddr_in &destAdress)
        {
            int ret = sendto(mySocket, reinterpret_cast<char*>(&datagram), sizeof(DatagramByServer), 0, reinterpret_cast<const sockaddr*>(&destAdress), sizeof(sockaddr_in));
            if (ret < 0)
                return false;
            return true;
        }





        bool Receiv()  
        {
            switch(ReceivDatagram()) // recu physique du datagrame
            {
                case 'u': return false; case 'f': return Receiv();
            }

            if(bufferDatagram->nClient == 65535) // if new client
            {
                if (manageNewClient() == false) return Receiv();
                else return true;
            }
            else if(bufferDatagram->nClient >= N_MAX_CLIENTS || allClients[bufferDatagram->nClient].taken != 't' || allClients[bufferDatagram->nClient].address.sin_addr.s_addr != distantAddress.sin_addr.s_addr) // if nClient non valide ou type=1
                return Receiv();

            uint16_t diff = SequenceDiff(bufferDatagram->id ,allClients[bufferDatagram->nClient].lastIdReceiv);  // calcul de la difference entre l'id recu et le lastId pour determiner si c'est un new packet


            if(diff == 0)  // si doublon
                return Receiv();
            else if(diff >= (std::numeric_limits<uint16_t>::max)() / 2 + 128)
                return Receiv();
            else if(diff > (std::numeric_limits<uint16_t>::max)() / 2)  // si packet ancien
            {
                if(verifyIfAlreadyReceivedAndActualizePreviousacks(diff - (std::numeric_limits<uint16_t>::max)() / 2, allClients[bufferDatagram->nClient])) return Receiv();
                else
                {
                    if(bufferDatagram->type == 2)
                    {
                        bufferDatagram->type = 20;
                        allClients[bufferDatagram->nClient].clock1Tick = true;
                        return true; // return datagram to application
                    }
                    else if(bufferDatagram->type == 3) 
                    {
                        std::cout << "Diconect client 1" << std::endl;
                        DisconnectClient(allClients[bufferDatagram->nClient]);
                        return true;
                    }
                }
            }
            else if(diff >= 64) return Receiv();
            else   // si nouveau packet
            {
                updatePreviousAcks(diff, allClients[bufferDatagram->nClient]);
                if(managePacketsToResend(allClients[bufferDatagram->nClient]) == false) {std::cout << "managePacketResend fail" << std::endl; DisconnectClient(allClients[bufferDatagram->nClient]); return true;} //analyse du lastPreviousAck grace au ack recu et du lastAck recu  pour savoir si on doit renvoyer des packets
                if(bufferDatagram->type == 2)
                {
                    allClients[bufferDatagram->nClient].clock1Tick = true;
                    return true;  // return datagram to aplication
                }
                else if(bufferDatagram->type == 3) 
                {
                    std::cout << "Disconect client" << std::endl;
                    DisconnectClient(allClients[bufferDatagram->nClient]);
                    return true;
                }
            }
            return Receiv();
        }

        void Send(DatagramByServer &datagram, Client &client)  // je m'occuperai later de changer le & par l'aspiration de valeur
        {
            client.actualIdSend += 1;
            datagram.nClient = client.nClient;
            datagram.id = client.actualIdSend;
            datagram.ack = client.lastIdReceiv;
            datagram.previousAcks = client.previousAcks;
            //le type et les datas sont defini par l'utilisateur de la classe directement
            SendDatagram(datagram, client.address);

            client.allDatagramsSent[client.actualPosInArrayDatagramsSent] = datagram; // attention copie inutile (peut etre)
            client.actualPosInArrayDatagramsSent += 1;

            SendDatagram(client.allDatagramsSent[client.actualPosInArrayDatagramsSent - 3], client.address);
            
        }

        void ManageClientsActivity(float deltaTime)  
        {
            clock1 = clock1 + deltaTime;
            if(clock1 >= 5)
            {
                clock1 = 0;
                for(int i=0; i<N_MAX_CLIENTS; i++)
                {
                    if(allClients[i].taken != 'f' && allClients[i].clock1Tick == false)
                    {
                        DisconnectClient(allClients[i]); // attention, cette fonction modifie le bufferDatagram
                    }
                    allClients[i].clock1Tick = false;
                }
            }
        }


        void ValidNewClientOrNot(uint16_t pNclient, bool validOrNot)
        {
            if(validOrNot == true)
            {
                std::cout << "joueur accepte par serveur" << std::endl;
                allClients[pNclient].taken = 't';
                DatagramByServer datagram;
                datagram.nClient = pNclient;
                datagram.type = 1;
                SendDatagram(datagram, allClients[pNclient].address);
                SendDatagram(datagram, allClients[pNclient].address);
            }
            else
            {
                std::cout << "refus du joueur par le serveur" << std::endl;
                allClients[pNclient].taken = 'f';
                nClientsConnected -= 1;
                DatagramByServer datagram;
                datagram.nClient = 65535;
                datagram.type = 3;
                SendDatagram(datagram, allClients[pNclient].address);
                SendDatagram(datagram, allClients[pNclient].address);
            }
        }







    private:

        void InitUdp(uint16_t pPort)
        {

            if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
                std::cout << "La fonction WSAStartup a echoue." << std::endl;
            else
            {
                mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                if (mySocket == INVALID_SOCKET)
                    std::cout << "La fonction socket a echoue." << std::endl;
                else
                {
                    u_long mode = 1;
                    if(ioctlsocket(mySocket, FIONBIO, &mode) == -1)
                        std::cout << "fail to unblock socket" << std::endl;
                    myAddress.sin_family = AF_INET;
                    inet_pton(AF_INET, "192.168.1.201", &myAddress.sin_addr);  // ADRESSE IP DU SERVEUR (a modifier si le serveur change d'endroit)
                    myAddress.sin_port = htons(pPort);

                    if (bind(mySocket, reinterpret_cast<sockaddr*>(&myAddress), sizeof(myAddress)) == SOCKET_ERROR)
                        std::cout << "La fonction bind a echoue." << std::endl;
                }
            }
        }

        inline void CloseUdp()
        {
            closesocket(mySocket);
            WSACleanup();
        }





        inline void updatePreviousAcks(uint16_t diff, Client &client)
        {
            for(int i=0; i<diff; i++)
            {
                client.oldPreviousAcks = client.oldPreviousAcks << 1; if((client.previousAcks & 0b1000000000000000000000000000000000000000000000000000000000000000) != 0) client.oldPreviousAcks |= 1ULL; client.previousAcks = client.previousAcks << 1;
            }
            client.previousAcks |= 1ULL;
            client.lastIdReceiv = bufferDatagram->id;
        }

        inline bool verifyIfAlreadyReceivedAndActualizePreviousacks(uint16_t diff, Client &client)
        {
            if(diff < 64)
            {
                if((client.previousAcks & (1ULL << diff)) == 0)
                {
                    client.previousAcks |= (1ULL << diff);
                    return false;
                }
                else return true;
            }
            else
            {
                diff -= 64;
                if((client.oldPreviousAcks & (1ULL << diff)) == 0)
                {
                    client.oldPreviousAcks |= (1ULL << diff);
                    return false;
                }
                else return true;
            }
        }

        inline bool managePacketsToResend(Client &client)
        {
            if(SequenceDiff(client.actualIdSend, bufferDatagram->ack) > (std::numeric_limits<uint16_t>::max)() / 2) { std::cout << "ManagePacketResend pb 1   " << (int)client.actualIdSend << "  " << (int)bufferDatagram->ack << std::endl; return false; }
            uint16_t diff = SequenceDiff(bufferDatagram->ack, client.lastAckReceived);
            if(diff == 0) ;
            else if(diff >= 64) { std::cout << "ManagePacketResend pb 2   " << (int)bufferDatagram->ack << "   " << (int)client.lastAckReceived << std::endl; return false; }
            else
            {
                for(int i=0; i<diff; i++)
                {
                    if((client.lastPreviousAckReceived & (0b1000000000000000000000000000000000000000000000000000000000000000 >> i)) == 0)
                    {
                        uint16_t idPacket = bufferDatagram->ack - (uint16_t)63 - diff + (uint16_t)i;
                        uint16_t dif = SequenceDiff(client.actualIdSend, idPacket);
                        if(dif < 256)
                        {
                            SendDatagram(client.allDatagramsSent[client.actualPosInArrayDatagramsSent - dif], client.address);  // RENVOIS 2 FOIS LE MESSAGE UNACK
                            SendDatagram(client.allDatagramsSent[client.actualPosInArrayDatagramsSent - dif], client.address);
                        }
                    }
                }

                client.lastAckReceived = bufferDatagram->ack;
            }
            client.lastPreviousAckReceived = bufferDatagram->previousAcks;
            return true;
        }



        inline bool IsSequenceNewer(uint16_t sNew, uint16_t sLast)
		{
			if (sNew == sLast)
				return false;
			return (sNew > sLast && sNew - sLast <= (std::numeric_limits<uint16_t>::max)() / 2)
				|| (sNew < sLast && sLast - sNew > (std::numeric_limits<uint16_t>::max)() / 2);
		}

        uint16_t SequenceDiff(uint16_t sNew, uint16_t sLast)
		{
			if (sNew == sLast)
				return 0;
            
            if(IsSequenceNewer(sNew, sLast))
            {
                if (sNew > sLast && sNew - sLast <= (std::numeric_limits<uint16_t>::max)() / 2)
                    return sNew - sLast;
                return ((std::numeric_limits<uint16_t>::max)() - sLast) + sNew + 1;
            }
            else
            {
                if (sLast > sNew && sLast - sNew <= (std::numeric_limits<uint16_t>::max)() / 2)
                    return (std::numeric_limits<uint16_t>::max)() / 2 + (sLast - sNew);
                return (std::numeric_limits<uint16_t>::max)() / 2 + (((std::numeric_limits<uint16_t>::max)() - sLast) + sLast + 1);
            }
		}

        bool manageNewClient()
        {
            int newId = 70000;

            if(bufferDatagram->type != 1 || nClientsConnected >= N_MAX_CLIENTS) return false;

            for(int i=0; i<N_MAX_CLIENTS; i++) // verifier que l'address n'est pas deja enregistrer chez un client
            {
                if(allClients[i].taken != 'f')
                {
                    if(allClients[i].address.sin_addr.s_addr == distantAddress.sin_addr.s_addr && allClients[i].address.sin_port == distantAddress.sin_port) return false;
                }
                else newId = i;
            }

            if(newId == 70000) return false;

            allClients[newId].taken = 'i';       // enregistrement du nouveau client
            allClients[newId].nClient = newId;
            allClients[newId].address = distantAddress;
            allClients[newId].previousAcks = -1; allClients[newId].oldPreviousAcks = -1; allClients[newId].lastIdReceiv = -1;
            allClients[newId].lastPreviousAckReceived = -1; allClients[newId].lastAckReceived = -1;
            allClients[newId].actualPosInArrayDatagramsSent = 0; allClients[newId].actualIdSend = -1;
            allClients[newId].clock1Tick = true;
            bufferDatagram->nClient = newId;
            nClientsConnected += 1;
            return true;

        }


        void DisconnectClient(Client &client)
        {
            bufferDatagram->type = 3;
            DatagramByServer datag;
            datag.type = 3;
            Send(datag, client);
            client.taken = 'f';
            nClientsConnected -= 1;
        }
};