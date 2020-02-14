// Ejercicios 1, 2 y 3 (tema 1, parte 1)
#include <GL/glut.h>

const GLfloat SUN_SCALE = 4.0f;
const GLfloat EARTH_SCALE = 1.0f;
const GLfloat SATELLITE_SCALE = 0.25f;
const GLfloat MOON_SCALE = 0.4f;

const GLfloat SUN_EARTH_DISTANCE = 9.0f;
const GLfloat EARTH_SATELLITE_DISTANCE = 5.0f;
const GLfloat EARTH_MOON_DISTANCE = 3.0f;

const GLfloat EARTH_ROTATION_SPEED = 1.0f;
const GLfloat EARTH_SELF_ROTATION_SPEED = EARTH_MOON_DISTANCE * 365.25f;
const GLfloat SATELLITE_ROTATION_SPEED = 0.05f;
const GLfloat SATELLITE_SELF_ROTATION_SPEED = 0.3f;
const GLfloat MOON_ROTATION_SPEED = 0.1f;
const GLfloat MOON_SELF_ROTATION_SPEED = EARTH_SELF_ROTATION_SPEED / 28.0f;

void drawBox();
void drawBoxWithTriangles();
void drawSolarSystem();
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
bool animation = false;

float xrot = 0.0f;
float yrot = 0.0f;
float xdiff = 0.0f;
float ydiff = 0.0f;

GLfloat earthRotation = 0.0f;
GLfloat earthSelfRotation = 0.0f;
GLfloat moonRotation = 0.0f;
GLfloat moonSelfRotation = 0.0f;
GLfloat satelliteRotation = 0.0f;
GLfloat satelliteSelfRotation = 0.0f;

int g_Width = 800;                          // Ancho inicial de la ventana
int g_Height = 500;                         // Altura incial de la ventana

GLboolean flagW = false;
GLboolean flagP = true;


int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(g_Width, g_Height);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
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

void drawBox()
{
	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.0f, 0.0f);
	// FRONT
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, 0.5f, 0.5f);
	glVertex3f(-0.5f, 0.5f, 0.5f);
	// BACK
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, 0.5f, -0.5f);
	glVertex3f(0.5f, 0.5f, -0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);

	glColor3f(0.0f, 1.0f, 0.0f);
	// LEFT
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(-0.5f, 0.5f, 0.5f);
	glVertex3f(-0.5f, 0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	// RIGHT
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(0.5f, 0.5f, -0.5f);
	glVertex3f(0.5f, 0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);

	glColor3f(0.0f, 0.0f, 1.0f);
	// TOP
	glVertex3f(-0.5f, 0.5f, 0.5f);
	glVertex3f(0.5f, 0.5f, 0.5f);
	glVertex3f(0.5f, 0.5f, -0.5f);
	glVertex3f(-0.5f, 0.5f, -0.5f);
	// BOTTOM
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glEnd();
}

void drawBoxWithTriangles()
{
	glShadeModel(GL_FLAT);

	glBegin(GL_TRIANGLE_STRIP);

	glColor3f(1.0f, 0.0f, 0.0f);	// FRONT	

	glVertex3f(-0.5, -0.5, 0.5);
	glVertex3f(0.5, -0.5, 0.5);
	glVertex3f(-0.5, 0.5, 0.5);
	glVertex3f(0.5, 0.5, 0.5);

	glColor3f(0.0f, 1.0f, 0.0f);	// TOP

	glVertex3f(-0.5, 0.5, -0.5);
	glVertex3f(0.5, 0.5, -0.5);

	glColor3f(0.0f, 0.0f, 1.0f);	// BACK

	glVertex3f(-0.5, -0.5, -0.5);
	glVertex3f(0.5, -0.5, -0.5);

	glEnd();

	glBegin(GL_TRIANGLE_STRIP);

	glColor3f(1.0f, 1.0f, 0.0f);	// RIGHT

	glVertex3f(0.5, 0.5, -0.5);
	glVertex3f(0.5, 0.5, 0.5);
	glVertex3f(0.5, -0.5, -0.5);
	glVertex3f(0.5, -0.5, 0.5);

	glColor3f(0.0f, 1.0f, 1.0f);	// BOTTOM

	glVertex3f(-0.5, -0.5, -0.5);
	glVertex3f(-0.5, -0.5, 0.5);

	glColor3f(1.0f, 0.0f, 1.0f);	//LEFT

	glVertex3f(-0.5, 0.5, -0.5);
	glVertex3f(-0.5, 0.5, 0.5);

	glEnd();
}

void drawSolarSystem() {

	// SUN
	glPushMatrix();
	glScalef(SUN_SCALE, SUN_SCALE, SUN_SCALE);
	drawBox();
	glPopMatrix();

	glRotatef(earthRotation, 0.0f, 1.0f, 0.0f);
	glTranslatef(SUN_EARTH_DISTANCE, 0.0f, 0.0f);

	// EARTH
	glPushMatrix();
	glRotatef(earthSelfRotation, 0.0f, 1.0f, 0.0f);
	glScalef(EARTH_SCALE, EARTH_SCALE, EARTH_SCALE);
	drawBox();
	glPopMatrix();

	// MOON
	glPushMatrix();
	glRotatef(moonRotation, 0.0f, 1.0f, 0.0f);
	glTranslatef(EARTH_MOON_DISTANCE, 0.0f, 0.0f);
	glRotatef(moonSelfRotation, 0.0f, 1.0f, 0.0f);
	drawBox();
	glPopMatrix();

	// SATELLITE
	glPushMatrix();
	glRotatef(satelliteRotation, 0.0f, 0.0f, 1.0f);
	glTranslatef(EARTH_SATELLITE_DISTANCE, 0.0f, 0.0f);
	glRotatef(satelliteSelfRotation, 0.0f, 1.0f, 0.0f);
	drawBox();
	glPopMatrix();

	

	glFlush();
}

bool init()
{
	glClearColor(0.93f, 0.93f, 0.93f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);

	return true;
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	gluLookAt(
		0.0f, 5.0f, 30.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);

	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);

	if (flagW)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//drawBox();

	//drawBoxWithTriangles();

	drawSolarSystem();

	glutSwapBuffers();
}

void resize(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	g_Width = w;
	g_Height = h;
	glViewport(0, 0, g_Width, g_Height);

	if (flagP) {
		gluPerspective(45.0f, 1.0f * g_Width / g_Height, 1.0f, 100.0f);
	}
	else {
		glOrtho(-10.0f, 10.0f, -10.0f, 10.0f, -100.0f, 100.0f);
	}


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void idle()
{
	if (!mouseDown && animation)
	{
		//xrot += 0.3f;
		//yrot += 0.4f;
		earthRotation += EARTH_ROTATION_SPEED;
		earthSelfRotation += EARTH_SELF_ROTATION_SPEED;
		moonRotation += MOON_ROTATION_SPEED;
		moonSelfRotation += MOON_SELF_ROTATION_SPEED;
		satelliteRotation += SATELLITE_ROTATION_SPEED;
		satelliteSelfRotation += SATELLITE_SELF_ROTATION_SPEED;
	}

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: case 'q': case 'Q':
		exit(1);
		break;
	case 'a': case 'A':
		animation = !animation;
		break;
	case 'w': case 'W':
		flagW = !flagW;
		break;
	case 'p': case 'P':
		flagP = !flagP;
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
