#pragma  once
#include "GLModel.h"
#include "3DCircleExtract.h"
#include "SliceBBox.h"
#include "GCSlice.h"



//////////////////////////////////////////////////////////////////////////
//@intro	������Ҫʵ�ֶ���άģ�ͽ����и�õ�����GCSlice
//////////////////////////////////////////////////////////////////////////
class Enroll{
public:
	GLModel* mModel;		//��άģ��
	Normal mAxisNormal;		//���Գ��᷽������
	Point mLowPointOnAxis,mHighPointOnAxis; //	ͶӰ���Գ����ϵ���ߵ����͵�
	vector<vector<Vertex>> mLineSegments;	//��άģ����������ཻԲ�ܼ���
	vector<Vertex> mProfileCenters;		//��άģ���������ཻԲ�ܵ�Բ�ļ���
	vector<float> mProfileRadius;		//��άģ���������ཻԲ�ܵİ뾶����
	vector<SliceBBox> sliceBBoxes;		//��άģ�ͱ�������зֵõ���ÿ�����ֵ�BoundingBox����

public:
	//���캯��
	Enroll(GLModel* glmmodel,vector<Vertex> profilecenters,vector<float> profileradius,vector<SliceBBox> bboxes);
	
	//����άģ�ͽ����з֣����Slice����
	vector<GCSlice> CutModel();
	
	//������άģ����ͶӰ�ڶԳ����ϵ���ߵ����͵�
	void CalLowAndHigh(Vertex lowestVertex,Vertex highestVertex,Normal axis,Point &lowestPointOnAxis,Point &highestPointOnAxis,Normal &low2Top);
	
	//��������Ƭ�����з�
	vector<Face> CutFace(Face face, vector<Vertex> interVertices,vector<Texture> interTextures);
	
	//�ж���������Ƭ�Ƿ���ƽ���ཻ���Լ���������������
	int IsFaceCutByPlane(Face face,float* abcd,VECTOR3 planePoint, vector<Vertex> &interPoints,vector<Texture> &interPointTextures);
	
	//�ж��߶���ƽ���Ƿ��ཻ�Լ���������������
	bool PlaneLineIntersectPt(Normal planeNormal,Vertex plabePoint,Vertex vertexA,Vertex vertexB,Vertex &interVertex,Texture texture_a,Texture texture_b,Texture &interTexture);
	
	//����ģ���������ཻԲ�ܵİ뾶
	float LineSegmentRadius(Vertex centerPoint,Normal axisNormal,vector<Vertex> lineSegment3D);
	
	//����ģ���������ཻԲ�ܵ�Բ��
	Vertex LineSegmentCenter(vector<Vertex> lineSegment3d);
	
	//����㵽ֱ�ߵľ���
	float DisToAxis(Vertex centerPoint,Normal axisNormal, Vertex curPoint);
};


