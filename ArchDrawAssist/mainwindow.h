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

	//���캯��
	MainWindow(QWidget *parent = 0);
	
	//��������
	~MainWindow();

	//�����źźͲ�
	void InitConnect();

private slots:

	//��Obj�ļ�
	void OpenFile();
	
	//����Obj�ļ�
	void SaveFiles();
	
private:
	DisplayBox* displaybox;		//�������ؼ�

private:
	Ui::MainWindowClass ui;		
};

#endif // MAINWINDOW_H
