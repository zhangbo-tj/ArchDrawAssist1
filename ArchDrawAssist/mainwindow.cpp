#include "StdAfx.h"
#include "mainwindow.h"


//////////////////////////////////////////////////////////////////////////
//@intro	构造函数
//////////////////////////////////////////////////////////////////////////
MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	std::cout << "MainWindow Created..." << std::endl;
	ui.setupUi(this);
	/*modelArea = new GLModelArea(this);
	setCentralWidget(modelArea);*/
	displaybox = new DisplayBox(this);
	setCentralWidget(displaybox);
	InitConnect();

}

//////////////////////////////////////////////////////////////////////////
//@intro	析构函数
//////////////////////////////////////////////////////////////////////////
MainWindow::~MainWindow()
{
	/*if(modelArea)
		delete modelArea;
	modelArea = NULL;*/

}

//////////////////////////////////////////////////////////////////////////
//@intro	连接信号和槽
//////////////////////////////////////////////////////////////////////////
void MainWindow::InitConnect(){
	connect(ui.actionImport, SIGNAL(triggered()), this, SLOT(OpenFile()));
	connect(ui.actionSave, SIGNAL(triggered()), this, SLOT(SaveFiles()));
	connect(ui.actionSaveTexture, SIGNAL(triggered()), displaybox->mTextureArea, SLOT(SaveTextureFile()));

	//设置“显示对称轴”和显示“坐标系”相关联的函数
	connect(ui.actionCoordidateSystem, SIGNAL(triggered()), displaybox->mModelArea, SLOT(ChangeCoorSystemStatus()));
	connect(ui.actionSymmetryAxis, SIGNAL(triggered()), displaybox->mModelArea, SLOT(ChangeAxisStatus()));
	connect(ui.actionStartUnfold, SIGNAL(triggered()), displaybox, SLOT(StartUnfoldModel()));

	connect(ui.actionImport1, SIGNAL(triggered()), this, SLOT(OpenFile()));
	connect(ui.actionSave1, SIGNAL(triggered()), this, SLOT(SaveFiles()));
	connect(ui.actionSaveTexture1, SIGNAL(triggered()), displaybox->mTextureArea, SLOT(SaveTextureFile()));

	connect(ui.actionCoordidateSystem1,  SIGNAL(triggered()), displaybox->mModelArea, SLOT(ChangeCoorSystemStatus()));
	connect(ui.actionSymmetryAxis1, SIGNAL(triggered()), displaybox->mModelArea, SLOT(ChangeAxisStatus()));
	connect(ui.actionStartUnfold1, SIGNAL(triggered()), displaybox, SLOT(StartUnfoldModel()));
}

//////////////////////////////////////////////////////////////////////////
//@intro	打开文件槽
//////////////////////////////////////////////////////////////////////////
void MainWindow::OpenFile(){
	QTextCodec::setCodecForTr(QTextCodec::codecForName("gbk"));
	QString file = QFileDialog::getOpenFileName(this, QObject::tr("打开文件"),"", "*.obj");
	if (!file.size()) return;
	displaybox->LoadObjFile(file);
}


//////////////////////////////////////////////////////////////////////////
//@intro	保存所有文件，暂未实现
//////////////////////////////////////////////////////////////////////////
void MainWindow::SaveFiles(){
	std::cout << "The save files is triggered" << std::endl;
}


