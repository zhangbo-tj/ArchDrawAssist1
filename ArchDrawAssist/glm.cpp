/*    
 *  GLM library.  Wavefront .obj file format reader/writer/manipulator.
 *
 *  Written by Nate Robins, 1997.
 *  email: ndr@pobox.com
 *  www: http://www.pobox.com/~ndr
 */

/* includes */
#include <cmath>
using namespace std;

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "glm.h"
#include <fstream>
#include <vector>
#include <gl\gl.h>										
#include <gl\glu.h>					
#include <gl\GLAUX.H>
#include <crtdbg.h>
#include <atlconv.h>
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment( lib, "glaux.lib" )
#pragma comment( lib, "glu32.lib" ) 
#pragma comment( lib, "OpenGL32.lib" )
/* Some <math.h> files do not define M_PI... */

#include <iostream>
using namespace std;
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* defines */
#define T(x) model->triangles[(x)]

/* enums */
enum { X, Y, Z, W };      /* elements of a vertex X=0，Y=1，Z=2，W=3*/

/* global variables */
/*
GLubyte *_teximage_new;
static int _teximageWidth_new = 1, _teximageHeight_new = 1;
*/

#define MAX_TEXTURES 100
static GLuint textureArray[MAX_TEXTURES] = {0};

/* typedefs */

/* _GLMnode: general purpose node
 */
typedef struct _GLMnode {
  GLuint           index;
  GLboolean        averaged;
  struct _GLMnode* next;
} GLMnode;

/* strdup is actually not a standard ANSI C or POSIX routine
   so implement a private one.  OpenVMS does not have a strdup; Linux's
   standard libc doesn't declare strdup by default (unless BSD or SVID
   interfaces are requested). */
//复制一个string指向的字符串
  static char *
stralloc(const char *string)
{
  char *copy;

  copy = (char*) malloc(strlen(string) + 1);
  if (copy == NULL)
    return NULL;
  strcpy(copy, string);
  return copy;
}

/* private functions */

/* _glmMax: returns the maximum of two floats */
  static GLfloat
_glmMax(GLfloat a, GLfloat b) 
{
  if (a > b)
    return a;
  return b;
}

/* _glmAbs: returns the absolute value of a float */
  static GLfloat
_glmAbs(GLfloat f)
{
  if (f < 0)
    return -f;
  return f;
}

/* _glmDot: compute the dot product of two vectors
 *
 * u - array of 3 GLfloats (GLfloat u[3])
 * v - array of 3 GLfloats (GLfloat v[3])
 */
  static GLfloat
_glmDot(GLfloat* u, GLfloat* v)
{
  assert(u);
  assert(v);

  /* compute the dot product */
  return u[X] * v[X] + u[Y] * v[Y] + u[Z] * v[Z];
}

/* _glmCross: compute the cross product of two vectors
 *
 * u - array of 3 GLfloats (GLfloat u[3])
 * v - array of 3 GLfloats (GLfloat v[3])
 * n - array of 3 GLfloats (GLfloat n[3]) to return the cross product in
 */
  static GLvoid
_glmCross(GLfloat* u, GLfloat* v, GLfloat* n)
{
  assert(u);
  assert(v);
  assert(n);

  /* compute the cross product (u x v for right-handed [ccw]) */
  n[X] = u[Y] * v[Z] - u[Z] * v[Y];
  n[Y] = u[Z] * v[X] - u[X] * v[Z];
  n[Z] = u[X] * v[Y] - u[Y] * v[X];
}

/* _glmNormalize: normalize a vector
 *
 * n - array of 3 GLfloats (GLfloat n[3]) to be normalized
 */
  static GLvoid
_glmNormalize(GLfloat* n)
{
  GLfloat l;

  assert(n);

  /* normalize */
  l = (GLfloat)sqrt(n[X] * n[X] + n[Y] * n[Y] + n[Z] * n[Z]);
  n[0] /= l;
  n[1] /= l;
  n[2] /= l;
}


  /* _glmFindGroup: Find a group in the model
 */
  //找到名字为name的group
  GLMgroup*
_glmFindGroup(GLMmodel* model, char* name)
{
  GLMgroup* group;

  assert(model);

  group = model->groups;
  while(group) {
    if (!strcmp(name, group->name))
      break;
    group = group->next;
  }

  return group;
}
/* _glmAddGroup: Add a group to the model
 */
  GLMgroup*
_glmAddGroup(GLMmodel* model, char* name)  //group = _glmAddGroup(model, "default");
{
  GLMgroup* group;

  group = _glmFindGroup(model, name);   //在model里有'default'这个group，此行的group=default
  if (!group) {
    group = (GLMgroup*)malloc(sizeof(GLMgroup));
    group->name = stralloc(name);
    group->material = 0;
    group->numtriangles = 0;
    group->triangles = NULL;
    group->next = model->groups;//此行和下一行代码即为把group（default）插入到model groups
    model->groups = group;
    model->numgroups++;
  }

  return group;
}

/* _glmFindMaterial: Find a material in the model
 */
  GLuint
_glmFindMaterial(GLMmodel* model, char* name)
{
  GLuint i;

  for (i = 0; i < model->nummaterials; i++) {
    if (!strcmp(model->materials[i].name, name))  //比较两个字符串
      goto found;
  }

  /* didn't find the name, so set it as the default material */
  printf("_glmFindMaterial():  can't find material \"%s\".\n", name);
  i = 0;

found:
  return i;
}


/* _glmDirName: return the directory given a path
 *
 * path - filesystem path
 *
 * The return value should be free'd.
 */
  static char*
_glmDirName(char* path)
{
  char* dir;
  char* s;

  dir = stralloc(path); //分配内存

  s = strrchr(dir, '/'); //strrchr() 函数查找字符在指定字符串中从正面开始的最后一次出现的位置，
                         //如果成功，则返回从该位置到字符串结尾的所有字符，如果失败，则返回 false。
  if (s)
    s[1] = '\0';
  else
    dir[0] = '\0';
  return dir;
  
}
/* _glmReadBMP: read a BMP texture image file
 *
 * filename - filename of the BMP texture
 *
 * return 
 *    whether the BMP is read correctly
 */



  int _glmReadBMP(GLuint textureArray[], char* strFileName, int textureID)
{
	AUX_RGBImageRec *pBitmap = NULL;
	
	if(!strFileName){
		printf("Cannot find the BMP texture!!\n");     // 如果无此文件，则直接返回
		return 0;
	}
	printf("bmp file name:%s\n",strFileName);
	pBitmap = auxDIBImageLoadA(strFileName);				// 装入位图，并保存数据
	if(pBitmap == NULL)	{								// 如果装入位图失败，则退出
		printf("Cannot open the BMP texture\n");
		return 0;
	}
	
	glGenTextures(1, &textureArray[textureID]);//根据纹理参数返回n个纹理索引
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);// 设置像素对齐格式

	glBindTexture(GL_TEXTURE_2D, textureArray[textureID]); //绑定纹理
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pBitmap->sizeX , pBitmap->sizeY , GL_RGB, GL_UNSIGNED_BYTE, pBitmap->data);// 生成纹理
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	 printf("texture %d : %s is loaded\n", textureArray[textureID], strFileName); 
	if (pBitmap)										// 释放位图占用的资源
	{
		if (pBitmap->data)						
		{
			free(pBitmap->data);				
		}

		free(pBitmap);					
	}
	return 1;
}


/* _glmReadMTL: read a wavefront material library file
 *
 * model - properly initialized GLMmodel structure
 * name  - name of the material library
 */
static void _glmReadMTL(GLMmodel* model, char* name)
{
  FILE* file;
  char* dir;
  char* filename;
  char  buf[128];
  GLuint nummaterials, i;

  dir = _glmDirName(model->pathname);    //dir=bowl1.obj的路径
  filename = (char*)malloc(sizeof(char) * (strlen(dir) + strlen(name) + 1));
  strcpy(filename, dir);
  strcat(filename, name);
  free(dir);

  /* open the file */
  file = fopen(filename, "r"); //r 以只读方式打开文件.返回值：文件顺利打开后，指向该流的文件指针就会被返回。如果文件打开失败则返回NULL
  if (!file) {
    fprintf(stderr, "_glmReadMTL() failed: can't open material file \"%s\".\n",
        filename);
    exit(1);
  }
  free(filename);

  /* count the number of materials in the file */
  nummaterials = 1;
  while(fscanf(file, "%s", buf) != EOF) {  //从file文件中以‘s%格式’（读入一个字符串，遇空字符‘\0'结束）读取字符串，输出到buf,fscanf遇到空格和换行时结束
    switch(buf[0]) {
      case '#':        /* comment */
        /* eat up rest of line */
        fgets(buf, sizeof(buf), file); //char *fgets(char *buf, int bufsize, FILE *stream);从文件结构体指针stream中读取数据，每次读取一行。读取的数据保存在buf指向的字符数组中
        break;
      case 'n':        /* newmtl */
        fgets(buf, sizeof(buf), file);
        nummaterials++;
        sscanf(buf, "%s %s", buf, buf);
        break;
      default:
        /* eat up rest of line */
        fgets(buf, sizeof(buf), file);
        break;
    }
  }

  rewind(file);  //将文件内部的位置指针重新指向一个流的开头

  /* allocate memory for the materials */
  model->materials = (GLMmaterial*)malloc(sizeof(GLMmaterial) * nummaterials);
  model->nummaterials = nummaterials;

  /* set the default material */
  for (i = 0; i < nummaterials; i++) {
    model->materials[i].name = NULL;
    model->materials[i].shininess = 0;
    model->materials[i].diffuse[0] = 0.8;
    model->materials[i].diffuse[1] = 0.8;
    model->materials[i].diffuse[2] = 0.8;
    model->materials[i].diffuse[3] = 1.0;
    model->materials[i].ambient[0] = 0.2;
    model->materials[i].ambient[1] = 0.2;
    model->materials[i].ambient[2] = 0.2;
    model->materials[i].ambient[3] = 1.0;
    model->materials[i].specular[0] = 0.0;
    model->materials[i].specular[1] = 0.0;
    model->materials[i].specular[2] = 0.0;
    model->materials[i].specular[3] = 1.0;
    model->materials[i].map_file = NULL;
  }
  model->materials[0].name = stralloc("default");
  /*
   * glGenTextures(nummaterials, textureArray);
   */

  /* now, read in the data */
  nummaterials = 0;
  while(fscanf(file, "%s", buf) != EOF) {
    switch(buf[0]) {
      case '#':        /* comment */
        /* eat up rest of line */
        fgets(buf, sizeof(buf), file);
        break;
      case 'n':        /* newmtl */
        fgets(buf, sizeof(buf), file);
        sscanf(buf, "%s %s", buf, buf);
        nummaterials++;
        model->materials[nummaterials].name = stralloc(buf);
        break;
      case 'N':
        fscanf(file, "%f", &model->materials[nummaterials].shininess);
        /* wavefront shininess is from [0, 1000], so scale for OpenGL */
        model->materials[nummaterials].shininess /= 1000.0;
        model->materials[nummaterials].shininess *= 128.0;
        break;
      case 'm':        /* texture map */
        fgets(buf, sizeof(buf), file);
        sscanf(buf, "%s %s", buf, buf);
        model->materials[nummaterials].map_file = stralloc(buf);
		_glmReadBMP(textureArray, buf, nummaterials);
        break;
      case 'K':
        switch(buf[1]) {
          case 'd':
            fscanf(file, "%f %f %f",
                &model->materials[nummaterials].diffuse[0],
                &model->materials[nummaterials].diffuse[1],
                &model->materials[nummaterials].diffuse[2]);
            break;
          case 's':
            fscanf(file, "%f %f %f",
                &model->materials[nummaterials].specular[0],
                &model->materials[nummaterials].specular[1],
                &model->materials[nummaterials].specular[2]);
            break;
          case 'a':
            fscanf(file, "%f %f %f",
                &model->materials[nummaterials].ambient[0],
                &model->materials[nummaterials].ambient[1],
                &model->materials[nummaterials].ambient[2]);
            break;
          default:
            /* eat up rest of line */
            fgets(buf, sizeof(buf), file);
            break;
        }
        break;
      default:
        /* eat up rest of line */
        fgets(buf, sizeof(buf), file);
        break;
    }
  }
}

/* _glmWriteMTL: write a wavefront material library file
 *
 * model      - properly initialized GLMmodel structure
 * modelpath  - pathname of the model being written
 * mtllibname - name of the material library to be written
 */
  static GLvoid
_glmWriteMTL(GLMmodel* model, char* modelpath, char* mtllibname)
{
  FILE* file;
  char* dir;
  char* filename;
  GLMmaterial* material;
  GLuint i;

  dir = _glmDirName(modelpath);
  filename = (char*)malloc(sizeof(char) * (strlen(dir) + strlen(mtllibname)));
  strcpy(filename, dir);
  strcat(filename, mtllibname);
  free(dir);

  /* open the file */
  file = fopen(filename, "w");
  if (!file) {
    fprintf(stderr, "_glmWriteMTL() failed: can't open file \"%s\".\n",
        filename);
    exit(1);
  }
  free(filename);

  /* spit out a header */
  fprintf(file, "#  \n");
  fprintf(file, "#  Wavefront MTL generated by GLM library\n");
  fprintf(file, "#  \n");
  fprintf(file, "#  GLM library copyright (C) 1997 by Nate Robins\n");
  fprintf(file, "#  email: ndr@pobox.com\n");
  fprintf(file, "#  www:   http://www.pobox.com/~ndr\n");
  fprintf(file, "#  \n\n");

  for (i = 0; i < model->nummaterials; i++) {
    material = &model->materials[i];
    fprintf(file, "newmtl %s\n", material->name);
    fprintf(file, "Ka %f %f %f\n", 
        material->ambient[0], material->ambient[1], material->ambient[2]);
    fprintf(file, "Kd %f %f %f\n", 
        material->diffuse[0], material->diffuse[1], material->diffuse[2]);
    fprintf(file, "Ks %f %f %f\n", 
        material->specular[0],material->specular[1],material->specular[2]);
    fprintf(file, "Ns %f\n", material->shininess);
    fprintf(file, "\n");
  }
}


/* _glmFirstPass: first pass at a Wavefront OBJ file that gets all the
 * statistics of the model (such as #vertices, #normals, etc)
 *
 * model - properly initialized GLMmodel structure
 * file  - (fopen'd) file descriptor 
 */
  static GLvoid
_glmFirstPass(GLMmodel* model, FILE* file) 
{
  GLuint    numvertices;    /* number of vertices in model */
  GLuint    numnormals;      /* number of normals in model */
  GLuint    numtexcoords;    /* number of texcoords in model */
  GLuint    numtriangles;    /* number of triangles in model */
  GLMgroup* group;      /* current group */
  unsigned  v, n, t;
  char      buf[128];

  /* make a default group */
  group = _glmAddGroup(model, "default");


  numvertices = numnormals = numtexcoords = numtriangles = 0;
  while(fscanf(file, "%s", buf) != EOF) {//以%s格式取出file文件中的字符让buf存储,fscanf遇到空格和换行时结束
    switch(buf[0]) {
      case '#':        /* comment */
        /* eat up rest of line */
        fgets(buf, sizeof(buf), file);
        break;
      case 'v':        /* v, vn, vt */
        switch(buf[1]) {
          case '\0':      /* vertex */
            /* eat up rest of line */
            fgets(buf, sizeof(buf), file);
            numvertices++;
            break;
          case 'n':        /* normal */
            /* eat up rest of line */
            fgets(buf, sizeof(buf), file);
            numnormals++;
            break;
          case 't':        /* texcoord */
            /* eat up rest of line */
            fgets(buf, sizeof(buf), file);
            numtexcoords++;
            break;
          default:
            printf("_glmFirstPass(): Unknown token \"%s\".\n", buf);
            exit(1);
            break;
        }
        break;
      case 'm':
        fgets(buf, sizeof(buf), file);
        sscanf(buf, "%s %s", buf, buf);
        model->mtllibname = stralloc(buf);
        _glmReadMTL(model, buf);
        break;
      case 'u':
        /* eat up rest of line */
        fgets(buf, sizeof(buf), file);//从file中读取sizeof（buf）个字符，buf用来接收字符串，fgets遇到空格不结束
        break;
      case 'g':        /* group */
        /* eat up rest of line */
        fgets(buf, sizeof(buf), file);
        sscanf(buf, "%s", buf);
        group = _glmAddGroup(model, buf);
        break;
      case 'f':        /* face */
        v = n = t = 0;
        fscanf(file, "%s", buf);   //从file中读取字符串，送到buf中存储，遇到空格和换行都结束。所以只能读取1/1/1,即buf里为1/1/1
        /* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
        if (strstr(buf, "//")) { //搜索一个字符串在另一个字符串中的第一次出现
          /* v//n */
          sscanf(buf, "%d//%d", &v, &n);  //从字符串buf中读取第一个三角形的顶点
          fscanf(file, "%d//%d", &v, &n);  //从file中读取第二个三角形的顶点
          fscanf(file, "%d//%d", &v, &n);   //从file中读取第三个三角形的顶点
          numtriangles++;
          group->numtriangles++;
          while(fscanf(file, "%d//%d", &v, &n) > 0) { //fscanf()返回值：整型，成功返回读入的参数的个数，失败返回EOF(-1)。
            numtriangles++;
            group->numtriangles++;
          }
        } else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) {
          /* v/t/n */
          fscanf(file, "%d/%d/%d", &v, &t, &n);
          fscanf(file, "%d/%d/%d", &v, &t, &n);
          numtriangles++;
          group->numtriangles++;
          while(fscanf(file, "%d/%d/%d", &v, &t, &n) > 0) {
            numtriangles++;
            group->numtriangles++;
          }
        } else if (sscanf(buf, "%d/%d", &v, &t) == 2) {
          /* v/t */
          fscanf(file, "%d/%d", &v, &t);
          fscanf(file, "%d/%d", &v, &t);
          numtriangles++;
          group->numtriangles++;
          while(fscanf(file, "%d/%d", &v, &t) > 0) {
            numtriangles++;
            group->numtriangles++;
          }
        } else {
          /* v */
          fscanf(file, "%d", &v);
          fscanf(file, "%d", &v);
          numtriangles++;
          group->numtriangles++;
          while(fscanf(file, "%d", &v) > 0) {
            numtriangles++;
            group->numtriangles++;
          }
        }
        break;

      default:
        /* eat up rest of line */
        fgets(buf, sizeof(buf), file);
        break;
    }
  }

  /* announce the model statistics */
  printf(" Vertices: %d\n", numvertices);
  printf(" Normals: %d\n", numnormals);
  printf(" Texcoords: %d\n", numtexcoords);
  printf(" Triangles: %d\n", numtriangles);
  printf(" Groups: %d\n", model->numgroups);

  /* set the stats in the model structure */
  model->numvertices  = numvertices;
  model->numnormals   = numnormals;
  model->numtexcoords = numtexcoords;
  model->numtriangles = numtriangles;

  /* allocate memory for the triangles in each group */
  group = model->groups;
  while(group) {
    group->triangles = (GLuint*)malloc(sizeof(GLuint) * group->numtriangles);
    group->numtriangles = 0;
    group = group->next;
  }
}

/* _glmSecondPass: second pass at a Wavefront OBJ file that gets all
 * the data.
 *
 * model - properly initialized GLMmodel structure
 * file  - (fopen'd) file descriptor 
 */
  static GLvoid
_glmSecondPass(GLMmodel* model, FILE* file) 
{
  GLuint    numvertices;    /* number of vertices in model */
  GLuint    numnormals;      /* number of normals in model */
  GLuint    numtexcoords;    /* number of texcoords in model */
  GLuint    numtriangles;    /* number of triangles in model */
  GLfloat*  vertices;      /* array of vertices  */
  GLfloat*  normals;      /* array of normals */
  GLfloat*  texcoords;      /* array of texture coordinates */
  GLMgroup* group;      /* current group pointer */
  GLuint    material;      /* current material */
  GLuint    v, n, t;
  char      buf[128];

  /* set the pointer shortcuts */
  vertices     = model->vertices;
  normals      = model->normals;
  texcoords    = model->texcoords;
  group        = model->groups;

  /* on the second pass through the file, read all the data into the
     allocated arrays */
  numvertices = numnormals = numtexcoords = 1;
  numtriangles = 0;
  material = 0;
  while(fscanf(file, "%s", buf) != EOF) {
    switch(buf[0]) {
      case '#':        /* comment */
        /* eat up rest of line */
        fgets(buf, sizeof(buf), file);
        break;
      case 'v':        /* v, vn, vt */
        switch(buf[1]) {
          case '\0':      /* vertex */
            fscanf(file, "%f %f %f", 
                &vertices[3 * numvertices + X], 
                &vertices[3 * numvertices + Y], 
                &vertices[3 * numvertices + Z]);
            numvertices++;
            break;
          case 'n':        /* normal */
            fscanf(file, "%f %f %f", 
                &normals[3 * numnormals + X],
                &normals[3 * numnormals + Y], 
                &normals[3 * numnormals + Z]);
            numnormals++;
            break;
          case 't':        /* texcoord */
            fscanf(file, "%f %f", 
                &texcoords[2 * numtexcoords + X],
                &texcoords[2 * numtexcoords + Y]);
            numtexcoords++;
            break;
        }
        break;
      case 'u':
        fgets(buf, sizeof(buf), file);
        sscanf(buf, "%s %s", buf, buf);
        group->material = material = _glmFindMaterial(model, buf);
        break;
      case 'g':        /* group */
        /* eat up rest of line */
        fgets(buf, sizeof(buf), file);
        sscanf(buf, "%s", buf);
        group = _glmFindGroup(model, buf);
        group->material = material;
        break;
      case 'f':        /* face */
        v = n = t = 0;
        fscanf(file, "%s", buf);
        /* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
        if (strstr(buf, "//")) {
          /* v//n */
          sscanf(buf, "%d//%d", &v, &n);
          T(numtriangles).vindices[0] = v;
          T(numtriangles).nindices[0] = n;
          fscanf(file, "%d//%d", &v, &n);
          T(numtriangles).vindices[1] = v;
          T(numtriangles).nindices[1] = n;
          fscanf(file, "%d//%d", &v, &n);
          T(numtriangles).vindices[2] = v;
          T(numtriangles).nindices[2] = n;
          group->triangles[group->numtriangles++] = numtriangles;
          numtriangles++;
          while(fscanf(file, "%d//%d", &v, &n) > 0) {
            T(numtriangles).vindices[0] = T(numtriangles-1).vindices[0];
            T(numtriangles).nindices[0] = T(numtriangles-1).nindices[0];
            T(numtriangles).vindices[1] = T(numtriangles-1).vindices[2];
            T(numtriangles).nindices[1] = T(numtriangles-1).nindices[2];
            T(numtriangles).vindices[2] = v;
            T(numtriangles).nindices[2] = n;
            group->triangles[group->numtriangles++] = numtriangles;
            numtriangles++;
          }
        } else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) {
          /* v/t/n */
          T(numtriangles).vindices[0] = v;
          T(numtriangles).tindices[0] = t;
          T(numtriangles).nindices[0] = n;
          fscanf(file, "%d/%d/%d", &v, &t, &n);
          T(numtriangles).vindices[1] = v;
          T(numtriangles).tindices[1] = t;
          T(numtriangles).nindices[1] = n;
          fscanf(file, "%d/%d/%d", &v, &t, &n);
          T(numtriangles).vindices[2] = v;
          T(numtriangles).tindices[2] = t;
          T(numtriangles).nindices[2] = n;
          group->triangles[group->numtriangles++] = numtriangles;
          numtriangles++;
         while(fscanf(file, "%d/%d/%d", &v, &t, &n) > 0) {
            T(numtriangles).vindices[0] = T(numtriangles-1).vindices[0];
            T(numtriangles).tindices[0] = T(numtriangles-1).tindices[0];
            T(numtriangles).nindices[0] = T(numtriangles-1).nindices[0];
            T(numtriangles).vindices[1] = T(numtriangles-1).vindices[2];
            T(numtriangles).tindices[1] = T(numtriangles-1).tindices[2];
            T(numtriangles).nindices[1] = T(numtriangles-1).nindices[2];
            T(numtriangles).vindices[2] = v;
            T(numtriangles).tindices[2] = t;
            T(numtriangles).nindices[2] = n;
            group->triangles[group->numtriangles++] = numtriangles;
            numtriangles++;
          }
        } else if (sscanf(buf, "%d/%d", &v, &t) == 2) {
          /* v/t */
          T(numtriangles).vindices[0] = v;
          T(numtriangles).tindices[0] = t;
          fscanf(file, "%d/%d", &v, &t);
          T(numtriangles).vindices[1] = v;
          T(numtriangles).tindices[1] = t;
          fscanf(file, "%d/%d", &v, &t);
          T(numtriangles).vindices[2] = v;
          T(numtriangles).tindices[2] = t;
          group->triangles[group->numtriangles++] = numtriangles;
          numtriangles++;
          while(fscanf(file, "%d/%d", &v, &t) > 0) {
            T(numtriangles).vindices[0] = T(numtriangles-1).vindices[0];
            T(numtriangles).tindices[0] = T(numtriangles-1).tindices[0];
            T(numtriangles).vindices[1] = T(numtriangles-1).vindices[2];
            T(numtriangles).tindices[1] = T(numtriangles-1).tindices[2];
            T(numtriangles).vindices[2] = v;
            T(numtriangles).tindices[2] = t;
            group->triangles[group->numtriangles++] = numtriangles;
            numtriangles++;
          }
        } else {
          /* v */
          sscanf(buf, "%d", &v);
          T(numtriangles).vindices[0] = v;
          fscanf(file, "%d", &v);
          T(numtriangles).vindices[1] = v;
          fscanf(file, "%d", &v);
          T(numtriangles).vindices[2] = v;
          group->triangles[group->numtriangles++] = numtriangles;
          numtriangles++;
          while(fscanf(file, "%d", &v) > 0) {
            T(numtriangles).vindices[0] = T(numtriangles-1).vindices[0];
            T(numtriangles).vindices[1] = T(numtriangles-1).vindices[2];
            T(numtriangles).vindices[2] = v;
            group->triangles[group->numtriangles++] = numtriangles;
            numtriangles++;
          }
        }
        break;

      default:
        /* eat up rest of line */
        fgets(buf, sizeof(buf), file);
        break;
    }
  }


  /* announce the memory requirements */
  printf(" Memory: %d bytes\n",
      numvertices  * 3*sizeof(GLfloat) +
      numnormals   * 3*sizeof(GLfloat) * (numnormals ? 1 : 0) +
      numtexcoords * 3*sizeof(GLfloat) * (numtexcoords ? 1 : 0) +
      numtriangles * sizeof(GLMtriangle));

}




/* public functions */

/* glmUnitize: "unitize" a model by translating it to the origin and
 * scaling it to fit in a unit cube around the origin.  Returns the
 * scalefactor used.
 *
 * model - properly initialized GLMmodel structure 
 */
  GLfloat
glmUnitize(GLMmodel* model, GLfloat center[3])
{
  GLuint  i;
  GLfloat maxx, minx, maxy, miny, maxz, minz;
  GLfloat cx, cy, cz, w, h, d;
  GLfloat scale;

  assert(model);
  assert(model->vertices);

  /* get the max/mins */
  maxx = minx = model->vertices[3 + X];
  maxy = miny = model->vertices[3 + Y];
  maxz = minz = model->vertices[3 + Z];

  for (i = 1; i <= model->numvertices; i++) {
    if (maxx < model->vertices[3 * i + X])
      maxx = model->vertices[3 * i + X];
    if (minx > model->vertices[3 * i + X])
      minx = model->vertices[3 * i + X];

    if (maxy < model->vertices[3 * i + Y])
      maxy = model->vertices[3 * i + Y];
    if (miny > model->vertices[3 * i + Y])
      miny = model->vertices[3 * i + Y];

    if (maxz < model->vertices[3 * i + Z])
      maxz = model->vertices[3 * i + Z];
    if (minz > model->vertices[3 * i + Z])
      minz = model->vertices[3 * i + Z];
  }
  /* calculate model width, height, and depth */
  w = _glmAbs(maxx) + _glmAbs(minx);
  h = _glmAbs(maxy) + _glmAbs(miny);
  d = _glmAbs(maxz) + _glmAbs(minz);

  /* calculate center of the model */
  cx = (maxx + minx) / 2.0;
  cy = (maxy + miny) / 2.0;
  cz = (maxz + minz) / 2.0;

  /* calculate unitizing scale factor */
  scale = 2.0 / _glmMax(_glmMax(w, h), d);
  /* translate around center then scale */
  for (i = 1; i <= model->numvertices; i++) {
    model->vertices[3 * i + X] -= cx;
    model->vertices[3 * i + Y] -= cy;
    model->vertices[3 * i + Z] -= cz;
    model->vertices[3 * i + X] *= scale;
    model->vertices[3 * i + Y] *= scale;
    model->vertices[3 * i + Z] *= scale;
  }

  center[0] = cx;
  center[1] = cy;
  center[2] = cz;
  return scale;
}


/* glmFacetNormals: Generates facet normals for a model (by taking the
 * cross product of the two vectors derived from the sides of each
 * triangle).  Assumes a counter-clockwise winding.
 *
 * model - initialized GLMmodel structure
 */
  //计算模型面的法向
  GLvoid
glmFacetNormals(GLMmodel* model)
{
  GLuint  i;
  GLfloat u[3];
  GLfloat v[3];

  assert(model);
  assert(model->vertices);

  /* clobber any old facetnormals */
  if (model->facetnorms)
    free(model->facetnorms);

  /* allocate memory for the new facet normals */
  model->numfacetnorms = model->numtriangles;
  model->facetnorms = (GLfloat*)malloc(sizeof(GLfloat) *
      3 * (model->numfacetnorms + 1));

  for (i = 0; i < model->numtriangles; i++) {
    model->triangles[i].findex = i+1;

    u[X] = model->vertices[3 * T(i).vindices[1] + X] -
      model->vertices[3 * T(i).vindices[0] + X];
    u[Y] = model->vertices[3 * T(i).vindices[1] + Y] -
      model->vertices[3 * T(i).vindices[0] + Y];
    u[Z] = model->vertices[3 * T(i).vindices[1] + Z] -
      model->vertices[3 * T(i).vindices[0] + Z];

    v[X] = model->vertices[3 * T(i).vindices[2] + X] -
      model->vertices[3 * T(i).vindices[0] + X];
    v[Y] = model->vertices[3 * T(i).vindices[2] + Y] -
      model->vertices[3 * T(i).vindices[0] + Y];
    v[Z] = model->vertices[3 * T(i).vindices[2] + Z] -
      model->vertices[3 * T(i).vindices[0] + Z];

    _glmCross(u, v, &model->facetnorms[3 * (i+1)]);
    _glmNormalize(&model->facetnorms[3 * (i+1)]);
  }
}


/* glmDelete: Deletes a GLMmodel structure.
 *
 * model - initialized GLMmodel structure
 */
  GLvoid
glmDelete(GLMmodel* model)
{
  GLMgroup* group;
  GLuint i;

  assert(model);

  if (model->pathname)   free(model->pathname);
  if (model->mtllibname) free(model->mtllibname);
  if (model->vertices)   free(model->vertices);
  if (model->normals)    free(model->normals);
  if (model->texcoords)  free(model->texcoords);
  if (model->facetnorms) free(model->facetnorms);
  if (model->triangles)  free(model->triangles);
  if (model->materials) {
    for (i = 0; i < model->nummaterials; i++)
      free(model->materials[i].name);
  }
  free(model->materials);
  while(model->groups) {
    group = model->groups;
    model->groups = model->groups->next;
    free(group->name);
    free(group->triangles);
    free(group);
  }

  free(model);
}

/* glmReadOBJ: Reads a model description from a Wavefront .OBJ file.
 * Returns a pointer to the created object which should be free'd with
 * glmDelete().
 *
 * filename - name of the file containing the Wavefront .OBJ format data.   obj格式的文件  
 */
  GLMmodel* 
glmReadOBJ(char* filename)   //filename=bowl1.obj
{
  GLMmodel* model;
  FILE*     file;

  /* open the file */
  file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "glmReadOBJ() failed: can't open data file \"%s\".\n",
        filename);
    exit(1);
  }

#if 0
  /* announce the model name */
  printf("Model: %s\n", filename);
#endif

  /* allocate a new model */
  model = (GLMmodel*)malloc(sizeof(GLMmodel));
  model->pathname      = stralloc(filename);    // model->pathname =C:\Users\Guosheng\Desktop\OpenGLDemo\Model\model\bowl1.obj
  model->mtllibname    = NULL;
  model->numvertices   = 0;
  model->vertices      = NULL;
  model->numnormals    = 0;
  model->normals       = NULL;
  model->numtexcoords  = 0;
  model->texcoords     = NULL;
  model->numfacetnorms = 0;
  model->facetnorms    = NULL;
  model->numtriangles  = 0;
  model->triangles     = NULL;
  model->nummaterials  = 0;
  model->materials     = NULL;
  model->numgroups     = 0;
  model->groups        = NULL;
  model->position[0]   = 0.0;
  model->position[1]   = 0.0;
  model->position[2]   = 0.0;

  /* make a first pass through the file to get a count of the number
     of vertices, normals, texcoords & triangles */
  _glmFirstPass(model, file);

  /* allocate memory */
  model->vertices = (GLfloat*)malloc(sizeof(GLfloat) *
      3 * (model->numvertices + 1));
  model->triangles = (GLMtriangle*)malloc(sizeof(GLMtriangle) *
      model->numtriangles);
  if (model->numnormals) {
    model->normals = (GLfloat*)malloc(sizeof(GLfloat) *
        3 * (model->numnormals + 1));
  }
  if (model->numtexcoords) {
    model->texcoords = (GLfloat*)malloc(sizeof(GLfloat) *
        2 * (model->numtexcoords + 1));
  }

  /* rewind to beginning of file and read in the data this pass */
  rewind(file);

  _glmSecondPass(model, file);

  /* close the file */
  fclose(file);

  return model;
}

/* glmWriteOBJ: Writes a model description in Wavefront .OBJ format to
 * a file.
 *
 * model    - initialized GLMmodel structure
 * filename - name of the file to write the Wavefront .OBJ format data to
 * mode     - a bitwise or of values describing what is written to the file
 *            GLM_NONE     -  render with only vertices
 *            GLM_FLAT     -  render with facet normals
 *            GLM_SMOOTH   -  render with vertex normals
 *            GLM_TEXTURE  -  render with texture coords
 *            GLM_COLOR    -  render with colors (color material)
 *            GLM_MATERIAL -  render with materials
 *            GLM_COLOR and GLM_MATERIAL should not both be specified.  
 *            GLM_FLAT and GLM_SMOOTH should not both be specified.  
 */
  GLvoid
glmWriteOBJ(GLMmodel* model, char* filename, GLuint mode)
{
  GLuint    i;
  FILE*     file;
  GLMgroup* group;

  assert(model);

  /* do a bit of warning */
  if (mode & GLM_FLAT && !model->facetnorms) {
    printf("glmWriteOBJ() warning: flat normal output requested "
        "with no facet normals defined.\n");
    mode &= ~GLM_FLAT;
  }
  if (mode & GLM_SMOOTH && !model->normals) {
    printf("glmWriteOBJ() warning: smooth normal output requested "
        "with no normals defined.\n");
    mode &= ~GLM_SMOOTH;
  }
  if (mode & GLM_TEXTURE && !model->texcoords) {
    printf("glmWriteOBJ() warning: texture coordinate output requested "
        "with no texture coordinates defined.\n");
    mode &= ~GLM_TEXTURE;
  }
  if (mode & GLM_FLAT && mode & GLM_SMOOTH) {
    printf("glmWriteOBJ() warning: flat normal output requested "
        "and smooth normal output requested (using smooth).\n");
    mode &= ~GLM_FLAT;
  }

  /* open the file */
  file = fopen(filename, "w");
  if (!file) {
    fprintf(stderr, "glmWriteOBJ() failed: can't open file \"%s\" to write.\n",
        filename);
    exit(1);
  }

  /* spit out a header */
  fprintf(file, "#  \n");
  fprintf(file, "#  Wavefront OBJ generated by GLM library\n");
  fprintf(file, "#  \n");
  fprintf(file, "#  GLM library copyright (C) 1997 by Nate Robins\n");
  fprintf(file, "#  email: ndr@pobox.com\n");
  fprintf(file, "#  www:   http://www.pobox.com/~ndr\n");
  fprintf(file, "#  \n");

  if (mode & GLM_MATERIAL && model->mtllibname) {
    fprintf(file, "\nmtllib %s\n\n", model->mtllibname);
    _glmWriteMTL(model, filename, model->mtllibname);
  }

  /* spit out the vertices */
  fprintf(file, "\n");
  fprintf(file, "# %d vertices\n", model->numvertices);
  for (i = 1; i <= model->numvertices; i++) {
    fprintf(file, "v %f %f %f\n", 
        model->vertices[3 * i + 0],
        model->vertices[3 * i + 1],
        model->vertices[3 * i + 2]);
  }

  /* spit out the smooth/flat normals */
  if (mode & GLM_SMOOTH) {
    fprintf(file, "\n");
    fprintf(file, "# %d normals\n", model->numnormals);
    for (i = 1; i <= model->numnormals; i++) {
      fprintf(file, "vn %f %f %f\n", 
          model->normals[3 * i + 0],
          model->normals[3 * i + 1],
          model->normals[3 * i + 2]);
    }
  } else if (mode & GLM_FLAT) {
    fprintf(file, "\n");
    fprintf(file, "# %d normals\n", model->numfacetnorms);
    for (i = 1; i <= model->numnormals; i++) {
      fprintf(file, "vn %f %f %f\n", 
          model->facetnorms[3 * i + 0],
          model->facetnorms[3 * i + 1],
          model->facetnorms[3 * i + 2]);
    }
  }

  /* spit out the texture coordinates */
  if (mode & GLM_TEXTURE) {
    fprintf(file, "\n");
    fprintf(file, "# %d texcoords\n", model->numtexcoords); // ypchui@20070522: texcoords -> numtexcoords
    for (i = 1; i <= model->numtexcoords; i++) {
      fprintf(file, "vt %f %f\n", 
          model->texcoords[2 * i + 0],
          model->texcoords[2 * i + 1]);
    }
  }

  fprintf(file, "\n");
  fprintf(file, "# %d groups\n", model->numgroups);
  fprintf(file, "# %d faces (triangles)\n", model->numtriangles);
  fprintf(file, "\n");

  group = model->groups;
  while(group) {
    fprintf(file, "g %s\n", group->name);
    if (mode & GLM_MATERIAL)
      fprintf(file, "usemtl %s\n", model->materials[group->material].name);
    for (i = 0; i < group->numtriangles; i++) {
      if (mode & GLM_SMOOTH && mode & GLM_TEXTURE) {
        fprintf(file, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
            T(group->triangles[i]).vindices[0], 
           T(group->triangles[i]).tindices[0], 
		   T(group->triangles[i]).nindices[0], 
            
            T(group->triangles[i]).vindices[1],
            T(group->triangles[i]).tindices[1],
			T(group->triangles[i]).nindices[1],
            
            T(group->triangles[i]).vindices[2],
            T(group->triangles[i]).tindices[2],
			T(group->triangles[i]).nindices[2]
            );
      } else if (mode & GLM_FLAT && mode & GLM_TEXTURE) {
        fprintf(file, "f %d/%d %d/%d %d/%d\n",
            T(group->triangles[i]).vindices[0],
            T(group->triangles[i]).findex,
            T(group->triangles[i]).vindices[1],
            T(group->triangles[i]).findex,
            T(group->triangles[i]).vindices[2],
            T(group->triangles[i]).findex);
      } else if (mode & GLM_TEXTURE) {
        fprintf(file, "f %d/%d %d/%d %d/%d\n",
            T(group->triangles[i]).vindices[0],
            T(group->triangles[i]).tindices[0],
            T(group->triangles[i]).vindices[1],
            T(group->triangles[i]).tindices[1],
            T(group->triangles[i]).vindices[2],
            T(group->triangles[i]).tindices[2]);
      } else if (mode & GLM_SMOOTH) {
        fprintf(file, "f %d//%d %d//%d %d//%d\n",
            T(group->triangles[i]).vindices[0],
            T(group->triangles[i]).nindices[0],
            T(group->triangles[i]).vindices[1],
            T(group->triangles[i]).nindices[1],
            T(group->triangles[i]).vindices[2], 
            T(group->triangles[i]).nindices[2]);
      } else if (mode & GLM_FLAT) {
        fprintf(file, "f %d//%d %d//%d %d//%d\n",
            T(group->triangles[i]).vindices[0], 
            T(group->triangles[i]).findex,
            T(group->triangles[i]).vindices[1],
            T(group->triangles[i]).findex,
            T(group->triangles[i]).vindices[2],
            T(group->triangles[i]).findex);
      } else {
        fprintf(file, "f %d %d %d\n",
            T(group->triangles[i]).vindices[0],
            T(group->triangles[i]).vindices[1],
            T(group->triangles[i]).vindices[2]);
      }
    }
    fprintf(file, "\n");
    group = group->next;
  }

  fclose(file);
}

/* glmDraw: Renders the model to the current OpenGL context using the
 * mode specified.
 *
 * model    - initialized GLMmodel structure
 * mode     - a bitwise OR of values describing what is to be rendered.
 *            GLM_NONE     -  render with only vertices
 *            GLM_FLAT     -  render with facet normals
 *            GLM_SMOOTH   -  render with vertex normals
 *            GLM_TEXTURE  -  render with texture coords
 *            GLM_COLOR    -  render with colors (color material)
 *            GLM_MATERIAL -  render with materials
 *            GLM_COLOR and GLM_MATERIAL should not both be specified.  
 *            GLM_FLAT and GLM_SMOOTH should not both be specified.  
 */
  GLvoid
glmDraw(GLMmodel* model, GLuint mode)
{
  GLuint i;
  GLuint j;
  GLMgroup* group;
   
  assert(model);
  assert(model->vertices);
 
  /* do a bit of warning */
  if (mode & GLM_FLAT && !model->facetnorms) {
    printf("glmDraw() warning: flat render mode requested "
        "with no facet normals defined.\n");
    mode &= ~GLM_FLAT;
  }
  if (mode & GLM_SMOOTH && !model->normals) {
    printf("glmDraw() warning: smooth render mode requested "
        "with no normals defined.\n");
    mode &= ~GLM_SMOOTH;
  }
  if (mode & GLM_TEXTURE && !model->texcoords) {
    printf("glmDraw() warning: texture render mode requested "
        "with no texture coordinates defined.\n");
    mode &= ~GLM_TEXTURE;
  }
  if (mode & GLM_FLAT && mode & GLM_SMOOTH) {
    printf("glmDraw() warning: flat render mode requested "
        "and smooth render mode requested (using smooth).\n");
    mode &= ~GLM_FLAT;
  }
  if (mode & GLM_COLOR && !model->materials) {
    printf("glmDraw() warning: color render mode requested "
        "with no materials defined.\n");
    mode &= ~GLM_COLOR;
  }
  if (mode & GLM_MATERIAL && !model->materials) {
    printf("glmDraw() warning: material render mode requested "
        "with no materials defined.\n");
    mode &= ~GLM_MATERIAL;
  }
  if (mode & GLM_COLOR && mode & GLM_MATERIAL) {
    printf("glmDraw() warning: color and material render mode requested "
        "using only material mode\n");
    mode &= ~GLM_COLOR;
  }
  if (mode & GLM_COLOR)
    glEnable(GL_COLOR_MATERIAL);
  if (mode & GLM_MATERIAL)
    glDisable(GL_COLOR_MATERIAL);

 // glPushMatrix();
  //glTranslatef(model->position[0], model->position[1], model->position[2]);//改变位置
  group = model->groups;
  while (group) {
    if (mode & GLM_MATERIAL) {
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, 
          model->materials[group->material].ambient);
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, 
          model->materials[group->material].diffuse);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, 
          model->materials[group->material].specular);
      glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 
          model->materials[group->material].shininess);
    }

    if (mode & GLM_COLOR) {
      glColor3fv(model->materials[group->material].diffuse);
    }

    /* add code here to bind texture for each model */
    if (mode & GLM_TEXTURE) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, textureArray[group->material]);
      
     // printf("texture %d : %s is bound\n", textureArray[group->material], 
     //    model->materials[group->material].map_file); 
      
    }


    glBegin(GL_TRIANGLES);
    for (i = 0; i < group->numtriangles; i++) {
      if (mode & GLM_FLAT)
        glNormal3fv(&model->facetnorms[3 * T(group->triangles[i]).findex]);

      if (mode & GLM_SMOOTH)
        glNormal3fv(&model->normals[3 * T(group->triangles[i]).nindices[0]]);
      if (mode & GLM_TEXTURE)
        glTexCoord2fv(&model->texcoords[2*T(group->triangles[i]).tindices[0]]);
        glVertex3fv(&model->vertices[3 * T(group->triangles[i]).vindices[0]]);


     /* printf("%f %f %f\n", 
          model->vertices[3 * T(group->triangles[i]).vindices[0] + X],
          model->vertices[3 * T(group->triangles[i]).vindices[0] + Y],
          model->vertices[3 * T(group->triangles[i]).vindices[0] + Z]);*/


      if (mode & GLM_SMOOTH)
       glNormal3fv(&model->normals[3 * T(group->triangles[i]).nindices[1]]);
      if (mode & GLM_TEXTURE)
       glTexCoord2fv(&model->texcoords[2*T(group->triangles[i]).tindices[1]]);
       glVertex3fv(&model->vertices[3 * T(group->triangles[i]).vindices[1]]);
	   

    /*  printf("%f %f %f\n", 
          model->vertices[3 * T(group->triangles[i]).vindices[1] + X],
          model->vertices[3 * T(group->triangles[i]).vindices[1] + Y],
          model->vertices[3 * T(group->triangles[i]).vindices[1] + Z]);*/


      if (mode & GLM_SMOOTH)
        glNormal3fv(&model->normals[3 * T(group->triangles[i]).nindices[2]]);
      if (mode & GLM_TEXTURE)
        glTexCoord2fv(&model->texcoords[2*T(group->triangles[i]).tindices[2]]);
        glVertex3fv(&model->vertices[3 * T(group->triangles[i]).vindices[2]]);

    /*  printf("%f %f %f\n", 
          model->vertices[3 * T(group->triangles[i]).vindices[2] + X],
          model->vertices[3 * T(group->triangles[i]).vindices[2] + Y],
          model->vertices[3 * T(group->triangles[i]).vindices[2] + Z]);
*/
    }
    glEnd();

    group = group->next;
  }
  delete group;
 // glPopMatrix();

}

/* glmList: Generates and returns a display list for the model using
 * the mode specified.
 *
 * model    - initialized GLMmodel structure
 * mode     - a bitwise OR of values describing what is to be rendered.
 *            GLM_NONE     -  render with only vertices
 *            GLM_FLAT     -  render with facet normals
 *            GLM_SMOOTH   -  render with vertex normals
 *            GLM_TEXTURE  -  render with texture coords
 *            GLM_COLOR    -  render with colors (color material)
 *            GLM_MATERIAL -  render with materials
 *            GLM_COLOR and GLM_MATERIAL should not both be specified.  
 *            GLM_FLAT and GLM_SMOOTH should not both be specified.  
 */



//The Expansion of circular truncated cone and cone
//H express the height of cone
//h express the height of each vertex of triangle
//R express the radius of cone|the under surface radius of circular truncated cone
//r express the upper surface radius of circular truncated cone
//maxx express the max x of all vertexs,minx express the min x of all vertexs.To calculate the radius of the cone.
//maxy express the max y of all vertexs,miny express the min y of all vertexs.To calculate the height of the cone.
//rmaxx express the max x of upper circular truncated cone(圆台上底面)，rminx express the min x of upper circular truncated cone
GLvoid
glmZhankai(GLMmodel* model)
{
	glTranslated( -0.2, -1.0, -3.5 );
  GLuint i;
  GLuint j;
  GLMgroup* group;
 // assert(model);   //decide whether the model 
//  assert(model->vertices);

  GLdouble  H, h, height;
  GLdouble R, r;
  GLdouble  maxx, minx, maxy, miny, rmaxx = -100.0, rminx = 100.0;
  GLdouble h1, h2, h3;            //the height of each vertex of all triangles
  GLdouble r1, r2, r3;            //the height of each vertex of all triangles
  GLdouble  X1, X2, X3;           //expansived vertex's x of triangles
  GLdouble  Y1, Y2, Y3;           //expansived vertex's y of triangles
  GLdouble x1, x2, x3;            //each vertex's x of triangles
  GLdouble y1, y2, y3;            //each vertex's y of triangles
  GLdouble z1, z2, z3;            //each vertex's z of triangles

  //calculate the minx,maxx,miny and maxy of the model
  maxx = minx = model->vertices[3 + X];
  maxy = miny = model->vertices[3 + Y];
  for (i = 1; i <= model->numvertices; i++) {
	  if (maxx < model->vertices[3 * i + X])
		  maxx = model->vertices[3 * i + X];
	  if (minx > model->vertices[3 * i + X])
		  minx = model->vertices[3 * i + X];

	  if (maxy < model->vertices[3 * i + Y])
		  maxy = model->vertices[3 * i + Y];
	  if (miny > model->vertices[3 * i + Y])
		  miny = model->vertices[3 * i + Y];
  }
  
  //in the upper surface of circular truncated cone find the max x(rmaxx)and min x(rminx)  圆台
  for (i = 1; i <= model->numvertices; i++) {
	  if (maxy == model->vertices[3 * i + Y]){
		  if (rmaxx < model->vertices[3 * i + X])
			  rmaxx = model->vertices[3 * i + X];
		  if (rminx > model->vertices[3 * i + X])
			  rminx = model->vertices[3 * i + X];
	  }
  }
  
  r = (_glmAbs(rmaxx) + _glmAbs(rminx)) / 2;     //upper radius
  R = (_glmAbs(maxx) + _glmAbs(minx)) / 2;       //under radius
  //if the shape is circular truncated cone,image circular truncated cone is a cone.height express the cone's highest vertex's y value.
  height = maxx*(maxy - miny) / (maxx - rmaxx) + miny;
  //calculate the ture height of circular truncated cone(image it is a cone).   圆锥
  if (height > 0 && miny > 0)
	  H = height;
  if (height > 0 && miny < 0)
	  H = height + _glmAbs(miny);
  if (height < 0)
	  H = _glmAbs(miny) - _glmAbs(height);
  group = model->groups;
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, textureArray[group->material]);
  while (group){
	  glBegin(GL_TRIANGLES);
		 
	  for (i = 0; i < group->numtriangles; i++){
		  x1 = model->vertices[3 * T(group->triangles[i]).vindices[0] + X];
		  y1 = model->vertices[3 * T(group->triangles[i]).vindices[0] + Y];
		  z1 = model->vertices[3 * T(group->triangles[i]).vindices[0] + Z];
		  x2 = model->vertices[3 * T(group->triangles[i]).vindices[1] + X];
		  y2 = model->vertices[3 * T(group->triangles[i]).vindices[1] + Y];
		  z2 = model->vertices[3 * T(group->triangles[i]).vindices[1] + Z];
		  x3 = model->vertices[3 * T(group->triangles[i]).vindices[2] + X];
		  y3 = model->vertices[3 * T(group->triangles[i]).vindices[2] + Y];
		  z3 = model->vertices[3 * T(group->triangles[i]).vindices[2] + Z];
		  h1 = height - y1;
		  h2 = height - y2;
		  h3 = height - y3;
		  r1 = h1*R / H;
		  r2 = h2*R / H;
		  r3 = h3*R / H;

		  if (x1 >= 0)
		  {
			  X1 = sqrt(r1*r1 + h1*h1)*cos((r1 *acos(z1 / r1)) / sqrt(r1*r1 + h1*h1));
			  Y1 = sqrt(r1*r1 + h1*h1)*sin((r1 *acos(z1 / r1)) / sqrt(r1*r1 + h1*h1));
		  }
		  if (x1 < 0)
		  {
			  X1 = sqrt(r1*r1 + h1*h1)*cos(r1 *(acos((-1)*z1 / r1) + M_PI) / sqrt(r1*r1 + h1*h1));
			  Y1 = sqrt(r1*r1 + h1*h1)*sin(r1 *(acos((-1)*z1 / r1) + M_PI) / sqrt(r1*r1 + h1*h1));
		  }

		  if (x2 >= 0)
		  {
			  X2 = sqrt(r2*r2 + h2*h2)*cos((r2 *acos(z2 / r2)) / sqrt(r2*r2 + h2*h2));
			  Y2 = sqrt(r2*r2 + h2*h2)*sin((r2 *acos(z2 / r2)) / sqrt(r2*r2 + h2*h2));
		  }
		  if (x2 < 0)
		  {
			  X2 = sqrt(r2*r2 + h2*h2)*cos(r2 *(acos((-1)*z2 / r2) + M_PI) / sqrt(r2*r2 + h2*h2));
			  Y2 = sqrt(r2*r2 + h2*h2)*sin(r2 *(acos((-1)*z2 / r2) + M_PI) / sqrt(r2*r2 + h2*h2));
		  }

		  if (x3 >= 0)
		  {
			  X3 = sqrt(r3*r3 + h3*h3)*cos((r3 *acos(z3 / r3)) / sqrt(r3*r3 + h3*h3));
			  Y3 = sqrt(r3*r3 + h3*h3)*sin((r3 *acos(z3 / r3)) / sqrt(r3*r3 + h3*h3));
		  }
		  if (x3 < 0)
		  {
			  X3 = sqrt(r3*r3 + h3*h3)*cos(r3 *(acos((-1)*z3 / r3) + M_PI) / sqrt(r3*r3 + h3*h3));
			  Y3 = sqrt(r3*r3 + h3*h3)*sin(r3 *(acos((-1)*z3 / r3) + M_PI) / sqrt(r3*r3 + h3*h3));
		  }

		  //handle the triangle vertexs located in the diffient side of the expansion line 
		  if (z1 >= 0 && z2 >= 0 && z3 >= 0){

			  if (x1 > 0 && x2 > 0 && x3 < 0){

				  X1 = sqrt(r1*r1 + h1*h1)*cos(r1 *(acos(z1 / r1) + 2 * M_PI) / sqrt(r1*r1 + h1*h1));
				  Y1 = sqrt(r1*r1 + h1*h1)*sin(r1 *(acos(z1 / r1) + 2 * M_PI) / sqrt(r1*r1 + h1*h1));
				  X2 = sqrt(r2*r2 + h2*h2)*cos(r2 *(acos(z2 / r2) + 2 * M_PI) / sqrt(r2*r2 + h2*h2));
				  Y2 = sqrt(r2*r2 + h2*h2)*sin(r2 *(acos(z2 / r2) + 2 * M_PI) / sqrt(r2*r2 + h2*h2));
			  }
			  if (x1>0 && x2 < 0 && x3>0){

				  X1 = sqrt(r1*r1 + h1*h1)*cos(r1 *(acos(z1 / r1) + 2 * M_PI) / sqrt(r1*r1 + h1*h1));
				  Y1 = sqrt(r1*r1 + h1*h1)*sin(r1 *(acos(z1 / r1) + 2 * M_PI) / sqrt(r1*r1 + h1*h1));
				  X3 = sqrt(r3*r3 + h3*h3)*cos(r3 *(acos(z3 / r3) + 2 * M_PI) / sqrt(r3*r3 + h3*h3));
				  Y3 = sqrt(r3*r3 + h3*h3)*sin(r3 *(acos(z3 / r3) + 2 * M_PI) / sqrt(r3*r3 + h3*h3));
			  }
			  if (x1 < 0 && x2>0 && x3 > 0){

				  X2 = sqrt(r2*r2 + h2*h2)*cos(r2 *(acos(z2 / r2) + 2 * M_PI) / sqrt(r2*r2 + h2*h2));
				  Y2 = sqrt(r2*r2 + h2*h2)*sin(r2 *(acos(z2 / r2) + 2 * M_PI) / sqrt(r2*r2 + h2*h2));
				  X3 = sqrt(r3*r3 + h3*h3)*cos(r3 *(acos(z3 / r3) + 2 * M_PI) / sqrt(r3*r3 + h3*h3));
				  Y3 = sqrt(r3*r3 + h3*h3)*sin(r3 *(acos(z3 / r3) + 2 * M_PI) / sqrt(r3*r3 + h3*h3));
			  }
			  if (x1 >= 0 && x2 <= 0 && x3 <= 0){

				  X1 = sqrt(r1*r1 + h1*h1)*cos(r1 *(acos(z1 / r1) + 2 * M_PI) / sqrt(r1*r1 + h1*h1));
				  Y1 = sqrt(r1*r1 + h1*h1)*sin(r1 *(acos(z1 / r1) + 2 * M_PI) / sqrt(r1*r1 + h1*h1));
			  }
			  if (x1 <= 0 && x2 >= 0 && x3 <= 0){

				  X2 = sqrt(r2*r2 + h2*h2)*cos(r2 *(acos(z2 / r2) + 2 * M_PI) / sqrt(r2*r2 + h2*h2));
				  Y2 = sqrt(r2*r2 + h2*h2)*sin(r2 *(acos(z2 / r2) + 2 * M_PI) / sqrt(r2*r2 + h2*h2));
			  }
			  if (x1 <= 0 && x2 <= 0 && x3 >= 0){

				  X3 = sqrt(r3*r3 + h3*h3)*cos(r3 *(acos(z3 / r3) + 2 * M_PI) / sqrt(r3*r3 + h3*h3));
				  Y3 = sqrt(r3*r3 + h3*h3)*sin(r3 *(acos(z3 / r3) + 2 * M_PI) / sqrt(r3*r3 + h3*h3));
			  }


		  }

		  if (h1 == 0)
		  {
			  X1 = 0;
			  Y1 = 0;
		  }
		  if (h2 == 0)
		  {
			  X2 = 0;
			  Y2 = 0;
		  }
		  if (h3 == 0)
		  {
			  X3 = 0;
			  Y3 = 0;
		  }
		  if((_isnan(X1)||(_isnan(X2))||(_isnan(X3))))
		  {
			continue;
		  }

		 // //printf("%f %f %f\n",  X1,X2,X3);
		  glTexCoord2fv(&model->texcoords[2 * T(group->triangles[i]).tindices[0]]);
		  glVertex2f(X1, Y1);
		  glTexCoord2fv(&model->texcoords[2 * T(group->triangles[i]).tindices[1]]);
		  glVertex2f(X2, Y2);
		  glTexCoord2fv(&model->texcoords[2 * T(group->triangles[i]).tindices[2]]);
		  glVertex2f(X3, Y3);	

	  }
	  glEnd();
	  group = group->next;
  }
	

}

GLvoid Rotate(GLMmodel* model){


}

