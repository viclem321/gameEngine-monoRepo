#ifndef CHARACTER_H
#define CHARACTER_H

const float SPEED_CHARACTER = 1.f; const float ORIENTATION_SPEED_CHARACTER = 20.f;  const uint8_t CHARACTER_LATENCE = 2;

struct Player
{
    bool connected;
    unsigned char stateInfo;  // 'd' = data,  'a' = send all info (for begin),  'n' for send nothing
    uint16_t timeWithoutSendState;
    Character *character;
};


class Character
{
    public:

       std::string name;
       glm::vec3 position;
       Model *model;
       float yaw; // if yaw = 0, character should look toward z axys
       glm::vec3 frontDirection; // vector normalize toward the front of character
       glm::vec3 leftDirection; // vector normalize toward the left of character
       unsigned char stateMoove; uint16_t stateMooveLastModifyFrame; bool modifyStateMooveThisFrame; // 'n' = no moove, 'w' = walk, 'r' = retreat;
       unsigned char stateOrientation; uint16_t stateOrientationLastModifyFrame; bool modifyStateOrientationThisFrame; // 'n' = no rotate, 'l' = left, 'r' = right;
       unsigned char allNextStateMoove[CHARACTER_LATENCE]; unsigned char allNextStateOrientation[CHARACTER_LATENCE]; static uint8_t allNextStateCursor;


       Character() = default;

       Character(std::string pName, glm::vec3 pPosition);    

       ~Character();



        void UpdatePosition(Map &map);
        
        void Update(Map &map);

       
};



#endif