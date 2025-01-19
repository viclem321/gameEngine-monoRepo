// var const
const uint8_t OTHERS_CLIENTS_SERVER1_CAPACITY = 20;
const uint8_t OTHERS_CLIENTS_SERVER2_CAPACITY = 30;



// PACKET BY SERVER
struct OtherClient { uint16_t nClient; glm::vec3 position; float yaw; unsigned char actionMoove; unsigned char actionOrientation; };

struct DataTemporalByServer { glm::vec3 position; float yaw; unsigned char stateMoove; unsigned char actionMoove; unsigned char stateOrientation; unsigned char actionOrientation; };
struct DataUntemporalByServer { uint16_t nOthersClientActions; OtherClient otherClients[OTHERS_CLIENTS_SERVER1_CAPACITY]; };

struct DataServer1{ DataTemporalByServer temporal; DataUntemporalByServer untemporal;};
struct DataServer2 { uint8_t nElements; OtherClient otherClients[OTHERS_CLIENTS_SERVER2_CAPACITY];};
union DataServer {DataServer1 data1; DataServer2 data2;};

struct SendFrameServer { uint8_t numFrameTestClient; };
struct SendDataServer { uint8_t numPacket; DataServer dataServer; };
union DataPacketByServer { SendFrameServer sendFrame; SendDataServer sendData;};

struct GamePacketByServer
{
    uint8_t typePacket; //  0 = coordination de frame, 1 = envoie de toute les donnes(begin), 2 = actualisation des datas ingame ;
    uint16_t actualFrame;
    DataPacketByServer dataPacket;
};







// PACKET BY CLIENT

struct AskFrameClient { uint8_t clientFrameTest; };
struct BeginGameClient { uint16_t actualFrame; };
struct InputPacketClient { uint16_t actualFrame; unsigned char actionMoove; unsigned char actionOrientation; };
union DataPacketByClient { AskFrameClient askFrame; BeginGameClient beginGame; InputPacketClient inputPacket;};

struct GamePacketByClient
{
    uint8_t typePacket; //  0 = demande coordination de frame(AskFrame),   1 = demande donnes de depart(BeginGame),   2 = send input(InputPacket) ;
    DataPacketByClient dataPacketByClient;   
};




// OTHER STRUCTS
struct DatagramServerReceived {DatagramByServer *allDatagrams; uint16_t pushEmplacement; uint16_t pullEmplacement;};
struct DatagramClientReceived {DatagramByClient *allDatagrams; uint16_t pushEmplacement; uint16_t pullEmplacement;};