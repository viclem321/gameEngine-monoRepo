#ifndef MESH_H
#define MESH_H


using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
    //color ambient diffuse
    glm::vec4 AmbDiff;
    //color specular
    glm::vec4 Specular;
    //color shininess
    float Shininess;
    //use texture diff
    float useDiffuseTexture;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh {
public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned char typeModel; //'m' for matrix or 'v' for vector3  'n' for no instancing
    unsigned int VAOs[5], VBOs[5];
    unsigned int EBO;
    unsigned char currentVAOandVBO;
    unsigned short nObjectslied;


    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, unsigned char pTypeModel)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        typeModel = pTypeModel;
        nObjectslied = 0;
        currentVAOandVBO = 0;

        setupMesh();
    }


    void Draw(Shader &shader, int pNInstance, unsigned short pCurrentVao)
    {
        // bind appropriate textures
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr   = 1;
        unsigned int heightNr   = 1;
        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            string number;
            string name = textures[i].type;
            if(name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if(name == "texture_specular")
                number = std::to_string(specularNr++); // transfer unsigned int to string
            else if(name == "texture_normal")
                number = std::to_string(normalNr++); // transfer unsigned int to string
             else if(name == "texture_height")
                number = std::to_string(heightNr++); // transfer unsigned int to string

            // now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        
        // draw mesh
        glBindVertexArray(VAOs[pCurrentVao]);
        if(pNInstance < 2)
        {
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        }
        else
        {
            glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, pNInstance);
        }
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }




    unsigned short SetModelArray(int length, void *modelArray)
	{
        currentVAOandVBO = nObjectslied;

        glGenVertexArrays(1, &VAOs[currentVAOandVBO]);
        glGenBuffers(1, &VBOs[currentVAOandVBO]);

		glBindVertexArray(VAOs[currentVAOandVBO]);
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[currentVAOandVBO]);

        if(currentVAOandVBO == 0)
        {
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        }

        if(typeModel == 'v')
        {
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex) + length * sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), &vertices[0]);
            glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), sizeof(glm::vec3) * length, modelArray);
            // je pourrais ensuite supp le modelArray pour opti
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

            glEnableVertexAttribArray(11);
            glVertexAttribPointer(11, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(vertices.size() * sizeof(Vertex)));
            glVertexAttribDivisor(11, 1);
        }
        else if(typeModel == 'm')
        {
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex) + length * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), &vertices[0]); 
            glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), sizeof(glm::mat4) * length, modelArray);
            // je pourrais ensuite supp le modelArray pour opti
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

            glEnableVertexAttribArray(11);
            glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(vertices.size() * sizeof(Vertex)));
            glEnableVertexAttribArray(12);
            glVertexAttribPointer(12, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(vertices.size() * sizeof(Vertex) + sizeof(glm::vec4)));
            glEnableVertexAttribArray(13);
            glVertexAttribPointer(13, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(vertices.size() * sizeof(Vertex) + sizeof(glm::vec4) * 2));
            glEnableVertexAttribArray(14);
            glVertexAttribPointer(14, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(vertices.size() * sizeof(Vertex) + sizeof(glm::vec4) * 3));

            glVertexAttribDivisor(11, 1);
            glVertexAttribDivisor(12, 1);
            glVertexAttribDivisor(13, 1);
            glVertexAttribDivisor(14, 1);
        }
        else
            std::cout << "Fonction SetModelArray : appel inutile" << std::endl;

        // vertex Positions
        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);	
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		// ids bones
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
		// weights
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
        // color ambiant/diffuse
        glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, AmbDiff));
        // specular
        glEnableVertexAttribArray(8);
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Specular));
        // shininess
        glEnableVertexAttribArray(9);
		glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Shininess));
        // use texture diff
		glEnableVertexAttribArray(10);
		glVertexAttribPointer(10, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, useDiffuseTexture));

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        nObjectslied += 1;
        return nObjectslied - 1;

	}



private:

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        if(typeModel == 'n' || typeModel == 'N')
        {
            // create buffers/arrays
            glGenVertexArrays(1, &VAOs[currentVAOandVBO]);
            glGenBuffers(1, &VBOs[currentVAOandVBO]);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAOs[currentVAOandVBO]);
            glBindBuffer(GL_ARRAY_BUFFER, VBOs[currentVAOandVBO]);

            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


            // VBO ATTRIBS
            // vertex Positions
            glEnableVertexAttribArray(0);	
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
            // vertex normals
            glEnableVertexAttribArray(1);	
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
            // vertex texture coords
            glEnableVertexAttribArray(2);	
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
            // vertex tangent
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
            // vertex bitangent
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
            // ids bones
            glEnableVertexAttribArray(5);
            glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
            // weights
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
            // color ambiant/diffuse
            glEnableVertexAttribArray(7);
            glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, AmbDiff));
            // specular
            glEnableVertexAttribArray(8);
            glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Specular));
            // shininess
            glEnableVertexAttribArray(9);
            glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Shininess));
            // use texture diff
            glEnableVertexAttribArray(10);
            glVertexAttribPointer(10, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, useDiffuseTexture));

            glBindVertexArray(0);
        }

    }
};
#endif