#pragma  once
#include "G3DDataType.h"
#include "GeneralUse.h"
#include <vector>
#include <gl\glut.h>
#include <direct.h>
#include <io.h>
#include <stdio.h>
#include <string>
#include <SVD>

using namespace std;
using namespace Eigen;
/* defines */
#define GLM_NONE     (0)    /* render with only vertices */
#define GLM_FLAT     (1 << 0)    /* render with facet normals */       // 0000 0001��1����0λ,��0000 0001
#define GLM_SMOOTH   (1 << 1)    /* render with vertex normals */      // 0000 0001��1����1λ����0000 0010
#define GLM_TEXTURE  (1 << 2)    /* render with texture coords */      //0000 0001��1����2λ����0000 0100
#define GLM_COLOR    (1 << 3)    /* render with colors */              //0000 0001��1����3λ����0000 1000
#define GLM_MATERIAL (1 << 4)    /* render with materials */           //0000 0001��1����4λ����0001 0000


//////////////////////////////////////////////////////////////////////////
//@intro	��άģ�͵��࣬��Ҫ������άģ�͵���ؼ���
//////////////////////////////////////////////////////////////////////////
class GLModel{
public:
	char* mPathname;		//path to this model
	char* mMtllibname;	//name of the material library

	vector<Vertex> mVertices;	//array of vertices in model
	vector<Normal> mNormals;		//array of normals in model
	vector<Texture> mTexcoods;	//array of texcoods in model
	vector<Material> mMaterials;		//array of materials in model
	vector<Triangle> mTriangles;		//array of triangles in model
	vector<Normal> mFacetNormals;	//array of facetnorms in model
	vector<Group> mGroups;	//array of groups in model
	vector<Normal> mPrincipleDir1, mPrincipleDir2;  //curvature ������

	vector<vector<int>> v_1ringVer;	//�����1 ring �ڽӶ���
	vector<vector<int>> v_1ringTri;	//�����1 ring�ڽ�������
	vector<vector<int>> t_1ringTri;	//�����ε�1 ring �ڽ�������

	float mAvePt[3];		//��άģ�͵�����ƽ��ֵ
	float mMaxPt[3],mMinPt[3];	//��άģ�͵���������±߽�
	Normal mAxisNormal;		//��άģ�͵ĶԳ���
	GLuint* mTextureArray;	//��άģ�͵���������
	
public:
	//�������
	void ClearTextures();		

	//���캯��
	GLModel();

	//�����ǰ����
	void ClearData();

	//���ظ�ֵ������
	GLModel& operator=(const GLModel & model);
	
	//�ж�bounding box�Ƿ�Ϊunit box
	bool IsUnitBox();

	//����ģ������ƽ��ֵ(�м�ֵ)�����ֵ����Сֵ
	void CalAverage();

	//����������
	void CalPrincipal();

	//��ȡ�������ļ�
	bool Read_pdirFile(string strfile);
	
	//��λ��ģ��
	void Unitlize();
	
	//����ģ����ķ�����
	void CalFacetNormals();
	
	//��ȡ���������ڵ�������Ƭ
	Triangle GetTriofPoint();
	
	//����������
	void GenIncidentTable(); 
	
	//����Գ��᷽������
	void CalAxisNormal();
};

//��ȡobj�ļ���ȡ��άģ��
GLModel* GLMReadOBJ(const char* filename);

//����άģ��д��obj�ļ�
void GLMWriteOBJ(GLModel* model, char* filename,GLuint mode);

//������Ӧ�Ĳ���
int GLMFindMaterial(GLModel* model, char* name);

//��ȡMTL�ļ�
void GLMReadMTL(GLModel* model, char* buf);

//д��MTL�ļ�
void GLMWriteMTL(GLModel model, char* modelpath,char* mtllibname);

//��ȡBMP�����ļ�
int GLMReadBMP(GLuint textureArray[], char* strFileName,int textureID);

//��ȡ��άģ�����ڵ�·��
char* GLMDirName(char* path);

//������άģ��
void GLMDraw(GLModel* model,GLuint mode);
