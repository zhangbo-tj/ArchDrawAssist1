#include "StdAfx.h"
#include "GLModelArea.h"

//////////////////////////////////////////////////////////////////////////
//@intro	计算点curPoint到轴上的距离
//@param	centerPoint:轴上的某点
//@param	axisNormal:对称轴的方向向量
//@param	curPoint:当前点的坐标
//return	点到对称轴的距离
//////////////////////////////////////////////////////////////////////////
float disToAxis(Vertex centerPoint,Normal axisNormal, Vertex curPoint){
	Normal normal1 = Normal(curPoint.GetX() - centerPoint.GetX(), curPoint.GetY() - centerPoint.GetY(), curPoint.GetZ() - centerPoint.GetZ());
	//Normal normal1 = Normal(curPoint.vec[0] - centerPoint.vec[0], curPoint.vec[1] - centerPoint.vec[1], curPoint.vec[2] - centerPoint.vec[2]);
	Normal crossNormal = normal1.Cross(axisNormal);
	return crossNormal.GetLength() / axisNormal.GetLength();
}


//////////////////////////////////////////////////////////////////////////
//@return	计算profile的平均半径
//@param	centerPoint:对称轴上的点
//@param	axisNormal:对称轴的方向向量
//@param	lineSegment3D:profile上的点集
//@return	profile上的点到对称轴的平均距离
//////////////////////////////////////////////////////////////////////////
float lineSegment_radius(Vertex centerPoint,Normal axisNormal,vector<Vertex> lineSegment3D){
	float sumDistance= 0;
	if(lineSegment3D.size() == 0){
		return 0;
	}

	for(int i = 0;i < lineSegment3D.size();i++){
		sumDistance += disToAxis(centerPoint,axisNormal,lineSegment3D[i]);
	}
	return sumDistance / lineSegment3D.size();
}


//////////////////////////////////////////////////////////////////////////
//@intro	构造函数，初始化glModelArea控件
//@param	model:三维模型
//@param	parent:当前控件的父控件
//////////////////////////////////////////////////////////////////////////
GLModelArea::GLModelArea(GLModel* model, QWidget* parent){
	setMouseTracking(true);
	setFocusPolicy(Qt::ClickFocus);
	LoadModel(model);
}


//////////////////////////////////////////////////////////////////////////
//@intro	刷新构件的内容
//  [4/15/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
void GLModelArea::UpdateTimer(){
	updateGL();
	//cout<<" The time is 0.1 seconds"<<endl;
}

//////////////////////////////////////////////////////////////////////////
//@intro	析构函数
//////////////////////////////////////////////////////////////////////////
GLModelArea::~GLModelArea(void){

}


//////////////////////////////////////////////////////////////////////////
//  [4/10/2016 zhangbo]
//@intro	创建右键菜单选项，并连接信号与槽
//////////////////////////////////////////////////////////////////////////
void GLModelArea::CreateActions(){
	//创建菜单、菜单项
	right_button_menu = new QMenu();
	QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));
	actionDeletePlane = new QAction(tr("删除横截面"),this);
	actionAddPlane = new QAction(tr("添加横截面"),this);
	actionResetTrackball = new QAction(tr("重置"),this);
	
	//连接信号与槽
	connect(actionDeletePlane,SIGNAL(triggered()),this,SLOT(DeleteCuttingPlane()));
	connect(actionAddPlane,SIGNAL(triggered()),this,SLOT(AddCuttingPlane()));
	connect(actionResetTrackball,SIGNAL(triggered()),this,SLOT(ResetCoordSystem()));

	//设置slice片段的展开方式的二级菜单
	//select_enroll_mode_menu = new QMenu();	//设置展开方式的二级菜单
	actionSelectCylinderMode = new QAction(tr("圆柱展开"),this);		//选取圆柱展开方式
	actionSelectTrunConeMode = new QAction(tr("圆台展开"),this);	//选取圆台展开方式
	
	actionSelectCylinderMode->setCheckable(true);
	actionSelectTrunConeMode->setCheckable(true);

	//连接信号与槽
	connect(actionSelectCylinderMode,SIGNAL(triggered()),this,SLOT(ChangeEnrollMode2Cylinder()));
	connect(actionSelectTrunConeMode,SIGNAL(triggered()),this,SLOT(ChangeEnrollMode2TruncatedCone()));

}



//////////////////////////////////////////////////////////////////////////
//删除选中的横截面
//////////////////////////////////////////////////////////////////////////
void GLModelArea::DeleteCuttingPlane(){
	vector<Vertex>::iterator it = profileCenters.begin();
	profileCenters.erase(it+selected_plane_index);
	profileRadius.erase(profileRadius.begin()+selected_plane_index);
	sliceBBoxes[selected_plane_index-1].setMax(sliceBBoxes[selected_plane_index].getMaxBounding());
	sliceBBoxes.erase(sliceBBoxes.begin()+selected_plane_index);
	selected_plane_index = -1;
	b_plane_selected = false;
	updateGL();
}



//////////////////////////////////////////////////////////////////////////
//添加横截面
//////////////////////////////////////////////////////////////////////////
void GLModelArea::AddCuttingPlane(){
	int insertPosition = 0;

	GLdouble modelView[16];
	GLdouble projection[16];
	GLint viewport[4];

	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetDoublev(GL_PROJECTION_MATRIX,projection);
	glGetIntegerv(GL_VIEWPORT,viewport);

	GLdouble world_x,world_y,world_z;
	float winZ;
	
	glReadPixels(context_position_x,context_position_y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&winZ);

	gluUnProject((GLdouble)context_position_x,(GLdouble)context_position_y,winZ,
		modelView,projection,viewport,
		&world_x,&world_y,&world_z);

	for(int i = 0;i< profileRadius.size()-1;i++){
		double dot1 = (world_x-profileCenters[i].GetX())*glmModel->mAxisNormal.GetX() + 
			(world_y - profileCenters[i].GetY())*glmModel->mAxisNormal.GetY()+
			(world_z-profileCenters[i].GetZ())*glmModel->mAxisNormal.GetZ();
		double dot2 = (world_x-profileCenters[i+1].GetX())*glmModel->mAxisNormal.GetX() + 
			(world_y - profileCenters[i+1].GetY())*glmModel->mAxisNormal.GetY()+
			(world_z-profileCenters[i+1].GetZ())*glmModel->mAxisNormal.GetZ(); 
		if(dot1 * dot2 < 0){
			insertPosition = i;
		}
	}

	Normal axis_normal = glmModel->mAxisNormal;
	double dotProduct = world_x * axis_normal.GetX() + world_y * axis_normal.GetY() + world_z * axis_normal.GetZ();
	double length = axis_normal.GetLength();
	double axisLength = axis_normal.GetLength();
	double lengthUnit = dotProduct / sqrt(length);
	Vertex planePoint = Vertex((float)lengthUnit * axis_normal.GetX(),(float)lengthUnit *axis_normal.GetY(),(float)lengthUnit *axis_normal.GetZ());
	float radius =  CalIntersectRadius(planePoint);
	SliceBBox addedBox = SliceBBox(glmModel->mMinPt,glmModel->mMaxPt,profileCenters[insertPosition],planePoint);
	profileCenters.insert(profileCenters.begin()+insertPosition+1,planePoint);
	profileRadius.insert(profileRadius.begin()+insertPosition+1,radius);
	sliceBBoxes.insert(sliceBBoxes.begin() + insertPosition,addedBox);
	sliceBBoxes[insertPosition+1].setMin(planePoint);
	updateGL();
}


//////////////////////////////////////////////////////////////////////////
//重新设置三维坐标系，即重新设置trackball
//////////////////////////////////////////////////////////////////////////
void GLModelArea::ResetCoordSystem(){
	trackball.SetIdentity();
	updateGL();
}


//////////////////////////////////////////////////////////////////////////
//读入模型
//@param	model:三维模型
//////////////////////////////////////////////////////////////////////////
void GLModelArea::LoadModel(GLModel* model)
{
	if(!glmModel){
		paintNum = 0;
	}else{
		paintNum = 1;
		glClearColor(0.0f, 0.0f, 0.0f, 0);
	}
	glmModel = model;
	ReadMtlFile();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	isLeftPressed = false;

	modelCenter[0]=0;modelCenter[1]=0;modelCenter[2] = 0;
	b_show_coord_system = false;	//显示三维模型初始化为false,即默认不显示坐标系
	b_show_principal_axis = false;	//显示对称轴初始化为false,即默认不显示对称轴

	b_plane_selected = false;
	b_leftbutton_pressed = false;
	selected_plane_index = -1;
	selected_slice_index = -1;

	//创建定时器，每隔0.1s更新一下构件的内容
	QTimer::singleShot(100,this,SLOT(UpdateTimer()));
	QTimer::singleShot(200,this,SLOT(UpdateTimer()));
	QTimer::singleShot(300,this,SLOT(UpdateTimer()));
	QTimer::singleShot(400,this,SLOT(UpdateTimer()));
	QTimer::singleShot(500,this,SLOT(UpdateTimer()));

	float r = width() / height();
	float offsetX, offsetY;
	float w = width(),h = height();
	if(h /w >= 1.25){
		offsetX = glmModel->mMaxPt[0] * 1.25;
		offsetY = offsetX * h / w;
	}
	else{
		offsetY = glmModel->mMaxPt[1] * 1.25;
		offsetX = offsetY * w / h;
	}
	glOrtho(-offsetX,offsetX,-offsetY,offsetY,-3,3);
}


//////////////////////////////////////////////////////////////////////////
//@intro	初始化参数
//////////////////////////////////////////////////////////////////////////
void GLModelArea::initializeGL(){
	ReadMtlFile();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(45.0f, 0.5f, 0.01f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0.0f,0.0f,0.0f, 0);
	
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT |GL_STENCIL_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_CULL_FACE);
    
    // Setup other misc features.
    glEnable( GL_LIGHTING );
    glEnable( GL_NORMALIZE );
    glShadeModel( GL_SMOOTH );

    
    // Setup lighting model.
	GLfloat light_model_ambient[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat light0_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat light0_direction[] = {0.0f, 0.0f, 10.0f, 0.0f};
	GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

	glLightfv(GL_LIGHT0, GL_POSITION, light0_direction);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_model_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glEnable( GL_LIGHT0 );
	
	trackball.center = Point3f(0,0,0);
	trackball.radius = 1;
	//trackball.current_mode = new vcg::CylinderMode(vcg::Point3f(0,0,0),vcg::Point3f(0,1,0));
	trackball.ClearModes();
	
	trackball.modes[0] = NULL;
	trackball.modes[Trackball::BUTTON_LEFT] = new SphereMode();
	//trackball.modes[Trackball::BUTTON_LEFT] = new CylinderMode(Point3f(0,0,0),Point3f(0,1,0));
	trackball.modes[Trackball::BUTTON_MIDDLE] = new PanMode();
	trackball.modes[Trackball::BUTTON_LEFT|Trackball::KEY_CTRL] = new PanMode();
	trackball.modes[Trackball::WHEEL] = new ScaleMode();

	trackball.GetView();
	trackball.Apply(true);
	CreateActions();
}

//////////////////////////////////////////////////////////////////////////
//@intro	右键菜单事件
//@param	右键菜单事件
//////////////////////////////////////////////////////////////////////////
void GLModelArea::contextMenuEvent(QContextMenuEvent* e){
	right_button_menu->clear();
	context_position_x = e->x();
	context_position_y = height() - e->y();
	double world_x,world_y,world_z;
	GetWorldCoordinate(context_position_x,context_position_y,world_x,world_y,world_z);
	if(b_plane_selected== true && selected_plane_index != -1){
		right_button_menu->addAction(actionDeletePlane);
		right_button_menu->addAction(actionResetTrackball);
	}else{
		right_button_menu->addAction(actionResetTrackball);
		
		float* minPt = glmModel->mMinPt;
		float* maxPt = glmModel->mMaxPt;
		if(world_x >= minPt[0] && world_x <= maxPt[0] && world_y >= minPt[1] && world_y <= maxPt[1] && world_z >= minPt[2] && world_z <= maxPt[2]){
			right_button_menu->addAction(actionAddPlane);
			select_enroll_mode_menu = right_button_menu->addMenu(tr("设置展开方式"));
			select_enroll_mode_menu->addAction(actionSelectCylinderMode);
			select_enroll_mode_menu->addAction(actionSelectTrunConeMode);

			if(sliceBBoxes[selected_slice_index].isCylinderMode()){
				actionSelectCylinderMode->setChecked(true);
				actionSelectTrunConeMode->setChecked(false);
			}
			else{
				actionSelectCylinderMode->setChecked(false);
				actionSelectTrunConeMode->setChecked(true);
			}
		}
		
	}
	right_button_menu->exec(QCursor::pos());
	e->accept();
}


//////////////////////////////////////////////////////////////////////////
//@intro	重新设置控件大小
//@param	newWidth：控件新的宽度
//@param	newHeight:控件新的高度
//  [4/15/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
void GLModelArea::resizeGL(int newWidth, int newHeight){
	glViewport(0, 0, (GLint)newWidth, (GLint)newHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float r = newWidth / (float)newHeight;
	//gluPerspective(60, r, 0.1, 10);
	float offsetX, offsetY;
	float w = width(), h = height();
	if(h /w >= 1.25){
		offsetX = glmModel->mMaxPt[0] * 1.25;
		offsetY = offsetX * h / w;
	}
	else{
		offsetY = glmModel->mMaxPt[1] * 1.25;
		offsetX = offsetY * w / h;
	}

	glOrtho(-offsetX,offsetX,-offsetY,offsetY,-3,3);
	glMatrixMode(GL_MODELVIEW);
}


//////////////////////////////////////////////////////////////////////////
//@intro	临时函数，用于画出三维模型的外围盒子
//  [4/15/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
void DrawBoundingBox(float minX,float maxX,float minY,float maxY,float minZ,float maxZ){
	glBegin(GL_LINES);
		glVertex3f(minX,minY,maxZ);		glVertex3f(maxX,minY,maxZ);
		glVertex3f(maxX,minY,maxZ);		glVertex3f(maxX,minY,minZ);
		glVertex3f(maxX,minY,minZ);		glVertex3f(minZ,minY,minZ);
		glVertex3f(minX,minY,minZ);		glVertex3f(minX,minY,maxZ);

		glVertex3f(minX,maxY,maxZ);		glVertex3f(maxX,maxY,maxZ);
		glVertex3f(maxX,maxY,maxZ);		glVertex3f(maxX,maxY,minZ);
		glVertex3f(maxX,maxY,minZ);		glVertex3f(minZ,maxY,minZ);
		glVertex3f(minZ,maxY,minZ);		glVertex3f(minX,maxY,maxZ);

		glVertex3f(minX,minY,maxZ);		glVertex3f(minX,maxY,maxZ);
		glVertex3f(minX,minY,minZ);		glVertex3f(minX,maxY,minZ);
		glVertex3f(maxX,minY,minZ);		glVertex3f(maxX,minY,minZ);
		glVertex3f(maxX,minY,maxZ);		glVertex3f(maxX,maxY,maxZ);
	glEnd();
		
}


//////////////////////////////////////////////////////////////////////////
//@intro	绘制控件
//  [4/15/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
void GLModelArea::paintGL(){
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslated(0,glmModel->mAvePt[1],-(glmModel->mMaxPt[2]-glmModel->mMinPt[2])*2);
	//glPushMatrix();
	//gluLookAt(0,0,3, 0,0,0, 0,1,0);

	//设置trackball的参数
	trackball.center = Point3f(glmModel->mAvePt[0],glmModel->mAvePt[1],glmModel->mAvePt[2]);
	float tempRadius = sqrt( (glmModel->mMaxPt[0] - glmModel->mAvePt[0]) * (glmModel->mMaxPt[0] - glmModel->mAvePt[0]) + 
		(glmModel->mMaxPt[1] - glmModel->mAvePt[1]) * (glmModel->mMaxPt[1] - glmModel->mAvePt[1]) + 
		(glmModel->mMaxPt[2] - glmModel->mAvePt[2]) * (glmModel->mMaxPt[2] - glmModel->mAvePt[2]) );
	trackball.radius = tempRadius / 2;
	trackball.GetView();
	trackball.Apply(false);
	if(paintNum < 6){	//用于分别获取三维模型4个方向的截图
		if(paintNum < 5){
			glClearColor(0.0f, 0.0f, 0.0f, 0);
		}else{
			glClearColor(1.0f, 1.0f, 1.0f, 0);
		}
		string imageName = GLMDirName(glmModel->mPathname);
		QPixmap pixmap;
		switch(paintNum){
		case 2:
			imageName += "image1.bmp";
			glRotatef(90,0,1,0);	//右视图
			glColor3f(1,1,1);
			GLMDraw(glmModel,GLM_SMOOTH|GLM_TEXTURE );	//绘制三维模型
			pixmap = QPixmap::grabWindow(QApplication::desktop()->winId(),mapToGlobal(pos()).x(),mapToGlobal(pos()).y(),frameGeometry().width()-10,frameGeometry().height()-10);
			pixmap.save(QString(QString::fromLocal8Bit(imageName.c_str())),"bmp");
			break;
		case 3:
			imageName += "image2.bmp";
			glRotatef(180,0,1,0);	//后视图
			glColor3f(1,1,1);
			GLMDraw(glmModel,GLM_SMOOTH|GLM_TEXTURE );	//绘制三维模型
			pixmap = QPixmap::grabWindow(QApplication::desktop()->winId(),mapToGlobal(pos()).x(),mapToGlobal(pos()).y(),frameGeometry().width()-10,frameGeometry().height()-10);
			pixmap.save(QString(QString::fromLocal8Bit(imageName.c_str())),"bmp");
			break;
		case 4:
			imageName += "image3.bmp";	
			glRotatef(270,0,1,0);	//前视图
			glColor3f(1,1,1);
			GLMDraw(glmModel,GLM_SMOOTH|GLM_TEXTURE );	//绘制三维模型
			pixmap = QPixmap::grabWindow(QApplication::desktop()->winId(),mapToGlobal(pos()).x(),mapToGlobal(pos()).y(),frameGeometry().width()-10,frameGeometry().height()-10);
			pixmap.save(QString(QString::fromLocal8Bit(imageName.c_str())),"bmp");
			break;
		case 5:
			imageName += "image4.bmp";	//正视图
			glColor3f(1,1,1);
			GLMDraw(glmModel,GLM_SMOOTH|GLM_TEXTURE );	//绘制三维模型
			pixmap = QPixmap::grabWindow(QApplication::desktop()->winId(),mapToGlobal(pos()).x(),mapToGlobal(pos()).y(),frameGeometry().width()-10,frameGeometry().height()-10);
			pixmap.save(QString(QString::fromLocal8Bit(imageName.c_str())),"bmp");
			break;
		default:
			glColor3f(1,1,1);
			GLMDraw(glmModel,GLM_SMOOTH|GLM_TEXTURE );	//绘制三维模型
			break;
		}
		
		paintNum += 1;
		
		
		
	}else{
		if(paintNum == 6){
			ComputeBestCuts();
			if(profileCenters.size() == 0){
				CutModelUniformly();
				
			}
			int centerSize = profileCenters.size();
			float bottom2TopLength = Normal(profileCenters[centerSize-1].GetX()-profileCenters[0].GetX(),
				profileCenters[centerSize-1].GetY()-profileCenters[0].GetY(),
				profileCenters[centerSize-1].GetZ()-profileCenters[0].GetZ()).GetLength();
			disThreshold = bottom2TopLength / ((profileCenters.size()-1) * 8);	//用于判断是否选取切分平面的临界值
			paintNum++;
		}

		glColor3f(1,1,1);
		GLMDraw(glmModel,GLM_SMOOTH|GLM_TEXTURE );	//绘制三维模型
		ShowPrincipalAxis(lowestVertexOnAxis,highestVertexOnAxis,glmModel->mAxisNormal);  //显示三维模型旋转对称轴
		ShowCoorSystem();	//显示空间直角坐标系
		ShowCuttingPlanes();
		ShowSelectedSliceBBox();
	}
}


//////////////////////////////////////////////////////////////////////////
//@intro	根据图片计算梯度
//@return	返回图片的梯度值
//  [4/15/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
vector<float> GLModelArea::ComputeGradients(){
	int scale  = 1;
	int delta = 0;
	int ddepth = CV_16S;
	cv::Mat src_gray, dst, grad_x;
	vector<cv::Mat>  src;
	char str[100];
	vector<float> gradient;
	for(int k = 1; k <= 4; k++){
		sprintf(str, "image%d.bmp",k);
		src.push_back(cv::imread(str));
		cv::cvtColor(src.at(k - 1), src_gray, CV_RGB2GRAY);

		cv::Sobel(src_gray, grad_x, ddepth, 1, 0 ,3, scale,delta,cv::BORDER_DEFAULT);
		cv::convertScaleAbs(grad_x, dst);

		if(gradient.size() == 0){
			gradient.resize(src.at(k-1).rows);
		}

		for(int i = 0; i < src.at(k-1).rows;i++){
			float sum = 0.0;
			uchar* p;
			p = dst.ptr<uchar>(i);
			for(int j = 0; j < src.at(k-1).cols; j++){
				sum += p[j];
			}
			gradient[i] += sum;
		}
	}
	string gradientFile = GLMDirName(glmModel->mPathname);
	gradientFile += "gradient.txt";
	std::ofstream ofs;
	ofs.open(gradientFile);
	for(auto i = 0; i < gradient.size();i++){
		ofs << gradient[i]<<endl;
	}
	return gradient;
}


//////////////////////////////////////////////////////////////////////////
//@intro	根据三维模型四个方向的截图计算最佳的的切分位置
//  [4/15/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
void GLModelArea::ComputeBestCuts(){
	vector<float> gradients= ComputeGradients();
	int startCol, endCol;
	int height = 0;
	for(int i = 0; i< gradients.size();i++){
		if(gradients[i] != 0){
			startCol = i;
			break;
		}
	}
	for(int j = gradients.size();j >0;j--){
		if(gradients[j-1] !=0 ){
			endCol = j;
			break;
		}
	}
	height = endCol - startCol;
	vector<float> smallGradients;
	vector<int> cutCols;
	for(int i = startCol; i < endCol; i++){
		if(gradients[i] <= 4100){
			smallGradients.push_back(i);
		}
	}
	cutCols.push_back(startCol);
	for(auto i = 1; i < smallGradients.size();i++){
		if(smallGradients[i] - smallGradients[i-1] != 1 && gradients.at(smallGradients[i]) == 4080){
			cutCols.push_back(smallGradients[i]);
		}
	}
	cutCols.push_back(endCol);

	Vertex lowestVertexOnAxis, highestVertexOnAxis;
	Normal axisNormal = glmModel->mAxisNormal;
	float minDis = 2000, maxDis = -2000;
	Vertex lowestVertex,highestVertex;
	vector<Vertex> vertices = glmModel->mVertices;
	for( int i = 0;i < vertices.size(); i++){
		Vertex tempVertex = vertices[i];
		float dotProduct = tempVertex.GetX() * axisNormal.GetX() + tempVertex.GetY() * axisNormal.GetY() + tempVertex.GetZ() * axisNormal.GetZ();
		if( dotProduct < minDis){
			minDis = dotProduct;
			lowestVertex = tempVertex;
		}
		else if(dotProduct > maxDis){
			maxDis = dotProduct;
			highestVertex = tempVertex;
		}
	}

	Normal low2top;
	ComputeLowestAndHighest(lowestVertex,highestVertex,axisNormal,lowestVertexOnAxis,highestVertexOnAxis,low2top);

	vector<Vertex> tempPoints;
	vector<float> tempRadii;
	for(int i = 0; i < cutCols.size();i++){
		int colsNum = cutCols[i] - startCol;
		float tempX = lowestVertexOnAxis.GetX() + low2top.GetX() * colsNum / (endCol - startCol);
		float tempY = lowestVertexOnAxis.GetY() + low2top.GetY() * colsNum / (endCol - startCol);
		float tempZ = lowestVertexOnAxis.GetZ() + low2top.GetZ() * colsNum / (endCol - startCol);
		Vertex tempP = Vertex(tempX,tempY, tempZ);
		tempPoints.push_back(tempP);
		tempRadii.push_back(CalIntersectRadius(tempP));
	}

	vector<SliceBBox> tempboxVector;
	for(int i = 0 ; i < tempPoints.size() -1;i++){
		SliceBBox box = SliceBBox(glmModel->mMinPt,glmModel->mMaxPt,tempPoints[i],tempPoints[i+1]);
		tempboxVector.push_back(box);
	}
	profileCenters = tempPoints;
	profileRadius = tempRadii;
	sliceBBoxes = tempboxVector;
}


//////////////////////////////////////////////////////////////////////////
//@intro	显示三维模型的切分平面
//////////////////////////////////////////////////////////////////////////
void GLModelArea::ShowCuttingPlanes(){
	glLineWidth(1);
	for(int i= 0;i < profileCenters.size();i++){
		if(i == selected_plane_index){
			DrawWiredCircleGreen(profileCenters[i].GetX(),profileCenters[i].GetY(),profileCenters[i].GetZ(),profileRadius[i]*1.1);
			//draw_solid_circle_green(profileCenters[i].getX(),profileCenters[i].getY(),profileCenters[i].getZ(),profileRadius[i]*1.1);
		}
		else{
			DrawWiredCircleRed(profileCenters[i].GetX(),profileCenters[i].GetY(),profileCenters[i].GetZ(),profileRadius[i]*1.1);
			//draw_solid_circle_red(profileCenters[i].getX(),profileCenters[i].getY(),profileCenters[i].getZ(),profileRadius[i]*1.1);
		}
	}
}


//////////////////////////////////////////////////////////////////////////
//@intro	读取MTL文件生成三维模型对应的纹理
//////////////////////////////////////////////////////////////////////////
void GLModelArea::ReadMtlFile(){
	FILE* file;
	char* filename;
	char buf[128];
	int materialNum = 0;
	char* map_file;
	
	//free(dir);
	char* dir = GLMDirName(glmModel->mPathname);
	char* mtlFileName = (char*)malloc(sizeof(char) * (strlen(dir) + strlen(glmModel->mMtllibname) + 1));
	strcpy(mtlFileName, dir);
	strcat(mtlFileName, glmModel->mMtllibname);
	file = fopen(mtlFileName, "r");
	//file = fopen(glmModel->mtllibname,"r");
	if(!file){
		fprintf(stderr,"readMTLfile() failed: can't open material file \"%s\".\n",filename);
	}
	//free(filename);

	while(fscanf(file,"%s",buf) != EOF){
		switch(buf[0]){
		case 'n':
			materialNum++;
			break;
		case 'm':
			fgets(buf,sizeof(buf),file);
			sscanf(buf,"%s %s",buf,buf);
			map_file = buf;
			glmModel->mMaterials[materialNum].SetMapfile(map_file);
			ReadBMPFile(dir, map_file,materialNum);
			break;
		default:
			fgets(buf,sizeof(buf),file);
			break;
		}
	}
}



//////////////////////////////////////////////////////////////////////////
//@intro	读取纹理BMP文件，生成模型纹理,放在这里是因为之前的位置出现了错误，无法正确生成纹理
//@param	strFileName:纹理图片名称
//@param	textureID:当前生成纹理的标识位
//////////////////////////////////////////////////////////////////////////
void GLModelArea::ReadBMPFile(char* dir, char* strFileName,int textureID){
	char* bmpFileName = (char*)malloc(sizeof(char) * (strlen(dir) + strlen(strFileName) + 1 ));
	strcpy(bmpFileName,dir);
	strcat(bmpFileName, strFileName);
	AUX_RGBImageRec *pBitmap = NULL;
	pBitmap = auxDIBImageLoadA(bmpFileName);
	if(!pBitmap){
		std::cout<<"Can't find the BMP file"<<endl;
		return;
	}

	if(pBitmap == NULL){
		std::cout<<"Can't read the BMP file: "<<bmpFileName<<endl;
		return;
	}
	if(glIsTexture(glmModel->mTextureArray[textureID])){
		glDeleteTextures(1,&glmModel->mTextureArray[textureID]);
	}
	glGenTextures(1,&glmModel->mTextureArray[textureID]);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);

	glBindTexture(GL_TEXTURE_2D,glmModel->mTextureArray[textureID]);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	gluBuild2DMipmaps(GL_TEXTURE_2D,3,pBitmap->sizeX,pBitmap->sizeY,GL_RGB,GL_UNSIGNED_BYTE,pBitmap->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	if(pBitmap){
		if(pBitmap->data){
			free(pBitmap->data);
		}
		free(pBitmap);
	}
	return;
}



//////////////////////////////////////////////////////////////////////////
//@intro	将Qt鼠标键盘事件转换为VCG库内的鼠标键盘事件
//////////////////////////////////////////////////////////////////////////
vcg::Trackball::Button QT2VCG(Qt::MouseButton qtbt, Qt::KeyboardModifiers modifiers){
	int vcgbt = vcg::Trackball::BUTTON_NONE;
	if(qtbt&Qt::LeftButton) vcgbt|= vcg::Trackball::BUTTON_LEFT;
	if(qtbt& Qt::RightButton) vcgbt |= vcg::Trackball::BUTTON_RIGHT;
	if(qtbt& Qt::MidButton) vcgbt|= vcg::Trackball::BUTTON_MIDDLE;
	if(modifiers& Qt::ShiftModifier) vcgbt |= vcg::Trackball::KEY_SHIFT;
	if(modifiers & Qt::ControlModifier) vcgbt|= vcg::Trackball::KEY_CTRL;
	if(modifiers & Qt::AltModifier) vcgbt |= vcg::Trackball::KEY_ALT;

	return vcg::Trackball::Button(vcgbt);
};

//////////////////////////////////////////////////////////////////////////
//@intro	设置滚动鼠标滚轮时的事件
//////////////////////////////////////////////////////////////////////////
void GLModelArea::wheelEvent(QWheelEvent *e){
	const int WHEEL_STEP = 120;
	double change_rate = 0.1;
	double change = (e->delta()<0)?(1+change_rate):(1-change_rate);
	double size_temp= 0.0;
	
	trackball.MouseWheel(e->delta()/float(WHEEL_STEP));
	updateGL();
}


//////////////////////////////////////////////////////////////////////////
//@intro	设置移动鼠标时的事件
//////////////////////////////////////////////////////////////////////////
void GLModelArea::mouseMoveEvent(QMouseEvent *e){
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);
	if(b_plane_selected && b_leftbutton_pressed){
		MovePlane(e->x(),height()-e->y());
	}
	else{
		trackball.MouseMove(e->x(),height()-e->y());
	}
	
	updateGL();
}



//////////////////////////////////////////////////////////////////////////
//@intro	设置按下鼠标时的事件
//////////////////////////////////////////////////////////////////////////
void GLModelArea::mousePressEvent(QMouseEvent *e){
	if(e->button()==Qt::LeftButton){
		//b_plane_selected = true;
		b_leftbutton_pressed = true;
		int selected_index = GetSelectedPlane(e->x(),height()-e->y());
		if(selected_index != -1){
			selected_plane_index = selected_index;
			b_plane_selected = true;
			selected_slice_index = -1;
		}
		else{
			selected_plane_index = selected_index;
			b_plane_selected = false;
			selected_slice_index = GetSlectedBBox(e->x(),height()-e->y());	
			trackball.MouseDown(e->x(),e->y(),QT2VCG(e->button(),e->modifiers()));
		}		
	}
	if(e->button() == Qt::RightButton){
		//b_plane_selected = false;
		//selected_plane_index = -1;
		int selected_index = GetSelectedPlane(e->x(),height()-e->y());
		if(selected_index != -1){
			selected_plane_index = selected_index;
			b_plane_selected = true;
		}
		else{
			selected_plane_index = selected_index;
			b_plane_selected = false;
		}		
	}
	//update();
	updateGL();
}

//////////////////////////////////////////////////////////////////////////
//@intro	设置鼠标释放时的事件
//////////////////////////////////////////////////////////////////////////
void GLModelArea::mouseReleaseEvent(QMouseEvent *e){
	if(e->button()== Qt::LeftButton){
		b_leftbutton_pressed = false;
		trackball.MouseUp(e->x(),height()-e->y(),QT2VCG(e->button(),e->modifiers()));
	}

	if(e->button() == Qt::RightButton){
		/*b_plane_selected = false;
		selected_plane_index = -1;*/
	}
}



//////////////////////////////////////////////////////////////////////////
//@intro	获取选中的横截面
//@param	mouse_x,mouse_y:当前鼠标的位置
//@return	选中横截面的索引
//////////////////////////////////////////////////////////////////////////
int GLModelArea::GetSelectedPlane(int mouse_x,int mouse_y){
	double world_x,world_y,world_z;
	GetWorldCoordinate(mouse_x,mouse_y,world_x,world_y,world_z);
	C3DCircleExtract testPlane;
	for(int i = 1; i < profileCenters.size()-1;i++){
		testPlane.setPlane(profileCenters[i],glmModel->mAxisNormal);
		double tempDistance = fabs(world_x * testPlane.abcd[0] + world_y * testPlane.abcd[1] + world_z * testPlane.abcd[2] + testPlane.abcd[3]);
		if(tempDistance < disThreshold){
			return i;
		}
	}
	return -1;
}


//////////////////////////////////////////////////////////////////////////
//@intro	获取选中的BBox
//@param	mouse_x,mosue_y:鼠标的位置
//@return	返回选中的bbox的索引
//////////////////////////////////////////////////////////////////////////
int GLModelArea::GetSlectedBBox(int mouse_x, int mouse_y){
	double world_x,world_y,world_z;
	GetWorldCoordinate(mouse_x,mouse_y,world_x,world_y,world_z);
	//判断当前点所在的bbox
	for(int i = 0; i< sliceBBoxes.size();i++){
		SliceBBox box = sliceBBoxes[i];
		//判断当前点是否位于bbox内
		if(box.isPointInBBox(world_x,world_y,world_z)){
			return i;
		}
	}
	return -1;
}


//////////////////////////////////////////////////////////////////////////
//@intro	移动选中的横截面
//@param	mouse_x,mouse_y:鼠标位置
//@return	void
//  [4/18/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
void GLModelArea::MovePlane(int mouse_x,int mouse_y){
	double world_x,world_y,world_z;
	GetWorldCoordinate(mouse_x,mouse_y,world_x,world_y,world_z);

	Normal axis_normal = glmModel->mAxisNormal;
	double dotProduct = world_x * axis_normal.GetX() + world_y * axis_normal.GetY() + world_z * axis_normal.GetZ();
	double length = axis_normal.GetLength();
	double axisLength = axis_normal.GetLength();
	double lengthUnit = dotProduct / sqrt(length);
	Vertex movedPoint = Vertex((float)lengthUnit * axis_normal.GetX(),(float)lengthUnit *axis_normal.GetY(),(float)lengthUnit *axis_normal.GetZ());
	Vertex upperPoint = profileCenters[selected_plane_index + 1];
	Vertex lowerPoint = profileCenters[selected_plane_index - 1];

	Normal normal2upper = Normal(upperPoint.GetX() - movedPoint.GetX(), upperPoint.GetY() - movedPoint.GetY(), upperPoint.GetZ() - movedPoint.GetZ());
	Normal normal2lower = Normal(lowerPoint.GetX() - movedPoint.GetX(), lowerPoint.GetY() - movedPoint.GetY(), lowerPoint.GetZ() - movedPoint.GetZ());

	if(axis_normal.DotProduct(normal2upper) > 0 && axis_normal.DotProduct(normal2lower) < 0){
		profileCenters[selected_plane_index] = movedPoint;
		profileRadius[selected_plane_index] = CalIntersectRadius(movedPoint);
		sliceBBoxes[selected_plane_index].setMin(movedPoint);
		sliceBBoxes[selected_plane_index-1].setMax(movedPoint);
	}
}


//////////////////////////////////////////////////////////////////////////
//@intro	键盘按压事件
//@param	e:键盘事件
//////////////////////////////////////////////////////////////////////////
void GLModelArea::keyPressEvent(QKeyEvent* e){
	if(e->key()== Qt::Key_Control)
		trackball.MouseDown(0,0,QT2VCG(Qt::NoButton,Qt::ControlModifier));
	if(e->key()== Qt::Key_Shift) 
		trackball.MouseDown(0,0,QT2VCG(Qt::NoButton,Qt::ShiftModifier));
	if(e->key()== Qt::Key_Alt)
		trackball.MouseDown(0,0,QT2VCG(Qt::NoButton,Qt::AltModifier));
}


//////////////////////////////////////////////////////////////////////////
//@intro	改变对称轴显示状态，如果已显示则修改为不显示，否则改为显示
//////////////////////////////////////////////////////////////////////////
void GLModelArea::ChangeAxisStatus(){
	if(b_show_principal_axis == true){
		b_show_principal_axis = false;
	}else{
		b_show_principal_axis = true;
	}
	updateGL();
}

//////////////////////////////////////////////////////////////////////////
//@intro	显示三维模型对称轴
//@param lowestPoint:三维模型上的点在对称轴上的投影最低点
//@param highestPoint:三维模型上的点在对称轴上的投影最高点
//@param axisNormal:三维模型对称轴方向向量
//////////////////////////////////////////////////////////////////////////
void GLModelArea::ShowPrincipalAxis(Vertex lowestPoint,Vertex highestPoint,Normal axisNormal){
	if(b_show_principal_axis== true){
		//设置线段类型为虚线
		glLineStipple(2, 0x5555);
		glEnable(GL_LINE_STIPPLE);

		//计算对称轴线段的起始点
		Point point1 = Point(lowestPoint.GetX()-axisNormal.GetX(), lowestPoint.GetY()-axisNormal.GetY(),lowestPoint.GetZ()-axisNormal.GetZ());
		Point point2 = Point(highestPoint.GetX() + axisNormal.GetX(),highestPoint.GetY() + axisNormal.GetY(), highestPoint.GetZ() + axisNormal.GetZ());

		glBegin(GL_LINES);
		//对称轴颜色设为黄色
			glColor3f(1,1,0);
			glVertex3f(point1.GetX(),point1.GetY() * 1.1, point1.GetZ());
			glVertex3f(point2.GetX(),point2.GetY() * 1.1,point2.GetZ());
		glEnd();
	}
}

//////////////////////////////////////////////////////////////////////////
//@intro	在屏幕上打印字符串
//@param	str：要打印的字符串
//////////////////////////////////////////////////////////////////////////
void drawString(const char* str){
	static int isFirstCall = 1;
	static GLuint lists;

	if(isFirstCall){
		isFirstCall = 0;
		lists = glGenLists(128);
		wglUseFontBitmaps(wglGetCurrentDC(),0,128,lists);

	}
	for(;*str != '\0';++str){
		glCallList(lists+*str);
	}
}

//////////////////////////////////////////////////////////////////////////
//@intro	修改空间直角坐标系的显示状态
//////////////////////////////////////////////////////////////////////////
void GLModelArea::ChangeCoorSystemStatus(){
	if(b_show_coord_system == false){
		b_show_coord_system = true;
	}else{
		b_show_coord_system = false;
	}
	updateGL();
}

//////////////////////////////////////////////////////////////////////////
//@intro	绘制空间直角坐标系
//////////////////////////////////////////////////////////////////////////
void GLModelArea::ShowCoorSystem(){
	if(b_show_coord_system == false)
		return;
	float scale = 0.7;
	//设置线段宽度为2
	glLineWidth(2);
	//禁用线段的虚线模式
	glDisable(GL_LINE_STIPPLE);
	//分别绘制x,y,z轴
	glBegin(GL_LINES);
		glColor3f(1,0,0);
		glVertex3f(0,0,0);
		glVertex3f(2*scale,0,0);

		glColor3f(0,1,0);
		glVertex3f(0,0,0);
		glVertex3f(0,2*scale,0);

		glColor3f(0,0,1);
		glVertex3f(0,0,0);
		glVertex3f(0,0,2*scale);
	glEnd();
	
	
	
	//x轴箭头,采用8个三角形拟合显示
	glColor3f(1, 0, 0);
	glBegin(GL_TRIANGLES);
	glVertex3f(1.85*scale, 0.03, 0.01); //第1个三角形
	glVertex3f(1.85*scale, 0.01, 0.03);
	glVertex3f(2*scale, 0, 0);

	glVertex3f( 1.85*scale, 0.01, 0.03); //第2*scale个三角形
	glVertex3f(1.85*scale, -0.01, 0.03);
	glVertex3f(2*scale, 0, 0);

	glVertex3f(1.85*scale, -0.01, 0.03); //第3个三角形
	glVertex3f(1.85*scale, -0.03, 0.01);
	glVertex3f(2*scale, 0, 0);

	glVertex3f(1.85*scale, -0.03,  0.01);  //第4个三角形
	glVertex3f(1.85*scale, -0.03, -0.01);
	glVertex3f(2*scale, 0, 0);

	glVertex3f(1.85*scale, -0.03, -0.01);  //第5个三角形
	glVertex3f(1.85*scale, -0.01, -0.03);
	glVertex3f(2*scale, 0, 0);

	glVertex3f(1.85*scale, -0.01, -0.03);  //第6个三角形
	glVertex3f(1.85*scale,  0.01, -0.03);
	glVertex3f(2*scale, 0, 0);

	glVertex3f(1.85*scale, 0.01, -0.03);  //第7个三角形
	glVertex3f(1.85*scale, 0.03, -0.01);
	glVertex3f(2*scale, 0, 0);

	glVertex3f(1.85*scale, 0.03, -0.01);  //第8个三角形
	glVertex3f(1.85*scale, 0.03,  0.01);
	glVertex3f(2*scale, 0, 0);
	glEnd();
	glRasterPos3f(2.05*scale,0,0);	//设置光标插入的位置
	drawString("X");

	//y轴箭头,采用8个三角形拟合显示
	glColor3f(0, 1, 0);
	glBegin(GL_TRIANGLES);
	glVertex3f( 0.03,1.85*scale, 0.01); //第1个三角形
	glVertex3f( 0.01,1.85*scale, 0.03);
	glVertex3f(0, 2*scale, 0);

	glVertex3f( 0.01, 1.85*scale, 0.03); //第2*scale个三角形
	glVertex3f( -0.01,1.85*scale, 0.03);
	glVertex3f(0, 2*scale, 0);

	glVertex3f( -0.01,1.85*scale, 0.03); //第3个三角形
	glVertex3f( -0.03,1.85*scale, 0.01);
	glVertex3f(0, 2*scale, 0);

	glVertex3f( -0.03,1.85*scale,  0.01);  //第4个三角形
	glVertex3f( -0.03,1.85*scale, -0.01);
	glVertex3f(0, 2*scale, 0);

	glVertex3f( -0.03,1.85*scale, -0.01);  //第5个三角形
	glVertex3f( -0.01,1.85*scale, -0.03);
	glVertex3f(0, 2*scale, 0);

	glVertex3f( -0.01,1.85*scale, -0.03);  //第6个三角形
	glVertex3f(  0.01,1.85*scale, -0.03);
	glVertex3f(0, 2*scale, 0);

	glVertex3f( 0.01,1.85*scale, -0.03);  //第7个三角形
	glVertex3f( 0.03,1.85*scale, -0.01);
	glVertex3f(0, 2*scale, 0);

	glVertex3f( 0.03,1.85*scale, -0.01);  //第8个三角形
	glVertex3f( 0.03,1.85*scale,  0.01);
	glVertex3f(0, 2*scale, 0);
	glEnd();
	glRasterPos3f(0,2.05*scale,0); //设置光标插入的位置
	drawString("Y");
	
	//z轴箭头,采用8个三角形拟合显示
	glColor3f(0,0,1);
	glBegin(GL_TRIANGLES);
	glVertex3f(0.03, 0.01, 1.85*scale); //第1个三角形
	glVertex3f(0.01, 0.03, 1.85*scale);
	glVertex3f(0,0,2*scale);

	glVertex3f( 0.01, 0.03, 1.85*scale); //第2*scale个三角形
	glVertex3f(-0.01, 0.03, 1.85*scale);
	glVertex3f(0,0,2*scale);

	glVertex3f(-0.01, 0.03, 1.85*scale); //第3个三角形
	glVertex3f(-0.03, 0.01, 1.85*scale);
	glVertex3f(0,0,2*scale);

	glVertex3f(-0.03,  0.01, 1.85*scale);  //第4个三角形
	glVertex3f(-0.03, -0.01, 1.85*scale);
	glVertex3f(0,0,2*scale);

	glVertex3f(-0.03, -0.01, 1.85*scale);  //第5个三角形
	glVertex3f(-0.01, -0.03, 1.85*scale);
	glVertex3f(0,0,2*scale);

	glVertex3f(-0.01, -0.03, 1.85*scale);  //第6个三角形
	glVertex3f( 0.01, -0.03, 1.85*scale);
	glVertex3f(0,0,2*scale);

	glVertex3f(0.01, -0.03, 1.85*scale);  //第7个三角形
	glVertex3f(0.03, -0.01, 1.85*scale);
	glVertex3f(0,0,2*scale);

	glVertex3f(0.03, -0.01, 1.85*scale);  //第8个三角形
	glVertex3f(0.03,  0.01, 1.85*scale);
	glVertex3f(0,0,2*scale);
	glEnd();
	glRasterPos3f(0,0,2.05*scale); //设置光标插入的位置
	drawString("Z");
}


//////////////////////////////////////////////////////////////////////////
//@intro	设置选中的slice的展开方式为拟合圆柱展开
//////////////////////////////////////////////////////////////////////////
void GLModelArea::ChangeEnrollMode2Cylinder(){
	if(sliceBBoxes[selected_slice_index].isCylinderMode())
		return;
	sliceBBoxes[selected_slice_index].setCylinderMode();
}


//////////////////////////////////////////////////////////////////////////
//@intro	设置选中的slice的展开方式为拟合圆台展开
//////////////////////////////////////////////////////////////////////////
void GLModelArea::ChangeEnrollMode2TruncatedCone(){
	if(sliceBBoxes[selected_slice_index].isTrunConeMode())
		return;
	sliceBBoxes[selected_slice_index].setTruncatedConeMode();
}


//////////////////////////////////////////////////////////////////////////
//@intro	在指定位置画红色的实心圆
//@param	x,y,z：圆心位置
//@param	radius:圆的半径
//////////////////////////////////////////////////////////////////////////
void GLModelArea::DrawSolidCircleRed(float x, float y,float z, float radius){
	int count;
	int sections = 200;

	float TWOPI = 2 * M_PI;
	glColor3f(1,0,0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x, y);
	for(count = 0; count <= sections; count++){
		glVertex3f(x + radius *cos(count * TWOPI / sections), y ,z+ radius * sin(count * TWOPI / sections));
	}
	glEnd();
}



//////////////////////////////////////////////////////////////////////////
//@intro	在指定位置画红色的空心圆
//@param	x,y,z:圆心位置
//@param	radius:圆心半径
//////////////////////////////////////////////////////////////////////////
void GLModelArea::DrawWiredCircleRed(float x, float y,float z, float radius){
	int count;
	int sections = 200;
	float TWOPI = 2 * M_PI;
	glColor3f(1,0,0);
	glBegin(GL_LINE_STRIP);
	for(count = 0;count <= sections; count++){
		glVertex3f(x + radius *cos(count * TWOPI / sections), y ,z+ radius * sin(count * TWOPI / sections));
	}
	glEnd();
}


//////////////////////////////////////////////////////////////////////////
//@intro	在指定位置画绿色的实心圆
//@param	x,y,z:圆心位置
//@param	radius:圆的半径
//////////////////////////////////////////////////////////////////////////
void GLModelArea::DrawSolidCircleGreen(float x, float y,float z, float radius){
	int count;
	int sections = 200;

	float TWOPI = 2 * M_PI;
	glColor3f(0,1,0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x, y);
	for(count = 0; count <= sections; count++){
		glVertex3f(x + radius *cos(count * TWOPI / sections), y ,z+ radius * sin(count * TWOPI / sections));
	}
	glEnd();
}


//////////////////////////////////////////////////////////////////////////
//@intro	在指定位置画绿色的空心圆
//@param	x,y,z:圆心的位置
//@param	radius:圆的半径
//////////////////////////////////////////////////////////////////////////
void GLModelArea::DrawWiredCircleGreen(float x, float y,float z, float radius){
	int count;
	int sections = 200;
	float TWOPI = 2 * M_PI;
	glColor3f(0,1,0);
	glBegin(GL_LINE_STRIP);
	for(count = 0;count <= sections; count++){
		glVertex3f(x + radius *cos(count * TWOPI / sections), y ,z+ radius * sin(count * TWOPI / sections));
	}
	glEnd();
}


//////////////////////////////////////////////////////////////////////////
//@intro	计算平面与线段的交点
//@param	planeNormal:平面法向量
//@param	planePoint:平面上的点
//@param	vertexA,vertexB:线段的两个端点
//@return	interVertex:交点坐标
//@return	返回判断平面与线段是否会相交
//////////////////////////////////////////////////////////////////////////
bool GLModelArea::PlaneLineSegmentIntersectPt(Normal planeNormal,Vertex planePoint,Vertex vertexA,Vertex vertexB,Vertex &interVertex){
	float linesegAB[3]; //A->B
	linesegAB[0] = vertexB.GetX() - vertexA.GetX();
	linesegAB[1] = vertexB.GetY() - vertexA.GetY();
	linesegAB[2] = vertexB.GetZ() - vertexA.GetZ();

	float linesegLength = sqrt(linesegAB[0] * linesegAB[0] + linesegAB[1] * linesegAB[1] + linesegAB[2] * linesegAB[2]);
	if(linesegLength < 1.0e-8){ //向量模为0
		return false;
	}
	linesegAB[0] /= linesegLength;
	linesegAB[1] /= linesegLength;
	linesegAB[2] /= linesegLength;
	float vpt = planeNormal.GetX() * linesegAB[0] + planeNormal.GetY() * linesegAB[1] + planeNormal.GetZ() * linesegAB[2]; //点积判断是否平行
	if(fabs(vpt) < 1.0e-8){ //线段与平面平行
		return false;
	}

	float t = ((planePoint.GetX() - vertexA.GetX()) * planeNormal.GetX() + (planePoint.GetY() - vertexA.GetY()) * planeNormal.GetY() + (planePoint.GetZ() - vertexA.GetZ()) * planeNormal.GetZ() ) / vpt;
	if( t < 0 || t > linesegLength){
		return false;
	}
	interVertex = Vertex(vertexA.GetX() + t * linesegAB[0], vertexA.GetY() + t * linesegAB[1], vertexA.GetZ() + t * linesegAB[2]);
	return true;
}

//////////////////////////////////////////////////////////////////////////
//判断三角面片与平面是否相交并求出交点
//@param	face:三角面片
//@param	abcd,planePoint:平面参数
//@param	interPoints:三角面片与平面的交点
//////////////////////////////////////////////////////////////////////////
bool GLModelArea::FaceIntersectPlane(Face face,float* abcd,VECTOR3 planePoint,vector<Vertex> &interPoints){
	Vertex vertexA = face.GetVertexA();
	Vertex vertexB = face.GetVertexB();
	Vertex vertexC = face.GetVertexC();
	Vertex interVertex1,interVertex2;

	Normal triNormal = Normal(vertexA,vertexB,vertexC);
	//三角面片与平面平行，则返回0
	if(IsParallel(VECTOR3(triNormal.GetX(),triNormal.GetY(),triNormal.GetZ()),VECTOR3(abcd[0],abcd[1],abcd[2]),1.0e-6)){
		return 0;
	}
	float dis[3];  //记录点到平面的距离
	dis[0] = abcd[0] * vertexA.GetX() + abcd[1] * vertexA.GetY() + abcd[2] * vertexA.GetZ() + abcd[3];
	dis[1] = abcd[0] * vertexB.GetX() + abcd[1] * vertexB.GetY() + abcd[2] * vertexB.GetZ() + abcd[3];
	dis[2] = abcd[0] * vertexC.GetX() + abcd[1] * vertexC.GetY() + abcd[2] * vertexC.GetZ() + abcd[3];

	float upper[3];
	upper[0] = IsVertexUpperPlane(vertexA,abcd,planePoint);
	upper[1] = IsVertexUpperPlane(vertexB,abcd,planePoint);
	upper[2] = IsVertexUpperPlane(vertexC,abcd,planePoint);

		//a,b,c都在平面的同一侧，则三角面片不会被分割   3个
	if( (upper[0] == -1 && upper[1] == -1 && upper[2] == -1) || (upper[0] == 1 && upper[1] == 1 && upper[2] == 1) ){
		return 0;
	}
	//三角面片的两个顶点都在平面上
	//顶点a,b在平面上
	else if( fabs(dis[0]) < 1.0e-8 && fabs(dis[1]) < 1.0e-8 ){
		//else if(dis[0] == 0 && dis[1] == 0){
		interPoints.push_back(vertexA);
		interPoints.push_back(vertexB);
		return 1;
	}	
	//顶点a,c在平面上
	else if( fabs(dis[0]) < 1.0e-8 && fabs(dis[2]) < 1.0e-8 ){
		//else if(dis[0] == 0 && dis[2] == 0){
		interPoints.push_back(vertexA);
		interPoints.push_back(vertexC);
		return 1;
	}
	//顶点b,c在平面上
	else if( fabs(dis[1]) < 1.0e-8 && fabs(dis[2]) < 1.0e-8 ){
		//else if(dis[1] == 0 && dis[2] == 0){
		interPoints.push_back(vertexB);
		interPoints.push_back(vertexC);
		return 1;
	}
	//三角面片的一个顶点在平面上，但是另外两个顶点都在平面的同侧，即三角面片不会被平面分割,返回值为1 6个
	//顶点a在平面上,b,c在同侧
	else if(fabs(dis[0]) < 1.0e-8 && upper[1] * upper[2] == 1 ){
		//else if(fabs(dis[0]) < 1.0e-8 && dis[1] * dis[2] > 0 ){
		interPoints.push_back(vertexA);
		//interPointTextures.push_back(textureA);
		return 1;
	}
	//顶点b在平面上,a,c在同侧

	else if(fabs(dis[1]) < 1.0e-8 && upper[0] * upper[2] == 1) {
		//else if( fabs(dis[1]) < 1.0e-8 && dis[0] * dis[2] > 0){
		interPoints.push_back(vertexB);
		return 1;
	}
	//顶点c在平面上,a,b在同侧
	else if(fabs(dis[2]) < 1.0e-8 && upper[0] * upper[1] == 1){
		//else if(fabs(dis[2]) < 1.0e-8 && dis[0] * dis[1] > 0){
		//else if(dis[2] == 0 && dis[0] * dis[1] > 0){
		interPoints.push_back(vertexC);
		return 1;
	}
	//三角面片的一个顶点在平面上，另外两个顶点在平面的两侧，此时需要将三角面片分割为2个小三角面片
	//顶点a在平面上，b,c分别在平面的两侧
	else if(fabs(dis[0]) < 1.0e-8 && upper[1] * upper[2] == -1 ){
		//else if(fabs(dis[0]) < 1.0e-8 && dis[1] * dis[2] < 0){
		//else if(dis[0] == 0 && dis[1] * dis[2] < 0 ){
		interPoints.push_back(vertexA);

		if(PlaneLineSegmentIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexB,vertexC,interVertex1)){
			interPoints.push_back(interVertex1);
		}
		return 2;
	}
	//顶点b在平面上，a,c分别在平面两侧
	else if(fabs(dis[1]) < 1.0e-8 && upper[0] * upper[2] == -1) {
		//else if(fabs(dis[1]) < 1.0e-8 && dis[0] * dis[2] < 0){
		//else if( dis[1] == 0 && dis[0] * dis[2] < 0 ){
		interPoints.push_back(vertexB);
		if(PlaneLineSegmentIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexA,vertexC,interVertex1)){
			interPoints.push_back(interVertex1);
		}
		return 2;
	}
	//顶点c在平面上，a,b分别在平面两侧
	else if(fabs(dis[2]) < 1.0e-8 && upper[0] * upper[1] == 1){
		//else if(fabs(dis[2]) < 1.0e-8 && dis[0] * dis[1] < 0){
		//else if(dis[2] == 0 && dis[0] * dis[1] < 0){
		interPoints.push_back(vertexC);
		if(PlaneLineSegmentIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexA,vertexB,interVertex1)){
			interPoints.push_back(interVertex1);
		}
		return 2;
	}




	//三角面片的三个顶点被分隔到平面的两侧，此时需要将三角面片分割为3个小的三角面片
	//a与b,c分别在平面的两侧
	else if( dis[0] * dis[1] < 0  && dis[0] * dis[2] < 0 && dis[1] * dis[2] > 0){
		//else if( (dis[0] < 0 && dis[1] > 0 && dis[2] > 0) || (dis[0] > 0 && dis[1] < 0 && dis[2] < 0) ){
		if(PlaneLineSegmentIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexA,vertexB,interVertex1)){
			interPoints.push_back(interVertex1);
		}
		if(PlaneLineSegmentIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexA,vertexC,interVertex2)){
			interPoints.push_back(interVertex2);
		}
		return 3;
	}
	//b与a,c分别在平面的两侧
	else if( dis[0] * dis[1] < 0 && dis[0] *dis[2] > 0 && dis[1] * dis[2] < 0){
		//else if( (dis[0] > 0 && dis[1] < 0 && dis[2] > 0) || (dis[0] < 0 && dis[1] > 0 && dis[2] < 0) ){
		if( PlaneLineSegmentIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexB,vertexA,interVertex1) ){
			interPoints.push_back(interVertex1);
		}
		if( PlaneLineSegmentIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexB,vertexC,interVertex2) ){
			interPoints.push_back(interVertex2);
		}
		return 3;
	}
	//c与a,b分别在平面的两侧
	else if(dis[0] * dis[1] > 0 &&  dis[0] * dis[2] < 0 && dis[1] * dis[2] < 0 ){
		//else if((dis[0] < 0 && dis[1] < 0 && dis[2] > 0) || (dis[0] > 0 && dis[1] > 0 && dis[2] < 0)){
		if( PlaneLineSegmentIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexC,vertexA, interVertex1) ){
			interPoints.push_back(interVertex1);
		}
		if( PlaneLineSegmentIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexC,vertexB,interVertex2) ){
			interPoints.push_back(interVertex2);
		}
		return 3;
	}

	else{
		return 0;
	}
	return 0;
}


//////////////////////////////////////////////////////////////////////////
//@intro	计算三角面片与平面的交点坐标
//@param	vertexA,vertexB,vertexC：三角面片的三个顶点
//@param	abcd:平面的参数
//@param	planePoint:平面上的点
//@return	interPoints:三角面片与平面的交点坐标
//@return	三角面片是否与平面相交
//////////////////////////////////////////////////////////////////////////
bool GLModelArea::FaceIntersectPlane(Vertex vertexA,Vertex vertexB,Vertex vertexC,float* abcd,VECTOR3 planePoint,vector<Vertex> &interPoints){
	Vertex interVertex1,interVertex2;

	Normal triNormal = Normal(vertexA,vertexB,vertexC);
	//三角面片与平面平行，则返回0
	if(IsParallel(VECTOR3(triNormal.GetX(),triNormal.GetY(),triNormal.GetZ()),VECTOR3(abcd[0],abcd[1],abcd[2]),1.0e-6)){
		return 0;
	}
	float dis[3];  //记录点到平面的距离
	dis[0] = abcd[0] * vertexA.GetX() + abcd[1] * vertexA.GetY() + abcd[2] * vertexA.GetZ() + abcd[3];
	dis[1] = abcd[0] * vertexB.GetX() + abcd[1] * vertexB.GetY() + abcd[2] * vertexB.GetZ() + abcd[3];
	dis[2] = abcd[0] * vertexC.GetX() + abcd[1] * vertexC.GetY() + abcd[2] * vertexC.GetZ() + abcd[3];

	float upper[3];
	upper[0] = IsVertexUpperPlane(vertexA,abcd,planePoint);
	upper[1] = IsVertexUpperPlane(vertexB,abcd,planePoint);
	upper[2] = IsVertexUpperPlane(vertexC,abcd,planePoint);

	//a,b,c都在平面的同一侧，则三角面片不会被分割   3个
	if( (upper[0] == -1 && upper[1] == -1 && upper[2] == -1) || (upper[0] == 1 && upper[1] == 1 && upper[2] == 1) ){
		return 0;
	}
	//三角面片的两个顶点都在平面上
	//顶点a,b在平面上
	else if( fabs(dis[0]) < 1.0e-8 && fabs(dis[1]) < 1.0e-8 ){
		//else if(dis[0] == 0 && dis[1] == 0){
		interPoints.push_back(vertexA);
		interPoints.push_back(vertexB);
		return 1;
	}	
	//顶点a,c在平面上
	else if( fabs(dis[0]) < 1.0e-8 && fabs(dis[2]) < 1.0e-8 ){
		//else if(dis[0] == 0 && dis[2] == 0){
		interPoints.push_back(vertexA);
		interPoints.push_back(vertexC);
		return 1;
	}
	//顶点b,c在平面上
	else if( fabs(dis[1]) < 1.0e-8 && fabs(dis[2]) < 1.0e-8 ){
		//else if(dis[1] == 0 && dis[2] == 0){
		interPoints.push_back(vertexB);
		interPoints.push_back(vertexC);
		return 1;
	}
	//三角面片的一个顶点在平面上，但是另外两个顶点都在平面的同侧，即三角面片不会被平面分割,返回值为1 6个
	//顶点a在平面上,b,c在同侧
	else if(fabs(dis[0]) < 1.0e-8 && upper[1] * upper[2] == 1 ){
		//else if(fabs(dis[0]) < 1.0e-8 && dis[1] * dis[2] > 0 ){
		interPoints.push_back(vertexA);
		//interPointTextures.push_back(textureA);
		return 1;
	}
	//顶点b在平面上,a,c在同侧

	else if(fabs(dis[1]) < 1.0e-8 && upper[0] * upper[2] == 1) {
		//else if( fabs(dis[1]) < 1.0e-8 && dis[0] * dis[2] > 0){
		interPoints.push_back(vertexB);
		return 1;
	}
	//顶点c在平面上,a,b在同侧
	else if(fabs(dis[2]) < 1.0e-8 && upper[0] * upper[1] == 1){
		//else if(fabs(dis[2]) < 1.0e-8 && dis[0] * dis[1] > 0){
		//else if(dis[2] == 0 && dis[0] * dis[1] > 0){
		interPoints.push_back(vertexC);
		return 1;
	}
	//三角面片的一个顶点在平面上，另外两个顶点在平面的两侧，此时需要将三角面片分割为2个小三角面片
	//顶点a在平面上，b,c分别在平面的两侧
	else if(fabs(dis[0]) < 1.0e-8 && upper[1] * upper[2] == -1 ){
		//else if(fabs(dis[0]) < 1.0e-8 && dis[1] * dis[2] < 0){
		//else if(dis[0] == 0 && dis[1] * dis[2] < 0 ){
		interPoints.push_back(vertexA);

		if(PlaneLineSegmentIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexB,vertexC,interVertex1)){
			interPoints.push_back(interVertex1);
		}
		return 2;
	}
	//顶点b在平面上，a,c分别在平面两侧
	else if(fabs(dis[1]) < 1.0e-8 && upper[0] * upper[2] == -1) {
		//else if(fabs(dis[1]) < 1.0e-8 && dis[0] * dis[2] < 0){
		//else if( dis[1] == 0 && dis[0] * dis[2] < 0 ){
		interPoints.push_back(vertexB);
		if(PlaneLineSegmentIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexA,vertexC,interVertex1)){
			interPoints.push_back(interVertex1);
		}
		return 2;
	}
	//顶点c在平面上，a,b分别在平面两侧
	else if(fabs(dis[2]) < 1.0e-8 && upper[0] * upper[1] == 1){
		//else if(fabs(dis[2]) < 1.0e-8 && dis[0] * dis[1] < 0){
		//else if(dis[2] == 0 && dis[0] * dis[1] < 0){
		interPoints.push_back(vertexC);
		if(PlaneLineSegmentIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexA,vertexB,interVertex1)){
			interPoints.push_back(interVertex1);
		}
		return 2;
	}




	//三角面片的三个顶点被分隔到平面的两侧，此时需要将三角面片分割为3个小的三角面片
	//a与b,c分别在平面的两侧
	else if( dis[0] * dis[1] < 0  && dis[0] * dis[2] < 0 && dis[1] * dis[2] > 0){
		//else if( (dis[0] < 0 && dis[1] > 0 && dis[2] > 0) || (dis[0] > 0 && dis[1] < 0 && dis[2] < 0) ){
		if(PlaneLineSegmentIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexA,vertexB,interVertex1)){
			interPoints.push_back(interVertex1);
		}
		if(PlaneLineSegmentIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexA,vertexC,interVertex2)){
			interPoints.push_back(interVertex2);
		}
		return 3;
	}
	//b与a,c分别在平面的两侧
	else if( dis[0] * dis[1] < 0 && dis[0] *dis[2] > 0 && dis[1] * dis[2] < 0){
		//else if( (dis[0] > 0 && dis[1] < 0 && dis[2] > 0) || (dis[0] < 0 && dis[1] > 0 && dis[2] < 0) ){
		if( PlaneLineSegmentIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexB,vertexA,interVertex1) ){
			interPoints.push_back(interVertex1);
		}
		if( PlaneLineSegmentIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexB,vertexC,interVertex2) ){
			interPoints.push_back(interVertex2);
		}
		return 3;
	}
	//c与a,b分别在平面的两侧
	else if(dis[0] * dis[1] > 0 &&  dis[0] * dis[2] < 0 && dis[1] * dis[2] < 0 ){
		//else if((dis[0] < 0 && dis[1] < 0 && dis[2] > 0) || (dis[0] > 0 && dis[1] > 0 && dis[2] < 0)){
		if( PlaneLineSegmentIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexC,vertexA, interVertex1) ){
			interPoints.push_back(interVertex1);
		}
		if( PlaneLineSegmentIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexC,vertexB,interVertex2) ){
			interPoints.push_back(interVertex2);
		}
		return 3;
	}

	else{
		return 0;
	}
	return 0;
}


//////////////////////////////////////////////////////////////////////////
//@intro	计算当前切分平面与三维模型的交点半径
//@param	当前切分平面上的点
//////////////////////////////////////////////////////////////////////////
float GLModelArea::CalIntersectRadius(Vertex planePoint){
	vector<Vertex> vertices = glmModel->mVertices;
	vector<Triangle> triangles = glmModel->mTriangles;
	vector<Vertex> lineSegment;
	vector<Vertex> line;
	C3DCircleExtract cuttingPlane;
	cuttingPlane.setPlane(planePoint,glmModel->mAxisNormal);

	for(int i =0;i<triangles.size();i++){
		Triangle tri = triangles[i];
		int* vindices = tri.GetVindices();
		Vertex vertexA = vertices.at(vindices[0]);
		Vertex vertexB = vertices.at(vindices[1]);
		Vertex vertexC = vertices.at(vindices[2]);
		if(FaceIntersectPlane(vertexA,vertexB,vertexC,cuttingPlane.abcd,cuttingPlane.center,line)>0){
			for(int j = 0;j < line.size();j++){
				lineSegment.push_back(line[j]);
			}
		}

	}
	return lineSegment_radius(planePoint,glmModel->mAxisNormal,lineSegment);
}


//////////////////////////////////////////////////////////////////////////
//@intro	将三维模型均匀切分为10份，即均匀生成11个切分平面
//////////////////////////////////////////////////////////////////////////
void GLModelArea::CutModelUniformly(){
	int intervalNum = 10;
	vector<Vertex> vertices = glmModel->mVertices;
	vector<Triangle> triangles = glmModel->mTriangles;
	float minDis = 2000, maxDis = -2000;
	Vertex lowestVertex,highestVertex;
	
	Normal axisNormal = glmModel->mAxisNormal;

	for( int i = 0;i < vertices.size(); i++){
		Vertex tempVertex = vertices[i];
		float dotProduct = tempVertex.GetX() * axisNormal.GetX() + tempVertex.GetY() * axisNormal.GetY() + tempVertex.GetZ() * axisNormal.GetZ();
		if( dotProduct < minDis){
			minDis = dotProduct;
			lowestVertex = tempVertex;
		}
		else if(dotProduct > maxDis){
			maxDis = dotProduct;
			highestVertex = tempVertex;
		}
	}
	
	Normal low2top;
	ComputeLowestAndHighest(lowestVertex,highestVertex,axisNormal,lowestVertexOnAxis,highestVertexOnAxis,low2top);

	for(int i = 0;i<=intervalNum;i++){
		float tempX = lowestVertexOnAxis.GetX() + low2top.GetX() * i / intervalNum;
		float tempY = lowestVertexOnAxis.GetY() + low2top.GetY() * i / intervalNum;
		float tempZ = lowestVertexOnAxis.GetZ() + low2top.GetZ() * i / intervalNum;

		Vertex planePoint = Vertex(tempX,tempY,tempZ);
		profileCenters.push_back(planePoint);
		profileRadius.push_back(CalIntersectRadius(planePoint));
	}
	for(int i = 0; i< intervalNum;i++){
		SliceBBox box = SliceBBox(glmModel->mMinPt,glmModel->mMaxPt,profileCenters[i],profileCenters[i+1]);
		sliceBBoxes.push_back(box);
	}
}


//////////////////////////////////////////////////////////////////////////
//@intro	分别计算两个顶点lowestVertex和highestVertex在对称轴axis上的投影坐标
//@param	lowestVertex：最低顶点
//@param	highestVertex:最高顶点
//@param	axis:对称轴
//@return	lowestPointOnAxis:最低顶点lowestVertex在对称轴上的投影坐标
//@return	highestPointOnAxis:最高顶点highestVertex在对称轴上的投影坐标
//@return	low2Top:最低投影点到最高投影点之间的向量
void GLModelArea::ComputeLowestAndHighest(Vertex lowestVertex,Vertex highestVertex,Normal axis,Vertex &lowestPointOnAxis,Vertex &highestPointOnAxis,Normal &low2Top){
	float lowestDotproduct = fabs(lowestVertex.GetX() * axis.GetX() + lowestVertex.GetY() * axis.GetY() + lowestVertex.GetZ() * axis.GetZ());
	float highestDotProduct = fabs(highestVertex.GetX() * axis.GetX() + highestVertex.GetY() * axis.GetY() + highestVertex.GetZ() * axis.GetZ());
	float lowestSquare = axis.GetX() * axis.GetX() + axis.GetY() * axis.GetY() + axis.GetZ() * axis.GetZ();
	float lowestUnit = sqrt(lowestDotproduct*lowestDotproduct / lowestSquare);
	Normal normal1 = Normal(lowestUnit*axis.GetX(), lowestUnit * axis.GetY(),lowestUnit * axis.GetZ());
	Normal normal2 = Normal(lowestUnit*axis.GetX()*(-1), lowestUnit * axis.GetY()*(-1),lowestUnit * axis.GetZ()*(-1));
	if(normal1.DotProduct(axis) < 0){
		lowestPointOnAxis = Vertex(normal1.GetX(),normal1.GetY(),normal1.GetZ());
	}
	else{
		lowestPointOnAxis = Vertex(normal2.GetX(),normal2.GetY(),normal2.GetZ());
	}
	float highestUnit = sqrt(highestDotProduct * highestDotProduct / lowestSquare);
	Normal normal3 = Normal(highestUnit * axis.GetX(),highestUnit * axis.GetY(),highestUnit * axis.GetZ());
	Normal normal4 = Normal(highestUnit * axis.GetX()*(-1),highestUnit * axis.GetY()*(-1),highestUnit * axis.GetZ()*(-1));
	if(normal3.DotProduct(axis) > 0){
		highestPointOnAxis = Vertex(normal3.GetX(),normal3.GetY(),normal3.GetZ());
	}
	else{
		highestPointOnAxis = Vertex(normal4.GetX(),normal4.GetY(),normal4.GetZ());
	}
	float px = highestPointOnAxis.GetX() - lowestPointOnAxis.GetX();
	float py = highestPointOnAxis.GetY() - lowestPointOnAxis.GetY();
	float pz = highestPointOnAxis.GetZ() - lowestPointOnAxis.GetZ();
	low2Top = Normal(px,py,pz);
}

//
////////////////////////////////////////////////////////////////////////////
////@intro	对三维模型进行切分
////@return	vector<GCSlice>：GCSlice数组
////////////////////////////////////////////////////////////////////////////
vector<GCSlice> GLModelArea::SplitModel(){
	Enroll enroll = Enroll(glmModel,profileCenters,profileRadius,sliceBBoxes);
	return enroll.CutModel();
}	


//////////////////////////////////////////////////////////////////////////
//@intro	画出选中的BBox
//////////////////////////////////////////////////////////////////////////
void GLModelArea::ShowSelectedSliceBBox(){
	if(selected_slice_index == -1)	//如果没有选中BBox，则直接返回
		return;

	//获取选中的BBox
	SliceBBox box = sliceBBoxes[selected_slice_index];
	float* minPt = box.getMinBounding();
	float* maxPt = box.getMaxBounding();

	float minX = minPt[0], minY = minPt[1], minZ = minPt[2];
	float maxX = maxPt[0], maxY = maxPt[1], maxZ = maxPt[2];

	//设置线段类型为虚线
	glLineStipple(2, 0x5555);
	glEnable(GL_LINE_STIPPLE);
	glLineWidth(2);

	glColor3f(0,1,0);
	//分别画出BBox的12条横线
	glBegin(GL_LINES);
		glVertex3f(minX,minY,maxZ);		glVertex3f(maxX,minY,maxZ);
		glVertex3f(maxX,minY,maxZ);		glVertex3f(maxX,minY,minZ);
		glVertex3f(maxX,minY,minZ);		glVertex3f(minZ,minY,minZ);
		glVertex3f(minX,minY,minZ);		glVertex3f(minX,minY,maxZ);

		glVertex3f(minX,maxY,maxZ);		glVertex3f(maxX,maxY,maxZ);
		glVertex3f(maxX,maxY,maxZ);		glVertex3f(maxX,maxY,minZ);
		glVertex3f(maxX,maxY,minZ);		glVertex3f(minZ,maxY,minZ);
		glVertex3f(minZ,maxY,minZ);		glVertex3f(minX,maxY,maxZ);

		glVertex3f(minX,minY,maxZ);		glVertex3f(minX,maxY,maxZ);
		glVertex3f(minX,minY,minZ);		glVertex3f(minX,maxY,minZ);
		glVertex3f(maxX,minY,minZ);		glVertex3f(maxX,minY,minZ);
		glVertex3f(maxX,minY,maxZ);		glVertex3f(maxX,maxY,maxZ);
	glEnd();
}



//************************************
// Method:    GetWorldCoordinate
// FullName:  GLModelArea::GetWorldCoordinate
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: int mouse_x
// Parameter: int mouse_y
// Parameter: double & world_x
// Parameter: double & world_y
// Parameter: double & world_z
//************************************
void GLModelArea::GetWorldCoordinate(int mouse_x, int mouse_y, double &world_x,double &world_y,double &world_z){
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