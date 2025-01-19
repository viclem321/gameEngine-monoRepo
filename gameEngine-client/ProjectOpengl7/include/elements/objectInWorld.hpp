#ifndef OBJECTINWORLD_H
#define OBJECTINWORLD_H


class ObjectInWorld
{
    public:
        unsigned char modelType;     //  'm' for pass matrix models for each mesh,     'v' for pass vector3 models for each mesh,   'n' if i dont pass model1 in buffer(mat4),     'N' if i dont pass model1 in buffer(vec3)
        unsigned char collisionType;    //  'c' for basique collision,   'n' for no collision,    'h' for heightmap collision
        unsigned char drawType;         //  'a' for always draw,       'n' for draw only if near
        void *arrayPositions;      // matrice or vector model
        Model *model;
        int nInstances;        // le nb d'instance prevu pour cette objet
        unsigned short numVAO;   // numero de l'objet pour le modele, uniquement dans le cas de l'instanciation

        ObjectInWorld(Model *pModel, void *pArrayPositions, unsigned char pModelType = 'n', bool pCollision = false, unsigned char pDrawType = 'a', int pNInstances = 1)
        {
                collisionType = pCollision;
                drawType = pDrawType;
                nInstances = pNInstances;
                modelType = pModelType;
                arrayPositions = pArrayPositions;
                if(modelType == pModel->typeModel)
                        model = pModel;
                else
                        std::cout << "les information de l'objet ne correspondent pas avc le modele" << std::endl;
                
                if(modelType == 'm' || modelType == 'v')
                        numVAO = model->SetModelArray(nInstances, arrayPositions);
                else
                        numVAO = 0;
        }      

        ~ObjectInWorld()
        {
                std::cout << "Un ObjectInWorld detruit " << std::endl;
        }




        bool isOnOrForwardPlan(Plan &plan)
        {
                glm::vec3 *pointeurPos = (glm::vec3*)arrayPositions;
                // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
                const float r = model->boundingBox.lengthX * std::abs(plan.normal.x) + model->boundingBox.lengthY * std::abs(plan.normal.y) +
                        model->boundingBox.lengthZ * std::abs(plan.normal.z);

                return -r <= glm::dot(plan.normal, *pointeurPos + model->boundingBox.center) - plan.distance;
        }

        bool isOnFrustum(Camera &cam)
        {
                return (isOnOrForwardPlan(cam.frustum.leftFace) && isOnOrForwardPlan(cam.frustum.rightFace) && isOnOrForwardPlan(cam.frustum.topFace) && isOnOrForwardPlan(cam.frustum.bottomFace) && isOnOrForwardPlan(cam.frustum.nearFace) && isOnOrForwardPlan(cam.frustum.farFace));
        }







        void Draw(Shader &pShader)
       {
            pShader.use();

            if(modelType == 'n')
            {
                glm::mat4 *test = (glm::mat4*)arrayPositions;
                glUniformMatrix4fv(glGetUniformLocation(pShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(*test));
            }
            else if(modelType == 'N')
            {
                glm::vec3 *test = (glm::vec3*)arrayPositions;
                glUniform3f(glGetUniformLocation(pShader.ID, "model"), test->x, test->y, test->z);
            }
            
            model->Draw(pShader, nInstances, numVAO);
       }

       
};




#endif