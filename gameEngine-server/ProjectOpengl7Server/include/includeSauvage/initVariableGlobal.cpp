// VARIABLES GLOBALES



//initialisation des variables statics de la class infoGlobal
const uint16_t InfoGlobal::nMaxClients = 100; uint16_t InfoGlobal::nConnectedClients = 0;
uint16_t InfoGlobal::currentFrame = 0; uint16_t InfoGlobal::coefFrame = 1;
const std::chrono::milliseconds InfoGlobal::frameTime(50); const int InfoGlobal::frameTimeInt = 50; // en milliseconde
std::mutex InfoGlobal::mutex1Reseau; Udp *InfoGlobal::myUdp;



//initialisation variables statics de la class Player/Character
uint8_t Character::allNextStateCursor = 0;



// END VARIABLE GLOBAL
