#pragma  once

#include "GLModel.h"
#include "SliceBBox.h"

//////////////////////////////////////////////////////////////////////////
//@intro	ƽ���ϵ�����꣬��Ҫ����չ�����Slice�ļ���
//////////////////////////////////////////////////////////////////////////
struct PlanePoint{
	float mPx;	//x����
	float mPy;	//y����

	///////////////////////////////////////////////
	//@intro	����ת��Ϊ����
	///////////////////////////////////////////////
	float* ToFloat(){
		static float n[2];
		n[0] = mPx;
		n[1] = mPy;
		return n;
	}

	///////////////////////////////////////////////
	//@intro	���캯��
	///////////////////////////////////////////////
	PlanePoint(float x,float y){
		mPx = x;
		mPy = y;
	}

	///////////////////////////////////////////////
	//@inro		Ĭ�Ϲ��캯��
	///////////////////////////////////////////////
	PlanePoint(){
		mPx = mPy = 0;
	}		

	///////////////////////////////////////////////
	//@intro	��ά��ĵ��
	//@param	point:��ά��
	//@return	��˵�ֵ
	///////////////////////////////////////////////
	float dotProduct(PlanePoint point){
		return (mPx * point.mPx + mPy * point.mPy);
	}

	/////////////////////////////////////////////
	//@intro	��ά���ƽ��
	//@param	offsetX:ƽ�Ƶ�X����
	//@param	offsetY:ƽ�Ƶ�Y����
	/////////////////////////////////////////////
	void Translate(float offsetX,float offsetY){
		mPx += offsetX;  
		mPy += offsetY;
	}	
};


//////////////////////////////////////////////////////////////////////////
//@intro	ƽ���ϵ���������Ҫ����չ�����Slice�ļ���
//////////////////////////////////////////////////////////////////////////
struct PlaneVector{
	float mVx;	//�����ĵ�һ����
	float mVy;	//�����ĵڶ�����
	
	///////////////////////////////////////////////////////
	//@intro	�������캯��
	//@param	startPoint:������ʼ������
	//@param	endPoint:�����յ�����
	///////////////////////////////////////////////////////
	PlaneVector(PlanePoint startPoint,PlanePoint endPoint){
		mVx = endPoint.mPx - startPoint.mPx;  
		mVy = endPoint.mPy - startPoint.mPy;
	}	

	///////////////////////////////////////////////////////
	//@intro	���캯��
	//@param	tempX,tempY:��������������
	///////////////////////////////////////////////////////
	PlaneVector(float tempX,float tempY){
		mVx = tempX;
		mVy = tempY;
	}

	///////////////////////////////////////////////////////
	//@intro	�����������
	//@param	pVector:���˵�����
	//@return	��˵Ľ��
	//////////////////////////////////////////////////////
	float DotProduct(PlaneVector pVector){
		return (mVx * pVector.mVx + mVy * pVector.mVy);
	}

	/////////////////////////////////////////////////////
	//@intro	���������ĳ���
	//@return	�����ĳ���
	//  [5/9/2016 zhangbo]
	/////////////////////////////////////////////////////
	float Length(){
		return sqrt(mVx * mVx + mVy * mVy);
	}

	////////////////////////////////////////////////////
	//@intro	���س˺�*������,����ǰ���������쳤
	//@param	mul:���˵ı���
	//@return	�쳤�������
	//  [5/9/2016 zhangbo]
	///////////////////////////////////////////////////
	PlaneVector& operator*(float mul){
		mVx *= mul;
		mVy *= mul;
		return *this;
	}

	/////////////////////////////////////////////////
	//@intro	���������е�λ��
	/////////////////////////////////////////////////
	void Unit(){
		float length = Length();
		mVx /= length; 
		mVy /= length;
	}
};

//////////////////////////////////////////////////////////////////////////
//@intro	��ά������Ƭ����Ҫ����չ�����ƽ��Slice����
//////////////////////////////////////////////////////////////////////////
struct PlaneFace{
	PlanePoint mVertexA, mVertexB, mVertexC;	//�����ε���������
	Texture mTextureA, mTextureB, mTextureC;	//�����������������������
	
	//////////////////////////////////////////////////////////////////////////
	//@intro	���캯��
	//@param	vertexA, vertexB, vertexC:������Ƭ��������������
	//@param	textureA, textureB, textureC:������Ƭ���������������
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
	//@intro	��������Ƭ����ƽ�Ʋ���
	//@param	offsetX:ƽ�Ƶ�X����
	//@param	offsetY:ƽ�Ƶ�Y����
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
	vector<PlaneFace> mPlaneFaces;	//ƽ��������Ƭ����
	float mMinRadius,mMidRadius,mMaxRadius;	//չ�����Բ��Ƭ�ε���С�뾶�����뾶
	PlanePoint mCenterPoint;	//չ����Բ����Բ��
	EnrollMode mEnrollMode;		//չ����ʽ����Ҫȥ����
	PlanePoint mLeftEndPoint,mMiddlePoint,mRightEndPoint;	//չ����õ���ƽ��Slice��3�����Ƶ�
	float mMinPt[2],mMaxPt[2];	//չ�����Slice�ı߽�
	float mRadiusThreshold;		//minRadius,middleRadius,maxRadius֮��Ĳ�ֵ
	bool mIsSelected;	//��ǰslice�Ƿ�ѡ��
	bool mIsLeftSelected, mIsRightSelected;		//����Ƶ���ҿ��Ƶ��Ƿ�ѡ��
	vector<vector<float>> mProportions;		//Բ̨չ����ʽÿ��������ռ�ı���
	vector<vector<float>> mDistances;			//Բ��չ����ʽÿ��������Գ���ľ���

public:
	//���캯��
	EnrolledSlice(vector<PlaneFace> faces,PlanePoint leftPoint,PlanePoint middlePoint, PlanePoint rightPoint,
		float minRadius,float maxRadius,float radiusThres,PlanePoint centerPoint,EnrollMode enrollMode);
	
	//����չ�����Slice
	void DrawSlice();

	//���µ�ǰչ�����Slice�����±߽�
	void UpdateBoudnary();

	//�жϵ㵱ǰ���Ƿ���EnrolledSlice��
	bool IsPointInSlice(float px,float py);

	//����չ��Slice�ı߽�
	void ShowBoundary();

	//����Slice�Ŀ��Ƶ�
	void ShowControlPoints();

	//�����ض�λ�úͰ뾶��Բ��,��Ҫ���ڻ���Բ���ı߽�
	void DrawArc(float centerX,float centerY,float startAngle,float endAngle,float radius);

	//�Ե�ǰSlice����ƽ�ƣ�ƽ����Ϊ(offetX,offsetY)
	void Translate(float offsetX,float offsetY);

	//�ı䵱ǰSlice��ѡ��״̬�������ǰ״̬Ϊ��ѡ�У����޸�Ϊδѡ��
	void ChangeSelectedState();

	//�жϸ�Slice�Ƿ�ѡ��
	bool IsSliceSelected();		

	//�ı䵱ǰSlice����С�뾶�����뾶
	void ChangeRadius(PlanePoint oldCenterPoint,PlanePoint oldMidPoint,PlanePoint oldRightEndPoint,float oldMinRadius,float oldMaxRadius,
		PlanePoint newCenterPoint,PlanePoint newMidPoint,PlanePoint newRightPoint,float newMinRadius,float newMaxRadius);

	//�޸�ÿһ�����λ��
	void ChangePoint(PlanePoint &oldPoint,
		PlanePoint oldCenterPoint,PlanePoint oldMidPoint,float oldMinRadius,float oldMaxRadius,float oldMaxAngle,
		PlanePoint newCenterPoint,PlanePoint newMidPoint,float newMinRadius,float newMaxRadius,float newMaxAngle);
	float CalCosAngle(PlanePoint leftPoint,PlanePoint midPoint,PlanePoint rightPoint);

	//����ʵ��Բ�����ڻ��ƿ��Ƶ�
	void Draw_solid_circle_red(float x, float y, float radius);

	//�ж��Ƿ�����˻��Ҷ˵Ŀ��Ƶ㱻ѡ��
	bool IsControlEndSelected(float clickX, float clickY);

	//�ֱ����ÿ��������Ƭ��ÿ���������Բ�����������Գ���֮��ľ���ı�������Slice�����ߵ����λ��
	void ComputeRelativeParameters(vector<vector<float>> &proportionVector,vector<vector<float>> &distanceVector);

	//ͨ���ƶ����˵Ŀ��Ƶ���ʵ��Slice�ı���
	void MoveEndPoint(float clickX, float newClickY);

	//����Slice�Ŀ��Ƶ㱻ѡ�е�״̬
	void ResetControlPoint();


	//����Բ̨��ʽչ��ʱ���µĲ�������չ��,��ȡ����µ�����λ��
	void MovePointInFrustum(PlanePoint &point, PlanePoint newMiddlePoint, PlanePoint newCenterPoint,
		float proportion,float relativeDistance,float maxAngle, float newMidRadius);

	//������Բ���巽ʽչ��ʱ�������µĲ�����������������λ��
	void MovePointInCylinder(PlanePoint &point, PlanePoint newMidPoint,float proportion,float newMaxLength);

	//��ȡ��EnrolledSlice���ϱ߽磬�����������ֵ
	float GetUpperBound();

	//��ȡ��EnrolledSlice���±߽磬����С����ֵ
	float GetLowerBound();
};