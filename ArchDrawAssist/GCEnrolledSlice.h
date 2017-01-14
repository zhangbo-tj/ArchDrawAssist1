#pragma  once

#include "GLModel.h"
#include "SliceBBox.h"

//////////////////////////////////////////////////////////////////////////
//@intro	平面上点的坐标，主要用于展开后的Slice的计算
//////////////////////////////////////////////////////////////////////////
struct PlanePoint{
	float mPx;	//x坐标
	float mPy;	//y坐标

	///////////////////////////////////////////////
	//@intro	将点转换为数组
	///////////////////////////////////////////////
	float* ToFloat(){
		static float n[2];
		n[0] = mPx;
		n[1] = mPy;
		return n;
	}

	///////////////////////////////////////////////
	//@intro	构造函数
	///////////////////////////////////////////////
	PlanePoint(float x,float y){
		mPx = x;
		mPy = y;
	}

	///////////////////////////////////////////////
	//@inro		默认构造函数
	///////////////////////////////////////////////
	PlanePoint(){
		mPx = mPy = 0;
	}		

	///////////////////////////////////////////////
	//@intro	二维点的点乘
	//@param	point:二维点
	//@return	点乘的值
	///////////////////////////////////////////////
	float dotProduct(PlanePoint point){
		return (mPx * point.mPx + mPy * point.mPy);
	}

	/////////////////////////////////////////////
	//@intro	二维点的平移
	//@param	offsetX:平移的X距离
	//@param	offsetY:平移的Y距离
	/////////////////////////////////////////////
	void Translate(float offsetX,float offsetY){
		mPx += offsetX;  
		mPy += offsetY;
	}	
};


//////////////////////////////////////////////////////////////////////////
//@intro	平面上的向量，主要用于展开后的Slice的计算
//////////////////////////////////////////////////////////////////////////
struct PlaneVector{
	float mVx;	//向量的第一参数
	float mVy;	//向量的第二参数
	
	///////////////////////////////////////////////////////
	//@intro	向量构造函数
	//@param	startPoint:向量起始点坐标
	//@param	endPoint:向量终点坐标
	///////////////////////////////////////////////////////
	PlaneVector(PlanePoint startPoint,PlanePoint endPoint){
		mVx = endPoint.mPx - startPoint.mPx;  
		mVy = endPoint.mPy - startPoint.mPy;
	}	

	///////////////////////////////////////////////////////
	//@intro	构造函数
	//@param	tempX,tempY:向量的两个参数
	///////////////////////////////////////////////////////
	PlaneVector(float tempX,float tempY){
		mVx = tempX;
		mVy = tempY;
	}

	///////////////////////////////////////////////////////
	//@intro	向量点乘运算
	//@param	pVector:被乘的向量
	//@return	点乘的结果
	//////////////////////////////////////////////////////
	float DotProduct(PlaneVector pVector){
		return (mVx * pVector.mVx + mVy * pVector.mVy);
	}

	/////////////////////////////////////////////////////
	//@intro	计算向量的长度
	//@return	向量的长度
	//  [5/9/2016 zhangbo]
	/////////////////////////////////////////////////////
	float Length(){
		return sqrt(mVx * mVx + mVy * mVy);
	}

	////////////////////////////////////////////////////
	//@intro	重载乘号*操作符,将当前向量进行伸长
	//@param	mul:所乘的倍数
	//@return	伸长后的向量
	//  [5/9/2016 zhangbo]
	///////////////////////////////////////////////////
	PlaneVector& operator*(float mul){
		mVx *= mul;
		mVy *= mul;
		return *this;
	}

	/////////////////////////////////////////////////
	//@intro	对向量进行单位化
	/////////////////////////////////////////////////
	void Unit(){
		float length = Length();
		mVx /= length; 
		mVy /= length;
	}
};

//////////////////////////////////////////////////////////////////////////
//@intro	二维三角面片，主要用于展开后的平面Slice操作
//////////////////////////////////////////////////////////////////////////
struct PlaneFace{
	PlanePoint mVertexA, mVertexB, mVertexC;	//三角形的三个顶点
	Texture mTextureA, mTextureB, mTextureC;	//三角形三个顶点的纹理坐标
	
	//////////////////////////////////////////////////////////////////////////
	//@intro	构造函数
	//@param	vertexA, vertexB, vertexC:三角面片的三个顶点坐标
	//@param	textureA, textureB, textureC:三角面片的三个顶点的纹理
	//  [5/9/2016 zhangbo]
	//////////////////////////////////////////////////////////////////////////
	PlaneFace(PlanePoint vertexA,PlanePoint vertexB,PlanePoint vertexC,
		Texture textureA,Texture textureB,Texture textureC){
			mVertexA = vertexA;
			mVertexB = vertexB;
			mVertexC = vertexC;
			mTextureA = textureA;
			mTextureB = textureB;
			mTextureC = textureC;
	}
	
	//////////////////////////////////////////////////////////////////////////
	//@intro	对三角面片进行平移操作
	//@param	offsetX:平移的X距离
	//@param	offsetY:平移的Y距离
	//  [5/9/2016 zhangbo]
	//////////////////////////////////////////////////////////////////////////
	void Translate(float offsetX,float offsetY){
		mVertexA.Translate(offsetX,offsetY);
		mVertexB.Translate(offsetX,offsetY);
		mVertexC.Translate(offsetX,offsetY);
	}
};

class EnrolledSlice{
private:
	vector<PlaneFace> mPlaneFaces;	//平面三角面片集合
	float mMinRadius,mMidRadius,mMaxRadius;	//展开后的圆环片段的最小半径和最大半径
	PlanePoint mCenterPoint;	//展开后圆环的圆心
	EnrollMode mEnrollMode;		//展开方式，需要去掉？
	PlanePoint mLeftEndPoint,mMiddlePoint,mRightEndPoint;	//展开后得到的平面Slice的3个控制点
	float mMinPt[2],mMaxPt[2];	//展开后的Slice的边界
	float mRadiusThreshold;		//minRadius,middleRadius,maxRadius之间的差值
	bool mIsSelected;	//当前slice是否被选中
	bool mIsLeftSelected, mIsRightSelected;		//左控制点和右控制点是否被选中
	vector<vector<float>> mProportions;		//圆台展开方式每个顶点所占的比例
	vector<vector<float>> mDistances;			//圆柱展开方式每个顶点离对称轴的距离

public:
	//构造函数
	EnrolledSlice(vector<PlaneFace> faces,PlanePoint leftPoint,PlanePoint middlePoint, PlanePoint rightPoint,
		float minRadius,float maxRadius,float radiusThres,PlanePoint centerPoint,EnrollMode enrollMode);
	
	//绘制展开后的Slice
	void DrawSlice();

	//更新当前展开后的Slice的上下边界
	void UpdateBoudnary();

	//判断点当前点是否在EnrolledSlice内
	bool IsPointInSlice(float px,float py);

	//画出展开Slice的边界
	void ShowBoundary();

	//画出Slice的控制点
	void ShowControlPoints();

	//画出特定位置和半径的圆弧,主要用于画出圆环的边界
	void DrawArc(float centerX,float centerY,float startAngle,float endAngle,float radius);

	//对当前Slice进行平移，平移量为(offetX,offsetY)
	void Translate(float offsetX,float offsetY);

	//改变当前Slice的选中状态，如果当前状态为已选中，则修改为未选中
	void ChangeSelectedState();

	//判断该Slice是否被选中
	bool IsSliceSelected();		

	//改变当前Slice的最小半径和最大半径
	void ChangeRadius(PlanePoint oldCenterPoint,PlanePoint oldMidPoint,PlanePoint oldRightEndPoint,float oldMinRadius,float oldMaxRadius,
		PlanePoint newCenterPoint,PlanePoint newMidPoint,PlanePoint newRightPoint,float newMinRadius,float newMaxRadius);

	//修改每一个点的位置
	void ChangePoint(PlanePoint &oldPoint,
		PlanePoint oldCenterPoint,PlanePoint oldMidPoint,float oldMinRadius,float oldMaxRadius,float oldMaxAngle,
		PlanePoint newCenterPoint,PlanePoint newMidPoint,float newMinRadius,float newMaxRadius,float newMaxAngle);
	float CalCosAngle(PlanePoint leftPoint,PlanePoint midPoint,PlanePoint rightPoint);

	//绘制实心圆，用于绘制控制点
	void Draw_solid_circle_red(float x, float y, float radius);

	//判断是否有左端或右端的控制点被选中
	bool IsControlEndSelected(float clickX, float clickY);

	//分别计算每个三角面片的每个顶点的相对参数：包括与对称轴之间的距离的比例和与Slice中轴线的相对位置
	void ComputeRelativeParameters(vector<vector<float>> &proportionVector,vector<vector<float>> &distanceVector);

	//通过移动两端的控制点来实现Slice的变形
	void MoveEndPoint(float clickX, float newClickY);

	//重置Slice的控制点被选中的状态
	void ResetControlPoint();


	//在以圆台方式展开时以新的参数进行展开,获取点的新的坐标位置
	void MovePointInFrustum(PlanePoint &point, PlanePoint newMiddlePoint, PlanePoint newCenterPoint,
		float proportion,float relativeDistance,float maxAngle, float newMidRadius);

	//计算以圆柱体方式展开时，按照新的参数，计算点的新坐标位置
	void MovePointInCylinder(PlanePoint &point, PlanePoint newMidPoint,float proportion,float newMaxLength);

	//获取该EnrolledSlice的上边界，即最大纵坐标值
	float GetUpperBound();

	//获取该EnrolledSlice的下边界，即最小坐标值
	float GetLowerBound();
};