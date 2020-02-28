// Ejercicio 6 (tema 1, parte 3)
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <ctime>
#include <cstdlib>

int state = 0;

GLuint vboVertex;
GLuint vboVertex2;
GLuint vboColor;
GLuint vboColor2;
GLuint vboVertexColor;
GLuint vboIndex;
GLuint vboIndex2;

GLuint vaoEnable7;
GLuint vaoEnable8;
GLuint vaoEnable9;

GLuint vaoDisable7;
GLuint vaoDisable8;
GLuint vaoDisable9;

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

GLuint initVBOfloat(int dataSize, GLfloat *vertexData);
GLuint initVBOubyte(int dataSize, GLubyte *vertexData);
GLuint initVAO(GLboolean enable, GLuint lState);

bool init();
void display();
void resize(int, int);
void idle();
void enable();
void disable();
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

	enable();

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

					switch (state) {
					case 0:
						draw1Cube();
						break;
					case 1:
						glDrawArrays(GL_TRIANGLES, 0, 36);
						break;
					case 2:
						glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);
						break;
					case 3:
						glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);
						break;
					case 4:
					case 7:
						glDrawArrays(GL_TRIANGLES, 0, 36);
						break;
					case 5:
					case 8:
						glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, (char*)NULL + 0);
						break;
					case 6:
					case 9:
						glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, (char*)NULL + 0);
						break;
					}
				glPopMatrix();
			}
		}
	}
	disable();
}


bool init()
{
	glClearColor(0.93f, 0.93f, 0.93f, 0.0f);
 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearDepth(1.0f);

	glShadeModel(GL_SMOOTH);

	vboVertex = initVBOfloat(108, vertices1);
	vboColor = initVBOfloat(108, colores1);

	vboVertex2 = initVBOfloat(36, vertices2);
	vboColor2 = initVBOfloat(36, colores2);
	vboIndex = initVBOubyte(36, indices);

	vboVertexColor = initVBOfloat(72, vertices3);
	vboIndex2 = initVBOubyte(36, indices);
	
	
	vaoEnable7 = initVAO(true, 7);
	vaoDisable7 = initVAO(false, 7);
	
	vaoEnable8 = initVAO(true, 8);
	vaoDisable8 = initVAO(false, 8);

	vaoEnable9 = initVAO(true, 9);
	vaoDisable9 = initVAO(false, 9);
	
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

	// draw1Cube();
	drawMultipleCubes();

	printFPS();
	glutSwapBuffers();
}

void disable() {
	switch (state) {
	case 0:

		break;
	case 1:
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		break;
	case 2:
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_INDEX_ARRAY);
		break;
	case 3:
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		break;
	case 4:
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		break;
	case 5:
	case 6:
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_INDEX_ARRAY);
		break;
	case 7:
		glBindVertexArray(vaoDisable7);
		break;
	case 8:
		glBindVertexArray(vaoDisable8);
		break;
	case 9:
		glBindVertexArray(vaoDisable9);
		break;
	}
}

GLuint initVBOfloat(int dataSize, GLfloat *vertexData) {
	GLuint VBOBuffers;

	glGenBuffers(1, &VBOBuffers);
	glBindBuffer(GL_ARRAY_BUFFER, VBOBuffers);
	glBufferData(GL_ARRAY_BUFFER, dataSize*sizeof(GL_FLOAT), vertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//delete [] vertexData;
	return VBOBuffers;
}

GLuint initVBOubyte(int dataSize, GLubyte *vertexData) {
	GLuint VBOBuffers;

	glGenBuffers(1, &VBOBuffers);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOBuffers);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize * sizeof(GL_UNSIGNED_BYTE), vertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//delete [] vertexData;
	return VBOBuffers;
}

GLuint initVAO(GLboolean enable, GLuint lState) {
	GLuint vaoHandle;

	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	if (enable) {
		switch (lState) {
		case 7:
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
			glVertexPointer(3, GL_FLOAT, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, vboColor);
			glColorPointer(3, GL_FLOAT, 0, 0);
			break;
		case 8:
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glEnableClientState(GL_INDEX_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, vboVertex2);
			glVertexPointer(3, GL_FLOAT, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, vboColor2);
			glColorPointer(3, GL_FLOAT, 0, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndex);
			break;
		case 9:
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glEnableClientState(GL_INDEX_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, vboVertexColor);
			glVertexPointer(3, GL_FLOAT, 6 * sizeof(GL_FLOAT), (char*)NULL + 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndex2);
			glColorPointer(3, GL_FLOAT, 6 * sizeof(GL_FLOAT), (char*)NULL + 3 * sizeof(GL_FLOAT));
			break;
		}
	}
	else {
		switch (lState) {
			case 7:
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glDisableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_COLOR_ARRAY);
				break;
			case 8:
			case 9:
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				glDisableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_COLOR_ARRAY);
				glDisableClientState(GL_INDEX_ARRAY);
				break;
		}
	}
	glBindVertexArray(0);
	return vaoHandle;
}

void enable() {
	switch (state) {
		case 0:
			break;
		case 1:
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glVertexPointer(3, GL_FLOAT, 0, vertices1);
			glColorPointer(3, GL_FLOAT, 0, colores1);
			break;
		case 2:
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glEnableClientState(GL_INDEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, 0, vertices2);
			glColorPointer(3, GL_FLOAT, 0, colores2);
			break;
		case 3:
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glEnableClientState(GL_INDEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, 6 * sizeof(GL_FLOAT), vertices3);
			glColorPointer(3, GL_FLOAT, 6 * sizeof(GL_FLOAT), &vertices3[3]);
			break;
		case 4:
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
			glVertexPointer(3, GL_FLOAT, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, vboColor);
			glColorPointer(3, GL_FLOAT, 0, 0);
			break;
		case 5:
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glEnableClientState(GL_INDEX_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, vboVertex2);
			glVertexPointer(3, GL_FLOAT, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, vboColor2);
			glColorPointer(3, GL_FLOAT, 0, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndex);
			break;
		case 6:
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glEnableClientState(GL_INDEX_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, vboVertexColor);
			glVertexPointer(3,  GL_FLOAT, 6 * sizeof(GL_FLOAT), (char*)NULL + 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndex2);
			glColorPointer(3, GL_FLOAT, 6 * sizeof(GL_FLOAT), (char*)NULL + 3 * sizeof(GL_FLOAT));
			break;
		case 7:
			glBindVertexArray(vaoEnable7);
			break;
		case 8:
			glBindVertexArray(vaoEnable8);
			break;
		case 9:
			glBindVertexArray(vaoEnable9);
			break;
	}

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
	case '0':
		state = 0;
		break;
	case '1':
		state = 1;
		break;
	case '2':
		state = 2;
		break;
	case '3':
		state = 3;
		break;
	case '4':
		state = 4;
		break;
	case '5':
		state = 5;
		break;
	case '6':
		state = 6;
		break;
	case '7':
		state = 7;
		break;
	case'8':
		state = 8;
		break;
	case '9':
		state = 9;
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
	std::cout << "State: " << state << " -->" << fps << std::endl;
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


