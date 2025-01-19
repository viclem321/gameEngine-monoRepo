#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include<limits>


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

class Udp
{
    public:

        WSADATA wsaData; SOCKET mySocket; sockaddr_in myAddress, serverAddress, distantAddress;
        uint16_t myNClient;
        uint8_t buffer[1420]; DatagramByServer *bufferDatagram;
        uint16_t lastIdReceiv, actualIdSend;
        uint64_t previousAcks, oldPreviousAcks;
        uint16_t lastAckReceived; uint64_t lastPreviousAckReceived;
        DatagramByClient allDatagramsSent[256]; uint8_t actualPosInArrayDatagramsSent;

        Udp(uint16_t pPort, sockaddr_in pServerAddress)
        {
            serverAddress = pServerAddress;
            lastIdReceiv = -1; actualIdSend = -1; previousAcks = -1; oldPreviousAcks = -1;
            lastAckReceived = -1; lastPreviousAckReceived = -1;
            actualPosInArrayDatagramsSent = 0;
            bufferDatagram = (DatagramByServer*)buffer;
            InitUdp(pPort);
        }

        ~Udp() {CloseUdp();}




        inline unsigned char ReceivDatagram()
        {
            socklen_t fromLen = sizeof(distantAddress);
            int ret = recvfrom(mySocket, reinterpret_cast<char*>(buffer), 1420, 0, reinterpret_cast<sockaddr*>(&distantAddress), &fromLen);
            if (ret <= 500 || distantAddress.sin_addr.s_addr != serverAddress.sin_addr.s_addr || distantAddress.sin_port != serverAddress.sin_port)
            {
                if(ret == -1 && WSAGetLastError() == WSAEWOULDBLOCK) return 'u';
                else {std::cout << "PAS NORMAL   " << WSAGetLastError() << std::endl; return 'f';}
            }        
            return 't';
        }

        inline bool SendDatagram(DatagramByClient &datagram)
        {
            int ret = sendto(mySocket, reinterpret_cast<char*>(&datagram), sizeof(DatagramByClient), 0, reinterpret_cast<const sockaddr*>(&serverAddress), sizeof(serverAddress));
            if (ret < 0) return false;
            return true;
        }





        bool Receiv()
        {
            switch(ReceivDatagram()) // recu physique du datagrame
            {
                case 'u': return false; case 'f': return Receiv();
            }


            uint16_t diff = SequenceDiff(bufferDatagram->id ,lastIdReceiv);  // calcul de la difference entre l'id recu et le lastId pour determiner si c'est un new packet

            if(diff == 0)  // si doublon
                return Receiv();
            else if(diff >= (std::numeric_limits<uint16_t>::max)() / 2 + 128)
                return Receiv();
            else if(diff > (std::numeric_limits<uint16_t>::max)() / 2)  // si packet ancien
            {
                if(verifyIfAlreadyReceivedAndActualizePreviousacks(diff - (std::numeric_limits<uint16_t>::max)() / 2))
                    return Receiv();
                else
                {
                    if(bufferDatagram->type == 2)
                    {
                        bufferDatagram->type = 20;
                        return true; // return datagram to application
                    }
                    else if(bufferDatagram->type == 3) return true;
                }
            }
            else if(diff >= 64) return Receiv();
            else   // si nouveau packet
            {
                updatePreviousAcks(diff);
                if(managePacketsToResend() == false) { std::cout << "managePacketResend fail" << std::endl; Disconnect(); return false; } //analyse du lastPreviousAck grace au ack recu et du lastAck recu  pour savoir si on doit renvoyer des packets
                if(bufferDatagram->type == 2 || bufferDatagram->type == 3) return true;  // return datagram to aplication
            }

            return Receiv();
        }

        void Send(DatagramByClient &datagram)  // je m'occuperai later de changer le & par l'aspiration de valeur
        {
            actualIdSend += 1;
            datagram.nClient = myNClient;
            datagram.id = actualIdSend;
            datagram.ack = lastIdReceiv;
            datagram.previousAcks = previousAcks;
            //le type et les datas sont defini par l'utilisateur de la classe directement
            SendDatagram(datagram);

            allDatagramsSent[actualPosInArrayDatagramsSent] = datagram; // attention copie inutile (peut etre)
            actualPosInArrayDatagramsSent += 1;
            SendDatagram(allDatagramsSent[actualPosInArrayDatagramsSent - 2]);
            
        }


        bool processHandshake() // a appeler deux fois
        {
            DatagramByClient datagram;
            datagram.nClient = 65535;
            datagram.type = 1;
            SendDatagram(datagram);
            Sleep(500);
            while(1)
            {
                switch(ReceivDatagram())
                {
                    case 'u': return false;

                    case 'f': continue;

                    case 't':
                        if(bufferDatagram->type != 1 || bufferDatagram->nClient == 65535) continue;
                        else
                        {
                            myNClient = bufferDatagram->nClient;
                            std::cout << "Vous etes connecte au serveur avec succes" << std::endl;
                            return true;
                        }

                }
            }
        }


        void Disconnect() // advice : after using this one, use delete (in main)
        {
            std::cout << "fonction Disconnect" << std::endl;
            DatagramByClient datag;
            datag.type = 3;
            Send(datag);
            CloseUdp();
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
                    myAddress.sin_addr.s_addr = INADDR_ANY;
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





        inline void updatePreviousAcks(uint16_t diff)
        {
            for(int i=0; i<diff; i++)
            {
                oldPreviousAcks = oldPreviousAcks << 1; if((previousAcks & 0b1000000000000000000000000000000000000000000000000000000000000000) != 0) oldPreviousAcks |= 1ULL; previousAcks = previousAcks << 1;
            }
            previousAcks |= 1ULL;
            lastIdReceiv = bufferDatagram->id;
        }

        inline bool verifyIfAlreadyReceivedAndActualizePreviousacks(uint16_t diff)
        {
            if(diff < 64)
            {
                if((previousAcks & (1ULL << diff)) == 0)
                {
                    previousAcks |= (1ULL << diff);
                    return false;
                }
                else return true;
            }
            else
            {
                diff -= 64;
                if((oldPreviousAcks & (1ULL << diff)) == 0)
                {
                    oldPreviousAcks |= (1ULL << diff);
                    return false;
                }
                else return true;
            }
        }

        inline bool managePacketsToResend()
        {
            if(SequenceDiff(actualIdSend, bufferDatagram->ack) > (std::numeric_limits<uint16_t>::max)() / 2 ) { std::cout << "ManagePacketResend pb 1   " << (int)actualIdSend << "  " << (int)bufferDatagram->ack << std::endl; return false; }
            uint16_t diff = SequenceDiff(bufferDatagram->ack, lastAckReceived);
            if(diff == 0) ;
            else if(diff >= 64) { std::cout << "ManagePacketResend pb 2   " << (int)bufferDatagram->ack << "   " << (int)lastAckReceived << std::endl; return false; }
            else
            {
                for(int i=0; i<diff; i++)
                {
                    if((lastPreviousAckReceived & (0b1000000000000000000000000000000000000000000000000000000000000000 >> i)) == 0)
                    {
                        uint16_t idPacket = bufferDatagram->ack - (uint16_t)63 - diff + (uint16_t)i;
                        uint16_t dif = SequenceDiff(actualIdSend, idPacket);
                        if(dif < 256)
                        {
                        SendDatagram(allDatagramsSent[actualPosInArrayDatagramsSent - dif]);  // RENVOIS 2 FOIS LE MESSAGE UNACK
                        SendDatagram(allDatagramsSent[actualPosInArrayDatagramsSent - dif]);
                        }
                    }
                }

                lastAckReceived = bufferDatagram->ack;
            }
            lastPreviousAckReceived = bufferDatagram->previousAcks;

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


};