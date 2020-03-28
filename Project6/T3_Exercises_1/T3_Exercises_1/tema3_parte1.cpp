// Ejercicios 1 y 2 (parte 1) del tema 3

#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <string>
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "vboteapot.h"
#include "teapotdata.h"
#include "tga.h"

void initCube();
int initSphere(float, unsigned int, unsigned int);
int initTeapot(int, glm::mat4);
void drawCube();
void drawSphere();
void drawTeapot();

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
bool animation = true;
 
float xrot = 0.0f;
float yrot = 0.0f;
float xdiff = 0.0f;
float ydiff = 0.0f;

int g_Width = 500;                          // Ancho inicial de la ventana
int g_Height = 500;                         // Altura incial de la ventana

GLuint cubeVAOHandle;
GLuint sphereVAOHandle;
GLuint teapotVAOHandle;
GLuint programID;
GLuint locUniformMap;
GLuint locUniformMVPM, locUniformMVM, locUniformMM, locUniformNM;
GLuint locUniformDrawSky, locUniformCamera, locUniformChromatic, locUniformFresnel;

int numVertTeapot, numVertSphere;

GLuint TEXTURE_NUMBER = 1, drawSky, applyChromaticDispersion, applyFresnel;
GLboolean APPLY_CRYSTAL_MAT = false, ACTIVATE_CHROMATIC_DISPERSION = false, ACTIVATE_FRESNEL = false;
const std::string TEXTURE_CUBE_MAP_ARRAY[] = {"posx", "negx", "posy", "negy", "posz", "negz"};
const GLenum TEXTURE_CUBE_MAP_ENUM[] = {
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

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
// vertice tiene 4 components (x,y,z) de reales, por tanto, el tamaño del vertex
// array es 144 floats (36 * 4 = 144).
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

GLfloat normales1[] = { 0, 0, 1,   0, 0, 1,   0, 0, 1,      // v0-v1-v2 (front)
                        0, 0, 1,   0, 0, 1,   0, 0, 1,      // v2-v3-v0

                        1, 0, 0,   1, 0, 0,   1, 0, 0,      // v0-v3-v4 (right)
                        1, 0, 0,   1, 0, 0,   1, 0, 0,      // v4-v5-v0

                        0, 1, 0,   0, 1, 0,   0, 1, 0,      // v0-v5-v6 (top)
                        0, 1, 0,   0, 1, 0,   0, 1, 0,      // v6-v1-v0

                       -1, 0, 0,  -1, 0, 0,  -1, 0, 0,      // v1-v6-v7 (left)
                       -1, 0, 0,  -1, 0, 0,  -1, 0, 0,      // v7-v2-v1

                        0,-1, 0,   0,-1, 0,   0,-1, 0,      // v7-v4-v3 (bottom)
                        0,-1, 0,   0,-1, 0,   0,-1, 0,      // v3-v2-v7

                        0, 0,-1,   0, 0,-1,   0, 0,-1,      // v4-v7-v6 (back)
						0, 0,-1,   0, 0,-1,   0, 0,-1 };    // v6-v5-v4

GLfloat texCoord1[] = { 1, 1,	0, 1,	0, 0,		// v0-v1-v2 (front)
						0, 0,	1, 0,	1, 1,		// v2-v3-v0

                        1, 1,   1, 0,   0, 0,		// v0-v3-v4 (right)
                        0, 0,   0, 1,   1, 1,		// v4-v5-v0

                        1, 0,   1, 1,	0, 1,		// v0-v5-v6 (top)
						0, 1,	0, 0,	1, 0,		// v6-v1-v0

						0, 1,	1, 1,	1, 0,		// v1-v6-v7 (left)
						1, 0,	0, 0,	0, 1,		// v7-v2-v1

						0, 0,   1, 0,   1, 1,		// v7-v4-v3 (bottom)
                        1, 1,	0, 1,	0, 0,		// v3-v2-v7

						0, 0,	1, 0,	1, 1,		// v4-v7-v6 (back)
						1, 1,	0, 1,   0, 0 };		// v6-v5-v4

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

// BEGIN: Inicializa primitivas ////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Init para drawCube_VAO
///////////////////////////////////////////////////////////////////////////////
void initCube() 
{
GLuint vboHandle;

	glGenVertexArrays (1, &cubeVAOHandle);
	glBindVertexArray (cubeVAOHandle);

	glGenBuffers(1, &vboHandle); 
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle);    
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1) + sizeof(normales1) + sizeof(texCoord1), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices1), vertices1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices1), sizeof(normales1), normales1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices1) + sizeof(normales1), sizeof(texCoord1), texCoord1);

	// Tarea a realizar (ejercicio 1): Localizar las variables in del vertex shader para la posición,
	// la normal y las coordenadas de textura, habilitar estos atributos y establecer los punteros

	GLuint locPosition = glGetAttribLocation(programID, "aPosition");
	glEnableVertexAttribArray(locPosition);
	glVertexAttribPointer(locPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint locNormal = glGetAttribLocation(programID, "aNormal");
	glEnableVertexAttribArray(locNormal);
	glVertexAttribPointer(locNormal, 3, GL_FLOAT, GL_FALSE, 0, (char*)NULL + sizeof(vertices1));

	GLuint locTexture = glGetAttribLocation(programID, "aTexCoord");
	glEnableVertexAttribArray(locTexture);
	glVertexAttribPointer(locTexture, 2, GL_FLOAT, GL_FALSE, 0, (char*)NULL + sizeof(vertices1) + sizeof(normales1));

	glBindVertexArray (0);
}

///////////////////////////////////////////////////////////////////////////////
// Init Sphere
// parametros: 
//		radius - radio de la esfera
//      rings - número de anillos paralelos
//		sectors - numero de divisiones de los anillos
// return:
//		número de vertices
///////////////////////////////////////////////////////////////////////////////
int initSphere(float radius, unsigned int rings, unsigned int sectors)
{
    const float R = 1.0f/(float)(rings-1);
    const float S = 1.0f/(float)(sectors-1);
	const double PI = 3.14159265358979323846;

    GLfloat *sphere_vertices = new GLfloat[rings * sectors * 3];
    GLfloat *sphere_normals = new GLfloat[rings * sectors * 3];
    GLfloat *sphere_texcoords = new GLfloat[rings * sectors * 2];
    GLfloat *v = sphere_vertices;
    GLfloat *n = sphere_normals;
    GLfloat *t = sphere_texcoords;
    for(unsigned int r = 0; r < rings; r++) for(unsigned int s = 0; s < sectors; s++) {
            float const y = float( sin( -PI/2 + PI * r * R ) );
            float const x = float( cos(2*PI * s * S) * sin( PI * r * R ) );
            float const z = float( sin(2*PI * s * S) * sin( PI * r * R ) );

            *t++ = s*S;
            *t++ = r*R;

            *v++ = x * radius;
            *v++ = y * radius;
            *v++ = z * radius;

            *n++ = x;
            *n++ = y;
            *n++ = z;
    }

    GLushort *sphere_indices = new GLushort[rings * sectors * 4];
    GLushort *i = sphere_indices;
    for(unsigned int r = 0; r < rings; r++) for(unsigned int s = 0; s < sectors; s++) {
            *i++ = r * sectors + s;
            *i++ = r * sectors + (s+1);
            *i++ = (r+1) * sectors + (s+1);
            *i++ = (r+1) * sectors + s;
    }

    glGenVertexArrays( 1, &sphereVAOHandle );
    glBindVertexArray(sphereVAOHandle);

    unsigned int handle[4];
    glGenBuffers(4, handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, (rings * sectors * 3) * sizeof(GLfloat), sphere_vertices, GL_STATIC_DRAW); // Datos de la posición de los vértices
	// Tarea a realizar (ejercicio 1): Localizar la variable in del vertex shader para la posición,
	// habilitar el atributo y establecer el puntero
	GLuint locPosition = glGetAttribLocation(programID, "aPosition");
	glEnableVertexAttribArray(locPosition);
	glVertexAttribPointer(locPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, (rings * sectors * 3) * sizeof(GLfloat), sphere_normals, GL_STATIC_DRAW); // Datos de las normales de los vértices
	// Tarea a realizar (ejercicio 1): idem para la normal.
	GLuint locNormal = glGetAttribLocation(programID, "aNormal");
	glEnableVertexAttribArray(locNormal);
	glVertexAttribPointer(locNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
    glBufferData(GL_ARRAY_BUFFER, (rings * sectors * 2) * sizeof(float), sphere_texcoords, GL_STATIC_DRAW); // Datos de las coordenadas de textura
	// Tarea a realizar (ejercicio 1): idem para la las coordenadas de textura.
	GLuint locTexture = glGetAttribLocation(programID, "aTexCoord");
	glEnableVertexAttribArray(locTexture);
	glVertexAttribPointer(locTexture, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, rings * sectors * 4 * sizeof(GLushort), sphere_indices, GL_STATIC_DRAW); // Array de índices

    delete [] sphere_vertices;
    delete [] sphere_normals;
    delete [] sphere_texcoords;
    delete [] sphere_indices;

    glBindVertexArray(0);

	return rings * sectors * 4;
}


///////////////////////////////////////////////////////////////////////////////
// Init Teapot
// parametros: 
//		grid - número de rejillas
//		transform - matriz de tranformación del modelo
// return:
//		número de vertices
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

    glGenVertexArrays( 1, &teapotVAOHandle );
    glBindVertexArray(teapotVAOHandle);

    unsigned int handle[4];
    glGenBuffers(4, handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, (3 * verts) * sizeof(float), v, GL_STATIC_DRAW); // Datos de la posición de los vértices
	GLuint loc1 = glGetAttribLocation(programID, "aPosition");   
    glVertexAttribPointer( loc1, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)) );
    glEnableVertexAttribArray(loc1);  // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, (3 * verts) * sizeof(float), n, GL_STATIC_DRAW); // Datos de las normales de los vértices
	GLuint loc2 = glGetAttribLocation(programID, "aNormal");   
    glVertexAttribPointer( loc2, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)) );
    glEnableVertexAttribArray(loc2);  // Vertex normal

    glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
    glBufferData(GL_ARRAY_BUFFER, (2 * verts) * sizeof(float), tc, GL_STATIC_DRAW); // Datos de las coordenadas de textura
	GLuint loc3 = glGetAttribLocation(programID, "aTexCoord");   
    glVertexAttribPointer( loc3, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)) );
    glEnableVertexAttribArray(loc3);  // texture coords

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * faces * sizeof(unsigned int), el, GL_STATIC_DRAW); // Array de índices

    delete [] v;
    delete [] n;
    delete [] el;
    delete [] tc;

    glBindVertexArray(0);

	return 6 * faces;
}

// END: Inicializa primitivas ////////////////////////////////////////////////////////////////////////////////////

// BEGIN: Funciones de dibujo ////////////////////////////////////////////////////////////////////////////////////

void drawCube()
{
	glBindVertexArray(cubeVAOHandle);
    glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void drawTeapot()  {
    glBindVertexArray(teapotVAOHandle);
    glDrawElements(GL_TRIANGLES, numVertTeapot, GL_UNSIGNED_INT, ((GLubyte *)NULL + (0)));
	glBindVertexArray(0);
}

void drawSphere()  {
    glBindVertexArray(sphereVAOHandle);
    glDrawElements(GL_QUADS, numVertSphere, GL_UNSIGNED_SHORT, ((GLubyte *)NULL + (0)));
	glBindVertexArray(0);
}

// END: Funciones de dibujo ////////////////////////////////////////////////////////////////////////////////////

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

	programID = glCreateProgram();

	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	loadSource(vertexShaderID, "tema3_parte1.vert");
	glCompileShader(vertexShaderID);
	printCompileInfoLog(vertexShaderID);
	glAttachShader(programID, vertexShaderID);

	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	loadSource(fragmentShaderID, "tema3_parte1.frag");
	glCompileShader(fragmentShaderID);
	printCompileInfoLog(fragmentShaderID);
	glAttachShader(programID, fragmentShaderID);

	glLinkProgram(programID);
	printLinkInfoLog(programID);
	validateProgram(programID);

	initCube();
	numVertTeapot = initTeapot(10, glm::mat4(1.0f));
	numVertSphere = initSphere(1.0f, 30, 60);

	// Inicializa Texturas
	TGAFILE tgaImage;
	GLuint textId;
 
	// Mapa de entorno
	// Tarea a realizar (ejercicio 1):cargar el mapa de entorno y definir los parámetros de la textura
	glGenTextures(1, &textId);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textId);

	std::string textureFile;
	for (unsigned i = 0; i < 6; ++i) {
		textureFile = "texturas/t" + std::to_string(TEXTURE_NUMBER) + TEXTURE_CUBE_MAP_ARRAY[i] + ".tga";
		if (LoadTGAFile(const_cast<char*>(textureFile.c_str()), &tgaImage)) {
			glTexImage2D(TEXTURE_CUBE_MAP_ENUM[i], 0, GL_RGB, tgaImage.imageWidth, tgaImage.imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, tgaImage.imageData);
		}
		else {
			std::cout << "Error loading: texturas/t" + std::to_string(TEXTURE_NUMBER) + TEXTURE_CUBE_MAP_ARRAY[i] + ".tga";
		}
	}
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	locUniformCamera = glGetUniformLocation(programID, "uCameraPos");
	// Tarea a realizar (ejercicios 1 y 2): localizar el resto de varaibles uniform definidas en los shaders.
	
	locUniformMVPM = glGetUniformLocation(programID, "uModelViewProjMatrix");
	locUniformMVM = glGetUniformLocation(programID, "uModelViewMatrix");
	locUniformMM = glGetUniformLocation(programID, "uModelMatrix");
	locUniformNM = glGetUniformLocation(programID, "uNormalMatrix");
	locUniformCamera = glGetUniformLocation(programID, "uCameraPos");
	locUniformDrawSky = glGetUniformLocation(programID, "uDrawSky");
	locUniformMap = glGetUniformLocation(programID, "uTexMap");
	locUniformChromatic = glGetUniformLocation(programID, "uApplyChromaticDispersion");
	locUniformFresnel = glGetUniformLocation(programID, "uApplyFresnel");

	return true;
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 Projection = glm::perspective(45.0f, 1.0f * g_Width / g_Height, 1.0f, 100.0f);
	
	glm::vec3 cameraPos = vec3( 7.0f * cos( yrot / 100 ), 2.0f * sin(xrot / 100), 7.0f * sin( yrot / 100 ) * cos(xrot /100) );
	glm::mat4 View = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	glm::mat4 ModelSky = glm::scale(glm::mat4(1.0), glm::vec3(25.0f, 25.0f, 25.0f));
	glm::mat4 ModelCube = glm::translate(glm::scale(glm::mat4(1.0), glm::vec3(0.4f, 0.4f, 0.4f)),vec3(-3.0f, 0.0f, 3.0f));
	glm::mat4 ModelSphere = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
	glm::mat4 ModelTeapot = glm::translate(glm::rotate(glm::scale(glm::mat4(1.0f),vec3(0.25, 0.25, 0.25)), -90.0f, vec3(1.0, 0.0, 0.0)), vec3(4.0f, 0.0f, 2.0f));

	glm::mat4 mvp;
	glm::mat4 mv;
	glm::mat3 nm;

	glUseProgram(programID);
	glUniform3f (locUniformCamera, cameraPos.x, cameraPos.y, cameraPos.z);
	// Tareas a realizar (ejercicio 1 y 2): establecer los valores para las variables uniform de tipo sampler

	drawSky = 0;
	applyChromaticDispersion = 0;
	applyFresnel = 0;

	glUniform3fv(locUniformCamera, 1, &cameraPos[0]);
	glUniform1i(locUniformMap, 0);

// Tarea a realizar (ejercicio 1b): descomentar el código

	// Dibuja el entorno
	mvp = Projection * View * ModelSky;
	mv = View * ModelSky;
	nm = glm::mat3(glm::transpose(glm::inverse(mv)));
	// Tarea a realizar: pasarle a las variables uniform las matrices
	glUniformMatrix4fv(locUniformMVPM, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(locUniformMM, 1, GL_FALSE, &ModelSky[0][0]);
	glUniformMatrix3fv(locUniformNM, 1, GL_FALSE, &nm[0][0]);
	glUniform1i(locUniformDrawSky, drawSky);

	drawCube(); // Cubo donde se pega la textura del entorno

	drawSky = APPLY_CRYSTAL_MAT ? 2 : 1;
	applyChromaticDispersion = ACTIVATE_CHROMATIC_DISPERSION ? 1 : 0;
	applyFresnel = ACTIVATE_FRESNEL ? 1 : 0;

	// Dibuja la esfera que refleja/refracta el entorno
	mvp = Projection * View * ModelSphere;
	mv = View * ModelSphere;
	nm = glm::mat3(glm::transpose(glm::inverse(ModelSphere)));
	// Tarea a realizar (ejercicio 1): pasarle a las variables uniform las matrices
	glUniformMatrix4fv(locUniformMVPM, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(locUniformMM, 1, GL_FALSE, &ModelSphere[0][0]);
	glUniformMatrix3fv(locUniformNM, 1, GL_FALSE, &nm[0][0]);
	glUniform1i(locUniformDrawSky, drawSky);
	glUniform1i(locUniformChromatic, applyChromaticDispersion);
	glUniform1i(locUniformFresnel, applyFresnel);

	drawSphere();

	// Dibuja la tetera
	mvp = Projection * View * ModelTeapot;
	mv = View * ModelTeapot;
	nm = glm::mat3(glm::transpose(glm::inverse(ModelTeapot)));
	// Tarea a realizar (ejercicio 1): pasarle a las variables uniform las matrices
	glUniformMatrix4fv(locUniformMVPM, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(locUniformMM, 1, GL_FALSE, &ModelTeapot[0][0]);
	glUniformMatrix3fv(locUniformNM, 1, GL_FALSE, &nm[0][0]);
	glUniform1i(locUniformDrawSky, drawSky);
	glUniform1i(locUniformChromatic, applyChromaticDispersion);
	glUniform1i(locUniformFresnel, applyFresnel);

	drawTeapot();

	// Dibuja el cubo que refleja/refracta el entorno
	mvp = Projection * View * ModelCube;
	mv = View * ModelCube;
	nm = glm::mat3(glm::transpose(glm::inverse(ModelCube)));
	// Tarea a realizar (ejercicio 1): pasarle a las variables uniform las matrices
	glUniformMatrix4fv(locUniformMVPM, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(locUniformMM, 1, GL_FALSE, &ModelCube[0][0]);
	glUniformMatrix3fv(locUniformNM, 1, GL_FALSE, &nm[0][0]);
	glUniform1i(locUniformDrawSky, drawSky);
	glUniform1i(locUniformChromatic, applyChromaticDispersion);
	glUniform1i(locUniformFresnel, applyFresnel);

	drawCube();

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
		exit(1); 
		break;
	case 'a': case 'A':
		animation = !animation;
		break;
	case 't': case 'T':
		APPLY_CRYSTAL_MAT = !APPLY_CRYSTAL_MAT;
		break;
	case 'c': case 'C':
		ACTIVATE_CHROMATIC_DISPERSION = !ACTIVATE_CHROMATIC_DISPERSION;
		break;
	case 'f': case 'F':
		ACTIVATE_FRESNEL = !ACTIVATE_FRESNEL;
		break;
	}
	// Tarea a realizar (ejercicio 2): activar la tecla 'R'
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
