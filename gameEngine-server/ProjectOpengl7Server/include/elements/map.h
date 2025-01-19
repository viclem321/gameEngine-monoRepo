#ifndef MAP_H
#define MAP_H


class TileMap
{
    public:

        std::vector<ObjectInWorld*> *tileMap;
        int nCaseLargeur, nCaseLongueur, nCaseHauteur, volumeCase;

        TileMap(float pLargeurMap, float pLongueurMap, float pHauteurMap, int pVolumeCase)
        {
            nCaseLargeur = static_cast<int>(ceil(pLargeurMap / pVolumeCase));
            nCaseLongueur = static_cast<int>(ceil(pLongueurMap / pVolumeCase));
            nCaseHauteur = static_cast<int>(ceil(pHauteurMap / pVolumeCase));
            volumeCase = pVolumeCase;
            tileMap = (std::vector<ObjectInWorld*>*) calloc(nCaseLargeur * nCaseHauteur * nCaseLongueur, sizeof(std::vector<ObjectInWorld*>));
        }

        ~TileMap()
        {
            for(int i = 0; i < nCaseHauteur * nCaseLargeur * nCaseLongueur; i++)
                delete &tileMap[i];
            free(tileMap);
        }


        std::vector<ObjectInWorld*>* FindVector(glm::vec3 position)
        {
            int caseZ = static_cast<int>(floor(position.z / volumeCase));
            int caseX = static_cast<int>(floor(position.x / volumeCase));
            int caseY = static_cast<int>(floor(position.y / volumeCase));
            return &(tileMap[caseY * nCaseLargeur * nCaseHauteur + caseX * nCaseLargeur + caseY]);
        }
};

class Map
{
    public:
       std::string name;
       unsigned short *pixelsMap; // unsgn short if 2 octets and char if 1
       int w,h,c;
       float largeurMap;
       float longueurMap;
       int divisionMap;
       TileMap *tileMap;

       Map(std::string path, std::string pName, int plargeurMap, int pDivisionMap)    // chemin de l'image, name que l'on veut donner a la map, largeur reelle de la map en comptant les contours , nombre par lequel les positions ont ete divise dans blender pour normaliser la couleur, pathColor
       {
           name = pName;
           pixelsMap = stbi_load_16(path.c_str(), &w, &h, &c, 0);
           largeurMap = static_cast<float>(plargeurMap);
           longueurMap = static_cast<float>(plargeurMap);
           divisionMap = pDivisionMap;
           if(!pixelsMap)
           {
               std::cout << "Texture failed to load at path: " << path << std::endl;
			   stbi_image_free(pixelsMap);
           }
           tileMap = new TileMap(largeurMap, longueurMap, static_cast<float>(divisionMap), 100);
       }

       ~Map()
       {
            std::cout << "map detruite" << std::endl;
       }




       float getHeight(float pX, float pZ)
       {
           float x = pX/largeurMap;
           float z = pZ/largeurMap;

           int intX = static_cast<int>(floor(x * (h - 1)));
           int intZ = static_cast<int>(floor(z * (w - 1)));
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

           int intX = static_cast<int>(floor(x));
           int intZ = static_cast<int>(floor(z));

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

       
};




#endif