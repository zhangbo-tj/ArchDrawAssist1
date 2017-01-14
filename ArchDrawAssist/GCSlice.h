#pragma once
#include "GLModel.h"
#include "3DCircleExtract.h"
#include <utility>
#include <iterator>
#include "vertexHull.h"
#include "SliceBBox.h"
#include "GCEnrolledSlice.h"

//////////////////////////////////////////////////////////////////////////
//@intro	չ�������άSlice
//////////////////////////////////////////////////////////////////////////
class GCSlice{
private:
	vector<Face> mFaces;		//������Ƭ����
	Normal mAxisNormal;		//���Գ���
	Vertex mLowerPoint,mUpperPoint;	//���º�����ϵĵ�
	float mLowerRadius,mHigherRadius;		//�Ͻ���İ뾶���½���İ뾶
	float mHeight,H;		//�߶�
	float mMinRadius,mMaxRadius;		//����Բ̨չ��ʱ�����뾶����С�뾶	
	bool mFlag;		//Բ̨�Ƿ���������
	GLuint mTextureNum;		//������
	EnrollMode mEnrollMode;	//չ����ʽ
	
public:
	//���캯��
	GCSlice(Normal axisnormal,Vertex lowerpoint,Vertex higherpoint,float lowRadius, float highRadius,EnrollMode mode);
	
	//�ж�������Ƭ�Ƿ�λ��slice�����ڣ���λ�ڸ�slice�����½����м�
	bool IsContainFace(Face face);

	//��������Ƭ��ӵ�slice��
	void AddFace(Face face);
	
	//��ȡ������Ƭ��
	int GetFacesSize();

	//�ж�Բ̨�Ƿ���������
	bool IsRight();

	//��ȡ��ǰslice������������Ƭ
	vector<Face> GetFaces();

	//���Բ̨
	void ConialfrustumFitting();

	//����㵽�Գ���ľ���
	float Vertex2Axis(Vertex curPoint,Normal axisNormal);

	//���Բ�������ҵ�����Գ�����Զ�ľ���
	float CylinderSegmentFitting();

	//չ��Slice,�õ�չ�����ƽ��������Ƭ����
	EnrolledSlice EnrollSlice(float& baseY);

	//����Բ̨��ʽչ��������Ƭ���õ�չ�����ƽ�������μ���
	vector<PlaneFace> EnrollAsCylinderSegment(float &baseY,PlanePoint &leftPoint,PlanePoint &middlePoint,PlanePoint &rightPoint,
		float &minRadius,float &maxRadius,float &radiusThres,PlanePoint &centerPoint);

	//����Բ̨��ʽչ��������Ƭ���õ�չ�����ƽ�������μ���
	vector<PlaneFace> EnrollAsConicalFrustum(float &baseY,PlanePoint &leftPoint,PlanePoint &middlePoint,PlanePoint &rightPoint,
		float &minRadius,float &maxRadius,float &radiusThres,PlanePoint &centerPoint);

	//����Բ����ʽչ��������Ƭ���õ�չ�����������Ƭ
	PlaneFace ExpandAsCylinderSegment(Face face,float R,float addedY);

	//����Բ̨��ʽչ��������Ƭ �õ�չ�����������Ƭ
	PlaneFace ExpandAsConicalFrustum(Face face, float height, float R, float H, bool flag);

};
