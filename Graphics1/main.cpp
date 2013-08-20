#include <iostream>
#include <graphics\glew.h>
#include <graphics\glut.h>
#include <gl\GLU.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "camera.h"
#pragma comment(lib, "freeglut")
using namespace std;

string trim(const string& str, const string& whitespace = " \t"){
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == string::npos) return ""; 
    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;
    return str.substr(strBegin, strRange);
}

bool camSet = false;
camera *cam = new camera();
class node{
private:
	
public:
	string name;
	vector<node*> children;
	string shape;
	vector<float> size_specs;
	float tx, ty, tz, angle, rx, ry, rz;
	node* parent;

	node(string name, vector<node*> children, string shape, vector<float> size_specs, float tx, float ty, float tz, float angle, float rx, float ry, float rz){
		this->name = name;
		this->children = children;
		this->shape = shape;
		this->size_specs = size_specs;
		this->tx = tx, this->ty = ty, this->tz = tz, this->angle = angle, this->rx = rx, this->ry = ry, this->rz = rz;
		for (size_t i = 0, len = children.size(); i < len; ++i){
			 (*children[i]).parent = this;
		}
		this->parent = NULL;
		if (shape=="sphere"){

		}else if(shape=="cylinder"){
			
		}else{
		
		}
	}

	static node* read_node(ifstream &filehandle){
		if (filehandle.is_open()){
			node* n;
			string line;
			getline(filehandle, line);
			line = trim(line);
			stringstream stream(line);
			string name, shape;
			int no_of_children;
			float tx, ty, tz, angle, rx, ry, rz;
			vector<float> size_specs;
			if(stream >> name >> shape >> no_of_children >> tx >> ty >> tz >> angle >> rx >> ry >> rz) {
				// cout<<endl<<name<<"-"<<shape<<"-"<<no_of_children<<"-"<<tx<<"-"<<ty<<"-"<<tz<<"-"<<angle<<"-"<<rx<<"-"<<ry<<"-"<<rz;
				float s;
				while(stream >> s) size_specs.push_back(s);
			}else{
				cout<<"File Format is not Ok!! "<<name;
				getchar();
			}
			
			vector<node*> children;
			for (int i = 0; i < no_of_children; i++){
				children.push_back(read_node(filehandle));
			}

			n = new node(name, children, shape, size_specs, tx, ty, tz, angle, rx, ry, rz);
			return n;
		}else{
			cout<<"File handle not open!!";
			exit(0);
		}
	}
	
	void drawSphere(float x, float y, float z, float radius, int slices=32, int stacks=32){
		glutSolidSphere(radius, slices, stacks);
		//glutWireSphere(double radius, int slices, int stacks);  
	}

	void drawCylinder(float x, float y, float z, float radius, float height, int slices=32, int stacks=32){
		GLUquadricObj *quadObj = gluNewQuadric();
		gluCylinder(quadObj, radius , radius, height, slices, stacks);
		glRotatef(180, 1,0,0); 
		gluDisk(quadObj, 0.0f, radius, slices, 1); 
		glRotatef(180, 1,0,0); 
		glTranslatef(0.0f, 0.0f, height); 
		gluDisk(quadObj, 0.0f, radius, slices, 1); 
		glTranslatef(0.0f, 0.0f, -height);
	}

	void draw(){
		if (this->name=="camera" || this->shape=="camera"){
			if (!camSet) cam = new camera(this->tx, this->ty, this->tz, this->rx, this->ry, this->rz);
			camSet = true;
		}else{
			glPushMatrix();
			glTranslatef(this->tx,this->ty,this->tz);
			glRotatef(this->angle, this->rx, this->ry, this->rz);
			if(this->shape=="sphere"){
				if (this->size_specs.size()){
					if (this->name=="head") glColor4f(1.f,1.f,1.f,0.5f);
					drawSphere(this->tx, this->ty, this->tz, this->size_specs[0]);
				}else{
					cout<<"size specs of the "<<this->shape<<" "<<this->name<<" not available!!"<< endl;
					getchar();
					exit(0);
				}
			}else if(this->shape=="cylinder"){			
				if (this->size_specs.size()>1){
					if (this->name=="torso") glColor4f(0.f,0.f,1.f,0.5f);
					if (this->name=="thigh" || this->name=="foreleg") glColor4f(1.f,1.f,0.f,0.5f);
					if (this->name=="foot") glColor4f(.6f,.3f,.6f,.5f);
					if (this->name=="upperarm" || this->name=="forearm") glColor4f(0.f,1.f,0.f,0.5f);
					if ( this->name=="palm" ) glColor4f(1.f,.9f,3.f,1.f);
					if (this->name=="finger") glColor4f(.2f,.2f,.9f,1.f);
					drawCylinder(this->tx, this->ty, this->tz, this->size_specs[0], this->size_specs[1]);
				}else{
					cout<<"size specs of the "<<this->shape<<" "<<this->name<<" not available!!"<< endl;
					getchar();
					exit(0);
				}
			}
		}
		for (size_t i = 0; i < this->children.size(); i++){
			this->children[i]->draw();
		}
		if (!(this->name=="camera" || this->shape=="camera")){
			glRotatef(-this->angle, this->rx, this->ry, this->rz);
			glTranslatef(-this->tx, -this->ty, -this->tz);
			glPopMatrix();
		}
	}

	vector<node*> findHands(){
		vector<node*> h;
		if (this->name=="upperarm") h.push_back(this);
		for (size_t i = 0; i < this->children.size(); i++){
			vector<node*> temp = this->children[i]->findHands();
			for (size_t j = 0; j < temp.size(); j++) h.push_back(temp[j]);
		}
		return h;
	}

	void print(const int tabs = 0){
		node root = *this;
		int t = tabs;
		while (t--) cout<<"-- ";
		cout<<root.name<<"-"<<root.shape<<"-"<<root.children.size()<<"-"<<root.tx<<"-"<<root.ty<<"-"<<root.tz<<"-"<<root.angle<<"-"<<root.rx<<"-"<<root.ry<<"-"<<root.rz<<endl;
		for (size_t i = 0, len = root.children.size(); i < len; ++i) root.children[i]->print(tabs+1);
	}
};

node* root;
vector<node*> hands;
bool keyStates[256];// = new bool[256];
bool keySpecialStates[256]; // = new bool[256]; // Create an array of boolean values of length 256 (0-255) 
bool movingUp = false; // Whether or not we are moving up or down  
float yLocation = 0.0f; // Keep track of our position on the y axis.
float yRotationAngle = 0.0f; // The angle of rotation for our object  
GLfloat angle = 0.0;

void renderPrimitive (void) {  
	glColor3f(0.0f, 0.0f, 1.0f); // Set the colour of the square to blue
	glBegin(GL_QUADS); // Start drawing a quad primitive  
		glVertex3f(-1.0f, -1.0f, 0.0f); // The bottom left corner  
		glColor3f(0.0f, 1.0f, 1.0f); // Set the colour of the square to blue
		glVertex3f(-1.0f, 1.0f, 0.0f); // The top left corner  
		glColor3f(1.0f, 0.0f, 1.0f); // Set the colour of the square to blue
		glVertex3f(1.0f, 1.0f, 0.0f); // The top right corner  
		glColor3f(1.0f, 1.0f, 0.0f); // Set the colour of the square to blue
		glVertex3f(1.0f, -1.0f, 0.0f); // The bottom right corner  
		glColor3f(0.5f, 0.5f, 1.0f); // Set the colour of the square to blue
	glEnd();  
}

//void display2 (void) {  
//	keyOperations();  
//	glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // Clear the background of our window to red  
//	glClear(GL_COLOR_BUFFER_BIT); //Clear the colour buffer (more buffers later on)
//	glEnable(GL_BLEND); //enable the blending
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //set the blend function
//	glLoadIdentity(); // Load the Identity Matrix to reset our drawing locations  
//	glTranslatef(0.0f, 0.0f, -5.0f); // Push eveything 5 units back into the scene, otherwise we won't see the primitive  
//	glTranslatef(0.0f, yLocation, 0.0f); // Translate our object along the y axis  
//	glRotatef(yRotationAngle, 0.0f, 1.0f, 0.0f); // Rotate our object around the y axis  
//	//renderPrimitive(); // Render the primitive  
//	glutWireCube(2.0f); // Render the primitive  
//	glutSwapBuffers();
//	glFlush(); // Flush the OpenGL buffers to the window   
//	if (movingUp) // If we are moving up  
//		yLocation -= 0.005f; // Move up along our yLocation  
//	else  // Otherwise  
//		yLocation += 0.005f; // Move down along our yLocation  
//
//	if (yLocation < -3.0f) // If we have gone up too far  
//		movingUp = false; // Reverse our direction so we are moving down  
//	else if (yLocation > 3.0f) // Else if we have gone down too far  
//		movingUp = true; // Reverse our direction so we are moving up
//
//	yRotationAngle += 0.005f; // Increment our rotation value 
//	if (yRotationAngle > 360.0f) // If we have rotated beyond 360 degrees (a full rotation)  
//		yRotationAngle -= 360.0f; // Subtract 360 degrees off of our rotation  
//}  

void cube (void) {
     //scaled
    glScalef( 2.0, 0.5, 1.0 );  //twice as wide, half the height, same depth
    glRotatef(angle, 1.0, 0.0, 0.0);
    glRotatef(angle, 0.0, 1.0, 0.0);
    glRotatef(angle, 0.0, 0.0, 1.0);
    glColor4f(1.0, 0.0, 0.0, 0.25); //25% visible
    glutWireCube(2);
    //non scaled
    glRotatef(angle, 1.0, 0.0, 0.0);
    glRotatef(angle, 0.0, 1.0, 0.0);
    glRotatef(angle, 0.0, 0.0, 1.0);
    glColor4f(0.0, 1.0, 0.0, 0.25); //25% visible
    glutSolidCube(1);
}

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
	hands = root->findHands();
}

void keyOperations (void) {  
	if (keyStates['q']|| keyStates['Q']){
		exit(0);
	}
	if(keyStates['[']){
		cam->eyez += 0.1f;
	}else if(keyStates[']']){
		cam->eyez -= 0.1f;
	} 
	
	if(keyStates['r'] || keyStates['R']){
		if (keyStates['R']) camSet = false;
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

	if(keyStates['i']){
		size_t h = 0;
		if (hands.size()>h){
			hands[h]->angle += 1.0f;
			if (hands[h]->angle>360.0f)	hands[h]->angle = 0.0f;
		}
	} 
	if(keyStates['o']){
		size_t h = 1;
		if (hands.size()>h){
			hands[h]->angle += 1.0f;
			if (hands[h]->angle>360.0f)	hands[h]->angle = 0.0f;
		}
	}
} 

void keySpecialOperations(void) {  
	if (keySpecialStates[GLUT_KEY_LEFT]) { // If the left arrow key has been pressed  
		cam->eyex -= 0.1f;
	}else if (keySpecialStates[GLUT_KEY_RIGHT]) { // If the left arrow key has been pressed  
		cam->eyex += 0.1f;
	}else if (keySpecialStates[GLUT_KEY_UP]) { // If the left arrow key has been pressed  
		cam->eyey += 0.1f;
	}else if (keySpecialStates[GLUT_KEY_DOWN]) { // If the left arrow key has been pressed  
		cam->eyey -= 0.1f;
	}
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
    glClear (GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
    //glEnable(GL_BLEND); //enable the blending
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //set the blend function
    glLoadIdentity();  
	gluLookAt (cam->eyex, cam->eyey, cam->eyez, cam->centerx, cam->centery, cam->centerz, cam->upx, cam->upy, cam->upz);
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINES);
	for (GLfloat i = -2.5; i <= 2.5; i += 0.25) {
		glVertex3f(i, 0, 2.5); glVertex3f(i, 0, -2.5);
		glVertex3f(2.5, 0, i); glVertex3f(-2.5, 0, i);
	}
	glEnd();
    //cube();
	root->draw();
	glPopMatrix();
	glFlush();
	//GLUquadricObj *quadObj = gluNewQuadric();
	//glRotatef(90.0f, 1.0f, 1.0f, 0.0f);
	//gluCylinder(quadObj, 0.1f , 0.1f, 3.0f, 32, 32);
    //glutSwapBuffers();
    // angle ++;
}

int main (int argc, char **argv) {	
	setRoot();
	fill_n( keyStates, 256, false);
	fill_n( keySpecialStates, 256, false);

	glutInit(&argc, argv); // Initialize GLUT  
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGBA); // Set up a basic display buffer (only single buffered for now)  
	glutInitWindowSize (600, 600); // Set the width and height of the window  
	glutInitWindowPosition (500, 50); // Set the position of the window  
	glutCreateWindow ("Your first OpenGL Window"); // Set the title for the window  

	glutDisplayFunc(display); // Tell GLUT to use the method "display" for rendering  
	glutIdleFunc(display); // Tell GLUT to use the method "display" as our idle method as well  
	glutReshapeFunc(reshape); // Tell GLUT to use the method "reshape" for reshaping  
	
	glutKeyboardFunc(keyPressed); // Tell GLUT to use the method "keyPressed" for key presses  
	glutKeyboardUpFunc(keyUp); // Tell GLUT to use the method "keyUp" for key up events    
	
	glutSpecialFunc(keySpecial); // Tell GLUT to use the method "keySpecial" for special key presses  
	glutSpecialUpFunc(keySpecialUp); // Tell GLUT to use the method "keySpecialUp" for special up key events  
	
	glutMainLoop(); // Enter GLUT's main loop 
}
