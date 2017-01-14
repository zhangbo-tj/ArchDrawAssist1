#pragma once
#include "StdAfx.h"
#include "GL\glew.h"
#include <GL\GLAUX.H>
#include <QWidget>
#include <QWheelEvent>
#include <QtOpenGL\QGLwidget>
#include <QApplication>
#include <QDesktopWidget>
#include <wrap\gui\trackball.h>
#include "GLModel.h"
#include "enroll.h"
#include "GCEnrolledSlice.h"
#include <iostream>
#include <QMessageBox>
using namespace std;
using namespace vcg;


//////////////////////////////////////////////////////////////////////////
//@intro	������ʾչ�����Slice�Ŀؼ�����֧�ֶ�չ��������ֶ�����
//////////////////////////////////////////////////////////////////////////
class GLTextureArea:public QGLWidget{
public:
	Q_OBJECT
public:
	GLTextureArea(QWidget* parent = 0);	//������
	GLTextureArea(vector<GCSlice> GSSlices,QWidget* parent = 0);	//������
	GLTextureArea(GLModel* model, QWidget* parent =0);	//������
	GLTextureArea(GLModel* model,vector<Face> faceVector, QWidget *parent);	//������
	~GLTextureArea();	//��������

public slots:
	void SaveTextureFile();

public:
	//��ʼ���ؼ�����
	void initializeGL();	

	//���¶��嵱ǰ�ؼ��ĳߴ�
	void resizeGL(int newWidth,int newHeight);	

	//���Ƶ�ǰ�ؼ�
	void paintGL();	

	//������άģ��
	void loadModel(GLModel* model);	
	
	//��ȡMTL�ļ�������άģ�Ͷ�Ӧ������
	void readMtlFile();	

	//��ȡslice����
	void loadSlices(vector<GCSlice> gcSlices);	

private:
	//�����¼�
	void wheelEvent(QWheelEvent *e);

	//����ƶ��¼�
	void mouseMoveEvent(QMouseEvent *e);	

	//��ѹ����¼�
	void mousePressEvent(QMouseEvent *e);	

	//�ͷ�����¼�
	void mouseReleaseEvent(QMouseEvent *e);		

	//�ͷż����¼�
	void keyReleaseEvent(QKeyEvent *e);		

	//��ѹ�����¼�
	void keyPressEvent(QKeyEvent *e);

	//����ǰ���λ��ת��Ϊ��ά�ռ�����ϵ�ڵĵ�
	void GetWorldCoordinate(int mouse_x,int mouse_y,double &world_x,double &world_y, double &world_z);
	
	//��ȡ���ѡ�е�Slice������
	int GetSelectedSlice(float px, float py);

private:
	GLModel* glmModel;	//��άģ��
	vector<GCSlice> mSlices;	//slice����
	vector<EnrolledSlice> mEnrolledSlices;
	vcg::Trackball trackball;	//�켣�����ڶ����������¼�
	int mSelectedSliceIndex;	//��ѡ�е�Slice������
	bool mControlPointSelected;

	float oldX,oldY;	//���ԭ����λ��
	bool mLeftKeyPressed;	//�������Ƿ񱻰���
		
	float mTranslateX, mTranslateY;		//ƽ�Ʒ���
};