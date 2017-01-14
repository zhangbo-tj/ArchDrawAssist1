#pragma once

#include <iostream>
#include <QWidget>
#include <QtOpenGL\QGLwidget>
#include <QtGui>
#include "glm.h"


class GLMTexture:public QGLWidget{
	public:
	Q_OBJECT
public:
	GLMTexture(QWidget *parent = 0);
	~GLMTexture(void);

	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();
	void loadTextures();
	void keyPressEvent(QKeyEvent *e);
};