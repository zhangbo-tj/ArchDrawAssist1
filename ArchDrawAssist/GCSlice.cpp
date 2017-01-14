#include "StdAfx.h"
#include "GCSlice.h"

//////////////////////////////////////////////////////////////////////////
//@intro	初始化GCSlice
//@param	axisnormal:三维模型的对称轴的法向量
//@param	lowerpoint:slice下截面上的点
//@param	higherpoint:slice上截面上的点
//@param	lowrRadius:slice下截面的半径
//@param	highRadius：slice上截面的半径
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
//@intro	判断三角面片是否位于slice区间内，即位于该slice的上下截面中间
//@param	face:三角面片
//@return	是否在区间内，在区间内则返回true，不在区间内则返回false
//////////////////////////////////////////////////////////////////////////
bool GCSlice::IsContainFace(Face face){
	Vertex vertexA = face.GetVertexA();
	Vertex vertexB = face.GetVertexB();
	Vertex vertexC = face.GetVertexC();
	
	//分别计算slice的上下横截面	
	C3DCircleExtract lowerPlane,higherPlane;
	lowerPlane.setPlane(mLowerPoint,mAxisNormal);
	higherPlane.setPlane(mUpperPoint,mAxisNormal);
	bool higherThanLow = false, lowerThanUpper = false;

	//判断三角面片是否在slice下截面上方
	if(IsFaceUpperPlane(face,lowerPlane.abcd,lowerPlane.center) == 1){
		higherThanLow = true;
	}
	else{
		return false;
	}
	//判断三角面片是否位于slice上截面下方
	if(IsFaceUpperPlane(face,higherPlane.abcd,higherPlane.center) == -1){
		lowerThanUpper = true;
	}
	else{
		return false;
	}

	//如果三角面片同时位于下截面上方和上截面下方则返回true,否则返回false
	if( higherThanLow && lowerThanUpper){
		return true;
	}
	return false;
}



//////////////////////////////////////////////////////////////////////////
//@intro	将三角面片添加到slice内
//@param	face：三角面片
//////////////////////////////////////////////////////////////////////////
void GCSlice::AddFace(Face face){
	mFaces.push_back(face);
}


//////////////////////////////////////////////////////////////////////////
//@intro	拟合圆台
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
//@intro	按照圆台方式展开Slice
//@param	baseY:展开后得到的Slice的下底面
//@return	baseY:展开后得到的Slice的上顶面
//@return	leftPoint:展开后得到的Slice的左侧中点坐标
//@return	middlePoint:展开后得到的Slice的中间中点坐标
//@return	rightPoint:展开后得到的Slice的右侧中点坐标
//@return	vector<PlaneFace>:展开后的三角面片集合
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
//@intro	按照圆台方式展开三角面片 得到展开后的三角面片
//@param	face:三角面片
//@param	height:将圆台看做圆锥后圆锥的高度
//@param	R:圆台的下底面半径（较大的底面半径）
//@param	H:圆台的高度
//@param	flag:圆台是正立的还是倒立的
//@return	PlaneTriangle：展开后的三角面片
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
	bool isFacePositive = face.GetPositive();//判断三角面片是位于yoz平面右边还是左边
	//如果当前slice是正圆台
	if(flag==1)	//	当前代码的计算方法默认三维模型的对称轴是平行于y轴的
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
	else	//当前slice是倒圆台
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
		//根据三角面片位于yoz平面的右侧或左侧计算展开后的位置
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
//@intro	将数值限定在范围[min,max]内
//@param	num:要进行限定的数值
//@param	min,max:分别为限定范围的下界和上界
//@return	限定范围后得到的数值
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
//@intro	获取三角面片数
//////////////////////////////////////////////////////////////////////////
int GCSlice::GetFacesSize(){
	return mFaces.size();
}


//////////////////////////////////////////////////////////////////////////
//@intro	判断圆台是否是正立的
//////////////////////////////////////////////////////////////////////////
bool GCSlice::IsRight(){
	if(mLowerRadius >= mHigherRadius){
		return true;
	}
	return false;
}


//////////////////////////////////////////////////////////////////////////
//@intro	获取当前slice的所有三角面片
//////////////////////////////////////////////////////////////////////////
vector<Face> GCSlice::GetFaces(){
	return mFaces;
}

//////////////////////////////////////////////////////////////////////////
//@intro	展开Slice,得到展开后的平面三角面片集合
//@return	EnrolledSlice:展开后的Slice
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
//@intro	计算点到对称轴的距离
//@param	vertex:当前点的坐标
//@param	axisNormal:对称轴向量
//  [4/12/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
float GCSlice::Vertex2Axis(Vertex curPoint,Normal axisNormal){
	Normal vertexNormal = Normal(curPoint.GetX(), curPoint.GetY(), curPoint.GetZ());

	Normal crossProduct = vertexNormal.Cross(axisNormal);
	return crossProduct.GetLength() / axisNormal.GetLength();
}


//////////////////////////////////////////////////////////////////////////
//@intro	拟合圆柱，即找到距离对称轴最远的距离
//@return	返回拟合圆柱的底面半径
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
//@intro	按照圆台方式展开三角面片，得到展开后的平面三角形集合
//@param	baseY:圆台的下底面
//@param	leftPoint:展开后的Slice的左边中点
//@param	middlePoint:展开后的Slice的中间中点
//@param	rightPoint:展开后的Slice的右边中点
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
//@intro	按照圆柱方式展开三角面片，得到展开后的三角面片
//@param	face:三角面片
//@param	R:圆柱的半径
//@param	addedY:增加的Y分量，为了不同的展开后的Slice不重合
//@return	PlaneTriangle:展开后的三角面片
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

	bool isFacePositive = face.GetPositive();	//判断三角面片是否位于yoz平面右侧

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