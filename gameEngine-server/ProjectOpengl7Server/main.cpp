#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include<fonctionsUtilitaire.cpp>
#include <reseau/udpServer.h>


#include <infoGlobal.h>
#include <model/model_animation.h>
#include <elements/character.hpp>
#include <elements/map.h>
#include <elements/objectInWorld.hpp>

#include <elements/character.cpp>
#include <reseau/gameNetwork.cpp>




#include <includeSauvage/initVariableGlobal.cpp>   // initialisation des variables global


int main()
{
	#include <includeSauvage/initProgram.cpp>   // init stb_img


	#include <includeSauvage/beforeLoop.cpp>   // config vao/vbo/ebo/ubo +


	// ------LOOP--------
	while (1)
	{

		#include <includeSauvage/updateData.cpp>   // deltaTime + update character/objects

	}
	//-----------END LOOP-------------



	#include <includeSauvage/endProgram.cpp>  // finish program
}