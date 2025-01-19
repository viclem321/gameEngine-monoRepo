#include <reseau/structPacketData.h>


//manage recepetion
const uint16_t SIZE_TAMPON_RECEIVED = InfoGlobal::nMaxClients * 2; // taille du tampon entre le jeu et le moteur
void ManageAllDatagramsReceived(DatagramClientReceived &datagramsReceived)
{
    while (true)
    {
        InfoGlobal::mutex1Reseau.lock();
        if (InfoGlobal::myUdp->Receiv() == true)
        {
            while (datagramsReceived.pushEmplacement == datagramsReceived.pullEmplacement - 1 || (datagramsReceived.pullEmplacement == 0 && datagramsReceived.pushEmplacement == SIZE_TAMPON_RECEIVED - 1))
            {
                InfoGlobal::mutex1Reseau.unlock(); Sleep(4); InfoGlobal::mutex1Reseau.lock();
            }
            datagramsReceived.allDatagrams[datagramsReceived.pushEmplacement] = *InfoGlobal::myUdp->bufferDatagram;
            datagramsReceived.pushEmplacement += 1;
            if (datagramsReceived.pushEmplacement > SIZE_TAMPON_RECEIVED - 1) datagramsReceived.pushEmplacement = 0;
        }
        InfoGlobal::mutex1Reseau.unlock();
        Sleep(1);
    }

    delete InfoGlobal::myUdp;
}


// manage packet
bool ManagePacket(DatagramByClient &datagram, Player *allPlayers, uint16_t *pNplayersActualize, uint16_t *pListPlayersActualize, DatagramClientReceived &datagramsReceived)
{
    if (datagram.nClient >= InfoGlobal::nMaxClients) return false;
    GamePacketByClient *gamePacket = reinterpret_cast<GamePacketByClient*>(datagram.data);

    if(datagram.type == 1)
    {
        allPlayers[datagram.nClient].connected = true;
        allPlayers[datagram.nClient].stateInfo = 'd'; allPlayers[datagram.nClient].timeWithoutSendState = 0;
        InfoGlobal::nConnectedClients += 1;
        allPlayers[datagram.nClient].character = new Character("askyp", glm::vec3(150.0f, 30.0f, 150.0f));
        InfoGlobal::myUdp->ValidNewClientOrNot(datagram.nClient, true);
        
    }
    else if(datagram.type == 3)
    {
        std::cout << "demande de deco d'un joueur" << std::endl;
        allPlayers[datagram.nClient].connected = false;
        allPlayers[datagram.nClient].stateInfo = 'n';
        delete(allPlayers[datagram.nClient].character);
        InfoGlobal::nConnectedClients -= 1;
    }
    else if(datagram.type == 2 || datagram.type == 20)
    {
        if(gamePacket->typePacket == 0)
        {
            DatagramByServer dtg; dtg.type = 2;
            GamePacketByServer *gamePacketServer =  reinterpret_cast<GamePacketByServer*>(dtg.data);
            gamePacketServer->typePacket = 0;
            gamePacketServer->actualFrame = InfoGlobal::currentFrame;
            gamePacketServer->dataPacket.sendFrame.numFrameTestClient = gamePacket->dataPacketByClient.askFrame.clientFrameTest;
            InfoGlobal::myUdp->Send(dtg, InfoGlobal::myUdp->allClients[datagram.nClient]);
        }

        else if(gamePacket->typePacket == 1)
        {
            allPlayers[datagram.nClient].stateInfo = 'a';
        }


        else if(gamePacket->typePacket == 2)
        {
            InputPacketClient *inputData = &gamePacket->dataPacketByClient.inputPacket;

            uint16_t testdiff1 = SequenceDiff(InfoGlobal::currentFrame, inputData->actualFrame);
            if(testdiff1 > (std::numeric_limits<uint16_t>::max)() / 2 + 2 || (testdiff1 > 80 && testdiff1 <= (std::numeric_limits<uint16_t>::max)() / 2)) return false;
            if(testdiff1 > (std::numeric_limits<uint16_t>::max)() / 2) 
            {
                if(datagramsReceived.pushEmplacement != datagramsReceived.pullEmplacement - 1 && (datagramsReceived.pullEmplacement != 0 || datagramsReceived.pushEmplacement != SIZE_TAMPON_RECEIVED - 1))
                {
                    memcpy(&datagramsReceived.allDatagrams[datagramsReceived.pushEmplacement], &datagram, sizeof(DatagramByClient));
                    datagramsReceived.pushEmplacement++; if (datagramsReceived.pushEmplacement > SIZE_TAMPON_RECEIVED - 1) datagramsReceived.pushEmplacement = 0;
                }
                return false;
            }

            uint16_t testdiffMoove1 = SequenceDiff(inputData->actualFrame, allPlayers[datagram.nClient].character->stateMooveLastModifyFrame);
            if(testdiffMoove1 < (std::numeric_limits<uint16_t>::max)() / 2 && testdiffMoove1 != 0)
            {
                allPlayers[datagram.nClient].character->stateMooveLastModifyFrame = inputData->actualFrame;
                if(inputData->actionMoove == 'n' || inputData->actionMoove == 'w' || inputData->actionMoove == 'r')
                {
                    allPlayers[datagram.nClient].character->allNextStateMoove[Character::allNextStateCursor] = inputData->actionMoove;
                    allPlayers[datagram.nClient].character->modifyStateMooveThisFrame = true;
                }
            }
            uint16_t testdiffOrientation1 = SequenceDiff(inputData->actualFrame, allPlayers[datagram.nClient].character->stateOrientationLastModifyFrame);
            if(testdiffOrientation1 < (std::numeric_limits<uint16_t>::max)() / 2 && testdiffOrientation1 != 0)
            {
                allPlayers[datagram.nClient].character->stateOrientationLastModifyFrame = inputData->actualFrame;
                if(inputData->actionOrientation == 'n' || inputData->actionOrientation == 'l' || inputData->actionOrientation == 'r')
                {
                    allPlayers[datagram.nClient].character->allNextStateOrientation[Character::allNextStateCursor] = inputData->actionOrientation;
                    allPlayers[datagram.nClient].character->modifyStateOrientationThisFrame = true;
                }
            }

        }

    }
    
    return true;
}



// manage send packet
void SendPacketGame(Player *allPlayers, const uint16_t nPlayerActualize, const uint16_t *listPlayersActualize)
{
    OtherClient allOtherClients[OTHERS_CLIENTS_SERVER1_CAPACITY + OTHERS_CLIENTS_SERVER2_CAPACITY * 4];

    for(int j=0; j<InfoGlobal::nMaxClients; j++)
    {
        if(allPlayers[j].connected == true && allPlayers[j].stateInfo != 'n')
        {

            uint16_t actualPlayer = 0; uint16_t nPackets; uint16_t realnPlayerActualize;

            if(allPlayers[j].stateInfo == 'a')
            {
                for(int i=0; i<InfoGlobal::nMaxClients; i++)
                {
                    if(allPlayers[i].connected == true && i != j)
                    {
                        allOtherClients[actualPlayer].nClient = i;
                        allOtherClients[actualPlayer].position = allPlayers[i].character->position;
                        allOtherClients[actualPlayer].yaw = allPlayers[i].character->yaw;
                        allOtherClients[actualPlayer].actionMoove = allPlayers[i].character->allNextStateMoove[Character::allNextStateCursor];
                        allOtherClients[actualPlayer].actionOrientation = allPlayers[i].character->allNextStateOrientation[Character::allNextStateCursor];
                        actualPlayer++;
                        if(actualPlayer >= InfoGlobal::nConnectedClients - 1) break;
                    }
                }
                if(InfoGlobal::nConnectedClients - 1 <= OTHERS_CLIENTS_SERVER1_CAPACITY) nPackets = 1;
                else nPackets = ceil((InfoGlobal::nConnectedClients - 1 - OTHERS_CLIENTS_SERVER1_CAPACITY) / OTHERS_CLIENTS_SERVER2_CAPACITY) + 1;
            }

            else if(allPlayers[j].stateInfo == 'd')
            {
                realnPlayerActualize = nPlayerActualize;
                for(int i=0; i<nPlayerActualize; i++)
                {
                    if(listPlayersActualize[i] != j)
                    {
                        allOtherClients[actualPlayer].nClient = listPlayersActualize[i];
                        allOtherClients[actualPlayer].position = allPlayers[listPlayersActualize[i]].character->position;
                        allOtherClients[actualPlayer].yaw = allPlayers[listPlayersActualize[i]].character->yaw;
                        allOtherClients[actualPlayer].actionMoove = allPlayers[listPlayersActualize[i]].character->allNextStateMoove[Character::allNextStateCursor];
                        allOtherClients[actualPlayer].actionOrientation = allPlayers[listPlayersActualize[i]].character->allNextStateOrientation[Character::allNextStateCursor];
                        actualPlayer++;
                    }
                    else realnPlayerActualize -= 1;
                }
                if(realnPlayerActualize <= OTHERS_CLIENTS_SERVER1_CAPACITY) nPackets = 1;
                else nPackets = ceil((realnPlayerActualize - OTHERS_CLIENTS_SERVER1_CAPACITY) / OTHERS_CLIENTS_SERVER2_CAPACITY) + 1;
            }


            DatagramByServer dtgr; dtgr.type = 2; GamePacketByServer *gamePacketServer = reinterpret_cast<GamePacketByServer*>(dtgr.data);
            gamePacketServer->actualFrame = InfoGlobal::currentFrame; gamePacketServer->dataPacket.sendData.numPacket = 1;
            if(allPlayers[j].stateInfo == 'a') gamePacketServer->typePacket = 1; else if(allPlayers[j].stateInfo == 'd') gamePacketServer->typePacket = 2;

            gamePacketServer->dataPacket.sendData.dataServer.data1.temporal.position = allPlayers[j].character->position;
            gamePacketServer->dataPacket.sendData.dataServer.data1.temporal.yaw = allPlayers[j].character->yaw;
            gamePacketServer->dataPacket.sendData.dataServer.data1.temporal.stateMoove = allPlayers[j].character->stateMoove;
            gamePacketServer->dataPacket.sendData.dataServer.data1.temporal.actionMoove = allPlayers[j].character->allNextStateMoove[Character::allNextStateCursor];
            gamePacketServer->dataPacket.sendData.dataServer.data1.temporal.stateOrientation = allPlayers[j].character->stateOrientation;
            gamePacketServer->dataPacket.sendData.dataServer.data1.temporal.actionOrientation = allPlayers[j].character->allNextStateOrientation[Character::allNextStateCursor];
            if(allPlayers[j].stateInfo == 'a') gamePacketServer->dataPacket.sendData.dataServer.data1.untemporal.nOthersClientActions = InfoGlobal::nConnectedClients - 1; else if(allPlayers[j].stateInfo == 'd') gamePacketServer->dataPacket.sendData.dataServer.data1.untemporal.nOthersClientActions = realnPlayerActualize;
            memcpy( gamePacketServer->dataPacket.sendData.dataServer.data1.untemporal.otherClients, &allOtherClients[0], sizeof(OtherClient) * OTHERS_CLIENTS_SERVER1_CAPACITY );
            InfoGlobal::mutex1Reseau.lock();
            InfoGlobal::myUdp->Send(dtgr, InfoGlobal::myUdp->allClients[j]);

            for(int i=1; i < nPackets; i++)
            {
                gamePacketServer->dataPacket.sendData.numPacket = i + 1;
                if(i == nPackets - 1) gamePacketServer->dataPacket.sendData.dataServer.data2.nElements = (InfoGlobal::nConnectedClients - 1) - (OTHERS_CLIENTS_SERVER1_CAPACITY + OTHERS_CLIENTS_SERVER2_CAPACITY*(nPackets - 2));
                else gamePacketServer->dataPacket.sendData.dataServer.data2.nElements = OTHERS_CLIENTS_SERVER2_CAPACITY;
                uint16_t clientBeginner = OTHERS_CLIENTS_SERVER1_CAPACITY + OTHERS_CLIENTS_SERVER2_CAPACITY * (i-1);
                memcpy( gamePacketServer->dataPacket.sendData.dataServer.data2.otherClients, &allOtherClients[clientBeginner], sizeof(OtherClient) * OTHERS_CLIENTS_SERVER2_CAPACITY ); 
                InfoGlobal::myUdp->Send(dtgr, InfoGlobal::myUdp->allClients[j]);
            }
            InfoGlobal::mutex1Reseau.unlock(); 

            if(allPlayers[j].stateInfo == 'a') allPlayers[j].stateInfo = 'd';
        }
    }
}