#include <iostream>
#include <cmath>
#include <cstdlib>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "modelAnim.h"

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void animacion();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(0.0f, 2.0f, -30.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
float range = 0.0f;
float rot = 0.0f;
float DoorRot = 180.0f;
float recRot = 0.0f;
float movCamera = 0.0f;
float ballRotX,ballRotY,ballRotZ = 0.0f;
float amX,amY,amZ = 0.0f;
bool isDoorOpen = true;
bool doorMoving = false;
bool isRecOpen = false;
bool recMoving = false;
bool drawLightCubes = false;
bool ballMoving = true;

// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
//glm::vec3 PosIni(0.0f, 2.0f, -30.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);

//Keyframe positions
glm::vec3 ballPosIni(0.4, 11.725, -7.5);
glm::vec3 escobPosIni(0, 0, 0);
glm::vec3 pinPosIni(0, 0, 0);

glm::vec3 Pos(0, 0, 0);
glm::vec3 pinOffset(1, 12, -34.5);


bool active;


// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

// Keyframes
float ballPosX = ballPosIni.x, ballPosY = ballPosIni.y, ballPosZ = ballPosIni.z, 
escobPosX = escobPosIni.x, escobPosY = escobPosIni.y, escobPosZ = escobPosIni.z,
pinPosX = pinPosIni.x, pinPosY = pinPosIni.y, pinPosZ = pinPosIni.z;
float pinRot = 0.0f;

#define MAX_FRAMES 9
int i_max_steps = 100;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	//Bola bolos
	float ballPosX;		//Variable para PosicionX
	float ballPosY;		//Variable para PosicionY
	float ballPosZ;		//Variable para PosicionZ
	float ballIncX;		//Variable para IncrementoX
	float ballIncY;		//Variable para IncrementoY
	float ballIncZ;		//Variable para IncrementoZ
	bool ballMoving;
	//Escobilla
	float escobPosX;		//Variable para PosicionX
	float escobPosY;		//Variable para PosicionY
	float escobPosZ;		//Variable para PosicionZ
	float escobIncX;		//Variable para IncrementoX
	float escobIncY;		//Variable para IncrementoY
	float escobIncZ;		//Variable para IncrementoZ

	float pinPosX;		//Variable para PosicionX
	float pinPosY;		//Variable para PosicionY
	float pinPosZ;		//Variable para PosicionZ
	float pinIncX;		//Variable para IncrementoX
	float pinIncY;		//Variable para IncrementoY
	float pinIncZ;		//Variable para IncrementoZ

	float pinRot;
	float rotInc;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir datos
bool play = false;
int playIndex = 0;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	//Luz PB
	glm::vec3(8,3,-120),
	//Luz1F
	//glm::vec3(-15,23,-95),
	glm::vec3(0,18,-90),
	//Luz Luminaria
	glm::vec3(6.9,11,-61),
	//Luz bolos
	glm::vec3(0.8,14.6,-135)
};

glm::vec3 LightP1;




void saveFrame(void)
{

	printf("Frame Saved %f, %f, %f\n", Pos.x, Pos.y, Pos.z);

	KeyFrame[FrameIndex].ballPosX = ballPosX;
	KeyFrame[FrameIndex].ballPosY = ballPosY;
	KeyFrame[FrameIndex].ballPosZ = ballPosZ;

	KeyFrame[FrameIndex].escobPosX = escobPosX;
	KeyFrame[FrameIndex].escobPosY = escobPosY;
	KeyFrame[FrameIndex].escobPosZ = escobPosZ;

	KeyFrame[FrameIndex].pinPosX = pinPosX;
	KeyFrame[FrameIndex].pinPosY = pinPosY;
	KeyFrame[FrameIndex].pinPosZ = pinPosZ;

	KeyFrame[FrameIndex].pinRot = pinRot;


	FrameIndex++;
}

void resetElements(void)
{
	ballPosX = KeyFrame[0].ballPosX;
	ballPosY = KeyFrame[0].ballPosY;
	ballPosZ = KeyFrame[0].ballPosZ;

	escobPosX = KeyFrame[0].escobPosX;
	escobPosY = KeyFrame[0].escobPosY;
	escobPosZ = KeyFrame[0].escobPosZ;

	pinPosX = KeyFrame[0].pinPosX;
	pinPosY = KeyFrame[0].pinPosY;
	pinPosZ = KeyFrame[0].pinPosZ;

	pinRot = KeyFrame[0].pinRot;
}

void interpolation(void)
{
	KeyFrame[playIndex].ballIncX = (KeyFrame[playIndex + 1].ballPosX - KeyFrame[playIndex].ballPosX) / i_max_steps;
	KeyFrame[playIndex].ballIncY = (KeyFrame[playIndex + 1].ballPosY - KeyFrame[playIndex].ballPosY) / i_max_steps;
	KeyFrame[playIndex].ballIncZ = (KeyFrame[playIndex + 1].ballPosZ - KeyFrame[playIndex].ballPosZ) / i_max_steps;
	

	KeyFrame[playIndex].escobIncX = (KeyFrame[playIndex + 1].escobPosX - KeyFrame[playIndex].escobPosX) / i_max_steps;
	KeyFrame[playIndex].escobIncY = (KeyFrame[playIndex + 1].escobPosY - KeyFrame[playIndex].escobPosY) / i_max_steps;
	KeyFrame[playIndex].escobIncZ = (KeyFrame[playIndex + 1].escobPosZ - KeyFrame[playIndex].escobPosZ) / i_max_steps;

	KeyFrame[playIndex].pinIncX = (KeyFrame[playIndex + 1].pinPosX - KeyFrame[playIndex].pinPosX) / i_max_steps;
	KeyFrame[playIndex].pinIncY = (KeyFrame[playIndex + 1].pinPosY - KeyFrame[playIndex].pinPosY) / i_max_steps;
	KeyFrame[playIndex].pinIncZ = (KeyFrame[playIndex + 1].pinPosZ - KeyFrame[playIndex].pinPosZ) / i_max_steps;
	

	KeyFrame[playIndex].rotInc = (KeyFrame[playIndex + 1].pinRot - KeyFrame[playIndex].pinRot) / 20;

	//KeyFrame[playIndex].rotInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;

}

void setFrameValues(float ballX, float ballY, float ballZ,float escobX, float escobY, float escobZ, float pinX, float pinY, float pinZ, float rot)
{
	ballPosX = ballX;
	ballPosY = ballY;
	ballPosZ = ballZ;

	escobPosX = escobX;
	escobPosY = escobY;
	escobPosZ = escobZ;
	
	pinPosX = pinX;
	pinPosY = pinY;
	pinPosZ = pinZ;

	pinRot = rot;
}


int main()
{
	// Init GLFW
	glfwInit();




	// Set all the required options for GLFW
	/*(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);*/

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "316118299_ProyectoFinal_GPO9", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	printf("%f", glfwGetTime());

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader lightingShader("Shaders/lighting.vs", "Shaders/lighting.frag");
	Shader lampShader("Shaders/lamp.vs", "Shaders/lamp.frag");
	Shader SkyBoxshader("Shaders/SkyBox.vs", "Shaders/SkyBox.frag");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.frag");

	Model Fachada((char*)"Models/fachada/fachada2.obj");
	Model pins((char*)"Models/pinFormation/pinFormation.obj");
	Model Balls((char*)"Models/Balls/balls.obj");
	Model Banqueta((char*)"Models/fachada/banqueta.obj");
	Model Edificios((char*)"Models/fachada/Extras/buildings.obj");
	Model Luminaria((char*)"Models/fachada/Extras/luminaria.obj");
	Model Concealer((char*)"Models/fachada/Extras/concealer.obj");
	Model Escobilla((char*)"Models/Moviles/escobilla.obj");


	Model BowlingBall((char*)"Models/BowlingBall/ball.obj");

	Model Colector((char*)"Models/Colector/Colector.obj");
	Model Panel((char*)"Models/ControlPanel/control.obj");
	Model Mesa((char*)"Models/Mesa/mesa.obj");
	Model pin((char*)"Models/Pin/pin.obj");
	Model pin2((char*)"Models/Pin/pinT.obj");
	Model ScoreBoard((char*)"Models/scoreboard/scoreboard.obj");
	Model Chair((char*)"Models/Silla/silla.obj");
	Model Sillas((char*)"Models/Sillas/sillasMult.obj");
	Model SillasAlt((char*)"Models/SillasAlt/sillasMult.obj");
	Model Boleras((char*)"Models/fachada/boleras.obj");
	Model Recepcion((char*)"Models/Moviles/puertaR.obj");
	Model PC((char*)"Models/Computadoras/pc.obj");



	//Objeto traslucido
	Model FachadaCristales((char*)"Models/fachada/fachadaCristales.obj");
	Model PuertaIzq((char*)"Models/Puertas/puertaIzq2.obj");
	Model PuertaDer((char*)"Models/Puertas/puertaDer2.obj");

	// Build and compile our shader program

	//Inicializacion de KeyFrames

	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].ballPosX = 0;
		KeyFrame[i].ballPosY = 0;
		KeyFrame[i].ballPosZ = 0;
		KeyFrame[i].ballIncX = 0;
		KeyFrame[i].ballIncY = 0;
		KeyFrame[i].ballIncZ = 0;

		KeyFrame[i].escobPosX = 0;
		KeyFrame[i].escobPosY = 0;
		KeyFrame[i].escobPosZ = 0;
		KeyFrame[i].escobIncX = 0;
		KeyFrame[i].escobIncY = 0;
		KeyFrame[i].escobIncZ = 0;

		KeyFrame[i].pinPosX = 0;
		KeyFrame[i].pinPosY = 0;
		KeyFrame[i].pinPosZ = 0;
		KeyFrame[i].pinIncX = 0;
		KeyFrame[i].pinIncY = 0;
		KeyFrame[i].pinIncZ = 0;

		KeyFrame[i].pinRot = 0;



	}



	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] =
	{
		// Positions            // Normals              // Texture Coords
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
	};


	GLfloat skyboxVertices[] = {
		// Positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};


	GLuint indices[] =
	{  // Note that we start from 0!
		0,1,2,3,
		4,5,6,7,
		8,9,10,11,
		12,13,14,15,
		16,17,18,19,
		20,21,22,23,
		24,25,26,27,
		28,29,30,31,
		32,33,34,35
	};

	// Positions all containers
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Texture Coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	// Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Set the vertex attributes (only position data for the lamp))
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);


	//SkyBox
	GLuint skyboxVBO, skyboxVAO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// Load textures
	vector<const GLchar*> faces;
	faces.push_back("SkyBox/right.tga");
	faces.push_back("SkyBox/left.tga");
	faces.push_back("SkyBox/top.tga");
	faces.push_back("SkyBox/bottom.tga");
	faces.push_back("SkyBox/back.tga");
	faces.push_back("SkyBox/front.tga");

	GLuint cubemapTexture = TextureLoading::LoadCubemap(faces);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);

	//Create Keyframe Animation
	//F1
	setFrameValues(0.75,11.725,-13,0,0,0,0,10,0,0);
	saveFrame();
	//F2
	setFrameValues(0.4, 11.725, -7.5,0,0,0,0,0,0,0);
	saveFrame();
	//F3
	setFrameValues(0.4, 15, -7.5, 0, 0, 0, 0, 0, 0, 0);
	saveFrame();
	//F4
	setFrameValues(0.75, 11.725, -19, 0, 0, 0, 0, 0, 0, 0);
	saveFrame();
	//F4
	setFrameValues(0.75, 11.725, -34, 0, 0, 0, 0, 0, 0, 0);
	saveFrame();
	//F5
	setFrameValues(0.75, 11.725, -40, 0, -4, 0, 0, -0.5, 0, 90);
	saveFrame();
	//F7
	setFrameValues(0.75, 11.725, -40, 0, -4, -5, 0, 0, -10, 90);
	saveFrame();
	//F8
	setFrameValues(0.75, 11.725, -40, 0, 0, -5, 0, 0, -10, 0);
	saveFrame();
	//F9
	setFrameValues(0.75, 11.725, -40, 0, 0, 0, 0, 10, 0, 0);
	saveFrame();


	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();
		animacion();


		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw skybox as last
		glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
		SkyBoxshader.Use();
		glm::mat4 view;
		view = camera.GetViewMatrix();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // Set depth function back to default


		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);
		// == ==========================
		// Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		// the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		// by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		// by using 'Uniform buffer objects', but that is something we discuss in the 'Advanced GLSL' tutorial.
		// == ==========================
		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.1f, 0.1f, 0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.1f, 0.1f, 0.2f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.3f, 0.3f, 0.3f);


		// Point light 1
		//PB
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), 1, 1, 1);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 0.5, 0.5, 0.5);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.022f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.0019f);



		// Point light 2
		//1F
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), 0.5f, 0.5f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 0.1f, 0.1f, 0.2f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.07f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.017f);

		// Point light 3
		//Fachada
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), 1.0f, 1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 1.0f, 1.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.0075f);

		// Point light 4
		//Bolera
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), LightP1.x, LightP1.y, LightP1.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), LightP1.x, LightP1.y, LightP1.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.032f);

		// SpotLight
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.032f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));

		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

		// Create camera transformations

		view = camera.GetViewMatrix();


		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Bind diffuse map
		//glBindTexture(GL_TEXTURE_2D, texture1);*/

		// Bind specular map
		/*glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);*/


		glBindVertexArray(VAO);
		glm::mat4 tmp = glm::mat4(1.0f); //Temp
		glm::mat4 modelPos = glm::mat4(1.0f); //Temp



		//Carga de modelo 
		//Fachada
		//Modelos estaticos (principales)
		view = camera.GetViewMatrix();
		glm::mat4 model(1);
		model = glm::mat4(1);
		modelPos = model = glm::translate(model, glm::vec3(0.0f, 0.0f, -100.0f));
		//model = glm::rotate(model, glm::radians(rot), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::translate(model, glm::vec3(0.0f, 2.5f, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Fachada.Draw(lightingShader);
		//Banqueta
		view = camera.GetViewMatrix();
		//model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Banqueta.Draw(lightingShader);
		view = camera.GetViewMatrix();
		//model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Concealer.Draw(lightingShader);
		//Edificios Adicionales (Ambiente)
		view = camera.GetViewMatrix();
		//model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Edificios.Draw(lightingShader);
		//Luminaria
		view = camera.GetViewMatrix();
		//model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Luminaria.Draw(lightingShader);
		//Boleras
		view = camera.GetViewMatrix();
		model = modelPos;
		model = glm::translate(model, glm::vec3(0.3f, 0.0f, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Boleras.Draw(lightingShader);

		//Pinos estaticos (no animados)
		view = camera.GetViewMatrix();
		model = glm::translate(model, glm::vec3(-0.5f, 0.0f, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		pins.Draw(lightingShader);
		view = camera.GetViewMatrix();
		model = glm::translate(model, glm::vec3(31.0f, 0.0f, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		pins.Draw(lightingShader);

		//Bolas de bolos estáticas
		view = camera.GetViewMatrix();
		model = modelPos;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Balls.Draw(lightingShader);

		//Colectores
		view = camera.GetViewMatrix();
		model = modelPos;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Colector.Draw(lightingShader);

		view = camera.GetViewMatrix();
		model = glm::translate(model, glm::vec3(16.0f, 0.0f, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Colector.Draw(lightingShader);

		view = camera.GetViewMatrix();
		model = glm::translate(model, glm::vec3(14.5f, 0.0f, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Colector.Draw(lightingShader);

		//Paneles de control
		view = camera.GetViewMatrix();
		model = modelPos;
		//model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Panel.Draw(lightingShader);

		view = camera.GetViewMatrix();
		model = glm::translate(model, glm::vec3(16.0f, 0.0f, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Panel.Draw(lightingShader);

		view = camera.GetViewMatrix();
		model = glm::translate(model, glm::vec3(14.5f, 0.0f, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Panel.Draw(lightingShader);

		//Mesas
		view = camera.GetViewMatrix();
		model = modelPos;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Mesa.Draw(lightingShader);

		view = camera.GetViewMatrix();
		model = glm::translate(model, glm::vec3(16.0f, 0.0f, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Mesa.Draw(lightingShader);

		view = camera.GetViewMatrix();
		model = glm::translate(model, glm::vec3(14.5f, 0.0f, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Mesa.Draw(lightingShader);

		//Pantallas de score
		view = camera.GetViewMatrix();
		model = modelPos;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		ScoreBoard.Draw(lightingShader);

		view = camera.GetViewMatrix();
		model = glm::translate(model, glm::vec3(14.5f, 0.0f, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		ScoreBoard.Draw(lightingShader);

		view = camera.GetViewMatrix();
		model = glm::translate(model, glm::vec3(14.5f, 0.0f, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		ScoreBoard.Draw(lightingShader);

		//Sillas
		//Primera mesa
		view = camera.GetViewMatrix();
		model = modelPos;
		model = glm::translate(model, glm::vec3(-2.5f, 0.0f, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Chair.Draw(lightingShader);
		view = camera.GetViewMatrix();
		model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Chair.Draw(lightingShader);

		//Segunda mesa
		view = camera.GetViewMatrix();
		model = glm::translate(model, glm::vec3(13.0f, 0.0f, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Chair.Draw(lightingShader);
		view = camera.GetViewMatrix();
		model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Chair.Draw(lightingShader);

		//Tercera mesa
		view = camera.GetViewMatrix();
		model = glm::translate(model, glm::vec3(12.0f, 0.0f, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Chair.Draw(lightingShader);
		view = camera.GetViewMatrix();
		model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Chair.Draw(lightingShader);

		//Sillas empotradas
		view = camera.GetViewMatrix();
		model = modelPos;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Sillas.Draw(lightingShader);

		view = camera.GetViewMatrix();
		model = glm::translate(model, glm::vec3(16.0f, 0.0f, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Sillas.Draw(lightingShader);

		view = camera.GetViewMatrix();
		model = glm::translate(model, glm::vec3(15.5f, 0.0f, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Sillas.Draw(lightingShader);

		//Orientacion inversa
		view = camera.GetViewMatrix();
		model = modelPos;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		SillasAlt.Draw(lightingShader);

		view = camera.GetViewMatrix();
		model = glm::translate(model, glm::vec3(15.0f, 0.0f, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		SillasAlt.Draw(lightingShader);

		view = camera.GetViewMatrix();
		model = glm::translate(model, glm::vec3(15.0f, 0.0f, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		SillasAlt.Draw(lightingShader);

		//Computadoras
		view = camera.GetViewMatrix();
		model = modelPos;
		//model = glm::translate(model, glm::vec3(15.8f, -3.9f, -15.5f));
		//model = glm::rotate(model, glm::radians(recRot), glm::vec3(0.0f, 0.0f, 1));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		PC.Draw(lightingShader);

		//----------------------------------------------------------------------------------------
		//Objetos moviles
		//Puerta recepcion - Animacion sencilla 2
		view = camera.GetViewMatrix();
		model = modelPos;
		model = glm::translate(model, glm::vec3(15.8f, -3.9f, -15.5f));
		model = glm::rotate(model, glm::radians(recRot), glm::vec3(0.0f, 0.0f, 1));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Recepcion.Draw(lightingShader);

		//Bola Bolos
		view = camera.GetViewMatrix();
		model = modelPos;
		model = glm::translate(model, glm::vec3(ballPosX,ballPosY,ballPosZ));
		model = glm::rotate(model,glm::radians(ballRotX), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(ballRotY), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(ballRotZ), glm::vec3(0, 0, 1));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		BowlingBall.Draw(lightingShader);

		//Escobilla
		view = camera.GetViewMatrix();
		model = modelPos;
		model = glm::translate(model, glm::vec3(escobPosX, escobPosY, escobPosZ));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		Escobilla.Draw(lightingShader);

		//Bolos
		//Primera hilera
		view = camera.GetViewMatrix();
		model = modelPos;
		model = glm::translate(model, pinOffset);
		model = glm::translate(model, glm::vec3(pinPosX, pinPosY, pinPosZ));
		model = glm::rotate(model, glm::radians(-pinRot), glm::vec3(0, 0, 1));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		pin2.Draw(lightingShader);

		//Segunda Hilera
		view = camera.GetViewMatrix();
		model = modelPos;
		model = glm::translate(model, pinOffset);
		model = glm::translate(model, glm::vec3(0.35f, 0.0f, -0.5f));
		model = glm::translate(model, glm::vec3(pinPosX, pinPosY, pinPosZ));
		model = glm::rotate(model, glm::radians(pinRot), glm::vec3(1, 0, 0));
		//model = glm::rotate(model, glm::radians(pinRot), glm::vec3(0, 0, 1));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		pin2.Draw(lightingShader);
		view = camera.GetViewMatrix();
		model = modelPos;
		model = glm::translate(model, pinOffset);
		model = glm::translate(model, glm::vec3(-0.35f, 0.0f, -0.5f));
		model = glm::translate(model, glm::vec3(pinPosX, pinPosY, pinPosZ));
		model = glm::rotate(model, glm::radians(pinRot), glm::vec3(1, 0, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		pin2.Draw(lightingShader);

		//Tercera hilera
		view = camera.GetViewMatrix();
		model = modelPos;
		model = glm::translate(model, pinOffset);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
		model = glm::translate(model, glm::vec3(pinPosX, pinPosY, pinPosZ));
		//model = glm::rotate(model, glm::radians(pinRot), glm::vec3(0, 0, 1));
		model = glm::rotate(model, glm::radians(-pinRot), glm::vec3(1, 0, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		pin2.Draw(lightingShader);
		view = camera.GetViewMatrix();
		model = modelPos;
		model = glm::translate(model, pinOffset);
		model = glm::translate(model, glm::vec3(0.6f, 0.0f, -1.0f));
		model = glm::translate(model, glm::vec3(pinPosX, pinPosY, pinPosZ));
		model = glm::rotate(model, glm::radians(-pinRot), glm::vec3(0, 0, 1));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		pin2.Draw(lightingShader);
		view = camera.GetViewMatrix();
		model = modelPos;
		model = glm::translate(model, pinOffset);
		model = glm::translate(model, glm::vec3(-0.6f, 0.0f, -1.0f));
		model = glm::translate(model, glm::vec3(pinPosX, pinPosY, pinPosZ));
		model = glm::rotate(model, glm::radians(-pinRot), glm::vec3(0, 0, 1));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		pin2.Draw(lightingShader);

		//Cuarta hilera
		view = camera.GetViewMatrix();
		model = modelPos;
		model = glm::translate(model, pinOffset);
		model = glm::translate(model, glm::vec3(0.85f, 0.0f, -1.5f));
		model = glm::translate(model, glm::vec3(pinPosX, pinPosY, pinPosZ));
		model = glm::rotate(model, glm::radians(pinRot), glm::vec3(1, 0, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		pin2.Draw(lightingShader);
		view = camera.GetViewMatrix();
		model = modelPos;
		model = glm::translate(model, pinOffset);
		model = glm::translate(model, glm::vec3(-0.85f, 0.0f, -1.5f));
		model = glm::translate(model, glm::vec3(pinPosX, pinPosY, pinPosZ));
		model = glm::rotate(model, glm::radians(pinRot), glm::vec3(1, 0, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		pin2.Draw(lightingShader);
		view = camera.GetViewMatrix();
		model = modelPos;
		model = glm::translate(model, pinOffset);
		model = glm::translate(model, glm::vec3(-0.35f, 0.0f, -1.5f));
		model = glm::translate(model, glm::vec3(pinPosX, pinPosY, pinPosZ));
		model = glm::rotate(model, glm::radians(-pinRot), glm::vec3(1, 0, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		pin2.Draw(lightingShader);
		view = camera.GetViewMatrix();
		model = modelPos;
		model = glm::translate(model, pinOffset);
		model = glm::translate(model, glm::vec3(0.35f, 0.0f, -1.5f));
		model = glm::translate(model, glm::vec3(pinPosX, pinPosY, pinPosZ));
		model = glm::rotate(model, glm::radians(-pinRot), glm::vec3(0, 0, 1));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		pin2.Draw(lightingShader);


		//------------------------------------------------------------------------------
		//Modelos traslucidos
		//Cristales fachada
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Puertas
		tmp = model = modelPos;
		//model = glm::scale(model, glm::vec3(1.0f));
		model = glm::translate(model, glm::vec3(-12.5f, -1.2f, -30.7f));
		model = glm::rotate(model, glm::radians(-DoorRot), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 1.0);
		PuertaIzq.Draw(lightingShader);
		model = tmp;
		//model = glm::scale(model, glm::vec3(1.0f));
		model = glm::translate(model, glm::vec3(-12.5f, -1.2f, -20.7f));
		model = glm::rotate(model, glm::radians(DoorRot), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 1.0);
		PuertaDer.Draw(lightingShader);

		model = modelPos;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 1.0);
		FachadaCristales.Draw(lightingShader);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glBindVertexArray(0);


		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1);
		model = glm::translate(model, lightPos);
		//model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		// Draw the light object (using light's vertex attributes)
		glBindVertexArray(lightVAO);
		for (GLuint i = 0; i < 4; i++)
		{
			model = glm::mat4(1);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			if (drawLightCubes || (i==2))
			{
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}
		glBindVertexArray(0);


		// Swap the screen buffers
		glfwSwapBuffers(window);
	}




	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();




	return 0;
}


void animacion()
{

	//Movimiento del personaje

	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
								  //Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			ballPosX += KeyFrame[playIndex].ballIncX;
			ballPosY += KeyFrame[playIndex].ballIncY;
			ballPosZ += KeyFrame[playIndex].ballIncZ;

			escobPosX += KeyFrame[playIndex].escobIncX;
			escobPosY += KeyFrame[playIndex].escobIncY;
			escobPosZ += KeyFrame[playIndex].escobIncZ;

			pinPosX += KeyFrame[playIndex].pinIncX;
			pinPosY += KeyFrame[playIndex].pinIncY;
			pinPosZ += KeyFrame[playIndex].pinIncZ;

			pinRot += KeyFrame[playIndex].rotInc;

			i_curr_steps++;
		}

	}
}


// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (keys[GLFW_KEY_L])
	{
		if (play == false && (FrameIndex > 1))
		{

			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
		}

	}

	//if (keys[GLFW_KEY_K])
	//{
	//	if (FrameIndex < MAX_FRAMES)
	//	{
	//		saveFrame();
	//	}

	//	rot = -25.0f;//Variable que maneja el giro de la camara

	//}

	if (keys[GLFW_KEY_P])
	{
		doorMoving = true;
		if (isDoorOpen)
		{
			isDoorOpen = false;
			//DoorRot = 180.0f;
		}
		else
		{
			isDoorOpen = true;
			//DoorRot = 90.0f;
		}
	}

	if (keys[GLFW_KEY_R])
	{
		recMoving = true;
		if (isRecOpen)
		{
			isRecOpen = false;
			//DoorRot = 180.0f;
		}
		else
		{
			isRecOpen = true;
			//DoorRot = 90.0f;
		}
	}

	if (keys[GLFW_KEY_B])
	{
		amX = rand() % 20;
		amY = rand() % 20;
		amZ = rand() % 20;
		if ((rand() % 4) >= 2)
		{
			amX = -amX;
		}
		if ((rand() % 4) >= 2)
		{
			amY = -amY;
		}
		if ((rand() % 4) >= 2)
		{
			amZ = -amZ;
		}
	}


	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	if (keys[GLFW_KEY_SPACE])
	{
		active = !active;
		if (active)
			LightP1 = glm::vec3(1.0f, 1.0f, 1.0f);
		else
			LightP1 = glm::vec3(0.0f, 0.0f, 0.0f);
	}
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{

	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

// Moves/alters the camera positions based on user input
void DoMovement()
{
	//Puertas
	//DoorRot = 90 - abierto
	//			180 - cerrado
	if (doorMoving)
	{
		if (isDoorOpen)
		{
			if (DoorRot >= 180.0f)
			{
				DoorRot = 180.0f;
				doorMoving = false;
			}
			else
			{
				DoorRot = DoorRot + 2.0f;
			}
		}
		else
		{
			if (DoorRot <= 30.0f)
			{
				DoorRot = 30.0f;
				doorMoving = false;
			}
			else
			{
				DoorRot = DoorRot - 2.0f;
			}
		}
	}

	if (recMoving)
	{
		if (isRecOpen)
		{
			if (recRot >= 120.0f)
			{
				recRot = 120.0f;
				recMoving = false;
			}
			else
			{
				recRot = recRot + 2.0f;
			}
		}
		else
		{
			if (recRot <= 0.0f)
			{
				recRot = 0.0f;
				recMoving = false;
			}
			else
			{
				recRot = recRot - 2.0f;
			}
		}
	}

	if (keys[GLFW_KEY_1])
	{

		movCamera = 0.01f;//Manda una velocidad de 0.01 a la camara automatica

	}

	//Bowling Ball
	if (ballMoving)
	{
		ballRotX += amX;
		ballRotY += amY;
		ballRotZ += amZ;
	}


	//Mov Personaje
	/*if (keys[GLFW_KEY_H])
	{
		ballPosZ += 0.5;
		Pos.z += 0.5;
	}

	if (keys[GLFW_KEY_Y])
	{
		ballPosZ -= 0.5;
		Pos.z -= 0.5;
	}

	if (keys[GLFW_KEY_U])
	{
		ballPosX -= 0.5;
		Pos.x -= 0.5;
	}

	if (keys[GLFW_KEY_J])
	{
		ballPosX += 0.5;
		Pos.x += 0.5;
	}

	if (keys[GLFW_KEY_O])
	{
		ballPosY += 0.5;
		Pos.y += 0.5;
	}

	if (keys[GLFW_KEY_P])
	{
		ballPosY -= 0.5;
		Pos.y -= 0.5;
	}*/




	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);

	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);


	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);


	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}






}