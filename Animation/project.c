#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "glut.h"


// display
#define WIN_POSX    400
#define WIN_POSY    400
#define WIN_WIDTH   400
#define WIN_HEIGHT  300

// texture mapping
#define IW	128			// texture mapping readin Image Width - background
#define IH	128				// texture mapping readin Image Height - background
unsigned char GroundImage		[IW][IH][4];
unsigned char WallImage_1		[IW][IH][4];
unsigned char WallImage_2		[IW][IH][4];

// constants for characters
#define RADIUS_OF_HEAD  10      // head as a ball

#define RADIUS_OF_BODY  2
#define HEIGHT_OF_BODY  50      // body as a thick cylinder

#define RADIUS_OF_LIMBS  1
#define LENGTH_OF_LIMBS  30       // limbs as thin cylinders

#define ARM_POSITION 45     //position that arms are placed

// tunring degrees for animation
#define PI 3.14159
#define THETA 90		//angle for upper-arm and thigh	as initial position
#define PHI 120			//angle for shank as initial position
#define ALPHA 30		//angle for lower-arm
double theta, phi;		//rotation degree for upper arm and thigh
double alpha;			//rotation degree for lower arm
double t_prev; 			//previous time elapsed

typedef struct point{	// define a structure for positioning
	double x;
	double y;
	double z;
}position;
position displacement_character;	//position of the main character in the view window
position displacement_camera;		//position of the camera

// lighting and shadowing
GLfloat light0_x = 150, light0_y = 80, light0_z = -9;	// the white light 
GLfloat light1_x = 300, light1_y = 80, light1_z = 100;	// the brassy light

void init_positioning(){
	displacement_character.x = displacement_character.y = displacement_character.z = 0;
	displacement_camera.x = 40; displacement_camera.y = 20; displacement_camera.z = 400;
}

// define light source and surface type
void init_lighting(){
	// light 0
	GLfloat light0_position[] = {light0_x, light0_y, light0_z, 1.0 };	//point source
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glEnable(GL_LIGHT0);

	// light 1
	GLfloat light1_ambient[] = { 0.9, 0.7, 0.6, 1.0 }; glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	GLfloat light1_diffuse[] = { 0.9, 0.7, 0.6, 1.0 }; glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	GLfloat light1_specular[] = { 0.9, 0.7, 0.6, 1.0 }; glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);

	GLfloat light1_position[] = {light1_x, light1_y, light1_z, 1.0 };	//point source	
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glEnable(GL_LIGHT1);

	glEnable(GL_LIGHTING);
}

void init_material(){
    // assume all objects use the same material
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 75.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

void loadTextureImage(unsigned char Image[][IH][4], int index){
    int i, j, c;
	int a, b;
 
    for (i = 0; i < IH; i++) {
       for (j = 0; j < IW; j++) {
          a = b = c = ((((i&0x8)==0)^((j&0x8))==0))*255;	//draw a checkboard for texture mapping
		  if (c == 0){
			  if (index == 0){a = 150; b = c = 20;}
			  if (index == 1){a = 20; b = c = 150;}
			  if (index == 2){a = c = 150; b = 20;}
		  }
          Image[i][j][0] = (GLubyte) a;		//R
          Image[i][j][1] = (GLubyte) b;		//G
          Image[i][j][2] = (GLubyte) c;		//B
          Image[i][j][3] = (GLubyte) 255;
       }
    }


	// FILE *fp;
	// int  i, j, k;
	// unsigned char temp;

	// char* filename = "Ground.bmp";
	// if ((fp = fopen(filename, "rb")) == NULL) {		// if throw warnings for fopen
	// 												// right-click the project name -> properties -> C/C++ -> preprocessor -> definition
	// 												// add "_CRT_SECURE_NO_WARNINGS" -> save
	// 	printf("Error (ReadImage) : Cannot read the file!!\n");
	// 	exit(1);
	// }

	// for (i = 0; i < IW; i++) {
	// 	for (j = 0; j < IH; j++) {
	// 		for (k = 0; k < 3; k++) {       // k = 0 is Red  k = 1 is Green K = 2 is Blue
	// 			fscanf(fp, "%c", &temp);
	// 			Image[i][j][k] = (unsigned char)temp;
	// 			printf("input temp = %c\n", temp);
	// 		}
	// 		Image[i][j][3] = (unsigned char)0;         // alpha = 0.0
	// 	}
	// }
	// fclose(fp);
}

void textMap(){
	// ground: X-Z plane
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, IW, IH, 0, GL_RGBA, GL_UNSIGNED_BYTE, GroundImage);
    glEnable (GL_TEXTURE_2D);

    // mix the texture with the material and lighting
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	// assign the full range of texture colors to a quadrilateral
	glBegin (GL_QUADS);
		glTexCoord2f (0.0, 0.0); glVertex3f(-200, -100, -1500);
		glTexCoord2f (1.0, 0.0); glVertex3f(600, -100, -1500);
		glTexCoord2f (1.0, 1.0); glVertex3f(600, -100, 1500);
		glTexCoord2f (0.0, 1.0); glVertex3f(-200, -100, 1500);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	// wall_1: X-Y plane
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, IW, IH, 0, GL_RGBA, GL_UNSIGNED_BYTE, WallImage_1);
    glEnable (GL_TEXTURE_2D);

    // mix the texture with the material and lighting
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	// assign the full range of texture colors to a quadrilateral
	glBegin (GL_QUADS);
		glTexCoord2f (0.0, 0.0); glVertex3f(-20, -100, -10);
		glTexCoord2f (1.0, 0.0); glVertex3f(600, -100, -10);
		glTexCoord2f (1.0, 1.0); glVertex3f(600, 700, -10);
		glTexCoord2f (0.0, 1.0); glVertex3f(-20, 700, -10);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	// wall_2: Y-Z plane
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, IW, IH, 0, GL_RGBA, GL_UNSIGNED_BYTE, WallImage_2);
    glEnable (GL_TEXTURE_2D);

    // mix the texture with the material and lighting
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	// assign the full range of texture colors to a quadrilateral
	glBegin (GL_QUADS);
		glTexCoord2f (1.0, 0.0); glVertex3f(-10, -100, -1500);
		glTexCoord2f (0.0, 0.0); glVertex3f(-10, -100, 1500);
		glTexCoord2f (0.0, 1.0); glVertex3f(-10, 700, 1500);
		glTexCoord2f (1.0, 1.0); glVertex3f(-10, 700, -1500);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

// draw head
void draw_head(){
	glPushMatrix();

	//glColor3f(0.0, 0.0, 0.0);	//disabled for lighting and shading
	glTranslatef(0, RADIUS_OF_HEAD, 0);
	glutSolidSphere(RADIUS_OF_HEAD, 100, 100);

	glPopMatrix();
}

// draw cylinder
GLUquadricObj *trunk_middle, *trunk_top, *trunk_bottom;

void trunk(double radius, double length) {
	
	glPushMatrix();

	// glColor3f(0.0, 0.0, 0.0);

	glRotatef(-90, 1.0, 0.0, 0.0);		//parallel to y-axis
	
	gluCylinder(trunk_middle, radius, radius, length, 100, 100);
	gluDisk (trunk_bottom, 0, radius, 100, 100);		//seal the bottom of body
	glPushMatrix();

	glTranslatef(0, length, 0);
	gluDisk (trunk_top, 0, radius, 100, 100);		//seal the top of body
	glPopMatrix();

	glPopMatrix();
}

// draw body
void draw_body(){

	glPushMatrix();
	trunk(RADIUS_OF_BODY, HEIGHT_OF_BODY);
	glPopMatrix();

}

// integrate all parts for the main character
void draw_character(){
	glPushMatrix();

	glTranslatef(displacement_character.x, displacement_character.y, displacement_character.z);
	glScalef(-1, 1, 1);
	glRotatef(5, 0, 0, 1);
    draw_head();

    glTranslatef(0.0, -HEIGHT_OF_BODY, 0.0);		// move to draw the body part
    draw_body();

    glPushMatrix();

    glRotatef(theta, 0, 0, 1);      //rotate with animation
    glRotatef(180 - 0.5 * THETA, 0, 0, 1);
    trunk(RADIUS_OF_LIMBS, LENGTH_OF_LIMBS);    // upper left leg

    glTranslatef(0, LENGTH_OF_LIMBS, 0);
    glRotatef(phi, 0, 0, 1);        //rotate with animation
    trunk(RADIUS_OF_LIMBS, LENGTH_OF_LIMBS);    // lower left leg

    glPopMatrix();
    glPushMatrix();

    glRotatef(-theta, 0, 0, 1);     //rotate with animation
    glRotatef(0.5 * THETA - 180, 0, 0, 1);
    trunk(RADIUS_OF_LIMBS, LENGTH_OF_LIMBS);    //upper right leg
    
    glTranslatef(0, LENGTH_OF_LIMBS,0);
    glRotatef(PHI, 0, 0, 1);
    glRotatef(-phi, 0, 0, 1);        //rotate with animation
    trunk(RADIUS_OF_LIMBS, LENGTH_OF_LIMBS);    //lower right leg

    glPopMatrix();

    glTranslatef(0, ARM_POSITION, 0);   //move drawing position
    
    glPushMatrix();
    
    glRotatef(theta, 0, 0, 1);      //rotate with animation
    glRotatef(90 + 0.5 * THETA, 0, 0, 1);
    trunk(RADIUS_OF_LIMBS, LENGTH_OF_LIMBS);    //upper left arm
    
    glTranslatef(0, LENGTH_OF_LIMBS, 0);
    glRotatef(-0.5*PHI, 0, 0, 1);
    glRotatef(-0.3*alpha, 0, 0, 1);       //rotate with animation
    trunk(RADIUS_OF_LIMBS, LENGTH_OF_LIMBS);    //lower left arm

    glPopMatrix();
    glPushMatrix();

    glRotatef(-theta, 0, 0, 1);      //rotate with animation
    glRotatef(-90 - 0.5 * THETA, 0, 0, 1);
    trunk(RADIUS_OF_LIMBS, LENGTH_OF_LIMBS);    //upper right arm
    
    glTranslatef(0, LENGTH_OF_LIMBS, 0);
    glRotatef(-0.5*PHI, 0, 0, 1);
    glRotatef(-0.3*alpha, 0, 0, 1);
    trunk(RADIUS_OF_LIMBS, LENGTH_OF_LIMBS);    //lower right arm

	glPopMatrix();
	
	glPopMatrix();
}

void draw_character_with_shadow(){
	init_material();
	init_lighting();
	draw_character();		// draw the main character
	double delta = 0.5;		// add a small margin between shadows and planes to avoid flickering shadows
	
	GLfloat M[16], N[16], Q[16];
	for (int i = 0; i < 16; i++){
		M[i] = 0;
		N[i] = 0;
		Q[i] = 0;
	}
	M[0] = M[5] = M[10] = 1;	// shadow in X-Z plane
	N[0] = N[5] = N[10] = 1;	// shadow in X-Y plane
	Q[0] = Q[5] = Q[10] = 1;	// shadow in Y-Z plane

	// draw the shadow of light 0
	GLfloat shadowColor[] = {0.1, 0.1, 0.1, 0.95};
	
	// for X-Z plane
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(light0_x+delta, light0_y+delta, light0_z+delta);
	M[7] = -1.0/(light0_y - (-100));	// relative distance of y (the ground is set at y = -100)
	glMultMatrixf(M);
	glTranslatef(-light0_x-delta, -light0_y-delta, -light0_z-delta);
	glDisable(GL_LIGHTING);
	glColor3fv(shadowColor);
	draw_character();
	glEnable(GL_LIGHTING);
	glPopMatrix();

	// for X-Y plane
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(light0_x+delta, light0_y+delta, light0_z+delta);
	N[11] = -1.0/(light0_z - (-10));
	glMultMatrixf(N);
	glTranslatef(-light0_x-delta, -light0_y-delta, -light0_z-delta);
	glDisable(GL_LIGHTING);
	glColor3fv(shadowColor);
	draw_character();
	glEnable(GL_LIGHTING);
	glPopMatrix();

	// for Y-Z plane
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(light0_x+delta, light0_y+delta, light0_z+delta);
	Q[3] = -1.0/(light0_x - (-10));
	glMultMatrixf(Q);
	glTranslatef(-light0_x-delta, -light0_y-delta, -light0_z-delta);
	glDisable(GL_LIGHTING);
	glColor3fv(shadowColor);
	draw_character();
	glEnable(GL_LIGHTING);
	glPopMatrix();


	// draw the shadow of light 1

	// for X-Z plane
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(light1_x+delta, light1_y+delta, light1_z+delta);
	M[7] = -1.0/(light1_y - (-100));        // shadows in the X-Z plane
	glMultMatrixf(M);
	glTranslatef(-light1_x-delta, -light1_y-delta, -light1_z-delta);
	glDisable(GL_LIGHTING);
	glColor3fv(shadowColor);
	draw_character();
	glEnable(GL_LIGHTING);
	glPopMatrix();

	// for X-Y plane
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(light1_x+delta, light1_y+delta, light1_z+delta);
	N[11] = -1.0/(light1_z - (-10));
	glMultMatrixf(N);
	glTranslatef(-light1_x-delta, -light1_y-delta, -light1_z-delta);
	glDisable(GL_LIGHTING);
	glColor3fv(shadowColor);
	draw_character();
	glEnable(GL_LIGHTING);
	glPopMatrix();

	// for Y-Z plane
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(light1_x+delta, light1_y+delta, light1_z+delta);
	Q[3] = -1.0/(light1_x - (-10));
	glMultMatrixf(Q);
	glTranslatef(-light1_x-delta, -light1_y-delta, -light1_z-delta);
	glDisable(GL_LIGHTING);
	glColor3fv(shadowColor);
	draw_character();
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

// for animation
void animate() {
	// For legs and arms:
	// theta represents the movement of the whole lumb,
	// phi represents the movement of the outlying part.

	double t;
	double swing_angle_theta = THETA;
	double swing_angle_phi = PHI;
	double swing_angle_alpha = ALPHA;
	double iteration_time = 400.0;
	double moving_time = 3100.0;
	double camera_moving_time = 3000.0;
	double camera_r = 150;
	double camera_theta = 70;
	double camera_phi = -90;
	position camera_track_length;
	position character_track_length;
	camera_track_length.x = 200;
	camera_track_length.y = 80;
	camera_track_length.z = 350;

	character_track_length.x = 220;
	character_track_length.y = 0;
	character_track_length.z = 150;

	double time = glutGet(GLUT_ELAPSED_TIME) - t_prev;		//return elapsed time in ms since the last call
	t = (int) time % (int)(iteration_time);		//make the character continuously run

	if (time < moving_time){
		theta = swing_angle_theta * t / iteration_time;
		phi = swing_angle_phi * t / iteration_time;
		alpha = swing_angle_alpha * t / iteration_time;

		displacement_camera.x = camera_track_length.x * time / moving_time;
		displacement_character.x = character_track_length.x * time / moving_time;
		displacement_camera.y = camera_track_length.y * time / moving_time;
		displacement_character.y = character_track_length.y * time / moving_time;
		displacement_camera.z = camera_track_length.z * time / moving_time;
		displacement_character.z = character_track_length.z * time / moving_time;

		// printf("time = %f\n", time);
		// printf("displacement_character.x = %f\n", displacement_character.x);
		// printf("displacement_camera.z = %f\n", displacement_camera.z);
	}
	else if (time < moving_time + camera_moving_time){
		double tmp = time - moving_time;
		double cos_phi = cos((camera_phi * tmp / camera_moving_time)*PI/180);
		double sin_phi = sin((camera_phi * tmp / camera_moving_time)*PI/180);
		double cos_theta = cos((camera_theta * tmp / camera_moving_time)*PI/180);
		double sin_theta = sin((camera_theta * tmp / camera_moving_time)*PI/180);
		displacement_camera.y = camera_track_length.y + camera_r * cos_phi * sin_theta;
		displacement_camera.z = camera_track_length.z + camera_r * sin_phi;
	}

	glutPostRedisplay();		//make things change
}

void draw_scene(){

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport); // viewport is by default the display window

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (double)(viewport[2] / viewport[3]), 0.1, 1000);

	glMatrixMode(GL_MODELVIEW);			// do the modelview before projection
	glLoadIdentity();
	gluLookAt(200+displacement_camera.x, 40+displacement_camera.y, 500+displacement_camera.z, 0, 0, 0, 0, 1, 0);
	// camera position (in "x, y, z" order): 200~400, 20, 500~850;
	// character position: 0~220, 0, 0~150; 

	glClear(GL_DEPTH_BUFFER_BIT);			// clearly tell the program whether to deect depth
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.8, 0.8, 0.8, 0.0);	// Set display-window color to grey.
	glClear(GL_COLOR_BUFFER_BIT);		// Clear display window.

	textMap();
    draw_character_with_shadow();

	glutSwapBuffers();
}

void init(){
    loadTextureImage(GroundImage, 0);
	loadTextureImage(WallImage_1, 1);
	loadTextureImage(WallImage_2, 2);
	init_positioning();
	init_material();
	init_lighting();
	glEnable(GL_DEPTH_TEST);
}

int main (int argc, char** argv){
	
	glutInit (&argc, argv);			                      		// Initialize GLUT
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);	// Set display mode = double buffering		
	glutInitWindowPosition(WIN_POSX, WIN_POSY);         		// Set display-window position at (WIN_POSX, WIN_POSY) 
																// where (0, 0) is top left corner of monitor screen
	glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);					// Set display-window width and height.

	glutCreateWindow ("Running Man");					  // Create display window.

    init();
	trunk_bottom = gluNewQuadric();
	trunk_middle = gluNewQuadric();
	trunk_top = gluNewQuadric();

	glutDisplayFunc (draw_scene);   // put everything you wish to draw in drawscene
	glutIdleFunc(animate);			//after drawing a frame, having nothing to do, go to the idle function
										//if computer fast, go to idel function frequently
	glutMainLoop();

	return 0;
}