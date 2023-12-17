#include <math.h>
#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <ctime>
#include <iostream> 
#include <Windows.h>

int WIDTH = 1280;
int HEIGHT = 720;

GLuint tex;
GLuint starsTex;
GLuint coinTex;
char title[] = "3D Model Loader Sample";

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 200;

class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//================================================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
};

//Vector Eye(20, 5, 20);
Vector Eye(3, 4, 30);
Vector At(3, 3, -30);
Vector Up(0, 1, 0);

int cameraZoom = 0;

// Model Variables
Model_3DS model_house;
Model_3DS model_tree;
Model_3DS model_mike;
Model_3DS model_cactus;
Model_3DS model_vendingM;


//house models
Model_3DS model_door;
Model_3DS model_sofa;
Model_3DS model_teddy;
Model_3DS model_car;
Model_3DS model_table;


// Textures
GLTexture tex_ground;
GLTexture tex_stars;
GLTexture tex_ground2;
GLTexture tex_wall;
GLTexture tex_ceiling;


// cameras
enum CameraMode {
	FIRST_PERSON,
	THIRD_PERSON,
	STILL_CAM
};

CameraMode currentCameraMode = THIRD_PERSON;


// Extra variables
float camX = 0.0f;
float camY = 20.0f;
float camZ = 40.0f;

float mikeX = 0.0f; // Initial X position
float mikeZ = 19.0f; // Initial Z position
float initialMikeY = 0.0f; // Initial Y position
float mikeY = initialMikeY;
float mikeSpeed = 1.0f; // Movement speed

bool isJumping = false;
float jumpHeight = 2.0f;
float mikeRotation = 130;
float camRotation = 0;

GLfloat lightIntensityDay[] = { 0.7, 0.7, 0.7, 1.0f }; // Initial daytime light intensity
GLfloat lightIntensityNight[] = { 0.2, 0.2, 0.2, 1.0f }; // Initial nighttime light intensity
bool isDaytime = true;
time_t lastTime = time(nullptr);
const int transitionDuration = 15;
const int transitionDuration2 = 30;

int score = 0;
bool CoinTaken1 = false;
bool CoinTaken2 = false;
bool CoinTaken3 = false;
bool CoinTaken4 = false;
bool CoinTaken5 = false;
bool CoinTaken6 = false;
bool CoinTaken7 = false;
bool CoinTaken8 = false;
bool checkCollisions = true;

float coinFloatOffset = 0.0f;
float coinFloatSpeed = 0.005f;

bool cactusHit1 = false;
bool cactusHit2 = false;
bool cactusHit3 = false;
bool cactusHit4 = false;


bool vendingMachineHit = false;
bool displayWinScreen = false;
bool collidedVM = false;
bool collidedDoor = false;
bool collidedSofa = false;
bool teddy1Taken = false;
bool teddy2Taken = false;
bool carTaken = false;

int timer = 120;



//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// Define Light source 0 Specular light
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

//=======================================================================
// Render Ground Function
//=======================================================================
void RenderGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}
void RenderGround2()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground2.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

//=======================================================================
// Other fns
//=======================================================================

//------------------------------------------------------------------
//-------------------check house collison---------------------------

bool houseCollide = true;

void checkHouseCollision() {
	// Define the bounding box of the house
	float houseMinX = -1.0f;
	float houseMaxX = 1.0f;
	float houseMinZ = -1.0f;
	float houseMaxZ = 1.0f;

	// Define the bounding box of the Mike model
	float mikeMinX = mikeX - 3.0f;  // Assuming the width of Mike is 2 units
	float mikeMaxX = mikeX + 3.0f;
	float mikeMinZ = mikeZ - 3.0f;  // Assuming the depth of Mike is 2 units
	float mikeMaxZ = mikeZ + 3.0f;

	// Check for collision in the X and Z dimensions
	if ((mikeMaxX >= houseMinX && mikeMinX <= houseMaxX) &&
		(mikeMaxZ >= houseMinZ && mikeMinZ <= houseMaxZ)) {
		houseCollide = true;
	}
	else {
		houseCollide = false;
	}
}

void performJump() {
	if (isJumping) {
		mikeY += 0.05f; // Increment the Y position to simulate the jump

		if (mikeY >= initialMikeY + jumpHeight) {
			isJumping = false; // Finish the jump when reaching the peak
			mikeY = initialMikeY; // Reset the Y position to the initial value
		}

		glutPostRedisplay();
	}
}

void updateScore(int newScore) {
	score = newScore;
	glutPostRedisplay();
}
/*
void switchCamera() {
	switch (currentCameraMode) {
	case FIRST_PERSON:
		currentCameraMode = THIRD_PERSON;
		break;
	case THIRD_PERSON:
		currentCameraMode = STILL_CAM;
		break;
	case STILL_CAM:
		currentCameraMode = FIRST_PERSON;
		break;
	default:
		break;
	}
}
*/
void setCamera() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (currentCameraMode == FIRST_PERSON) {
		// First Person POV using mike's position
		float lookAtOffset = 10.0f;
		float lookAtX = mikeX + lookAtOffset * sin(camRotation * 3.14159 / 180.0);
		float lookAtZ = mikeZ - lookAtOffset * cos(camRotation * 3.14159 / 180.0);
		gluLookAt(mikeX, mikeY + 3.0f, mikeZ, lookAtX, mikeY + 1.0f, lookAtZ, 0.0, 1.0, 0.0);
	}
	else if (currentCameraMode == THIRD_PERSON) {
		// Third Person POV 
		float distance = 3.0f;
		float thirdPersonCamX = mikeX - distance * sin(camRotation * 3.14159 / 180.0);
		float thirdPersonCamZ = mikeZ + distance * cos(camRotation * 3.14159 / 180.0);
		gluLookAt(thirdPersonCamX, mikeY + 3.0f, thirdPersonCamZ, mikeX, mikeY + 2.5f, mikeZ, 0.0, 1.0, 0.0);
	}
	else if (currentCameraMode == STILL_CAM) {
		gluLookAt(camX, camY, camZ, 0, 0, 0, 0.0, 1.0, 0.0);
	}
}


GLfloat lightPosition[] = { 1.0f, 1.0f, 1.0f, 0.0f };
GLfloat rotationSpeed = 1.0f;
void updateLightPosition() {
	static float angle = 0.0f; // Static variable to retain the angle between function calls
	angle += rotationSpeed * 0.016; // Adjust the factor to control the speed of rotation

	lightPosition[0] = 5.0f * cos(angle);
	lightPosition[2] = 5.0f * sin(angle);

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	glutPostRedisplay();
}

void switchDayNight(int value) {
	isDaytime = !isDaytime;
	glutTimerFunc(transitionDuration * 1000, switchDayNight, 0);
	glutPostRedisplay();
}

void updateLightIntensity() {
	time_t currentTime = time(nullptr);
	double elapsedTimes = difftime(currentTime, lastTime);
	int elapsedTime = static_cast<int>(elapsedTimes);

	GLfloat lightIntensity[4];

	if (isDaytime) {
		// transition factor for the daytime (bright to dim)
		float transitionFactor = static_cast<float>(elapsedTime % transitionDuration2) / transitionDuration2;

		for (int i = 0; i < 4; ++i) {
			lightIntensity[i] = (1 - transitionFactor) * lightIntensityDay[i] + transitionFactor * lightIntensityNight[i];
		}

	}
	else {
		//  transition factor for the nighttime (dim to bright)
		float transitionFactor = static_cast<float>(elapsedTime % transitionDuration2) / transitionDuration2;

		for (int i = 0; i < 4; ++i) {
			lightIntensity[i] = (1 - transitionFactor) * lightIntensityNight[i] + transitionFactor * lightIntensityDay[i];
		}
	}




	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);
	glutPostRedisplay();
}

void sound(int reason) {
	switch (reason) {

	case 0: //collect coin
		PlaySound("sound-coin.wav", NULL, SND_FILENAME | SND_ASYNC);
		break;

	case 1: //hit obstacle
		PlaySound("ouch.wav", NULL, SND_FILENAME | SND_ASYNC);
		break;
	case 2: //reached vending machine
		PlaySound("boo laugh.wav", NULL, SND_FILENAME | SND_ASYNC);
		break;
	case 3: // GO OUT DOOR
		PlaySound("doorr.wav", NULL, SND_FILENAME | SND_ASYNC);
		break;
	case 4: // 
		PlaySound("booCall.wav", NULL, SND_FILENAME | SND_ASYNC);
		break;
	default:
		break;

	}
}

void renderBitmapString(float x, float y, void* font, const char* string) {
	glRasterPos2f(x, y);
	glColor3f(1.0f, 1.0f, 1.0f); // (white)
	while (*string) {
		glutBitmapCharacter(font, *string);
		string++;
	}
}

void drawScore() {
	// Render the score
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, WIDTH, 0, HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Display score in the top left corner
	char scoreStr[15];
	snprintf(scoreStr, sizeof(scoreStr), "Score: %d", score);
	renderBitmapString(10, HEIGHT - 20, GLUT_BITMAP_HELVETICA_12, scoreStr);

	// Restore the original projection and modelview matrices
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glutPostRedisplay;

}



void drawCoin(float x, float y, float z) {
	glPushMatrix();
	GLUquadricObj* qobjz;
	qobjz = gluNewQuadric();
	glTranslatef(x, y + coinFloatOffset, z);

	glScaled(2, 2, 2);
	glBindTexture(GL_TEXTURE_2D, coinTex);
	gluQuadricTexture(qobjz, true);
	gluQuadricNormals(qobjz, GL_SMOOTH);
	gluDisk(qobjz, 0, 0.2, 20, 20);
	gluDeleteQuadric(qobjz);

	glPopMatrix();
}


void updateCoinFloating() {
	coinFloatOffset += coinFloatSpeed;
	if (coinFloatOffset > 0.2f || coinFloatOffset < -0.2f) {
		coinFloatSpeed = -coinFloatSpeed;
	}
}
void handleCoinCollision(float coinX, float coinZ, bool& coinTakenFlag) {
	float distanceX = coinX - mikeX;
	float distanceZ = coinZ - mikeZ;

	// collision distance
	float collisionThreshold = 1.0f;

	if (abs(distanceX) < collisionThreshold && abs(distanceZ) < collisionThreshold && isJumping && !coinTakenFlag) {
		// Coin has been taken during a jump
		coinTakenFlag = true;
		score += 10;
		updateScore(score);
		sound(0);
	}
}

void handleCoinCollisions() {
	handleCoinCollision(12, 11, CoinTaken1);
	handleCoinCollision(5, 16, CoinTaken2);
	handleCoinCollision(15, 0, CoinTaken3);
	handleCoinCollision(-5, 10, CoinTaken4);
	handleCoinCollision(-10, 16, CoinTaken5);
	handleCoinCollision(-15, 0, CoinTaken6);
	handleCoinCollision(0, -15, CoinTaken7);
	handleCoinCollision(-8, -10, CoinTaken8);
}

void handleCactusCollision(float cactusX, float cactusZ, float cactusRadius, bool& collided) {
	float distanceX = cactusX - mikeX;
	float distanceZ = cactusZ - mikeZ;

	// distance between Mike and the cactus
	float distance = sqrt(distanceX * distanceX + distanceZ * distanceZ);


	float collisionThreshold = 1.0f + cactusRadius;

	if (distance < collisionThreshold && !collided) {
		// Collision occurred
		collided = true;
		// Push Mike back as if he was hit by the cactus
		float pushBackDistance = 1.0f;
		float directionX = distanceX / distance;
		float directionZ = distanceZ / distance;

		// Move Mike away from the cactus along the collision direction
		mikeX -= pushBackDistance * directionX;
		mikeZ -= pushBackDistance * directionZ;
		sound(1);
	}
}

void handleCactusCollisions() {


	cactusHit1 = false;
	cactusHit2 = false;
	cactusHit3 = false;
	cactusHit4 = false;

	float cactus1X = -10, cactus1Z = 10, cactus1Radius = 2.0f;
	float cactus2X = 10, cactus2Z = 10, cactus2Radius = 2.0f;
	float cactus3X = -10, cactus3Z = -10, cactus3Radius = 2.0f;
	float cactus4X = 10, cactus4Z = -10, cactus4Radius = 2.0f;

	// Check collisions with each cactus
	handleCactusCollision(cactus1X, cactus1Z, cactus1Radius, cactusHit1);
	handleCactusCollision(cactus2X, cactus2Z, cactus2Radius, cactusHit2);
	handleCactusCollision(cactus3X, cactus3Z, cactus3Radius, cactusHit3);
	handleCactusCollision(cactus4X, cactus4Z, cactus4Radius, cactusHit4);
}


void drawText(const char* text, float x, float y, float z) {
	glRasterPos3f(x, y, z);
	for (const char* c = text; *c != '\0'; ++c) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
	}
}


void handleVendingMachineCollision(float vendingX, float vendingZ, float vendingRadius, bool& collided) {
	float distanceX = mikeX - (-18);
	float distanceZ = mikeZ - (-18);

	float collisionThreshold = 2.0f;
	// Calculate the distance between Mike and the vending machine
	float distance = sqrt(distanceX * distanceX + distanceZ * distanceZ);

	if (distance < collisionThreshold && !collidedVM) {
		if (score < 30) {
			// Show a message if the score is less than 30
			glColor3f(1.0f, 0.0f, 0.0f); // Set color to red
			drawText("No enough money to buy food!", 0, 8, 0); // Replace drawText with your text rendering function
		}
		else {
			// Collision occurred
			collidedVM = true;
			sound(2);
			vendingMachineHit = true;
			displayWinScreen = true;
		} // Activate flag to display the win screen
	}
}
void handleVendingMachineCollisions() {
	vendingMachineHit = false;

	float vendingMachineX = -18, vendingMachineZ = -18, vendingMachineRadius = 3.0f;

	// Check collision with the vending machine
	handleVendingMachineCollision(vendingMachineX, vendingMachineZ, vendingMachineRadius, vendingMachineHit);
}

void print(double x, double y, char* string)
{
	int len, i;

	//set the position of the text in the window using the x and y coordinates
	glRasterPos2f(x, y);

	//get the length of the string to display
	len = (int)strlen(string);

	//loop to display character by character
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, string[i]);
	}
}

void drawYouWinDisplay() {
	glClearColor(0.2, 0.5, 0, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1, 1, 1);
	char* p0s[20];
	char* p1s[20];
	sprintf((char*)p0s, "YOU WIN! BOO IS HAPPY ^.^  Score: % d", score);
	print(0.2, 0.01, (char*)p0s);
}


void drawYouLoseDisplay() {
	glClearColor(0.5, 0.0, 0.0, 1.0f); // Maroon color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1, 1, 1);
	char* p0s[20];
	char* p1s[20];
	sprintf((char*)p0s, "YOU LOSE! BOO IS STARVING ( * - * )  Score: % d", score);
	print(0.2, 0.01, (char*)p0s);
}

void drawTimer() {
	glColor3f(0, 0, 0);  // Set text color to white

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, WIDTH, 0, HEIGHT);  // Set up orthographic projection

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glRasterPos2i(WIDTH - 100, HEIGHT - 20);  // Position in the top right corner

	char timerText[20];
	sprintf(timerText, "Timer: %d s", timer);

	for (int i = 0; timerText[i] != '\0'; ++i) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, timerText[i]);
	}

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void timerFunction(int value) {
	if (timer > 0) {
		timer--;
		glutTimerFunc(1000, timerFunction, 0);
	}
	else {

		drawYouLoseDisplay();
		glutSwapBuffers();
		return;
	}
	glutPostRedisplay();
}

/*void drawYouWinDisplay() {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, WIDTH, 0, HEIGHT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glColor3f(1.0f, 1.0f, 1.0f); // Set color to white

	// Position the text in the center of the screen
	const char* winMessage = "YOU WIN!";
	int length = strlen(winMessage);
	glRasterPos2i(WIDTH / 2 - length * 5, HEIGHT / 2);

	// Loop through each character of the message to display it
	for (int i = 0; i < length; ++i) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, winMessage[i]);
	}

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}*/
//==============================================================
//SECOND SCENE
//===================================================================

void drawWall()
{
	glDisable(GL_LIGHTING);

	glColor3f(1, 1, 1);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, tex_wall.texture[0]);

	glPushMatrix();
	glBegin(GL_QUADS);

	//Front Wall
	// Vertex 1
	glTexCoord2f(0, 0);
	glVertex3f(-20, 0, -20);
	// Vertex 2
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	// Vertex 3
	glTexCoord2f(5, 2);  // height
	glVertex3f(20, 10, -20);
	// Vertex 4
	glTexCoord2f(0, 2);  //  height
	glVertex3f(-20, 10, -20);

	//Left Wall
	glNormal3f(-1, 0, 0);
	// Vertex 1
	glTexCoord2f(0, 0);
	glVertex3f(-20, 0, -20);
	// Vertex 2
	glTexCoord2f(2, 0);
	glVertex3f(-20, 0, 20);
	// Vertex 3
	glTexCoord2f(2, 2);  //height and depth
	glVertex3f(-20, 10, 20);
	// Vertex 4
	glTexCoord2f(0, 2);
	glVertex3f(-20, 10, -20);

	// Right wall
	glNormal3f(1, 0, 0);
	// Vertex 1
	glTexCoord2f(0, 0);
	glVertex3f(20, 0, -20);
	// Vertex 2
	glTexCoord2f(2, 0);
	glVertex3f(20, 0, 20);
	// Vertex 3
	glTexCoord2f(2, 2);
	glVertex3f(20, 10, 20);
	// Vertex 4
	glTexCoord2f(0, 2);
	glVertex3f(20, 10, -20);


	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);
}
void drawRoof()
{
	glDisable(GL_LIGHTING);

	// Set roof color
	glColor3f(0.5, 0.2, 0.2);

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ceiling.texture[0]);

	// Draw roof
	glPushMatrix();
	glBegin(GL_TRIANGLES);

	// Front triangle
	glNormal3f(0, 1, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-20, 10, -20);
	glTexCoord2f(1, 0);
	glVertex3f(20, 10, -20);
	glTexCoord2f(0.5, 1);
	glVertex3f(0, 10 + 20, 0);

	// Back triangle
	glNormal3f(0, 1, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-20, 10, 20);
	glTexCoord2f(1, 0);
	glVertex3f(20, 10, 20);
	glTexCoord2f(0.5, 1);
	glVertex3f(0, 10 + 20, 0);

	// Left side triangle
	glNormal3f(-1, 1, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-20, 10, -20);
	glTexCoord2f(1, 0);
	glVertex3f(0, 10 + 20, 0);
	glTexCoord2f(0.5, 1);
	glVertex3f(-20, 10, 20);

	// Right side triangle
	glNormal3f(1, 1, 0);
	glTexCoord2f(0, 0);
	glVertex3f(20, 10, -20);
	glTexCoord2f(1, 0);
	glVertex3f(0, 10 + 20, 0);
	glTexCoord2f(0.5, 1);
	glVertex3f(20, 10, 20);

	glPopMatrix();
	glEnd();

	glEnable(GL_LIGHTING);
}
void handleDoorCollisions() {
	float doorZ = -18.0f;
	float doorXMin = -2.0f;
	float doorXMax = 2.0f;

	float mikeWidth = 2.0f; // Width of Mike's model along the X-axis
	float mikeDepth = 2.0f; // Depth of Mike's model along the Z-axis

	float mikeMinX = mikeX - mikeWidth / 2.0f;
	float mikeMaxX = mikeX + mikeWidth / 2.0f;
	float mikeMinZ = mikeZ - mikeDepth / 2.0f;
	float mikeMaxZ = mikeZ + mikeDepth / 2.0f;

	if (mikeZ == doorZ && mikeMinX <= doorXMax && mikeMaxX >= doorXMin &&
		mikeMinZ <= doorZ && mikeMaxZ >= doorZ) {
		// Collision occurred with the door
		sound(3);
		houseCollide = false;
		glutPostRedisplay();
	}
}


void handleSofaCollision(float sofaX, float sofaZ, float sofaWidth, float sofaDepth, float sofaHeight, bool& collided) {
	float distanceX = sofaX - mikeX;
	float distanceZ = sofaZ - mikeZ;

	// Calculate the distance between Mike and the sofa
	float distance = sqrt(distanceX * distanceX + distanceZ * distanceZ);

	// Calculate the collision threshold based on sofa dimensions
	float collisionThreshold = (sofaWidth + sofaDepth + sofaHeight) / 3.0f;

	if (distance < collisionThreshold && !collided) {
		// Collision occurred
		collided = true;

		float pushBackDistance = 1.0f; // Adjust as needed

		// Calculate direction to push "mike" back
		float directionX = distanceX / distance;
		float directionZ = distanceZ / distance;

		// Move "mike" away from the sofa along the collision direction
		mikeX -= pushBackDistance * directionX;
		mikeZ -= pushBackDistance * directionZ;


		sound(1);
	}
}

void handleSofaCollisions() {

	float sofa1X = -15.0f, sofa1Z = 0.0f, sofa1Width = 5.0f, sofa1Depth = 10.0f, sofa1Height = 3.0f;

	bool collidedSofa1 = false;

	// Check collision with the sofa
	handleSofaCollision(sofa1X, sofa1Z, sofa1Width, sofa1Depth, sofa1Height, collidedSofa1);


}


void handleTeddy1Collision() {
	float teddy1X = 8.0f;
	float teddy1Z = 0.0f;
	float collisionThreshold = 1.0f;

	float distanceX = teddy1X - mikeX;
	float distanceZ = teddy1Z - mikeZ;

	if (abs(distanceX) < collisionThreshold && abs(distanceZ) < collisionThreshold && !teddy1Taken) {
		teddy1Taken = true;
		sound(4);

		glutPostRedisplay(); // Redisplay the scene if needed
	}
}

void handleTeddy2Collision() {
	float teddy2X = 12.0f;
	float teddy2Z = 4.0f;
	float collisionThreshold = 1.0f;

	float distanceX = teddy2X - mikeX;
	float distanceZ = teddy2Z - mikeZ;

	if (abs(distanceX) < collisionThreshold && abs(distanceZ) < collisionThreshold && !teddy2Taken) {
		teddy2Taken = true;
		sound(4);

		glutPostRedisplay(); // Redisplay the scene if needed
	}
}


void handleCarCollision() {
	float carX = -8.0f;
	float carZ = 6.0f;
	float collisionThreshold = 2.0f; // Adjust the collision threshold as needed

	float distanceX = carX - mikeX;
	float distanceZ = carZ - mikeZ;

	if (abs(distanceX) < collisionThreshold && abs(distanceZ) < collisionThreshold && !carTaken) {
		carTaken = true;
		// Handle collision with the car
		sound(4); // Define car collision sound ID
		// Perform actions on collision (e.g., make the car disappear)

		glutPostRedisplay(); // Redisplay the scene if needed
	}
}

bool collidedTable = false;

void handleTableCollision() {
	float tableX = 0.0f;
	float tableZ = 0.0f;
	float collisionThreshold = 4.0f; // Adjust the collision threshold as needed

	float distanceX = tableX - mikeX;
	float distanceZ = tableZ - mikeZ;

	// Calculate the distance between Mike and the table
	float distance = sqrt(distanceX * distanceX + distanceZ * distanceZ);

	if (distance < collisionThreshold && !collidedTable) {
		collidedTable = true;

		// Push Mike back as if he collided with the table
		float pushBackDistance = 1.0f;
		float directionX = distanceX / distance;
		float directionZ = distanceZ / distance;

		// Move Mike away from the table along the collision direction
		mikeX -= pushBackDistance * directionX;
		mikeZ -= pushBackDistance * directionZ;

		sound(1); // Define table collision sound ID

		glutPostRedisplay(); // Redisplay the scene if needed
	}
}


//======================================================================
// Display Function
//=======================================================================
void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	setCamera();
	performJump();

	GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	if (timer <= 0) {
		drawYouLoseDisplay();
		glutSwapBuffers();
		return;
	}

	drawScore();
	drawTimer();
	drawScore();


	if (houseCollide == false) {
		// Check house collision
		checkHouseCollision();
		//outside
		updateLightIntensity();

		//TRYING TO MAKE MIKE INITIALLY LOOK LIKE HE EXITS FROM THE HOUSE
			/*	mikeZ = 6;
				mikeRotation = 310;
				camRotation = 180;*/

				// Draw Ground
		RenderGround();


		// Draw Tree Model
		glPushMatrix();
		glTranslatef(10, 0, 0);
		glScalef(0.7, 0.7, 0.7);
		model_tree.Draw();
		glPopMatrix();

		// Draw house Model
		glPushMatrix();
		glRotatef(90.f, 1, 0, 0);
		model_house.Draw();
		glPopMatrix();


		//sky box
		if (isDaytime) {
			glPushMatrix();

			GLUquadricObj* qobj;
			qobj = gluNewQuadric();
			glTranslated(50, 0, 0);
			glRotated(90, 1, 0, 1);
			glBindTexture(GL_TEXTURE_2D, tex);
			gluQuadricTexture(qobj, true);
			gluQuadricNormals(qobj, GL_SMOOTH);
			gluSphere(qobj, 100, 100, 100);
			gluDeleteQuadric(qobj);


			glPopMatrix();
		}
		else {
			glPushMatrix();

			GLUquadricObj* qobj;
			qobj = gluNewQuadric();
			glTranslated(50, 0, 0);
			glRotated(90, 1, 0, 1);
			glEnable(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, tex_stars.texture[0]);

			//glBindTexture(GL_TEXTURE_2D, starsTex);
			gluQuadricTexture(qobj, true);
			gluQuadricNormals(qobj, GL_SMOOTH);
			gluSphere(qobj, 100, 100, 100);
			gluDeleteQuadric(qobj);


			glPopMatrix();

		}
		updateCoinFloating();

		if (!CoinTaken1) {
			drawCoin(12, 3, 11);
		}

		if (!CoinTaken2) {
			drawCoin(5, 3, 16);
		}

		if (!CoinTaken3) {
			drawCoin(15, 3, 0);
		}
		if (!CoinTaken4) {
			drawCoin(-5, 3, 10);
		}

		if (!CoinTaken5) {
			drawCoin(-10, 3, 16);
		}

		if (!CoinTaken6) {
			drawCoin(-15, 3, 0);
		}

		if (!CoinTaken7) {
			drawCoin(0, 3, -15);
		}

		if (!CoinTaken8) {
			drawCoin(-8, 3, -10);
		}
		//cactus(obstacles)
		glPushMatrix();
		glTranslatef(-10, 0, 10);
		model_cactus.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(10, 0, 10);
		model_cactus.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-10, 0, -10);
		model_cactus.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(10, 0, -10);
		model_cactus.Draw();
		glPopMatrix();

		//vending machine
		glPushMatrix();
		glTranslatef(-18, 0, -18);
		glScalef(2, 2, 2);
		model_vendingM.Draw();
		glPopMatrix();


		// Draw mike
		glPushMatrix();
		glTranslatef(mikeX, mikeY, mikeZ);
		glRotatef(mikeRotation, 0.0f, 1.0f, 0.0f);
		glScalef(0.75, 0.75, 0.75);
		glColor3f(0, 1, 0);
		model_mike.Draw();
		glPopMatrix();

		if (checkCollisions) {
			handleCoinCollisions();
			handleCactusCollisions();
			handleVendingMachineCollisions();
		}
	}
	else {
		handleSofaCollisions();
		handleTeddy1Collision();
		handleTeddy2Collision();
		handleCarCollision();
		collidedTable = false;
		handleTableCollision();
		handleDoorCollisions();


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		updateLightPosition();
		/*if (currentCameraMode == STILL_CAM) {
			camX = 0;
			camY = 10;
			camZ = 30;
		}*/

		//door 
		glPushMatrix();
		glTranslatef(0, 3, -21);
		glRotatef(90, 1, 0, 0);
		glScalef(0.45, 0.45, 0.45);
		//glColor3f(1.0f, 0.96f, 0.90f);
		model_door.Draw();
		glPopMatrix();

		//sofa
		glPushMatrix();
		glTranslatef(-15, 2, 0);
		glRotatef(-90, 1, 0, 0);
		glRotatef(90, 0, 0, 1);
		glScalef(0.04, 0.04, 0.04);
		model_sofa.Draw();
		glPopMatrix();

		//car
		if (!carTaken) {
			glPushMatrix();
			glTranslatef(-8, 0, 6);
			glScalef(0.008, 0.008, 0.008);
			model_car.Draw();
			glPopMatrix();
		}

		//table
		glPushMatrix();
		glTranslatef(0, 0, 0);
		glScalef(0.06, 0.06, 0.06);
		model_table.Draw();
		glPopMatrix();


		//teddy1
		if (!teddy1Taken) {
			glPushMatrix();
			glTranslatef(8, 0, 0);
			glRotatef(-90, 1, 0, 0);
			glRotatef(-90, 0, 0, 1);
			glScalef(0.04, 0.04, 0.04);
			glColor3f(0.4f, 0.2f, 0.0f);
			model_teddy.Draw();
			glPopMatrix();
		}

		if (!teddy2Taken) {
			glPushMatrix();
			glTranslatef(12, 0, 4);
			glRotatef(-90, 1, 0, 0);
			glRotatef(-90, 0, 0, 1);
			glScalef(0.04, 0.04, 0.04);
			glColor3f(0.4f, 0.2f, 0.0f);
			model_teddy.Draw();
			glPopMatrix();
		}



		// Draw Ground
		RenderGround2();

		// Draw Walls
		drawWall();

		// Draw Roof
		drawRoof();

		// Draw mike
		glPushMatrix();
		glTranslatef(mikeX, mikeY, mikeZ);
		glRotatef(mikeRotation, 0.0f, 1.0f, 0.0f);
		glScalef(0.75, 0.75, 0.75);
		glColor3f(0, 1, 0);
		model_mike.Draw();
		glPopMatrix();





	}

	if (displayWinScreen) {
		drawYouWinDisplay();
	}

	glutSwapBuffers();
}

//=======================================================================
// Keyboard Function
//=======================================================================
void myKeyboard(unsigned char button, int x, int y)
{
	float d = 0.4;
	switch (button)
	{
	case 'P':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'O':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 27:
		exit(0);
		break;
	case '1':
		currentCameraMode = THIRD_PERSON; // Third Person POV

		//switchCamera();
		break;
	case '2':
		currentCameraMode = FIRST_PERSON; // First Person POV

		//switchCamera();
		break;
	case '3':
		currentCameraMode = STILL_CAM; // Third Person POV
		if (houseCollide) {
			camX = 0;
			camY = 10;
			camZ = 30;
		}
		else {
			camX = 0;
			camY = 20;
			camZ = 40;
		}		break;

	case 'w':
		if (currentCameraMode == STILL_CAM) {
			// Move the camera along the Y-axis
			camY += d;
		}
		break;
	case 's':
		if (currentCameraMode == STILL_CAM) {
			// Move the camera along the Y-axis in the opposite direction
			camY -= d;
		}
		break;
	case 'a':
		if (currentCameraMode == STILL_CAM) {
			// Move the camera along the X-axis
			camX -= d;
		}
		break;
	case 'd':
		if (currentCameraMode == STILL_CAM) {
			// Move the camera along the X-axis in the opposite direction
			camX += d;
		}
		break;
	case 'q':
		if (currentCameraMode == STILL_CAM) {
			// Move the camera along the Z-axis
			camZ += d;
		}
		break;
	case 'e':
		if (currentCameraMode == STILL_CAM) {
			// Move the camera along the Z-axis in the opposite direction
			camZ -= d;
		}
		break;
	default:
		break;

	}

	glutPostRedisplay();
}

void special(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP: // Move forward
		if (mikeZ - mikeSpeed >= -19) // Check if moving forward will keep Mike within the ground's Z boundaries
			mikeZ -= mikeSpeed;
		mikeRotation = 130; // Set the rotation angle to face a specific direction when moving forward
		camRotation = 0;
		break;
	case GLUT_KEY_DOWN: // Move backward
		if (mikeZ + mikeSpeed <= 19) // Check if moving backward will keep Mike within the ground's Z boundaries
			mikeZ += mikeSpeed;
		mikeRotation = 310; // Set the rotation angle to face a specific direction when moving backward
		camRotation = 180;
		break;
	case GLUT_KEY_LEFT: // Move left
		if (mikeX - mikeSpeed >= -19) // Check if moving left will keep Mike within the ground's X boundaries
			mikeX -= mikeSpeed;
		mikeRotation = 220; // Set the rotation angle to face a specific direction when moving left
		camRotation = -90;
		break;
	case GLUT_KEY_RIGHT: // Move right
		if (mikeX + mikeSpeed <= 19) // Check if moving right will keep Mike within the ground's X boundaries
			mikeX += mikeSpeed;
		mikeRotation = 400; // Set the rotation angle to face a specific direction when moving right
		camRotation = 90;
		break;
	default:
		break;
	}

	glutPostRedisplay();
}


//=======================================================================
// Motion Function
//=======================================================================
void myMotion(int x, int y)
{
	y = HEIGHT - y;

	if (cameraZoom - y > 0)
	{
		Eye.x += -0.1;
		Eye.z += -0.1;
	}
	else
	{
		Eye.x += 0.1;
		Eye.z += 0.1;
	}

	cameraZoom = y;

	glLoadIdentity();	//Clear Model_View Matrix

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters

	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glutPostRedisplay();	//Re-draw scene 
}

//=======================================================================
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{
	y = HEIGHT - y;

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		cameraZoom = y;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		isJumping = true;
		glutPostRedisplay();
	}
}

//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	model_house.Load("Models/house/house.3DS");
	model_tree.Load("Models/tree/Tree1.3ds");
	model_mike.Load("Models/mike/Toy monstr N210613.3DS");
	model_cactus.Load("Models/cactus/cacutus.3ds");
	model_vendingM.Load("Models/vending-machine/untitled.3ds");
	model_door.Load("Models/door/porta.3ds");
	model_sofa.Load("Models/sofaa/untitled.3ds");
	model_teddy.Load("Models/teddy/untitled.3ds");
	model_car.Load("Models/car/car.3ds");
	model_table.Load("Models/table/untitled.3ds");



	// Loading texture files
	tex_ground.Load("Textures/ground.bmp");
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
	//loadBMP(&starsTex, "Textures/stars.bmp", false);
	tex_stars.Load("Textures/stars.bmp");
	loadBMP(&coinTex, "textures/coin.bmp", false);
	tex_ground2.Load("Textures/floor.bmp");
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);

	tex_wall.Load("Textures/ceiling.bmp");
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);

	tex_ceiling.Load("Textures/ceiling.bmp");
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
}


//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	glutKeyboardFunc(myKeyboard);

	glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	glutReshapeFunc(myReshape);

	glutSpecialFunc(special);

	myInit();

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutTimerFunc(transitionDuration * 1000, switchDayNight, 0);
	glutTimerFunc(1000, timerFunction, 0);

	glutMainLoop();
}