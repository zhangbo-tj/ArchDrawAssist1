#include "StdAfx.h"
#include "DisplayBox.h"

//////////////////////////////////////////////////////////////////////////
//@intro	控件构造函数
//@param	parent:当前控件的父控件
//////////////////////////////////////////////////////////////////////////
DisplayBox::DisplayBox(QWidget *parent){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//设置默认读取的obj文件
	string objFileName = "vaze.obj";
	
	//读入obj文件
	mModel = GLMReadOBJ(objFileName.data());
	mModel->Unitlize();	//对三维模型进行归一化
	mModel->CalAxisNormal();

	mModelArea = new GLModelArea(mModel,this);		//初始化显示三维模型的控件
	//vector<Face> faces = tempEnroll.splitModel1();
	mTextureArea = new GLTextureArea(mModel,this);	//初始化显示纹理的控件
	//textureArea = new GLTextureArea(glmModel,faces,this);
	
	//初始化窗口和控件布局，并将控件添加到当前窗口中
	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(mModelArea);
	layout->addWidget(mTextureArea);
	this->setLayout(layout);
}


//////////////////////////////////////////////////////////////////////////
//@intro	控件析构函数
//////////////////////////////////////////////////////////////////////////
DisplayBox::~DisplayBox(){
	
}


//////////////////////////////////////////////////////////////////////////
//@intro	读入obj文件，
//@param	fileName:obj文件名	
//////////////////////////////////////////////////////////////////////////
void DisplayBox::LoadObjFile(QString fileName){
	cout<<"The loaded obj file name: "<<fileName.toLatin1().data()<<endl;
	mModel->ClearData();
	mModel = GLMReadOBJ(fileName.toLatin1().data());
	mModel->CalFacetNormals();
	mModel->Unitlize();	//对三维模型进行归一化
	mModel->CalAxisNormal();
	//glmModel->unitlize();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	mModelArea->LoadModel(mModel);
	mTextureArea->loadModel(mModel);
}

////////////////////////////////////////////////////////////////////////////
//@intro	开始展开三维模型纹理
////////////////////////////////////////////////////////////////////////////
void DisplayBox::StartUnfoldModel(){
	vector<GCSlice> slices = mModelArea->SplitModel();
	mTextureArea->loadSlices(slices);
}