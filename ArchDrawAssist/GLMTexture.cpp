
#include "GLMTexture.h"

GLMTexture::GLMTexture(QWidget *parent)
{
  //  setCaption("The Opengl for Qt Framework");
  
}

//这是对虚函数，这里是重写该函数
void GLMTexture::initializeGL()
{
    setGeometry(300, 150, 500, 500);//设置窗口初始位置和大小
    loadTextures();
    glEnable(GL_TEXTURE_2D);//允许采用2D纹理技术
    glShadeModel(GL_SMOOTH);//设置阴影平滑模式
    glClearColor(0.0, 0.0, 0.0, 0);//改变窗口的背景颜色，不过我这里貌似设置后并没有什么效果
    glClearDepth(1.0);//设置深度缓存
    glEnable(GL_DEPTH_TEST);//允许深度测试
    glDepthFunc(GL_LEQUAL);//设置深度测试类型
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);//进行透视校正
}

void GLMTexture::paintGL()
{
    //glClear()函数在这里就是对initializeGL()函数中设置的颜色和缓存深度等起作用
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*下面开始画立方体,并对其进行纹理映射*/
    glLoadIdentity();
   // glRotatef(rotate_angle, -0.2f, 0.2f, -0.3f);
    glBegin(GL_QUADS);
    //上顶面
    glTexCoord2f(0.0, 1.0);//将2D的纹理坐标映射到3D的空间物体表面上
    glVertex3f(-0.3f, 0.3f, -0.3f);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-0.3f, 0.3f, 0.3f);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(0.3f, 0.3f, 0.3f);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(0.3f, 0.3f, -0.3f);
    //下顶面
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-0.3f, -0.3f, -0.3f);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-0.3f, -0.3f, 0.3f);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(0.3f, -0.3f, 0.3f);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(0.3f, -0.3f, -0.3f);
    //正前面
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-0.3f, 0.3f, 0.3f);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-0.3f, -0.3f, 0.3f);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(0.3f, -0.3f, 0.3f);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(0.3f, 0.3f, 0.3f);
    //右侧面
    glTexCoord2f(0.0, 1.0);
    glVertex3f(0.3f, 0.3f, 0.3f);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(0.3f, -0.3f, 0.3f);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(0.3f, -0.3f, -0.3f);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(0.3f, 0.3f, -0.3f);
    //背后面
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-0.3f, 0.3f, -0.3f);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(0.3f, 0.3f, -0.3f);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(0.3f, -0.3f, -0.3f);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(-0.3f, -0.3f, -0.3f);
    //左侧面
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-0.3f, 0.3f, -0.3f);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-0.3f, -0.3f, -0.3f);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(-0.3f, -0.3f, 0.3f);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(-0.3f, 0.3f, 0.3f);
//    rotate_angle -= 3;
    glEnd();
}

//该程序是设置opengl场景透视图，程序中至少被执行一次(程序启动时).
void GLMTexture::resizeGL(int width, int height)
{
    if(0 == height)
        height = 1;//防止一条边为0
    glViewport(0, 0, (GLint)width, (GLint)height);//重置当前视口，本身不是重置窗口的，只不过是这里被Qt给封装好了
    glMatrixMode(GL_PROJECTION);//选择投影矩阵
    glLoadIdentity();//重置选择好的投影矩阵
   // gluPerspective(45.0, (GLfloat)width/(GLfloat)height, 0.1, 100.0);//建立透视投影矩阵
    glMatrixMode(GL_MODELVIEW);//以下2句和上面出现的解释一样
    glLoadIdentity();

}
void GLMTexture::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
        //F1键为全屏和普通屏显示切换键
       /* case Qt::Key_F1:
            if(fullscreen)
                showFullScreen();
            else
            {
                setGeometry(300, 150, 500, 500);
                showNormal();
            }
            updateGL();
            break;*/
        //Ese为退出程序键
        case Qt::Key_Escape:
            close();
    }
}

/*装载纹理*/
void GLMTexture::loadTextures()
{
    QImage tex, buf;
    if(!buf.load("../opengl_nehe_05/cherry.jpg"))
    {
        qWarning("Cannot open the image...");
        QImage dummy(128, 128, QImage::Format_RGB32);//当没找到所需打开的图片时，创建一副128*128大小，深度为32位的位图
        dummy.fill(Qt::green);
        buf = dummy;
    }
    tex = convertToGLFormat(buf);//将Qt图片的格式buf转换成opengl的图片格式tex
    glGenTextures(1, &texture[0]);//开辟一个纹理内存，内存指向texture[0]
    glBindTexture(GL_TEXTURE_2D, texture[0]);//将创建的纹理内存指向的内容绑定到纹理对象GL_TEXTURE_2D上，经过这句代码后，以后对
                                            //GL_TEXTURE_2D的操作的任何操作都同时对应与它所绑定的纹理对象
    glTexImage2D(GL_TEXTURE_2D, 0, 3, tex.width(), tex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());//开始真正创建纹理数据
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//当所显示的纹理比加载进来的纹理小时，采用GL_LINEAR的方法来处理
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//当所显示的纹理比加载进来的纹理大时，采用GL_LINEAR的方法来处理
}

GLMTexture::~GLMTexture()
{
}