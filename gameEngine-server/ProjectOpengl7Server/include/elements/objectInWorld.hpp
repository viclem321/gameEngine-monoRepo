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
        }      

        ~ObjectInWorld()
        {
                std::cout << "Un ObjectInWorld detruit " << std::endl;
        }

       
};




#endif