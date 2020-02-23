// Ejercicio 6 (tema 1, parte 3)
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <ctime>
#include <cstdlib>


//  Número de frames
int frameCount = 0;
//  Número de frames por segundo
float fps = 0;
//  currentTime y previousTime - momento actual y momento previo
int currentTime = 0, previousTime = 0;
void calculateFPS();
void printFPS();

void draw1Cube();
void drawMultipleCubes();

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


// cubo ///////////////////////////////////////////////////////////////////////
//    v6----- v5
//   /|      /|
//  v1------v0|
//  | |     | |
//  | |v7---|-|v4
//  |/      |/
//  v2------v3

// Coordenadas del vertex array  =====================================
// Un cubo tiene 6 lados y cada lado tiene 2 triangles, por tanto, un cubo
// tiene 36 vértices (6 lados * 2 trian * 3 vertices = 36 vertices). Y cada
// vertice tiene 3 components (x,y,z) de reales, por tanto, el tamaño del vertex
// array es 108 floats (36 * 3 = 108).
GLfloat vertices1[] = { 1, 1, 1,  -1, 1, 1,  -1,-1, 1,      // v0-v1-v2 (front)
                       -1,-1, 1,   1,-1, 1,   1, 1, 1,      // v2-v3-v0

                        1, 1, 1,   1,-1, 1,   1,-1,-1,      // v0-v3-v4 (right)
                        1,-1,-1,   1, 1,-1,   1, 1, 1,      // v4-v5-v0

                        1, 1, 1,   1, 1,-1,  -1, 1,-1,      // v0-v5-v6 (top)
                       -1, 1,-1,  -1, 1, 1,   1, 1, 1,      // v6-v1-v0

                       -1, 1, 1,  -1, 1,-1,  -1,-1,-1,      // v1-v6-v7 (left)
                       -1,-1,-1,  -1,-1, 1,  -1, 1, 1,      // v7-v2-v1

                       -1,-1,-1,   1,-1,-1,   1,-1, 1,      // v7-v4-v3 (bottom)
                        1,-1, 1,  -1,-1, 1,  -1,-1,-1,      // v3-v2-v7

                        1,-1,-1,  -1,-1,-1,  -1, 1,-1,      // v4-v7-v6 (back)
                       -1, 1,-1,   1, 1,-1,   1,-1,-1 };    // v6-v5-v4
// color array
GLfloat colores1[]   = { 1, 1, 1,   1, 1, 0,   1, 0, 0,      // v0-v1-v2 (front)
                        1, 0, 0,   1, 0, 1,   1, 1, 1,      // v2-v3-v0

                        1, 1, 1,   1, 0, 1,   0, 0, 1,      // v0-v3-v4 (right)
                        0, 0, 1,   0, 1, 1,   1, 1, 1,      // v4-v5-v0

                        1, 1, 1,   0, 1, 1,   0, 1, 0,      // v0-v5-v6 (top)
                        0, 1, 0,   1, 1, 0,   1, 1, 1,      // v6-v1-v0

                        1, 1, 0,   0, 1, 0,   0, 0, 0,      // v1-v6-v7 (left)
                        0, 0, 0,   1, 0, 0,   1, 1, 0,      // v7-v2-v1

                        0, 0, 0,   0, 0, 1,   1, 0, 1,      // v7-v4-v3 (bottom)
                        1, 0, 1,   1, 0, 0,   0, 0, 0,      // v3-v2-v7

                        0, 0, 1,   0, 0, 0,   0, 1, 0,      // v4-v7-v6 (back)
                        0, 1, 0,   0, 1, 1,   0, 0, 1 };    // v6-v5-v4



// Vertex array eliminando los vértices repetidos (más compacto)
GLfloat vertices2[]= { 1, 1, 1,  -1, 1, 1,  -1,-1, 1,   1,-1, 1,   // v0,v1,v2,v3 (front)
						1,-1,-1,   1, 1,-1,  -1, 1,-1,  -1,-1,-1};	// v4,v5,v6,v7 (back)

// Color array eliminando los vértices repetidos
GLfloat colores2[]  = { 1, 1, 1,   1, 1, 0,   1, 0, 0,   1, 0, 1,   // v0,v1,v2,v3 (front)
						0, 0, 1,   0, 1, 1,   0, 1, 0,   0, 0, 0};	// v4,v5,v6,v7 (back)

// Index array 
GLubyte indices[] = { 0, 1, 2,   2, 3, 0,      // front
                       0, 3, 4,   4, 5, 0,      // right
                       0, 5, 6,   6, 1, 0,      // top
                       1, 6, 7,   7, 2, 1,      // left
                       7, 4, 3,   3, 2, 7,      // bottom
                       4, 7, 6,   6, 5, 4 };    // back

// Vertex array entrelazado
// Los dos atributos (posición y color) se empaquetan juntos. El resultado es 
// algo similar a un conjunto de estructuras: ((V,C), (V,C), (V,C),...).
GLfloat vertices3[]= {	 1, 1, 1,  1, 1, 1,	// v0
						-1, 1, 1,  1, 1, 0,	// v1
						-1,-1, 1,  1, 0, 0,	// v2
						 1,-1, 1,  1, 0, 1,	// v3
						 1,-1,-1,  0, 0, 1,	// v4
						 1, 1,-1,  0, 1, 1,	// v5
						-1, 1,-1,  0, 1, 0,	// v6
						-1,-1,-1,  0, 0, 0};// v7


int main(int argc, char *argv[])
{
	glutInit(&argc, argv); 
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(g_Width, g_Height);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Programa Ejemplo");
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	  /* Problem: glewInit failed, something is seriously wrong. */
	  std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
	  system("pause");
	  exit(-1);
	}
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

///////////////////////////////////////////////////////////////////////////////
// dibuja 1 cubo: modo inmediato
// 78 llamadas = 36 glVertex*() llamadas + 36 glColor*() llamadas + 6 glNormal*() llamadas
///////////////////////////////////////////////////////////////////////////////
void draw1Cube()
{
    glBegin(GL_TRIANGLES);
        // front faces
        glNormal3f(0,0,1);
        // face v0-v1-v2
        glColor3f(1,1,1);
        glVertex3f(1,1,1);
        glColor3f(1,1,0);
        glVertex3f(-1,1,1);
        glColor3f(1,0,0);
        glVertex3f(-1,-1,1);
        // face v2-v3-v0
        glColor3f(1,0,0);
        glVertex3f(-1,-1,1);
        glColor3f(1,0,1);
        glVertex3f(1,-1,1);
        glColor3f(1,1,1);
        glVertex3f(1,1,1);

        // right faces
        glNormal3f(1,0,0);
        // face v0-v3-v4
        glColor3f(1,1,1);
        glVertex3f(1,1,1);
        glColor3f(1,0,1);
        glVertex3f(1,-1,1);
        glColor3f(0,0,1);
        glVertex3f(1,-1,-1);
        // face v4-v5-v0
        glColor3f(0,0,1);
        glVertex3f(1,-1,-1);
        glColor3f(0,1,1);
        glVertex3f(1,1,-1);
        glColor3f(1,1,1);
        glVertex3f(1,1,1);

        // top faces
        glNormal3f(0,1,0);
        // face v0-v5-v6
        glColor3f(1,1,1);
        glVertex3f(1,1,1);
        glColor3f(0,1,1);
        glVertex3f(1,1,-1);
        glColor3f(0,1,0);
        glVertex3f(-1,1,-1);
        // face v6-v1-v0
        glColor3f(0,1,0);
        glVertex3f(-1,1,-1);
        glColor3f(1,1,0);
        glVertex3f(-1,1,1);
        glColor3f(1,1,1);
        glVertex3f(1,1,1);

        // left faces
        glNormal3f(-1,0,0);
        // face  v1-v6-v7
        glColor3f(1,1,0);
        glVertex3f(-1,1,1);
        glColor3f(0,1,0);
        glVertex3f(-1,1,-1);
        glColor3f(0,0,0);
        glVertex3f(-1,-1,-1);
        // face v7-v2-v1
        glColor3f(0,0,0);
        glVertex3f(-1,-1,-1);
        glColor3f(1,0,0);
        glVertex3f(-1,-1,1);
        glColor3f(1,1,0);
        glVertex3f(-1,1,1);

        // bottom faces
        glNormal3f(0,-1,0);
        // face v7-v4-v3
        glColor3f(0,0,0);
        glVertex3f(-1,-1,-1);
        glColor3f(0,0,1);
        glVertex3f(1,-1,-1);
        glColor3f(1,0,1);
        glVertex3f(1,-1,1);
        // face v3-v2-v7
        glColor3f(1,0,1);
        glVertex3f(1,-1,1);
        glColor3f(1,0,0);
        glVertex3f(-1,-1,1);
        glColor3f(0,0,0);
        glVertex3f(-1,-1,-1);

        // back faces
        glNormal3f(0,0,-1);
        // face v4-v7-v6
        glColor3f(0,0,1);
        glVertex3f(1,-1,-1);
        glColor3f(0,0,0);
        glVertex3f(-1,-1,-1);
        glColor3f(0,1,0);
        glVertex3f(-1,1,-1);
        // face v6-v5-v4
        glColor3f(0,1,0);
        glVertex3f(-1,1,-1);
        glColor3f(0,1,1);
        glVertex3f(1,1,-1);
        glColor3f(0,0,1);
        glVertex3f(1,-1,-1);
    glEnd();
}

////////////////////////////////////////////////////////////////////////////////////
// Dibuja múltiples cubos
////////////////////////////////////////////////////////////////////////////////////
void drawMultipleCubes()
{
	const int N = 40; // Número de cubos en cada eje
	const float RangoIni = -1.0f;
	const float RangoFin =  1.0f;
	float x, y, z;
	float scale = (RangoFin - RangoIni) / (N * 3);

	for ( int i = 0; i < N; i ++ )
	{
		x = RangoIni + i * (RangoFin - RangoIni) / (N - 1);
		for ( int j = 0; j < N; j ++ )
		{
			y = RangoIni + j * (RangoFin - RangoIni) / (N - 1);
			for ( int k = 0; k < N; k ++ )
			{
				z = RangoIni + k * (RangoFin - RangoIni) / (N - 1);
				glPushMatrix();
					glTranslatef(x,y,z);
					glScalef(scale,scale,scale);
					draw1Cube();
				glPopMatrix();
			}
		}
	}
}


bool init()
{
	glClearColor(0.93f, 0.93f, 0.93f, 0.0f);
 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearDepth(1.0f);

	glShadeModel(GL_SMOOTH);

	return true;
}
 
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	gluLookAt(
	0.0f, 0.0f, 5.0f,
	0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f);

	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);

	draw1Cube();
	// drawMultipleCubes();

	printFPS();
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

     //  Calcula FPS
    calculateFPS();

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

//-------------------------------------------------------------------------
//  Print FPS
//-------------------------------------------------------------------------
void printFPS()
{
	std::cout.precision(4);
	std::cout << fps << std::endl;
}

//-------------------------------------------------------------------------
// Calcula el frame rate por segundo
//-------------------------------------------------------------------------
void calculateFPS()
{
    frameCount++;

    currentTime = glutGet(GLUT_ELAPSED_TIME);

    int timeInterval = currentTime - previousTime;

    if(timeInterval > 1000)
    {
        fps = frameCount / (timeInterval / 1000.0f);

        previousTime = currentTime;

		frameCount = 0;
    }
}


