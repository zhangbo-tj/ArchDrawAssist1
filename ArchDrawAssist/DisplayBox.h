#ifndef DISPLAYBOX_H
#define DISPLAYBOX_H
#include "StdAfx.h"
#include <qgroupbox.h>
#include <QHBoxLayout>
#include <QWidget>
#include "GLModelArea.h"
#include "GLTextureArea.h"
#include "GLModel.h"
#include "GL\glaux.h"

//////////////////////////////////////////////////////////////////////////
//@intro	该类主要用于实现窗口的主控件，即显示三维模型和展开后的纹理的控件区域
//////////////////////////////////////////////////////////////////////////
class DisplayBox:public QWidget{
	public:
	Q_OBJECT
public:
	DisplayBox(QWidget *parent = 0);
	~DisplayBox();

public:
	//读取obj文件
	void LoadObjFile(QString fileName);

public slots:
	//开始展开模型的槽
	void StartUnfoldModel();
	
public:
	GLModelArea *mModelArea;		//显示三维模型的控件
	GLTextureArea *mTextureArea;		//显示二维纹理的区域
	GLModel* mModel;		//三维模型

	//	GLuint* textureArray;
};

#endif