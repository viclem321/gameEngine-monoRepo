Character::Character(std::string pName, glm::vec3 pPosition)
{
    name = pName;
    position = pPosition;
    model = new Model(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.8f, 3.0, 0.6f));
    yaw = 0.0f;
    frontDirection = glm::vec3(std::sin(glm::radians(yaw)), 0, std::cos(glm::radians(yaw)));
    leftDirection = glm::vec3(std::sin(glm::radians(yaw + 90.f)), 0, std::cos(glm::radians(yaw + 90.f)));
    stateMoove = 'n'; stateMooveLastModifyFrame = InfoGlobal::currentFrame; stateOrientation = 'n'; stateOrientationLastModifyFrame = InfoGlobal::currentFrame;
    for(int i=0; i<CHARACTER_LATENCE; i++) { allNextStateMoove[i] = 'n'; allNextStateOrientation[i] = 'n';} modifyStateMooveThisFrame = true; modifyStateOrientationThisFrame = true;
}    


Character::~Character()
{
    delete model;
    std::cout << "Personnage detruit " << std::endl;
}



void Character::UpdatePosition(Map &map)
{
    //orientation
    if (stateOrientation == 'l')
    {
        yaw = yaw + (ORIENTATION_SPEED_CHARACTER * InfoGlobal::coefFrame); if(yaw > 360) yaw -= 360;
        frontDirection = glm::vec3(std::sin(glm::radians(yaw)), 0, std::cos(glm::radians(yaw)));
        leftDirection = glm::vec3(std::sin(glm::radians(yaw + 90.f)), 0, std::cos(glm::radians(yaw + 90.f)));
    }
    else if (stateOrientation == 'r')
    {
        yaw = yaw - (ORIENTATION_SPEED_CHARACTER * InfoGlobal::coefFrame); if(yaw < 0) yaw += 360;
        frontDirection = glm::vec3(std::sin(glm::radians(yaw)), 0, std::cos(glm::radians(yaw)));
        leftDirection = glm::vec3(std::sin(glm::radians(yaw + 90.f)), 0, std::cos(glm::radians(yaw + 90.f)));
    }


    //position
    glm::vec3 lastPosition = position;
    if (stateMoove == 'w') // front
    {
        position = position + (frontDirection * (SPEED_CHARACTER * InfoGlobal::coefFrame));
        position.y = map.getHeightInterpolate(position.x, position.z);
        if(position.y > lastPosition.y + 8.f)
        {
            position = lastPosition;
        }
        else
        {
            position = glm::normalize(position - lastPosition);
            position = lastPosition + (position * (SPEED_CHARACTER * InfoGlobal::coefFrame));
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
                position = lastPosition;
            }
        }
    }
    else if (stateMoove == 'r') // behind
    {
        position = position - (frontDirection * (SPEED_CHARACTER * InfoGlobal::coefFrame));
        position.y = map.getHeightInterpolate(position.x, position.z);
    }
}




void Character::Update(Map &map)
{
    UpdatePosition(map);
}

