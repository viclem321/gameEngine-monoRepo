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

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stb_image.h>
#include <fonctionsUtilitaire.cpp>
#include <network/udpClient.h>

#include <infoGlobal.h>
#include <model/shader_m.h>
#include <model/animator.h>
#include <model/model_animation.h>
#include <elements/map.h>
#include <elements/character.hpp>
#include <network/gameNetwork.cpp>
#include <elements/camera.h>
#include <elements/objectInWorld.hpp>

#include <elements/character.cpp>




#include <includeSauvage/variableGlobal.cpp>   // declaration global variable  +  initialisation static class

#include <includeSauvage/inputFunctions.cpp>   // all input functions for glfw


int main()
{
	#include <includeSauvage/initProgram.cpp>   // init glfw/glad/stb_img/opengl

	#include <includeSauvage/beforeLoop.cpp>   // config vao/vbo/ebo/ubo + load shader/model/anim/map



	// ------------LOOP------------

	while (!glfwWindowShouldClose(window) && InfoGlobal::online == true)
	{
		#include <includeSauvage/updateData.cpp>   // deltaTime + update character/objects/anims

		#include <includeSauvage/updateRender.cpp>   // clear + update view/projection in UBO + render

		#include <includeSauvage/endLoop.cpp> // finish loop


		processInput(window);
		InfoInputs::mouseMoved = false;
		InfoInputs::mouseScrolled = false;
		glfwPollEvents();
	}

	//-----------END LOOP-------------


	#include <includeSauvage/endProgram.cpp>   // end program
}