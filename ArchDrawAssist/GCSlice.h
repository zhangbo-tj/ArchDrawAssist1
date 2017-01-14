#pragma once
#include "GLModel.h"
#include "3DCircleExtract.h"
#include <utility>
#include <iterator>
#include "vertexHull.h"
#include "SliceBBox.h"
#include "GCEnrolledSlice.h"

//////////////////////////////////////////////////////////////////////////
//@intro	展开后的三维Slice
//////////////////////////////////////////////////////////////////////////
class GCSlice{
private:
	vector<Face> mFaces;		//三角面片集合
	Normal mAxisNormal;		//主对称轴
	Vertex mLowerPoint,mUpperPoint;	//上下横截面上的点
	float mLowerRadius,mHigherRadius;		//上截面的半径和下截面的半径
	float mHeight,H;		//高度
	float mMinRadius,mMaxRadius;		//按照圆台展开时的最大半径和最小半径	
	bool mFlag;		//圆台是否是正立的
	GLuint mTextureNum;		//纹理编号
	EnrollMode mEnrollMode;	//展开方式
	
public:
	//构造函数
	GCSlice(Normal axisnormal,Vertex lowerpoint,Vertex higherpoint,float lowRadius, float highRadius,EnrollMode mode);
	
	//判断三角面片是否位于slice区间内，即位于该slice的上下截面中间
	bool IsContainFace(Face face);

	//将三角面片添加到slice内
	void AddFace(Face face);
	
	//获取三角面片数
	int GetFacesSize();

	//判断圆台是否是正立的
	bool IsRight();

	//获取当前slice的所有三角面片
	vector<Face> GetFaces();

	//拟合圆台
	void ConialfrustumFitting();

	//计算点到对称轴的距离
	float Vertex2Axis(Vertex curPoint,Normal axisNormal);

	//拟合圆柱，即找到距离对称轴最远的距离
	float CylinderSegmentFitting();

	//展开Slice,得到展开后的平面三角面片集合
	EnrolledSlice EnrollSlice(float& baseY);

	//按照圆台方式展开三角面片，得到展开后的平面三角形集合
	vector<PlaneFace> EnrollAsCylinderSegment(float &baseY,PlanePoint &leftPoint,PlanePoint &middlePoint,PlanePoint &rightPoint,
		float &minRadius,float &maxRadius,float &radiusThres,PlanePoint &centerPoint);

	//按照圆台方式展开三角面片，得到展开后的平面三角形集合
	vector<PlaneFace> EnrollAsConicalFrustum(float &baseY,PlanePoint &leftPoint,PlanePoint &middlePoint,PlanePoint &rightPoint,
		float &minRadius,float &maxRadius,float &radiusThres,PlanePoint &centerPoint);

	//按照圆柱方式展开三角面片，得到展开后的三角面片
	PlaneFace ExpandAsCylinderSegment(Face face,float R,float addedY);

	//按照圆台方式展开三角面片 得到展开后的三角面片
	PlaneFace ExpandAsConicalFrustum(Face face, float height, float R, float H, bool flag);

};
