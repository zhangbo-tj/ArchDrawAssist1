#include "StdAfx.h"
#include "DisplayBox.h"

//////////////////////////////////////////////////////////////////////////
//@intro	�ؼ����캯��
//@param	parent:��ǰ�ؼ��ĸ��ؼ�
//////////////////////////////////////////////////////////////////////////
DisplayBox::DisplayBox(QWidget *parent){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//����Ĭ�϶�ȡ��obj�ļ�
	string objFileName = "vaze.obj";
	
	//����obj�ļ�
	mModel = GLMReadOBJ(objFileName.data());
	mModel->Unitlize();	//����άģ�ͽ��й�һ��
	mModel->CalAxisNormal();

	mModelArea = new GLModelArea(mModel,this);		//��ʼ����ʾ��άģ�͵Ŀؼ�
	//vector<Face> faces = tempEnroll.splitModel1();
	mTextureArea = new GLTextureArea(mModel,this);	//��ʼ����ʾ����Ŀؼ�
	//textureArea = new GLTextureArea(glmModel,faces,this);
	
	//��ʼ�����ںͿؼ����֣������ؼ���ӵ���ǰ������
	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(mModelArea);
	layout->addWidget(mTextureArea);
	this->setLayout(layout);
}


//////////////////////////////////////////////////////////////////////////
//@intro	�ؼ���������
//////////////////////////////////////////////////////////////////////////
DisplayBox::~DisplayBox(){
	
}


//////////////////////////////////////////////////////////////////////////
//@intro	����obj�ļ���
//@param	fileName:obj�ļ���	
//////////////////////////////////////////////////////////////////////////
void DisplayBox::LoadObjFile(QString fileName){
	cout<<"The loaded obj file name: "<<fileName.toLatin1().data()<<endl;
	mModel->ClearData();
	mModel = GLMReadOBJ(fileName.toLatin1().data());
	mModel->CalFacetNormals();
	mModel->Unitlize();	//����άģ�ͽ��й�һ��
	mModel->CalAxisNormal();
	//glmModel->unitlize();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	mModelArea->LoadModel(mModel);
	mTextureArea->loadModel(mModel);
}

////////////////////////////////////////////////////////////////////////////
//@intro	��ʼչ����άģ������
////////////////////////////////////////////////////////////////////////////
void DisplayBox::StartUnfoldModel(){
	vector<GCSlice> slices = mModelArea->SplitModel();
	mTextureArea->loadSlices(slices);
}