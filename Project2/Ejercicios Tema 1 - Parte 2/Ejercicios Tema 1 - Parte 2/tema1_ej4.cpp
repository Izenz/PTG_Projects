// Ejercicios 4 y 5 (tema 1, parte 2)

#include <GL/glut.h>
#include <iostream>
//#include "tga.h"

void drawPlane();
bool init();
void display();
void resize(int, int);
void idle();
void keyboard(unsigned char, int, int);
void specialKeyboard(int, int, int);
void mouse(int, int, int, int);
void mouseMotion(int, int);

bool fullscreen = false;
bool mouseDown = false;
bool animation = true;
 
float xrot = 0.0f;
float yrot = 0.0f;
float xdiff = 0.0f;
float ydiff = 0.0f;

int g_Width = 500;                          // Ancho inicial de la ventana
int g_Height = 500;                         // Altura incial de la ventana

// Intensidad de la luz
const GLfloat intensidadLuz[] = {1.0f, 1.0f, 1.0f, 1.0f};	
// Posición de la luz
const GLfloat luzLocal[] = {0.5f, 0.5f, 1.0f, 1.0f}; // Tanto la luz puntual como el foco
const GLfloat luzDireccional[] = {0.5f, 0.5f, 1.0f, 0.0f}; // Luz direccional
// Dirección a la que apunta el foco
const GLfloat direccionFoco[] = {-0.5f, -0.5f, -4.0f}; // para que apunte al centro escena: dir =  -pos(luzLocal) - pos(lookat)
// Amplitud del cono de luz del foco (en grados)
const GLfloat amplitudFoco = 10.0f; //grados

// Id de la textura
GLuint texture_id; 

GLboolean lightningActivated = false;
GLboolean smoothingActivated = false;
GLint activeLight = 0;

int main(int argc, char *argv[])
{
	glutInit(&argc, argv); 
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(g_Width, g_Height);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Programa Ejemplo");
	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);
 
	glutMainLoop();
 
	return EXIT_SUCCESS;
}
 

void drawPlane()
{
	const int numDivisiones = 100;
	const int TAM = 3.0f;
	for (int i = 0; i < numDivisiones; i ++ )
	{
		float y0 = TAM * float(i) / numDivisiones - TAM / 2.0f;
		float yf = TAM * float(i+1) / numDivisiones - TAM / 2.0f;
		glBegin(GL_TRIANGLE_STRIP);
		for ( int j = 0; j <= numDivisiones; j ++ )
		{
			float x = TAM * float(j) / numDivisiones - TAM / 2.0f;
			// glTexCoord2f(x,y0);
			// glNormal3f(0.0f, 0.0f, 1.0f);
			glVertex3f(x, y0, 0.0f);
			// glTexCoord2f(x,yf);
			// glNormal3f(0.0f, 0.0f, 1.0f);
			glVertex3f(x, yf, 0.0f);
		}
		glEnd();
	}
}

bool init()
{
	glClearColor(0.93f, 0.93f, 0.93f, 0.0f);
 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);

	// Tarea por hacer: Activar la iluminación.
	// glEnable(GL_LIGHTING);
	
	// Tarea por hacer: definir 3 luces.
	// 1º luz: puntual
	glLightfv(GL_LIGHT0, GL_POSITION, luzLocal);
	glLightfv(GL_LIGHT0, GL_AMBIENT, intensidadLuz);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, intensidadLuz);
	glLightfv(GL_LIGHT0, GL_SPECULAR, intensidadLuz);

	// 2ª luz: focal
	glLightfv(GL_LIGHT1, GL_POSITION, luzLocal);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, direccionFoco);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, amplitudFoco);
	glLightfv(GL_LIGHT1, GL_AMBIENT, intensidadLuz);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, intensidadLuz);
	glLightfv(GL_LIGHT2, GL_SPECULAR, intensidadLuz);
	
	// 3ª luz: Direccional
	glLightfv(GL_LIGHT2, GL_POSITION, luzDireccional);
	glLightfv(GL_LIGHT2, GL_AMBIENT, intensidadLuz);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, intensidadLuz);
	glLightfv(GL_LIGHT2, GL_SPECULAR, intensidadLuz);

	glEnable(GL_LIGHT0);
	/*
	
	// Tarea por hacer: Crear el obj. textura, cargar la imagen tga, activar la textura, cargar los datos en el objeto textura y definir filtros, modo repetición y modo pegado
	// Inicializa Texturas
	TGAFILE tgaImage[1];
	glGenTextures (1, &texture_id);  
	std::cout << "Cargando la textura metalSheetDiffuse.tga" << std::endl;
	if ( LoadTGAFile("metalSheetDiffuse.tga", &tgaImage[0]) )
    {
	   glBindTexture(GL_TEXTURE_2D, texture_id);
	   
	   std::cout << "tamanyo (w,h) = (" << tgaImage[0].imageWidth << "," << tgaImage[0].imageHeight << ")" << std::endl;
	   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tgaImage[0].imageWidth, tgaImage[0].imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, tgaImage[0].imageData);
		// gluBuild2DMipmaps(GL_TEXTURE_2D,			// texture to specify
		//				 GL_RGBA,				// internal texture storage format
		//				 tgaImage[0].imageWidth,   // texture width
		//				 tgaImage[0].imageHeight,	// texture height
		//				 GL_RGBA,				// pixel format
		//				 GL_UNSIGNED_BYTE,		// color component format
		//				 tgaImage[0].imageData);	// pointer to texture image

	   glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	   ....
    }
   else
	   std::cout << "Error al cargar la textura stoneDiffuse.tga" << std::endl;
	*/

	return true;
}
 
void display()
{
	GLfloat colorAmbient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat colorBronzeDiff[4] = { 0.8f, 0.6f, 0.0f, 1.0f };
	GLfloat colorBronzeSpec[4] = { 1.0f, 1.0f, 0.4f, 1.0f };

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
 
	gluLookAt(
	0.0f, 0.0f, 3.0f,
	0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f);
 
	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);
 
	if (lightningActivated) {
		glEnable(GL_LIGHTING);
	}
	else {
		glDisable(GL_LIGHTING);
	}

	glShadeModel(smoothingActivated ? GL_SMOOTH : GL_FLAT);
	
	// Tarea por hacer: Activar el material antes de dibujar los objetos de la escena.
	glMaterialfv(GL_FRONT, GL_AMBIENT, colorAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, colorBronzeDiff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, colorBronzeSpec);
	glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);
	//glColor4fv(colorBronzeDiff); // Color de los objetos dibujados en la escena
	
	
	drawPlane();
	glutSolidTeapot(0.5f);
	//glutWireTeapot(0.5f);
	//glutSolidCube(0.5f);
	//glutWireCube(0.5f);
	//glutSolidSphere(0.5f, 20, 20);
	//glutWireSphere(0.5f, 20, 20);
	//glutSolidCone(0.25f, 0.5f, 20, 1);
	//glutWireCone(0.25f, 0.5f, 20, 1);
	//glutSolidTorus(0.25, 0.5, 10, 20);
	//glutWireTorus(0.25, 0.5, 10, 20);

	glPopMatrix();
	glutSwapBuffers();
}
 
void resize(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	g_Width = w;
	g_Height = h;
	glViewport(0, 0, g_Width, g_Height);
 
	gluPerspective(45.0f, 1.0f * g_Width / g_Height, 1.0f, 100.0f);
 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
 
void idle()
{
	if (!mouseDown && animation)
	{
		xrot += 0.3f;
		yrot += 0.4f;
	}
 
	glutPostRedisplay();
}
 
void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 27 : case 'q': case 'Q':
		exit(1); 
		break;
	case 'a': case 'A':
		animation = !animation;
		break;
	case 'l': case 'L': 
		// Tarea por hacer: activar/desctivar la iluminación.
		lightningActivated = !lightningActivated;
		break;
	case 's': case 'S': 
		// Tarea por hacer: cambiar el modo de sombreado (de suave a plano y viceversa).
		smoothingActivated = !smoothingActivated;
		break;
	case 'c': case 'C':
		// Tarea por hacer: cambiar la luz activa.
		++activeLight;
		if (activeLight > 2)
			activeLight = 0;

		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
		switch (activeLight) {
		case 0:
			glEnable(GL_LIGHT0);
			break;
		case 1:
			glEnable(GL_LIGHT1);
			break;
		case 2:
			glEnable(GL_LIGHT2);
			break;
		}
		break;
	case 't': case 'T': 
		// Tarea por hacer: activa/desactiva textura
		break;
	case 'm': case 'M':
		// Tarea por hacer: modo de pegado de la textura
		break;
	case 'e': case 'E':
		// Tarea por hacer: modo de repetición de la textura
		break;
	case 'f': case 'F':
		// Tarea por hacer: cambiar el filtro de minificación de la textura
		break;	
	}
}
 
void specialKeyboard(int key, int x, int y)
{
	if (key == GLUT_KEY_F1)
	{
		fullscreen = !fullscreen;
 
		if (fullscreen)
			glutFullScreen();
		else
		{
			glutReshapeWindow(g_Width, g_Height);
			glutPositionWindow(50, 50);
		}
	}
}
 
void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		mouseDown = true;
 
		xdiff = x - yrot;
		ydiff = -y + xrot;
	}
	else
		mouseDown = false;
}
 
void mouseMotion(int x, int y)
{
	if (mouseDown)
	{
		yrot = x - xdiff;
		xrot = y + ydiff;
 
		glutPostRedisplay();
	}
}
