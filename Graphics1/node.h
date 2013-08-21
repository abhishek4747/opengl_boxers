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
#include "camera.h"
#include "functions.h"

using namespace std;
camera *cam = new camera();

bool camSet = false;
bool startWalking = false;

class node{
public:
	string name, shape;
	vector<node*> children;
	vector<float> size_specs;
	float tx, ty, tz, angle, rx, ry, rz;
	node* parent;

	node(string name, vector<node*> children, string shape, vector<float> size_specs, float tx, float ty, float tz, float angle, float rx, float ry, float rz){
		this->name = name, this->children = children, this->shape = shape, this->size_specs = size_specs;
		this->tx = tx, this->ty = ty, this->tz = tz, this->angle = angle, this->rx = rx, this->ry = ry, this->rz = rz;
		for (size_t i = 0, len = children.size(); i < len; ++i){
			 (*children[i]).parent = this;
		}
		this->parent = NULL;
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
				exit(0);
			}
			
			vector<node*> children;
			for (int i = 0; i < no_of_children; i++){
				children.push_back(read_node(filehandle));
			}

			n = new node(name, children, shape, size_specs, tx, ty, tz, angle, rx, ry, rz);
			return n;
		}else{
			cout<<"File handle not open!!";
			getchar();
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
		
		// For covering top and bottom
		glRotatef(180, 1,0,0); 
		gluDisk(quadObj, 0.0f, radius, slices, 1); 
		glRotatef(180, 1,0,0); 
		glTranslatef(0.0f, 0.0f, height); 
		gluDisk(quadObj, 0.0f, radius, slices, 1); 
		glTranslatef(0.0f, 0.0f, -height);
	}

	void draw(){
		if (this->name=="torso") glColor4f(0.f,0.f,1.f,0.5f);
		if (this->name=="thigh" || this->name=="foreleg") glColor4f(1.f,1.f,0.f,0.5f);
		if (this->name=="foot") glColor4f(.6f,.3f,.6f,.5f);
		if (this->name=="upperarm" || this->name=="forearm") glColor4f(0.f,1.f,0.f,0.5f);
		if (this->name=="palm" ) glColor4f(1.f,.9f,3.f,1.f);
		if (this->name=="finger") glColor4f(.2f,.2f,.9f,1.f);
		if (this->name=="head") glColor4f(1.f,1.f,1.f,0.5f);
		if (this->name=="eye") glColor4f(.4f, .9f, .1f, 0.5f);
		if (this->name=="pupil") glColor4f(0.f, 0.f, 0.f, 1.f);
		if (this->name=="nose") glColor4f(1.f, 0.f, 0.f, 1.f);
		if (this->name=="mouth") glColor4f(1.f, 0.f, 0.f, 1.f);


		if (this->name=="camera" || this->shape=="camera"){
			if (!camSet) cam = new camera(this->tx, this->ty, this->tz, this->rx, this->ry, this->rz);
			camSet = true;
		}else{
			glPushMatrix();
			glTranslatef(this->tx,this->ty,this->tz);
			glRotatef(this->angle, this->rx, this->ry, this->rz);
			if(this->shape=="sphere"){
				if (this->size_specs.size()){
					drawSphere(this->tx, this->ty, this->tz, this->size_specs[0]);
				}else{
					cout<<"size specs of the "<<this->shape<<" "<<this->name<<" not available!!"<< endl;
					getchar();
					exit(0);
				}
			}else if(this->shape=="cylinder"){			
				if (this->size_specs.size()>1){
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

	vector<node*> findBodyPart(string part){
		vector<node*> h;
		if (this->name==part) h.push_back(this);
		for (size_t i = 0; i < this->children.size(); i++){
			vector<node*> temp = this->children[i]->findBodyPart(part);
			for (size_t j = 0; j < temp.size(); j++) h.push_back(temp[j]);
		}
		return h;
	}

	void static angleIncreaseToAsync(node* n, float ang, int waittime = 0, int ms = 2000 , float scale = 0.01f){
		Sleep(waittime);
		if (n->angle<ang){
			float dist = (ang - n->angle)/scale;
			int speed = (int) (ms/dist);
			while(n->angle<ang) {
				n->angle += scale;
				Sleep(speed);
			}
		}
	}

	void static makeFistAsync(vector<node*> fings, bool close = true, int waittime = 0, int ms = 2000, float scale = 1.0f){
		size_t len = fings.size();
		if (len){
			if (close){
				float dist = (90.f - fings[0]->angle)/scale;
				int speed = (int) (ms/dist);
				while (fings[0]->angle<90.f) {
					for (size_t i = 0; i < fings.size(); i++) {
						fings[i]->angle += scale;
					}
					Sleep(speed);
				}
			}else{
				float dist = (fings[0]->angle- 0.f)/scale;
				int speed = (int) (ms/dist);				
				while (fings[0]->angle>0.f) {
					for (size_t i = 0; i < fings.size(); i++) {
						fings[i]->angle -= scale;
					}
					Sleep(speed);
				}
			}
		}
	}

	void static rotateWristAsync(node* wrist, bool down = true, bool left = true, int waittime = 0, int ms = 2000){
		float scale = 1.f;
		Sleep(waittime);
		if (down && left){
			float dist = (180.f - wrist->angle)/scale;
			int speed = (int) (ms/dist);
			while (wrist->angle<180.f) {
					wrist->angle += scale;
				Sleep(speed);
			}
		}else if(!down && left){
			float dist = (wrist->angle- 0.f)/scale;
			int speed = (int) (ms/dist);				
			while (wrist->angle>0.f) {
				wrist->angle -= scale;
				Sleep(speed);
			}
		}else if (down && !left){
			float dist = (wrist->angle + 180.0f)/scale;
			int speed = (int) (ms/dist);
			while (wrist->angle>-180.f) {
					wrist->angle -= scale;
				Sleep(speed);
			}
		}else{
			cout<<"hreer"<<wrist->angle;
			float dist = (0.f - wrist->angle)/scale;
			int speed = (int) (ms/dist);				
			while (wrist->angle<0.f) {
				wrist->angle += scale;
				Sleep(speed);
			}
		}
	}
	
	void static angleDecreaseToAsync(node* n, float ang, /*bool x, bool y, bool z,*/ int waittime = 0, int ms = 2000 , float scale = 0.01f){
		Sleep(waittime);
		if (n->angle>ang){
			/*n->rx = x?1:0;
			n->ry = y?1:0;
			n->rz = z?1:0;*/
			float dist = (n->angle - ang)/scale;
			int speed = (int) (ms/dist);
			while(n->angle>ang) {
				n->angle -= scale;
				Sleep(speed);
			}
		}
	}

	

	void print(const int tabs = 0){
		node root = *this;
		int t = tabs;
		while (t--) cout<<"-- ";
		cout<<root.name<<"-"<<root.shape<<"-"<<root.children.size()<<"-"<<root.tx<<"-"<<root.ty<<"-"<<root.tz<<"-"<<root.angle<<"-"<<root.rx<<"-"<<root.ry<<"-"<<root.rz<<endl;
		for (size_t i = 0, len = root.children.size(); i < len; ++i) root.children[i]->print(tabs+1);
	}
};