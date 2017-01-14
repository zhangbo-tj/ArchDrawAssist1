#pragma  once
#include "StdAfx.h"
#include <math.h>
#include <vector>
#include <assert.h>
#include "KDTree\matrix.h"

using namespace std;

#define M_PI		3.14159265358979323846


//////////////////////////////////////////////////////////////////////////
//@intro	��ά�����࣬��Ҫ������άģ���ϵĶ������ز���
//////////////////////////////////////////////////////////////////////////
class Vertex{
public:
	float mVx,mVy,mVz;// �����x,y,z����
public:	
	//Ĭ�Ϲ��캯��
	Vertex();	

	//��ʼ����������
	void Init();	

	//���캯��
	Vertex(float vx,float vy,float vz);		
	
	//���ظ�ֵ������
	Vertex &operator=(Vertex v);
	
	//�����ཻ
	Vertex Add(Vertex v);
	
	//�������
	Vertex Minus(Vertex v);

	//�����������ʵ��
	Vertex Mul(float d);

	//����������
	Vertex Cross(Vertex v);
	
	//����������ת��Ϊfloat*ָ��
	float* ToFloat();

	//����������
	float Dot(Vertex v);

	//���㵥λ��
	void Normalize();

	//��ȡ�����x����
	float GetX();

	//��ȡ�����y����
	float GetY();

	//��ȡ�����y����
	float GetZ();
	
	//���ö����X����
	void SetX(float vx);

	//���ö����Y����
	void SetY(float vy);

	//���ö����Z����
	void SetZ(float vz);

	//���ö�������
	void Set(float* v);

	//���ö�������
	void Set(float vx, float vy, float vz);
	
	//���ö�������
	void Set(double vx, double vy, double vz);
};


/////////////////////////////////////////////
//@intro	��ά�ռ��ڵķ�������
/////////////////////////////////////////////
class Normal{
private:
	float mNx,mNy,mNz;	//��������X,Y,Z����
public:
	//���������캯��
	Normal();	

	//���������캯��
	Normal(float nx,float ny,float nz);	

	//���������캯��
	Normal(Vertex v1, Vertex v2, Vertex v3);
	
	//��ʼ������������
	void Init();

	//��λ��������
	void Normalize();

	//���ظ�ֵ�����
	Normal& operator=(Normal n);

	//���س˺������
	Normal& operator*(float f);

	//���ؼӺ������
	Normal& operator+(Normal n);

	//���ؼ��������
	Normal& operator-(Normal n);
	
	//���÷���������
	void Set(float _x,float _y,float _z);
	
	//�������������
	Normal Cross(Normal v);

	//��������ת��Ϊָ��
	float* ToFloat();

	//��ȡ��������X����
	float GetX();

	//��ȡ��������Y����
	float GetY();

	//��ȡ��������Z����
	float GetZ();

	//���÷�������X����
	void SetX(float vx);

	//���÷�������Y����
	void SetY(float vy);

	//���÷�������Z����
	void SetZ(float vz);

	//���������÷�����
	void Set(float* v);

	//�������������
	float DotProduct(Normal tempNormal);
	
	//���㷨��������
	float GetLength();
};


////////////////////////////////////////////////////////////////////////
//@intro	��ά�ռ��ڵĵ㣬������ά�����ز���
////////////////////////////////////////////////////////////////////////
class Point{
private: 
	float mPx,mPy,mPz;	//���X,Y,Z����
	Normal mNormal;	//��ķ�����
	Normal mPrincipal1,mPrincipal2;		//�������������
	int index;		//�������
	int trinum;//���������εı��
public:
	//Ĭ�Ϲ��캯��
	Point();

	//���캯��
	Point(float px,float py,float pz,int num = 0);
	
	//����ά����ת��Ϊ����
	float* ToFloat();

	//��ȡ���X����
	float GetX();

	//��ȡ���Y����
	float GetY();

	//��ȡ���Z����
	float GetZ();

	//��ȡ�����ڵ�������Ƭ
	int GetTrinum();

	//���õ��X����
	void SetX(float px);

	//���õ��Y����
	void SetY(float py);

	//���õ��Z����
	void SetZ(float pz);

	//���õ����ڵ�������Ƭ
	void SetTrinum(int num);

	//��ȡ�������
	int GetIndex();

	//��ȡ��ķ�����
	Normal GetNormal();

	//��ȡ�������������1
	Normal GetPrincipal1();

	//��ȡ�������������2
	Normal GetPrincipal2();

	//���õ�ķ�����
	void SetNormal(Normal n);
	
	//���õ������������1
	void SetPrincipal1(Normal n);

	//���õ������������2
	void SetPrincipal2(Normal n);

	//���õ������
	void SetIndex(int i);
};


//////////////////////////////////////////////////////////
//@intro	�����࣬������άģ��������������Ͳ���
/////////////////////////////////////////////////////////
class Texture{
private:
	float mTx,mTy;
public:
	//Ĭ�Ϲ��캯��
	Texture();

	//���캯��
	Texture(float tx, float ty);
	
	//��ʼ������
	void Init();
	
	//���ط��ϼӷ������
	Texture& operator+=(const Texture tex);
	
	//����������ת��Ϊ����
	float* ToFloat();

	//��ȡ����X����
	float GetX();

	//��ȡ����Y����
	float GetY();

	//��������X����
	void SetX(float tx);

	//��������Y����
	void SetY(float ty);

	//������������
	void Set(float tx,float ty);
	
	//��������������Ϊ�����ֵ
	void Set(float* t);
};


//////////////////////////////////////////////////////////////////////////
//@intro	��ά�ռ���������Ƭ
/////////////////////////////////////////////////////////////////////////
class Triangle{
private:
	int mVindices[3];	//������Ƭ������������
	int mNindices[3];	//������Ƭ���㷨������������
	int mTindices[3];	//������Ƭ������������
	int mFindex;		//������Ƭ���������� 
public:
	//���캯��
	Triangle(int v[3],int n[3],int t[3],int f);
	
	//Ĭ�Ϲ��� ����
	Triangle();

	//��ȡ������Ƭ������������
	int* GetVindices();

	//��ȡ������Ƭ���㷨������������
	int* GetNindices();

	//��ȡ������Ƭ����������������
	int* GetTindices();

	//��ȡ������Ƭ����������
	int GetFindex();

	//����������Ƭ������������
	void SetVindices(int* v);

	//����������Ƭ���㷨��������
	void SetNindices(int* n);

	//����������Ƭ������������
	void SetTindices(int* t);

	//����������Ƭ����������
	void SetFindex(int f);
};

//////////////////////////////////////////////////////////////////////////
//@intro	��ά�ռ���������Ƭ�࣬��Ҫ���ڼ�����άģ�ͱ�������з�ʱ��Slice����
//////////////////////////////////////////////////////////////////////////
class Face{
public:
	Vertex mVertexA,mVertexB,mVertexC;		//������Ƭ����������
	Texture mTextureA,mTextureB,mTextureC;		//������Ƭ�����������������
	int mGroupNum;	//������Ƭ���ڵ�Group
	bool mIsPositive;	//������Ƭ�Ƿ�λ��yozƽ����Ҳ�

	//�ֱ�ȡ������Ƭ��������������
	Vertex GetVertexA();	
	Vertex GetVertexB();
	Vertex GetVertexC();

	//��ȡ������Ƭ�����������������
	Texture GetTextureA();
	Texture GetTextureB();
	Texture GetTextureC();
	
	//Ĭ�Ϲ��캯��
	Face();

	//���캯��
	Face(Vertex a,Vertex b,Vertex c,Texture texture_a,Texture texture_b,Texture texture_c);
	
	//����������Ƭ���ڵ�Group
	void SetGroup(int num);

	//��ȡ������Ƭ���ڵ�Group
	int GetGroup();

	//����������Ƭ�Ƿ�λ��yoz�������Ҳ�
	void SetPositive(bool p);

	//��ȡ������Ƭ�Ƿ�λ��������Ƭ���Ҳ�����
	bool GetPositive();
};

//////////////////////////////////////////////////////////////////////////
//@intro	��άģ�͵Ĳ����࣬���ڴ�����άģ�͵ĳ���������
//////////////////////////////////////////////////////////////////////////
class Material{
private:
	char* m_name;			//name of matrial
	float mDiffuse[4];	//diffuse component
	float mAmbient[4];	//ambient component
	float mSpecular[4];	//specular component
	float mEmmissive[4];	//emmissive component
	float mShininess;	//specular exponent
	char*  map_file;	//filename of the texture map
public:
	//���캯��
	Material(char* n, float d[4],float a[4],float s[4],float e[4],float sh,char* file);
	
	//���캯��
	Material(char* n);

	//�������������
	void SetDiffuse(float d[4]);

	//���û��������
	void SetAmbient(float a[4]);

	//���þ��淴�����
	void SetSpecular(float s[4]);

	//���ò�������
	void SetName(char* n);

	//���þ��淴�����
	void SetShiness(float sh);

	//��������ӳ���ļ�����
	void SetMapfile(char* mfile);

	//��ȡ���������
	float* GetDiffuse();

	//��ȡ���������
	float* GetAmbient();

	//��ȡ���淴�����
	float* GetSpecular();

	//��ȡ������
	char* GetName();

	//��ȡ���淴��ϵ��
	float GetShiness();

	//��ȡ����ӳ���ļ���
	char* GetMapfile();
		
};

//////////////////////////////////////////////////////////////////////////
//@intro	��άģ�͵�Group�࣬��Ϊ������Ƭ�ļ���
//////////////////////////////////////////////////////////////////////////
class Group{
private:
	char* mName;		//name of this group
	vector<int> mTriangles;	//group�е�triangle��������
	int mMaterialno;			//group��material������
public:
	//Ĭ�Ϲ��캯��
	Group();

	//���캯��
	Group(char* n,int m);

	//���캯��
	Group(char* gname);

	//��ȡ����
	char* GetName();

	//��ȡGroup�е�������Ƭ��������
	vector<int> GetTriangles();

	//��ȡGroup��Ӧ�Ĳ��ʱ��
	int GetMaterialno();

	//����Group����
	void SetName(char* n);

	//���ò��ʱ��
	void SetMaterialno(int m);

	//���Group�����������Ƭ
	void AddTriangle(int t);
};



/////////////////////////////////////////////////////
//@intro	�жϵ�P�Ƿ��ڵ�A,B,C���ڲ� 
/////////////////////////////////////////////////////
bool SameSide(Vertex A, Vertex B, Vertex C, Point P);

/////////////////////////////////////////////////////
//@intro	�жϵ�vertex�Ƿ��ڶ���ΪA,B,C���������ڲ�                                                    
/////////////////////////////////////////////////////
bool IsInTriangle(Vertex A, Vertex B, Vertex C, Point point);

/////////////////////////////////////////////////////
//@intro	�ж�����������Ƭ�Ƿ�����                                                                    
/////////////////////////////////////////////////////
bool IsNeighborTri(Triangle triangle1,Triangle triangle2);

/////////////////////////////////////////////////////
//@intro	������������֮�����                                                                     
/////////////////////////////////////////////////////
float Distance(Vertex v1, Vertex v2);

/////////////////////////////////////////////////////
//@intro	���㶥����������֮��ľ���                                                                      
/////////////////////////////////////////////////////
float Distance(Vertex v1, Point p2);

/////////////////////////////////////////////////////
//@intro	��������������֮��ľ���                                                                      
/////////////////////////////////////////////////////
float Distance(Point p1, Point p2);

/////////////////////////////////////////////////////
//@intro	�����������㹹�ɵ������ε����                                                                      
/////////////////////////////////////////////////////
float TriangleArea(Vertex va,Vertex vb,Vertex vc);

/////////////////////////////////////////////////////
//@intro	�����������㹹�ɵ������ε����
////////////////////////////////////////////////////
float TriangleArea(Vertex va,Point vb,Vertex vc);


//////////////////////////////////////////////////////////////////////////
//@intro	�ж϶����Ƿ�λ��ƽ���Ϸ�
//////////////////////////////////////////////////////////////////////////
int IsVertexUpperPlane(Vertex vertexA, float* abcd, VECTOR3 centerPoint);

//////////////////////////////////////////////////////////////////////////
//@intro	�ж�������Ƭ�Ƿ�λ��ƽ���Ϸ�
//////////////////////////////////////////////////////////////////////////
int IsFaceUpperPlane(Face face,float* abcd,VECTOR3 centerPoint);