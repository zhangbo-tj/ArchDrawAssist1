#include "StdAfx.h"
#include "enroll.h"

float toZero(float temp){
	if(fabs(temp - 0) < 1.0e-6)
		temp = 0;
	return temp;
}

///////////////////////////////////////////////////////////////////////////
//@intro	���캯��
//@param	glmModel:��άģ��
//@param	profileCenters:��άģ���������ཻԲ�ܵ�Բ�ļ���
//@param	profileRadii:��άģ���������ཻԲ�ܵİ뾶����
//@param	bboxes:��άģ�ͱ�������зֺ��Ƭ�ε�Bounding Box����
//////////////////////////////////////////////////////////////////////////
Enroll::Enroll(GLModel* glmModel,vector<Vertex> profileCenters,vector<float> profileRadii,vector<SliceBBox> bboxes){
	mModel= glmModel;
	mProfileCenters = profileCenters;
	mProfileRadius = profileRadii;
	sliceBBoxes = bboxes;
}



//////////////////////////////////////////////////////////////////////////
//@intro	�ֱ������������lowestVertex��highestVertex�ڶԳ���axis�ϵ�ͶӰ����
//@param	lowestVertex����Ͷ���
//@param	highestVertex:��߶���
//@param	axis:�Գ���
//���	lowestPointOnAxis:��Ͷ���lowestVertex�ڶԳ����ϵ�ͶӰ����
//		highestPointOnAxis:��߶���highestVertex�ڶԳ����ϵ�ͶӰ����
//		low2Top:���ͶӰ�㵽���ͶӰ��֮�������
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
//@intro	����ƽ����������Ƭ�����������Ƭ�����и�
//@param	face:��ǰ������Ƭ
//@param	interVertices��������Ƭ��ƽ�潻������
//@param	interTextures:������Ƭ��ƽ�潻�����������
//@return	vector<Face>:�зֺ��������Ƭ����
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
	//������Ƭ��ƽ����һ����ͬ�ڶ���Ľ��㣬��������Ƭ����ƽ��ָ�Ϊ2��������Ƭ
	if( interTextures.size() == 1){
		//������Ƭ�е�BC����ƽ���ཻ
		if( interVertices[0].GetX() == vertexA.GetX() && interVertices[0].GetY() == vertexA.GetY() && interVertices[0].GetZ() == vertexA.GetZ() ){
			face1 = Face(vertexA,vertexB,interVertices[1],textureA,textureB,interTextures[0]);
			face2 = Face(vertexA,vertexC,interVertices[1],textureA,textureC,interTextures[0]);
		}
		//������Ƭ�е�AC����ƽ���ཻ
		if( interVertices[0].GetX() == vertexB.GetX() && interVertices[0].GetY() == vertexB.GetY() && interVertices[0].GetZ() == vertexB.GetZ() ){
			face1 = Face(vertexB,vertexA,interVertices[1],textureB,textureA,interTextures[0]);
			face2 = Face(vertexB,vertexC,interVertices[1],textureB,textureC,interTextures[0]);
		}
		//������Ƭ�е�AB����������Ƭ�ཻ
		if( interVertices[0].GetX() == vertexC.GetX() && interVertices[0].GetY() == vertexC.GetY() && interVertices[0].GetZ() == vertexC.GetZ() ){
			face1 = Face(vertexC,vertexA,interVertices[1],textureC,textureA,interTextures[0]);
			face2 = Face(vertexC,vertexB,interVertices[1],textureC,textureB,interTextures[0]);
		}
		dividedFaces.push_back(face1);
		dividedFaces.push_back(face2);
		return dividedFaces;
	}
	//������Ƭ��ƽ����2������ͬ�ڶ���Ľ��㣬��������Ƭ����ƽ��ָ�Ϊ3��������Ƭ
	else if(interTextures.size() == 2){
		//������Ƭ��AB�ߺ�AC����ƽ���ཻ
		if( interVertices[0].GetX() == vertexA.GetX() && interVertices[0].GetY() == vertexA.GetY() && interVertices[0].GetZ() == vertexA.GetZ() ){
			face1 = Face(vertexA,interVertices[1],interVertices[2],textureA,interTextures[0],interTextures[1]);
			face2 = Face(vertexB,vertexC,interVertices[1],textureB,textureC,interTextures[0]);
			face3 = Face(vertexC,interVertices[2],interVertices[1],textureC,interTextures[1],interTextures[0]);
		}
		//������Ƭ�е�BA�ߺ�BC����ƽ���ཻ
		if( interVertices[0].GetX() == vertexB.GetX() && interVertices[0].GetY() == vertexB.GetY() && interVertices[0].GetZ() == vertexB.GetZ() ){
			face1 = Face(vertexB,interVertices[1],interVertices[2],textureB,interTextures[0],interTextures[1]);
			face2 = Face(vertexA,vertexC,interVertices[1],textureA,textureC,interTextures[0]);
			face3 = Face(vertexC,interVertices[2],interVertices[1],textureC,interTextures[1],interTextures[0]);
		}
		//������Ƭ�е�CA�ߺ�CB����ƽ���ཻ
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
//�ж�������Ƭ�Ƿ�ƽ���з�
//@param	face:��ǰ������Ƭ
//@param	abcd,planePoint��ƽ��Ĳ���
//@return	interPoints:������Ƭ��ƽ�潻������
//@return	interPointsTextures:������Ƭ��ƽ�潻����������꣬������������Ƭ�Ķ���
//@return	������Ƭ��ƽ���зֺ������������0��ʾ�޽��㣬����1��ʾ������Ƭ��һ��������ƽ���ϣ�
//			����2��ʾ������Ƭ��һ������λ��ƽ���ϣ���������Ƭ��һ������ƽ���ཻ
//			����3��ʾ������Ƭ����������ƽ���ཻ
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
	//������Ƭ��ƽ��ƽ�У��򷵻�0
	if(IsParallel(VECTOR3(triNormal.GetX(),triNormal.GetY(),triNormal.GetZ()),VECTOR3(abcd[0],abcd[1],abcd[2]),1.0e-6)){
		return 0;
	}
	float dis[3];  //��¼�㵽ƽ��ľ���
	dis[0] = abcd[0] * vertexA.GetX() + abcd[1] * vertexA.GetY() + abcd[2] * vertexA.GetZ() + abcd[3];
	dis[1] = abcd[0] * vertexB.GetX() + abcd[1] * vertexB.GetY() + abcd[2] * vertexB.GetZ() + abcd[3];
	dis[2] = abcd[0] * vertexC.GetX() + abcd[1] * vertexC.GetY() + abcd[2] * vertexC.GetZ() + abcd[3];

	float upper[3];
	upper[0] = IsVertexUpperPlane(vertexA,abcd,planePoint);
	upper[1] = IsVertexUpperPlane(vertexB,abcd,planePoint);
	upper[2] = IsVertexUpperPlane(vertexC,abcd,planePoint);
	//bool upperThanPlane[3];

	//a,b,c����ƽ���ͬһ�࣬��������Ƭ���ᱻ�ָ�   3��
	if( (upper[0] == -1 && upper[1] == -1 && upper[2] == -1) || (upper[0] == 1 && upper[1] == 1 && upper[2] == 1) ){
		return 0;
	}
	/*if( (dis[0] > 0 && dis[1] > 0 && dis[2] > 0) || (dis[0] < 0 && dis[1] < 0 && dis[2] < 0) ){
	return 0;
	}*/
	//������Ƭ���������㶼��ƽ����
	//����a,b��ƽ����
	else if( fabs(dis[0]) < 1.0e-8 && fabs(dis[1]) < 1.0e-8 ){
		//else if(dis[0] == 0 && dis[1] == 0){
		interPoints.push_back(vertexA);
		interPoints.push_back(vertexB);
		return 1;
	}	
	//����a,c��ƽ����
	else if( fabs(dis[0]) < 1.0e-8 && fabs(dis[2]) < 1.0e-8 ){
		//else if(dis[0] == 0 && dis[2] == 0){
		interPoints.push_back(vertexA);
		interPoints.push_back(vertexC);
		return 1;
	}
	//����b,c��ƽ����
	else if( fabs(dis[1]) < 1.0e-8 && fabs(dis[2]) < 1.0e-8 ){
		//else if(dis[1] == 0 && dis[2] == 0){
		interPoints.push_back(vertexB);
		interPoints.push_back(vertexC);
		return 1;
	}
	//������Ƭ��һ��������ƽ���ϣ����������������㶼��ƽ���ͬ�࣬��������Ƭ���ᱻƽ��ָ�,����ֵΪ1 6��
	//����a��ƽ����,b,c��ͬ��
	else if(fabs(dis[0]) < 1.0e-8 && upper[1] * upper[2] == 1 ){
		//else if(fabs(dis[0]) < 1.0e-8 && dis[1] * dis[2] > 0 ){
		interPoints.push_back(vertexA);
		//interPointTextures.push_back(textureA);
		return 1;
	}
	//����b��ƽ����,a,c��ͬ��

	else if(fabs(dis[1]) < 1.0e-8 && upper[0] * upper[2] == 1) {
		//else if( fabs(dis[1]) < 1.0e-8 && dis[0] * dis[2] > 0){
		interPoints.push_back(vertexB);
		return 1;
	}
	//����c��ƽ����,a,b��ͬ��
	else if(fabs(dis[2]) < 1.0e-8 && upper[0] * upper[1] == 1){
		//else if(fabs(dis[2]) < 1.0e-8 && dis[0] * dis[1] > 0){
		//else if(dis[2] == 0 && dis[0] * dis[1] > 0){
		interPoints.push_back(vertexC);
		return 1;
	}

	//������Ƭ��һ��������ƽ���ϣ���������������ƽ������࣬��ʱ��Ҫ��������Ƭ�ָ�Ϊ2��С������Ƭ
	//����a��ƽ���ϣ�b,c�ֱ���ƽ�������
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
	//����b��ƽ���ϣ�a,c�ֱ���ƽ������
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
	//����c��ƽ���ϣ�a,b�ֱ���ƽ������
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




	//������Ƭ���������㱻�ָ���ƽ������࣬��ʱ��Ҫ��������Ƭ�ָ�Ϊ3��С��������Ƭ
	//a��b,c�ֱ���ƽ�������
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
	//b��a,c�ֱ���ƽ�������
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
	//c��a,b�ֱ���ƽ�������
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
//@intro	�ж��߶���ƽ�潻��
//@param	planeNormal:ƽ�淨����
//@param	planePoint:ƽ����һ��
//@param	vertexA, vertexB:�߶ε������˵�
//@param	texture_a, texture_b: �߶������˵����������
//@return	interVertex:���������
//@return	interTexture:�������������
//@return	bool:�߶���ƽ���Ƿ��ཻ
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
	if(linesegLength < 1.0e-8){ //����ģΪ0
		return false;
	}
	linesegAB[0] /= linesegLength;
	linesegAB[1] /= linesegLength;
	linesegAB[2] /= linesegLength;

	textureAB[0] /= linesegLength;
	textureAB[1] /= linesegLength;

	float vpt = planeNormal.GetX() * linesegAB[0] + planeNormal.GetY() * linesegAB[1] + planeNormal.GetZ() * linesegAB[2]; //����ж��Ƿ�ƽ��
	if(fabs(vpt) < 1.0e-8){ //�߶���ƽ��ƽ��
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
//@intro	����profile��ƽ���뾶
//@param	centerPoint:�Գ����ϵĵ�
//@param	axisNormal:�Գ���ķ�������
//@param	lineSegment3D:profile�ϵĵ㼯
//@return	profile�ϵĵ㵽�Գ����ƽ������
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
//@intro	����profile������
//@param	lineSegment3D:����profile�Ķ�������
//@return	profile�����ĵ�����
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
//@intro	�����curPoint�����ϵľ���
//@param	centerPoint:���ϵ�ĳ��
//@param	axisNormal:�Գ���ķ�������
//@param	curPoint:��ǰ�������
//@return	�㵽�Գ���ľ���
//////////////////////////////////////////////////////////////////////////
float Enroll::DisToAxis(Vertex centerPoint,Normal axisNormal, Vertex curPoint){
	Normal normal1 = Normal(curPoint.GetX() - centerPoint.GetX(), curPoint.GetY() - centerPoint.GetY(), curPoint.GetZ() - centerPoint.GetZ());
	//Normal normal1 = Normal(curPoint.vec[0] - centerPoint.vec[0], curPoint.vec[1] - centerPoint.vec[1], curPoint.vec[2] - centerPoint.vec[2]);
	Normal crossNormal = normal1.Cross(axisNormal);
	return crossNormal.GetLength() / axisNormal.GetLength();
}

//////////////////////////////////////////////////////////////////////////
//����άģ�ͽ��зָ��Ϊ���ɸ�slice
//@return	vector<GCSlice>:slice����
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
		//�����ǰ������Ƭ��ƽ���ཻ����������Ƭ�ָ�
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
		//ʹ�õ�ǰ�з�ƽ�������������Ƭ�����з�
		for(int j = 0; j < faces.size();j++){
			line.clear();
			lineTextures.clear();
			dividedFaces.clear();

			//�жϵ�ǰ������Ƭ�Ƿ����з�ƽ���ཻ
			if(IsFaceCutByPlane(faces[j],testPlane.abcd,testPlane.center,line,lineTextures)>0){
				if(line.size() == 1){  //������Ƭ��ƽ�潻��Ϊ1����������Ƭ��һ������λ���з�ƽ����
					lineSeg.push_back(line[0]);
				}else if(line.size() == 2){ //������Ƭ��ƽ�潻��Ϊ2����������Ƭ��һ����λ��ƽ���ϣ�����һ������ƽ���ཻ
					lineSeg.push_back(line[0]);
					lineSeg.push_back(line[1]);
				}else if(line.size() == 3){ //������Ƭ����������ƽ���ཻ��
					lineSeg.push_back(line[1]);
					lineSeg.push_back(line[2]);
				}
				//������Ƭ���з�ƽ���з�
				if(lineTextures.size() > 0){
					Face tempFace = faces[j];
					dividedFaces = CutFace(faces[j],line,lineTextures); //�Ե�ǰ������Ƭ�����з֣������зֺ��������Ƭ����
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


