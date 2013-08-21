#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <thread>
#include <graphics\glew.h>
#include <graphics\glut.h>
#include <gl\GLU.h>
#include <Windows.h>
#include <math.h>
#include "node.h"

/** Pending Tasks
  * 1. Make opponent face opponent
  * 2. Make Eyes
  * 3. Make Nose
  * 4. Make a Smile and Angry face
  * 5. Make Hair
  * 6. Make a Hit
  * 7. Color them appropriately, Resize them appropriately
  * 8. Walk
  * 9. Reset arm from any configuration
  * 10. Write glTransformertyui
  * 11. Write glRotate
  * 12. Mouse Navigation
  * 13. Write Shortcuts on screen
  * 14. Create a boxing Arena and Maximize screen
  * 15. Kick
  * 16. Palm
  * 17. Save State
 */


#pragma comment(lib, "freeglut")

using namespace std;

node* root;
vector<node*> upperarms;
vector<node*> forearms;
vector<node*> thighs;
vector<node*> forelegs;
vector<node*> fingers;
vector<node*> fings;

bool keyStates[256];// = new bool[256];
bool keySpecialStates[256]; // = new bool[256]; // Create an array of boolean values of length 256 (0-255) 

bool movingUp = false; // Whether or not we are moving up or down  
float yLocation = 0.0f; // Keep track of our position on the y axis.
float yRotationAngle = 0.0f; // The angle of rotation for our object  

void reshape (int width, int height) {  
	glViewport(0, 0, (GLsizei)width, (GLsizei)height); // Set our viewport to the size of our window  
	glMatrixMode(GL_PROJECTION); // Switch to the projection matrix so that we can manipulate how our scene is viewed  
	glLoadIdentity(); // Reset the projection matrix to the identity matrix so that we don't get any artifacts (cleaning up)  
	gluPerspective(60, (GLfloat)width / (GLfloat)height, 1.0, 100.0); // Set the Field of view angle (in degrees), the aspect ratio of our window, and the new and far planes  
	glMatrixMode(GL_MODELVIEW); // Switch back to the model view matrix, so that we can start drawing shapes correctly  
}  

void setRoot(){
	ifstream filein;
	filein.open("input2.txt");
	if (!filein.good()){
		cout<<"\nFile not found!!";
		getchar();
		exit(0);
	}
	root = node::read_node(filein);
	//root->print();
	filein.close();
	upperarms = root->findBodyPart("upperarm");
	forearms = root->findBodyPart("forearm");
	fingers = root->findBodyPart("finger");
	thighs = root->findBodyPart("thigh");
	forelegs = root->findBodyPart("foreleg");
}

void keyOperations (void) {  
	if (keyStates['q']|| keyStates['Q']){
		exit(0);
	}
	if(keyStates['0']){
		cam->eyez += 0.1f;
	}else if(keyStates['.']){
		cam->eyez -= 0.1f;
	}
	
	if(keyStates['r'] || keyStates['R'] || keyStates['5']){
		if (keyStates['R'] || keyStates['5']) camSet = false;
		keyStates['r'] = keyStates['R'] = false;
		setRoot();
	}

	if(keyStates['-']){
		cam->eyex *= 1.01f;
		cam->eyey *= 1.01f;
		cam->eyez *= 1.01f;
	}else if(keyStates['+']){
		cam->eyex /= 1.01f;
		cam->eyey /= 1.01f;
		cam->eyez /= 1.01f;
	}

	if(keyStates['4']){
		float deg = 1.f;
		float x = cam->eyex;
		float z = cam->eyez;
		cam->eyex = x*cos(degreeToRadian(deg)) - z*sin(degreeToRadian(deg));
		cam->eyez = x*sin(degreeToRadian(deg)) + z*cos(degreeToRadian(deg));
	}else if(keyStates['6']){
		float deg = -1.f;
		float x = cam->eyex;
		float z = cam->eyez;
		cam->eyex = x*cos(degreeToRadian(deg)) - z*sin(degreeToRadian(deg));
		cam->eyez = x*sin(degreeToRadian(deg)) + z*cos(degreeToRadian(deg));
	}
	if(keyStates['8']){
		cam->eyey += exp(cam->eyey/100.f)/100.f;
	}else if (keyStates['2']){
		cam->eyey -= exp(cam->eyey/100.f)/100.f;
	}
	if (keyStates['1']) { 
		cam->eyex -= 0.1f;
	}else if (keyStates['3']) { 
		cam->eyex += 0.1f;
	}

	if(keyStates['o']){
		size_t h = 0;
		if (upperarms.size()>h){
			upperarms[h]->angle += 1.0f;
			if (upperarms[h]->angle>360.0f)	upperarms[h]->angle = 0.0f;
		}
	} 
	if(keyStates['i']){
		size_t h = 1;
		if (upperarms.size()>h){
			upperarms[h]->angle += 1.0f;
			if (upperarms[h]->angle>360.0f)	upperarms[h]->angle = 0.0f;
		}
	}

	if (keyStates['h']){
		keyStates['h'] = false;
		cout<<cam->eyex<<" "<<cam->eyey<<" "<<cam->eyez<<endl; 
	}

	if (keyStates['s']){
		keyStates['s'] = false;
		startWalking = !startWalking;
		
	}
	if (keyStates['u']){
		keyStates['u'] = false;
		size_t u = 1;
		if (upperarms.size()>u){
			fings = upperarms[u]->findBodyPart("finger");
			if (fings.size()>u) {
				thread t1(&node::makeFistAsync, fings, true, 0, 800, 1.0f);		
				t1.detach();
			}
		}

		if (upperarms.size()>u){
			thread t1(&node::angleDecreaseToAsync, upperarms[u], -45.f, 0, 1500,  0.5f);		
			t1.detach();
			thread t2(&node::angleIncreaseToAsync, upperarms[u], 85.f, 1500, 500, 0.5f);		
			t2.detach();
		}

		if (forearms.size()>u) {
			thread t1(&node::angleIncreaseToAsync, forearms[u], 135.f, 0, 1500,  0.5f);		
			t1.detach();
			thread t2(&node::angleDecreaseToAsync, forearms[u], 0.f, 1500, 500, 0.5f);		
			t2.detach();
		}
	}

	if (keyStates['j']){
		keyStates['j']=false;
		size_t u = 1;
		if (upperarms.size()>u){
			fings = upperarms[u]->findBodyPart("finger");
			if (fings.size()>u) {
				thread t1(&node::makeFistAsync, fings, false, 0, 800, 1.0f);		
				t1.detach();
			}
		}
	}

	if (keyStates[';']){
		keyStates[';']=false;
		size_t u = 0;
		if (upperarms.size()>u){
			fings = upperarms[u]->findBodyPart("finger");
			if (fings.size()>u) {
				thread t1(&node::makeFistAsync, fings, false, 0, 800, 1.0f);		
				t1.detach();
			}
		}
	}

	if (keyStates['m']){
		keyStates['m']=false;
		size_t u = 1;
		if (upperarms.size()>u){
			fings = upperarms[u]->findBodyPart("finger");
			if (fings.size()>u) {
				thread t1(&node::makeFistAsync, fings, true, 0, 800, 1.0f);		
				t1.detach();
			}
		}
	}

	if (keyStates['/']){
		keyStates['/']=false;
		size_t u = 0;
		if (upperarms.size()>u){
			fings = upperarms[u]->findBodyPart("finger");
			if (fings.size()>u) {
				thread t1(&node::makeFistAsync, fings, true, 0, 800, 1.0f);		
				t1.detach();
			}
		}
	}

	if (keyStates['p']){
		keyStates['p'] = false;
		size_t u = 0;
		if (upperarms.size()>u){
			fings = upperarms[u]->findBodyPart("finger");
			if (fings.size()>u) {
				thread t1(&node::makeFistAsync, fings, true, 0, 800, 1.0f);		
				t1.detach();
			}
		}

		if (upperarms.size()>u){
			thread t1(&node::angleDecreaseToAsync, upperarms[u], -45.f, 0, 1500,  0.5f);		
			t1.detach();
			thread t2(&node::angleIncreaseToAsync, upperarms[u], 85.f, 1500, 500, 0.5f);		
			t2.detach();
		}

		if (forearms.size()>u) {
			thread t1(&node::angleIncreaseToAsync, forearms[u], 135.f, 0, 1500,  0.5f);		
			t1.detach();
			thread t2(&node::angleDecreaseToAsync, forearms[u], 0.f, 1500, 500, 0.5f);		
			t2.detach();
		}
	}
} 

void keySpecialOperations(void) {  
	//if (keySpecialStates[GLUT_KEY_LEFT]) { // If the left arrow key has been pressed  
	//	cam->eyex -= 0.1f;
	//}else if (keySpecialStates[GLUT_KEY_RIGHT]) { // If the left arrow key has been pressed  
	//	cam->eyex += 0.1f;
	//}else if (keySpecialStates[GLUT_KEY_UP]) { // If the left arrow key has been pressed  
	//	cam->eyey += 0.1f;
	//}else if (keySpecialStates[GLUT_KEY_DOWN]) { // If the left arrow key has been pressed  
	//	cam->eyey -= 0.1f;
	//}
}

void keyPressed (unsigned char key, int x, int y) {  
	keyStates[key] = true; // Set the state of the current key to pressed 
}  

void keyUp (unsigned char key, int x, int y) {  
	keyStates[key] = false; // Set the state of the current key to not pressed  
}

void keySpecial (int key, int x, int y) {
	keySpecialStates[key] = true;
} 

void keySpecialUp (int key, int x, int y) {  
	keySpecialStates[key] = false;
}

void display (void) {
	keyOperations();
	keySpecialOperations();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
    //glEnable(GL_BLEND); //enable the blending
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //set the blend function
    glLoadIdentity();  
	gluLookAt (cam->eyex, cam->eyey, cam->eyez, cam->centerx, cam->centery, cam->centerz, cam->upx, cam->upy, cam->upz);
	// A white colored mesh 
	/*glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINES);
	for (GLfloat i = -2.5; i <= 2.5; i += 0.25) {
		glVertex3f(i, 0, 2.5); glVertex3f(i, 0, -2.5);
		glVertex3f(2.5, 0, i); glVertex3f(-2.5, 0, i);
	}
	glEnd();*/
	root->draw();
	glPopMatrix();
	glFlush();
}

int main (int argc, char **argv) {	
	setRoot();
	fill_n( keyStates, 256, false);
	fill_n( keySpecialStates, 256, false);

	glutInit(&argc, argv); // Initialize GLUT  
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH); // Set up a basic display buffer (only single buffered for now)  
	glutInitWindowSize (600, 690); // Set the width and height of the window  
	glutInitWindowPosition (750, 0); // Set the position of the window  
	glutCreateWindow ("Graphics Assignment-1 : Boxing match"); // Set the title for the window  
	
	glEnable(GL_DEPTH_TEST);
	glutDisplayFunc(display); // Tell GLUT to use the method "display" for rendering  
	glutIdleFunc(display); // Tell GLUT to use the method "display" as our idle method as well  
	glutReshapeFunc(reshape); // Tell GLUT to use the method "reshape" for reshaping  
	
	glutKeyboardFunc(keyPressed); // Tell GLUT to use the method "keyPressed" for key presses  
	glutKeyboardUpFunc(keyUp); // Tell GLUT to use the method "keyUp" for key up events    
	
	glutSpecialFunc(keySpecial); // Tell GLUT to use the method "keySpecial" for special key presses  
	glutSpecialUpFunc(keySpecialUp); // Tell GLUT to use the method "keySpecialUp" for special up key events  
	
	glutMainLoop(); // Enter GLUT's main loop 
}
