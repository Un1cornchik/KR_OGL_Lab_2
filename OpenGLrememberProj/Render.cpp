#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"
#include <math.h>

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;


	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}


	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist * cos(fi2) * cos(fi1),
			camDist * cos(fi2) * sin(fi1),
			camDist * sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}


	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);


		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale * 0.08;
		s.Show();

		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ����������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale * 1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL* ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ������ ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01 * dx;
		camera.fi2 += -0.01 * dy;
	}


	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k * r.direction.X() + r.origin.X();
		y = k * r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02 * dy);
	}


}

void mouseWheelEvent(OpenGL* ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01 * delta;

}

void keyDownEvent(OpenGL* ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL* ogl, int key)
{

}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL* ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE* texarray;

	//������ ��������, (������*������*4      4, ������ ��� ���� �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������ ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH);


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}

///////////////////////////////////////////////////////////////////////////////////

static void F_Normal(const double(&NA)[3], const double(&NB)[3], const double(&NC)[3], double& Nx, double& Ny, double& Nz)
{
	Nx = (NB[1] - NA[1]) * (NC[2] - NA[2]) - (NC[1] - NA[1]) * (NB[2] - NA[2]);
	Ny = -(NB[0] - NA[0]) * (NC[2] - NA[2]) + (NC[0] - NA[0]) * (NB[2] - NA[2]);
	Nz = (NB[0] - NA[0]) * (NC[1] - NA[1]) - (NC[0] - NA[0]) * (NB[1] - NA[1]);
}

static void F_Curve(const double(&FA)[3], const double(&FB)[3], const double(&FD)[3],
	const double(&FA1)[3], const double(&FB1)[3], const double(&FD1)[3], double R, int m, bool flag)
{
	//���������� ������� �������
	double Nx{ 0 }, Ny{ 0 }, Nz{ 0 };
	//������� � ����. ���������. ������� � ������� �� ��-�� ��������� ��� ���������� ������
	double S[] = { 0, 0, 0 };
	double T[] = { 0, 0, 0 };
	//������� � ����. ���������. ������� � ������� �� ������� ��-�� ��� ���������� ������
	double S1[] = { 0, 0, 0 };
	double T1[] = { 0, 0, 0 };
	//���������� �. C - �������� AB
	double FC[] = { (FA[0] + FB[0]) / 2, (FA[1] + FB[1]) / 2, (FA[2] + FB[2]) / 2 };
	//���������� �. C1 - �������� A1B1
	double FC1[] = { (FA1[0] + FB1[0]) / 2, (FA1[1] + FB1[1]) / 2, (FA1[2] + FB1[2]) / 2 };
	//���������� ������� ������� � ���������� ������� AB �� ��������� ���������
	double norm[] = { -(FB[1] - FA[1]), (FB[0] - FA[0]) };
	//����������� "���" ������� �������
	double tau = { 0.0 };
	if (!flag)
		tau = sqrt((pow(R, 2) - (pow(FC[0] - FA[0], 2) + pow(FC[1] - FA[1], 2))) / (pow(norm[0], 2) + pow(norm[1], 2)));
	else
		tau = -sqrt((pow(R, 2) - (pow(FC[0] - FA[0], 2) + pow(FC[1] - FA[1], 2))) / (pow(norm[0], 2) + pow(norm[1], 2)));
	//���������� �. O - ����� ������ ���-�� ������� R (������� ������) � ��-�� ���������
	double O[] = { FC[0] + tau * norm[0], FC[1] + tau * norm[1], FC[2] };
	//���������� �. O1 - ����� ������ ���-�� ������� R (������� ������) � ������� ��-��
	double O1[] = { FC1[0] + tau * norm[0], FC1[1] + tau * norm[1], FC1[2] };
	//����������� ���� fi_min
	double fi_min = atan2(FA[1] - O[1], FA[0] - O[0]);
	//������������ ���� fi_max
	double fi_max = atan2(FB[1] - O[1], FB[0] - O[0]);
	//�������� ���� � �������� � ������ ���-�� ������� R (fi_max - fi_min)  
	double delta_fi = { 0 };
	if (fi_min * fi_max > 0) delta_fi = fabs(fabs(fi_max) - fabs(fi_min));
	else if (fabs(fi_min) + fabs(fi_max) < PI) delta_fi = fabs(fi_min) + fabs(fi_max);
	else delta_fi = 2 * PI - (fabs(fi_min) + fabs(fi_max));
	//�������� ���� �������� ��������� �����
	if (!flag)
	{
		for (double i = 0.0; i < m; i++)
		{
			S[0] = O[0] + R * cos(fi_min + delta_fi * i / m);
			S[1] = O[1] + R * sin(fi_min + delta_fi * i / m);
			S[2] = O[2];
			T[0] = O[0] + R * cos(fi_min + delta_fi * (i + 1) / m);
			T[1] = O[1] + R * sin(fi_min + delta_fi * (i + 1) / m);
			T[2] = O[2];
			S1[0] = O1[0] + R * cos(fi_min + delta_fi * i / m);
			S1[1] = O1[1] + R * sin(fi_min + delta_fi * i / m);
			S1[2] = O1[2];
			T1[0] = O1[0] + R * cos(fi_min + delta_fi * (i + 1) / m);
			T1[1] = O1[1] + R * sin(fi_min + delta_fi * (i + 1) / m);
			T1[2] = O1[2];

			glBegin(GL_TRIANGLES);
			//�������� ��������� ��������� ����� (��������� ������������� DST)
			glColor3d(0.5, 0.5, 0.5);
			glNormal3d(0, 0, -1);
			glTexCoord2d(0, 0);
			glVertex3dv(FD);
			glTexCoord2d(1, 0);
			glVertex3dv(S);
			glTexCoord2d(0, 1);
			glVertex3dv(T);

			//�������� ������� ��������� ��������� ����� (��������� ������������� D1S1T1)
			glColor3d(0.7, 0.7, 0.7);
			glNormal3d(0, 0, 1);
			glTexCoord2d(0, 0);
			glVertex3dv(FD1);
			glTexCoord2d(1, 0);
			glVertex3dv(S1);
			glTexCoord2d(0, 1);
			glVertex3dv(T1);
			glEnd();

			//�������� ������� ����������� ��������� �����
			glBegin(GL_QUADS);
			glColor3d(0.3, 0.3, 0.3);
			F_Normal(S1, T1, S, Nx, Ny, Nz);
			glNormal3d(Nx, Ny, Nz);
			glTexCoord2d(0, 0);
			glVertex3dv(S);
			glTexCoord2d(1, 0);
			glVertex3dv(T);
			glTexCoord2d(0, 1);
			glVertex3dv(T1);
			glTexCoord2d(1, 1);
			glVertex3dv(S1);
			glEnd();
		};
	}
	else
	{
		for (double i = 0.0; i < m; i++)
		{
			S[0] = O[0] + R * cos(fi_min - delta_fi * i / m);
			S[1] = O[1] + R * sin(fi_min - delta_fi * i / m);
			S[2] = O[2];
			T[0] = O[0] + R * cos(fi_min - delta_fi * (i + 1) / m);
			T[1] = O[1] + R * sin(fi_min - delta_fi * (i + 1) / m);
			T[2] = O[2];
			S1[0] = O1[0] + R * cos(fi_min - delta_fi * i / m);
			S1[1] = O1[1] + R * sin(fi_min - delta_fi * i / m);
			S1[2] = O1[2];
			T1[0] = O1[0] + R * cos(fi_min - delta_fi * (i + 1) / m);
			T1[1] = O1[1] + R * sin(fi_min - delta_fi * (i + 1) / m);
			T1[2] = O1[2];

			glBegin(GL_TRIANGLES);
			//�������� ��������� ��������� ����� (��������� ������������� DST)
			glColor3d(0.5, 0.5, 0.5);
			glNormal3d(0, 0, -1);
			glTexCoord2d(0, 0);
			glVertex3dv(FD);
			glTexCoord2d(1, 0);
			glVertex3dv(S);
			glTexCoord2d(0, 1);
			glVertex3dv(T);

			//�������� ������� ��������� ��������� ����� (��������� ������������� D1S1T1)
			glColor3d(0.7, 0.7, 0.7);
			glNormal3d(0, 0, 1);
			glVertex3dv(FD1);
			glTexCoord2d(0, 0);
			glVertex3dv(S1);
			glTexCoord2d(1, 0);
			glVertex3dv(T1);
			glTexCoord2d(0, 1);
			glEnd();

			//�������� ������� ����������� ��������� �����
			glBegin(GL_QUADS);
			glColor3d(0.3, 0.3, 0.3);
			F_Normal(S1, T1, S, Nx, Ny, Nz);
			glNormal3d(Nx, Ny, Nz);
			glTexCoord2d(0, 0);
			glVertex3dv(S);
			glTexCoord2d(1, 0);
			glVertex3dv(T);
			glTexCoord2d(0, 1);
			glVertex3dv(T1);
			glTexCoord2d(1, 1);
			glVertex3dv(S1);
			glEnd();
		};
	};
}


static void My_Figure()
{
	//�������� ������
	double A[] = { 4,0,0 };
	double B[] = { 0,5,0 };
	double C[] = { 4,7,0 };
	double D[] = { 0,12,0 };
	double E[] = { 7,16,0 };
	double F[] = { 6,8,0 };
	double G[] = { 10,5,0 };
	double H[] = { 6,5,0 };

	double A1[] = { 4,0,2 };
	double B1[] = { 0,5,2 };
	double C1[] = { 4,7,2 };
	double D1[] = { 0,12,2 };
	double E1[] = { 7,16,2 };
	double F1[] = { 6,8,2 };
	double G1[] = { 10,5,2 };
	double H1[] = { 6,5,2 };

	double Nx{ 0 }, Ny{ 0 }, Nz{ 0 }; //���������� ������� �������

	//���������
	glBegin(GL_TRIANGLES);
	glColor3d(0.5, 0.5, 0.5);
	//ABH
	/*
	glNormal3d(0, 0, -1);
	glTexCoord2d(0, 0);
	glVertex3dv(A);
	glTexCoord2d(1, 0);
	glVertex3dv(B);
	glTexCoord2d(0, 1);
	glVertex3dv(H);
	//*/
	//BFG
	glNormal3d(0, 0, -1);
	glTexCoord2d(0, 0);
	glVertex3dv(B);
	glTexCoord2d(1, 0);
	glVertex3dv(F);
	glTexCoord2d(0, 1);
	glVertex3dv(G);
	//CDF
	glNormal3d(0, 0, -1);
	glTexCoord2d(0, 0);
	glVertex3dv(C);
	glTexCoord2d(1, 0);
	glVertex3dv(D);
	glTexCoord2d(0, 1);
	glVertex3dv(F);
	//DEF
	/*
	glNormal3d(0, 0, -1);
	glTexCoord2d(0, 0);
	glVertex3dv(D);
	glTexCoord2d(1, 0);
	glVertex3dv(E);
	glTexCoord2d(0, 1);
	glVertex3dv(F);
	//*/
	
	//����
	glColor3d(0.7, 0.7, 0.7);
	//A1B1H1
	/*
	glNormal3d(0, 0, 1);
	glTexCoord2d(1, 1);
	glVertex3dv(A1);
	glTexCoord2d(1, 0);
	glVertex3dv(B1);
	glTexCoord2d(0, 1);
	glVertex3dv(H1);
	//*/
	//B1F1G1	
	glNormal3d(0, 0, 1);
	glTexCoord2d(1, 1);
	glVertex3dv(B1);
	glTexCoord2d(1, 0);
	glVertex3dv(F1);
	glTexCoord2d(0, 1);
	glVertex3dv(G1);

	//C1D1F1
	glNormal3d(0, 0, 1);
	glTexCoord2d(1, 1);
	glVertex3dv(C1);
	glTexCoord2d(1, 0);
	glVertex3dv(D1);
	glTexCoord2d(0, 1);
	glVertex3dv(F1);
	//D1E1F1
	/*
	glNormal3d(0, 0, 1);
	glTexCoord2d(1, 1);
	glVertex3dv(D1);
	glTexCoord2d(1, 0);
	glVertex3dv(E1);
	glTexCoord2d(0, 1);
	glVertex3dv(F1);
	//*/
	glEnd();

	//������� �����
	glBegin(GL_QUADS);
	glColor3d(0.3, 0.3, 0.3);
	//AA1B1B
	/*
	F_Normal(A1, B1, A, Nx, Ny, Nz);
	glNormal3d(Nx, Ny, Nz);
	glTexCoord2d(0, 0);
	glVertex3dv(A);
	glTexCoord2d(1, 0);
	glVertex3dv(A1);
	glTexCoord2d(0, 1);
	glVertex3dv(B1);
	glTexCoord2d(1, 1);
	glVertex3dv(B);
	//*/
	//BB1C1C
	F_Normal(B1, C1, B, Nx, Ny, Nz);
	glNormal3d(Nx, Ny, Nz);
	glTexCoord2d(0, 0);
	glVertex3dv(B);
	glTexCoord2d(1, 0);
	glVertex3dv(B1);
	glTexCoord2d(0, 1);
	glVertex3dv(C1);
	glTexCoord2d(1, 1);
	glVertex3dv(C);
	//CC1D1D
	F_Normal(C1, D1, C, Nx, Ny, Nz);
	glNormal3d(Nx, Ny, Nz);
	glTexCoord2d(0, 0);
	glVertex3dv(C);
	glTexCoord2d(1, 0);
	glVertex3dv(C1);
	glTexCoord2d(0, 1);
	glVertex3dv(D1);
	glTexCoord2d(1, 1);
	glVertex3dv(D);
	//DD1E1E
	/*
	F_Normal(D1, E1, D, Nx, Ny, Nz);
	glNormal3d(Nx, Ny, Nz);
	glTexCoord2d(0, 0);
	glVertex3dv(D);
	glTexCoord2d(1, 0);
	glVertex3dv(D1);
	glTexCoord2d(0, 1);
	glVertex3dv(E1);
	glTexCoord2d(1, 1);
	glVertex3dv(E);
	//*/
	//EE1F1F
	F_Normal(E1, F1, E, Nx, Ny, Nz);
	glNormal3d(Nx, Ny, Nz);
	glTexCoord2d(0, 0);
	glVertex3dv(E);
	glTexCoord2d(1, 0);
	glVertex3dv(E1);
	glTexCoord2d(0, 1);
	glVertex3dv(F1);
	glTexCoord2d(1, 1);
	glVertex3dv(F);
	//FF1G1G	
	F_Normal(F1, G1, F, Nx, Ny, Nz);
	glNormal3d(Nx, Ny, Nz);
	glTexCoord2d(0, 0);
	glVertex3dv(F);
	glTexCoord2d(1, 0);
	glVertex3dv(F1);
	glTexCoord2d(0, 1);
	glVertex3dv(G1);
	glTexCoord2d(1, 1);
	glVertex3dv(G);
	//GG1H1H
	F_Normal(G1, H1, G, Nx, Ny, Nz);
	glNormal3d(Nx, Ny, Nz);
	glTexCoord2d(0, 0);
	glVertex3dv(G);
	glTexCoord2d(1, 0);
	glVertex3dv(G1);
	glTexCoord2d(0, 1);
	glVertex3dv(H1);
	glTexCoord2d(1, 1);
	glVertex3dv(H);
	//HH1A1A
	F_Normal(H1, A1, H, Nx, Ny, Nz);
	glNormal3d(Nx, Ny, Nz);
	glTexCoord2d(0, 0);
	glVertex3dv(H);
	glTexCoord2d(1, 0);
	glVertex3dv(H1);
	glTexCoord2d(0, 1);
	glVertex3dv(A1);
	glTexCoord2d(1, 1);
	glVertex3dv(A);
	glEnd();

	F_Curve(A, B, H, A1, B1, H1, 5, 100, true);
	F_Curve(D, E, F, D1, E1, F1, 8, 100, false);
}



void Render(OpenGL* ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  
	
	glBindTexture(GL_TEXTURE_2D, texId);

	My_Figure();

	//��������� ������ ������

	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
									//(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R=" << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;

	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}