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
//@intro	������Ҫ����ʵ�ִ��ڵ����ؼ�������ʾ��άģ�ͺ�չ���������Ŀؼ�����
//////////////////////////////////////////////////////////////////////////
class DisplayBox:public QWidget{
	public:
	Q_OBJECT
public:
	DisplayBox(QWidget *parent = 0);
	~DisplayBox();

public:
	//��ȡobj�ļ�
	void LoadObjFile(QString fileName);

public slots:
	//��ʼչ��ģ�͵Ĳ�
	void StartUnfoldModel();
	
public:
	GLModelArea *mModelArea;		//��ʾ��άģ�͵Ŀؼ�
	GLTextureArea *mTextureArea;		//��ʾ��ά���������
	GLModel* mModel;		//��άģ��

	//	GLuint* textureArray;
};

#endif