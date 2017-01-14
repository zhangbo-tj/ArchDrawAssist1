#include "StdAfx.h"
#include "mainwindow.h"


//////////////////////////////////////////////////////////////////////////
//@intro	���캯��
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
//@intro	��������
//////////////////////////////////////////////////////////////////////////
MainWindow::~MainWindow()
{
	/*if(modelArea)
		delete modelArea;
	modelArea = NULL;*/

}

//////////////////////////////////////////////////////////////////////////
//@intro	�����źźͲ�
//////////////////////////////////////////////////////////////////////////
void MainWindow::InitConnect(){
	connect(ui.actionImport, SIGNAL(triggered()), this, SLOT(OpenFile()));
	connect(ui.actionSave, SIGNAL(triggered()), this, SLOT(SaveFiles()));
	connect(ui.actionSaveTexture, SIGNAL(triggered()), displaybox->mTextureArea, SLOT(SaveTextureFile()));

	//���á���ʾ�Գ��ᡱ����ʾ������ϵ��������ĺ���
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
//@intro	���ļ���
//////////////////////////////////////////////////////////////////////////
void MainWindow::OpenFile(){
	QTextCodec::setCodecForTr(QTextCodec::codecForName("gbk"));
	QString file = QFileDialog::getOpenFileName(this, QObject::tr("���ļ�"),"", "*.obj");
	if (!file.size()) return;
	displaybox->LoadObjFile(file);
}


//////////////////////////////////////////////////////////////////////////
//@intro	���������ļ�����δʵ��
//////////////////////////////////////////////////////////////////////////
void MainWindow::SaveFiles(){
	std::cout << "The save files is triggered" << std::endl;
}


