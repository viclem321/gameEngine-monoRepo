// VARIABLES GLOBALES


//initialisation des variables statics de la class infoGlobal
bool InfoGlobal::online = true; const uint16_t InfoGlobal::nMaxClients = 100;
unsigned int InfoGlobal::SCR_WIDTH = 1400;  unsigned int InfoGlobal::SCR_HEIGHT = 900;
uint16_t InfoGlobal::currentFrame = (std::numeric_limits<uint16_t>::max)() ; uint16_t InfoGlobal::coefFrame = 1;
const std::chrono::milliseconds InfoGlobal::frameTime(50); const int InfoGlobal::frameTimeInt = 50; // en milliseconde
Udp *InfoGlobal::myUdp; std::mutex InfoGlobal::mutex1Reseau;


//initialisation des variables statics de la class InfoInputs
bool InfoInputs::Kpressed, InfoInputs::KlastPressed, InfoInputs::Lpressed, InfoInputs::LlastPressed, InfoInputs::Jpressed, InfoInputs::JlastPressed, InfoInputs::Ppressed, InfoInputs::PlastPressed, InfoInputs::Opressed, InfoInputs::Wpressed, InfoInputs::Spressed, InfoInputs::Apressed, InfoInputs::Dpressed, InfoInputs::Bpressed = false;
bool InfoInputs::firstMouse = true;  bool InfoInputs::mouseMoved = false;  bool InfoInputs::mouseScrolled = false; 
float InfoInputs::Xmouse, InfoInputs::XlastMouse = InfoGlobal::SCR_WIDTH/2;   float InfoInputs::Ymouse, InfoInputs::YlastMouse = InfoGlobal::SCR_HEIGHT/2;
float InfoInputs::mouseScrollOffset = 0;






//initialisation des autres variables statics
Model *OtherPlayers::model; Animation* OtherPlayers::allAnimations[2];


// END VARIABLE GLOBAL


