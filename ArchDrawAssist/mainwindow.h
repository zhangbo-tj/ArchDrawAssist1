#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "StdAfx.h"
#include <QtGui/QMainWindow>
#include "GeneratedFiles\ui_mainwindow.h"
#include "GLModelArea.h"
#include <iostream>
#include <QFileDialog>
#include <QtCore\qtextcodec.h>
#include "DisplayBox.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:

	//构造函数
	MainWindow(QWidget *parent = 0);
	
	//析构函数
	~MainWindow();

	//连接信号和槽
	void InitConnect();

private slots:

	//打开Obj文件
	void OpenFile();
	
	//保存Obj文件
	void SaveFiles();
	
private:
	DisplayBox* displaybox;		//窗口主控件

private:
	Ui::MainWindowClass ui;		
};

#endif // MAINWINDOW_H
