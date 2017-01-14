#include "StdAfx.h"
#include "enroll.h"

float toZero(float temp){
	if(fabs(temp - 0) < 1.0e-6)
		temp = 0;
	return temp;
}

///////////////////////////////////////////////////////////////////////////
//@intro	构造函数
//@param	glmModel:三维模型
//@param	profileCenters:三维模型与横截面相交圆周的圆心集合
//@param	profileRadii:三维模型与横截面相交圆周的半径集合
//@param	bboxes:三维模型被横截面切分后的片段的Bounding Box集合
//////////////////////////////////////////////////////////////////////////
Enroll::Enroll(GLModel* glmModel,vector<Vertex> profileCenters,vector<float> profileRadii,vector<SliceBBox> bboxes){
	mModel= glmModel;
	mProfileCenters = profileCenters;
	mProfileRadius = profileRadii;
	sliceBBoxes = bboxes;
}



//////////////////////////////////////////////////////////////////////////
//@intro	分别计算两个顶点lowestVertex和highestVertex在对称轴axis上的投影坐标
//@param	lowestVertex：最低顶点
//@param	highestVertex:最高顶点
//@param	axis:对称轴
//输出	lowestPointOnAxis:最低顶点lowestVertex在对称轴上的投影坐标
//		highestPointOnAxis:最高顶点highestVertex在对称轴上的投影坐标
//		low2Top:最低投影点到最高投影点之间的向量
void Enroll::CalLowAndHigh(Vertex lowestVertex,Vertex highestVertex,Normal axis,Point &lowestPointOnAxis,Point &highestPointOnAxis,Normal &low2Top){
	float lowestDotproduct = fabs(lowestVertex.GetX() * axis.GetX() + lowestVertex.GetY() * axis.GetY() + lowestVertex.GetZ() * axis.GetZ());
	float highestDotProduct = fabs(highestVertex.GetX() * axis.GetX() + highestVertex.GetY() * axis.GetY() + highestVertex.GetZ() * axis.GetZ());
	float lowestSquare = axis.GetX() * axis.GetX() + axis.GetY() * axis.GetY() + axis.GetZ() * axis.GetZ();
	float lowestUnit = sqrt(lowestDotproduct*lowestDotproduct / lowestSquare);
	Normal normal1 = Normal(lowestUnit*axis.GetX(), lowestUnit * axis.GetY(),lowestUnit * axis.GetZ());
	Normal normal2 = Normal(lowestUnit*axis.GetX()*(-1), lowestUnit * axis.GetY()*(-1),lowestUnit * axis.GetZ()*(-1));
	if(normal1.DotProduct(axis) < 0){
		lowestPointOnAxis = Point(toZero(normal1.GetX()),toZero(normal1.GetY()),toZero(normal1.GetZ()));
	}
	else{
		lowestPointOnAxis = Point(toZero(normal2.GetX()),toZero(normal2.GetY()),toZero(normal2.GetZ()));
	}
	float highestUnit = sqrt(highestDotProduct * highestDotProduct / lowestSquare);
	Normal normal3 = Normal(highestUnit * axis.GetX(),highestUnit * axis.GetY(),highestUnit * axis.GetZ());
	Normal normal4 = Normal(highestUnit * axis.GetX()*(-1),highestUnit * axis.GetY()*(-1),highestUnit * axis.GetZ()*(-1));
	if(normal3.DotProduct(axis) > 0){
		highestPointOnAxis = Point(toZero(normal3.GetX()),toZero(normal3.GetY()),toZero(normal3.GetZ()));
	}
	else{
		highestPointOnAxis = Point(toZero(normal4.GetX()),toZero(normal4.GetY()),toZero(normal4.GetZ()));
	}
	float px = highestPointOnAxis.GetX() - lowestPointOnAxis.GetX();
	float py = highestPointOnAxis.GetY() - lowestPointOnAxis.GetY();
	float pz = highestPointOnAxis.GetZ() - lowestPointOnAxis.GetZ();
	low2Top = Normal(px,py,pz);
}



//////////////////////////////////////////////////////////////////////////
//@intro	根据平面与三角面片交点对三角面片进行切割
//@param	face:当前三角面片
//@param	interVertices：三角面片与平面交点坐标
//@param	interTextures:三角面片与平面交点的纹理坐标
//@return	vector<Face>:切分后的三角面片集合
//////////////////////////////////////////////////////////////////////////
vector<Face> Enroll::CutFace(Face face, vector<Vertex> interVertices,vector<Texture> interTextures){
	Vertex vertexA = face.GetVertexA();
	Vertex vertexB = face.GetVertexB();
	Vertex vertexC = face.GetVertexC();

	Texture textureA = face.GetTextureA();
	Texture textureB = face.GetTextureB();
	Texture textureC = face.GetTextureC();
	Face face1,face2,face3;
	/*int *fvindices, *ftindices, *nindices;*/
	vector<Face> dividedFaces;
	//三角面片与平面有一个不同于顶点的交点，即三角面片将被平面分割为2个三角面片
	if( interTextures.size() == 1){
		//三角面片中的BC边与平面相交
		if( interVertices[0].GetX() == vertexA.GetX() && interVertices[0].GetY() == vertexA.GetY() && interVertices[0].GetZ() == vertexA.GetZ() ){
			face1 = Face(vertexA,vertexB,interVertices[1],textureA,textureB,interTextures[0]);
			face2 = Face(vertexA,vertexC,interVertices[1],textureA,textureC,interTextures[0]);
		}
		//三角面片中的AC边与平面相交
		if( interVertices[0].GetX() == vertexB.GetX() && interVertices[0].GetY() == vertexB.GetY() && interVertices[0].GetZ() == vertexB.GetZ() ){
			face1 = Face(vertexB,vertexA,interVertices[1],textureB,textureA,interTextures[0]);
			face2 = Face(vertexB,vertexC,interVertices[1],textureB,textureC,interTextures[0]);
		}
		//三角面片中的AB边与三角面片相交
		if( interVertices[0].GetX() == vertexC.GetX() && interVertices[0].GetY() == vertexC.GetY() && interVertices[0].GetZ() == vertexC.GetZ() ){
			face1 = Face(vertexC,vertexA,interVertices[1],textureC,textureA,interTextures[0]);
			face2 = Face(vertexC,vertexB,interVertices[1],textureC,textureB,interTextures[0]);
		}
		dividedFaces.push_back(face1);
		dividedFaces.push_back(face2);
		return dividedFaces;
	}
	//三角面片与平面有2两个不同于顶点的交点，即三角面片将被平面分割为3个三角面片
	else if(interTextures.size() == 2){
		//三角面片的AB边和AC边与平面相交
		if( interVertices[0].GetX() == vertexA.GetX() && interVertices[0].GetY() == vertexA.GetY() && interVertices[0].GetZ() == vertexA.GetZ() ){
			face1 = Face(vertexA,interVertices[1],interVertices[2],textureA,interTextures[0],interTextures[1]);
			face2 = Face(vertexB,vertexC,interVertices[1],textureB,textureC,interTextures[0]);
			face3 = Face(vertexC,interVertices[2],interVertices[1],textureC,interTextures[1],interTextures[0]);
		}
		//三角面片中的BA边和BC边与平面相交
		if( interVertices[0].GetX() == vertexB.GetX() && interVertices[0].GetY() == vertexB.GetY() && interVertices[0].GetZ() == vertexB.GetZ() ){
			face1 = Face(vertexB,interVertices[1],interVertices[2],textureB,interTextures[0],interTextures[1]);
			face2 = Face(vertexA,vertexC,interVertices[1],textureA,textureC,interTextures[0]);
			face3 = Face(vertexC,interVertices[2],interVertices[1],textureC,interTextures[1],interTextures[0]);
		}
		//三角面片中的CA边和CB边与平面相交
		if( interVertices[0].GetX() == vertexC.GetX() && interVertices[0].GetY() == vertexC.GetY() && interVertices[0].GetZ() == vertexC.GetZ() ){
			face1 = Face(vertexC,interVertices[1],interVertices[2],textureC,interTextures[0],interTextures[1]);
			face2 = Face(vertexA,vertexB,interVertices[1],textureA,textureB,interTextures[0]);
			face3 = Face(vertexB,interVertices[2],interVertices[1],textureB,interTextures[1],interTextures[0]);
		}

		dividedFaces.push_back(face1);
		dividedFaces.push_back(face2);
		dividedFaces.push_back(face3);
		return dividedFaces;
	}
	return dividedFaces;
}



//////////////////////////////////////////////////////////////////////////
//判断三角面片是否被平面切分
//@param	face:当前三角面片
//@param	abcd,planePoint：平面的参数
//@return	interPoints:三角面片与平面交点坐标
//@return	interPointsTextures:三角面片与平面交点的纹理坐标，不包含三角面片的顶点
//@return	三角面片被平面切分后的数量，返回0表示无交点，返回1表示三角面片的一个顶点在平面上，
//			返回2表示三角面片的一个顶点位于平面上，且三角面片的一条边与平面相交
//			返回3表示三角面片的两条边与平面相交
//////////////////////////////////////////////////////////////////////////
int Enroll::IsFaceCutByPlane(Face face,float* abcd,VECTOR3 planePoint,vector<Vertex> &interPoints,vector<Texture> &interPointTextures){

	Vertex vertexA = face.GetVertexA();
	Vertex vertexB = face.GetVertexB();
	Vertex vertexC = face.GetVertexC();
	Vertex interVertex1,interVertex2;
	Texture interTexture1,interTexture2;

	Texture textureA = face.GetTextureA();
	Texture textureB = face.GetTextureB();
	Texture textureC = face.GetTextureC();

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
	//bool upperThanPlane[3];

	//a,b,c都在平面的同一侧，则三角面片不会被分割   3个
	if( (upper[0] == -1 && upper[1] == -1 && upper[2] == -1) || (upper[0] == 1 && upper[1] == 1 && upper[2] == 1) ){
		return 0;
	}
	/*if( (dis[0] > 0 && dis[1] > 0 && dis[2] > 0) || (dis[0] < 0 && dis[1] < 0 && dis[2] < 0) ){
	return 0;
	}*/
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

		if(PlaneLineIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexB,vertexC,interVertex1,textureB,textureC,interTexture1)){
			interPoints.push_back(interVertex1);
			interPointTextures.push_back(interTexture1);
		}
		return 2;
	}
	//顶点b在平面上，a,c分别在平面两侧
	else if(fabs(dis[1]) < 1.0e-8 && upper[0] * upper[2] == -1) {
		//else if(fabs(dis[1]) < 1.0e-8 && dis[0] * dis[2] < 0){
		//else if( dis[1] == 0 && dis[0] * dis[2] < 0 ){
		interPoints.push_back(vertexB);
		if(PlaneLineIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexA,vertexC,interVertex1,textureA,textureC,interTexture1)){
			interPoints.push_back(interVertex1);
			interPointTextures.push_back(interTexture1);
		}
		return 2;
	}
	//顶点c在平面上，a,b分别在平面两侧
	else if(fabs(dis[2]) < 1.0e-8 && upper[0] * upper[1] == 1){
		//else if(fabs(dis[2]) < 1.0e-8 && dis[0] * dis[1] < 0){
		//else if(dis[2] == 0 && dis[0] * dis[1] < 0){
		interPoints.push_back(vertexC);
		if(PlaneLineIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexA,vertexB,interVertex1,textureA,textureB,interTexture1)){
			interPoints.push_back(interVertex1);
			interPointTextures.push_back(interTexture1);
		}
		return 2;
	}




	//三角面片的三个顶点被分隔到平面的两侧，此时需要将三角面片分割为3个小的三角面片
	//a与b,c分别在平面的两侧
	else if( dis[0] * dis[1] < 0  && dis[0] * dis[2] < 0 && dis[1] * dis[2] > 0){
		//else if( (dis[0] < 0 && dis[1] > 0 && dis[2] > 0) || (dis[0] > 0 && dis[1] < 0 && dis[2] < 0) ){
		interPoints.push_back(vertexA);
		if(PlaneLineIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexA,vertexB,interVertex1,textureA,textureB,interTexture1)){
			interPoints.push_back(interVertex1);
			interPointTextures.push_back(interTexture1);
		}
		if(PlaneLineIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexA,vertexC,interVertex2,textureA,textureC,interTexture2)){
			interPoints.push_back(interVertex2);
			interPointTextures.push_back(interTexture2);
		}
		return 3;
	}
	//b与a,c分别在平面的两侧
	else if( dis[0] * dis[1] < 0 && dis[0] *dis[2] > 0 && dis[1] * dis[2] < 0){
		//else if( (dis[0] > 0 && dis[1] < 0 && dis[2] > 0) || (dis[0] < 0 && dis[1] > 0 && dis[2] < 0) ){
		interPoints.push_back(vertexB);
		if( PlaneLineIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexB,vertexA,interVertex1,textureB,textureA,interTexture1) ){
			interPoints.push_back(interVertex1);
			interPointTextures.push_back(interTexture1);
		}
		if( PlaneLineIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexB,vertexC,interVertex2,textureB,textureC,interTexture2) ){
			interPoints.push_back(interVertex2);
			interPointTextures.push_back(interTexture2);
		}
		return 3;
	}
	//c与a,b分别在平面的两侧
	else if(dis[0] * dis[1] > 0 &&  dis[0] * dis[2] < 0 && dis[1] * dis[2] < 0 ){
		//else if((dis[0] < 0 && dis[1] < 0 && dis[2] > 0) || (dis[0] > 0 && dis[1] > 0 && dis[2] < 0)){
		interPoints.push_back(vertexC);
		if( PlaneLineIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexC,vertexA, interVertex1,textureC,textureA,interTexture1) ){
			interPoints.push_back(interVertex1);
			interPointTextures.push_back(interTexture1);
		}
		if( PlaneLineIntersectPt(Normal(abcd[0],abcd[1],abcd[2]),Vertex(planePoint.vec[0],planePoint.vec[1],planePoint.vec[2]),vertexC,vertexB,interVertex2,textureC,textureB,interTexture2) ){
			interPoints.push_back(interVertex2);
			interPointTextures.push_back(interTexture2);
		}
		return 3;
	}

	else{
		return 0;
	}
	return 0;
}


//////////////////////////////////////////////////////////////////////////
//@intro	判断线段与平面交点
//@param	planeNormal:平面法向量
//@param	planePoint:平面上一点
//@param	vertexA, vertexB:线段的两个端点
//@param	texture_a, texture_b: 线段两个端点的纹理坐标
//@return	interVertex:交点的坐标
//@return	interTexture:交点的纹理坐标
//@return	bool:线段与平面是否相交
//////////////////////////////////////////////////////////////////////////
bool Enroll::PlaneLineIntersectPt(Normal planeNormal,Vertex planePoint,Vertex vertexA,Vertex vertexB,
	Vertex &interVertex,Texture texture_a,Texture texture_b,Texture &interTexture){
	float linesegAB[3]; //A->B
	linesegAB[0] = vertexB.GetX() - vertexA.GetX();
	linesegAB[1] = vertexB.GetY() - vertexA.GetY();
	linesegAB[2] = vertexB.GetZ() - vertexA.GetZ();
	float textureAB[2];
	textureAB[0] = texture_b.GetX() - texture_a.GetX();
	textureAB[1] = texture_b.GetY() - texture_a.GetY();
	float linesegLength = sqrt(linesegAB[0] * linesegAB[0] + linesegAB[1] * linesegAB[1] + linesegAB[2] * linesegAB[2]);
	if(linesegLength < 1.0e-8){ //向量模为0
		return false;
	}
	linesegAB[0] /= linesegLength;
	linesegAB[1] /= linesegLength;
	linesegAB[2] /= linesegLength;

	textureAB[0] /= linesegLength;
	textureAB[1] /= linesegLength;

	float vpt = planeNormal.GetX() * linesegAB[0] + planeNormal.GetY() * linesegAB[1] + planeNormal.GetZ() * linesegAB[2]; //点积判断是否平行
	if(fabs(vpt) < 1.0e-8){ //线段与平面平行
		return false;
	}

	float t = ((planePoint.GetX() - vertexA.GetX()) * planeNormal.GetX() + (planePoint.GetY() - vertexA.GetY()) * planeNormal.GetY() + (planePoint.GetZ() - vertexA.GetZ()) * planeNormal.GetZ() ) / vpt;
	if( t < 0 || t > linesegLength){
		return false;
	}
	interVertex = Vertex(vertexA.GetX() + t * linesegAB[0], vertexA.GetY() + t * linesegAB[1], vertexA.GetZ() + t * linesegAB[2]);
	interTexture = Texture(texture_a.GetX() + t *textureAB[0], texture_a.GetY() + t * textureAB[1]);
	return true;
}


//////////////////////////////////////////////////////////////////////////
//@intro	计算profile的平均半径
//@param	centerPoint:对称轴上的点
//@param	axisNormal:对称轴的方向向量
//@param	lineSegment3D:profile上的点集
//@return	profile上的点到对称轴的平均距离
//////////////////////////////////////////////////////////////////////////
float Enroll::LineSegmentRadius(Vertex centerPoint,Normal axisNormal,vector<Vertex> lineSegment3D){
	float sumDistance= 0;
	if(lineSegment3D.size() == 0){
		return 0;
	}

	for(int i = 0;i < lineSegment3D.size();i++){
		sumDistance += DisToAxis(centerPoint,axisNormal,lineSegment3D[i]);
	}
	return sumDistance / lineSegment3D.size();
}

//////////////////////////////////////////////////////////////////////////
//@intro	计算profile的中心
//@param	lineSegment3D:代表profile的顶点数组
//@return	profile的中心点坐标
//////////////////////////////////////////////////////////////////////////
Vertex Enroll::LineSegmentCenter(vector<Vertex> lineSegment3d){
	float center[3], sum3D[3];
	Vertex profile_center;
	float lengthsum = 0.0;

	center[0] = sum3D[0] = center[1] = sum3D[1] = center[2] = sum3D[2] = 0;
	int j;
	if(lineSegment3d.size()<=1)
		return Vertex(0,0,0);
	for(int i=0;i < lineSegment3d.size();i++){
		sum3D[0] += lineSegment3d[i].GetX();
		sum3D[1] += lineSegment3d[i].GetY();
		sum3D[2] += lineSegment3d[i].GetZ();

	}
	return Vertex(sum3D[0]/lineSegment3d.size(),sum3D[1]/lineSegment3d.size(),sum3D[2]/lineSegment3d.size());
}


//////////////////////////////////////////////////////////////////////////
//@intro	计算点curPoint到轴上的距离
//@param	centerPoint:轴上的某点
//@param	axisNormal:对称轴的方向向量
//@param	curPoint:当前点的坐标
//@return	点到对称轴的距离
//////////////////////////////////////////////////////////////////////////
float Enroll::DisToAxis(Vertex centerPoint,Normal axisNormal, Vertex curPoint){
	Normal normal1 = Normal(curPoint.GetX() - centerPoint.GetX(), curPoint.GetY() - centerPoint.GetY(), curPoint.GetZ() - centerPoint.GetZ());
	//Normal normal1 = Normal(curPoint.vec[0] - centerPoint.vec[0], curPoint.vec[1] - centerPoint.vec[1], curPoint.vec[2] - centerPoint.vec[2]);
	Normal crossNormal = normal1.Cross(axisNormal);
	return crossNormal.GetLength() / axisNormal.GetLength();
}

//////////////////////////////////////////////////////////////////////////
//对三维模型进行分割，分为若干个slice
//@return	vector<GCSlice>:slice数组
//////////////////////////////////////////////////////////////////////////
vector<GCSlice> Enroll::CutModel(){
	vector<Triangle> triangles = mModel->mTriangles;
	vector<Vertex> vertices = mModel->mVertices;
	vector<Texture> textures = mModel->mTexcoods;

	mAxisNormal = mModel->mAxisNormal;
	vector<Face> faces;
	C3DCircleExtract testPlane;
	vector<Vertex> line,lineSeg;
	vector<Texture> lineTextures;
	vector<Face> dividedFaces;

	for(int i = 0;i < triangles.size();i++){
		Triangle tri = triangles[i];
		int* vindices = tri.GetVindices();
		int* tindices = tri.GetTindices();

		Vertex vertexA = vertices.at(vindices[0]);
		Vertex vertexB = vertices.at(vindices[1]);
		Vertex vertexC = vertices.at(vindices[2]);

		Texture textureA = textures.at(tindices[0]);
		Texture textureB = textures.at(tindices[1]);
		Texture textureC = textures.at(tindices[2]);

		Face face = Face(vertexA,vertexB,vertexC,textureA,textureB,textureC);
		faces.push_back(face);
	}

	testPlane.setPlane(Vertex(0,0,0),Normal(1,0,0));
	for(int j = 0; j < faces.size();j++){
		line.clear();
		lineTextures.clear();
		dividedFaces.clear();
		//如果当前三角面片与平面相交，则将三角面片分割
		if(IsFaceCutByPlane(faces[j],testPlane.abcd,testPlane.center,line,lineTextures) > 0){
			if(lineTextures.size() > 0){
				Face tempFace = faces[j];
				dividedFaces = CutFace(faces[j],line,lineTextures);
				faces[j] = dividedFaces[0];
				for(int m = 1; m < dividedFaces.size();m++){
					//cout<<"cut by yoz"<<endl;
					faces.push_back(dividedFaces[m]);
				}
				j--;
			}
		}
	}

	for(int i = 0;i < mProfileCenters.size();i++){
		lineSeg.clear();
		testPlane.setPlane(mProfileCenters[i],mAxisNormal);
		//使用当前切分平面对所有三角面片进行切分
		for(int j = 0; j < faces.size();j++){
			line.clear();
			lineTextures.clear();
			dividedFaces.clear();

			//判断当前三角面片是否与切分平面相交
			if(IsFaceCutByPlane(faces[j],testPlane.abcd,testPlane.center,line,lineTextures)>0){
				if(line.size() == 1){  //三角面片与平面交点为1，即三角面片的一个顶点位于切分平面上
					lineSeg.push_back(line[0]);
				}else if(line.size() == 2){ //三角面片与平面交点为2，即三角面片的一条边位于平面上，或者一条边与平面相交
					lineSeg.push_back(line[0]);
					lineSeg.push_back(line[1]);
				}else if(line.size() == 3){ //三角面片的两条边与平面相交，
					lineSeg.push_back(line[1]);
					lineSeg.push_back(line[2]);
				}
				//三角面片被切分平面切分
				if(lineTextures.size() > 0){
					Face tempFace = faces[j];
					dividedFaces = CutFace(faces[j],line,lineTextures); //对当前三角面片进行切分，返回切分后的三角面片数组
					faces[j] = dividedFaces[0];
					for(int m = 1; m < dividedFaces.size();m++){
						faces.push_back(dividedFaces[m]);
					}
					j--;
				}

			}

		}
		mLineSegments.push_back(lineSeg);
		
	}

	for(int i = 0; i < mLineSegments.size();i++){
		mProfileRadius[i] = LineSegmentRadius(mProfileCenters[i],mAxisNormal,mLineSegments[i]);
	}

	vector<GCSlice> slices;
	for(int i = 0; i < mLineSegments.size()-1;i++){
		GCSlice slice = GCSlice(mAxisNormal,mProfileCenters[i],mProfileCenters[i+1],mProfileRadius[i],mProfileRadius[i+1],sliceBBoxes[i].getEnrollMode());
		slices.push_back(slice);
	}
	testPlane.setPlane(Vertex(0,0,0),Normal(1,0,0));
	for(int i =0; i < faces.size();i++){
		if(IsFaceUpperPlane(faces[i],testPlane.abcd,testPlane.center) == 1){
			faces[i].SetPositive(true);
		}
		else if(IsFaceUpperPlane(faces[i],testPlane.abcd,testPlane.center) == -1){
			faces[i].SetPositive(false);
		}
		for(int j = 0; j < slices.size();j++){
			if(slices[j].IsContainFace(faces[i])){
				slices[j].AddFace(faces[i]);
			}
		}
	}
	return slices;
}


