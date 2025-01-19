
//------------------FUNCTIONS INPUTS-------------------------------------------------

void processInput(GLFWwindow* window)
{
	//quitter le programme
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	//copie des "pressed" vers les "lastPressed" pour maj
	InfoInputs::KlastPressed = InfoInputs::Kpressed; InfoInputs::JlastPressed = InfoInputs::Jpressed; InfoInputs::LlastPressed = InfoInputs::Lpressed; InfoInputs::PlastPressed = InfoInputs::Ppressed;

	//maj des touches pressed
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) InfoInputs::Ppressed = true; else InfoInputs::Ppressed = false;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) InfoInputs::Opressed = true; else InfoInputs::Opressed = false;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) InfoInputs::Wpressed = true; else InfoInputs::Wpressed = false;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) InfoInputs::Spressed = true; else InfoInputs::Spressed = false;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) InfoInputs::Apressed = true; else InfoInputs::Apressed = false;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) InfoInputs::Dpressed = true; else InfoInputs::Dpressed = false;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) InfoInputs::Kpressed = true; else InfoInputs::Kpressed = false;
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) InfoInputs::Lpressed = true; else InfoInputs::Lpressed = false;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) InfoInputs::Jpressed = true; else InfoInputs::Jpressed = false;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) InfoInputs::Bpressed = true; else InfoInputs::Bpressed = false;

}




// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	InfoGlobal::SCR_WIDTH = width;
	InfoGlobal::SCR_HEIGHT = height;
	glViewport(0, 0, width, height);
}




// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	InfoInputs::mouseMoved = true;

	InfoInputs::XlastMouse = InfoInputs::Xmouse;
	InfoInputs::YlastMouse = InfoInputs::Ymouse;

	InfoInputs::Xmouse = xpos;
	InfoInputs::Ymouse = InfoGlobal::SCR_HEIGHT - ypos;



	if (InfoInputs::firstMouse == true)
	{
		InfoInputs::XlastMouse = xpos;
		InfoInputs::YlastMouse = InfoGlobal::SCR_HEIGHT - ypos;
		InfoInputs::firstMouse = false;

	}
}




// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	InfoInputs::mouseScrolled = true;
	InfoInputs::mouseScrollOffset = yoffset;
}

//---------------END FUNCTIONS INPUTS -------------------------------------------------------------


