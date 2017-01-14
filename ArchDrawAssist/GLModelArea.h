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

//opencv的头文件
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

	//构建QGL构建的必需函数
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();

	void CreateActions();

	//读取三维模型
	void LoadModel(GLModel* model);

	//读取MTL文件生成三维模型对应的纹理
	void ReadMtlFile();
	//读取相应的BMP文件生成纹理
	void ReadBMPFile(char* str, char* strFileName,int textureID);

	//对三维模型进行分割
	vector<GCSlice> SplitModel();

public slots:

	//改变对称轴显示状态
	void ChangeAxisStatus();

	//改变空间直角坐标系显示状态
	void ChangeCoorSystemStatus();

	//删除选中的横截面
	void DeleteCuttingPlane(); 

	//添加横截面
	void AddCuttingPlane();	

	//重置当前的显示坐标系
	void ResetCoordSystem();	
	
	//将选中的Slice的展开方式设为圆柱
	void ChangeEnrollMode2Cylinder();

	//将选中的Slice的展开方式设为圆台
	void ChangeEnrollMode2TruncatedCone();

	//刷新当前控件的内容
	void UpdateTimer();	

private :
	//鼠标滚轮事件
	void wheelEvent(QWheelEvent *e);	

	//鼠标移动事件
	void mouseMoveEvent(QMouseEvent *e);	

	//鼠标点击事件
	void mousePressEvent(QMouseEvent *e);	

	//鼠标按键释放事件
	void mouseReleaseEvent(QMouseEvent *e);

	//键盘按键事件
	void keyPressEvent(QKeyEvent* e);	

	//鼠标右键菜单事件
	void contextMenuEvent(QContextMenuEvent *e);	

	//获取选择的平面
	int GetSelectedPlane(int mouse_x,int mouse_y);	

	//获取选择的Bounding Box
	int GetSlectedBBox(int mouse_x, int mouse_y);	

	//移动横切面
	void MovePlane(int mouse_x,int mouse_y);	

	//计算平面和线段的交点
	bool PlaneLineSegmentIntersectPt(Normal planeNormal,Vertex planePoint,Vertex vertexA,Vertex vertexB,Vertex &interVertex);	

	//计算三角面片被横切面分割后的结果
	bool FaceIntersectPlane(Face face,float* abcd,VECTOR3 planePoint,vector<Vertex> &interPoints);	

	//计算三角面片被横切面分割后的结果
	bool FaceIntersectPlane(Vertex vertexA,Vertex vertexB,Vertex vertexC,float* abcd,VECTOR3 planePoint,vector<Vertex> &interPoints);	

	//计算横切面的半径
	float CalIntersectRadius(Vertex planePoint);	

	//计算梯度信息
	vector<float> ComputeGradients();

	//根据纹理信息计算最优切分位置
	void ComputeBestCuts();

	//显示横切面
	void ShowCuttingPlanes();	
	
	//显示对称轴
	void ShowPrincipalAxis(Vertex lowestPoint,Vertex highestPoint,Normal axisNormal);
	
	//显示空间直角坐标系
	void ShowCoorSystem();	

	//计算三维模型在对称轴上的投影的上下界
	void ComputeLowestAndHighest(Vertex lowestVertex,Vertex highestVertex,Normal axis,Vertex &lowestPointOnAxis,Vertex &highestPointOnAxis,Normal &low2Top); 

	 //将三维模型均匀地分为10块，此处作为初始情况出现，应该有优化后的分段方法
	void CutModelUniformly();
	
	//显示选中的Slice片段
	void ShowSelectedSliceBBox();

	void DrawSolidCircleRed(float x, float y,float z, float radius);		//画红色的实线圆
	void DrawWiredCircleRed(float x, float y,float z, float radius);		//画红色的虚线圆
	void DrawSolidCircleGreen(float x, float y,float z, float radius);	//画绿色的实线圆
	void DrawWiredCircleGreen(float x, float y,float z, float radius);	//画绿色的虚线圆
	void GetWorldCoordinate(int mouse_x, int mouse_y, double &world_x,double &world_y,double &world_z);

private:
	vcg::Trackball trackball;	//trackball，用于实现缩放平移等操作
	vector<GLfloat> bbox;	//

	GLModel* glmModel;	//三维模型
	//vector<GCSlice> cuttedSlices;
	vector<Vertex> profileCenters;	//横切面的中心坐标
	vector<float> profileRadius;	//横切面的半径
	vector<SliceBBox> sliceBBoxes;	//slice的Bounding Box

	double pModelViewMatrix[16];
	float modelCenter[3];	//三维模型的中心
	bool isLeftPressed;	

	bool b_show_principal_axis;	//是否显示对称轴
	bool b_show_coord_system;	//是否显示坐标系

	bool b_plane_selected;	//判断是否选中平面
	bool b_leftbutton_pressed;	//鼠标左键是否按下
	int selected_plane_index;	//选中平面的索引
	int selected_slice_index;	//选中的Slice片段的索引
	float disThreshold;		//在选中切分平面时的误差允许范围

	QMenu* right_button_menu;	//右键菜单
	QAction *actionDeletePlane,*actionAddPlane,*actionResetTrackball;	//鼠标右键选项

	QMenu* select_enroll_mode_menu;		//右键菜单
	QAction *actionSelectCylinderMode, *actionSelectTrunConeMode;  //右键菜单上的选项

	float context_position_x;	//右键菜单的位置
	float context_position_y;	//右键菜单的位置

	Vertex lowestVertexOnAxis,highestVertexOnAxis;	//三维模型在对称轴上的投影点的上下界

	int paintNum;	//渲染构件的次数
};