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
#include "teapotdata.h"
#include "vbotorus.h"

int initSphere(float radius, unsigned int rings, unsigned int sectors);
int initTeapot(int grid, glm::mat4 transform);
int initPlane();
int initTorus(float outerRadius, float innerRadius, int nsides, int nrings);
void drawSphere();
void drawTeapot();
void drawPlane();
void drawTorus();

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
 
float xrot = 1.0f;
float yrot = 0.0f;
float xdiff = 0.0f;
float ydiff = 0.0f;

int g_Width = 500;                          // Ancho inicial de la ventana
int g_Height = 500;                         // Altura incial de la ventana

GLuint cubeVAOHandle, sphereVAOHandle, teapotVAOHandle, planeVAOHandle, torusVAOHandle;
GLuint programID;
GLuint locUniformMVPM, locUniformMVM, locUniformNM;
GLuint locUniformLightPos, locUniformLightIntensity, locUniformLightK, locUniformLightDir, locUniformLightcutOffInt, locUniformLightcutOffExt;
GLuint locUniformMaterialAmbient, locUniformMaterialDiffuse, locUniformMaterialSpecular, locUniformMaterialShininess;

int numVertTeapot, numVertSphere, numVertPlane, numVertTorus;

GLuint textIds[3];

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
// Init Sphere
// parametros: 
//		radius - radio de la esfera
//      rings - n�mero de anillos paralelos
//		sectors - numero de divisiones de los anillos
// return:
//		n�mero de vertices
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
    glBufferData(GL_ARRAY_BUFFER, (rings * sectors * 3) * sizeof(GLfloat), sphere_vertices, GL_STATIC_DRAW); // Datos de la posici�n de los v�rtices
	GLuint loc1 = glGetAttribLocation(programID, "aPosition");   
	glEnableVertexAttribArray(loc1); // Vertex position
	glVertexAttribPointer( loc1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL + 0 ); 

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, (rings * sectors * 3) * sizeof(GLfloat), sphere_normals, GL_STATIC_DRAW); // Datos de las normales de los v�rtices
	GLuint loc2 = glGetAttribLocation(programID, "aNormal");   
	glEnableVertexAttribArray(loc2); // Vertex normal
	glVertexAttribPointer( loc2, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL + 0 ); 

    glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
    glBufferData(GL_ARRAY_BUFFER, (rings * sectors * 2) * sizeof(float), sphere_texcoords, GL_STATIC_DRAW); // Datos de las coordenadas de textura
	GLuint loc3 = glGetAttribLocation(programID, "aTexCoord");   
	glEnableVertexAttribArray(loc3); // texture coords
	glVertexAttribPointer( loc3, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL + 0 ); 

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, rings * sectors * 4 * sizeof(GLushort), sphere_indices, GL_STATIC_DRAW); // Array de �ndices

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
//		grid - n�mero de rejillas
//		transform - matriz de tranformaci�n del modelo
// return:
//		n�mero de vertices
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
    glBufferData(GL_ARRAY_BUFFER, (3 * verts) * sizeof(float), v, GL_STATIC_DRAW); // Datos de la posici�n de los v�rtices
	GLuint loc1 = glGetAttribLocation(programID, "aPosition");   
    glVertexAttribPointer( loc1, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)) );
    glEnableVertexAttribArray(loc1);  // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, (3 * verts) * sizeof(float), n, GL_STATIC_DRAW); // Datos de las normales de los v�rtices
	GLuint loc2 = glGetAttribLocation(programID, "aNormal");   
    glVertexAttribPointer( loc2, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)) );
    glEnableVertexAttribArray(loc2);  // Vertex normal

    glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
    glBufferData(GL_ARRAY_BUFFER, (2 * verts) * sizeof(float), tc, GL_STATIC_DRAW); // Datos de las coordenadas de textura
	GLuint loc3 = glGetAttribLocation(programID, "aTexCoord");   
    glVertexAttribPointer( loc3, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)) );
    glEnableVertexAttribArray(loc3);  // texture coords

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * faces * sizeof(unsigned int), el, GL_STATIC_DRAW); // Array de �ndices

    delete [] v;
    delete [] n;
    delete [] el;
    delete [] tc;

    glBindVertexArray(0);

	return 6 * faces;
}

int initPlane(float xsize, float zsize, int xdivs, int zdivs)
{
    
    float * v = new float[3 * (xdivs + 1) * (zdivs + 1)];
	float * n = new float[3 * (xdivs + 1) * (zdivs + 1)];
    float * tex = new float[2 * (xdivs + 1) * (zdivs + 1)];
    unsigned int * el = new unsigned int[6 * xdivs * zdivs];

    float x2 = xsize / 2.0f;
    float z2 = zsize / 2.0f;
    float iFactor = (float)zsize / zdivs;
    float jFactor = (float)xsize / xdivs;
    float texi = 1.0f / zdivs;
    float texj = 1.0f / xdivs;
    float x, z;
    int vidx = 0, tidx = 0;
    for( int i = 0; i <= zdivs; i++ ) {
        z = iFactor * i - z2;
        for( int j = 0; j <= xdivs; j++ ) {
            x = jFactor * j - x2;
            v[vidx] = x;
            v[vidx+1] = 0.0f;
            v[vidx+2] = z;
			n[vidx] = 0.0f;
			n[vidx+1] = 1.0f;
			n[vidx+2] = 0.0f;
            vidx += 3;
            tex[tidx] = j * texi;
            tex[tidx+1] = i * texj;
            tidx += 2;
        }
    }

    unsigned int rowStart, nextRowStart;
    int idx = 0;
    for( int i = 0; i < zdivs; i++ ) {
        rowStart = i * (xdivs+1);
        nextRowStart = (i+1) * (xdivs+1);
        for( int j = 0; j < xdivs; j++ ) {
            el[idx] = rowStart + j;
            el[idx+1] = nextRowStart + j;
            el[idx+2] = nextRowStart + j + 1;
            el[idx+3] = rowStart + j;
            el[idx+4] = nextRowStart + j + 1;
            el[idx+5] = rowStart + j + 1;
            idx += 6;
        }
    }

    unsigned int handle[4];
    glGenBuffers(4, handle);

	glGenVertexArrays( 1, &planeVAOHandle );
    glBindVertexArray(planeVAOHandle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 3 * (xdivs+1) * (zdivs+1) * sizeof(float), v, GL_STATIC_DRAW);
	GLuint loc1 = glGetAttribLocation(programID, "aPosition");   
    glVertexAttribPointer( loc1, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)) );
    glEnableVertexAttribArray(loc1);  // Vertex position

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 3 * (xdivs+1) * (zdivs+1) * sizeof(float), n, GL_STATIC_DRAW);
	GLuint loc2 = glGetAttribLocation(programID, "aNormal");   
    glVertexAttribPointer( loc2, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)) );
    glEnableVertexAttribArray(loc2);  // Vertex normal

    glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
    glBufferData(GL_ARRAY_BUFFER, 2 * (xdivs+1) * (zdivs+1) * sizeof(float), tex, GL_STATIC_DRAW);
	GLuint loc3 = glGetAttribLocation(programID, "aTexCoord");   
    glVertexAttribPointer( loc3, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)) );
    glEnableVertexAttribArray(loc3);  // texture coords

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * xdivs * zdivs * sizeof(unsigned int), el, GL_STATIC_DRAW);

    glBindVertexArray(0);
    
    delete [] v;
	delete [] n;
    delete [] tex;
    delete [] el;

	return 6 * xdivs * zdivs;
}

int initTorus(float outerRadius, float innerRadius, int nsides, int nrings) 
{
    int faces = nsides * nrings;
    int nVerts  = nsides * (nrings+1);   // One extra ring to duplicate first ring

    // Verts
    float * v = new float[3 * nVerts];
    // Normals
    float * n = new float[3 * nVerts];
    // Tex coords
    float * tex = new float[2 * nVerts];
    // Elements
    unsigned int * el = new unsigned int[6 * faces];

    // Generate the vertex data
    generateVerts(v, n, tex, el, outerRadius, innerRadius, nrings, nsides);

    // Create and populate the buffer objects
    unsigned int handle[4];
    glGenBuffers(4, handle);

    // Create the VAO
    glGenVertexArrays( 1, &torusVAOHandle );
    glBindVertexArray(torusVAOHandle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, (3 * nVerts) * sizeof(float), v, GL_STATIC_DRAW);
	GLuint loc1 = glGetAttribLocation(programID, "aPosition");   
    glVertexAttribPointer( loc1, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)) );
    glEnableVertexAttribArray(loc1);  // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, (3 * nVerts) * sizeof(float), n, GL_STATIC_DRAW);
	GLuint loc2 = glGetAttribLocation(programID, "aNormal");   
    glVertexAttribPointer( loc2, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)) );
    glEnableVertexAttribArray(loc2);  // Vertex normal

    glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
    glBufferData(GL_ARRAY_BUFFER, (2 * nVerts) * sizeof(float), tex, GL_STATIC_DRAW);
	GLuint loc3 = glGetAttribLocation(programID, "aTexCoord");   
    glVertexAttribPointer( loc3, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)) );
    glEnableVertexAttribArray(loc3);  // texture coords

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * faces * sizeof(unsigned int), el, GL_STATIC_DRAW);

	glBindVertexArray(0);

    delete [] v;
    delete [] n;
    delete [] el;
    delete [] tex;

	return 6 * faces;
}

// END: Inicializa primitivas ////////////////////////////////////////////////////////////////////////////////////

// BEGIN: Funciones de dibujo ////////////////////////////////////////////////////////////////////////////////////

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

void drawPlane() {
    glBindVertexArray(planeVAOHandle);
    glDrawElements(GL_TRIANGLES, numVertPlane, GL_UNSIGNED_INT, ((GLubyte *)NULL + (0)));
	glBindVertexArray(0);
}

void drawTorus() {
    glBindVertexArray(torusVAOHandle);
    glDrawElements(GL_TRIANGLES, numVertTorus, GL_UNSIGNED_INT, ((GLubyte *)NULL + (0)));
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
	loadSource(vertexShaderID, "tema3_parte2.vert");
	std::cout << "Compilando shader de vertice ..." << std::endl;
	glCompileShader(vertexShaderID);
	printCompileInfoLog(vertexShaderID);
	glAttachShader(programID, vertexShaderID);

	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	loadSource(fragmentShaderID, "tema3_parte2.frag");
	std::cout << "Compilando shader de fragmento ..." << std::endl;
	glCompileShader(fragmentShaderID);
	printCompileInfoLog(fragmentShaderID);
	glAttachShader(programID, fragmentShaderID);

	glLinkProgram(programID);
	printLinkInfoLog(programID);
	validateProgram(programID);

	numVertTeapot = initTeapot(5, glm::mat4(1.0f));
	numVertSphere = initSphere(1.0f, 20, 30);
	numVertPlane = initPlane(10.0f, 10.0f, 2, 2);
	numVertTorus = initTorus(0.5f, 0.25f, 20, 40);

	// Localizaci�n del ID de las variables uniform
	locUniformMVPM = glGetUniformLocation(programID, "uModelViewProjMatrix");
	locUniformMVM = glGetUniformLocation(programID, "uModelViewMatrix");
	locUniformNM = glGetUniformLocation(programID, "uNormalMatrix");

	// Cuando las variables uniform son una estructura hay que localizar por separado cada campo
	locUniformLightPos = glGetUniformLocation(programID, "uLight.lightPos");
	locUniformLightIntensity = glGetUniformLocation(programID, "uLight.intensity");
	locUniformLightK = glGetUniformLocation(programID, "uLight.k");
	locUniformLightDir = glGetUniformLocation(programID, "uLight.dir");
	locUniformLightcutOffInt = glGetUniformLocation(programID, "uLight.cutOffInt");
	locUniformLightcutOffExt = glGetUniformLocation(programID, "uLight.cutOffExt");
	
	// Tarea por hacer: localizar el id para las propiedades del material.

	locUniformMaterialAmbient = glGetUniformLocation(programID, "uMaterial.ambient");
	locUniformMaterialDiffuse = glGetUniformLocation(programID, "uMaterial.diffuse");
	locUniformMaterialSpecular = glGetUniformLocation(programID, "uMaterial.specular");
	locUniformMaterialShininess = glGetUniformLocation(programID, "uMaterial.shininess");


	return true;
}
 
void display()
{
	static float DEGREE_TO_RADIANS = 3.141592 / 180;
	static int angle_degree = 0;
	angle_degree = (angle_degree + 1) % 360;
	float angle_radians = angle_degree * DEGREE_TO_RADIANS;

	struct LightInfo {
	 glm::vec4 lightPos; // posici�n de la luz
	 glm::vec3 intensity;
	 glm::vec3 k;
	 glm::vec3 dir; // direcci�n del foco
	 GLfloat cutOffInt; // �ngulo interior (�ngulo l�mite de la zona iluminada)
	 GLfloat cutOffExt; // �ngulo exterior (�ngulo l�mite de la zona de penumbra)
	};
	LightInfo light = { glm::vec4(3*cos(angle_radians), 2.5f, 3*sin(angle_radians), 1.0f),  // Pos. S.R. mundo
						glm::vec3(1.0f, 1.0f, 1.0f), 
						glm::vec3(1.0f, 0.0f, 0.0f), 
						glm::vec3(-3*cos(angle_radians), -2.0f, -3*sin(angle_radians)), // Dir. S.R. mundo
						35.0f,
						40.0f
	};

	struct MaterialInfo {
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		GLfloat shininess;
	};
	MaterialInfo gold = {glm::vec3(0.24725f, 0.1995f, 0.0745f), glm::vec3(0.75164f, 0.60648f, 0.22648f), glm::vec3(0.628281f, 0.555802f, 0.366065f), 52.0f};
	MaterialInfo perl = {glm::vec3(0.25f, 0.20725f, 0.20725f), glm::vec3(1.0f, 0.829f, 0.829f), glm::vec3(0.296648f, 0.296648f, 0.296648f), 12.0f};
	MaterialInfo bronze = {glm::vec3(0.2125f, 0.1275f, 0.054f), glm::vec3(0.714f, 0.4284f, 0.18144f), glm::vec3(0.393548f, 0.271906f, 0.166721f), 25.0f};
	MaterialInfo brass = {glm::vec3(0.329412f, 0.223529f, 0.027451f), glm::vec3(0.780392f, 0.568627f, 0.113725f), glm::vec3(0.992157f, 0.941176f, 0.807843f), 28.0f};
	MaterialInfo emerald = {glm::vec3(0.0215f, 0.1745f, 0.0215f), glm::vec3(0.07568f, 0.61424f, 0.07568f), glm::vec3(0.633f, 0.727811f, 0.633f), 28.0f};

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 Projection = glm::perspective(45.0f, 1.0f * g_Width / g_Height, 1.0f, 100.0f);
	
	glm::vec3 cameraPos = vec3( 5.0f * sin( xrot / 200 ) * cos( yrot / 100 ), 2.0f * cos(xrot / 200) + 3.0f, 5.0f * sin( xrot / 200 ) * sin( yrot / 100 ) );
	glm::mat4 View = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 ModelPlane = glm::translate(glm::scale(glm::mat4(1.0), glm::vec3(1.0f, 1.0f, 1.0f)),vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 ModelSphere = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f)), vec3(-3.0f,1.0f, 3.0f));
	glm::mat4 ModelTeapot = glm::translate(glm::rotate(glm::scale(glm::mat4(1.0f),vec3(0.25, 0.25, 0.25)), -90.0f, vec3(1.0, 0.0, 0.0)), vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 ModelTorus = glm::translate(glm::rotate(glm::mat4(1.0f), -90.0f, vec3(1.0, 0.0, 0.0)), vec3(2.0f, 0.0f, 0.25f));

	glm::mat4 mvp; // Model-view-projection matrix
	glm::mat4 mv;  // Model-view matrix
	glm::mat3 nm;  // Normal matrix

	glUseProgram(programID);
	
	// Paso al shader de las propiedades de la luz
	mv = View;
	nm = glm::mat3(glm::transpose(glm::inverse(mv)));
	glm::vec4 lpos = mv * light.lightPos;	// Posici�n de la luz puntual/foco en el S.R. vista
	glm::vec3 ldir = nm * light.dir;		// Direcci�n del foco en el S.R. vista
	glUniform4fv(locUniformLightPos, 1, &(lpos.x));
	glUniform3fv(locUniformLightIntensity, 1, &(light.intensity.r));
	glUniform3fv(locUniformLightK, 1, &(light.k[0]));
	glUniform3fv(locUniformLightDir, 1, &(ldir.x));
	glUniform1f(locUniformLightcutOffInt, light.cutOffInt);
	glUniform1f(locUniformLightcutOffExt, light.cutOffExt);


	// Tarea por hacer: paso al shader de las matrices y las propiedades del material (gold)
	glUniformMatrix4fv(locUniformMVPM, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(locUniformMVM, 1, GL_FALSE, &mv[0][0]);
	glUniformMatrix3fv(locUniformNM, 1, GL_FALSE, &nm[0][0]);

	glUniform3fv(locUniformMaterialAmbient, 1, &gold.ambient[0]);
	glUniform3fv(locUniformMaterialDiffuse, 1, &gold.diffuse[0]);
	glUniform3fv(locUniformMaterialSpecular, 1, &gold.specular[0]);
	glUniform1f(locUniformMaterialShininess, gold.shininess);

	// Dibuja Esfera.
	drawSphere();

	// Tarea por hacer: paso al shader de las matrices y las propiedades del material (brass)
	glUniformMatrix4fv(locUniformMVPM, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(locUniformMVM, 1, GL_FALSE, &mv[0][0]);
	glUniformMatrix3fv(locUniformNM, 1, GL_FALSE, &nm[0][0]);

	glUniform3fv(locUniformMaterialAmbient, 1, &brass.ambient[0]);
	glUniform3fv(locUniformMaterialDiffuse, 1, &brass.diffuse[0]);
	glUniform3fv(locUniformMaterialSpecular, 1, &brass.specular[0]);
	glUniform1f(locUniformMaterialShininess, brass.shininess);

	// Dibuja Tetera
	drawTeapot();

	// Tarea por hacer: paso al shader de las matrices y las propiedades del material (emerald)
	glUniformMatrix4fv(locUniformMVPM, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(locUniformMVM, 1, GL_FALSE, &mv[0][0]);
	glUniformMatrix3fv(locUniformNM, 1, GL_FALSE, &nm[0][0]);

	glUniform3fv(locUniformMaterialAmbient, 1, &emerald.ambient[0]);
	glUniform3fv(locUniformMaterialDiffuse, 1, &emerald.diffuse[0]);
	glUniform3fv(locUniformMaterialSpecular, 1, &emerald.specular[0]);
	glUniform1f(locUniformMaterialShininess, emerald.shininess);
	// Dibuja Donut
	drawTorus();

	// Tarea por hacer: paso al shader de las matrices y las propiedades del material (perl)
	glUniformMatrix4fv(locUniformMVPM, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(locUniformMVM, 1, GL_FALSE, &mv[0][0]);
	glUniformMatrix3fv(locUniformNM, 1, GL_FALSE, &nm[0][0]);

	glUniform3fv(locUniformMaterialAmbient, 1, &perl.ambient[0]);
	glUniform3fv(locUniformMaterialDiffuse, 1, &perl.diffuse[0]);
	glUniform3fv(locUniformMaterialSpecular, 1, &perl.specular[0]);
	glUniform1f(locUniformMaterialShininess, perl.shininess);
	// Dibuja Plano
	drawPlane();

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
