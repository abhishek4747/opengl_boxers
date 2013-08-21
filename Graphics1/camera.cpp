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

string trim(const string &str, const string &whitespace ){
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == string::npos) return ""; 
    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;
    return str.substr(strBegin, strRange);
}