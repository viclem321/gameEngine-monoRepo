#ifndef MAP_H
#define MAP_H


class TileMap
{
    public:

        vector<ObjectInWorld*> *tileMap;
        int nCaseLargeur, nCaseLongueur, nCaseHauteur, volumeCase;

        TileMap(float pLargeurMap, float pLongueurMap, float pHauteurMap, int pVolumeCase)
        {
            nCaseLargeur = ceil(pLargeurMap / pVolumeCase);
            nCaseLongueur = ceil(pLongueurMap / pVolumeCase);
            nCaseHauteur = ceil(pHauteurMap / pVolumeCase);
            volumeCase = pVolumeCase;
            tileMap = (vector<ObjectInWorld*>*) calloc(nCaseLargeur * nCaseHauteur * nCaseLongueur, sizeof(vector<ObjectInWorld*>));
        }

        ~TileMap()
        {
            for(int i = 0; i < nCaseHauteur * nCaseLargeur * nCaseLongueur; i++)
                delete &tileMap[i];
            free(tileMap);
        }


        vector<ObjectInWorld*>* FindVector(glm::vec3 position)
        {
            int caseZ = floor(position.z / volumeCase);
            int caseX = floor(position.x / volumeCase);
            int caseY = floor(position.y / volumeCase);
            return &(tileMap[caseY * nCaseLargeur * nCaseHauteur + caseX * nCaseLargeur + caseY]);
        }
};

class Map
{
    public:
       string name;
       Model *model;
       unsigned short *pixelsMap; // unsgn short if 2 octets and char if 1
       int w,h,c;
       float largeurMap;
       float longueurMap;
       int divisionMap;
       TileMap *tileMap;

       Map(string path, string pName, int plargeurMap, int pDivisionMap, string pathModel)    // chemin de l'image, name que l'on veut donner a la map, largeur reelle de la map en comptant les contours , nombre par lequel les positions ont ete divise dans blender pour normaliser la couleur, pathColor
       {
           name = pName;
           model = new Model(pathModel, glm::vec3(0), glm::vec3(0), 'n');
           pixelsMap = stbi_load_16(path.c_str(), &w, &h, &c, 0);
           largeurMap = plargeurMap;
           longueurMap = plargeurMap;
           divisionMap = pDivisionMap;
           if(!pixelsMap)
           {
               std::cout << "Texture failed to load at path: " << path << std::endl;
			   stbi_image_free(pixelsMap);
           }
           tileMap = new TileMap(largeurMap, longueurMap, divisionMap, 100);
       }

       ~Map()
       {
            delete model;
            std::cout << "map detruite" << std::endl;
       }




       float getHeight(float pX, float pZ)
       {
           float x = pX/largeurMap;
           float z = pZ/largeurMap;

           int intX = floor(x * (h - 1));
           int intZ = floor(z * (w - 1));
           int ligne = (intX * w);
           int colonne = intZ;
           int indicePixel = ligne + colonne;

           unsigned short pixel = pixelsMap[indicePixel];  // unsgn short if 2 octets and char if 1
           
           float resultat = (float)pixel;
           resultat = resultat / 65536 * divisionMap;  // 65536 if 2 octets and 256 if 1
           return resultat;
       }


       float getHeightInterpolate(float pX, float pZ)
       {
            float resultatInterX, resultatInterZ;
           float x = pX/largeurMap;
           float z = pZ/largeurMap;
           x = x * (h - 1);
           z = z * (w - 1);

           int intX = floor(x);
           int intZ = floor(z);

           int ligne = (intX * w);
           int colonne = intZ;
           int indicePixel = ligne + colonne;
           unsigned short pixel = pixelsMap[indicePixel];  // unsgn short if 2 octets and char if 1
           float resultat = (float)pixel;
           resultat = resultat / 65536 * divisionMap;  // 65536 if 2 octets and 256 if 1


           if(round(x) == floor(x))
           {
                x = (x - intX) + 0.5f;
                int indiceInterpolateX = ligne + colonne - w;
                unsigned short pixelInterpolateX = pixelsMap[indiceInterpolateX];
                resultatInterX = (float)pixelInterpolateX;
                resultatInterX = resultatInterX / 65536 * divisionMap;
                if(fabs(resultatInterX - resultat) > 8.f )
                    resultatInterX = resultat;
                else
                    resultatInterX =  resultat * x + resultatInterX * (1-x);
           }
            else if (round(x) == intX + 1)
            {
                x = (x - intX) - 0.5f;
                int indiceInterpolateX = ligne + colonne + w;
                unsigned short pixelInterpolateX = pixelsMap[indiceInterpolateX];
                resultatInterX = (float)pixelInterpolateX;
                resultatInterX = resultatInterX / 65536 * divisionMap;
                if(fabs(resultatInterX - resultat) > 8.f )
                    resultatInterX = resultat;
                else
                    resultatInterX =  resultatInterX * x + resultat * (1-x);
            }
            else
                std::cout << "PAS NORMAL, PROBLEME DANS GETHEIGHTINTERPOLATE x" << std::endl;

            if(round(z) == floor(z))
           {
                z = (z - intZ) + 0.5f;
                int indiceInterpolateZ = ligne + colonne - 1;
                unsigned short pixelInterpolateZ = pixelsMap[indiceInterpolateZ];
                resultatInterZ = (float)pixelInterpolateZ;
                resultatInterZ = resultatInterZ / 65536 * divisionMap;
                if(fabs(resultatInterZ - resultat) > 8.f )
                    resultatInterZ = resultat;
                else
                    resultatInterZ =  resultat * z + resultatInterZ * (1-z);
           }
            else if (round(z) == intZ + 1)
            {
                z = (z - intZ) - 0.5f;
                int indiceInterpolateZ = ligne + colonne + 1;
                unsigned short pixelInterpolateZ = pixelsMap[indiceInterpolateZ];
                resultatInterZ = (float)pixelInterpolateZ;
                resultatInterZ = resultatInterZ / 65536 * divisionMap;
                if(fabs(resultatInterZ - resultat) > 8.f )
                    resultatInterZ = resultat;
                else
                    resultatInterZ =  resultatInterZ * z + resultat * (1-z);
            }
            else
                std::cout << "PAS NORMAL, PROBLEME DANS GETHEIGHTINTERPOLATE z" << std::endl;

            resultat = (resultatInterX + resultatInterZ) / 2;

           
           return resultat;
       }

       

       void Draw(Shader pShader)
       {
            pShader.use();
            glm::mat4 model1 = glm::mat4(1.0f);
            glUniformMatrix4fv(glGetUniformLocation(pShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model1));
            model->Draw(pShader);
       }

       
};




#endif