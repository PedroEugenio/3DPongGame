/*
*SummerGame.cpp: Main file
*/
#include "stdafx.h"
#include <windows.h> 
#include <glut.h>  

#include <math.h>

#include "RgbImage.h"

#include <mmsystem.h>	//sound

// keycodes
#define VK_A 0x41
#define VK_S 0x53
#define VK_Z 0x5A

//window variables
int width = 800;
int height = 500;
int frame_rate = 1000 / 60; //60 fps

//rackets settings
int racket_width = 2;
int racket_height = 1;
int racket_depth = 1;
float racket_speed = 0.5;

//racket position front
float racket_posx = 10.0f;
float racket_posy = 0.0f;
float racket_posz = -10.0f;

//racket position bottom
float bottomracket_posx = 10.0f;
float bottomracket_posy = 0.0f;
float bottomracket_posz = -20.0f;

// ball
float ball_posx = 10.0;
float ball_posy = 0.5;
float ball_posz = -15.0;
float ball_dir_x = 0.0f;
float ball_dir_z = -1.0f;
float ball_size = 0.3;
float ball_speed = 0.1;

//score variables
int score_player1 = 0;
int score_player2 = 0;
char buffer1[15];
char buffer2[5];

float mat_especular[] = { 1.0, 1.0, 1.0, 1.0 };
float mat_brilho[] = { 100.0 };
float pos_luz[] = { 5.0, 30.0, -5.0, 1.0 };	//{x,y,z,type} type = 0.0 -> directional light
											// type = 1.0 -> position of the light

char* filename = "sand.bmp";

///////////////////Functions/////////////////////////////
void loadTextureFromFile(char *filename);
void init();
const char *score_string(int player1, int player2);
void draw_text(float x, float y, const char *text);
void rect(float posx, float posy, float posz, float racket_width, float racket_height, float racket_depth);
void keyboard();
void vec2_norm(float& x, float &y);
void ball();
void background();
void textDisplay();
void display();
void update(int value);
void reshape(GLsizei width, GLsizei height);
/////////////////////////////////////////////////////////

void loadTextureFromFile(char *filename)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);

	RgbImage theTexMap(filename);

	// Pixel alignment: each row is word aligned (aligned to a 4 byte boundary)
	// Therefore, no need to call glPixelStore( GL_UNPACK_ALIGNMENT, ... );

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, theTexMap.GetNumCols(),
		theTexMap.GetNumRows(), GL_RGB, GL_UNSIGNED_BYTE,
		theTexMap.ImageData());
}
void init() {


	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_COLOR_MATERIAL); //keep the colors
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_especular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_brilho);
	glLightfv(GL_LIGHT0, GL_POSITION, pos_luz);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
}

const char *score_string(int player1, int player2) { //convert integers scores into chars
	_itoa(score_player1, buffer1, 10);
	_itoa(score_player2, buffer2, 10);
	strcat(buffer1, " : ");
	strcat(buffer1, buffer2);

	return buffer1;
}

void draw_text(float x, float y, const char *text) {
	int len = strlen(text);
	glNormal3f(0.0,0.0,-1.0);
	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2f(x, y);
	for (int i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
	}
}

void rect(float posx, float posy, float posz, float racket_width, float racket_height, float racket_depth) {
	glLoadIdentity();	// Reset the model-view matrix
	glTranslatef(posx, posy, posz);

	glBegin(GL_QUADS);
	// Top face
	glNormal3f(0.0, 1.0, 0.0); //Normal vector perpendicular to the face
	glColor3f(0.2f, 0.5f, 1.0f);     // blue
	glVertex3f((racket_width / 2), (racket_height / 2), -(racket_depth / 2));
	glVertex3f(-(racket_width / 2), (racket_height / 2), -(racket_depth / 2));
	glVertex3f(-(racket_width / 2), (racket_height / 2), (racket_depth / 2));
	glVertex3f((racket_width / 2), (racket_height / 2), (racket_depth / 2));

	// Bottom face
	glNormal3f(0.0, -1.0, 0.0);
	glColor3f(1.0f, 0.5f, 0.0f);     // Orange
	glVertex3f((racket_width / 2), -(racket_height / 2), (racket_depth / 2));
	glVertex3f(-(racket_width / 2), -(racket_height / 2), (racket_depth / 2));
	glVertex3f(-(racket_width / 2), -(racket_height / 2), -(racket_depth / 2));
	glVertex3f((racket_width / 2), -(racket_height / 2), -(racket_depth / 2));

	// Front face
	glNormal3f(0.0,0.0, -1.0);
	glColor3f(1.0f, 0.0f, 0.0f);     // Red
	glVertex3f((racket_width / 2), (racket_height / 2), (racket_depth / 2));
	glVertex3f(-(racket_width / 2), (racket_height / 2), (racket_depth / 2));
	glVertex3f(-(racket_width / 2), -(racket_height / 2), (racket_depth / 2));
	glVertex3f((racket_width / 2), -(racket_height / 2), (racket_depth / 2));

	// Back face 
	glNormal3f(0.0, 0.0,1.0);
	glColor3f(0.5f, 0.5f, 0.0f);     // Yellow
	glVertex3f((racket_width / 2), -(racket_height / 2), -(racket_depth / 2));
	glVertex3f(-(racket_width / 2), -(racket_height / 2), -(racket_depth / 2));
	glVertex3f(-(racket_width / 2), (racket_height / 2), -(racket_depth / 2));
	glVertex3f((racket_width / 2), (racket_height / 2), -(racket_depth / 2));

	// Left face
	glNormal3f(-1.0, 0.0, 0.0);
	glColor3f(0.0f, 0.0f, 0.6f);     // Blue
	glVertex3f(-(racket_width / 2), (racket_height / 2), (racket_depth / 2));
	glVertex3f(-(racket_width / 2), (racket_height / 2), -(racket_depth / 2));
	glVertex3f(-(racket_width / 2), -(racket_height / 2), -(racket_depth / 2));
	glVertex3f(-(racket_width / 2), -(racket_height / 2), (racket_depth / 2));

	// Right face
	glNormal3f(1.0, 0.0, 0.0);
	glColor3f(0.5f, 0.0f, 0.5f);     // Magenta
	glVertex3f((racket_width / 2), (racket_height / 2), -(racket_depth / 2));
	glVertex3f((racket_width / 2), (racket_height / 2), (racket_depth / 2));
	glVertex3f((racket_width / 2), -(racket_height / 2), (racket_depth / 2));
	glVertex3f((racket_width / 2), -(racket_height / 2), -(racket_depth / 2));
	glEnd();  // End of drawing color-cube
}
void keyboard() {
	// racket
	if (GetAsyncKeyState(VK_RIGHT) && racket_posx <= 17.5)
		racket_posx += racket_speed;
	if (GetAsyncKeyState(VK_LEFT) && racket_posx >= 2.5)
		racket_posx -= racket_speed;
	if (GetAsyncKeyState(VK_S) && bottomracket_posx <= 17.5)
		bottomracket_posx += racket_speed;
	if (GetAsyncKeyState(VK_A) && bottomracket_posx >= 2.5)
		bottomracket_posx -= racket_speed;
}

void vec2_norm(float& x, float &y) {
	// sets a vectors length to 1 (which means that x + y == 1)
	float length = sqrt((x * x) + (y * y));
	if (length != 0.0f) {
		length = 1.0f / length;
		x *= length;
		y *= length;
	}
}

void ball() {
	// fly a bit
	ball_posx += ball_dir_x * ball_speed;
	ball_posz += ball_dir_z * ball_speed;

	// hit by bottom racket
	if (ball_posz <= -20 + racket_depth && ball_posz >= bottomracket_posz
		&& ball_posx <= bottomracket_posx + racket_width/2
		&& ball_posx >= bottomracket_posx - racket_width / 2) {
		// set direction depending on where it hit the racket
		float t = ((ball_posx - bottomracket_posx) / racket_width) - 0.5f;
		ball_dir_z = fabs(ball_dir_z); // force it to be positive
		ball_dir_x = t;
	}

	// hit by front racket
	if (ball_posz >= -10 - racket_depth && ball_posz >= racket_posz - racket_depth + 0.5 //To adjust the impact
		&& ball_posx <= racket_posx + racket_width/2
		&& ball_posx >= racket_posx - racket_width / 2) {
		// set direction depending on where it hit the racket
		float t = ((ball_posx - racket_posx) / racket_width) - 0.5f;
		ball_dir_z = -fabs(ball_dir_z); // force it to be negative
		ball_dir_x = t;
	}

	// hit bottom wall
	if (ball_posz < -21) {
		++score_player2;
		ball_posz = -15.0;
		ball_posx = 10.0;
		ball_dir_z = fabs(ball_dir_z); // force it to be positive
		ball_dir_x = 0;
	}

	// hit front wall
	if (ball_posz > -9) {
		++score_player1;
		ball_posz = -15.0;
		ball_posx = 10.0;
		ball_dir_z = -fabs(ball_dir_z); // force it to be negative
		ball_dir_x = 0;
	}

	// hit top wall
	if (ball_posx > 18) {
		ball_dir_x = -fabs(ball_dir_x); // force it to be negative
	}

	// hit bottom wall
	if (ball_posx < 2) {
		ball_dir_x = fabs(ball_dir_x); // force it to be positive
	}

	// make sure that length of dir stays at 1
	vec2_norm(ball_dir_x, ball_dir_z);
}

void background() {
	//Background
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glLoadIdentity();

	glBegin(GL_QUADS);
	glNormal3f(0.0,1.0,0.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(1.0, -0.5, -8.0);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(1.0, -0.5, -22.0);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(19.0, -0.5, -22.0);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(19.0, -0.5, -8.0);

	glEnd();
	glDisable(GL_TEXTURE_2D);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void textDisplay() {
	//Changing 3D-2D to draw the text score
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// Switch to orthographic projection
	glLoadIdentity();
	glOrtho(0.0, width, 0.0, height, 0.1, 20);

	glMatrixMode(GL_MODELVIEW);

	draw_text(width / 2 - 10, height - 30, score_string(score_player1, score_player2)); //score

	// get back the old prespective projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix

	background();

	rect(racket_posx, racket_posy, racket_posz, racket_width, racket_height, racket_depth);
	//printf("Front Racket: \nx: %f\ny: %f\nz: %f\n", racket_posx, racket_posy, racket_posz);
	rect(ball_posx, ball_posy, ball_posz, ball_size, ball_size, ball_size);
	rect(bottomracket_posx, bottomracket_posy, bottomracket_posz,racket_width,racket_height,racket_depth);
	//printf("Bottom Racket: \nx: %f\ny: %f\nz: %f\n", bottomracket_posx, bottomracket_posy, bottomracket_posz);
	

	//walls
	rect(10, 0, -22, 19, 1, 1); //bottom
	rect(10, 0, -8, 19, 1, 1);	//front
	rect(1, 0, -15, 1, 1, 14); //left
	rect(19, 0, -15, 1, 1, 14);	//Right
	
	textDisplay();
	

	glMatrixMode(GL_MODELVIEW);

	glutSwapBuffers();

}

void update(int value) {
	keyboard();
	ball();
	glutTimerFunc(frame_rate, update, 0);

	//Redisplay frame
	glutPostRedisplay();
}


void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
											   // Compute aspect ratio of the new window
	if (height == 0) height = 1;                // To prevent divide by 0
	GLfloat aspect = (GLfloat)width / (GLfloat)height;

	// Set the viewport to cover the new window
	glViewport(0, 0, width, height);

	// Set the aspect ratio of the clipping volume to match the viewport
	glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
	glLoadIdentity();

	// Enable perspective projection with fovy, aspect, zNear and zFar
	gluPerspective(50.0f, aspect, 0.1f, 100.0f);
	// gluLookAt( GLdouble eyex, GLdouble eyey, GLdouble eyez, 
				//GLdouble centerx, GLdouble centery, GLdouble centerz, 
				//GLdouble upx, GLdouble upy, GLdouble upz )
	gluLookAt(10, 15, 0, 10, 0, -15, 0, 5, 0);
}


int main(int argc, char** argv) {
	glutInit(&argc, argv);   
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH); // Enable double buffered mode
	glutInitWindowSize(width, height);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - 500) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - 500) / 2);
	glutCreateWindow("Summer Game");
	printf("Player 1:\nA - Mover para a Esquerda\nS - Mover para a Direita\n\nPlayer 2:\nSeta Direita - Mover para a Direita\nSeta Esquerda - Mover para a Esquerda\n");

	//Display
	glutDisplayFunc(display);       
	glutReshapeFunc(reshape);
	glutTimerFunc(frame_rate, update, 0);  

	//background sound
	sndPlaySoundA("beach.wav", SND_ASYNC | SND_LOOP); //SND_ASYNC | SND_LOOP to reproduce as background sound

	//Setup scene
	init();

	glutMainLoop();                 
	return 0;
}
