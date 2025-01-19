#ifndef INFOGLOBAL_H
#define INFOGLOBAL_H


class InfoGlobal
{
    public:

        // global
        const static uint16_t nMaxClients; static uint16_t nConnectedClients;
        // data frame
        static uint16_t currentFrame, coefFrame;
        // deltaTme
        const static std::chrono::milliseconds frameTime; const static int frameTimeInt;
        // network
        static std::mutex mutex1Reseau; static Udp *myUdp;


        InfoGlobal()
        {
            std::cout << "Vous avez creer une instance de InfoGlobal, pourquoi??" << std::endl;
        }



};



// DECLARATION DES CLASS POUR EVITER CONFLITS
class ObjectInWorld;
class Camera; struct Frustum; struct Plan;
struct Player; class Character;
class Map; class TileMap;




#endif