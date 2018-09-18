#define GLEW_STATIC

#include <windows.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include "Shader.h"
#include <vector>
#include <random>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
using namespace glm;
using namespace std;

void display();

GLuint VBO, VAO;

const int max_num_bubbles = 10;

vec2 p[max_num_bubbles];
vec2 v[max_num_bubbles];
GLfloat r[max_num_bubbles];
GLfloat vamp[max_num_bubbles];


bool adj[max_num_bubbles][max_num_bubbles];

int count = 0;

int num_bubbles = 0;
bool make_bubble_mode = false;

float Pi = 3.14159;

float t = 0;
float dt = .001;
Shader *pass1;

/*** physical constants ***/
const float k1 = 0;
const float k2 = 0;
const float gm1 = 0;
const float gm2 = 0;
const float gm3 = 0;
const float g = 0;

std::default_random_engine generator;
std::normal_distribution<double> distribution(0,1);

inline float pwf(vec2 v, int i) { return dot(v-p[i],v-p[i])/r[i] - r[i]; }

void compute_adj() {
	for (int i = 0; i < num_bubbles; i++) {
		for (int j = i+1; j < num_bubbles; j++) {
			vec2 disp = p[j] - p[i];
			float L = length(disp);
			float r1 = r[i]; float r2 = r[j];				
			if ( L < r1+r2 ) {
				vec2 ndisp = normalize(disp);
				
				float mid = (L*r1 - sqrt(r1)*sqrt(r2)*sqrt((L + r1 - r2)*(L - r1 + r2)))/(r1 - r2);
				if ( r1 == r2 ) mid = L/2.0;
				
				vec2 midp = mid*ndisp + p[i];
				float pw = pwf(midp,i);
				
				bool tadj = true; 
			
				for (int k = 0; k < num_bubbles; k++) {
					if ( i != k &&  j != k ) {
						
						if ( pwf(midp,k) < pw ) {
							tadj = false;
							break;
						}
					}
				}
				
				if ( tadj ) { 
				
					if ( !adj[i][j] ) {
						vamp[i] += .5;
						vamp[j] += .5;
					}
					adj[i][j] = true;
				}
			}
		}
	}
}


void physics() {
	
	// compute adjacency matrix by hand
	compute_adj();
	
	// forces
	for (int i = 0; i < num_bubbles; i++) {
		for (int j = i+1; j < num_bubbles; j++) {
			if ( adj[i][j] ) {
				vec2 disp = p[i] - p[j];
				float L = length(disp);
				vec2 ndisp = normalize(disp);
			
				float L0 = sqrt(r[i]*r[i] + r[j]*r[j] - r[i]*r[j]);
				
				float f = (L0-L);
				if ( f > 0 ) {
					f *= 100000.0f;
				} else {
					f *= 2000.0f;
				}			
				vec2 force = (f-200*dot(v[i]-v[j],ndisp))*ndisp - 3.0f*(v[i]-v[j]);
				v[i] += force/(r[i]*r[i]);
				v[j] -= force/(r[j]*r[j]);
			}
		}
		/*
		if ( p[i].y < r[i] ) {
			float f = 1/3.0*r[i] - p[i].y;
			
			if ( f > 0 ) {
					f *= 10000.0f;
			} else {
					f *= 1000.0f;
			}
			vec2 force = vec2(0,(f-200*v[i].y)) - vec2(20.0f*v[i].x,0);
			v[i] += force/(r[i]*r[i]);
		}*/
	}

	// area correction;
	for (int i = 0; i < num_bubbles; i++) {
		
		for (int j = i+1; j < num_bubbles; j++) {
			if ( adj[i][j] ) {
				vec2 disp = p[i] - p[j];
				float L = length(disp);
				vec2 ndisp = normalize(disp);
				
				float dldt = dot(v[i]-v[j],ndisp);
				
				float r1 = r[i]; float r2 = r[j];
				float dr1dt = -dldt*((sqrt(2)*sqrt(r1+r2-L)*r1*pow(r2,3))/(Pi*pow(r1*r2*(r1 + r2),1.5)));
				float dr2dt = -dldt*((sqrt(2)*sqrt(r1+r2-L)*r2*pow(r1,3))/(Pi*pow(r1*r2*(r1 + r2),1.5)));

				r[i] += std::max(0.0f,.5f*dt*dr1dt);
				r[j] += std::max(0.0f,.5f*dt*dr2dt);
			}
		}
	}
	
	
	for (int i = 0; i < num_bubbles; i++) {
		vec2 rv = 3.0f/(r[i]*r[i])*vec2(float(distribution(generator)),float(distribution(generator)));
		v[i] += rv;
		vamp[i] += dot(rv,rv)/10.0;
	}
	
	for (int i = 0; i < num_bubbles; i++) {
		//v[i] -= dt*vec2(0,10.0);
		vamp[i] *= .9996f;
		v[i] *= .9997f;
	}

	if ( make_bubble_mode ) {
		v[num_bubbles-1] = vec2(0,0);
		r[num_bubbles-1] += .03f;
	}
	
	for (int i = 0; i < num_bubbles; i++) {	
		p[i] += dt*v[i];
	}
	

}

void display()
{
	
	physics();
	physics();
	physics();
	physics();

	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glBindVertexArray(VAO);
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	pass1->Use();
	
	t += .001;
	glUniform2fv(glGetUniformLocation(pass1->Program, "p"), num_bubbles, &(p[0].x));
	glUniform1fv(glGetUniformLocation(pass1->Program, "r"), num_bubbles, &(r[0]));
	glUniform1fv(glGetUniformLocation(pass1->Program, "vamp"), num_bubbles, &(vamp[0]));
	glUniform1i(glGetUniformLocation(pass1->Program, "num_bubbles"), num_bubbles);
	glUniform1f(glGetUniformLocation(pass1->Program, "t"), t);
	
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glFlush();
}

void init()
{
	glDisable(GL_DEPTH_TEST);
	pass1 = new Shader("vert.vs","frag.fr");
	
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	GLfloat vertices[] = {-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f
	};

	// Vertex buffer object for the square
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);
	/*
	num_bubbles = 2;
	p[0] = vec2(100,256);
	p[1] = vec2(320,256);
	r[0] = 60;
	r[1] = 80;
	v[1] = vec2(-10,0);*/
}



void mousefun(int button, int state, int x, int y) {
	
	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		v[num_bubbles-1] = .5f*(vec2(x,512-y) - p[num_bubbles-1]);
		vamp[num_bubbles-1] = 2;
		make_bubble_mode = false;
	}
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if ( num_bubbles < 10) {
			make_bubble_mode = true;
			p[num_bubbles] = vec2(x,512-y);
			r[num_bubbles] = 10;
			num_bubbles++;
		}
	}
}

int pop_bubble(int i) {
	if ( i < num_bubbles ) {
		
		
	}
}


void keyfun(unsigned char key, int x, int y) {
	for (int i = 0; i < num_bubbles; i++) {
		for (int j = i+1; j < num_bubbles; j++) {
			if ( adj[i][j] )
				std::cout<<i<<" "<<j<<std::endl;
		}
	}
	
	std::cout<<std::endl;
}

int main(int argc, char** argv) {
	
	
	


	
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(512, 512);
	glutCreateWindow(argv[0]);

	glewExperimental = GL_TRUE;
	glewInit();
	
	init();
	glutMouseFunc(mousefun);
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutKeyboardFunc(keyfun);
	glutMainLoop();

	return 0;
}

