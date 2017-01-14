#include "StdAfx.h"
#include "GCSlice.h"

//////////////////////////////////////////////////////////////////////////
//@intro	��ʼ��GCSlice
//@param	axisnormal:��άģ�͵ĶԳ���ķ�����
//@param	lowerpoint:slice�½����ϵĵ�
//@param	higherpoint:slice�Ͻ����ϵĵ�
//@param	lowrRadius:slice�½���İ뾶
//@param	highRadius��slice�Ͻ���İ뾶
//////////////////////////////////////////////////////////////////////////
GCSlice::GCSlice(Normal axisnormal,Vertex lowerpoint,Vertex higherpoint,float lowRadius, float highRadius,EnrollMode mode){
	mAxisNormal = axisnormal;
	mLowerPoint = lowerpoint;
	mUpperPoint = higherpoint;
	mLowerRadius = lowRadius;
	mHigherRadius = highRadius;
	mEnrollMode = mode;
}




//////////////////////////////////////////////////////////////////////////
//@intro	�ж�������Ƭ�Ƿ�λ��slice�����ڣ���λ�ڸ�slice�����½����м�
//@param	face:������Ƭ
//@return	�Ƿ��������ڣ����������򷵻�true�������������򷵻�false
//////////////////////////////////////////////////////////////////////////
bool GCSlice::IsContainFace(Face face){
	Vertex vertexA = face.GetVertexA();
	Vertex vertexB = face.GetVertexB();
	Vertex vertexC = face.GetVertexC();
	
	//�ֱ����slice�����º����	
	C3DCircleExtract lowerPlane,higherPlane;
	lowerPlane.setPlane(mLowerPoint,mAxisNormal);
	higherPlane.setPlane(mUpperPoint,mAxisNormal);
	bool higherThanLow = false, lowerThanUpper = false;

	//�ж�������Ƭ�Ƿ���slice�½����Ϸ�
	if(IsFaceUpperPlane(face,lowerPlane.abcd,lowerPlane.center) == 1){
		higherThanLow = true;
	}
	else{
		return false;
	}
	//�ж�������Ƭ�Ƿ�λ��slice�Ͻ����·�
	if(IsFaceUpperPlane(face,higherPlane.abcd,higherPlane.center) == -1){
		lowerThanUpper = true;
	}
	else{
		return false;
	}

	//���������Ƭͬʱλ���½����Ϸ����Ͻ����·��򷵻�true,���򷵻�false
	if( higherThanLow && lowerThanUpper){
		return true;
	}
	return false;
}



//////////////////////////////////////////////////////////////////////////
//@intro	��������Ƭ��ӵ�slice��
//@param	face��������Ƭ
//////////////////////////////////////////////////////////////////////////
void GCSlice::AddFace(Face face){
	mFaces.push_back(face);
}


//////////////////////////////////////////////////////////////////////////
//@intro	���Բ̨
//  [4/12/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
void GCSlice::ConialfrustumFitting(){
	mFlag = IsRight();
	vector<pPOINT> RefectPoints;
	vector<Vertex> vertices;
	for(int i = 0;i< mFaces.size();i++){
		Vertex vertexA = mFaces[i].GetVertexA();
		Vertex vertexB = mFaces[i].GetVertexB();
		Vertex vertexC = mFaces[i].GetVertexC();
		
		pPOINT p1 = {vertexA.GetY(), sqrt(vertexA.GetX() * vertexA.GetX() + vertexA.GetZ() * vertexA.GetZ())};
		pPOINT p2 = {vertexB.GetY(), sqrt(vertexB.GetX() * vertexB.GetX() + vertexB.GetZ() * vertexB.GetZ())};
		pPOINT p3 = {vertexC.GetY(), sqrt(vertexC.GetX() * vertexC.GetX() + vertexC.GetZ() * vertexC.GetZ())};

		RefectPoints.push_back(p1);
		RefectPoints.push_back(p2);
		RefectPoints.push_back(p3);
	}
	CalcConvexHull(RefectPoints);

	float reflectmax_y = RefectPoints[0].y;
	float reflectmin_x = RefectPoints[0].x;
	float reflectmax_x = RefectPoints[0].x;
	float m1 = 0, m2 = 0, m3 = 0, m4 = 0;
	float x_y1 = 0, x_y2 = 0, x_y3 = 0, x_y4 = 0;
	float start_max = 0, end_max = 0, start_min = 0, end_min = 0;
	int k = 0;
	float V = 100;
	float VC = 0;
	float r1 = 0, r2 = 0, R1 = 0, R2 = 0;
	float distance = 0;
	for (int i = 0; i < RefectPoints.size(); i++){
		if (RefectPoints[i].y > reflectmax_y)
		{
			reflectmax_y = RefectPoints[i].y;
			k = i;
		}
		if (RefectPoints[i].x > reflectmax_x)
			reflectmax_x = RefectPoints[i].x;
		if (RefectPoints[i].x < reflectmin_x)
			reflectmin_x = RefectPoints[i].x;
	}
	if (k - 2 >= 0){
		if ((RefectPoints[k - 1].x != RefectPoints[k].x) && (RefectPoints[k - 1].x != RefectPoints[k - 2].x))
		{
			m1 = (RefectPoints[k - 1].y - RefectPoints[k].y) / (RefectPoints[k - 1].x - RefectPoints[k].x);
			m2 = (RefectPoints[k - 1].y - RefectPoints[k - 2].y) / (RefectPoints[k - 1].x - RefectPoints[k - 2].x);
			x_y1 = m1*(reflectmax_x - RefectPoints[k - 1].x) + RefectPoints[k - 1].y;
			x_y2 = m2*(reflectmax_x - RefectPoints[k - 1].x) + RefectPoints[k - 1].y;
			x_y3 = m1*(reflectmin_x - RefectPoints[k - 1].x) + RefectPoints[k - 1].y;
			x_y4 = m2*(reflectmin_x - RefectPoints[k - 1].x) + RefectPoints[k - 1].y;
			start_max = x_y1 < x_y2 ? x_y1 : x_y2;
			end_max = x_y1 > x_y2 ? x_y1 : x_y2;
			start_min = x_y3 < x_y4 ? x_y3 : x_y4;
			end_min = x_y3 > x_y4 ? x_y3 : x_y4;
			distance = (end_max - start_max) / 10;
			while (start_max < end_max){
				float  mp = (start_max - RefectPoints[k - 1].y) / (reflectmax_x - RefectPoints[k - 1].x);
				float  yp = mp*(reflectmin_x - RefectPoints[k - 1].x) + RefectPoints[k - 1].y;
				VC = start_max *start_max + start_max*yp + yp*yp;
				if (VC < V)
				{
					V = VC;
					mMinRadius = start_max;
					mMaxRadius = yp;
				}
				start_max = start_max + distance;
			}
			distance = (end_min - start_min) / 10;
			while (start_min < end_min){
				float  mp = (start_min - RefectPoints[k - 1].y) / (reflectmin_x - RefectPoints[k - 1].x);
				float  yp = mp*(reflectmax_x - RefectPoints[k - 1].x) + RefectPoints[k - 1].y;
				VC = start_min*start_min + start_min*yp + yp*yp;
				if (VC < V)
				{
					V = VC;
					mMinRadius = yp;
					mMaxRadius = start_min;
				}
				start_min = start_min + distance;
			}
		}
		else
			if (RefectPoints[k - 1].x == RefectPoints[k - 2].x){
				m1 = (RefectPoints[k - 1].y - RefectPoints[k].y) / (RefectPoints[k - 1].x - RefectPoints[k].x);
				mMinRadius = m1*(reflectmax_x - RefectPoints[k - 1].x) + RefectPoints[k - 1].y;
				mMaxRadius = m1*(reflectmin_x - RefectPoints[k - 1].x) + RefectPoints[k - 1].y;

			}
			else if (RefectPoints[k - 1].x == RefectPoints[k].x){
				m2 = (RefectPoints[k - 1].y - RefectPoints[k - 2].y) / (RefectPoints[k - 1].x - RefectPoints[k - 2].x);
				mMaxRadius = m2*(reflectmax_x - RefectPoints[k - 1].x) + RefectPoints[k - 1].y;
				mMinRadius = m2*(reflectmin_x - RefectPoints[k - 1].x) + RefectPoints[k - 1].y;
			}
	}


	if (k + 2 < RefectPoints.size()){
		if ((RefectPoints[k + 1].x != RefectPoints[k].x) && (RefectPoints[k + 1].x != RefectPoints[k + 2].x))
		{
			m1 = (RefectPoints[k + 1].y - RefectPoints[k].y) / (RefectPoints[k + 1].x - RefectPoints[k].x);
			m2 = (RefectPoints[k + 1].y - RefectPoints[k + 2].y) / (RefectPoints[k + 1].x - RefectPoints[k + 2].x);
			x_y1 = m1*(reflectmin_x - RefectPoints[k + 1].x) + RefectPoints[k + 1].y;
			x_y2 = m2*(reflectmin_x - RefectPoints[k + 1].x) + RefectPoints[k + 1].y;
			x_y3 = m1*(reflectmax_x - RefectPoints[k + 1].x) + RefectPoints[k + 1].y;
			x_y4 = m2*(reflectmax_x - RefectPoints[k + 1].x) + RefectPoints[k + 1].y;
			start_min = x_y1 > x_y2 ? (x_y2) : (x_y1);
			end_min = x_y1 < x_y2 ? (x_y2) : (x_y1);
			start_max = x_y3 < x_y4 ? x_y3 : x_y4;
			end_max = x_y3 > x_y4 ? x_y3 : x_y4;
			distance = (end_min - start_min) / 10;
			while (start_min < end_min){
				float  mp = (start_min - RefectPoints[k + 1].y) / (reflectmin_x - RefectPoints[k + 1].x);
				float  yp = mp*(reflectmax_x - RefectPoints[k + 1].x) + RefectPoints[k + 1].y;
				VC = start_min *start_min + start_min*yp + yp*yp;
				if (VC < V)
				{
					V = VC;
					mMinRadius = start_min;
					mMaxRadius = yp;
				}
				start_min = start_min + distance;
			}
			distance = (end_max - start_max) / 10;
			while (start_max < end_max){
				float  mp = (start_max - RefectPoints[k + 1].y) / (reflectmax_x - RefectPoints[k + 1].x);
				float  yp = mp*(reflectmin_x - RefectPoints[k + 1].x) + RefectPoints[k + 1].y;
				VC = start_max*start_max + start_max*yp + yp*yp;
				if (VC < V)
				{
					V = VC;
					mMinRadius = yp;
					mMaxRadius = start_max;
				}
				start_max = start_max + distance;
			}
		}
		else
			if (RefectPoints[k + 1].x == RefectPoints[k + 2].x){
				m1 = (RefectPoints[k + 1].y - RefectPoints[k].y) / (RefectPoints[k + 1].x - RefectPoints[k].x);
				r1 = m1*(reflectmax_x - RefectPoints[k + 1].x) + RefectPoints[k + 1].y;
				R1 = m1*(reflectmin_x - RefectPoints[k + 1].x) + RefectPoints[k + 1].y;
				VC = r1*r1 + r1*R1 + R1*R1;
				if (VC < V)
				{
					V = VC;
					mMinRadius = r1;
					mMaxRadius = R1;
				}
			}
			else if (RefectPoints[k + 1].x == RefectPoints[k].x){
				m2 = (RefectPoints[k + 1].y - RefectPoints[k + 2].y) / (RefectPoints[k + 1].x - RefectPoints[k + 2].x);
				R2 = m2*(reflectmax_x - RefectPoints[k + 1].x) + RefectPoints[k + 1].y;
				r2 = m2*(reflectmin_x - RefectPoints[k + 1].x) + RefectPoints[k + 1].y;
				VC = r2*r2 + r2*R2 + R2*R2;
				if (VC < V)
				{
					V = VC;
					mMinRadius = r2;
					mMaxRadius = R2;
				}
			}

	}
}

//////////////////////////////////////////////////////////////////////////
//@intro	����Բ̨��ʽչ��Slice
//@param	baseY:չ����õ���Slice���µ���
//@return	baseY:չ����õ���Slice���϶���
//@return	leftPoint:չ����õ���Slice������е�����
//@return	middlePoint:չ����õ���Slice���м��е�����
//@return	rightPoint:չ����õ���Slice���Ҳ��е�����
//@return	vector<PlaneFace>:չ�����������Ƭ����
//  [4/17/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
vector<PlaneFace> GCSlice::EnrollAsConicalFrustum(float &baseY,PlanePoint &leftPoint,PlanePoint &middlePoint,PlanePoint &rightPoint,
	float &tempMinRadius,float &tempMaxRadius,float &radiusThres,PlanePoint &centerPoint){
	float h1 = Distance(mLowerPoint, mUpperPoint);
	mMinRadius = mLowerRadius < mHigherRadius ? mLowerRadius : mHigherRadius;
	mMaxRadius = mLowerRadius > mHigherRadius ? mLowerRadius : mHigherRadius;

	H = (mMinRadius * h1 ) / (mMaxRadius - mMinRadius) + h1;

	if(mLowerRadius > mHigherRadius){
		mFlag = true;
		mHeight = mLowerPoint.GetY() + H;
	}else{
		mFlag = false;
		mHeight = mUpperPoint.GetY() - H;
	}
	vector<PlaneFace> enrolledFaces;
	for(auto i = 0; i < mFaces.size();i++){
		PlaneFace tempEnrolledTriangle = ExpandAsConicalFrustum(mFaces[i],mHeight,mMaxRadius,H,mFlag);
		enrolledFaces.push_back(tempEnrolledTriangle);
	}
	float minY = 1000, maxY = -1000;
	for(int i = 0; i < enrolledFaces.size();i++){
		PlaneFace tempFace = enrolledFaces[i];
		minY = minY < tempFace.mVertexA.mPy ? minY : tempFace.mVertexA.mPy;
		minY = minY < tempFace.mVertexB.mPy ? minY : tempFace.mVertexB.mPy;
		minY = minY < tempFace.mVertexC.mPy ? minY : tempFace.mVertexC.mPy;
		maxY = maxY > tempFace.mVertexA.mPy ? maxY : tempFace.mVertexA.mPy;
		maxY = maxY > tempFace.mVertexB.mPy ? maxY : tempFace.mVertexB.mPy;
		maxY = maxY > tempFace.mVertexC.mPy ? maxY : tempFace.mVertexC.mPy;
	}
	float addedY = minY < baseY ? baseY - minY : 0;
	baseY = maxY + addedY;
	for(int i = 0 ;i < enrolledFaces.size();i++){
		enrolledFaces[i].Translate(0,addedY);
	}
	float alpha = M_PI * mMaxRadius / sqrt(H * H + mMaxRadius * mMaxRadius);
	middlePoint = PlanePoint(0, (mLowerPoint.GetY() + mUpperPoint.GetY()) / 2);
	tempMaxRadius = sqrt(mMaxRadius * mMaxRadius + H * H);
	tempMinRadius = mMinRadius / mMaxRadius * tempMaxRadius;
	float tempMiddleRadius = (mMinRadius + mMaxRadius) / (2 * mMaxRadius) * tempMaxRadius;
	if(mFlag){
		rightPoint = PlanePoint(tempMiddleRadius * sin(alpha), mHeight - tempMiddleRadius * cos(alpha) + addedY);
		leftPoint = PlanePoint(-tempMiddleRadius * sin(alpha), mHeight - tempMiddleRadius * cos(alpha) + addedY);
		centerPoint = PlanePoint(0, mHeight + addedY);
	}else{
		rightPoint = PlanePoint(tempMiddleRadius * sin(alpha), mHeight + tempMiddleRadius * cos(alpha) + addedY);
		leftPoint = PlanePoint(-tempMiddleRadius * sin(alpha), mHeight + tempMiddleRadius * cos(alpha) + addedY);
		centerPoint = PlanePoint(0, mHeight + addedY);
	}
	radiusThres = tempMaxRadius - tempMinRadius;
	return enrolledFaces;
}

//////////////////////////////////////////////////////////////////////////
//@intro	����Բ̨��ʽչ��������Ƭ �õ�չ�����������Ƭ
//@param	face:������Ƭ
//@param	height:��Բ̨����Բ׶��Բ׶�ĸ߶�
//@param	R:Բ̨���µ���뾶���ϴ�ĵ���뾶��
//@param	H:Բ̨�ĸ߶�
//@param	flag:Բ̨�������Ļ��ǵ�����
//@return	PlaneTriangle��չ�����������Ƭ
//  [4/17/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
PlaneFace GCSlice::ExpandAsConicalFrustum(Face face, float height, float R, float H, bool flag){
	float x1=face.GetVertexA().GetX();
	float y1=face.GetVertexA().GetY();
	float z1=face.GetVertexA().GetZ();
	float u1=face.GetTextureA().GetX();
	float v1=face.GetTextureA().GetY();

	float x2=face.GetVertexB().GetX();
	float y2=face.GetVertexB().GetY();
	float z2=face.GetVertexB().GetZ();
	float u2=face.GetTextureB().GetX();
	float v2=face.GetTextureB().GetY();

	float x3=face.GetVertexC().GetX();
	float y3=face.GetVertexC().GetY();
	float z3=face.GetVertexC().GetZ();
	float u3=face.GetTextureC().GetX();
	float v3=face.GetTextureC().GetY();

	float  X1,X2,X3;           
	float  Y1,Y2,Y3;           
	float h1 =0;
	float h2 =0 ;
	float h3 =0 ;
	float r1=0;
	float r2=0;
	float r3=0;
	float angle1 = 0.0,angle2 = 0.0,angle3 = 0.0;
	X1=X2=X3=Y1=Y2=Y3=0;
	bool isFacePositive = face.GetPositive();//�ж�������Ƭ��λ��yozƽ���ұ߻������
	//�����ǰslice����Բ̨
	if(flag==1)	//	��ǰ����ļ��㷽��Ĭ����άģ�͵ĶԳ�����ƽ����y���
	{
		h1 =height - y1 ;
		h2 =height - y2 ;
		h3 =height - y3 ;
		r1 = sqrt(x1 * x1 + z1 * z1);
		r2 = sqrt(x2 * x2 + z2 * z2);
		r3 = sqrt(x3 * x3 + z3 * z3);
		float aa,bb,cc;
		aa = z1 / r1;
		bb = z2 / r2;
		cc = z3 / r3;
		if(isFacePositive){
			X1 =          sqrt(r1 * r1 + h1 * h1) * sin(M_PI * R / sqrt(H * H + R * R) - (r1 * acos(aa)) / sqrt(r1 * r1 + h1 * h1) );
			Y1 = height - sqrt(r1 * r1 + h1 * h1) * cos(M_PI * R / sqrt(H * H + R * R) - (r1 * acos(aa)) / sqrt(r1 * r1 + h1 * h1) );
			X2 =          sqrt(r2 * r2 + h2 * h2) * sin(M_PI * R / sqrt(H * H + R * R) - (r2 * acos(bb)) / sqrt(r2 * r2 + h2 * h2) );
			Y2 = height - sqrt(r2 * r2 + h2 * h2) * cos(M_PI * R / sqrt(H * H + R * R) - (r2 * acos(bb)) / sqrt(r2 * r2 + h2 * h2) );
			X3 =          sqrt(r3 * r3 + h3 * h3) * sin(M_PI * R / sqrt(H * H + R * R) - (r3 * acos(cc)) / sqrt(r3 * r3 + h3 * h3) );
			Y3 = height - sqrt(r3 * r3 + h3 * h3) * cos(M_PI * R / sqrt(H * H + R * R) - (r3 * acos(cc)) / sqrt(r3 * r3 + h3 * h3) );
		}else{
			X1 =          sqrt(r1 * r1 + h1 * h1) * sin(M_PI * R / sqrt(H * H + R * R) - r1 * (acos((-1)*aa) + M_PI) / sqrt(r1 * r1 + h1 * h1) );
			Y1 = height - sqrt(r1 * r1 + h1 * h1) * cos(M_PI * R / sqrt(H * H + R * R) - r1 * (acos((-1)*aa) + M_PI) / sqrt(r1 * r1 + h1 * h1) );
			X2 =          sqrt(r2 * r2 + h2 * h2) * sin(M_PI * R / sqrt(H * H + R * R) - r2 * (acos((-1)*bb) + M_PI) / sqrt(r2 * r2 + h2 * h2) );
			Y2 = height - sqrt(r2 * r2 + h2 * h2) * cos(M_PI * R / sqrt(H * H + R * R) - r2 * (acos((-1)*bb) + M_PI) / sqrt(r2 * r2 + h2 * h2) );
			X3 =          sqrt(r3 * r3 + h3 * h3) * sin(M_PI * R / sqrt(H * H + R * R) - r3 * (acos((-1)*cc) + M_PI) / sqrt(r3 * r3 + h3 * h3) );
			Y3 = height - sqrt(r3 * r3 + h3 * h3) * cos(M_PI * R / sqrt(H * H + R * R) - r3 * (acos((-1)*cc) + M_PI) / sqrt(r3 * r3 + h3 * h3) );
		}
	}
	else	//��ǰslice�ǵ�Բ̨
	{		
		h1 =y1 -height;
		h2 =y2 -height;
		h3 =y3 -height;
		/*r1=h1*R/H;
		r2=h2*R/H;
		r3=h3*R/H;*/
		r1 = sqrt(x1 * x1 + z1 * z1);
		r2 = sqrt(x2 * x2 + z2 * z2);
		r3 = sqrt(x3 * x3 + z3 * z3);
		float aa,bb,cc;
		aa=z1/r1;
		bb=z2/r2;
		cc=z3/r3;
		/*	aa = putNumInRange(aa,-1,1);
		bb = putNumInRange(bb,-1,1);
		cc = putNumInRange(cc,-1,1);*/
		//����������Ƭλ��yozƽ����Ҳ��������չ�����λ��
		if(isFacePositive){
			X1 =		  sqrt(r1 * r1 + h1 * h1) * sin(M_PI * R / sqrt(H * H + R * R) - (r1 * acos(aa)) / sqrt(r1 * r1 + h1 * h1) );
			Y1 = height + sqrt(r1 * r1 + h1 * h1) * cos(M_PI * R / sqrt(H * H + R * R) - (r1 * acos(aa)) / sqrt(r1 * r1 + h1 * h1) );
			X2 =          sqrt(r2 * r2 + h2 * h2) * sin(M_PI * R / sqrt(H * H + R * R) - (r2 * acos(bb)) / sqrt(r2 * r2 + h2 * h2) );
			Y2 = height + sqrt(r2 * r2 + h2 * h2) * cos(M_PI * R / sqrt(H * H + R * R) - (r2 * acos(bb)) / sqrt(r2 * r2 + h2 * h2) );
			X3 =          sqrt(r3 * r3 + h3 * h3) * sin(M_PI * R / sqrt(H * H + R * R) - (r3 * acos(cc)) / sqrt(r3 * r3 + h3 * h3) );
			Y3 = height + sqrt(r3 * r3 + h3 * h3) * cos(M_PI * R / sqrt(H * H + R * R) - (r3 * acos(cc)) / sqrt(r3 * r3 + h3 * h3) );
		}else{
			X1 =          sqrt(r1 * r1 + h1 * h1) * sin(M_PI * R / sqrt(H * H + R * R) - r1 * (acos((-1)*aa) + M_PI) / sqrt(r1 * r1 + h1 * h1) );
			Y1 = height + sqrt(r1 * r1 + h1 * h1) * cos(M_PI * R / sqrt(H * H + R * R) - r1 * (acos((-1)*aa) + M_PI) / sqrt(r1 * r1 + h1 * h1) );
			X2 =          sqrt(r2 * r2 + h2 * h2) * sin(M_PI * R / sqrt(H * H + R * R) - r2 * (acos((-1)*bb) + M_PI) / sqrt(r2 * r2 + h2 * h2) );
			Y2 = height + sqrt(r2 * r2 + h2 * h2) * cos(M_PI * R / sqrt(H * H + R * R) - r2 * (acos((-1)*bb) + M_PI) / sqrt(r2 * r2 + h2 * h2) );
			X3 =		  sqrt(r3 * r3 + h3 * h3) * sin(M_PI * R / sqrt(H * H + R * R) - r3 * (acos((-1)*cc) + M_PI) / sqrt(r3 * r3 + h3 * h3) );
			Y3 = height + sqrt(r3 * r3 + h3 * h3) * cos(M_PI * R / sqrt(H * H + R * R) - r3 * (acos((-1)*cc) + M_PI) / sqrt(r3 * r3 + h3 * h3) );
		}
		
	}

	if (h1 ==0)
	{
		X1 =0;
		Y1 =0;
	} 
	if (h2 ==0)
	{
		X2 =0;
		Y2 =0;
	} 
	if (h3 ==0)
	{
		X3 =0;
		Y3 =0;
	}
	PlanePoint pointA = PlanePoint(X1, Y1);
	PlanePoint pointB = PlanePoint(X2, Y2);
	PlanePoint pointC = PlanePoint(X3, Y3);
	Texture textureA = face.GetTextureA();
	Texture textureB = face.GetTextureB();
	Texture textureC = face.GetTextureC();
	return PlaneFace(pointA,pointB,pointC,textureA,textureB,textureC);
}


//////////////////////////////////////////////////////////////////////////
//@intro	����ֵ�޶��ڷ�Χ[min,max]��
//@param	num:Ҫ�����޶�����ֵ
//@param	min,max:�ֱ�Ϊ�޶���Χ���½���Ͻ�
//@return	�޶���Χ��õ�����ֵ
//////////////////////////////////////////////////////////////////////////
float putNumInRange(float num,float min,float max){
	if(num < min){
		num = min;
	}
	else if(num > max){
		num  = max;
	}
	return num;
}

//////////////////////////////////////////////////////////////////////////
//@intro	��ȡ������Ƭ��
//////////////////////////////////////////////////////////////////////////
int GCSlice::GetFacesSize(){
	return mFaces.size();
}


//////////////////////////////////////////////////////////////////////////
//@intro	�ж�Բ̨�Ƿ���������
//////////////////////////////////////////////////////////////////////////
bool GCSlice::IsRight(){
	if(mLowerRadius >= mHigherRadius){
		return true;
	}
	return false;
}


//////////////////////////////////////////////////////////////////////////
//@intro	��ȡ��ǰslice������������Ƭ
//////////////////////////////////////////////////////////////////////////
vector<Face> GCSlice::GetFaces(){
	return mFaces;
}

//////////////////////////////////////////////////////////////////////////
//@intro	չ��Slice,�õ�չ�����ƽ��������Ƭ����
//@return	EnrolledSlice:չ�����Slice
//  [4/17/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
EnrolledSlice GCSlice::EnrollSlice(float& baseY){
	PlanePoint leftPoint,middlePoint,rightPoint;
	vector<PlaneFace> enrolledFaces;
	float minRadius,maxRadius,radiusThres;
	PlanePoint centerPoint;
	if(mEnrollMode == EnrollMode::Cylinder){
		enrolledFaces = EnrollAsCylinderSegment(baseY,leftPoint,middlePoint,rightPoint,minRadius,maxRadius,radiusThres,centerPoint);
		return EnrolledSlice(enrolledFaces,leftPoint,middlePoint,rightPoint,minRadius,maxRadius,radiusThres,centerPoint,EnrollMode::Cylinder);
	}else{
		enrolledFaces = EnrollAsConicalFrustum(baseY,leftPoint,middlePoint,rightPoint,minRadius,maxRadius,radiusThres,centerPoint);
		return EnrolledSlice(enrolledFaces,leftPoint,middlePoint,rightPoint,minRadius,maxRadius,radiusThres,centerPoint,EnrollMode::ConicalFrustum);

	}
	
}



//////////////////////////////////////////////////////////////////////////
//@intro	����㵽�Գ���ľ���
//@param	vertex:��ǰ�������
//@param	axisNormal:�Գ�������
//  [4/12/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
float GCSlice::Vertex2Axis(Vertex curPoint,Normal axisNormal){
	Normal vertexNormal = Normal(curPoint.GetX(), curPoint.GetY(), curPoint.GetZ());

	Normal crossProduct = vertexNormal.Cross(axisNormal);
	return crossProduct.GetLength() / axisNormal.GetLength();
}


//////////////////////////////////////////////////////////////////////////
//@intro	���Բ�������ҵ�����Գ�����Զ�ľ���
//@return	�������Բ���ĵ���뾶
//////////////////////////////////////////////////////////////////////////
float GCSlice::CylinderSegmentFitting(){
	float minRadius = 1000, maxRadius = -1000;
	for(auto i = 0; i < mFaces.size();i++){
		float tempRadius1 = Vertex2Axis(mFaces[i].GetVertexA(), mAxisNormal);
		minRadius = minRadius < tempRadius1 ? minRadius : tempRadius1;
		maxRadius = maxRadius > tempRadius1 ? maxRadius : tempRadius1;

		float tempRadius2 = Vertex2Axis(mFaces[i].GetVertexB(), mAxisNormal);
		minRadius = minRadius < tempRadius2 ? minRadius : tempRadius2;
		maxRadius = maxRadius > tempRadius2 ? maxRadius : tempRadius2;

		float tempRadius3 = Vertex2Axis(mFaces[i].GetVertexC(), mAxisNormal);
		minRadius = minRadius < tempRadius3 ? minRadius : tempRadius3;
		maxRadius = maxRadius > tempRadius3 ? maxRadius : tempRadius3;
	}

	return maxRadius;
}

//////////////////////////////////////////////////////////////////////////
//@intro	����Բ̨��ʽչ��������Ƭ���õ�չ�����ƽ�������μ���
//@param	baseY:Բ̨���µ���
//@param	leftPoint:չ�����Slice������е�
//@param	middlePoint:չ�����Slice���м��е�
//@param	rightPoint:չ�����Slice���ұ��е�
//  [4/17/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
vector<PlaneFace> GCSlice::EnrollAsCylinderSegment(float &baseY,PlanePoint &leftPoint,PlanePoint &middlePoint,PlanePoint &rightPoint,
	float &minRadius,float &maxRadius,float &radiusThres,PlanePoint &centerPoint){
	float radius = CylinderSegmentFitting();
	float addedY = mLowerPoint.GetY() < baseY ? baseY - mLowerPoint.GetY() : 0;
	baseY = mUpperPoint.GetY() + addedY;
	vector<PlaneFace> enrolledFaces;
	for(auto i = 0; i < mFaces.size();i++){
		PlaneFace tempEnrolledFace = ExpandAsCylinderSegment(mFaces[i],radius,addedY);
		enrolledFaces.push_back(tempEnrolledFace);
	}
	radiusThres = fabs(mUpperPoint.GetY()-mLowerPoint.GetY());
	middlePoint = PlanePoint(0, (mUpperPoint.GetY() + mLowerPoint.GetY()) / 2);
	leftPoint = PlanePoint(-M_PI * radius,(mUpperPoint.GetY() + mLowerPoint.GetY()) / 2);
	rightPoint = PlanePoint(M_PI * radius, (mUpperPoint.GetY() + mLowerPoint.GetY()) / 2);
	minRadius = maxRadius = radius;
	centerPoint = middlePoint;
	return enrolledFaces;
}

//////////////////////////////////////////////////////////////////////////
//@intro	����Բ����ʽչ��������Ƭ���õ�չ�����������Ƭ
//@param	face:������Ƭ
//@param	R:Բ���İ뾶
//@param	addedY:���ӵ�Y������Ϊ�˲�ͬ��չ�����Slice���غ�
//@return	PlaneTriangle:չ�����������Ƭ
//  [4/17/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
PlaneFace GCSlice::ExpandAsCylinderSegment(Face face,float R,float addedY){
	float x1=face.GetVertexA().GetX();
	float y1=face.GetVertexA().GetY();
	float z1=face.GetVertexA().GetZ();
	float u1=face.GetTextureA().GetX();
	float v1=face.GetTextureA().GetY();

	float x2=face.GetVertexB().GetX();
	float y2=face.GetVertexB().GetY();
	float z2=face.GetVertexB().GetZ();
	float u2=face.GetTextureB().GetX();
	float v2=face.GetTextureB().GetY();

	float x3=face.GetVertexC().GetX();
	float y3=face.GetVertexC().GetY();
	float z3=face.GetVertexC().GetZ();
	float u3=face.GetTextureC().GetX();
	float v3=face.GetTextureC().GetY();

	float X1 = 0, X2 = 0, X3 = 0;
	float Y1 = 0, Y2 = 0, Y3 = 0;

	bool isFacePositive = face.GetPositive();	//�ж�������Ƭ�Ƿ�λ��yozƽ���Ҳ�

	float cosA = z1 / sqrt(x1 * x1 + z1 * z1);
	float cosB = z2 / sqrt(x2 * x2 + z2 * z2);
	float cosC = z3 / sqrt(x3 * x3 + z3 * z3);

	if(isFacePositive){
		X1 = acos(cosA) * R;
		X2 = acos(cosB) * R;
		X3 = acos(cosC) * R;
	}
	else{
		X1 = -acos(cosA)  * R;
		X2 = -acos(cosB)  * R;
		X3 = -acos(cosC)  * R;
	}
	Y1 = y1;
	Y2 = y2;
	Y3 = y3;

	PlanePoint pointA = PlanePoint(X1, Y1 + addedY);
	PlanePoint pointB = PlanePoint(X2, Y2 + addedY);
	PlanePoint pointC = PlanePoint(X3, Y3 + addedY);
	Texture textureA = face.GetTextureA();
	Texture textureB = face.GetTextureB();
	Texture textureC = face.GetTextureC();
	return PlaneFace(pointA, pointB,pointC,textureA,textureB,textureC);
}