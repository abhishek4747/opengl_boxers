#include "camera.h"

camera::camera(float eyex, float eyey, float eyez, float centerx, float centery, float centerz, float upx, float upy, float upz){
	this->eyex = eyex;
	this->eyey = eyey;
	this->eyez = eyez;
	this->centerx = centerx;
	this->centery = centery;
	this->centerz = centerz;
	this->upx = upx;
	this->upy = upy;
	this->upz = upz;
}

camera::camera(){
	this->eyex = this->eyey = this->eyez = this->centerx = this->centery = this->centerz = this->upx = this->upy = this->upz = 0.0f;
}