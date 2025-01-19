#include <network/structPacketData.h>

//manage recepetion
const uint16_t SIZE_TAMPON_RECEIVED = 110; // taille du tampon entre le jeu et le moteur
void ManageAllDatagramsReceived(DatagramServerReceived &datagramsReceived)
{

    while (true)
    {
        InfoGlobal::mutex1Reseau.lock();
        if (InfoGlobal::myUdp->Receiv() == true)
        {
            while (datagramsReceived.pushEmplacement == datagramsReceived.pullEmplacement - 1 || (datagramsReceived.pullEmplacement == 0 && datagramsReceived.pushEmplacement == SIZE_TAMPON_RECEIVED - 1) )
            {
                InfoGlobal::mutex1Reseau.unlock(); Sleep(2); InfoGlobal::mutex1Reseau.lock();
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



bool ManagePacket(DatagramByServer &datagram, Character &character, OtherPlayers* allPlayers[], Map &map, DatagramServerReceived &datagramsReceived)
{
    GamePacketByServer *gamePacketByServer = reinterpret_cast<GamePacketByServer*>(datagram.data);

    if(datagram.type == 3)
    {
        InfoGlobal::online = false;
    }


    else if(datagram.type == 2 || datagram.type == 20)
    {
        uint16_t diff1 = SequenceDiff(InfoGlobal::currentFrame, gamePacketByServer->actualFrame); uint16_t diff1Test = SequenceDiff(InfoGlobal::currentFrame - InfoGlobal::coefFrame, gamePacketByServer->actualFrame);
        if ( (diff1 >= N_CHARACTER_LAST_DATA_FRAME && diff1 <= (std::numeric_limits<uint16_t>::max)() / 2) || diff1 > (std::numeric_limits<uint16_t>::max)() / 2 + 2 ) return false;
        if(diff1Test > (std::numeric_limits<uint16_t>::max)() / 2)
        {
            if(datagramsReceived.pushEmplacement != datagramsReceived.pullEmplacement - 1 && (datagramsReceived.pullEmplacement != 0 || datagramsReceived.pushEmplacement != SIZE_TAMPON_RECEIVED - 1))
            {
                memcpy(&datagramsReceived.allDatagrams[datagramsReceived.pushEmplacement], &datagram, sizeof(DatagramByServer));
                datagramsReceived.pushEmplacement++; if (datagramsReceived.pushEmplacement > SIZE_TAMPON_RECEIVED - 1) datagramsReceived.pushEmplacement = 0;
            }
            return false;
        }


        uint16_t frame = character.currentPosLastDataFrames - diff1; if(frame >= N_CHARACTER_LAST_DATA_FRAME) frame =  N_CHARACTER_LAST_DATA_FRAME - (((std::numeric_limits<uint16_t>::max)() - frame) + 1);        

        if(gamePacketByServer->typePacket == 2)
        {

            if(gamePacketByServer->dataPacket.sendData.numPacket == 1)
            {
                uint16_t testLastFActualize = character.lastFrameActualize - (CHARACTER_STATE_LATENCY); if(testLastFActualize >= N_CHARACTER_LAST_DATA_FRAME) testLastFActualize =  N_CHARACTER_LAST_DATA_FRAME - (((std::numeric_limits<uint16_t>::max)() - testLastFActualize) + 1);
                if( IsSequenceNewer(gamePacketByServer->actualFrame, testLastFActualize) && (gamePacketByServer->dataPacket.sendData.dataServer.data1.temporal.actionMoove != character.lastDataFrames[frame].nextStateMoove || gamePacketByServer->dataPacket.sendData.dataServer.data1.temporal.actionOrientation != character.lastDataFrames[frame].nextStateOrientation || gamePacketByServer->dataPacket.sendData.dataServer.data1.temporal.stateMoove != character.lastDataFrames[frame].stateMoove || gamePacketByServer->dataPacket.sendData.dataServer.data1.temporal.stateOrientation != character.lastDataFrames[frame].stateOrientation))
                {
                    bool testChangeMoove = false;
                    character.lastDataFrames[frame].position = gamePacketByServer->dataPacket.sendData.dataServer.data1.temporal.position;
                    character.lastDataFrames[frame].yaw = gamePacketByServer->dataPacket.sendData.dataServer.data1.temporal.yaw;
                    if(character.lastDataFrames[frame].stateMoove != gamePacketByServer->dataPacket.sendData.dataServer.data1.temporal.stateMoove) testChangeMoove = true;
                    character.lastDataFrames[frame].stateMoove = gamePacketByServer->dataPacket.sendData.dataServer.data1.temporal.stateMoove;
                    character.lastDataFrames[frame].stateOrientation = gamePacketByServer->dataPacket.sendData.dataServer.data1.temporal.stateOrientation;
                    character.lastDataFrames[frame].nextStateMoove = gamePacketByServer->dataPacket.sendData.dataServer.data1.temporal.actionMoove;
                    character.lastDataFrames[frame].nextStateOrientation = gamePacketByServer->dataPacket.sendData.dataServer.data1.temporal.actionOrientation;
                    uint16_t lastFramee = frame - 1; if(lastFramee > N_CHARACTER_LAST_DATA_FRAME) lastFramee = N_CHARACTER_LAST_DATA_FRAME - 1;
                    if(character.lastDataFrames[frame].nextStateMoove != character.lastDataFrames[lastFramee].nextStateMoove) character.lastDataFrames[frame].switchMoove = true; else character.lastDataFrames[frame].switchMoove = false;
                    if(character.lastDataFrames[frame].nextStateOrientation != character.lastDataFrames[lastFramee].nextStateOrientation) character.lastDataFrames[frame].switchOrientation = true; else character.lastDataFrames[frame].switchOrientation = false;
                    if(testChangeMoove)
                    {
                        if(character.lastDataFrames[frame].stateMoove != character.lastDataFrames[lastFramee].stateMoove) character.UpdateAnim(frame) ;
                        else if(character.lastDataFrames[lastFramee].stateMoove == 'w') { character.animator->PlayAnimation(character.allAnimations[1]); character.animator->UpdateAnimation(0.05f); } 
                        else if(character.lastDataFrames[lastFramee].stateMoove == 'n') { character.animator->PlayAnimation(character.allAnimations[0]); character.animator->UpdateAnimation(0.05f); }
                    }

                    for(int i=0; i<diff1Test; i++)
                    {
                        uint16_t thisFrame = frame + i + 1; if(thisFrame >= N_CHARACTER_LAST_DATA_FRAME) thisFrame -= N_CHARACTER_LAST_DATA_FRAME;
                        uint16_t beforeFrame = frame + i; if(beforeFrame >= N_CHARACTER_LAST_DATA_FRAME) beforeFrame -= N_CHARACTER_LAST_DATA_FRAME;
                        uint16_t oldFrame = thisFrame - (CHARACTER_STATE_LATENCY - 1); if(oldFrame >= N_CHARACTER_LAST_DATA_FRAME) oldFrame = N_CHARACTER_LAST_DATA_FRAME - (((std::numeric_limits<uint16_t>::max)() - oldFrame) + 1);
                        character.lastDataFrames[thisFrame].stateMoove = character.lastDataFrames[oldFrame].nextStateMoove;
                        character.lastDataFrames[thisFrame].stateOrientation = character.lastDataFrames[oldFrame].nextStateOrientation;
                        if(character.lastDataFrames[thisFrame].nextStateMoove != character.lastDataFrames[beforeFrame].nextStateMoove) character.lastDataFrames[thisFrame].switchMoove = true; else character.lastDataFrames[thisFrame].switchMoove = false;
                        if(character.lastDataFrames[thisFrame].nextStateOrientation != character.lastDataFrames[beforeFrame].nextStateOrientation) character.lastDataFrames[thisFrame].switchOrientation = true; else character.lastDataFrames[thisFrame].switchOrientation = false;

                        if(character.lastDataFrames[thisFrame].stateMoove != character.lastDataFrames[beforeFrame].stateMoove) { testChangeMoove = true; character.UpdateAnim(thisFrame); }
                        else if(testChangeMoove == true) character.animator->UpdateAnimation(0.05f);

                        character.UpdatePosition(map, thisFrame);
                        
                    }
                }
                else if(IsSequenceNewer(gamePacketByServer->actualFrame, character.lastFrameActualize) && (fabs(character.lastDataFrames[frame].position.x - gamePacketByServer->dataPacket.sendData.dataServer.data1.temporal.position.x) >= 0.01f || fabs(character.lastDataFrames[frame].position.y - gamePacketByServer->dataPacket.sendData.dataServer.data1.temporal.position.y) >= 0.01f || fabs(character.lastDataFrames[frame].position.z - gamePacketByServer->dataPacket.sendData.dataServer.data1.temporal.position.z) >= 0.01f || fabs(character.lastDataFrames[frame].yaw - gamePacketByServer->dataPacket.sendData.dataServer.data1.temporal.yaw) >= 0.01))
                {
                    character.lastDataFrames[frame].position = gamePacketByServer->dataPacket.sendData.dataServer.data1.temporal.position;
                    character.lastDataFrames[frame].yaw = gamePacketByServer->dataPacket.sendData.dataServer.data1.temporal.yaw;

                    for(int i=0; i<diff1Test; i++) character.UpdatePosition(map, frame);
                }
                if( IsSequenceNewer(gamePacketByServer->actualFrame, character.lastFrameActualize) ) character.lastFrameActualize = gamePacketByServer->actualFrame;





                for(int i=0; i<gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.nOthersClientActions; i++)
                {
                    if(i>=OTHERS_CLIENTS_SERVER1_CAPACITY) break;

                    uint16_t nPlayer = gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.otherClients[i].nClient;
                    uint16_t diffWithLastActualizeFrame = SequenceDiff(gamePacketByServer->actualFrame, allPlayers[nPlayer]->lastActualizeFrame);
                    if(allPlayers[nPlayer]->connected == false) 
                    {
                        allPlayers[nPlayer]->NewPlayer(gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.otherClients[i].position, 'n', 'n', gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.otherClients[i].yaw, InfoGlobal::currentFrame);
                        allPlayers[nPlayer]->allStateMooveForLatency[CHARACTER_STATE_LATENCY - 1] = gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.otherClients[i].actionMoove;
                        allPlayers[nPlayer]->allStateOrientationForLatency[CHARACTER_STATE_LATENCY - 1] = gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.otherClients[i].actionOrientation;
                        allPlayers[nPlayer]->lastActualizeFramePosition = gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.otherClients[i].position;
                        allPlayers[nPlayer]->lastActualizeFrameYaw = gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.otherClients[i].yaw;
                        allPlayers[nPlayer]->BeginAnim();
                        uint16_t compteurDiff1 = 1;
                        for(int j=0; j<diff1Test; j++)
                        {
                            if(compteurDiff1 < CHARACTER_STATE_LATENCY && allPlayers[nPlayer]->allStateMooveForLatency[compteurDiff1] != 'o') allPlayers[nPlayer]->stateMoove = allPlayers[nPlayer]->allStateMooveForLatency[compteurDiff1];
                            if(compteurDiff1 < CHARACTER_STATE_LATENCY && allPlayers[nPlayer]->allStateOrientationForLatency[compteurDiff1] != 'o') allPlayers[nPlayer]->stateOrientation = allPlayers[nPlayer]->allStateOrientationForLatency[compteurDiff1];

                            allPlayers[nPlayer]->UpdatePosition(map);
                            allPlayers[nPlayer]->UpdateAnim();
                            compteurDiff1++;
                        }
                    }
                    else if( diffWithLastActualizeFrame > 0 && diffWithLastActualizeFrame < (std::numeric_limits<uint16_t>::max)() / 2 )
                    {
                        if(diffWithLastActualizeFrame >= CHARACTER_STATE_LATENCY) 
                        {
                            allPlayers[nPlayer]->allStateMooveForLatency[CHARACTER_STATE_LATENCY - 1] = gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.otherClients[i].actionMoove;
                            for(int j=0; j<CHARACTER_STATE_LATENCY - 1; j++) allPlayers[nPlayer]->allStateMooveForLatency[j] = 'o';
                            allPlayers[nPlayer]->allStateOrientationForLatency[CHARACTER_STATE_LATENCY - 1] = gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.otherClients[i].actionOrientation;
                            for(int j=0; j<CHARACTER_STATE_LATENCY - 1; j++) allPlayers[nPlayer]->allStateOrientationForLatency[j] = 'o';
                        }
                        else
                        {
                            for(int j=0; j<diffWithLastActualizeFrame; j++)
                            {
                                uint16_t compteur5 = 0;
                                while(true)
                                {
                                    allPlayers[nPlayer]->allStateMooveForLatency[compteur5] = allPlayers[nPlayer]->allStateMooveForLatency[compteur5 + 1];
                                    allPlayers[nPlayer]->allStateOrientationForLatency[compteur5] = allPlayers[nPlayer]->allStateOrientationForLatency[compteur5 + 1];
                                    compteur5++; if(compteur5 >= CHARACTER_STATE_LATENCY - 1) break;
                                }
                                allPlayers[nPlayer]->allStateMooveForLatency[CHARACTER_STATE_LATENCY - 1] = 'o';
                            }
                            allPlayers[nPlayer]->allStateMooveForLatency[CHARACTER_STATE_LATENCY - 1] = gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.otherClients[i].actionMoove;
                            allPlayers[nPlayer]->allStateOrientationForLatency[CHARACTER_STATE_LATENCY - 1] = gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.otherClients[i].actionOrientation;
                        }
                        allPlayers[nPlayer]->lastActualizeFramePosition = gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.otherClients[i].position;
                        allPlayers[nPlayer]->lastActualizeFrameYaw = gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.otherClients[i].yaw;

                        allPlayers[nPlayer]->position = gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.otherClients[i].position;
                        allPlayers[nPlayer]->yaw = gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.otherClients[i].yaw;
                        bool testSwitchMoove = false;
                        if( allPlayers[nPlayer]->allStateMooveForLatency[0] != 'o' && allPlayers[nPlayer]->allStateMooveForLatency[0] != allPlayers[nPlayer]->stateMoove) { allPlayers[nPlayer]->stateMoove = allPlayers[nPlayer]->allStateMooveForLatency[0]; allPlayers[nPlayer]->BeginAnim(); testSwitchMoove = true; }
                        if( allPlayers[nPlayer]->allStateOrientationForLatency[0] != 'o') allPlayers[nPlayer]->stateOrientation = allPlayers[nPlayer]->allStateOrientationForLatency[0]; 

                        uint16_t compteurDiff1 = 1;
                        for(int j=0; j<diff1Test; j++)
                        {
                            if(compteurDiff1 < CHARACTER_STATE_LATENCY)
                            {
                                if(allPlayers[nPlayer]->allStateMooveForLatency[compteurDiff1] != 'o' && allPlayers[nPlayer]->allStateMooveForLatency[compteurDiff1] != allPlayers[nPlayer]->stateMoove)
                                {
                                    allPlayers[nPlayer]->stateMoove = allPlayers[nPlayer]->allStateMooveForLatency[compteurDiff1];
                                    allPlayers[nPlayer]->BeginAnim(); testSwitchMoove = true;
                                }
                                if(allPlayers[nPlayer]->allStateOrientationForLatency[compteurDiff1] != 'o') allPlayers[nPlayer]->stateOrientation = allPlayers[nPlayer]->allStateOrientationForLatency[compteurDiff1];
                            }

                            allPlayers[nPlayer]->UpdatePosition(map);
                            if(testSwitchMoove == true)allPlayers[nPlayer]->UpdateAnim();
                            compteurDiff1++;
                        }
                        allPlayers[nPlayer]->lastActualizeFrame = gamePacketByServer->actualFrame;
                    }
                    else if(diffWithLastActualizeFrame > (std::numeric_limits<uint16_t>::max)() / 2 && diffWithLastActualizeFrame <= (std::numeric_limits<uint16_t>::max)() / 2 + CHARACTER_STATE_LATENCY - 1)
                    {
                        diffWithLastActualizeFrame = diffWithLastActualizeFrame - (std::numeric_limits<uint16_t>::max)() / 2; uint16_t latencyEmplacement = (CHARACTER_STATE_LATENCY - 1) - diffWithLastActualizeFrame; uint16_t latencyEmplacement2 = latencyEmplacement;
                        allPlayers[nPlayer]->allStateMooveForLatency[latencyEmplacement] = gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.otherClients[i].actionMoove;
                        allPlayers[nPlayer]->allStateOrientationForLatency[latencyEmplacement] = gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.otherClients[i].actionOrientation;
                        
                        latencyEmplacement++; latencyEmplacement2++;
                        while(latencyEmplacement < CHARACTER_STATE_LATENCY - 1  &&  allPlayers[nPlayer]->allStateMooveForLatency[latencyEmplacement] != 'o') { allPlayers[nPlayer]->allStateMooveForLatency[latencyEmplacement] = allPlayers[nPlayer]->allStateMooveForLatency[latencyEmplacement - 1]; latencyEmplacement++; }
                        while(latencyEmplacement2 < CHARACTER_STATE_LATENCY - 1  &&  allPlayers[nPlayer]->allStateOrientationForLatency[latencyEmplacement2] != 'o') { allPlayers[nPlayer]->allStateOrientationForLatency[latencyEmplacement2] = allPlayers[nPlayer]->allStateOrientationForLatency[latencyEmplacement2 - 1]; latencyEmplacement2++; }
                        
                        allPlayers[nPlayer]->position = allPlayers[nPlayer]->lastActualizeFramePosition;  allPlayers[nPlayer]->yaw = allPlayers[nPlayer]->lastActualizeFrameYaw;

                        uint16_t diff2Test = SequenceDiff(InfoGlobal::currentFrame - InfoGlobal::coefFrame, allPlayers[nPlayer]->lastActualizeFrame);
                        uint16_t compteur6 = 1; bool testSwitchMoove = false;
                        for(int j=0; j < diff2Test; j++)
                        {
                            if(compteur6 < CHARACTER_STATE_LATENCY)
                            {
                                if(allPlayers[nPlayer]->allStateMooveForLatency[compteur6] != 'o' && allPlayers[nPlayer]->allStateMooveForLatency[compteur6] != allPlayers[nPlayer]->stateMoove)
                                {
                                    allPlayers[nPlayer]->stateMoove = allPlayers[nPlayer]->allStateMooveForLatency[compteur6];
                                    allPlayers[nPlayer]->BeginAnim(); testSwitchMoove = true;
                                }
                                if(allPlayers[nPlayer]->allStateOrientationForLatency[compteur6] != 'o') allPlayers[nPlayer]->stateOrientation = allPlayers[nPlayer]->allStateOrientationForLatency[compteur6];
                            }
                            allPlayers[nPlayer]->UpdatePosition(map);
                            if(testSwitchMoove == true)allPlayers[nPlayer]->UpdateAnim();
                            compteur6++;
                        }
                        
                    }
                }
            }

            else if(gamePacketByServer->dataPacket.sendData.numPacket >= 2)
            {
                for(int i=0; i<gamePacketByServer->dataPacket.sendData.dataServer.data2.nElements; i++)
                {
                    uint16_t nPlayer = gamePacketByServer->dataPacket.sendData.dataServer.data2.otherClients[i].nClient;
                    uint16_t diffWithLastActualizeFrame = SequenceDiff(gamePacketByServer->actualFrame, allPlayers[nPlayer]->lastActualizeFrame);
                    if(allPlayers[nPlayer]->connected == false) 
                    {
                        allPlayers[nPlayer]->NewPlayer(gamePacketByServer->dataPacket.sendData.dataServer.data2.otherClients[i].position, 'n', 'n', gamePacketByServer->dataPacket.sendData.dataServer.data2.otherClients[i].yaw, InfoGlobal::currentFrame);
                        allPlayers[nPlayer]->allStateMooveForLatency[CHARACTER_STATE_LATENCY - 1] = gamePacketByServer->dataPacket.sendData.dataServer.data2.otherClients[i].actionMoove;
                        allPlayers[nPlayer]->allStateOrientationForLatency[CHARACTER_STATE_LATENCY - 1] = gamePacketByServer->dataPacket.sendData.dataServer.data2.otherClients[i].actionOrientation;
                        allPlayers[nPlayer]->lastActualizeFramePosition = gamePacketByServer->dataPacket.sendData.dataServer.data2.otherClients[i].position;
                        allPlayers[nPlayer]->lastActualizeFrameYaw = gamePacketByServer->dataPacket.sendData.dataServer.data2.otherClients[i].yaw;
                        allPlayers[nPlayer]->BeginAnim();
                        uint16_t compteurDiff1 = 1;
                        for(int j=0; j<diff1Test; j++)
                        {
                            if(compteurDiff1 < CHARACTER_STATE_LATENCY && allPlayers[nPlayer]->allStateMooveForLatency[compteurDiff1] != 'o') allPlayers[nPlayer]->stateMoove = allPlayers[nPlayer]->allStateMooveForLatency[compteurDiff1];
                            if(compteurDiff1 < CHARACTER_STATE_LATENCY && allPlayers[nPlayer]->allStateOrientationForLatency[compteurDiff1] != 'o') allPlayers[nPlayer]->stateOrientation = allPlayers[nPlayer]->allStateOrientationForLatency[compteurDiff1];

                            allPlayers[nPlayer]->UpdatePosition(map);
                            allPlayers[nPlayer]->UpdateAnim();
                            compteurDiff1++;
                        }
                    }
                    else if( diffWithLastActualizeFrame > 0 && diffWithLastActualizeFrame < (std::numeric_limits<uint16_t>::max)() / 2 )
                    {
                        if(diffWithLastActualizeFrame >= CHARACTER_STATE_LATENCY) 
                        {
                            allPlayers[nPlayer]->allStateMooveForLatency[CHARACTER_STATE_LATENCY - 1] = gamePacketByServer->dataPacket.sendData.dataServer.data2.otherClients[i].actionMoove;
                            for(int j=0; j<CHARACTER_STATE_LATENCY - 1; j++) allPlayers[nPlayer]->allStateMooveForLatency[j] = 'o';
                            allPlayers[nPlayer]->allStateOrientationForLatency[CHARACTER_STATE_LATENCY - 1] = gamePacketByServer->dataPacket.sendData.dataServer.data2.otherClients[i].actionOrientation;
                            for(int j=0; j<CHARACTER_STATE_LATENCY - 1; j++) allPlayers[nPlayer]->allStateOrientationForLatency[j] = 'o';
                        }
                        else
                        {
                            for(int j=0; j<diffWithLastActualizeFrame; j++)
                            {
                                uint16_t compteur5 = 0;
                                while(true)
                                {
                                    allPlayers[nPlayer]->allStateMooveForLatency[compteur5] = allPlayers[nPlayer]->allStateMooveForLatency[compteur5 + 1];
                                    allPlayers[nPlayer]->allStateOrientationForLatency[compteur5] = allPlayers[nPlayer]->allStateOrientationForLatency[compteur5 + 1];
                                    compteur5++; if(compteur5 >= CHARACTER_STATE_LATENCY - 1) break;
                                }
                                allPlayers[nPlayer]->allStateMooveForLatency[CHARACTER_STATE_LATENCY - 1] = 'o';
                            }
                            allPlayers[nPlayer]->allStateMooveForLatency[CHARACTER_STATE_LATENCY - 1] = gamePacketByServer->dataPacket.sendData.dataServer.data2.otherClients[i].actionMoove;
                            allPlayers[nPlayer]->allStateOrientationForLatency[CHARACTER_STATE_LATENCY - 1] = gamePacketByServer->dataPacket.sendData.dataServer.data2.otherClients[i].actionOrientation;
                        }
                        allPlayers[nPlayer]->lastActualizeFramePosition = gamePacketByServer->dataPacket.sendData.dataServer.data2.otherClients[i].position;
                        allPlayers[nPlayer]->lastActualizeFrameYaw = gamePacketByServer->dataPacket.sendData.dataServer.data2.otherClients[i].yaw;

                        allPlayers[nPlayer]->position = gamePacketByServer->dataPacket.sendData.dataServer.data2.otherClients[i].position;
                        allPlayers[nPlayer]->yaw = gamePacketByServer->dataPacket.sendData.dataServer.data2.otherClients[i].yaw;
                        bool testSwitchMoove = false;
                        if( allPlayers[nPlayer]->allStateMooveForLatency[0] != 'o' && allPlayers[nPlayer]->allStateMooveForLatency[0] != allPlayers[nPlayer]->stateMoove) { allPlayers[nPlayer]->stateMoove = allPlayers[nPlayer]->allStateMooveForLatency[0]; allPlayers[nPlayer]->BeginAnim(); testSwitchMoove = true; }
                        if( allPlayers[nPlayer]->allStateOrientationForLatency[0] != 'o') allPlayers[nPlayer]->stateOrientation = allPlayers[nPlayer]->allStateOrientationForLatency[0]; 

                        uint16_t compteurDiff1 = 1;
                        for(int j=0; j<diff1Test; j++)
                        {
                            if(compteurDiff1 < CHARACTER_STATE_LATENCY)
                            {
                                if(allPlayers[nPlayer]->allStateMooveForLatency[compteurDiff1] != 'o' && allPlayers[nPlayer]->allStateMooveForLatency[compteurDiff1] != allPlayers[nPlayer]->stateMoove)
                                {
                                    allPlayers[nPlayer]->stateMoove = allPlayers[nPlayer]->allStateMooveForLatency[compteurDiff1];
                                    allPlayers[nPlayer]->BeginAnim(); testSwitchMoove = true;
                                }
                                if(allPlayers[nPlayer]->allStateOrientationForLatency[compteurDiff1] != 'o') allPlayers[nPlayer]->stateOrientation = allPlayers[nPlayer]->allStateOrientationForLatency[compteurDiff1];
                            }

                            allPlayers[nPlayer]->UpdatePosition(map);
                            if(testSwitchMoove == true)allPlayers[nPlayer]->UpdateAnim();
                            compteurDiff1++;
                        }
                        allPlayers[nPlayer]->lastActualizeFrame = gamePacketByServer->actualFrame;
                    }
                    else if(diffWithLastActualizeFrame > (std::numeric_limits<uint16_t>::max)() / 2 && diffWithLastActualizeFrame <= (std::numeric_limits<uint16_t>::max)() / 2 + CHARACTER_STATE_LATENCY - 1)
                    {
                        diffWithLastActualizeFrame = diffWithLastActualizeFrame - (std::numeric_limits<uint16_t>::max)() / 2; uint16_t latencyEmplacement = (CHARACTER_STATE_LATENCY - 1) - diffWithLastActualizeFrame; uint16_t latencyEmplacement2 = latencyEmplacement;
                        allPlayers[nPlayer]->allStateMooveForLatency[latencyEmplacement] = gamePacketByServer->dataPacket.sendData.dataServer.data2.otherClients[i].actionMoove;
                        allPlayers[nPlayer]->allStateOrientationForLatency[latencyEmplacement] = gamePacketByServer->dataPacket.sendData.dataServer.data2.otherClients[i].actionOrientation;
                        
                        latencyEmplacement++; latencyEmplacement2++;
                        while(latencyEmplacement < CHARACTER_STATE_LATENCY - 1  &&  allPlayers[nPlayer]->allStateMooveForLatency[latencyEmplacement] != 'o') { allPlayers[nPlayer]->allStateMooveForLatency[latencyEmplacement] = allPlayers[nPlayer]->allStateMooveForLatency[latencyEmplacement - 1]; latencyEmplacement++; }
                        while(latencyEmplacement2 < CHARACTER_STATE_LATENCY - 1  &&  allPlayers[nPlayer]->allStateOrientationForLatency[latencyEmplacement2] != 'o') { allPlayers[nPlayer]->allStateOrientationForLatency[latencyEmplacement2] = allPlayers[nPlayer]->allStateOrientationForLatency[latencyEmplacement2 - 1]; latencyEmplacement2++; }
                        
                        allPlayers[nPlayer]->position = allPlayers[nPlayer]->lastActualizeFramePosition;  allPlayers[nPlayer]->yaw = allPlayers[nPlayer]->lastActualizeFrameYaw;

                        uint16_t diff2Test = SequenceDiff(InfoGlobal::currentFrame - InfoGlobal::coefFrame, allPlayers[nPlayer]->lastActualizeFrame);
                        uint16_t compteur6 = 1; bool testSwitchMoove = false;
                        for(int j=0; j<diff2Test; j++)
                        {
                            if(compteur6 < CHARACTER_STATE_LATENCY)
                            {
                                if(allPlayers[nPlayer]->allStateMooveForLatency[compteur6] != 'o' && allPlayers[nPlayer]->allStateMooveForLatency[compteur6] != allPlayers[nPlayer]->stateMoove)
                                {
                                    allPlayers[nPlayer]->stateMoove = allPlayers[nPlayer]->allStateMooveForLatency[compteur6];
                                    allPlayers[nPlayer]->BeginAnim(); testSwitchMoove = true;
                                }
                                if(allPlayers[nPlayer]->allStateOrientationForLatency[compteur6] != 'o') allPlayers[nPlayer]->stateOrientation = allPlayers[nPlayer]->allStateOrientationForLatency[compteur6];
                            }
                            allPlayers[nPlayer]->UpdatePosition(map);
                            if(testSwitchMoove == true)allPlayers[nPlayer]->UpdateAnim();
                            compteur6++;
                        }                      
                    }
                }
            }
        
        }
    }
    
    return true;
}




// coordonne le serveur et le client
bool EgalizeNetwork(std::chrono::steady_clock::time_point &start, DatagramServerReceived &datagramsReceived)
{
    uint16_t allTestFrame[5]; uint16_t nAlltestFrame = 0;
    DatagramByClient dtg; dtg.type = 2; GamePacketByClient *gamePacketByClient = reinterpret_cast<GamePacketByClient*>(dtg.data);
    gamePacketByClient->typePacket = 0;


    for(int i=0; i<=16; i++)
    {
        InfoGlobal::currentFrame += InfoGlobal::coefFrame; // update currentframe
	    start += (InfoGlobal::frameTime * InfoGlobal::coefFrame); // update chrono


        if(i < 4)
        {
            gamePacketByClient->dataPacketByClient.askFrame.clientFrameTest = InfoGlobal::currentFrame;
            InfoGlobal::mutex1Reseau.lock();
            InfoGlobal::myUdp->Send(dtg);
            InfoGlobal::mutex1Reseau.unlock();
        }
        else if(i == 7)
        {
            InfoGlobal::mutex1Reseau.lock();
            while (1)
            {
                if (datagramsReceived.pullEmplacement != datagramsReceived.pushEmplacement)
                {
                    GamePacketByServer *gamePacketByServer = reinterpret_cast<GamePacketByServer*>(datagramsReceived.allDatagrams[datagramsReceived.pullEmplacement].data);
                    if(gamePacketByServer->typePacket == 0 && gamePacketByServer->dataPacket.sendFrame.numFrameTestClient <= 4)
                    {
                        allTestFrame[gamePacketByServer->dataPacket.sendFrame.numFrameTestClient] = gamePacketByServer->actualFrame - gamePacketByServer->dataPacket.sendFrame.numFrameTestClient;
                        nAlltestFrame++;
                    }
                    datagramsReceived.pullEmplacement++;
                    if (datagramsReceived.pullEmplacement > SIZE_TAMPON_RECEIVED - 1)  datagramsReceived.pullEmplacement = 0;
                }
                else break;
            }
            InfoGlobal::mutex1Reseau.unlock();
            bool successFirstTest = false;
            for(int j=0; j<nAlltestFrame; j++)
            {
                uint16_t nOccurence = 1;
                for(int h=0; h<nAlltestFrame; h++)
                {
                    if(j == h) continue;
                    if(allTestFrame[j] == allTestFrame[h]) nOccurence++;
                }
                if(nOccurence >= 2)
                {
                    InfoGlobal::currentFrame = allTestFrame[j] + InfoGlobal::currentFrame; successFirstTest = true; break;
                }
            }
            if(successFirstTest == false) {std::cout << "succesFirstTest faise" << std::endl; return false; } // a voir plus tard;

            start += static_cast<std::chrono::milliseconds>(20); // attention valeur en brut
        }  
        else if(i > 7 && i <= 12)
        {
            gamePacketByClient->dataPacketByClient.askFrame.clientFrameTest = InfoGlobal::currentFrame;
            InfoGlobal::mutex1Reseau.lock();
            InfoGlobal::myUdp->Send(dtg);
            InfoGlobal::mutex1Reseau.unlock();
        }
        else if(i == 16)
        {
            uint16_t nTest2 = 0;
            InfoGlobal::mutex1Reseau.lock();
            while (1)
            {
                if (datagramsReceived.pullEmplacement != datagramsReceived.pushEmplacement)
                {
                    GamePacketByServer *gamePacketByServer = reinterpret_cast<GamePacketByServer*>(datagramsReceived.allDatagrams[datagramsReceived.pullEmplacement].data);
                    if(gamePacketByServer->typePacket == 0)
                    {
                        if(gamePacketByServer->actualFrame > gamePacketByServer->dataPacket.sendFrame.numFrameTestClient) nTest2++;
                    }
                    datagramsReceived.pullEmplacement++;
                    if (datagramsReceived.pullEmplacement > SIZE_TAMPON_RECEIVED - 1)  datagramsReceived.pullEmplacement = 0;
                }
                else break;
            }
            InfoGlobal::mutex1Reseau.unlock();
            if(nTest2 >= 2) start -= static_cast<std::chrono::milliseconds>(20);
        }

        auto end = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        if (diff < InfoGlobal::frameTimeInt)
        {
            Sleep(InfoGlobal::frameTimeInt - diff);
            InfoGlobal::coefFrame = 1;
	    }
        else if(diff < InfoGlobal::frameTimeInt + 10) InfoGlobal::coefFrame = 1;
        else
        {
            InfoGlobal::coefFrame = static_cast<int>(floor(diff / InfoGlobal::frameTimeInt) + 1);
            Sleep(InfoGlobal::frameTimeInt * InfoGlobal::coefFrame - diff);
        }
    }

    return true;
}


bool RecuperateStateOfTheGame(std::chrono::steady_clock::time_point &start, Character &character, OtherPlayers* allPlayers[], Map &map, DatagramServerReceived &datagramsReceived)
{
    for(int i=0; i<8; i++)
    {
        InfoGlobal::currentFrame += InfoGlobal::coefFrame; // update currentframe
	    start += (InfoGlobal::frameTime * InfoGlobal::coefFrame); // update chrono


        if(i == 0)
        {
            DatagramByClient dtg; dtg.type = 2; GamePacketByClient *gamePacketByClient = reinterpret_cast<GamePacketByClient*>(dtg.data); 
            gamePacketByClient->typePacket = 1; gamePacketByClient->dataPacketByClient.beginGame.actualFrame = InfoGlobal::currentFrame;
            InfoGlobal::mutex1Reseau.lock(); InfoGlobal::myUdp->Send(dtg); InfoGlobal::mutex1Reseau.unlock();
        }
        else if(i == 7)
        {
            uint8_t nPacketReceived = 0; uint8_t nPacketWaited = 255;
            InfoGlobal::mutex1Reseau.lock();
            while (1)
            {
                if (datagramsReceived.pullEmplacement != datagramsReceived.pushEmplacement)
                {
                    GamePacketByServer *gamePacketByServer = reinterpret_cast<GamePacketByServer*>(datagramsReceived.allDatagrams[datagramsReceived.pullEmplacement].data);
                    if(gamePacketByServer->typePacket == 1)
                    {
                        if(gamePacketByServer->dataPacket.sendData.numPacket == 1)
                        {
                            uint16_t indiceFrame = SequenceDiff(InfoGlobal::currentFrame, gamePacketByServer->actualFrame);
                            if(indiceFrame < N_CHARACTER_LAST_DATA_FRAME)
                            { 
                                indiceFrame = character.currentPosLastDataFrames - indiceFrame; if(indiceFrame >= (std::numeric_limits<uint16_t>::max)() - 5000) indiceFrame = N_CHARACTER_LAST_DATA_FRAME - ((std::numeric_limits<uint16_t>::max)() - indiceFrame) - 1;
                                character.nClient = datagramsReceived.allDatagrams[datagramsReceived.pullEmplacement].nClient; allPlayers[character.nClient]->connected = true;
                                character.lastDataFrames[indiceFrame].position = gamePacketByServer->dataPacket.sendData.dataServer.data1.temporal.position;
                                character.lastDataFrames[indiceFrame].yaw = gamePacketByServer->dataPacket.sendData.dataServer.data1.temporal.yaw;
                                character.lastDataFrames[indiceFrame].stateMoove = gamePacketByServer->dataPacket.sendData.dataServer.data1.temporal.actionMoove;
                                character.lastDataFrames[indiceFrame].stateOrientation = gamePacketByServer->dataPacket.sendData.dataServer.data1.temporal.actionOrientation;
                                character.lastDataFrames[indiceFrame].switchMoove = true; character.lastDataFrames[indiceFrame].switchOrientation = true;
                                character.lastFrameActualize = gamePacketByServer->actualFrame;
                                while(indiceFrame != character.currentPosLastDataFrames)
                                {
                                    uint16_t lastIndiceFrame = indiceFrame;
                                    indiceFrame++; if(indiceFrame >= N_CHARACTER_LAST_DATA_FRAME) indiceFrame = 0;
                                    character.lastDataFrames[indiceFrame].stateMoove = character.lastDataFrames[lastIndiceFrame].stateMoove;
                                    character.lastDataFrames[indiceFrame].stateOrientation = character.lastDataFrames[lastIndiceFrame].stateOrientation;
                                    character.Update(map, indiceFrame);
                                }
                            }
                            else { datagramsReceived.pullEmplacement++; if (datagramsReceived.pullEmplacement > SIZE_TAMPON_RECEIVED - 1)  datagramsReceived.pullEmplacement = 0; continue;}

                            for(int j=0; j < gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.nOthersClientActions; j++)
                            {
                                uint16_t nplayer = gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.otherClients[j].nClient;
                                allPlayers[nplayer]->connected = true;
                                allPlayers[nplayer]->position = gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.otherClients[j].position;
                                allPlayers[nplayer]->yaw = gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.otherClients[j].yaw;
                                allPlayers[nplayer]->stateMoove = gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.otherClients[j].actionMoove;
                                allPlayers[nplayer]->stateOrientation = gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.otherClients[j].actionOrientation;
                                allPlayers[nplayer]->lastActualizeFrame = gamePacketByServer->actualFrame;
                                if(allPlayers[nplayer]->stateMoove == 'w') allPlayers[nplayer]->animator->PlayAnimation(allPlayers[nplayer]->allAnimations[1]);
                                else if(allPlayers[nplayer]->stateMoove == 'n') allPlayers[nplayer]->animator->PlayAnimation(allPlayers[nplayer]->allAnimations[0]);
                                uint16_t diffFrame5 = InfoGlobal::currentFrame - allPlayers[nplayer]->lastActualizeFrame; if(InfoGlobal::currentFrame < allPlayers[nplayer]->lastActualizeFrame) { uint16_t testCurrentF = InfoGlobal::currentFrame + 20000; uint16_t testLastActuF = allPlayers[nplayer]->lastActualizeFrame + 20000; diffFrame5 = testCurrentF - testLastActuF;}
                                for(int k=0; k < diffFrame5; k++) allPlayers[nplayer]->UpdatePosition(map);
                                allPlayers[nplayer]->animator->UpdateAnimation(0.05f * diffFrame5);

                                if( j >= OTHERS_CLIENTS_SERVER1_CAPACITY - 1) break;

                            }
                            if(gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.nOthersClientActions <= OTHERS_CLIENTS_SERVER1_CAPACITY) nPacketWaited = 1;
                            else nPacketWaited = ceil((gamePacketByServer->dataPacket.sendData.dataServer.data1.untemporal.nOthersClientActions - OTHERS_CLIENTS_SERVER1_CAPACITY) / OTHERS_CLIENTS_SERVER2_CAPACITY) + 1;
                            nPacketReceived++;
                        }
                        else if(gamePacketByServer->dataPacket.sendData.numPacket >= 100) return false;
                        else
                        {
                            for(int j=0; j < gamePacketByServer->dataPacket.sendData.dataServer.data2.nElements; j++)
                            {
                                uint16_t nplayer = gamePacketByServer->dataPacket.sendData.dataServer.data2.otherClients[j].nClient;
                                allPlayers[nplayer]->connected = true;
                                allPlayers[nplayer]->position = gamePacketByServer->dataPacket.sendData.dataServer.data2.otherClients[j].position;
                                allPlayers[nplayer]->yaw = gamePacketByServer->dataPacket.sendData.dataServer.data2.otherClients[j].yaw;
                                allPlayers[nplayer]->stateMoove = gamePacketByServer->dataPacket.sendData.dataServer.data2.otherClients[j].actionMoove;
                                allPlayers[nplayer]->stateOrientation = gamePacketByServer->dataPacket.sendData.dataServer.data2.otherClients[j].actionOrientation;
                                allPlayers[nplayer]->lastActualizeFrame = gamePacketByServer->actualFrame;
                                if(allPlayers[nplayer]->stateMoove == 'w') allPlayers[nplayer]->animator->PlayAnimation(allPlayers[nplayer]->allAnimations[1]);
                                else if(allPlayers[nplayer]->stateMoove == 'n') allPlayers[nplayer]->animator->PlayAnimation(allPlayers[nplayer]->allAnimations[0]);
                                uint16_t diffFrame5 = InfoGlobal::currentFrame - allPlayers[nplayer]->lastActualizeFrame; if(InfoGlobal::currentFrame < allPlayers[nplayer]->lastActualizeFrame) { uint16_t testCurrentF = InfoGlobal::currentFrame + 20000; uint16_t testLastActuF = allPlayers[nplayer]->lastActualizeFrame + 20000; diffFrame5 = testCurrentF - testLastActuF;}
                                for(int k=0; k < diffFrame5; k++) allPlayers[nplayer]->UpdatePosition(map);
                                allPlayers[nplayer]->animator->UpdateAnimation(0.05f * diffFrame5);
                            }
                            nPacketReceived++;
                        }
                    }
                    datagramsReceived.pullEmplacement++; if (datagramsReceived.pullEmplacement > SIZE_TAMPON_RECEIVED - 1)  datagramsReceived.pullEmplacement = 0;
                }
                else break;
            }
            InfoGlobal::mutex1Reseau.unlock();
            if(nPacketReceived != nPacketWaited) return false;
        }

        auto end = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        if (diff < InfoGlobal::frameTimeInt)
        {
            Sleep(InfoGlobal::frameTimeInt - diff);
            InfoGlobal::coefFrame = 1;
	    }
        else if(diff < InfoGlobal::frameTimeInt + 10) InfoGlobal::coefFrame = 1;
        else
        {
            InfoGlobal::coefFrame = static_cast<int>(floor(diff / InfoGlobal::frameTimeInt) + 1);
            Sleep(InfoGlobal::frameTimeInt * InfoGlobal::coefFrame - diff);
        }
    }
    return true;
}

