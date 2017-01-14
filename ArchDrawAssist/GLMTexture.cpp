
#include "GLMTexture.h"

GLMTexture::GLMTexture(QWidget *parent)
{
  //  setCaption("The Opengl for Qt Framework");
  
}

//���Ƕ��麯������������д�ú���
void GLMTexture::initializeGL()
{
    setGeometry(300, 150, 500, 500);//���ô��ڳ�ʼλ�úʹ�С
    loadTextures();
    glEnable(GL_TEXTURE_2D);//�������2D������
    glShadeModel(GL_SMOOTH);//������Ӱƽ��ģʽ
    glClearColor(0.0, 0.0, 0.0, 0);//�ı䴰�ڵı�����ɫ������������ò�����ú�û��ʲôЧ��
    glClearDepth(1.0);//������Ȼ���
    glEnable(GL_DEPTH_TEST);//������Ȳ���
    glDepthFunc(GL_LEQUAL);//������Ȳ�������
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);//����͸��У��
}

void GLMTexture::paintGL()
{
    //glClear()������������Ƕ�initializeGL()���������õ���ɫ�ͻ�����ȵ�������
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*���濪ʼ��������,�������������ӳ��*/
    glLoadIdentity();
   // glRotatef(rotate_angle, -0.2f, 0.2f, -0.3f);
    glBegin(GL_QUADS);
    //�϶���
    glTexCoord2f(0.0, 1.0);//��2D����������ӳ�䵽3D�Ŀռ����������
    glVertex3f(-0.3f, 0.3f, -0.3f);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-0.3f, 0.3f, 0.3f);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(0.3f, 0.3f, 0.3f);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(0.3f, 0.3f, -0.3f);
    //�¶���
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-0.3f, -0.3f, -0.3f);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-0.3f, -0.3f, 0.3f);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(0.3f, -0.3f, 0.3f);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(0.3f, -0.3f, -0.3f);
    //��ǰ��
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-0.3f, 0.3f, 0.3f);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-0.3f, -0.3f, 0.3f);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(0.3f, -0.3f, 0.3f);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(0.3f, 0.3f, 0.3f);
    //�Ҳ���
    glTexCoord2f(0.0, 1.0);
    glVertex3f(0.3f, 0.3f, 0.3f);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(0.3f, -0.3f, 0.3f);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(0.3f, -0.3f, -0.3f);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(0.3f, 0.3f, -0.3f);
    //������
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-0.3f, 0.3f, -0.3f);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(0.3f, 0.3f, -0.3f);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(0.3f, -0.3f, -0.3f);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(-0.3f, -0.3f, -0.3f);
    //�����
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

//�ó���������opengl����͸��ͼ�����������ٱ�ִ��һ��(��������ʱ).
void GLMTexture::resizeGL(int width, int height)
{
    if(0 == height)
        height = 1;//��ֹһ����Ϊ0
    glViewport(0, 0, (GLint)width, (GLint)height);//���õ�ǰ�ӿڣ����������ô��ڵģ�ֻ���������ﱻQt����װ����
    glMatrixMode(GL_PROJECTION);//ѡ��ͶӰ����
    glLoadIdentity();//����ѡ��õ�ͶӰ����
   // gluPerspective(45.0, (GLfloat)width/(GLfloat)height, 0.1, 100.0);//����͸��ͶӰ����
    glMatrixMode(GL_MODELVIEW);//����2���������ֵĽ���һ��
    glLoadIdentity();

}
void GLMTexture::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
        //F1��Ϊȫ������ͨ����ʾ�л���
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
        //EseΪ�˳������
        case Qt::Key_Escape:
            close();
    }
}

/*װ������*/
void GLMTexture::loadTextures()
{
    QImage tex, buf;
    if(!buf.load("../opengl_nehe_05/cherry.jpg"))
    {
        qWarning("Cannot open the image...");
        QImage dummy(128, 128, QImage::Format_RGB32);//��û�ҵ�����򿪵�ͼƬʱ������һ��128*128��С�����Ϊ32λ��λͼ
        dummy.fill(Qt::green);
        buf = dummy;
    }
    tex = convertToGLFormat(buf);//��QtͼƬ�ĸ�ʽbufת����opengl��ͼƬ��ʽtex
    glGenTextures(1, &texture[0]);//����һ�������ڴ棬�ڴ�ָ��texture[0]
    glBindTexture(GL_TEXTURE_2D, texture[0]);//�������������ڴ�ָ������ݰ󶨵��������GL_TEXTURE_2D�ϣ�������������Ժ��
                                            //GL_TEXTURE_2D�Ĳ������κβ�����ͬʱ��Ӧ�������󶨵��������
    glTexImage2D(GL_TEXTURE_2D, 0, 3, tex.width(), tex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());//��ʼ����������������
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//������ʾ������ȼ��ؽ���������Сʱ������GL_LINEAR�ķ���������
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//������ʾ������ȼ��ؽ����������ʱ������GL_LINEAR�ķ���������
}

GLMTexture::~GLMTexture()
{
}