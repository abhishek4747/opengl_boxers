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

#pragma comment(lib, "freeglut")

using namespace std;

node* root;
vector<node*> upperarms;
vector<node*> forearms;
vector<node*> thighs;
vector<node*> forelegs;
vector<node*> fingers;
vector<node*> fings;
vector<node*> bots;
vector<node*> wrists;
vector<node*> elbows;
vector<node*> necks;

size_t bot = 0;
bool keyStates[256];// = new bool[256];
bool keySpecialStates[256]; // = new bool[256]; // Create an array of boolean values of length 256 (0-255) 
bool SHIFT = false, ALT = false, CTRL = false;

bool file_type = true;
bool movingUp = false; // Whether or not we are moving up or down  
float yLocation = 0.0f; // Keep track of our position on the y axis.
float yRotationAngle = 0.0f; // The angle of rotation for our object  


void reshape (int width, int height) {  
	glViewport(0, 0, (GLsizei)width, (GLsizei)height); // Set our viewport to the size of our window  
	glMatrixMode(GL_PROJECTION); // Switch to the projection matrix so that we can manipulate how our scene is viewed  
	glLoadIdentity(); // Reset the projection matrix to the identity matrix so that we don't get any artifacts (cleaning up)  
	gluPerspective(60, (GLfloat)width / (GLfloat)height, .1, 100.0); // Set the Field of view angle (in degrees), the aspect ratio of our window, and the new and far planes  
	glMatrixMode(GL_MODELVIEW); // Switch back to the model view matrix, so that we can start drawing shapes correctly  
}  
void loadBodyParts(){
	if (bots.size()>bot){
		upperarms = bots[bot]->findBodyPart("upperarm");
		forearms = bots[bot]->findBodyPart("elbow");
		elbows = bots[bot]->findBodyPart("forearm");
		fingers = bots[bot]->findBodyPart("finger");
		thighs = bots[bot]->findBodyPart("thigh");
		forelegs = bots[bot]->findBodyPart("foreleg");
		wrists = bots[bot]->findBodyPart("wristrotate");
		necks = bots[bot]->findBodyPart("neckh");
	}else{
		upperarms = root->findBodyPart("upperarm");
		forearms = root->findBodyPart("elbow");
		elbows = root->findBodyPart("forearm");
		fingers = root->findBodyPart("finger");
		thighs = root->findBodyPart("thigh");
		forelegs = root->findBodyPart("foreleg");
		wrists = root->findBodyPart("wristrotate");
		necks = bots[bot]->findBodyPart("neckh");
	}
}

void setRoot(){
	ifstream filein;
	if (file_type) filein.open("node.txt");
	else filein.open("saved_state.txt");
	if (!filein.good()){
		cout<<"\nFile not found!!";
		getchar();
		exit(0);
	}else if(file_type){
		cout<<"node.txt loaded"<<endl;
	}else{
		cout<<"saved_state.txt loaded"<<endl;
	}
	root = node::read_node(filein);
	//root->print();
	filein.close();
	bots = root->findBodyPart("body");
	loadBodyParts();
}

void saveState(){
	ofstream fileout;
	fileout.open("saved_state.txt", fstream::out);
	root->writeNode(fileout,0);
	fileout.close();
}

void punch(size_t u){
	// make fist
	if (upperarms.size()>u){
		fings = upperarms[u]->findBodyPart("finger");
		if (fings.size()>u) {
			thread t1(&node::makeFistAsync, fings, true, 0, 800, 1.0f);		
			t1.detach();
		}
	}

	// pull upperarm and then push
	if (upperarms.size()>u){
		thread t1(&node::angleDecreaseToAsync, upperarms[u], -45.f, 0, 1300,  0.5f);		
		t1.detach();
		thread t2(&node::angleIncreaseToAsync, upperarms[u], 110.f, 1500, 500, 0.5f);		
		t2.detach();
	}

	// rotate elbow
	if (elbows.size()>u){
		if (u==0){
			thread t2(&node::angleIncreaseToAsync, elbows[u], 30.f, 1500, 400, 0.5f);		
			t2.detach();
		}else {
			thread t2(&node::angleDecreaseToAsync, elbows[u], -30.f, 1500, 400, 0.5f);		
			t2.detach();
		}
	}

	// pull forearm and then punch
	if (forearms.size()>u) {
		thread t1(&node::angleIncreaseToAsync, forearms[u], 135.f, 0, 1500,  0.5f);		
		t1.detach();
		
		thread t2(&node::angleDecreaseToAsync, forearms[u], 0.f, 1500, 500, 0.5f);		
		t2.detach();
		
	}

	// rotate wrist
	if (wrists.size()>u) {
		bool left = u==0?true:false;
		thread t1(&node::rotateWristAsync, wrists[u], true, left, 1500, 300);		
		t1.detach();
	}

	// translate bot to opponent 
	if (bots.size()>bot) {
		int nextbot = (bot+1)%(bots.size());
		thread t1(&node::translateBotAsync, bots[bot], bots[nextbot], 1500, 300, 0.01f);		
		t1.detach();
	}
	
	// rotate bot facing opponent
	if (bots.size()>bot) {
		int nextbot = (bot+1)%(bots.size());	
		if (bots[bot]->children.size() >0 && bots[bot]->children.size() >0){
			thread t1(&node::rotateBotAsync, bots[bot]->children[0], bots[nextbot]->children[0], 1500, 300, 0.01f);		
			t1.detach();
		}
	}

	// Impact
	vector<node*> nextBotNecks = bots[(bot+1)%bots.size()]->findBodyPart("neckh");
	if (nextBotNecks.size()){
		thread t1(&node::angleIncreaseToAsync, nextBotNecks[0], 45.f, 1800, 300, 0.01f);
		t1.detach();
	}
}

void releasePunch(size_t u){
	int releaseTime = 800;
	if (upperarms.size()>u){
		fings = upperarms[u]->findBodyPart("finger");
		if (fings.size()>u) {
			thread t1(&node::makeFistAsync, fings, false, 0, releaseTime, 1.0f);		
			t1.detach();
		}
	}

	if (upperarms.size()>u){
		thread t1(&node::angleDecreaseToAsync, upperarms[u], 0.f, 0, releaseTime,  0.5f);		
		t1.detach();
	}

	if (forearms.size()>u) {
		thread t1(&node::angleIncreaseToAsync, forearms[u], 15.f, 0, releaseTime,  0.5f);		
		t1.detach();
	}
	if (elbows.size()>u){
		if (u==0){
			thread t1(&node::angleDecreaseToAsync, elbows[u], 0.f, 0, releaseTime,  0.5f);		
			t1.detach();
		}else{
			thread t1(&node::angleIncreaseToAsync, elbows[u], 0.f, 0, releaseTime,  0.5f);		
			t1.detach();
		}
	}
		

	if (wrists.size()>u) {
		bool left = u==0?true:false;
		thread t1(&node::rotateWristAsync, wrists[u], false, left, 0, releaseTime);		
		t1.detach();
	}

	vector<node*> nextBotNecks = bots[(bot+1)%bots.size()]->findBodyPart("neckh");
	if (nextBotNecks.size()){
		cout<<"HaHA";
		thread t1(&node::angleDecreaseToAsync, nextBotNecks[0], 15.f, 0,releaseTime*2, 0.01f);
		t1.detach();
	}
}

void keyOperations (void) {  
	if (keyStates['q'] || keyStates['Q'] || keyStates[27]){
		exit(0);
	}
	if(keyStates['0']){
		cam->eyez += 0.1f;
	}else if(keyStates['.']){
		cam->eyez -= 0.1f;
	}
	
	if(keyStates['-']){
		cam->eyey += exp(cam->eyey/100.f)/100.f;
	}else if(keyStates['+']){
		cam->eyey -= exp(cam->eyey/100.f)/100.f;
	}


	if (keyStates['1']) { 
		bot = 0;
		loadBodyParts();
	}else if (keyStates['2']) { 
		bot = 1;
		loadBodyParts();
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
	if (keyStates['g']){
		keyStates['g'] = false;
		myTranslatef(3.f,4.f,5.f);
	}

	if (keyStates['s']){
		keyStates['s'] = false;
		startWalking = !startWalking;	
	}

	if (keyStates['p']){
		keyStates['p'] = false;
		punch(0);
	}else if (keyStates['l']){
		keyStates['l'] = false;
		releasePunch(0);
	}
	if (keyStates['u']){
		keyStates['u'] = false;
		punch(1);
	}else if (keyStates['k']){
		keyStates['k'] = false;
		releasePunch(1);
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
} 

void keySpecialOperations(void) {  
	if (keySpecialStates[GLUT_KEY_LEFT]) { // If the left arrow key has been pressed  
		float deg = 1.f;
		float x = cam->eyex;
		float z = cam->eyez;
		cam->eyex = x*cos(degreeToRadian(deg)) - z*sin(degreeToRadian(deg));
		cam->eyez = x*sin(degreeToRadian(deg)) + z*cos(degreeToRadian(deg));
	}
	if (keySpecialStates[GLUT_KEY_RIGHT]) { // If the left arrow key has been pressed  
		float deg = -1.f;
		float x = cam->eyex;
		float z = cam->eyez;
		cam->eyex = x*cos(degreeToRadian(deg)) - z*sin(degreeToRadian(deg));
		cam->eyez = x*sin(degreeToRadian(deg)) + z*cos(degreeToRadian(deg));
	}
	if (keySpecialStates[GLUT_KEY_UP]) { // If the left arrow key has been pressed  
		cam->eyex /= 1.01f;
		cam->eyey /= 1.01f;
		cam->eyez /= 1.01f;
	}
	if (keySpecialStates[GLUT_KEY_DOWN]) { // If the left arrow key has been pressed  
		cam->eyex *= 1.01f;
		cam->eyey *= 1.01f;
		cam->eyez *= 1.01f;
	}

	if (keySpecialStates[GLUT_KEY_F5] || keySpecialStates[GLUT_KEY_F6]){
		if (keySpecialStates[GLUT_KEY_F6]){
			camSet = false;
			keySpecialStates[GLUT_KEY_F6] = false;
		}
		keySpecialStates[GLUT_KEY_F5] = false;
		setRoot();
	}

	if (keySpecialStates[GLUT_KEY_F7]){
		keySpecialStates[GLUT_KEY_F7] = false;
		saveState();
	}else if (keySpecialStates[GLUT_KEY_F8]){
		keySpecialStates[GLUT_KEY_F8] = false;
		file_type = !file_type;
		setRoot();
	}
}

void keyPressed (unsigned char key, int x, int y) {  
	keyStates[key] = true; // Set the state of the current key to pressed 
	if (glutGetModifiers() & GLUT_ACTIVE_SHIFT){
		SHIFT = true;
	}else {
		SHIFT = false;
	}
	if (glutGetModifiers() & GLUT_ACTIVE_CTRL){
		CTRL = true;
	}else {
		CTRL = false;
	}
	if (glutGetModifiers() & GLUT_ACTIVE_ALT){
		ALT = true;
	}else {
		ALT = false;
	}
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
    //g5lBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //set the blend function
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
	glutInitWindowSize (1350, 690); // Set the width and height of the window  
	glutInitWindowPosition (0, 0); // Set the position of the window  
	glutCreateWindow ("Graphics Assignment-1 : Boxing match"); // Set the title for the window  
	
	glEnable(GL_DEPTH_TEST);
	glShadeModel (GL_SMOOTH); 

	glutDisplayFunc(display); // Tell GLUT to use the method "display" for rendering  
	glutIdleFunc(display); // Tell GLUT to use the method "display" as our idle method as well  
	glutReshapeFunc(reshape); // Tell GLUT to use the method "reshape" for reshaping  
	
	glutKeyboardFunc(keyPressed); // Tell GLUT to use the method "keyPressed" for key presses  
	glutKeyboardUpFunc(keyUp); // Tell GLUT to use the method "keyUp" for key up events    
	
	glutSpecialFunc(keySpecial); // Tell GLUT to use the method "keySpecial" for special key presses  
	glutSpecialUpFunc(keySpecialUp); // Tell GLUT to use the method "keySpecialUp" for special up key events  
	
	glutMainLoop(); // Enter GLUT's main loop 
}
