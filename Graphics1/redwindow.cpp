#include <iostream>
#include <graphics\glew.h>
#include <graphics\glut.h>
#pragma comment(lib, "freeglut")

using namespace std;

void display (void) {  
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // Clear the background of our window to red  
	glClear(GL_COLOR_BUFFER_BIT); //Clear the colour buffer (more buffers later on)  
	glLoadIdentity(); // Load the Identity Matrix to reset our drawing locations  
	glFlush(); // Flush the OpenGL buffers to the window  
}  

int main2 (int argc, char **argv) {  
	glutInit(&argc, argv); // Initialize GLUT  
	glutInitDisplayMode (GLUT_SINGLE); // Set up a basic display buffer (only single buffered for now)  
	glutInitWindowSize (500, 500); // Set the width and height of the window  
	glutInitWindowPosition (100, 100); // Set the position of the window  
	glutCreateWindow ("Your first OpenGL Window"); // Set the title for the window  
	glutDisplayFunc(display); // Tell GLUT to use the method "display" for rendering  
  
	glutMainLoop(); // Enter GLUT's main loop  
}
