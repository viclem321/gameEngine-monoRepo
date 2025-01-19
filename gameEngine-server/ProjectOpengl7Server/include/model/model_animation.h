#ifndef MODEL_H
#define MODEL_H


struct Box
{
	glm::vec3 center;
	float lengthX;  // length betw center and xMax
	float lengthY;  // length betw center and yMax
	float lengthZ;  // length betw center and zMax
};

class Model 
{
public:
	unsigned char typeModel; // 'm' for matrix,       'v' for vec3,     'n' for dont pass model in buffer(no intanciation),    'N' same but vec3
	Box boundingBox;
	
	

    // constructor, expects a filepath to a 3D model.
    Model(glm::vec3 boxCenter, glm::vec3 boxLengths, unsigned char pTypeModel = 'n')
    {
		typeModel = pTypeModel;
		boundingBox.center = boxCenter; boundingBox.lengthX = boxLengths.x; boundingBox.lengthY = boxLengths.y; boundingBox.lengthZ = boxLengths.z;
    }
};




#endif