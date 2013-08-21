#include <string>
using namespace std;

class camera{
public:
	float eyex, eyey, eyez;
	float centerx, centery, centerz;
	float upx, upy, upz;
	camera(float eyex, float eyey, float eyez, float centerx, float centery, float centerz, float upx = 0.0f, float upy = 1.0f, float upz = 0.0f);
	camera();
};

string trim(const string &str, const string &whitespace = " \t");