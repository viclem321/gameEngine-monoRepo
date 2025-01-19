// BEGIN LOOP
InfoGlobal::currentFrame += InfoGlobal::coefFrame; // update currentframe
start += (InfoGlobal::frameTime * InfoGlobal::coefFrame); // update chrono
nPlayersActualize = 0;    Character::allNextStateCursor += 1;  if(Character::allNextStateCursor >= CHARACTER_LATENCE) Character::allNextStateCursor = 0;




// MANAGE PACKETS OF PLAYER + MANAGE NEW PLAYERS
InfoGlobal::mutex1Reseau.lock();
uint16_t pushEmpTest = datagramsReceived.pushEmplacement;
while (1)
{
    if (datagramsReceived.pullEmplacement != pushEmpTest)
    {
        ManagePacket(datagramsReceived.allDatagrams[datagramsReceived.pullEmplacement], allPlayers, &nPlayersActualize, listPlayersActualize, datagramsReceived);
        datagramsReceived.pullEmplacement++;
        if (datagramsReceived.pullEmplacement > SIZE_TAMPON_RECEIVED - 1)  datagramsReceived.pullEmplacement = 0;
    }
    else break;
}
InfoGlobal::mutex1Reseau.unlock();
// END MANAGE PACKETS OF PLAYER + MANAGE NEW PLAYERS





// UPDATE GAME
uint8_t lastCursorState = Character::allNextStateCursor - 1; if(lastCursorState >= CHARACTER_LATENCE) lastCursorState = CHARACTER_LATENCE - 1;
uint8_t oldCursorState = Character::allNextStateCursor - (CHARACTER_LATENCE - 1); if(oldCursorState > (std::numeric_limits<uint8_t>::max)() / 2) oldCursorState = (CHARACTER_LATENCE - 1) - ((std::numeric_limits<uint8_t>::max)() - oldCursorState);
for(int i=0; i<InfoGlobal::nMaxClients; i++)
{
	if(allPlayers[i].connected == true) 
    {
        if(allPlayers[i].character->modifyStateMooveThisFrame == false) allPlayers[i].character->allNextStateMoove[Character::allNextStateCursor] = allPlayers[i].character->allNextStateMoove[lastCursorState];
        if(allPlayers[i].character->modifyStateOrientationThisFrame == false) allPlayers[i].character->allNextStateOrientation[Character::allNextStateCursor] = allPlayers[i].character->allNextStateOrientation[lastCursorState];
        allPlayers[i].character->modifyStateMooveThisFrame = false; allPlayers[i].character->modifyStateOrientationThisFrame = false;
        if(allPlayers[i].character->allNextStateMoove[Character::allNextStateCursor] != allPlayers[i].character->allNextStateMoove[lastCursorState] || allPlayers[i].character->allNextStateOrientation[Character::allNextStateCursor] != allPlayers[i].character->allNextStateOrientation[lastCursorState])
        {
            allPlayers[i].timeWithoutSendState = 70;
        }
        else allPlayers[i].timeWithoutSendState += 1;

        uint16_t oldCursorStateTest;
        if(InfoGlobal::coefFrame < CHARACTER_LATENCE) { oldCursorStateTest = oldCursorState + (InfoGlobal::coefFrame - 1); if(oldCursorStateTest >= CHARACTER_LATENCE) oldCursorStateTest -= CHARACTER_LATENCE; }
        else oldCursorStateTest = lastCursorState;
        allPlayers[i].character->stateMoove = allPlayers[i].character->allNextStateMoove[oldCursorStateTest];        
        allPlayers[i].character->stateOrientation = allPlayers[i].character->allNextStateOrientation[oldCursorStateTest];


        if(allPlayers[i].timeWithoutSendState >= 10)
        {
            listPlayersActualize[nPlayersActualize] = i; nPlayersActualize += 1;
            allPlayers[i].timeWithoutSendState = 0;
        }

        allPlayers[i].character->Update(map1); 
    }
}




// SEND STATE TO PLAYERS
SendPacketGame(allPlayers, nPlayersActualize, listPlayersActualize);







//END LOOP, MANAGE TIME
auto end = std::chrono::steady_clock::now();
auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
if(diff > 3) std::cout << "DIFF   " << (int)diff << std::endl;
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

