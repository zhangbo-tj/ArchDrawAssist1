#pragma  once
#include "StdAfx.h"
#include <math.h>
#include <vector>
#include <assert.h>
#include "KDTree\matrix.h"

using namespace std;

#define M_PI		3.14159265358979323846


//////////////////////////////////////////////////////////////////////////
//@intro	三维顶点类，主要包含三维模型上的顶点的相关操作
//////////////////////////////////////////////////////////////////////////
class Vertex{
public:
	float mVx,mVy,mVz;// 顶点的x,y,z坐标
public:	
	//默认构造函数
	Vertex();	

	//初始化顶点坐标
	void Init();	

	//构造函数
	Vertex(float vx,float vy,float vz);		
	
	//重载赋值操作符
	Vertex &operator=(Vertex v);
	
	//顶点相交
	Vertex Add(Vertex v);
	
	//顶点相减
	Vertex Minus(Vertex v);

	//顶点坐标乘以实数
	Vertex Mul(float d);

	//顶点叉乘运算
	Vertex Cross(Vertex v);
	
	//将顶点坐标转换为float*指针
	float* ToFloat();

	//顶点点乘运算
	float Dot(Vertex v);

	//顶点单位化
	void Normalize();

	//获取顶点的x坐标
	float GetX();

	//获取顶点的y坐标
	float GetY();

	//获取顶点的y坐标
	float GetZ();
	
	//设置顶点的X坐标
	void SetX(float vx);

	//设置顶点的Y坐标
	void SetY(float vy);

	//设置顶点的Z坐标
	void SetZ(float vz);

	//设置顶点坐标
	void Set(float* v);

	//设置顶点坐标
	void Set(float vx, float vy, float vz);
	
	//设置顶点坐标
	void Set(double vx, double vy, double vz);
};


/////////////////////////////////////////////
//@intro	三维空间内的法向量类
/////////////////////////////////////////////
class Normal{
private:
	float mNx,mNy,mNz;	//法向量的X,Y,Z分量
public:
	//法向量构造函数
	Normal();	

	//法向量构造函数
	Normal(float nx,float ny,float nz);	

	//法向量构造函数
	Normal(Vertex v1, Vertex v2, Vertex v3);
	
	//初始化法向量参数
	void Init();

	//单位化法向量
	void Normalize();

	//重载赋值运算符
	Normal& operator=(Normal n);

	//重载乘号运算符
	Normal& operator*(float f);

	//重载加号运算符
	Normal& operator+(Normal n);

	//重载减号运算符
	Normal& operator-(Normal n);
	
	//设置法向量参数
	void Set(float _x,float _y,float _z);
	
	//法向量叉乘运算
	Normal Cross(Normal v);

	//将法向量转换为指针
	float* ToFloat();

	//获取法向量的X分量
	float GetX();

	//获取法向量的Y分量
	float GetY();

	//获取法向量的Z分量
	float GetZ();

	//设置法向量的X分量
	void SetX(float vx);

	//设置法向量的Y分量
	void SetY(float vy);

	//设置法向量的Z分量
	void SetZ(float vz);

	//用数组设置法向量
	void Set(float* v);

	//法向量点乘运算
	float DotProduct(Normal tempNormal);
	
	//计算法向量长度
	float GetLength();
};


////////////////////////////////////////////////////////////////////////
//@intro	三维空间内的点，用于三维点的相关操作
////////////////////////////////////////////////////////////////////////
class Point{
private: 
	float mPx,mPy,mPz;	//点的X,Y,Z坐标
	Normal mNormal;	//点的法向量
	Normal mPrincipal1,mPrincipal2;		//点的主方向向量
	int index;		//点的索引
	int trinum;//所在三角形的编号
public:
	//默认构造函数
	Point();

	//构造函数
	Point(float px,float py,float pz,int num = 0);
	
	//将三维坐标转换为数组
	float* ToFloat();

	//获取点的X坐标
	float GetX();

	//获取点的Y坐标
	float GetY();

	//获取点的Z坐标
	float GetZ();

	//获取点所在的三角面片
	int GetTrinum();

	//设置点的X坐标
	void SetX(float px);

	//设置点的Y坐标
	void SetY(float py);

	//设置点的Z坐标
	void SetZ(float pz);

	//设置点所在的三角面片
	void SetTrinum(int num);

	//获取点的索引
	int GetIndex();

	//获取点的法向量
	Normal GetNormal();

	//获取点的主方向向量1
	Normal GetPrincipal1();

	//获取点的主方向向量2
	Normal GetPrincipal2();

	//设置点的法向量
	void SetNormal(Normal n);
	
	//设置点的主方向向量1
	void SetPrincipal1(Normal n);

	//设置点的主方向向量2
	void SetPrincipal2(Normal n);

	//设置点的索引
	void SetIndex(int i);
};


//////////////////////////////////////////////////////////
//@intro	纹理类，用于三维模型纹理的相关运算和操作
/////////////////////////////////////////////////////////
class Texture{
private:
	float mTx,mTy;
public:
	//默认构造函数
	Texture();

	//构造函数
	Texture(float tx, float ty);
	
	//初始化纹理
	void Init();
	
	//重载符合加法运算符
	Texture& operator+=(const Texture tex);
	
	//将纹理坐标转换为数组
	float* ToFloat();

	//获取纹理X坐标
	float GetX();

	//获取纹理Y坐标
	float GetY();

	//设置纹理X坐标
	void SetX(float tx);

	//设置纹理Y坐标
	void SetY(float ty);

	//设置纹理坐标
	void Set(float tx,float ty);
	
	//将纹理坐标设置为数组的值
	void Set(float* t);
};


//////////////////////////////////////////////////////////////////////////
//@intro	三维空间内三角面片
/////////////////////////////////////////////////////////////////////////
class Triangle{
private:
	int mVindices[3];	//三角面片顶点索引数组
	int mNindices[3];	//三角面片顶点法向量索引数组
	int mTindices[3];	//三角面片纹理索引数组
	int mFindex;		//三角面片法向量索引 
public:
	//构造函数
	Triangle(int v[3],int n[3],int t[3],int f);
	
	//默认构造 函数
	Triangle();

	//获取三角面片顶点索引数组
	int* GetVindices();

	//获取三角面片顶点法向量索引数组
	int* GetNindices();

	//获取三角面片顶点纹理索引数组
	int* GetTindices();

	//获取三角面片法向量索引
	int GetFindex();

	//设置三角面片顶点索引数组
	void SetVindices(int* v);

	//设置三角面片顶点法向量数组
	void SetNindices(int* n);

	//设置三角面片顶点索引数组
	void SetTindices(int* t);

	//设置三角面片法向量索引
	void SetFindex(int f);
};

//////////////////////////////////////////////////////////////////////////
//@intro	三维空间内三角面片类，主要用于计算三维模型被横截面切分时的Slice计算
//////////////////////////////////////////////////////////////////////////
class Face{
public:
	Vertex mVertexA,mVertexB,mVertexC;		//三角面片的三个顶点
	Texture mTextureA,mTextureB,mTextureC;		//三角面片三个顶点的纹理坐标
	int mGroupNum;	//三角面片所在的Group
	bool mIsPositive;	//三角面片是否位于yoz平面的右侧

	//分别取三角面片的三个顶点坐标
	Vertex GetVertexA();	
	Vertex GetVertexB();
	Vertex GetVertexC();

	//获取三角面片三个顶点的纹理坐标
	Texture GetTextureA();
	Texture GetTextureB();
	Texture GetTextureC();
	
	//默认构造函数
	Face();

	//构造函数
	Face(Vertex a,Vertex b,Vertex c,Texture texture_a,Texture texture_b,Texture texture_c);
	
	//设置三角面片所在的Group
	void SetGroup(int num);

	//获取三角面片所在的Group
	int GetGroup();

	//设置三角面片是否位于yoz横截面的右侧
	void SetPositive(bool p);

	//获取三角面片是否位于三角面片的右侧属性
	bool GetPositive();
};

//////////////////////////////////////////////////////////////////////////
//@intro	三维模型的材质类，用于创建三维模型的场景和纹理
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
	//构造函数
	Material(char* n, float d[4],float a[4],float s[4],float e[4],float sh,char* file);
	
	//构造函数
	Material(char* n);

	//设置漫反射参数
	void SetDiffuse(float d[4]);

	//设置环境光参数
	void SetAmbient(float a[4]);

	//设置镜面反射参数
	void SetSpecular(float s[4]);

	//设置材质名字
	void SetName(char* n);

	//设置镜面反射参数
	void SetShiness(float sh);

	//设置纹理映射文件名称
	void SetMapfile(char* mfile);

	//获取漫反射参数
	float* GetDiffuse();

	//获取环境光参数
	float* GetAmbient();

	//获取镜面反射参数
	float* GetSpecular();

	//获取材质名
	char* GetName();

	//获取镜面反射系数
	float GetShiness();

	//获取纹理映射文件名
	char* GetMapfile();
		
};

//////////////////////////////////////////////////////////////////////////
//@intro	三维模型的Group类，作为三角面片的集合
//////////////////////////////////////////////////////////////////////////
class Group{
private:
	char* mName;		//name of this group
	vector<int> mTriangles;	//group中的triangle索引集合
	int mMaterialno;			//group的material的索引
public:
	//默认构造函数
	Group();

	//构造函数
	Group(char* n,int m);

	//构造函数
	Group(char* gname);

	//获取名称
	char* GetName();

	//获取Group中的三角面片索引集合
	vector<int> GetTriangles();

	//获取Group对应的材质编号
	int GetMaterialno();

	//设置Group名称
	void SetName(char* n);

	//设置材质编号
	void SetMaterialno(int m);

	//向该Group中添加三角面片
	void AddTriangle(int t);
};



/////////////////////////////////////////////////////
//@intro	判断点P是否在点A,B,C的内侧 
/////////////////////////////////////////////////////
bool SameSide(Vertex A, Vertex B, Vertex C, Point P);

/////////////////////////////////////////////////////
//@intro	判断点vertex是否在顶点为A,B,C的三角形内部                                                    
/////////////////////////////////////////////////////
bool IsInTriangle(Vertex A, Vertex B, Vertex C, Point point);

/////////////////////////////////////////////////////
//@intro	判断两个三角面片是否相邻                                                                    
/////////////////////////////////////////////////////
bool IsNeighborTri(Triangle triangle1,Triangle triangle2);

/////////////////////////////////////////////////////
//@intro	计算两个顶点之间距离                                                                     
/////////////////////////////////////////////////////
float Distance(Vertex v1, Vertex v2);

/////////////////////////////////////////////////////
//@intro	计算顶点与样本点之间的距离                                                                      
/////////////////////////////////////////////////////
float Distance(Vertex v1, Point p2);

/////////////////////////////////////////////////////
//@intro	计算两个样本点之间的距离                                                                      
/////////////////////////////////////////////////////
float Distance(Point p1, Point p2);

/////////////////////////////////////////////////////
//@intro	计算三个顶点构成的三角形的面积                                                                      
/////////////////////////////////////////////////////
float TriangleArea(Vertex va,Vertex vb,Vertex vc);

/////////////////////////////////////////////////////
//@intro	计算三个顶点构成的三角形的面积
////////////////////////////////////////////////////
float TriangleArea(Vertex va,Point vb,Vertex vc);


//////////////////////////////////////////////////////////////////////////
//@intro	判断顶点是否位于平面上方
//////////////////////////////////////////////////////////////////////////
int IsVertexUpperPlane(Vertex vertexA, float* abcd, VECTOR3 centerPoint);

//////////////////////////////////////////////////////////////////////////
//@intro	判断三角面片是否位于平面上方
//////////////////////////////////////////////////////////////////////////
int IsFaceUpperPlane(Face face,float* abcd,VECTOR3 centerPoint);