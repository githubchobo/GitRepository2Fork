#include <vgl.h>
#include <InitShader.h>
#include <mat.h>
#include "MyCube.h"
#include "MyPyramid.h"
#include "MyTarget.h"
#include <list>

float PI = 3.141592;
float g_time = 0;
float ang4 = -10;

struct MyVertex
{
	vec4 pos;
	vec4 color;
};

class MySphere
{
public:
	int m_NumDiv;
	int m_NumVertex;

	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_prog;

	bool m_bInit;

	MySphere()
	{
		m_NumDiv = 0;
		m_NumVertex = 0;
		m_bInit = false;
	}

	void init(int div)
	{
		if (m_NumDiv == div || div < 3) return;
		m_NumDiv = div;
		m_NumVertex = (m_NumDiv * 2 + (m_NumDiv - 2) * m_NumDiv * 2) * 3;

		MyVertex* vertices = new MyVertex[m_NumVertex];

		float r = 0.5f;

		int cur = 0;

		float da = 2 * 3.141592 / m_NumDiv;

		for (int i = 0; i < m_NumDiv; i++)
		{
			for (int j = 0; j < m_NumDiv; j++)
			{
				//(i,j),(i,j+1),(i+1,j+1),(i+1,j)
				//  a     b         c        d

				// i
				float x1 = sin(i * da);
				float z1 = cos(i * da);

				// i+1
				float x2 = sin((i + 1) * da);
				float z2 = cos((i + 1) * da);

				// j
				float t1 = PI / 2.0 - j * da / 2;
				float r1 = r * cos(t1);
				float y1 = r * sin(t1);


				// j+1
				float t2 = PI / 2.0 - (j + 1) * da / 2;
				float r2 = r * cos(t2);
				float y2 = r * sin(t2);

				vec4 a = vec4(x1 * r1, y1, z1 * r1, 1);
				vec4 b = vec4(x1 * r2, y2, z1 * r2, 1);
				vec4 c = vec4(x2 * r2, y2, z2 * r2, 1);
				vec4 d = vec4(x2 * r1, y1, z2 * r1, 1);

				vec4 color = vec4(0.8 - 0.05 * j, 0.8 - 0.05 * j, 0.8 - 0.05 * j, 1);
				//if ((i + j) % 2 == 1)
					//color = vec4(0, 0, 1, 1);

				if (j != m_NumDiv - 1)
				{
					vertices[cur].pos = a;
					vertices[cur].color = color;
					cur++;
					vertices[cur].pos = b;
					vertices[cur].color = color;
					cur++;
					vertices[cur].pos = c;
					vertices[cur].color = color;
					cur++;
				}
				if (j != 0)
				{
					vertices[cur].pos = c;
					vertices[cur].color = color;
					cur++;
					vertices[cur].pos = d;
					vertices[cur].color = color;
					cur++;
					vertices[cur].pos = a;
					vertices[cur].color = color;
					cur++;
				}
			}
		}

		if (m_bInit == false)
		{
			glGenVertexArrays(1, &m_vao);
			glBindVertexArray(m_vao);

			glGenBuffers(1, &m_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		}

		else
		{
			glBindVertexArray(m_vao);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		}

		glBufferData(GL_ARRAY_BUFFER, sizeof(MyVertex) * m_NumVertex, vertices, GL_STATIC_DRAW);

		if (m_bInit == false)
		{
			loadShaderAndConnect();
		}

		m_bInit = false;

		delete[] vertices;

	}

	void loadShaderAndConnect()
	{
		m_prog = InitShader("vshader.glsl", "fshader.glsl");
		glUseProgram(m_prog);

		GLuint vPosition = glGetAttribLocation(m_prog, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, sizeof(MyVertex), BUFFER_OFFSET(0));

		GLuint vColor = glGetAttribLocation(m_prog, "vColor");
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, sizeof(MyVertex), BUFFER_OFFSET(sizeof(vec4)));
	}

	void decreaseDiv()
	{
		init(m_NumDiv - 1);
	}

	void increaseDiv()
	{
		init(m_NumDiv + 1);
	}

	void Draw(GLuint program)
	{
		glBindVertexArray(m_vao);
		glUseProgram(program);

		glDrawArrays(GL_TRIANGLES, 0, m_NumVertex);
	}
};


struct MyCylinderVertex
{
	vec4 pos;
	vec4 color;
};

class MyCylinder
{
public:
	int m_NumPoly;
	int m_NumVertex;

	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_prog;

	bool m_bInit;

	MyCylinder()
	{
		m_NumPoly = 0;
		m_NumVertex = 0;
		m_vao = 0;
		m_vbo = 0;
		m_prog = 0;
		m_bInit = false;
	}

	void init(int poly, double nb)
	{
		if (m_NumPoly == poly || poly < 3)
			return;
		m_NumPoly = poly;
		m_NumVertex = m_NumPoly * 2 * 3 + m_NumPoly * 6; // Updated for top and bottom disks

		MyCylinderVertex* vertices = new MyCylinderVertex[m_NumVertex];

		float r = 0.5f;
		float h = 1.0f;

		int cur = 0;
		float da = 3.141592 * 2 / m_NumPoly;

		// Side faces
		for (int i = 0; i < m_NumPoly; i++)
		{
			float x1 = r * cos(i * da);
			float z1 = r * sin(i * da);

			float x2 = r * cos((i + 1) * da);
			float z2 = r * sin((i + 1) * da);

			vec4 a = vec4(x2, h / 2, z2, 1);
			vec4 b = vec4(x2, -h / 2, z2, 1);
			vec4 c = vec4(x1, -h / 2, z1, 1);
			vec4 d = vec4(x1, h / 2, z1, 1);

			float f = float(i) / m_NumPoly;
			vec4 color = vec4(0.4 + 0.5 * f + nb * 0.1, 0.4 + 0.5 * f + nb * 0.1, 0.4 + 0.5 * f + nb * 0.1, 1);

			vertices[cur].pos = a;
			vertices[cur].color = color;
			cur++;
			vertices[cur].pos = b;
			vertices[cur].color = color;
			cur++;
			vertices[cur].pos = c;
			vertices[cur].color = color;
			cur++;

			vertices[cur].pos = c;
			vertices[cur].color = color;
			cur++;
			vertices[cur].pos = d;
			vertices[cur].color = color;
			cur++;
			vertices[cur].pos = a;
			vertices[cur].color = color;
			cur++;
		}

		// Top disk
		for (int i = 0; i < m_NumPoly; i++)
		{
			float x = r * cos(i * da);
			float z = r * sin(i * da);

			vec4 center = vec4(0.0, h / 2, 0.0, 1);
			vec4 vertex1 = vec4(x, h / 2, z, 1);
			vec4 vertex2 = vec4(r * cos((i + 1) * da), h / 2, r * sin((i + 1) * da), 1);

			vec4 color = vec4(0.6 + nb * 0.1, 0.6 + nb * 0.1, 0.6 + nb * 0.1, 1); // White color for the top disk

			vertices[cur].pos = center;
			vertices[cur].color = color;
			cur++;
			vertices[cur].pos = vertex2;
			vertices[cur].color = color;
			cur++;
			vertices[cur].pos = vertex1;
			vertices[cur].color = color;
			cur++;
		}

		// Bottom disk
		for (int i = 0; i < m_NumPoly; i++)
		{
			float x = r * cos(i * da);
			float z = r * sin(i * da);

			vec4 center = vec4(0.0, -h / 2, 0.0, 1);
			vec4 vertex1 = vec4(x, -h / 2, z, 1);
			vec4 vertex2 = vec4(r * cos((i + 1) * da), -h / 2, r * sin((i + 1) * da), 1);

			vec4 color = vec4(0.6 + nb * 0.1, 0.6 + nb * 0.1, 0.6 + nb * 0.1, 1); // White color for the bottom disk

			vertices[cur].pos = center;
			vertices[cur].color = color;
			cur++;
			vertices[cur].pos = vertex1;
			vertices[cur].color = color;
			cur++;
			vertices[cur].pos = vertex2;
			vertices[cur].color = color;
			cur++;
		}

		if (!m_bInit)
		{
			glGenVertexArrays(1, &m_vao);
			glBindVertexArray(m_vao);

			glGenBuffers(1, &m_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		}
		else
		{
			glBindVertexArray(m_vao);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		}

		glBufferData(GL_ARRAY_BUFFER, sizeof(MyCylinderVertex) * m_NumVertex, vertices, GL_STATIC_DRAW);

		if (!m_bInit)
			loadShaderAndConnect();

		m_bInit = true;
		delete[] vertices;
	}

	void loadShaderAndConnect()
	{
		m_prog = InitShader("vshader.glsl", "fshader.glsl");
		glUseProgram(m_prog);

		GLuint vPosition = glGetAttribLocation(m_prog, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, sizeof(MyCylinderVertex), BUFFER_OFFSET(0));

		GLuint vColor = glGetAttribLocation(m_prog, "vColor");
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, sizeof(MyCylinderVertex), BUFFER_OFFSET(sizeof(vec4)));
	}


	void Draw(GLuint program)
	{
		glBindVertexArray(m_vao);
		glUseProgram(program);
		glDrawArrays(GL_TRIANGLES, 0, m_NumVertex);

	}
};




MyCube cube;
MyPyramid pyramid;
MyTarget target(&cube);
MyCylinder cylinder;
MyCylinder bolt;
MySphere sphere;
GLuint program;
GLuint uMat;

mat4 CTM;

bool bPlay = false;
bool bChasingTarget = false;
bool bDrawTarget = false;

float ang1 = 0;
float ang2 = 0;
float ang3 = 0;

void myInit()
{
	cube.Init();
	pyramid.Init();
	sphere.init(10);
	cylinder.init(6, 2);
	bolt.init(20, 0);

	program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);
}



void drawRobotArm(float ang1, float ang2, float ang3)
{
	mat4 temp = CTM;

	// BASE
	mat4 M(1.0);

	
	M = Translate(0, 0, 0.075) * Scale(0.3, 0.2, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	pyramid.Draw(program);

	M = Translate(0, 0, -0.075) * Scale(0.3, 0.2, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	pyramid.Draw(program);


	//Fixing Pin
	M = Translate(0, 0, 0.1) * RotateX(90) * Scale(0.07, 0.04, 0.07);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cylinder.Draw(program);

	M = Translate(0, 0, -0.1) * RotateX(90) * Scale(0.07, 0.04, 0.07);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cylinder.Draw(program);

	M = Translate(0, 0, 0.12) * RotateX(90) * Scale(0.03, 0.02, 0.03);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	bolt.Draw(program);

	M = Translate(0, 0, -0.12) * RotateX(90) * Scale(0.03, 0.02, 0.03);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	bolt.Draw(program);
	// Upper Arm
	CTM *= RotateZ(-ang1) * Translate(0, 0.15, 0);
	M = Scale(0.1, 0.4, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);


	// Lower Arm
	CTM *= Translate(0, 0.15, 0) * RotateZ(-ang2) * Translate(0, 0.2, 0);
	M = Translate(0, 0, -0.075) * Scale(0.1, 0.5, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	// Lower Arm2
	M = Translate(0, 0, 0.075) * Scale(0.1, 0.5, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	//Fixing Pin
	M = Translate(0, -0.2, 0.1) * RotateX(90) * Scale(0.07, 0.04, 0.07);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cylinder.Draw(program);

	M = Translate(0, -0.2, -0.1) * RotateX(90) * Scale(0.07, 0.04, 0.07);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cylinder.Draw(program);

	M = Translate(0, -0.2, 0.12) * RotateX(90) * Scale(0.03, 0.02, 0.03);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	bolt.Draw(program);

	M = Translate(0, -0.2, -0.12) * RotateX(90) * Scale(0.03, 0.02, 0.03);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	bolt.Draw(program);

	// Hand
	std::list<mat4> stack;


	CTM *= Translate(0, 0.2, 0) * RotateZ(ang3);
	M = Translate(0, 0, 0) * Scale(0.1, 0.1, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	M = Translate(0, 0, 0.1) * RotateX(90) * Scale(0.07, 0.04, 0.07);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cylinder.Draw(program);

	M = Translate(0, 0, -0.1) * RotateX(90) * Scale(0.07, 0.04, 0.07);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cylinder.Draw(program);

	M = Translate(0, 0, 0.12) * RotateX(90) * Scale(0.03, 0.02, 0.03);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	bolt.Draw(program);

	M = Translate(0, 0, -0.12) * RotateX(90) * Scale(0.03, 0.02, 0.03);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	bolt.Draw(program);


	stack.push_back(CTM);
	CTM *= RotateZ(60 - ang4) * Translate(0.1, -0.02, 0);
	M = Scale(0.1, 0.02, 0.02);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	CTM *= Translate(0.05, 0, 0);
	M = Scale(0.04, 0.04, 0.04);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	sphere.Draw(program);

	CTM *= RotateZ(60) * Translate(0.05, 0, 0);

	M = Scale(0.1, 0.02, 0.02);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);


	CTM = stack.back();
	stack.pop_back();
	CTM *= RotateZ(120 + ang4) * Translate(0.1, 0.02, 0);
	M = Scale(0.1, 0.02, 0.02);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	CTM *= Translate(0.05, 0, 0);
	M = Scale(0.04, 0.04, 0.04);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	sphere.Draw(program);

	CTM *= RotateZ(-60) * Translate(0.05, 0, 0);

	M = Scale(0.1, 0.02, 0.02);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);



	CTM = temp;
}

float getDistance(vec3 a, vec3 b)
{
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
}

int rotateDirection(vec3 redboxPos, float upLen)
{
	float da = 0.05;
	
	vec3 joint_minus = vec3(sin((ang1 - da) / 360 * 2 * 3.141592) * upLen, cos((ang1 - da) / 360 * 2 * 3.141592) * upLen, 0);
	vec3 joint_plus = vec3(sin((ang1 + da) / 360 * 2 * 3.141592) * upLen, cos((ang1 + da) / 360 * 2 * 3.141592) * upLen, 0);
	double distance1 = getDistance(joint_minus, redboxPos);
	double distance2 = getDistance(joint_plus, redboxPos);
	if (distance1 > distance2)
		return 1;
	return -1;
}

int rotateDirection1(vec3 from, vec3 to, float curAngle)
{
	vec3 t = normalize(to);

	vec3 minusRotate = vec3(sin((curAngle - 1) / 360 * 2 * 3.141592) , cos((curAngle - 1) / 360 * 2 * 3.141592), 0);
	vec3 plusRotate = vec3(sin((curAngle + 1) / 360 * 2 * 3.141592) , cos((curAngle + 1) / 360 * 2 * 3.141592), 0);
	if (getDistance(minusRotate, t) > getDistance(plusRotate, t))
		return 1;
	return -1;
}


double getCosByLength(double a, double b, double c)
{
	return (a * a + b * b - c * c) / (2 * a * b);
}

double getCosByVec(vec3 a, vec3 b)
{
	double dotProduct = a.x * b.x + a.y * b.y;
	double magnitudeA = std::sqrt(a.x * a.x + a.y * a.y);
	double magnitudeB = std::sqrt(b.x * b.x + b.y * b.y);

	if (magnitudeA == 0.0 || magnitudeB == 0.0) {
		std::cerr << "Error: One or both vectors have zero length." << std::endl;
		return std::numeric_limits<double>::quiet_NaN(); // Not a Number
	}

	return dotProduct / (magnitudeA * magnitudeB);
}

void computeAngle()
{
	//centerPos: 중심 12323
	//redboxPos: 타겟좌표 
	//joint: upper와 lower를 이어주는 관절의 좌표


	//직전 각도 저장을 할 변수 선언
	float t1 = ang1;
	float t2 = ang2;
	float t3 = ang3;

	//ctm 0, -0.4, 0 이 모두에 적용돼서 베이스를 0,0,0을 기준으로 다시 계산
	vec3 base(0, 0, 0);
	vec3 targetPos = target.currentPos;
	vec3 p1;
	float upLen = 0.3;
	float lowLen = 0.4;
	float hand = 0.2;

	float da = 1;
	vec3 upper_offset(0, 0, 0);
	da = rotateDirection1(base,targetPos,t1);

	
	float r1 = 0;
	
	for(int i=0;i<180;i++)
	{

		r1 = ang1 * PI / 180;
		
		p1 = vec3(sin(r1) * upLen, cos(r1) * upLen, 0);
		double p1toTdist = getDistance(p1, targetPos);

		if (p1toTdist >= lowLen + hand || p1toTdist + hand <= lowLen)
			ang1 = ang1 + da;

		if (getCosByLength(lowLen, p1toTdist, hand) > 1 || getCosByLength(lowLen, p1toTdist, hand) < -1)
			continue;

		ang2 = acos(getCosByLength(lowLen,p1toTdist,hand)) * 180 / PI;
		
		vec3 p1toB = base - p1;
		vec3 p1toT = targetPos - p1;
		float angle_center_p1_redbox = acos(getCosByVec(p1toB,p1toT)) * 180 / 3.141592;
		ang2 = 180 - (ang2 + angle_center_p1_redbox);
		if (da<0)
			ang2 = -ang2;
		vec3 p2 = p1 + vec3(sin((ang2) / 360 * 2 * 3.141592), cos((ang2) / 360 * 2 * 3.141592), 0);

		if (getCosByLength(lowLen, hand, p1toTdist) > 1 || getCosByLength(lowLen, hand, p1toTdist) < -1)
			continue;
		ang3 = 180 - acos(getCosByLength(lowLen,hand,p1toTdist)) * 180 / 3.141592;
		if (da > 0)
			ang3 = -ang3;
		break;
	}



	ang1 = t1 + (ang1 - t1) / 3.0;
	ang2 = t2 + (ang2 - t2) / 3.0;
	ang3 = t3 + (ang3 - t3) / 3.0;
}



void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	GLuint uColor = glGetUniformLocation(program, "uColor");
	glUniform4f(uColor, -1, -1, -1, -1);


	uMat = glGetUniformLocation(program, "uMat");
	CTM = Translate(0, -0.4, 0) * RotateY(g_time * 30);
	//CTM = Translate(0, -0.4, 0);

	drawRobotArm(ang1, ang2, ang3);

	glUniform4f(uColor, 1, 0, 0, 1);
	if (bDrawTarget == true)
		target.Draw(program, CTM, g_time);

	glutSwapBuffers();
}


void myIdle()
{
	if (bPlay)
	{
		g_time += 1 / 60.0f;
		Sleep(1 / 60.0f * 1000);

		if (bChasingTarget == false)
		{
			ang1 = 45 * sin(g_time * 3.141592);
			ang2 = 60 * sin(g_time * 2 * 3.141592);
			ang3 = 30 * sin(g_time * 3.141592);
		}
		else
			computeAngle();

		glutPostRedisplay();
	}
}

void myKeyboard(unsigned char c, int x, int y)
{

	switch (c)
	{
	case '1':
		bChasingTarget = !bChasingTarget;
		break;
	case '2':
		bDrawTarget = !bDrawTarget;
		break;
	case '3':
		target.toggleRandom();
		break;
	case ' ':
		bPlay = !bPlay;
		break;
	default:
		break;
	}
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Simple Robot Arm");

	glewExperimental = true;
	glewInit();

	myInit();
	glutDisplayFunc(myDisplay);
	glutKeyboardFunc(myKeyboard);
	glutIdleFunc(myIdle);

	glutMainLoop();

	return 0;
}
