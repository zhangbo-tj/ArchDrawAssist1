#include "StdAfx.h"
#include "GLTextureArea.h"


//////////////////////////////////////////////////////////////////////////
//@intro	当前控件构造器
//@param	parent:当前控件的父控件
//////////////////////////////////////////////////////////////////////////
GLTextureArea::GLTextureArea(QWidget* parent){
	setMouseTracking(true);
	setFocusPolicy(Qt::ClickFocus);
	mLeftKeyPressed = false;
	oldX = oldY = 0;
	mSelectedSliceIndex = -1;
	mTranslateX = mTranslateY = 0;
	mControlPointSelected = false;
}


//////////////////////////////////////////////////////////////////////////
//@intro	当前控件构造器
//@param	model:三维模型
//@param	parent:当前控件的父控件
//////////////////////////////////////////////////////////////////////////
GLTextureArea::GLTextureArea(GLModel* model, QWidget *parent){
	loadModel(model);
	
	setMouseTracking(true);
	setFocusPolicy(Qt::ClickFocus);
	mLeftKeyPressed = false;
	oldX = oldY = 0;
	mSelectedSliceIndex = -1;
	mTranslateX = mTranslateY = 0;
	mControlPointSelected = false;
}


//////////////////////////////////////////////////////////////////////////
//@intro	控件析构函数
//////////////////////////////////////////////////////////////////////////
GLTextureArea::~GLTextureArea(void){
	
}

//////////////////////////////////////////////////////////////////////////
//@intro	保存展开的纹理图片
//////////////////////////////////////////////////////////////////////////
void GLTextureArea::SaveTextureFile(){
	string imageName = GLMDirName(glmModel->mPathname);
	imageName += "savedTexture.bmp";
	QPixmap pixmap = QPixmap::grabWindow(QApplication::desktop()->winId(),mapToGlobal(pos()).x()-frameGeometry().width(),mapToGlobal(pos()).y(),frameGeometry().width()-10,frameGeometry().height()-10);
	pixmap.save(QString(imageName.c_str()),"bmp");
	QMessageBox::about(NULL,QObject::tr("通知"),QObject::tr("纹理截图已保存"));
}

//////////////////////////////////////////////////////////////////////////
//@intro	初始化当前控件的参数
//////////////////////////////////////////////////////////////////////////
void GLTextureArea::initializeGL(){
	readMtlFile();
	
	glClearColor(1.0, 1.0, 1.0, 0);
	//glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);

	glOrtho(-2,2,-2,2,-2,2);
	trackball.center = Point3f(glmModel->mAvePt[0],glmModel->mAvePt[1],glmModel->mAvePt[2]);
	trackball.radius = 1;

	trackball.ClearModes();
	trackball.modes[0] = NULL;
	trackball.modes[Trackball::BUTTON_LEFT] = new PanMode();
	//trackball.modes[Trackball::BUTTON_LEFT] = new PlaneMode(0,0,1,0);
	trackball.modes[Trackball::KEY_LEFT] = trackball.modes[Trackball::KEY_RIGHT] = trackball.modes[Trackball::KEY_UP] = trackball.modes[Trackball::KEY_DOWN] = new PanMode();
	trackball.modes[Trackball::WHEEL] = new ScaleMode();

	//trackball.GetView();
	trackball.Apply(false);
}


//////////////////////////////////////////////////////////////////////////
//@intro	控件重新修改大小事件
//@param	newWidth:控件新的宽度
//@param	newHeight:控件新的高度
//////////////////////////////////////////////////////////////////////////
void GLTextureArea::resizeGL(int newWidth,int newHeight){
	glViewport(0, 0, (GLint)newWidth, (GLint)newHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-2,2,-2,2,-2,2);
	//gluPerspective(45.0,(GLfloat)newWidth/(GLfloat)newHeight, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


//////////////////////////////////////////////////////////////////////////
//@intro	绘制当前控件函数
//////////////////////////////////////////////////////////////////////////
void GLTextureArea::paintGL(){
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
//	glTranslated(0,glmModel->avePt[1],-(glmModel->maxPt[2]-glmModel->minPt[2])*2);
	glPushMatrix();

	glTranslatef(mTranslateX,mTranslateY,0);
	//trackball.GetView();
	trackball.Apply(false);
	for(auto i = 0; i < mEnrolledSlices.size();i++){
		mEnrolledSlices[i].DrawSlice();
		//mEnrolledSlices[i].ShowBoundary();
	}
}
  

//////////////////////////////////////////////////////////////////////////
//@intro	读取三维模型
//////////////////////////////////////////////////////////////////////////
void GLTextureArea::loadModel(GLModel* model)
{
	
	glmModel = model;
	glmModel->CalFacetNormals();
	//glmModel->unitlize();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(1.0f, 1.0f, 1.0f, 0);
}


//////////////////////////////////////////////////////////////////////////
//@intro	读取MTL文件生成三维模型对应的纹理，重新映射纹理
//////////////////////////////////////////////////////////////////////////
void GLTextureArea::readMtlFile(){
	
	FILE* file;
	char* dir;
	char buf[128];
	int materialNum = 0;
	char* map_file;
	AUX_RGBImageRec *pBitmap = NULL;

	file = fopen(glmModel->mMtllibname,"r");
	while(fscanf(file,"%s",buf) != EOF){
		switch(buf[0]){
		case 'n':
			materialNum++;
			break;
		case 'm':
			fgets(buf,sizeof(buf),file);
			sscanf(buf,"%s %s",buf,buf);
			pBitmap = auxDIBImageLoadA(buf);	
			if(!pBitmap){
				std::cout<<"Can't find the BMP file"<<endl;
				return;
			}

			if(pBitmap == NULL){
				std::cout<<"Can't read the BMP file: "<<map_file<<endl;
				return;
			}
			
			gluBuild2DMipmaps(GL_TEXTURE_2D,3,pBitmap->sizeX,pBitmap->sizeY,GL_RGB,GL_UNSIGNED_BYTE,pBitmap->data);
		
		
			break;
		default:
			fgets(buf,sizeof(buf),file);
			break;
		}
	}
	cout<<"The mtl file is read successfully"<<endl;
}



//////////////////////////////////////////////////////////////////////////
//@intro	将Qt类型的鼠标键盘事件转换为VCG类型的鼠标键盘事件
//////////////////////////////////////////////////////////////////////////
vcg::Trackball::Button QT2VCG(Qt::MouseButton qtbt, Qt::KeyboardModifiers modifiers){
	int vcgbt = vcg::Trackball::BUTTON_NONE;
	if(qtbt&Qt::LeftButton) vcgbt|= vcg::Trackball::BUTTON_LEFT;
	if(qtbt& Qt::RightButton) vcgbt |= vcg::Trackball::BUTTON_RIGHT;
	if(qtbt& Qt::MidButton) vcgbt|= vcg::Trackball::BUTTON_MIDDLE;
	if(modifiers& Qt::ShiftModifier) vcgbt |= vcg::Trackball::KEY_SHIFT;
	if(modifiers & Qt::ControlModifier) vcgbt|= vcg::Trackball::KEY_CTRL;
	if(modifiers & Qt::AltModifier) vcgbt |= vcg::Trackball::KEY_ALT;
	//if(modifiers & Qt::KeypadModifier) vcgbt |= vcg::Trackball::KEY_UP;


	return vcg::Trackball::Button(vcgbt);
};



//////////////////////////////////////////////////////////////////////////
//@intro	将Qt键盘事件转换为VCG键盘事件
//@param	key:键盘按键
//@param	modifiers:键盘事件modifiers
//////////////////////////////////////////////////////////////////////////
vcg::Trackball::Button QTKey2VCG(int key,Qt::KeyboardModifiers modifiers){
	int vcgbt = 0;

	switch(key){
	case Qt::Key_Up:
		vcgbt = vcg::Trackball::KEY_UP;break;
	case Qt::Key_Down:
		vcgbt = vcg::Trackball::KEY_DOWN; break;
	case Qt::Key_Left:
		vcgbt = vcg::Trackball::KEY_LEFT; break;
	case Qt::Key_Right:
		vcgbt = vcg::Trackball::KEY_RIGHT; break;
	default:
		vcgbt = 0;
	}

	if(modifiers & Qt::ShiftModifier)	vcgbt |= vcg::Trackball::KEY_SHIFT;
	if(modifiers & Qt::ControlModifier)	vcgbt |= vcg::Trackball::KEY_CTRL;
	if(modifiers & Qt::AltModifier)	vcgbt |= vcg::Trackball::KEY_ALT;

	return vcg::Trackball::Button(vcgbt);
}

//////////////////////////////////////////////////////////////////////////
//@intro	鼠标滚轮滚动事件
//@param	e：鼠标滚轮事件
//////////////////////////////////////////////////////////////////////////
void GLTextureArea::wheelEvent(QWheelEvent *e){
	cout<<"wheel event"<<endl;
	const int WHEEL_STEP = 120;
	double change_rate = 0.1;
	double change = (e->delta()<0)?(1+change_rate):(1-change_rate);
	double size_temp= 0.0;
	trackball.MouseWheel(e->delta()/float(WHEEL_STEP));
	updateGL();
}


//////////////////////////////////////////////////////////////////////////
//@intro	鼠标移动事件
//@param	e:鼠标移动事件
//////////////////////////////////////////////////////////////////////////
void GLTextureArea::mouseMoveEvent(QMouseEvent *e){
	double world_x,world_y,world_z;
	GetWorldCoordinate(e->x(), height()-e->y(),world_x,world_y,world_z);
	if(mLeftKeyPressed && mSelectedSliceIndex != -1 && mControlPointSelected == false){
		mEnrolledSlices[mSelectedSliceIndex].Translate( (float)(world_x - oldX), (float)(world_y - oldY));
	}
	else if(mLeftKeyPressed && mSelectedSliceIndex != -1 && mControlPointSelected == true){
		float oldUpperBound = mEnrolledSlices[mSelectedSliceIndex].GetUpperBound();
		float oldLowerBound = mEnrolledSlices[mSelectedSliceIndex].GetLowerBound();
		mEnrolledSlices[mSelectedSliceIndex].MoveEndPoint((float)world_x,(float)world_y);
		float newUpperBound = mEnrolledSlices[mSelectedSliceIndex].GetUpperBound();
		float newLowerBound = mEnrolledSlices[mSelectedSliceIndex].GetLowerBound();
		for(auto i = 0; i < mEnrolledSlices.size(); i++){
			if(i < mSelectedSliceIndex){
				mEnrolledSlices[i].Translate(0, newLowerBound - oldLowerBound);
			}else if(i > mSelectedSliceIndex){
				mEnrolledSlices[i].Translate(0, newUpperBound - oldUpperBound);
			}
		}
		//mEnrolledSlices[mSelectedSliceIndex].MoveRightEndPoint((float)world_x,(float)world_y);
	}
	else if(mLeftKeyPressed && mSelectedSliceIndex == -1){
		mTranslateX += (float)(world_x - oldX);
		mTranslateY += (float)(world_y - oldY);
	}
	oldX = static_cast<float>(world_x);
	oldY = static_cast<float>(world_y);
	updateGL();
}



//////////////////////////////////////////////////////////////////////////
//@intro	鼠标移动事件
//@param	e:鼠标移动事件
//////////////////////////////////////////////////////////////////////////
void GLTextureArea::mousePressEvent(QMouseEvent *e){
	double world_x,world_y,world_z;
	GetWorldCoordinate(e->x(), height()-e->y(),world_x,world_y,world_z);
	if(e->button()==Qt::LeftButton){
		mLeftKeyPressed = true;
		mSelectedSliceIndex = GetSelectedSlice((float)world_x,(float)world_y);
		if(mSelectedSliceIndex != -1){
			if(mEnrolledSlices[mSelectedSliceIndex].IsControlEndSelected((float)world_x,(float)world_y)){
				mControlPointSelected = true;
			}else{
				mControlPointSelected = false;
			}
		}else{
			mControlPointSelected = false;
		}
	}
	oldX = world_x;
	oldY = world_y;
	updateGL();
}


//////////////////////////////////////////////////////////////////////////
//@intro	当前控件的鼠标释放事件
//@param	e:鼠标释放控件
//////////////////////////////////////////////////////////////////////////
void GLTextureArea::mouseReleaseEvent(QMouseEvent *e){
	double world_x,world_y,world_z;
	GetWorldCoordinate(e->x(), height()-e->y(),world_x,world_y,world_z);
	oldX = world_x;
	oldY = world_y;
	if(e->button()== Qt::LeftButton){
		trackball.MouseUp(e->x(),height()-e->y(),QT2VCG(e->button(),e->modifiers()));
		mLeftKeyPressed = false;
		if(mSelectedSliceIndex != -1 && mControlPointSelected){
			mEnrolledSlices[mSelectedSliceIndex].ResetControlPoint();
			mControlPointSelected = false;
		}
	}
	update();
	updateGL();
}

//////////////////////////////////////////////////////////////////////////
//@intro	当前控件的键盘释放事件
//@param	e:键盘释放控件
//////////////////////////////////////////////////////////////////////////
void GLTextureArea::keyReleaseEvent(QKeyEvent *e){
	if(e->key()== Qt::Key_Control)
		trackball.MouseUp(0,0,QT2VCG(Qt::NoButton,Qt::ControlModifier));
	else if(e->key()== Qt::Key_Shift) 
		trackball.MouseUp(0,0,QT2VCG(Qt::NoButton,Qt::ShiftModifier));
	else if(e->key()== Qt::Key_Alt)
		trackball.MouseUp(0,0,QT2VCG(Qt::NoButton,Qt::AltModifier));
	else if(mSelectedSliceIndex != -1 && e->key() == Qt::Key_Up){
		mEnrolledSlices[mSelectedSliceIndex].Translate(0,0.05);
	}else if(mSelectedSliceIndex != -1 && e->key() == Qt::Key_Down){
		mEnrolledSlices[mSelectedSliceIndex].Translate(0,-0.05);
	}else if(mSelectedSliceIndex != -1 && e->key() == Qt::Key_Left){
		mEnrolledSlices[mSelectedSliceIndex].Translate(-0.05,0);
	}else if(mSelectedSliceIndex != -1 && e->key() == Qt::Key_Right){
		mEnrolledSlices[mSelectedSliceIndex].Translate(0.05,0);
	}
	update();
	updateGL();
}


//////////////////////////////////////////////////////////////////////////
//@intro	当前控件的键盘按压事件
//@param	e:键盘按压事件
//////////////////////////////////////////////////////////////////////////
void GLTextureArea::keyPressEvent(QKeyEvent *e){
	if(e->key()== Qt::Key_Control)
		trackball.MouseDown(0,0,QT2VCG(Qt::NoButton,Qt::ControlModifier));
	else if(e->key()== Qt::Key_Shift) 
		trackball.MouseDown(0,0,QT2VCG(Qt::NoButton,Qt::ShiftModifier));
	else if(e->key()== Qt::Key_Alt){
		cout<<"mouse alt:"<<endl;
		trackball.MouseDown(0,0,QT2VCG(Qt::NoButton,Qt::AltModifier));
	}
}


//////////////////////////////////////////////////////////////////////////
//@intro	读入slices
//@param	slices:GCSlice数组
//////////////////////////////////////////////////////////////////////////
void GLTextureArea::loadSlices(vector<GCSlice> gcSlices){
	mSlices = gcSlices;
	float baseY = -100;
	if(mEnrolledSlices.size() > 0){
		mEnrolledSlices.clear();
	}
	for(auto i = 0; i < mSlices.size();i++){
		EnrolledSlice tempEnrollSlice = mSlices[i].EnrollSlice(baseY);
		mEnrolledSlices.push_back(tempEnrollSlice);
	}
	mControlPointSelected = false;
	mSelectedSliceIndex = -1;
	updateGL();
}


//////////////////////////////////////////////////////////////////////////
//@intro	将鼠标位置转换为世界坐标下的坐标值
//@param	mouse_x,mouse_y:鼠标位置
//@return	world_x,world_y,world_z:世界坐标系下的坐标值
//  [4/18/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
void GLTextureArea::GetWorldCoordinate(int mouse_x, int mouse_y, double &world_x,double &world_y,double &world_z){
	GLdouble modelView[16];
	GLdouble projection[16];
	GLint viewport[4];

	glGetDoublev(GL_MODELVIEW_MATRIX,modelView);
	glGetDoublev(GL_PROJECTION_MATRIX,projection);
	glGetIntegerv(GL_VIEWPORT,viewport);

	float winZ;
	glReadPixels(mouse_x,mouse_y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&winZ);
	gluUnProject((GLdouble)mouse_x,(GLdouble)mouse_y,winZ,
		modelView,projection,viewport,
		&world_x,&world_y,&world_z);
}


//////////////////////////////////////////////////////////////////////////
//@intro	获取当前被选中的Slice
//@param	px,py:当前点的坐标
//@return	int:被选中的Slice的索引
//  [4/18/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
int GLTextureArea::GetSelectedSlice(float px,float py){
	for(auto i = 0; i < mEnrolledSlices.size();i++){
		EnrolledSlice tempEnrolledSlice = mEnrolledSlices[i];
		if(tempEnrolledSlice.IsPointInSlice(px,py)){
			if(mSelectedSliceIndex != -1 && mSelectedSliceIndex != i){
				mEnrolledSlices[mSelectedSliceIndex].ChangeSelectedState();
				mEnrolledSlices[i].ChangeSelectedState();
				return i;
			}else if(mSelectedSliceIndex == -1){
				mEnrolledSlices[i].ChangeSelectedState();
				return i;
			}else{
				return i;
			}
			
		}else{

		}
	}
	for(auto i = 0; i < mEnrolledSlices.size();i++){
		if(mEnrolledSlices[i].IsSliceSelected()){
			mEnrolledSlices[i].ChangeSelectedState();
		}
	}
	return -1;
}

