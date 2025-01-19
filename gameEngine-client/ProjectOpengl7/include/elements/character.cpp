Character::Character(std::string pName, std::string pPath, glm::vec3 pPosition)
{
    name = pName;
    model = new Model(pPath, glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.8f, 3.0, 0.6f));
    allAnimations[0] = new Animation(pPath, model, 0);  // idle
    allAnimations[1] = new Animation(pPath, model, 1);  // run
    animator = new Animator(allAnimations[0]);
    currentPosLastDataFrames = 0; 
    for(int i=0; i<N_CHARACTER_LAST_DATA_FRAME; i++) { lastDataFrames[i].yaw = 0.0f; lastDataFrames[i].stateMoove = 'n'; lastDataFrames[i].nextStateMoove = 'n'; lastDataFrames[i].stateOrientation ='n'; lastDataFrames[i].nextStateOrientation = 'n'; lastDataFrames[i].switchMoove = false; lastDataFrames[i].switchOrientation = false; lastDataFrames[i].frontDirection = glm::vec3(std::sin(glm::radians(lastDataFrames[i].yaw)), 0, std::cos(glm::radians(lastDataFrames[i].yaw))); lastDataFrames[i].leftDirection = glm::vec3(std::sin(glm::radians(lastDataFrames[i].yaw + 90.f)), 0, std::cos(glm::radians(lastDataFrames[i].yaw + 90.f)));}
    timeWithoutTalkToServer = 0;
}    

Character::~Character()
{
    delete model; delete allAnimations[0]; delete allAnimations[1]; delete animator;
    std::cout << "Personnage detruit " << std::endl;
}


void Character::UpdateState() // call this function on time per frame only
{
    uint8_t actualIndiceFrame = currentPosLastDataFrames - InfoGlobal::coefFrame; if(actualIndiceFrame >= N_CHARACTER_LAST_DATA_FRAME) actualIndiceFrame = N_CHARACTER_LAST_DATA_FRAME - (((std::numeric_limits<uint8_t>::max)() - actualIndiceFrame) + 1) ;
    uint8_t lastIndiceFrame = actualIndiceFrame - 1; if(lastIndiceFrame >= N_CHARACTER_LAST_DATA_FRAME) lastIndiceFrame = N_CHARACTER_LAST_DATA_FRAME - 1;
    uint8_t oldIndiceFrame = actualIndiceFrame - (CHARACTER_STATE_LATENCY - 1); if(oldIndiceFrame >= N_CHARACTER_LAST_DATA_FRAME) oldIndiceFrame = N_CHARACTER_LAST_DATA_FRAME - (((std::numeric_limits<uint8_t>::max)() - oldIndiceFrame) + 1);
    for(int i=0; i<InfoGlobal::coefFrame; i++)
    {
        lastIndiceFrame = actualIndiceFrame;
        actualIndiceFrame += 1; if(actualIndiceFrame >= N_CHARACTER_LAST_DATA_FRAME) actualIndiceFrame -= N_CHARACTER_LAST_DATA_FRAME;
        oldIndiceFrame += 1; if(oldIndiceFrame >= N_CHARACTER_LAST_DATA_FRAME) oldIndiceFrame -= N_CHARACTER_LAST_DATA_FRAME;

        lastDataFrames[actualIndiceFrame].stateMoove = lastDataFrames[oldIndiceFrame].nextStateMoove; lastDataFrames[actualIndiceFrame].switchMoove = false; lastDataFrames[actualIndiceFrame].nextStateMoove = lastDataFrames[lastIndiceFrame].nextStateMoove;
        lastDataFrames[actualIndiceFrame].stateOrientation = lastDataFrames[oldIndiceFrame].nextStateOrientation; lastDataFrames[actualIndiceFrame].switchOrientation = false; lastDataFrames[actualIndiceFrame].nextStateOrientation = lastDataFrames[lastIndiceFrame].nextStateOrientation;
    }

    if(InfoInputs::Kpressed) lastDataFrames[currentPosLastDataFrames].nextStateMoove = 'w';  else lastDataFrames[currentPosLastDataFrames].nextStateMoove = 'n';
    if(lastDataFrames[currentPosLastDataFrames].nextStateMoove != lastDataFrames[lastIndiceFrame].nextStateMoove) lastDataFrames[currentPosLastDataFrames].switchMoove = true;

    if(InfoInputs::Jpressed) lastDataFrames[currentPosLastDataFrames].nextStateOrientation = 'l'; else if(InfoInputs::Lpressed) lastDataFrames[currentPosLastDataFrames].nextStateOrientation = 'r'; else lastDataFrames[currentPosLastDataFrames].nextStateOrientation = 'n';
    if(lastDataFrames[currentPosLastDataFrames].nextStateOrientation != lastDataFrames[lastIndiceFrame].nextStateOrientation) lastDataFrames[currentPosLastDataFrames].switchOrientation = true;


    if(lastDataFrames[currentPosLastDataFrames].switchMoove == true || lastDataFrames[currentPosLastDataFrames].switchOrientation == true) timeWithoutTalkToServer = 40;
    else timeWithoutTalkToServer += InfoGlobal::coefFrame;

}

void Character::UpdatePosition(Map &map, uint16_t indiceFrame)
{
    uint8_t lastIndiceFrame = indiceFrame - 1; if(lastIndiceFrame >= N_CHARACTER_LAST_DATA_FRAME) lastIndiceFrame = N_CHARACTER_LAST_DATA_FRAME - 1;

    //orientation
    if (lastDataFrames[indiceFrame].stateOrientation == 'l') // gauche
    {
        lastDataFrames[indiceFrame].yaw = lastDataFrames[lastIndiceFrame].yaw + ORIENTATION_SPEED_CHARACTER ; if(lastDataFrames[indiceFrame].yaw > 360) lastDataFrames[indiceFrame].yaw -= 360;
    }
    else if (lastDataFrames[indiceFrame].stateOrientation == 'r') // droite
    {
        lastDataFrames[indiceFrame].yaw = lastDataFrames[lastIndiceFrame].yaw - ORIENTATION_SPEED_CHARACTER ; if(lastDataFrames[indiceFrame].yaw < 0) lastDataFrames[indiceFrame].yaw += 360;
    }
    else
    {
        lastDataFrames[indiceFrame].yaw = lastDataFrames[lastIndiceFrame].yaw;
    }
    lastDataFrames[indiceFrame].frontDirection = glm::vec3(std::sin(glm::radians(lastDataFrames[indiceFrame].yaw)), 0, std::cos(glm::radians(lastDataFrames[indiceFrame].yaw)));
    lastDataFrames[indiceFrame].leftDirection = glm::vec3(std::sin(glm::radians(lastDataFrames[indiceFrame].yaw + 90.f)), 0, std::cos(glm::radians(lastDataFrames[indiceFrame].yaw + 90.f)));


    //position
    glm::vec3 lastPosition = lastDataFrames[lastIndiceFrame].position;
    if (lastDataFrames[indiceFrame].stateMoove == 'w') // front
    {
        lastDataFrames[indiceFrame].position = lastDataFrames[lastIndiceFrame].position + (lastDataFrames[indiceFrame].frontDirection * SPEED_CHARACTER);
        lastDataFrames[indiceFrame].position.y = map.getHeightInterpolate(lastDataFrames[indiceFrame].position.x, lastDataFrames[indiceFrame].position.z);
        if(lastDataFrames[indiceFrame].position.y > lastPosition.y + 8.f)
        {
            lastDataFrames[indiceFrame].position = lastDataFrames[lastIndiceFrame].position;
        }
        else
        {
            lastDataFrames[indiceFrame].position = glm::normalize(lastDataFrames[indiceFrame].position - lastDataFrames[lastIndiceFrame].position);
            lastDataFrames[indiceFrame].position = lastDataFrames[lastIndiceFrame].position + (lastDataFrames[indiceFrame].position * SPEED_CHARACTER);
            lastDataFrames[indiceFrame].position.y = map.getHeightInterpolate(lastDataFrames[indiceFrame].position.x, lastDataFrames[indiceFrame].position.z);
            if(lastDataFrames[indiceFrame].position.y > lastDataFrames[lastIndiceFrame].position.y + 8.f)
            {
                lastDataFrames[indiceFrame].position = lastDataFrames[lastIndiceFrame].position;
            }
        }

        std::vector<ObjectInWorld*> *currentCase = map.tileMap->FindVector(lastDataFrames[indiceFrame].position);
        for(int i=0; i < currentCase->size(); i++ )
        {
            ObjectInWorld *object = currentCase->at(i);
            Box *box = &(object->model->boundingBox);
            glm::vec3 realCenter = *((glm::vec3*)(object->arrayPositions)) + box->center;
            if(lastDataFrames[indiceFrame].position.x >= realCenter.x - box->lengthX && lastDataFrames[indiceFrame].position.x <= realCenter.x + box->lengthX && lastDataFrames[indiceFrame].position.z >= realCenter.z - box->lengthZ && lastDataFrames[indiceFrame].position.z <= realCenter.z + box->lengthZ && lastDataFrames[indiceFrame].position.y >= realCenter.y - box->lengthY && lastDataFrames[indiceFrame].position.y <= realCenter.y + box->lengthY)
            {
                lastDataFrames[indiceFrame].position = lastDataFrames[lastIndiceFrame].position; break;
            }
        }
    }
    else if(lastDataFrames[indiceFrame].stateMoove == 'n')
    {
        lastDataFrames[indiceFrame].position = lastDataFrames[lastIndiceFrame].position;
    }
}

void Character::UpdateAnim(uint16_t indiceFrame)
{
    uint16_t lastIndiceFrame = indiceFrame - 1; if(lastIndiceFrame >= N_CHARACTER_LAST_DATA_FRAME) lastIndiceFrame = N_CHARACTER_LAST_DATA_FRAME - 1;
    if(lastDataFrames[indiceFrame].stateMoove != lastDataFrames[lastIndiceFrame].stateMoove)
    {
        if(lastDataFrames[indiceFrame].stateMoove == 'w') animator->PlayAnimation(allAnimations[1]);  // begin anim walk
        else if(lastDataFrames[indiceFrame].stateMoove == 'n') animator->PlayAnimation(allAnimations[0]); // begin anim idle
    }
    else animator->UpdateAnimation(0.05f);
}

void Character::Update(Map &map, uint16_t indiceFrame)
{
    UpdatePosition(map, indiceFrame);
    UpdateAnim(indiceFrame);
}

void Character::Draw(Shader &pShader)
{
    pShader.use();
    glm::mat4 model1 = glm::mat4(1.0f);
    model1 = glm::translate(model1, glm::vec3(lastDataFrames[currentPosLastDataFrames].position.x, lastDataFrames[currentPosLastDataFrames].position.y - 0.16, lastDataFrames[currentPosLastDataFrames].position.z));
    model1 = glm::rotate(model1, glm::radians(lastDataFrames[currentPosLastDataFrames].yaw), glm::vec3(0, 1, 0));
    model1 = glm::scale(model1, glm::vec3(0.007f));
    glUniformMatrix4fv(glGetUniformLocation(pShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model1));
    auto transforms = animator->GetFinalBoneMatrices();
    for (int i = 0; i < transforms.size(); ++i)
        pShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
    model->Draw(pShader);
}








//OTHER PLAYERS

OtherPlayers::OtherPlayers()
{
    connected = false;
    animator = new Animator(allAnimations[0]);
}

void OtherPlayers::NewPlayer(glm::vec3 pPosition, unsigned char pStateMoove, unsigned char pStateOrientation, float pYaw, uint16_t frame)
{
    connected = true;
    position = pPosition;
    yaw = pYaw;
    stateMoove = pStateMoove; stateOrientation = pStateOrientation;
    for(int i=0; i<CHARACTER_STATE_LATENCY; i++) { allStateMooveForLatency[i] = 'o'; allStateOrientationForLatency[i] = 'o'; }
    lastActualizeFrame = frame; lastActualizeFramePosition = pPosition; lastActualizeFrameYaw = pYaw;
}

void OtherPlayers::UpdateState(uint16_t actualFrame)
{
    uint16_t testDiff7 = SequenceDiff(actualFrame, lastActualizeFrame);
    if(testDiff7 <= CHARACTER_STATE_LATENCY -1)
    {
        if(allStateMooveForLatency[testDiff7] != 'o' && allStateMooveForLatency[testDiff7] != stateMoove) { stateMoove = allStateMooveForLatency[testDiff7]; BeginAnim(); }
        else UpdateAnim();
        if(allStateOrientationForLatency[testDiff7] != 'o' && allStateOrientationForLatency[testDiff7] != stateOrientation) { stateOrientation = allStateOrientationForLatency[testDiff7]; }
    }
    else UpdateAnim();
}


void OtherPlayers::UpdatePosition(Map &map)
{

    //orientation
    if (stateOrientation == 'l') { yaw = yaw + (ORIENTATION_SPEED_CHARACTER); if(yaw > 360) yaw -= 360; }     // gauche
    else if (stateOrientation == 'r') { yaw = yaw - (ORIENTATION_SPEED_CHARACTER); if(yaw < 0) yaw += 360; }  // droite
    frontDirection = glm::vec3(std::sin(glm::radians(yaw)), 0, std::cos(glm::radians(yaw)));
    leftDirection = glm::vec3(std::sin(glm::radians(yaw + 90.f)), 0, std::cos(glm::radians(yaw + 90.f)));


    //position
    glm::vec3 lastPosition = position;
    if (stateMoove == 'w') // front
    {
        position = position + (frontDirection * SPEED_CHARACTER);
        position.y = map.getHeightInterpolate(position.x, position.z);
        if(position.y > lastPosition.y + 8.f)
        {
            position = lastPosition;
        }
        else
        {
            position = position - lastPosition;
            position = lastPosition + (position * SPEED_CHARACTER);
            position.y = map.getHeightInterpolate(position.x, position.z);
            if(position.y > lastPosition.y + 8.f)
            {
                position = lastPosition;
            }
        }

        std::vector<ObjectInWorld*> *currentCase = map.tileMap->FindVector(position);
        for(int i=0; i < currentCase->size(); i++ )
        {
            ObjectInWorld *object = currentCase->at(i);
            Box *box = &(object->model->boundingBox);
            glm::vec3 realCenter = *((glm::vec3*)(object->arrayPositions)) + box->center;
            if(position.x >= realCenter.x - box->lengthX && position.x <= realCenter.x + box->lengthX && position.z >= realCenter.z - box->lengthZ && position.z <= realCenter.z + box->lengthZ && position.y >= realCenter.y - box->lengthY && position.y <= realCenter.y + box->lengthY)
            {
                position = lastPosition; break;
            }
        }
    }
}

void OtherPlayers::BeginAnim()
{
    if(stateMoove == 'w') animator->PlayAnimation(allAnimations[1]);  // begin anim walk
    else if(stateMoove == 'n') animator->PlayAnimation(allAnimations[0]); // begin anim idle
}

void OtherPlayers::UpdateAnim()
{
    animator->UpdateAnimation(0.05f);
}


void OtherPlayers::Draw(Shader &pShader)
{
    pShader.use();
    glm::mat4 model1 = glm::mat4(1.0f);
    model1 = glm::translate(model1, glm::vec3(position.x, position.y - 0.16, position.z));
    model1 = glm::rotate(model1, glm::radians(yaw), glm::vec3(0, 1, 0));
    model1 = glm::scale(model1, glm::vec3(0.007f));
    glUniformMatrix4fv(glGetUniformLocation(pShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model1));
    auto transforms = animator->GetFinalBoneMatrices();
    for (int i = 0; i < transforms.size(); ++i)
        pShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
    model->Draw(pShader);
}