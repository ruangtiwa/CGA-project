////////////////////////////////////////////////////////////////
// OnTheRoad.cpp
//
// This program shows the animation of a car which go round and 
// round a circular track.
//
// Interaction:
// Press space to toggle between animation on and off.
// Press delete to reset.
// Press the x, X, y, Y, z, Z keys to rotate the scene.
//
// Credit : Sumanta Guha.
////////////////////////////////////////////////////////////////
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <GL/freeglut.h> 
#include "getBMP.h"

#define PI 3.14159265358979324

// Globals.
static float t = 0.0; // Animation parameter.
static float Xangle = 0.0, Yangle = 0.0, Zangle = 0.0; // Angles to rotate scene.
static int isAnimate = 0; // Animated?
static int animationPeriod = 50; // Time interval between frames.
static unsigned int base; // Display lists base index.
static float signPosition = -190.0; // Beginning position of car.
static float angleBG = 0.0; // Angle of rotation of texture coordinates.
static float carAngle = 0.0; // Angle for tirning car.
static float carTranslate = 0.0; // Angle for tirning car.
static unsigned int texture[4]; // Array of texture ids.
static int isLeft = 1; // The is left?
static int keyRight = 0; // Press right-arrow key ?
static int keyLeft = 1; // Press left-arrow key ?
static float carAngleChange = 2.0;

static bool isTurn = false;

// Load external textures.
void loadTextures()
{
	// Local storage for image data.
	imageFile *image[4];

	// Load the image.
	image[0] = getBMP("../../../Textures/grass.bmp");
	image[1] = getBMP("../../../Textures/road.bmp");
	image[2] = getBMP("../../../Textures/seaSky.bmp");
	image[3] = getBMP("../../../Textures/sign.bmp");

	// Create texture object texture[0]. 
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	// Specify image data for currently active texture object.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->width, image[0]->height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
	// Set texture parameters for wrapping.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture parameters for filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// End texture[0]

	// Road texture[1]
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[1]->width, image[1]->height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, image[1]->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// End road 

	// Bind sky image to texture[2]
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[2]->width, image[2]->height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, image[2]->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// End sky

	// winner texture[3]
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[3]->width, image[3]->height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, image[3]->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// End road 
	
}

// Draw winner sign
void drawWinnerSign(void) {

	glPushMatrix();
	glTranslatef(-8.0, 8.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glutSolidCylinder(1.0, 8.0, 10.0, 10.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(8.0, 8.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glutSolidCylinder(1.0, 8.0, 10.0, 10.0);
	glPopMatrix();
	
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glBegin(GL_POLYGON); 
	glTexCoord2f(0.0, 0.0); glVertex3f(-7.0, 5.0, 0.0); 
	glTexCoord2f(1.0, 0.0); glVertex3f(7.0, 5.0, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(7.0, 8.0, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-7.0, 8.0, 0.0);
	glEnd();
	glPopMatrix();

}

// Draw road
void drawRoad(void) {
	// Map the grass texture onto a rectangle along the xz-plane.
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0); glVertex3f(-10.0, 0.0, 30.0);
	glTexCoord2f(8.0, 0.0); glVertex3f(10.0, 0.0, 30.0);
	glTexCoord2f(8.0, 8.0); glVertex3f(10.0, 0.0, -280.0);
	glTexCoord2f(0.0, 8.0); glVertex3f(-10.0, 0.0, -280.0);
	glEnd();
	
}

void lineRoad(void) {
	float x = 0.0;
	for (int i = 0; i < 10; i++)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_POLYGON);
		glVertex3f(-1.0, 1.0, 0.0 + x);
		glVertex3f(1.0, 1.0, 0.0 + x);
		glVertex3f(1.0, 1.0, 3.0 + x);
		glVertex3f(-1.0, 1.0, 3.0 + x);
		glEnd();

		x = x + 2.0;
	}

}

// Routine to draw background.
void drawBG(void)
{
	glPushMatrix();

	// Grass
	glPushMatrix();
	// Activate texture object.
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	// Map the texture onto a square polygon.
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0); glVertex3f(-100.0, -0.5, 100.0);
	glTexCoord2f(8.0, 0.0); glVertex3f(100.0, -0.5, 100.0);
	glTexCoord2f(8.0, 8.0); glVertex3f(100.0, -0.5, -100.0);
	glTexCoord2f(0.0, 8.0); glVertex3f(-100.0, -0.5, -100.0);
	glEnd();
	glPopMatrix();
	// End grass
	
	glPushMatrix();
	glTranslatef(10.0 * cos(angleBG), 10.0 * sin(angleBG), 0.0);
	glPushMatrix();
	// Map the sky texture onto a rectangle parallel to the xy-plane.
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0); glVertex3f(-100.0, -30.0, -50.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(100.0, -30.0, -50.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(100.0, 120.0, -50.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-100.0, 120.0, -50.0);
	glEnd();
	glPopMatrix();
	glPopMatrix();

	glPopMatrix();

}

void drawCar(void)
{
	// Move the car to the middle of the road.
	float wheelCarAngle = -((1 - t)*0.0 + t*360.0);
	
	
	//glRotatef(carAngle, 0.0, 0.0, 1.0);
	//glTranslatef(0.0, -15.0, 0.0);

	// Start whole of car.
	glPushMatrix();
	glTranslatef(0.0, 0.0, 1.5);
	glRotatef(90.0, 1.0, 0.0, 0.0);

	glPushMatrix(); //---------------------------------------------------Start for body and front wheels.-------------------------------//
	glRotatef(carAngle, 0.0, 1.0, 0.0);
	// Start car body.

	// Windows of car.
		// Right Windows of car.
			// 1st right window.
			glPushMatrix();
			glColor3f(0.0, 1.0, 0.0);
			glTranslatef(3.0, 3.0, 1.55);
			glScalef(1.0, 1.0, 0.0);
			glutSolidCube(1.0);
			glPopMatrix();
			// End 1st right window.

			// 2nd right window.
			glPushMatrix();
			glColor3f(0.0, 1.0, 0.0);
			glTranslatef(1.0, 3.0, 1.55);
			glScalef(1.0, 1.0, 0.0);
			glutSolidCube(1.0);
			glPopMatrix();
			// End 2nd right window.

			// 3rd  right window.
			glPushMatrix();
			glColor3f(0.0, 1.0, 0.0);
			glTranslatef(-1.0, 3.0, 1.55);
			glScalef(1.0, 1.0, 0.0);
			glutSolidCube(1.0);
			glPopMatrix();
			// End 3rd right window.

			// 4th right window.
			glPushMatrix();
			glColor3f(0.0, 1.0, 0.0);
			glTranslatef(-3.0, 3.0, 1.55);
			glScalef(1.0, 1.0, 0.0);
			glutSolidCube(1.0);
			glPopMatrix();
			// End 4th right window.
		// End right window.

		// Left Windows of car.
			// 1st left window.
			glPushMatrix();
			glColor3f(0.0, 1.0, 0.0);
			glTranslatef(3.0, 3.0, -1.55);
			glScalef(1.0, 1.0, 0.0);
			glutSolidCube(1.0);
			glPopMatrix();
			// End 1st left window.

			// 2nd left window.
			glPushMatrix();
			glColor3f(0.0, 1.0, 0.0);
			glTranslatef(1.0, 3.0, -1.55);
			glScalef(1.0, 1.0, 0.0);
			glutSolidCube(1.0);
			glPopMatrix();
			// End 2nd left window.

			// 3rd  left window.
			glPushMatrix();
			glColor3f(0.0, 1.0, 0.0);
			glTranslatef(-1.0, 3.0, -1.55);
			glScalef(1.0, 1.0, 0.0);
			glutSolidCube(1.0);
			glPopMatrix();
			// End 3rd left window.

			// 4th left window.
			glPushMatrix();
			glColor3f(0.0, 1.0, 0.0);
			glTranslatef(-3.0, 3.0, -1.55);
			glScalef(1.0, 1.0, 0.0);
			glutSolidCube(1.0);
			glPopMatrix();
			// End 4th left window.
		// End left window.

		// Front Windows of car.
			// 1st front window.
			glPushMatrix();
			glColor3f(0.0, 1.0, 0.0);
			glTranslatef(4.55, 3.0, 0.0);
			glScalef(0.0, 1.0, 2.2);
			glutSolidCube(1.0);
			glPopMatrix();
			// End 1st left window.	
		// End front window.
	// End windows of car.

	// Draw red body of car
	glPushMatrix(); 
	glColor3f(1.0, 0.0, 0.0);
	glTranslatef(0.0, 2.5, 0.0);
	glScalef(3.0, 1.0, 1.0);
	glutSolidCube(3.0);
	glPopMatrix();
	// End car body.

	// Start 2nd and 4th wheel.
	glPushMatrix();
	glTranslatef(3.0, 0.0, 0.0);
	glRotatef(wheelCarAngle * 4, 0.0, 0.0, 1.0);
	glColor3f(0.0, 1.0, 0.0);

		// Start 4th wheel.
		glPushMatrix();
		glTranslatef(0.0, 0.0, -2.0);
		glutWireTorus(0.6, 1, 10, 10);
		glPopMatrix();
		// axles for 2,4 wheel.
		glPushMatrix();
		glTranslatef(0.0, 0.0, -2.0);
		glutWireCylinder(1.0, 4.0, 10.0, 10.0);
		glPopMatrix();
		// End 4th wheel.

		// Start 2nd wheel.
		glPushMatrix();
		glTranslatef(0.0, 0.0, 2.0);
		glutWireTorus(0.6, 1, 10, 10);
		glPopMatrix();
		// End 2nd wheel.
	glPopMatrix();
	// End 2nd and 4th wheel.

	glPopMatrix(); //---------------------------------------------------End for body and front wheels.-------------------------------//

	// Start 1st and 3rd wheel.
	glPushMatrix(); //********************************************************Start back wheels.***************************************//
	glRotatef(carAngle, 0.0, 1.0, 0.0);
		glPushMatrix();
		glTranslatef(-3.0, 0.0, 0.0);
		glRotatef(wheelCarAngle * 4, 0.0, 0.0, 1.0);
		glColor3f(0.0, 0.0, 0.0);

			// Start 3rd wheel.
			glPushMatrix();
			glTranslatef(0.0, 0.0, -2.0);
			glutWireTorus(0.6, 1, 10, 10);
			glPopMatrix();
			// Axles 1,3 wheel.
			glPushMatrix();
			glTranslatef(0.0, 0.0, -2.0);
			glutWireCylinder(1.0, 4.0, 10.0, 10.0);
			glPopMatrix();
			// End 3rd wheel.

			// Start 1st wheel.
			glPushMatrix();
			glTranslatef(0.0, 0.0, 2.0);
			glutWireTorus(0.6, 1, 10, 10);
			glPopMatrix();
			// End 1st wheel.
		glPopMatrix();
	glPopMatrix(); //********************************************************End back wheels.***************************************//
	// End 1st and 3rd wheel.

	glPopMatrix();
	// End car.	
}

// Initialization routine.
void setup(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);

	glEnable(GL_DEPTH_TEST); // Enable depth testing.	

    // Create texture ids.
	glGenTextures(4, texture);

	// Load external texture. 
	loadTextures();

	// Specify how texture values combine with current surface color values.
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	
	
}

// Drawing routine.
void drawScene(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//carTranslate += t;

	std::cout << "Translate" << std::endl;
	std::cout << carTranslate << std::endl;
	
	//std::cout << keyLeft << std::endl;

	glDisable(GL_TEXTURE_2D);
	glLoadIdentity();
	gluLookAt(0.0, 10.0, 20.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	
	// Draw Car.
	glPushMatrix();
	glRotatef(90.0, 0.0, 0.0, 1.0);
	glRotatef(90.0, 0.0, 1.0, 0.0);
	glTranslatef(0.0, carTranslate, 0.0);
	drawCar();
	glPopMatrix();
	// End car display.


	glPushMatrix();
	glTranslatef(0.0, -0.5, 0.0);
	lineRoad();
	glPopMatrix();

	// Turn on OpenGL texturing.
	glEnable(GL_TEXTURE_2D);

	// Road display
	glPushMatrix();
	
	drawRoad();
	glPopMatrix();

	// Draw grass.
	glPushMatrix();
	glTranslatef(0.5 * cos(angleBG), 0.5 * sin(angleBG), 0.0);
	drawBG();
	glPopMatrix();
	// End grass display.

	// Winner sign
	glPushMatrix();
	glTranslatef(0.0, 0.0, signPosition);
	drawWinnerSign();
	glPopMatrix();
	// End sign

	glutSwapBuffers();
}

// Timer function.
void animate(int value)
{
	if (isAnimate)
	{
		t += 0.01;
		if (t >= 1.0) t = 0.0;

		// Moving winner sign
		if (signPosition >= -190)
		{
			signPosition = signPosition + 1.0;
			if (signPosition == 10) {
				isAnimate = 0;
			}
				
		}

		if (keyLeft == 1)
		{
			std::cout << "Keyleft" << std::endl;
			std::cout << carAngle << std::endl;
			std::cout << carAngleChange << std::endl;
			if (isTurn == false) // Move in first time.
			{
				std::cout << "Here is first move." << std::endl;
				carTranslate += 0.13;
			}
			else // Move after be on right-side.
			{
				std::cout << "Here is first move." << std::endl;
				carTranslate += 0.29;
			}
			
			carAngle += carAngleChange;
			
			if (carAngle > 28.0)
			{
				carAngleChange *= -1;

			}

			if (carAngle < 1)
			{
				keyLeft = 0;
				
				//carAngle = 0;
			}
		}

		if (keyRight == 1)
		{
			std::cout << "Keyright" << std::endl;
			std::cout << carAngle << std::endl;
			std::cout << carAngleChange << std::endl;
			//if (windAngle <= -10.0) windAngleChange *= -1; --> right
			carTranslate -= 0.27;
			isTurn = true;
			carAngle += carAngleChange;

			if (carAngle < -28.0)
			{
				carAngleChange *= -1;
			}

			if (carAngle > 1)
			{
				keyRight = 0;
				
				//carAngle = 0;
			}
			
		}
		
		
		// Animation for background
		angleBG += 0.01;
		if (angleBG > 360.0) angleBG -= 360.0;

		
		glutPostRedisplay();
		glutTimerFunc(animationPeriod, animate, 1);
	}
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);

	glMatrixMode(GL_MODELVIEW);
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case ' ':
		if (isAnimate) isAnimate = 0;
		else
		{
			isAnimate = 1;
			animate(1);
		}
		break;
	case 127:
		if (isAnimate) isAnimate = 0;
		t = 0.0;
		signPosition = -190.0;
		carAngle = 0.0;
		glutPostRedisplay();
		break;
	case 'x':
		Xangle += 5.0;
		if (Xangle > 360.0) Xangle -= 360.0;
		glutPostRedisplay();
		break;
	case 'X':
		Xangle -= 5.0;
		if (Xangle < 0.0) Xangle += 360.0;
		glutPostRedisplay();
		break;
	case 'y':
		Yangle += 5.0;
		if (Yangle > 360.0) Yangle -= 360.0;
		glutPostRedisplay();
		break;
	case 'Y':
		Yangle -= 5.0;
		if (Yangle < 0.0) Yangle += 360.0;
		glutPostRedisplay();
		break;
	case 'z':
		Zangle += 5.0;
		if (Zangle > 360.0) Zangle -= 360.0;
		glutPostRedisplay();
		break;
	case 'Z':
		Zangle -= 5.0;
		if (Zangle < 0.0) Zangle += 360.0;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

void specialKeyInput(int key, int x, int y)
{
	 float tempAngle = 0.0;

	// Compute next position.
	 if (key == GLUT_KEY_LEFT) { 
		 keyLeft = 1;
		 keyRight = 0;
	 }

	 if (key == GLUT_KEY_RIGHT) {
		 keyRight = 1;
		 keyLeft = 0;
	 }
	
	carAngle = tempAngle;

	glutPostRedisplay();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
	std::cout << "Interaction:" << std::endl;
	std::cout << "Press space to toggle between animation on and off." << std::endl
		<< "Press delete to reset." << std::endl
		<< "Press the x, X, y, Y, z, Z keys to rotate the scene." << std::endl;
}

// Main routine.
int main(int argc, char **argv)
{
	printInteraction();
	glutInit(&argc, argv);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Car.cpp");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	glutSpecialFunc(specialKeyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}

