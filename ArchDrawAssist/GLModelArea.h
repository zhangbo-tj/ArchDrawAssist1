#pragma once
#include "StdAfx.h"
#include <string>
#include "GL\glew.h"
#include <QWidget>
#include <QtOpenGL\QGLwidget>
#include <QWheelEvent>
#include <QTextCodec>
#include <qmenu.h>
#include <gl\glext.h>
#include <gl\GLAUX.H>
#include <wrap\gui\trackball.h>
//#include "glm.h"
#include "3DCircleExtract.h"
#include "GLModel.h"
#include "SliceBBox.h"
#include <iostream>
#include <fstream>

#include <QApplication>
#include <QDesktopWidget>

#include "calGCGcity.h"
#include "enroll.h"
#include "GCSlice.h"
#include <QtCore>

//opencv��ͷ�ļ�
#include <cv.hpp>
#include <core\core.hpp>
#include <highgui\highgui.hpp>

using namespace std;
using namespace vcg;

class GLModelArea:public QGLWidget{
	public:
	Q_OBJECT
public:
	GLModelArea(GLModel* model,QWidget *parent = 0);
	~GLModelArea(void);

	//����QGL�����ı��躯��
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();

	void CreateActions();

	//��ȡ��άģ��
	void LoadModel(GLModel* model);

	//��ȡMTL�ļ�������άģ�Ͷ�Ӧ������
	void ReadMtlFile();
	//��ȡ��Ӧ��BMP�ļ���������
	void ReadBMPFile(char* str, char* strFileName,int textureID);

	//����άģ�ͽ��зָ�
	vector<GCSlice> SplitModel();

public slots:

	//�ı�Գ�����ʾ״̬
	void ChangeAxisStatus();

	//�ı�ռ�ֱ������ϵ��ʾ״̬
	void ChangeCoorSystemStatus();

	//ɾ��ѡ�еĺ����
	void DeleteCuttingPlane(); 

	//��Ӻ����
	void AddCuttingPlane();	

	//���õ�ǰ����ʾ����ϵ
	void ResetCoordSystem();	
	
	//��ѡ�е�Slice��չ����ʽ��ΪԲ��
	void ChangeEnrollMode2Cylinder();

	//��ѡ�е�Slice��չ����ʽ��ΪԲ̨
	void ChangeEnrollMode2TruncatedCone();

	//ˢ�µ�ǰ�ؼ�������
	void UpdateTimer();	

private :
	//�������¼�
	void wheelEvent(QWheelEvent *e);	

	//����ƶ��¼�
	void mouseMoveEvent(QMouseEvent *e);	

	//������¼�
	void mousePressEvent(QMouseEvent *e);	

	//��갴���ͷ��¼�
	void mouseReleaseEvent(QMouseEvent *e);

	//���̰����¼�
	void keyPressEvent(QKeyEvent* e);	

	//����Ҽ��˵��¼�
	void contextMenuEvent(QContextMenuEvent *e);	

	//��ȡѡ���ƽ��
	int GetSelectedPlane(int mouse_x,int mouse_y);	

	//��ȡѡ���Bounding Box
	int GetSlectedBBox(int mouse_x, int mouse_y);	

	//�ƶ�������
	void MovePlane(int mouse_x,int mouse_y);	

	//����ƽ����߶εĽ���
	bool PlaneLineSegmentIntersectPt(Normal planeNormal,Vertex planePoint,Vertex vertexA,Vertex vertexB,Vertex &interVertex);	

	//����������Ƭ��������ָ��Ľ��
	bool FaceIntersectPlane(Face face,float* abcd,VECTOR3 planePoint,vector<Vertex> &interPoints);	

	//����������Ƭ��������ָ��Ľ��
	bool FaceIntersectPlane(Vertex vertexA,Vertex vertexB,Vertex vertexC,float* abcd,VECTOR3 planePoint,vector<Vertex> &interPoints);	

	//���������İ뾶
	float CalIntersectRadius(Vertex planePoint);	

	//�����ݶ���Ϣ
	vector<float> ComputeGradients();

	//����������Ϣ���������з�λ��
	void ComputeBestCuts();

	//��ʾ������
	void ShowCuttingPlanes();	
	
	//��ʾ�Գ���
	void ShowPrincipalAxis(Vertex lowestPoint,Vertex highestPoint,Normal axisNormal);
	
	//��ʾ�ռ�ֱ������ϵ
	void ShowCoorSystem();	

	//������άģ���ڶԳ����ϵ�ͶӰ�����½�
	void ComputeLowestAndHighest(Vertex lowestVertex,Vertex highestVertex,Normal axis,Vertex &lowestPointOnAxis,Vertex &highestPointOnAxis,Normal &low2Top); 

	 //����άģ�;��ȵط�Ϊ10�飬�˴���Ϊ��ʼ������֣�Ӧ�����Ż���ķֶη���
	void CutModelUniformly();
	
	//��ʾѡ�е�SliceƬ��
	void ShowSelectedSliceBBox();

	void DrawSolidCircleRed(float x, float y,float z, float radius);		//����ɫ��ʵ��Բ
	void DrawWiredCircleRed(float x, float y,float z, float radius);		//����ɫ������Բ
	void DrawSolidCircleGreen(float x, float y,float z, float radius);	//����ɫ��ʵ��Բ
	void DrawWiredCircleGreen(float x, float y,float z, float radius);	//����ɫ������Բ
	void GetWorldCoordinate(int mouse_x, int mouse_y, double &world_x,double &world_y,double &world_z);

private:
	vcg::Trackball trackball;	//trackball������ʵ������ƽ�ƵȲ���
	vector<GLfloat> bbox;	//

	GLModel* glmModel;	//��άģ��
	//vector<GCSlice> cuttedSlices;
	vector<Vertex> profileCenters;	//���������������
	vector<float> profileRadius;	//������İ뾶
	vector<SliceBBox> sliceBBoxes;	//slice��Bounding Box

	double pModelViewMatrix[16];
	float modelCenter[3];	//��άģ�͵�����
	bool isLeftPressed;	

	bool b_show_principal_axis;	//�Ƿ���ʾ�Գ���
	bool b_show_coord_system;	//�Ƿ���ʾ����ϵ

	bool b_plane_selected;	//�ж��Ƿ�ѡ��ƽ��
	bool b_leftbutton_pressed;	//�������Ƿ���
	int selected_plane_index;	//ѡ��ƽ�������
	int selected_slice_index;	//ѡ�е�SliceƬ�ε�����
	float disThreshold;		//��ѡ���з�ƽ��ʱ���������Χ

	QMenu* right_button_menu;	//�Ҽ��˵�
	QAction *actionDeletePlane,*actionAddPlane,*actionResetTrackball;	//����Ҽ�ѡ��

	QMenu* select_enroll_mode_menu;		//�Ҽ��˵�
	QAction *actionSelectCylinderMode, *actionSelectTrunConeMode;  //�Ҽ��˵��ϵ�ѡ��

	float context_position_x;	//�Ҽ��˵���λ��
	float context_position_y;	//�Ҽ��˵���λ��

	Vertex lowestVertexOnAxis,highestVertexOnAxis;	//��άģ���ڶԳ����ϵ�ͶӰ������½�

	int paintNum;	//��Ⱦ�����Ĵ���
};