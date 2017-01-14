#pragma  once
#include "GLModel.h"
#include "3DCircleExtract.h"
#include "SliceBBox.h"
#include "GCSlice.h"



//////////////////////////////////////////////////////////////////////////
//@intro	该类主要实现对三维模型进行切割，得到若干GCSlice
//////////////////////////////////////////////////////////////////////////
class Enroll{
public:
	GLModel* mModel;		//三维模型
	Normal mAxisNormal;		//主对称轴方向向量
	Point mLowPointOnAxis,mHighPointOnAxis; //	投影到对称轴上的最高点和最低点
	vector<vector<Vertex>> mLineSegments;	//三维模型与横截面的相交圆周集合
	vector<Vertex> mProfileCenters;		//三维模型与横截面相交圆周的圆心集合
	vector<float> mProfileRadius;		//三维模型与横截面相交圆周的半径集合
	vector<SliceBBox> sliceBBoxes;		//三维模型被横截面切分得到的每个部分的BoundingBox集合

public:
	//构造函数
	Enroll(GLModel* glmmodel,vector<Vertex> profilecenters,vector<float> profileradius,vector<SliceBBox> bboxes);
	
	//对三维模型进行切分，获得Slice集合
	vector<GCSlice> CutModel();
	
	//计算三维模型在投影在对称轴上的最高点和最低点
	void CalLowAndHigh(Vertex lowestVertex,Vertex highestVertex,Normal axis,Point &lowestPointOnAxis,Point &highestPointOnAxis,Normal &low2Top);
	
	//对三角面片进行切分
	vector<Face> CutFace(Face face, vector<Vertex> interVertices,vector<Texture> interTextures);
	
	//判断三角面面片是否与平面相交，以及交点的坐标和纹理
	int IsFaceCutByPlane(Face face,float* abcd,VECTOR3 planePoint, vector<Vertex> &interPoints,vector<Texture> &interPointTextures);
	
	//判断线段与平面是否相交以及交点的坐标和纹理
	bool PlaneLineIntersectPt(Normal planeNormal,Vertex plabePoint,Vertex vertexA,Vertex vertexB,Vertex &interVertex,Texture texture_a,Texture texture_b,Texture &interTexture);
	
	//计算模型与横截面相交圆周的半径
	float LineSegmentRadius(Vertex centerPoint,Normal axisNormal,vector<Vertex> lineSegment3D);
	
	//计算模型与横截面相交圆周的圆心
	Vertex LineSegmentCenter(vector<Vertex> lineSegment3d);
	
	//计算点到直线的距离
	float DisToAxis(Vertex centerPoint,Normal axisNormal, Vertex curPoint);
};


