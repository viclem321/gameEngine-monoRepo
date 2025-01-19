//BEGIN LOOP
	InfoGlobal::currentFrame += InfoGlobal::coefFrame; // update currentframe
	start += (InfoGlobal::frameTime * InfoGlobal::coefFrame); // update chrono
	character.currentPosLastDataFrames += InfoGlobal::coefFrame; if(character.currentPosLastDataFrames >= N_CHARACTER_LAST_DATA_FRAME) character.currentPosLastDataFrames = 0;
//END BEGIN LOOP


//RECEPTION FROM SERVER
InfoGlobal::mutex1Reseau.lock();
uint16_t pushEmpTest = datagramsReceived.pushEmplacement;
while (1)
{
    if (datagramsReceived.pullEmplacement != pushEmpTest)
    {
        ManagePacket(datagramsReceived.allDatagrams[datagramsReceived.pullEmplacement], character, allPlayers, map1, datagramsReceived);
        datagramsReceived.pullEmplacement++; if (datagramsReceived.pullEmplacement > SIZE_TAMPON_RECEIVED - 1)  datagramsReceived.pullEmplacement = 0;
    }
    else break;
}
InfoGlobal::mutex1Reseau.unlock();
//END RECEPTION FROM SERVER


//UPDATE GAME
for(int i=0; i<InfoGlobal::nMaxClients; i++)
{
	if(allPlayers[i]->connected == true)
	{
		if(i == character.nClient)
		{
			character.UpdateState();
			for(int j=0; j<InfoGlobal::coefFrame; j++)
			{
				uint8_t positionLastDataF = (character.currentPosLastDataFrames - (InfoGlobal::coefFrame - 1)) + j; if(positionLastDataF >= N_CHARACTER_LAST_DATA_FRAME) positionLastDataF =  N_CHARACTER_LAST_DATA_FRAME - (((std::numeric_limits<uint16_t>::max)() - positionLastDataF) + 1);
				character.Update(map1, positionLastDataF); 
			}
		}
		else 
		{ 
			for(int j=0; j<InfoGlobal::coefFrame; j++) { allPlayers[i]->UpdateState((InfoGlobal::currentFrame - InfoGlobal::coefFrame) + j + 1); allPlayers[i]->UpdatePosition(map1); }
		}
	}
}

// camera update
camera.Update(UBO1);


//END UPDATE GAME





// SEND INPUT TO SERVER

if(character.timeWithoutTalkToServer >= 8)
{
	//ENVOIE DES INPUT
	DatagramByClient datagr; datagr.type = 2; GamePacketByClient *gamePacketByClient = reinterpret_cast<GamePacketByClient*>(datagr.data);
	gamePacketByClient->typePacket = 2;
	gamePacketByClient->dataPacketByClient.inputPacket.actualFrame = InfoGlobal::currentFrame;
	gamePacketByClient->dataPacketByClient.inputPacket.actionMoove = character.lastDataFrames[character.currentPosLastDataFrames].nextStateMoove;
	gamePacketByClient->dataPacketByClient.inputPacket.actionOrientation = character.lastDataFrames[character.currentPosLastDataFrames].nextStateOrientation;
	InfoGlobal::mutex1Reseau.lock(); InfoGlobal::myUdp->Send(datagr); InfoGlobal::mutex1Reseau.unlock();

	character.timeWithoutTalkToServer = 0;
}

// END SEND INPUT TO SERVER

