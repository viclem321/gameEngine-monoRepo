#ifndef CHARACTER_H
#define CHARACTER_H

const float SPEED_CHARACTER = 1.f; const float ORIENTATION_SPEED_CHARACTER = 20.f;  const uint8_t CHARACTER_STATE_LATENCY = 2;
const uint8_t N_CHARACTER_LAST_DATA_FRAME = 80;

struct LastFrames
{
    glm::vec3 position;
    float yaw; // if yaw = 0, character should look toward z axys
    glm::vec3 frontDirection;
    glm::vec3 leftDirection; // vector normalize toward the left of character
    unsigned char stateMoove; bool switchMoove; unsigned char nextStateMoove; // 'n' = no moove, 'w' = walk, 'r' = retreat;
    unsigned char stateOrientation; bool switchOrientation; unsigned char nextStateOrientation; // 'n' = no rotate, 'l' = left, 'r' = right;
};

class Character
{
    public:

       std::string name; uint16_t nClient;
       Model *model; Animation *allAnimations[2]; Animator *animator;
       LastFrames lastDataFrames[N_CHARACTER_LAST_DATA_FRAME]; uint8_t currentPosLastDataFrames; uint16_t lastFrameActualize;
       uint8_t timeWithoutTalkToServer;


       Character() = default;

       Character(std::string pName, std::string pPath, glm::vec3 pPosition);    

       ~Character();

        void UpdateState(); // call once per frame
        void UpdatePosition(Map &map, uint16_t indiceFrame);
        void UpdateAnim(uint16_t indiceFrame);
        void Update(Map &map, uint16_t indiceFrame);



       void Draw(Shader &pShader);
       
};


class OtherPlayers
{
    public:
    
        bool connected;
        glm::vec3 position;
        glm::vec3 frontDirection; glm::vec3 leftDirection;
        float yaw;
        unsigned char stateMoove; unsigned char stateOrientation;
        unsigned char allStateMooveForLatency[CHARACTER_STATE_LATENCY]; unsigned char allStateOrientationForLatency[CHARACTER_STATE_LATENCY];
        uint16_t lastActualizeFrame; glm::vec3 lastActualizeFramePosition; float lastActualizeFrameYaw;
        static Model *model; static Animation *allAnimations[2]; Animator *animator;

        OtherPlayers();

        void NewPlayer(glm::vec3 pPosition, unsigned char pStateMoove, unsigned char pStateOrientation, float pYaw, uint16_t frame);

        void UpdateState(uint16_t actualFrame); void UpdatePosition(Map &map);
        void BeginAnim(); void UpdateAnim();

        void Draw(Shader &pShader);
};



#endif