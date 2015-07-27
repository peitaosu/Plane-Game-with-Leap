#pragma once
#include "plane.h"

class PlaneGuard :
	public Plane
{
public:
	PlaneGuard(void);
	~PlaneGuard(void);

	void SetTarget(int target, double distance, double direction);
	void SetImpactable(bool flag);
	void SetX(int x);
	void SetY(int y);
	void SetMoveAction(bool moveable);

	void SetGuardImageInfo(HBITMAP *images, int n);

	//¸üÐÂÏÔÊ¾
	void UpdatePlane();
	void ShowPlane();

private:
	int m_Target;
	double m_Distance;
	double m_Angle;
	double m_iDirection;

	bool m_bImpactable;
	bool m_bFireOver;

	HBITMAP *m_GuardImage;
	int m_iNowImageIndex;
	int m_iImageIndexMax;

	bool m_bMoveable;
};
