// Ejercicios 4 y 5 (parte 2) del tema 2

#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "vboteapot.h"
#include "tga.h"

void initCube();
int initTeapot(int grid, glm::mat4 transform);
int initPlane(float xsize, float zsize, int xdivs, int zdivs);
void initVAO(float* v, float* n);
void drawCube();
void drawTeapot();
void drawPlane();

void loadSource(GLuint &shaderID, std::string name);
void printCompileInfoLog(GLuint shadID);
void printLinkInfoLog(GLuint programID);
void validateProgram(GLuint programID);

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
float radius = 0.0f;
float angle = 0.0f;
float umbralValue = 1.5f;

int g_Width = 500;                          // Ancho inicial de la ventana
int g_Height = 500;                         // Altura incial de la ventana

GLuint teapotVAOHandle;
GLuint locUniformMVPM;
GLuint locUniformEscala;
GLuint locUniformMColor;
GLuint locUniformUmbral;
GLuint locUniformStone, locUniformMoss, locUniformUseTexture;
GLuint locColorMatrix;
GLuint locUmbral;
GLuint locStoneTex;
GLuint locMossTex;
GLuint locTextureSelection;
GLuint vaoHandle;
GLuint programShaderID;
GLuint vertexShaderID;
GLuint fragmenShaderID;
GLuint loc3;
GLuint loc5;

int numVertTeapot;
int textureSelection = 0;
GLboolean KEY_P_PRESSED = false;
GLboolean KEY_D_PRESSED = false;
GLboolean colorRotationActivated = false;

// BEGIN: Carga shaders ////////////////////////////////////////////////////////////////////////////////////////////

void loadSource(GLuint &shaderID, std::string name) 
{
	std::ifstream f(name.c_str());
	if (!f.is_open()) 
	{
		std::cerr << "File not found " << name.c_str() << std::endl;
		system("pause");
		exit(EXIT_FAILURE);
	}

	// now read in the data
	std::string *source;
	source = new std::string( std::istreambuf_iterator<char>(f),   
						std::istreambuf_iterator<char>() );
	f.close();
   
	// add a null to the string
	*source += "\0";
	const GLchar * data = source->c_str();
	glShaderSource(shaderID, 1, &data, NULL);
	delete source;
}

void printCompileInfoLog(GLuint shadID) 
{
GLint compiled;
	glGetShaderiv( shadID, GL_COMPILE_STATUS, &compiled );
	if (compiled == GL_FALSE)
	{
		GLint infoLength = 0;
		glGetShaderiv( shadID, GL_INFO_LOG_LENGTH, &infoLength );

		GLchar *infoLog = new GLchar[infoLength];
		GLint chsWritten = 0;
		glGetShaderInfoLog( shadID, infoLength, &chsWritten, infoLog );

		std::cerr << "Shader compiling failed:" << infoLog << std::endl;
		system("pause");
		delete [] infoLog;

		exit(EXIT_FAILURE);
	}
}

void printLinkInfoLog(GLuint programID)
{
GLint linked;
	glGetProgramiv( programID, GL_LINK_STATUS, &linked );
	if(! linked)
	{
		GLint infoLength = 0;
		glGetProgramiv( programID, GL_INFO_LOG_LENGTH, &infoLength );

		GLchar *infoLog = new GLchar[infoLength];
		GLint chsWritten = 0;
		glGetProgramInfoLog( programID, infoLength, &chsWritten, infoLog );

		std::cerr << "Shader linking failed:" << infoLog << std::endl;
		system("pause");
		delete [] infoLog;

		exit(EXIT_FAILURE);
	}
}

void validateProgram(GLuint programID)
{
GLint status;
    glValidateProgram( programID );
    glGetProgramiv( programID, GL_VALIDATE_STATUS, &status );

    if( status == GL_FALSE ) 
	{
		GLint infoLength = 0;
		glGetProgramiv( programID, GL_INFO_LOG_LENGTH, &infoLength );

        if( infoLength > 0 ) 
		{
			GLchar *infoLog = new GLchar[infoLength];
			GLint chsWritten = 0;
            glGetProgramInfoLog( programID, infoLength, &chsWritten, infoLog );
			std::cerr << "Program validating failed:" << infoLog << std::endl;
			system("pause");
            delete [] infoLog;

			exit(EXIT_FAILURE);
		}
    }
}

// END:   Carga shaders ////////////////////////////////////////////////////////////////////////////////////////////


void initVAO(float *v, float *n) 
{
	GLuint vboHandle;

	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	glGenBuffers(1, &vboHandle);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v) + sizeof(n), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(v), v);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(v), sizeof(n), n);
	
}


// BEGIN: Inicializa primitivas de dibujo //////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Init Teapot
// parametros: 
//		grid - número de rejillas
//		transform - matriz de tranformación del modelo
// return:
//		número de vertices del modelo
///////////////////////////////////////////////////////////////////////////////
int initTeapot(int grid, glm::mat4 transform)
{
    int verts = 32 * (grid + 1) * (grid + 1);
    int faces = grid * grid * 32;
    float * v = new float[ verts * 3 ];
    float * n = new float[ verts * 3 ];
    float * tc = new float[ verts * 2 ];
    unsigned int * el = new unsigned int[faces * 6];

    generatePatches( v, n, tc, el, grid );
	moveLid(grid, v, transform);

	// Tarea por hacer (ejer. 4): Crear y activar VAO (OK?)
	initVAO(v, n);

    unsigned int handle[4];
    glGenBuffers(4, handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, (3 * verts) * sizeof(float), v, GL_STATIC_DRAW); // Datos de la posición de los vértices
	// Tarea por hacer (ejer. 4): Establecer el puntero al atributo del vertex shader y activar el array
	GLuint loc = glGetAttribLocation(programShaderID, "aVertex");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, (3 * verts) * sizeof(float), n, GL_STATIC_DRAW); // Datos de las normales de los vértices
	// Tarea por hacer (ejer. 4): Establecer el puntero al atributo del vertex shader y activar el array
	GLuint loc2 = glGetAttribLocation(programShaderID, "aNormal");
	glEnableVertexAttribArray(loc2);
	glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
    glBufferData(GL_ARRAY_BUFFER, (2 * verts) * sizeof(float), tc, GL_STATIC_DRAW); // Datos de las coordenadas de textura
	// Tarea por hacer (ejer. 4): Establecer el puntero al atributo del vertex shader y activar el array
	GLuint loc4 = glGetAttribLocation(programShaderID, "aTexCoord");
	glEnableVertexAttribArray(loc4);
	glVertexAttribPointer(loc4, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * faces * sizeof(unsigned int), el, GL_STATIC_DRAW); // Array de índices

	//loc3 = glGetUniformLocation(programShaderID, "uModelViewProjectionMatrix");


    delete [] v;
    delete [] n;
    delete [] el;
    delete [] tc;

    // Tarea por hacer (ejer. 4): Desactivar VAO (OK?)
	glBindVertexArray(0);

	return 6 * faces;
}
// END: Inicializa primitivas ////////////////////////////////////////////////////////////////////////////////////

// BEGIN: Funciones de dibujo ////////////////////////////////////////////////////////////////////////////////////
void drawTeapot()  {
    // Tarea por hacer (ejer. 4): Dibujar la tetera
	glBindVertexArray(vaoHandle);
	glDrawElements(GL_TRIANGLES, numVertTeapot, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
// END: Funciones de dibujo ////////////////////////////////////////////////////////////////////////////////////


// BEGIN: GLUT /////////////////////////////////////////////////////////////////////////////////////////////////

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

bool init()
{

	glClearColor(0.93f, 0.93f, 0.93f, 0.0f);
 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearDepth(1.0f);

	glShadeModel(GL_SMOOTH);


	// Tarea por hacer (ejer. 4): Crear el objeto programa, compilar los shaders, adjuntarlos y linkar
	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	loadSource(vertexShaderID, "tema2_ej2.vert");
	fragmenShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	loadSource(fragmenShaderID, "tema2_ej2.frag");

	printf("VertexShader\n");
	glCompileShader(vertexShaderID);
	printCompileInfoLog(vertexShaderID);
	printf("FragmentShader\n");
	glCompileShader(fragmenShaderID);
	printCompileInfoLog(fragmenShaderID);

	programShaderID = glCreateProgram();
	glAttachShader(programShaderID, vertexShaderID);
	glAttachShader(programShaderID, fragmenShaderID);

	printf("Program Shader");
	glLinkProgram(programShaderID);
	printLinkInfoLog(programShaderID);


	numVertTeapot = initTeapot(10, glm::translate(glm::mat4(1.0f),glm::vec3(0.0f, 0.0f, -1.5f)));

	
	// Inicializa Texturas
	GLuint textIds[2];	
	glGenTextures (2, textIds);
 
	TGAFILE tgaImage;
	char stone[] = "stone.tga";
	char moss[] = "moss.tga";
	// textura piedra
	if ( LoadTGAFile(stone, &tgaImage) )
	{
	   // Tarea a realizar (ejercicio 5): Activar la unidad de textura, enlazar el objeto textura, cargar en él los datos de la imagen y definir los parámetros de la textura
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textIds[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tgaImage.imageWidth, tgaImage.imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, tgaImage.imageData);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else std::cout << "Error al cargar la textura 'stone.tga'" << std::endl;
	
	// textura musgo
	if ( LoadTGAFile(moss, &tgaImage) )
	{
	   // Tarea a realizar (ejercicio 5): Activar la unidad de textura, enlazar el objeto textura, cargar en él los datos de la imagen y definir los parámetros de la textura
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textIds[1]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tgaImage.imageWidth, tgaImage.imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, tgaImage.imageData);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else std::cout << "Error al cargar la textura 'moss.tga'" << std::endl;	
	
	// Tarea a realizar (ejercicios 4 y 5): localizar las variables uniform definidas en los shaders.
	loc3 = glGetUniformLocation(programShaderID, "uModelViewProjectionMatrix");
	locColorMatrix = glGetUniformLocation(programShaderID, "uColorRotationMatrix");
	loc5 = glGetUniformLocation(programShaderID, "uRadius");
	locUmbral = glGetUniformLocation(programShaderID, "uUmbral");
	locStoneTex = glGetUniformLocation(programShaderID, "uStoneTexture");
	locMossTex = glGetUniformLocation(programShaderID, "uMossTexture");
	locTextureSelection = glGetUniformLocation(programShaderID, "uTexSelection");

	return true;
}
 
void display()
{
	static float time = 0;
	if (KEY_P_PRESSED) 
	{
		radius += 0.1f;
	}
	else 
	{
		radius = -1.5707963267949;
	}

	if (colorRotationActivated) {
		angle += 0.5;
	}
	
	if (KEY_D_PRESSED) {
		umbralValue += 0.1;
	}
	else {
		umbralValue = 1.5f;
	}
	
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 Projection = glm::perspective(45.0f, 1.0f * g_Width / g_Height, 1.0f, 100.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
	Model = glm::rotate(Model, xrot, glm::vec3(1.0f, 0.0f, 0.0f));
	Model = glm::rotate(Model, yrot, glm::vec3(0.0f, 1.0f, 0.0f)); 
	glm::mat4 mvp = Projection * View * Model;

	glm::mat4 mColorRotation = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));

	// Tarea por hacer (ejer. 4): Activar el objeto programa
	glUseProgram(programShaderID);
	glUniformMatrix4fv(loc3, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(locColorMatrix, 1, GL_FALSE, &mColorRotation[0][0]);
	glUniform1f(loc5, radius);
	glUniform1f(locUmbral, umbralValue);
	glUniform1i(locStoneTex, 0);
	glUniform1i(locMossTex, 1);
	glUniform1i(locTextureSelection, textureSelection);
	// Tarea por hacer (ejer. 4): Pasarle al vertex shader mvp y el resto de variables uniform
	drawTeapot();
	// Tarea por hacer (ejer. 4): Desactivar el objeto programa	
	glUseProgram(0);
	glutSwapBuffers();
}
 
void resize(int w, int h)
{
	g_Width = w;
	g_Height = h;
	glViewport(0, 0, g_Width, g_Height);
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
		glDeleteProgram(programShaderID);
		exit(1); 
		break;
	case 'a': case 'A':
		animation = !animation;
		break;
	case 'r': case 'R':
		colorRotationActivated = !colorRotationActivated;
		break;
	case 'p': case 'P':
		KEY_P_PRESSED = !KEY_P_PRESSED;
		break;
	case 'd': case 'D':
		KEY_D_PRESSED = !KEY_D_PRESSED;
		break;
	case 'm': case 'M':
		textureSelection++;
		textureSelection %= 3;
		break;
	// Tarea por hacer (ejer. 4): Activar las tareas asociadas a las teclas 'p', 'r', 'd' y 'm'.
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


