#ifndef INFOGLOBAL_H
#define INFOGLOBAL_H

class InfoInputs
{
    public:

        //keyboard
        static bool Kpressed;
        static bool KlastPressed;
        static bool Jpressed;
        static bool JlastPressed;
        static bool Lpressed;
        static bool LlastPressed;
        static bool Ppressed;                     //
        static bool PlastPressed;                 //   SUROUT NE PAS OUBLIER D'INNITIALISER A 0 (dans variableGlobal)
        static bool Opressed;                     //
        static bool Wpressed;                     //
        static bool Spressed;                     //   AND DONT FORGET TO UPDATE THE PROCESSINPUT WHEN I ADD A 'LASTPRESSED'
        static bool Apressed;                     //
        static bool Dpressed;                     //
        static bool Bpressed;
        //mouse
        static bool firstMouse;
        static bool mouseMoved;                   //
        static float Xmouse;                      //                 
        static float XlastMouse;                  //
        static float Ymouse;                      //   SUROUT NE PAS OUBLIER D'INNITIALISER A 0 (dans variableGlobal)
        static float YlastMouse;                  //
        //mouse scroll                            //
        static bool mouseScrolled;                //
        static float mouseScrollOffset;


        InfoInputs() { std::cout << "Vous avez creer une instance de InfoInputs, pourquoi??" << std::endl; }
};


class InfoGlobal
{
    public:
        //global
        static bool online; const static uint16_t nMaxClients;
        //screen
        static unsigned int SCR_WIDTH, SCR_HEIGHT;
        // data frame
        static uint16_t currentFrame, coefFrame;
        //deltaTme
        const static std::chrono::milliseconds frameTime; const static int frameTimeInt;
        //network
        static Udp *myUdp; static std::mutex mutex1Reseau;


        InfoGlobal() { std::cout << "Vous avez creer une instance de InfoGlobal, pourquoi??" << std::endl; }

};



// DECLARATION DES CLASS POUR EVITER CONFLITS
class ObjectInWorld;
class Camera; struct Frustum; struct Plan;
class Character;
class Map; class TileMap;




#endif