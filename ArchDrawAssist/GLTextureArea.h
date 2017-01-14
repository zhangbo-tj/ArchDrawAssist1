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
//@intro	用于显示展开后的Slice的控件，并支持对展开结果的手动调整
//////////////////////////////////////////////////////////////////////////
class GLTextureArea:public QGLWidget{
public:
	Q_OBJECT
public:
	GLTextureArea(QWidget* parent = 0);	//构造器
	GLTextureArea(vector<GCSlice> GSSlices,QWidget* parent = 0);	//构造器
	GLTextureArea(GLModel* model, QWidget* parent =0);	//构造器
	GLTextureArea(GLModel* model,vector<Face> faceVector, QWidget *parent);	//构造器
	~GLTextureArea();	//析构函数

public slots:
	void SaveTextureFile();

public:
	//初始化控件参数
	void initializeGL();	

	//重新定义当前控件的尺寸
	void resizeGL(int newWidth,int newHeight);	

	//绘制当前控件
	void paintGL();	

	//读入三维模型
	void loadModel(GLModel* model);	
	
	//读取MTL文件生成三维模型对应的纹理
	void readMtlFile();	

	//读取slice数组
	void loadSlices(vector<GCSlice> gcSlices);	

private:
	//滚轮事件
	void wheelEvent(QWheelEvent *e);

	//鼠标移动事件
	void mouseMoveEvent(QMouseEvent *e);	

	//按压鼠标事件
	void mousePressEvent(QMouseEvent *e);	

	//释放鼠标事件
	void mouseReleaseEvent(QMouseEvent *e);		

	//释放键盘事件
	void keyReleaseEvent(QKeyEvent *e);		

	//按压键盘事件
	void keyPressEvent(QKeyEvent *e);

	//将当前鼠标位置转换为三维空间坐标系内的点
	void GetWorldCoordinate(int mouse_x,int mouse_y,double &world_x,double &world_y, double &world_z);
	
	//获取鼠标选中的Slice的索引
	int GetSelectedSlice(float px, float py);

private:
	GLModel* glmModel;	//三维模型
	vector<GCSlice> mSlices;	//slice数组
	vector<EnrolledSlice> mEnrolledSlices;
	vcg::Trackball trackball;	//轨迹球，用于定义鼠标键盘事件
	int mSelectedSliceIndex;	//被选中的Slice的索引
	bool mControlPointSelected;

	float oldX,oldY;	//鼠标原来的位置
	bool mLeftKeyPressed;	//鼠标左键是否被按下
		
	float mTranslateX, mTranslateY;		//平移分量
};