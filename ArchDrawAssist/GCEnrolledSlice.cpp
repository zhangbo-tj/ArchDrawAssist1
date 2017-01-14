#include "StdAfx.h"
#include "GCEnrolledSlice.h"


//////////////////////////////////////////////////////////////////////////
//@intro	构造器，用于初始化展开后的Slice片段
//@param	tris:平面三角面片集合
//@param	leftPoint:Slice左侧中点坐标
//@param	middlePoint:Slice中间中点坐标
//@param	rightPoint:Slice右侧中点坐标
//  [4/17/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
EnrolledSlice::EnrolledSlice(vector<PlaneFace> faces,PlanePoint leftPoint,PlanePoint middlePoint, PlanePoint rightPoint,
	float minRadius, float maxRadius,float radiusThres,PlanePoint centerPoint,EnrollMode enrollMode){
	mPlaneFaces = faces;
	mLeftEndPoint = leftPoint;
	mMiddlePoint = middlePoint;
	mRightEndPoint = rightPoint;
	mMinRadius = minRadius;
	mMaxRadius = maxRadius;
	mMidRadius = (maxRadius + minRadius) / 2;
	mCenterPoint = centerPoint;
	mRadiusThreshold = radiusThres;
	mIsSelected = false;
	mIsLeftSelected = mIsRightSelected = false;
	mEnrollMode = enrollMode;
	ComputeRelativeParameters(mProportions,mDistances);
	UpdateBoudnary();	//更新展开后的边界
}

//////////////////////////////////////////////////////////////////////////
//@intro	绘制展开后的Slice
//  [4/17/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
void EnrolledSlice::DrawSlice(){
	if(mIsSelected){
		ShowBoundary();
		ShowControlPoints();
	}
	else{

	}
	glColor3f(1,1,1);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LINE_STIPPLE);
	glBegin(GL_TRIANGLES);
		for(int i = 0; i < mPlaneFaces.size();i++){
			PlaneFace tri = mPlaneFaces[i];
			glTexCoord2fv(tri.mTextureA.ToFloat());
			glVertex2fv(tri.mVertexA.ToFloat());
			glTexCoord2fv(tri.mTextureB.ToFloat());
			glVertex2fv(tri.mVertexB.ToFloat());
			glTexCoord2fv(tri.mTextureC.ToFloat());
			glVertex2fv(tri.mVertexC.ToFloat());
		}
	glEnd();
	
}


//////////////////////////////////////////////////////////////////////////
//@intro	更新当前展开后的Slice的上下边界
//@return	void
//  [4/17/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
void EnrolledSlice::UpdateBoudnary(){
	float minPt[2], maxPt[2];
	minPt[0] = minPt[1] = 100;
	maxPt[0] = maxPt[1] = -100;
	for(auto i = 0; i < mPlaneFaces.size();i++){
		PlaneFace tempPlaneFace = mPlaneFaces[i];
		minPt[0] = minPt[0] < tempPlaneFace.mVertexA.mPx ? minPt[0] : tempPlaneFace.mVertexA.mPx;
		minPt[0] = minPt[0] < tempPlaneFace.mVertexB.mPx ? minPt[0] : tempPlaneFace.mVertexB.mPx;
		minPt[0] = minPt[0] < tempPlaneFace.mVertexC.mPx ? minPt[0] : tempPlaneFace.mVertexC.mPx;

		minPt[1] = minPt[1] < tempPlaneFace.mVertexA.mPy ? minPt[1] : tempPlaneFace.mVertexA.mPy;
		minPt[1] = minPt[1] < tempPlaneFace.mVertexB.mPy ? minPt[1] : tempPlaneFace.mVertexB.mPy;
		minPt[1] = minPt[1] < tempPlaneFace.mVertexC.mPy ? minPt[1] : tempPlaneFace.mVertexC.mPy;

		maxPt[0] = maxPt[0] > tempPlaneFace.mVertexA.mPx ? maxPt[0] : tempPlaneFace.mVertexA.mPx;
		maxPt[0] = maxPt[0] > tempPlaneFace.mVertexB.mPx ? maxPt[0] : tempPlaneFace.mVertexB.mPx;
		maxPt[0] = maxPt[0] > tempPlaneFace.mVertexC.mPx ? maxPt[0] : tempPlaneFace.mVertexC.mPx;

		maxPt[1] = maxPt[1] > tempPlaneFace.mVertexA.mPy ? maxPt[1] : tempPlaneFace.mVertexA.mPy;
		maxPt[1] = maxPt[1] > tempPlaneFace.mVertexB.mPy ? maxPt[1] : tempPlaneFace.mVertexB.mPy;
		maxPt[1] = maxPt[1] > tempPlaneFace.mVertexC.mPy ? maxPt[1] : tempPlaneFace.mVertexC.mPy;
	}
	mMinPt[0] = minPt[0];	mMinPt[1] = minPt[1];
	mMaxPt[0] = maxPt[0];	mMaxPt[1] = maxPt[1];
}


//////////////////////////////////////////////////////////////////////////
//@intro	画出展开Slice的边界
//@return	void
//  [4/18/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
void EnrolledSlice::ShowBoundary(){
	PlaneVector middle2Right(mMiddlePoint,mRightEndPoint);
	PlaneVector yAxis(0,1);
	float dotProduct = middle2Right.DotProduct(yAxis) / (middle2Right.Length() * yAxis.Length());
	glColor3f(0,1,0);		//设定边框颜色为绿色
	//设置线段类型为虚线
	glLineStipple(2, 0x5555);
	glEnable(GL_LINE_STIPPLE);
	//展开方式为圆柱
	if(mEnrollMode == EnrollMode::Cylinder){
		glBegin(GL_LINE_LOOP);
			glVertex2f(mMinPt[0],mMinPt[1]);
			glVertex2f(mMinPt[0],mMaxPt[1]);
			glVertex2f(mMaxPt[0],mMaxPt[1]);
			glVertex2f(mMaxPt[0],mMinPt[1]);
		glEnd();
	}else{
		//展开方式为圆台
		bool rightFlag = mCenterPoint.mPy > mMiddlePoint.mPy ? true : false;	//判断圆台是正立或者倒立
		PlaneVector center2Middle(mCenterPoint,mMiddlePoint);
		PlaneVector center2Right(mCenterPoint,mRightEndPoint);
		
		float cosAlpha = center2Middle.DotProduct(center2Right) / (center2Middle.Length() * center2Right.Length());
		//圆台是正立的
		if(rightFlag){
			//DrawArc(0,0,0,M_PI,10);
			DrawArc(mCenterPoint.mPx,mCenterPoint.mPy,M_PI * 3 / 2 - acos(cosAlpha), M_PI * 3 / 2 + acos(cosAlpha), mMaxRadius);
			DrawArc(mCenterPoint.mPx,mCenterPoint.mPy,M_PI * 3 / 2 - acos(cosAlpha), M_PI * 3 / 2 + acos(cosAlpha), mMinRadius);
			DrawArc(mCenterPoint.mPx,mCenterPoint.mPy,M_PI * 3 / 2 - acos(cosAlpha), M_PI * 3 / 2 + acos(cosAlpha), mMidRadius);

			glBegin(GL_LINES);
				glVertex2f(mMaxRadius*sqrt(1- cosAlpha * cosAlpha), mCenterPoint.mPy - mMaxRadius * cosAlpha);
				glVertex2f(mMinRadius*sqrt(1- cosAlpha * cosAlpha), mCenterPoint.mPy - mMinRadius * cosAlpha);

				glVertex2f(-mMaxRadius*sqrt(1- cosAlpha * cosAlpha), mCenterPoint.mPy - mMaxRadius * cosAlpha);
				glVertex2f(-mMinRadius*sqrt(1- cosAlpha * cosAlpha), mCenterPoint.mPy - mMinRadius * cosAlpha);
			glEnd();
		}
		//圆台是倒立的
		else{
			DrawArc(mCenterPoint.mPx,mCenterPoint.mPy,M_PI / 2 - acos(cosAlpha), M_PI / 2 + acos(cosAlpha), mMaxRadius);
			DrawArc(mCenterPoint.mPx,mCenterPoint.mPy,M_PI / 2 - acos(cosAlpha), M_PI / 2 + acos(cosAlpha), mMinRadius);
			glBegin(GL_LINES);
				glVertex2f(mMaxRadius*sqrt(1- cosAlpha * cosAlpha), mCenterPoint.mPy + mMaxRadius * cosAlpha);
				glVertex2f(mMinRadius*sqrt(1- cosAlpha * cosAlpha), mCenterPoint.mPy + mMinRadius * cosAlpha);

				glVertex2f(-mMaxRadius*sqrt(1- cosAlpha * cosAlpha), mCenterPoint.mPy + mMaxRadius * cosAlpha);
				glVertex2f(-mMinRadius*sqrt(1- cosAlpha * cosAlpha), mCenterPoint.mPy + mMinRadius * cosAlpha);
			glEnd();
		}

		
	}
}

//////////////////////////////////////////////////////////////////////////
//@intro	画出Slice的控制点
//////////////////////////////////////////////////////////////////////////
void EnrolledSlice::ShowControlPoints(){
	//glColor3f(1,0,0);
	Draw_solid_circle_red(mLeftEndPoint.mPx, mLeftEndPoint.mPy, mRadiusThreshold / 10);
	Draw_solid_circle_red(mRightEndPoint.mPx, mRightEndPoint.mPy, mRadiusThreshold / 10);
	Draw_solid_circle_red(mMiddlePoint.mPx, mMiddlePoint.mPy, mRadiusThreshold / 10);
	if(mEnrollMode == EnrollMode::ConicalFrustum && (mIsLeftSelected || mIsRightSelected)){
		glColor3f(0,1,0);
		glBegin(GL_LINE_LOOP);
		glVertex2fv(mLeftEndPoint.ToFloat());
		glVertex2fv(mMiddlePoint.ToFloat());
		glVertex2fv(mRightEndPoint.ToFloat());
		glVertex2fv(mCenterPoint.ToFloat());

		glVertex2fv(mCenterPoint.ToFloat());
		glVertex2fv(mRightEndPoint.ToFloat());
		glVertex2fv(mLeftEndPoint.ToFloat());
		glVertex2fv(mMiddlePoint.ToFloat());
		glVertex2fv(mMiddlePoint.ToFloat());
		glVertex2fv(mCenterPoint.ToFloat());
		glEnd();
	}
}

//////////////////////////////////////////////////////////////////////////
//@intro	画出特定位置和半径的圆弧,主要用于画出圆环的边界
//@param	centerX,centerY:圆的圆心位置
//@param	startAngle:圆弧的开始角度
//@param	endAngle：圆弧的结束角度
//@param	radius:圆弧的半径
//  [4/18/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
void EnrolledSlice::DrawArc(float centerX,float centerY,float startAngle,float endAngle,float radius){
	glColor3f(0,1,0);
	glLineStipple(2, 0x5555);
	glEnable(GL_LINE_STIPPLE);
	float deltaAngle = M_PI / 360;
	glBegin(GL_LINE_STRIP);
	for(float i = startAngle; i <= endAngle; i+=deltaAngle){
		float vx = centerX + radius * cos(i);
		float vy = centerY + radius * sin(i);
		glVertex2f(vx, vy);
	}
	glEnd();
}


//////////////////////////////////////////////////////////////////////////
//@intro	判断点当前点是否在EnrolledSlice内
//@param	px,py:当前点的坐标
//@return	bool:当前点当前Slice内则返回true,否则返回false
//  [4/18/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
bool EnrolledSlice::IsPointInSlice(float px,float py){
	PlaneVector middle2Right(mMiddlePoint,mRightEndPoint);
	PlaneVector yAxis(0,1);
	float cosAlpha = middle2Right.DotProduct(yAxis) / (middle2Right.Length() * yAxis.Length());
	//按照圆柱方式展开
	if(mEnrollMode == EnrollMode::Cylinder){
		if(px >= mMinPt[0] && px <= mMaxPt[0] && py >= mMinPt[1] && py <= mMaxPt[1]){
			return true;
		}else{
			return false;
		}
	}
	//按照圆台方式展开
	else{
		float dis2Center = sqrt((px - mCenterPoint.mPx) * (px - mCenterPoint.mPx) + (py - mCenterPoint.mPy) * (py - mCenterPoint.mPy));
		if(dis2Center >= mMinRadius && dis2Center <= mMaxRadius){
			PlaneVector center2Middle(mCenterPoint, mMiddlePoint);
			PlaneVector center2Right(mCenterPoint, mRightEndPoint);
			PlaneVector center2CurPoint(mCenterPoint,PlanePoint(px,py));
			float cosMaxAngle = center2Middle.DotProduct(center2Right) / (center2Middle.Length() * center2Right.Length());
			float cosCurAngle = center2Middle.DotProduct(center2CurPoint) / (center2Middle.Length() * center2CurPoint.Length());
			if(cosCurAngle >= cosMaxAngle){
				return true;
			}else{
				return false;
			}
		}else{
			return false;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
//@intro	对当前Slice进行平移，平移量为(offetX,offsetY)
//@param	offetX:平移的X偏移量
//@param	offetY:平移的Y偏移量
//  [4/18/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
void EnrolledSlice::Translate(float offsetX,float offsetY){
	//平移所有三角面片
	for(auto i = 0; i < mPlaneFaces.size();i++){
		mPlaneFaces[i].Translate(offsetX,offsetY);
	}
	//分别平移几个控制点
	mCenterPoint.Translate(offsetX,offsetY);
	mLeftEndPoint.Translate(offsetX,offsetY);
	mMiddlePoint.Translate(offsetX,offsetY);
	mRightEndPoint.Translate(offsetX,offsetY);
	mMinPt[0] += offsetX;
	mMaxPt[0] += offsetX;
	mMinPt[1] += offsetY;
	mMaxPt[1] += offsetY;
}

//////////////////////////////////////////////////////////////////////////
//@intro	改变当前Slice的选中状态，如果当前状态为已选中，则修改为未选中
//			如果当前状态为未选中，则修改为已选中
//@return	void
//  [4/18/2016 zhangbo]

//////////////////////////////////////////////////////////////////////////
void EnrolledSlice::ChangeSelectedState(){
	if(mIsSelected){
		mIsSelected = false;
	}else{
		mIsSelected = true;
	}
}

//////////////////////////////////////////////////////////////////////////
//@intro	判断当前Slice是否已经被选中
//@return	bool:如果已经被选中则返回true,否则返回false
//  [4/18/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
bool EnrolledSlice::IsSliceSelected(){
	return mIsSelected;
}




//////////////////////////////////////////////////////////////////////////
//@intro	通过移动两端的控制点来实现Slice的变形
//@param	clickX,clickY:被选中的控制点的新的坐标位置
// s [4/25/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
void EnrolledSlice::MoveEndPoint(float clickX, float clickY){
	PlanePoint newLeftEndPoint,newRightEndPoint,newCenterPoint;
	float newMidRadius,newMaxRadius,newMinRadius;
	PlaneVector yAxis(0,1);
	//计算每个三角面片的每个顶点在整个展开后的Slice所占的比例,此处应该变为类的变量？！！
	if( (mIsLeftSelected && clickX >= mMiddlePoint.mPx)||(mIsRightSelected && clickX <= mMiddlePoint.mPx) ){
		return;
	}

	PlaneVector mid2End(clickX-mMiddlePoint.mPx, clickY - mMiddlePoint.mPy);
	float cosAlpha = mid2End.DotProduct(yAxis) / (mid2End.Length() * yAxis.Length());
	if(fabs(cosAlpha) < 1.0e-2){
		mEnrollMode = EnrollMode::Cylinder;
		//按照圆柱展开
		//cout<<"enroll in cylinder"<<endl;
		if(mIsLeftSelected){
			newLeftEndPoint = PlanePoint(clickX, clickY);
			newRightEndPoint = PlanePoint(2 * mMiddlePoint.mPx - clickX, clickY);
		}else if(mIsRightSelected){
			newRightEndPoint = PlanePoint(clickX, clickY);
			newLeftEndPoint = PlanePoint(2 * mMiddlePoint.mPx - clickX, clickY);
		}else{
			cout<<"WRONG: there is no control end point is selected"<<endl;
		}
		newCenterPoint = mMiddlePoint;
		float maxLength = mid2End.Length();
		for(int i = 0; i < mPlaneFaces.size();i++){
			MovePointInCylinder(mPlaneFaces[i].mVertexA,mMiddlePoint,mProportions[i][0],maxLength);
			MovePointInCylinder(mPlaneFaces[i].mVertexB,mMiddlePoint,mProportions[i][1],maxLength);
			MovePointInCylinder(mPlaneFaces[i].mVertexC,mMiddlePoint,mProportions[i][2],maxLength);
		}
		mLeftEndPoint = newLeftEndPoint;
		mRightEndPoint = newRightEndPoint;
		mCenterPoint = newCenterPoint;
	}else{
		mEnrollMode = EnrollMode::ConicalFrustum;
		newMidRadius  = fabs(mid2End.Length() / (2 * cosAlpha));
		newMaxRadius = newMidRadius + mRadiusThreshold / 2;
		newMinRadius = newMidRadius - mRadiusThreshold / 2;
	
		if(cosAlpha > 0){
			newCenterPoint = PlanePoint(mMiddlePoint.mPx, mMiddlePoint.mPy + newMidRadius);
		}else{
			newCenterPoint = PlanePoint(mMiddlePoint.mPx, mMiddlePoint.mPy - newMidRadius);
		}
		if(mIsLeftSelected){
			newLeftEndPoint = PlanePoint(clickX,clickY);
			newRightEndPoint = PlanePoint(2 * mMiddlePoint.mPx - clickX, clickY);
		}else if(mIsRightSelected){
			newRightEndPoint = PlanePoint(clickX,clickY);
			newLeftEndPoint = PlanePoint(2 * mMiddlePoint.mPx - clickX, clickY);
		}else{
			cout<<"WRONG: there is no control end point is selected "<<endl;
		}
		//根据新的Slice系数计算展开后的位置
		float newMaxAngle = acos( CalCosAngle(mMiddlePoint,newCenterPoint,newRightEndPoint) );
		for(int i = 0; i < mPlaneFaces.size();i++){
			MovePointInFrustum(mPlaneFaces[i].mVertexA, mMiddlePoint, newCenterPoint, mProportions[i][0],mDistances[i][0], newMaxAngle, newMidRadius);
			MovePointInFrustum(mPlaneFaces[i].mVertexB, mMiddlePoint, newCenterPoint, mProportions[i][1],mDistances[i][1], newMaxAngle, newMidRadius);
			MovePointInFrustum(mPlaneFaces[i].mVertexC, mMiddlePoint, newCenterPoint, mProportions[i][2],mDistances[i][2], newMaxAngle, newMidRadius);
		}
		mMidRadius = newMidRadius;
		mMinRadius = newMinRadius;
		mMaxRadius = newMaxRadius;
		mCenterPoint = newCenterPoint;
		mLeftEndPoint = newLeftEndPoint;
		mRightEndPoint = newRightEndPoint;
	}
	UpdateBoudnary();
}


//////////////////////////////////////////////////////////////////////////
//@intro	计算以圆柱体方式展开时，按照新的参数，计算点的新坐标位置
//@param	point:点的坐标位置
//@param	newMidPoint:展开后的Slice的中间控制点坐标
//@param	proportion:当前点在整个展开后的Slice长度所占的比例
//@param	newMaxLength:按照圆柱方式展开后距离对称轴的最远位置
//@return	point：计算得到的点的新的坐标位置
//  [4/25/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
void EnrolledSlice::MovePointInCylinder(PlanePoint &point, PlanePoint newMidPoint,float proportion,float newMaxLength){
	
	float newTempX = newMidPoint.mPx + newMaxLength * fabs(proportion) * (point.mPx >= newMidPoint.mPx ? 1 : -1);
	//cout<<"the minus value: "<<newTempX - newMidPoint.mx<<endl;
	float newTempY = point.mPy;
	point = PlanePoint(newTempX, newTempY);
}

//////////////////////////////////////////////////////////////////////////
//@intro	在以圆台方式展开时以新的参数进行展开,获取点的新的坐标位置
//@param	point:点的坐标位置
//@param	newMiddlePoint:按照圆台展开后的Slice的中间控制点
//@param	newCenterPoint:按照圆台展开后的Slice的圆心
//@param	proportion:每个点在整个Slice长度所占的比例
//@param	maxAngle:以圆台方式展开后的最大展开角度（以对称轴为参考）
//@param	newMinRadius:以圆台方式展开得到的圆环的内半径
//@return	point：计算得到的点的新的坐标位置
//  [4/25/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
void EnrolledSlice::MovePointInFrustum(PlanePoint &point, PlanePoint newMiddlePoint, PlanePoint newCenterPoint,
	float proportion,float relativeDistance,float maxAngle, float newMidRadius){
	float newCosAngle = cos( maxAngle * proportion);
	float newLength = newMidRadius + relativeDistance * (mCenterPoint.mPy >= mMiddlePoint.mPy ? -1 :1);
	float tempX;
	if(point.mPx == newCenterPoint.mPx){
		tempX = newCenterPoint.mPx;
	}else if(point.mPx > newCenterPoint.mPx){
		tempX = newCenterPoint.mPx + newLength * sqrt(1 - newCosAngle * newCosAngle);
	}else{
		tempX = newCenterPoint.mPx - newLength * sqrt(1- newCosAngle * newCosAngle);
	}
	float tempY = newCenterPoint.mPy + newLength * newCosAngle * (newCenterPoint.mPy >= newMiddlePoint.mPy ? -1 : 1);
	point = PlanePoint(tempX, tempY);
}

//////////////////////////////////////////////////////////////////////////
//@intro	改变当前Slice的最小半径和最大半径
//@param	oldCenterPoint:原来的中心点
//@param	oldMidPoint:原来的中间控制点
//@param	oldRightEndPoint:原来的右端控制点
//@param	oldMinRadius:原来的最小半径
//@param	oldMaxRadius:原来的最大半径

//@param	newCenterPoint:新的中心点
//@param	newMidPoint:新的中间控制点
//@param	newRightEndPoint:新的右端控制点
//@param	newMinRadius:新的最小展开半径
//@param	newMaxRadius:新的最大展开半径
//////////////////////////////////////////////////////////////////////////
void EnrolledSlice::ChangeRadius(PlanePoint oldCenterPoint,PlanePoint oldMidPoint,PlanePoint oldRightEndPoint,float oldMinRadius,float oldMaxRadius,
	PlanePoint newCenterPoint,PlanePoint newMidPoint,PlanePoint newRightEndPoint,float newMinRadius,float newMaxRadius){
	float oldMaxCosAngle = CalCosAngle(oldMidPoint,oldCenterPoint,oldRightEndPoint);
	float oldMaxAngle = acos(oldMaxCosAngle);

	float newMaxCosAngle = CalCosAngle(newMidPoint,newCenterPoint,newRightEndPoint);
	float newMaxAngle = acos(newMaxCosAngle);

	cout<<"oldMaxAngle: "<< oldMaxAngle <<"," << newMaxAngle<<endl;

	for(auto i= 0; i < mPlaneFaces.size();i++){
		ChangePoint(mPlaneFaces[i].mVertexA,oldCenterPoint,oldMidPoint,oldMinRadius,oldMaxRadius,oldMaxAngle,newCenterPoint,newMidPoint,newMinRadius,newMaxRadius,newMaxAngle);
		ChangePoint(mPlaneFaces[i].mVertexB,oldCenterPoint,oldMidPoint,oldMinRadius,oldMaxRadius,oldMaxAngle,newCenterPoint,newMidPoint,newMinRadius,newMaxRadius,newMaxAngle);
		ChangePoint(mPlaneFaces[i].mVertexC,oldCenterPoint,oldMidPoint,oldMinRadius,oldMaxRadius,oldMaxAngle,newCenterPoint,newMidPoint,newMinRadius,newMaxRadius,newMaxAngle);
	}
}


//////////////////////////////////////////////////////////////////////////
//@intro	修改每一个点的位置
//@param	oldPoint:顶点的旧坐标
//@param	oldMaxAngle:Slice的最初的最大展开角度
//@param	newMaxAngle:Slice新的最大展开角度
//@param	oldCenterPoint:Slice旧的中心点坐标
//@param	newCenterPoint:Slice新的中心点坐标
//@param	oldMidPoint:Slice旧的中心控制点坐标
//@param	newMidPoint:Slice新的中心控制点坐标
//////////////////////////////////////////////////////////////////////////
void EnrolledSlice::ChangePoint(PlanePoint &oldPoint,
	PlanePoint oldCenterPoint,PlanePoint oldMidPoint,float oldMinRadius,float oldMaxRadius,float oldMaxAngle,
	PlanePoint newCenterPoint,PlanePoint newMidPoint,float newMinRadius,float newMaxRadius,float newMaxAngle){

	float oldCosAngle = CalCosAngle(oldMidPoint,oldCenterPoint,oldPoint);
	float newCosAngle = cos(acos(fabs(oldCosAngle)) / oldMaxAngle * newMaxAngle);
	float oldLength = PlaneVector(oldPoint,oldCenterPoint).Length();
	float newLength = oldLength - oldMinRadius + newMinRadius;
	float tempY = newCenterPoint.mPy + newLength * newCosAngle * (newCenterPoint.mPy > newMidPoint.mPy ? -1: 1);
	float tempX = newCenterPoint.mPx + newLength * sqrt(1 - newCosAngle * newCosAngle) * (oldPoint.mPx > oldCenterPoint.mPx ?1 : -1);
	oldPoint = PlanePoint(tempX, tempY);
}

//////////////////////////////////////////////////////////////////////////
//@intro	计算两个向量（<midPoint,leftPoint>,<midPoint,rightPoint>）之间的夹角余弦值
//  [4/24/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
float EnrolledSlice::CalCosAngle(PlanePoint leftPoint,PlanePoint midPoint,PlanePoint rightPoint){
	PlaneVector mid2Left(midPoint,leftPoint);
	PlaneVector mid2Right(midPoint,rightPoint);
	return mid2Left.DotProduct(mid2Right) / (mid2Left.Length() * mid2Right.Length());
}

//////////////////////////////////////////////////////////////////////////
//@intro	绘制实心圆，用于绘制控制点
//@param	x,y:圆心位置坐标
//@param	radius:圆的半径
//  [4/24/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
void EnrolledSlice::Draw_solid_circle_red(float x, float y, float radius){
	int count;
	int sections = 200;

	float TWOPI = 2 * M_PI;
	glColor3f(1,0,0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x, y);
	for(count = 0; count <= sections; count++){
		glVertex2f(x + radius *cos(count * TWOPI / sections), y + radius * sin(count * TWOPI / sections));
	}
	glEnd();
}



//////////////////////////////////////////////////////////////////////////
//@intro	判断是否有左端或右端的控制点被选中
//@param	clickX,clickY:当前鼠标的坐标位置
//  [4/25/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
bool EnrolledSlice::IsControlEndSelected(float clickX, float clickY){
	PlaneVector point2Left(mLeftEndPoint.mPx-clickX, mLeftEndPoint.mPy - clickY);
	if(point2Left.Length() < mRadiusThreshold / 5){
		mIsLeftSelected = true;
		mIsRightSelected = false;
		return true;
	}else{
		PlaneVector point2Right(mRightEndPoint.mPx - clickX, mRightEndPoint.mPy - clickY);
		if(point2Right.Length() < mRadiusThreshold / 5){
			mIsRightSelected = true;
			mIsLeftSelected = false;
			return true;
		}
		else{
			return false;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
//@intro	重置Slice的控制点被选中的状态
//  [4/24/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
void EnrolledSlice::ResetControlPoint(){
	mIsLeftSelected = mIsRightSelected = false;
}

//////////////////////////////////////////////////////////////////////////
//@intro	分别计算每个三角面片的每个顶点的相对参数：包括与对称轴之间的距离的比例和与Slice中轴线的相对位置
//@return	proportionVector：每个三角面片的每个顶点与对称轴的距离所占最长距离的比例
//@return	distanceVector:每个三角面片的每个顶点距离Slice的中轴线的相对距离
//  [4/24/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
void EnrolledSlice::ComputeRelativeParameters(vector<vector<float>> &proportionVector,vector<vector<float>> &distanceVector){
	proportionVector.resize(mPlaneFaces.size());
	distanceVector.resize(mPlaneFaces.size());
	if(mEnrollMode == EnrollMode::ConicalFrustum){
		float oldMaxAngle = acos(CalCosAngle(mMiddlePoint,mCenterPoint,mRightEndPoint));
		for(auto i = 0; i < mPlaneFaces.size();i++){
			float tempAngle1 = acos(CalCosAngle(mMiddlePoint,mCenterPoint,mPlaneFaces[i].mVertexA));
			proportionVector[i].push_back(fabs(tempAngle1 / oldMaxAngle));
			float tempAngle2 = acos(CalCosAngle(mMiddlePoint,mCenterPoint,mPlaneFaces[i].mVertexB));
			proportionVector[i].push_back(fabs(tempAngle2 / oldMaxAngle));
			float tempAngle3 = acos(CalCosAngle(mMiddlePoint,mCenterPoint,mPlaneFaces[i].mVertexC));
			proportionVector[i].push_back(fabs(tempAngle3 / oldMaxAngle));

			float distance1 = PlaneVector(mPlaneFaces[i].mVertexA,mCenterPoint).Length() - mMidRadius;
			distanceVector[i].push_back(distance1);
			float distance2 = PlaneVector(mPlaneFaces[i].mVertexB,mCenterPoint).Length() - mMidRadius;
			distanceVector[i].push_back(distance2);
			float distance3 = PlaneVector(mPlaneFaces[i].mVertexC,mCenterPoint).Length() - mMidRadius;
			distanceVector[i].push_back(distance3);
		}
	}else{
		float maxLength = PlaneVector(mRightEndPoint,mMiddlePoint).Length();
		PlaneVector yAxis(0, 1);
		for(int i = 0; i < mPlaneFaces.size(); i++){
			float length1 = fabs(mPlaneFaces[i].mVertexA.mPx - mMiddlePoint.mPx);
			proportionVector[i].push_back(length1 / maxLength);

			float length2 = fabs(mPlaneFaces[i].mVertexB.mPx - mMiddlePoint.mPx);
			proportionVector[i].push_back(length2 / maxLength);

			float length3 = fabs(mPlaneFaces[i].mVertexC.mPx - mMiddlePoint.mPx);
			proportionVector[i].push_back(length3 / maxLength);

			float distance1 = mPlaneFaces[i].mVertexA.mPy - mMiddlePoint.mPy;
			distanceVector[i].push_back(distance1);
			float distance2 = mPlaneFaces[i].mVertexB.mPy - mMiddlePoint.mPy;
			distanceVector[i].push_back(distance2);
			float distance3 = mPlaneFaces[i].mVertexC.mPy - mMiddlePoint.mPy;
			distanceVector[i].push_back(distance3);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//@intro	获取该EnrolledSlice的上边界，即最大纵坐标值
//@return	纵坐标最大值
//////////////////////////////////////////////////////////////////////////
float EnrolledSlice::GetUpperBound(){
	return mMaxPt[1];
}

//////////////////////////////////////////////////////////////////////////
//@intro	获取该EnrolledSlice的下边界，即最小坐标值
//@return	纵坐标最小值
//////////////////////////////////////////////////////////////////////////
float EnrolledSlice::GetLowerBound(){
	return mMinPt[1];
}