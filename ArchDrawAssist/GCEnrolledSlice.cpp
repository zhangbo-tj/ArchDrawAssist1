#include "StdAfx.h"
#include "GCEnrolledSlice.h"


//////////////////////////////////////////////////////////////////////////
//@intro	�����������ڳ�ʼ��չ�����SliceƬ��
//@param	tris:ƽ��������Ƭ����
//@param	leftPoint:Slice����е�����
//@param	middlePoint:Slice�м��е�����
//@param	rightPoint:Slice�Ҳ��е�����
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
	UpdateBoudnary();	//����չ����ı߽�
}

//////////////////////////////////////////////////////////////////////////
//@intro	����չ�����Slice
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
//@intro	���µ�ǰչ�����Slice�����±߽�
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
//@intro	����չ��Slice�ı߽�
//@return	void
//  [4/18/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
void EnrolledSlice::ShowBoundary(){
	PlaneVector middle2Right(mMiddlePoint,mRightEndPoint);
	PlaneVector yAxis(0,1);
	float dotProduct = middle2Right.DotProduct(yAxis) / (middle2Right.Length() * yAxis.Length());
	glColor3f(0,1,0);		//�趨�߿���ɫΪ��ɫ
	//�����߶�����Ϊ����
	glLineStipple(2, 0x5555);
	glEnable(GL_LINE_STIPPLE);
	//չ����ʽΪԲ��
	if(mEnrollMode == EnrollMode::Cylinder){
		glBegin(GL_LINE_LOOP);
			glVertex2f(mMinPt[0],mMinPt[1]);
			glVertex2f(mMinPt[0],mMaxPt[1]);
			glVertex2f(mMaxPt[0],mMaxPt[1]);
			glVertex2f(mMaxPt[0],mMinPt[1]);
		glEnd();
	}else{
		//չ����ʽΪԲ̨
		bool rightFlag = mCenterPoint.mPy > mMiddlePoint.mPy ? true : false;	//�ж�Բ̨���������ߵ���
		PlaneVector center2Middle(mCenterPoint,mMiddlePoint);
		PlaneVector center2Right(mCenterPoint,mRightEndPoint);
		
		float cosAlpha = center2Middle.DotProduct(center2Right) / (center2Middle.Length() * center2Right.Length());
		//Բ̨��������
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
		//Բ̨�ǵ�����
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
//@intro	����Slice�Ŀ��Ƶ�
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
//@intro	�����ض�λ�úͰ뾶��Բ��,��Ҫ���ڻ���Բ���ı߽�
//@param	centerX,centerY:Բ��Բ��λ��
//@param	startAngle:Բ���Ŀ�ʼ�Ƕ�
//@param	endAngle��Բ���Ľ����Ƕ�
//@param	radius:Բ���İ뾶
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
//@intro	�жϵ㵱ǰ���Ƿ���EnrolledSlice��
//@param	px,py:��ǰ�������
//@return	bool:��ǰ�㵱ǰSlice���򷵻�true,���򷵻�false
//  [4/18/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
bool EnrolledSlice::IsPointInSlice(float px,float py){
	PlaneVector middle2Right(mMiddlePoint,mRightEndPoint);
	PlaneVector yAxis(0,1);
	float cosAlpha = middle2Right.DotProduct(yAxis) / (middle2Right.Length() * yAxis.Length());
	//����Բ����ʽչ��
	if(mEnrollMode == EnrollMode::Cylinder){
		if(px >= mMinPt[0] && px <= mMaxPt[0] && py >= mMinPt[1] && py <= mMaxPt[1]){
			return true;
		}else{
			return false;
		}
	}
	//����Բ̨��ʽչ��
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
//@intro	�Ե�ǰSlice����ƽ�ƣ�ƽ����Ϊ(offetX,offsetY)
//@param	offetX:ƽ�Ƶ�Xƫ����
//@param	offetY:ƽ�Ƶ�Yƫ����
//  [4/18/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
void EnrolledSlice::Translate(float offsetX,float offsetY){
	//ƽ������������Ƭ
	for(auto i = 0; i < mPlaneFaces.size();i++){
		mPlaneFaces[i].Translate(offsetX,offsetY);
	}
	//�ֱ�ƽ�Ƽ������Ƶ�
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
//@intro	�ı䵱ǰSlice��ѡ��״̬�������ǰ״̬Ϊ��ѡ�У����޸�Ϊδѡ��
//			�����ǰ״̬Ϊδѡ�У����޸�Ϊ��ѡ��
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
//@intro	�жϵ�ǰSlice�Ƿ��Ѿ���ѡ��
//@return	bool:����Ѿ���ѡ���򷵻�true,���򷵻�false
//  [4/18/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
bool EnrolledSlice::IsSliceSelected(){
	return mIsSelected;
}




//////////////////////////////////////////////////////////////////////////
//@intro	ͨ���ƶ����˵Ŀ��Ƶ���ʵ��Slice�ı���
//@param	clickX,clickY:��ѡ�еĿ��Ƶ���µ�����λ��
// s [4/25/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
void EnrolledSlice::MoveEndPoint(float clickX, float clickY){
	PlanePoint newLeftEndPoint,newRightEndPoint,newCenterPoint;
	float newMidRadius,newMaxRadius,newMinRadius;
	PlaneVector yAxis(0,1);
	//����ÿ��������Ƭ��ÿ������������չ�����Slice��ռ�ı���,�˴�Ӧ�ñ�Ϊ��ı���������
	if( (mIsLeftSelected && clickX >= mMiddlePoint.mPx)||(mIsRightSelected && clickX <= mMiddlePoint.mPx) ){
		return;
	}

	PlaneVector mid2End(clickX-mMiddlePoint.mPx, clickY - mMiddlePoint.mPy);
	float cosAlpha = mid2End.DotProduct(yAxis) / (mid2End.Length() * yAxis.Length());
	if(fabs(cosAlpha) < 1.0e-2){
		mEnrollMode = EnrollMode::Cylinder;
		//����Բ��չ��
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
		//�����µ�Sliceϵ������չ�����λ��
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
//@intro	������Բ���巽ʽչ��ʱ�������µĲ�����������������λ��
//@param	point:�������λ��
//@param	newMidPoint:չ�����Slice���м���Ƶ�����
//@param	proportion:��ǰ��������չ�����Slice������ռ�ı���
//@param	newMaxLength:����Բ����ʽչ�������Գ������Զλ��
//@return	point������õ��ĵ���µ�����λ��
//  [4/25/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
void EnrolledSlice::MovePointInCylinder(PlanePoint &point, PlanePoint newMidPoint,float proportion,float newMaxLength){
	
	float newTempX = newMidPoint.mPx + newMaxLength * fabs(proportion) * (point.mPx >= newMidPoint.mPx ? 1 : -1);
	//cout<<"the minus value: "<<newTempX - newMidPoint.mx<<endl;
	float newTempY = point.mPy;
	point = PlanePoint(newTempX, newTempY);
}

//////////////////////////////////////////////////////////////////////////
//@intro	����Բ̨��ʽչ��ʱ���µĲ�������չ��,��ȡ����µ�����λ��
//@param	point:�������λ��
//@param	newMiddlePoint:����Բ̨չ�����Slice���м���Ƶ�
//@param	newCenterPoint:����Բ̨չ�����Slice��Բ��
//@param	proportion:ÿ����������Slice������ռ�ı���
//@param	maxAngle:��Բ̨��ʽչ��������չ���Ƕȣ��ԶԳ���Ϊ�ο���
//@param	newMinRadius:��Բ̨��ʽչ���õ���Բ�����ڰ뾶
//@return	point������õ��ĵ���µ�����λ��
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
//@intro	�ı䵱ǰSlice����С�뾶�����뾶
//@param	oldCenterPoint:ԭ�������ĵ�
//@param	oldMidPoint:ԭ�����м���Ƶ�
//@param	oldRightEndPoint:ԭ�����Ҷ˿��Ƶ�
//@param	oldMinRadius:ԭ������С�뾶
//@param	oldMaxRadius:ԭ�������뾶

//@param	newCenterPoint:�µ����ĵ�
//@param	newMidPoint:�µ��м���Ƶ�
//@param	newRightEndPoint:�µ��Ҷ˿��Ƶ�
//@param	newMinRadius:�µ���Сչ���뾶
//@param	newMaxRadius:�µ����չ���뾶
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
//@intro	�޸�ÿһ�����λ��
//@param	oldPoint:����ľ�����
//@param	oldMaxAngle:Slice����������չ���Ƕ�
//@param	newMaxAngle:Slice�µ����չ���Ƕ�
//@param	oldCenterPoint:Slice�ɵ����ĵ�����
//@param	newCenterPoint:Slice�µ����ĵ�����
//@param	oldMidPoint:Slice�ɵ����Ŀ��Ƶ�����
//@param	newMidPoint:Slice�µ����Ŀ��Ƶ�����
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
//@intro	��������������<midPoint,leftPoint>,<midPoint,rightPoint>��֮��ļн�����ֵ
//  [4/24/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
float EnrolledSlice::CalCosAngle(PlanePoint leftPoint,PlanePoint midPoint,PlanePoint rightPoint){
	PlaneVector mid2Left(midPoint,leftPoint);
	PlaneVector mid2Right(midPoint,rightPoint);
	return mid2Left.DotProduct(mid2Right) / (mid2Left.Length() * mid2Right.Length());
}

//////////////////////////////////////////////////////////////////////////
//@intro	����ʵ��Բ�����ڻ��ƿ��Ƶ�
//@param	x,y:Բ��λ������
//@param	radius:Բ�İ뾶
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
//@intro	�ж��Ƿ�����˻��Ҷ˵Ŀ��Ƶ㱻ѡ��
//@param	clickX,clickY:��ǰ��������λ��
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
//@intro	����Slice�Ŀ��Ƶ㱻ѡ�е�״̬
//  [4/24/2016 zhangbo]
//////////////////////////////////////////////////////////////////////////
void EnrolledSlice::ResetControlPoint(){
	mIsLeftSelected = mIsRightSelected = false;
}

//////////////////////////////////////////////////////////////////////////
//@intro	�ֱ����ÿ��������Ƭ��ÿ���������Բ�����������Գ���֮��ľ���ı�������Slice�����ߵ����λ��
//@return	proportionVector��ÿ��������Ƭ��ÿ��������Գ���ľ�����ռ�����ı���
//@return	distanceVector:ÿ��������Ƭ��ÿ���������Slice�������ߵ���Ծ���
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
//@intro	��ȡ��EnrolledSlice���ϱ߽磬�����������ֵ
//@return	���������ֵ
//////////////////////////////////////////////////////////////////////////
float EnrolledSlice::GetUpperBound(){
	return mMaxPt[1];
}

//////////////////////////////////////////////////////////////////////////
//@intro	��ȡ��EnrolledSlice���±߽磬����С����ֵ
//@return	��������Сֵ
//////////////////////////////////////////////////////////////////////////
float EnrolledSlice::GetLowerBound(){
	return mMinPt[1];
}