#ifndef CAMERA_H
#define CAMERA_H


// Default camera values
const float YAW         = 0.0f;
const float PITCH       =  0.0f;
const float SPEED       =  32.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


struct Plan
{
    glm::vec3 normal = { 0.f, 1.f, 0.f };

    // distance from origin to the nearest point in the plan
    float     distance = 0.f;          
};

struct Frustum
{
    Plan topFace;
    Plan bottomFace;

    Plan rightFace;
    Plan leftFace;

    Plan farFace;
    Plan nearFace;
};



// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    char mode;    // 'b' for "bind to a player"  and  'f' for "free"   (by default f)
    Character *characterBinded;
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    Frustum frustum;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    float scroll;
    float nearr, farr, angle;

    // constructor with vectors
    Camera(Character *character, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        mode = 'f';
        characterBinded = character;
        scroll = 0;
        nearr = 0.1f;
        farr = 1000.f;
        angle = 45.f;
        updateCameraVectors();
        createFrustumFromCamera();
    }



    inline void BindOrUnbindToCharacter()
    {
        if(InfoInputs::Ppressed == true && InfoInputs::PlastPressed == false)
        {
            if(mode == 'f')
            {
                mode = 'b';
                Pitch = -17;
            }
            else if(mode == 'b')
                mode = 'f';
        }
    }


    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    inline void ProcessKeyboard()
    {
        if(mode == 'f')
        {
            float velocity = MovementSpeed * InfoGlobal::coefFrame;
            if (InfoInputs::Wpressed == true)
                Position += Front * velocity;
            if (InfoInputs::Spressed == true)
                Position -= Front * velocity;
            if (InfoInputs::Apressed == true)
                Position -= Right * velocity;
            if (InfoInputs::Dpressed == true)
                Position += Right * velocity;
        }
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    inline void ProcessMouseMovement()
    {
        if(InfoInputs::mouseMoved == true)
        {
            if(mode == 'f')
            {
                float xoffset = InfoInputs::Xmouse - InfoInputs::XlastMouse;
                float yoffset = InfoInputs::Ymouse - InfoInputs::YlastMouse;

                xoffset *= MouseSensitivity;
                yoffset *= MouseSensitivity;

                Yaw   -= xoffset;
                Pitch += yoffset;

                if (Pitch > 89.0f)
                    Pitch = 89.0f;
                if (Pitch < -89.0f)
                    Pitch = -89.0f;

                // update Front, Right and Up Vectors using the updated Euler angles
                updateCameraVectors();
            }
        }
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    inline void ProcessMouseScroll()
    {
        if(InfoInputs::mouseScrolled == true)
        {
            if(mode == 'b')
            {
                scroll = scroll + InfoInputs::mouseScrollOffset;
            }
        }
        Position += (Front * scroll);
    }

    inline void ViewProjectionUpdate(int UBO1)
    {
        glm::mat4 view1 = glm::mat4(1.0f);
		view1 = glm::lookAt(Position, Position + Front, Up);
		glm::mat4 projection1 = glm::mat4(1.0f);
		projection1 = glm::perspective(glm::radians(angle), (float)InfoGlobal::SCR_WIDTH / InfoGlobal::SCR_HEIGHT, nearr, farr);
		glBindBuffer(GL_UNIFORM_BUFFER, UBO1);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view1));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection1));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    inline void createFrustumFromCamera()
    {
        float halfVSide = farr* tanf(glm::radians(angle) * .5f);
        float halfHSide = halfVSide * ((float)InfoGlobal::SCR_WIDTH / InfoGlobal::SCR_HEIGHT);
        glm::vec3 frontMultFar = farr * Front;

        frustum.nearFace.normal = Front;
        frustum.nearFace.distance = glm::dot(frustum.nearFace.normal, Position + nearr * Front);
        frustum.farFace.normal = -Front;
        frustum.farFace.distance = glm::dot(frustum.farFace.normal, Position + frontMultFar);
        frustum.rightFace.normal = glm::normalize(glm::cross(Up,frontMultFar + Right * halfHSide));
        frustum.rightFace.distance = glm::dot(frustum.rightFace.normal, Position);
        frustum.leftFace.normal = glm::normalize(glm::cross(frontMultFar - Right * halfHSide, Up));
        frustum.leftFace.distance = glm::dot(frustum.leftFace.normal, Position);
        frustum.topFace.normal = glm::normalize(glm::cross(Right, frontMultFar - Up * halfVSide));
        frustum.topFace.distance = glm::dot(frustum.topFace.normal, Position);
        frustum.bottomFace.normal = glm::normalize(glm::cross(frontMultFar + Up * halfVSide, Right));
        frustum.bottomFace.distance = glm::dot(frustum.bottomFace.normal, Position);
    }

    void Update(int UBO1)
    {
        BindOrUnbindToCharacter();
        ProcessKeyboard();
        ProcessMouseMovement();
        
        if(mode == 'b')
        {
            Position = characterBinded->lastDataFrames[characterBinded->currentPosLastDataFrames].position;
            Position -= characterBinded->lastDataFrames[characterBinded->currentPosLastDataFrames].frontDirection * 18.f;
            Position.y += 13;
            Yaw = characterBinded->lastDataFrames[characterBinded->currentPosLastDataFrames].yaw;
        }
        
        updateCameraVectors();
        ProcessMouseScroll();

        ViewProjectionUpdate(UBO1);
        createFrustumFromCamera();
    }



    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }



private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.z = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.x = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};






#endif