#pragma once
#include "plane.h"

class EnemyPlane :
	public Plane
{
public:
	EnemyPlane(void);
	virtual ~EnemyPlane(void);

	void SetDelay(unsigned int delay);

	//更新显示
	void UpdatePlane();
	void ShowPlane();

	//移动
	void SetX(int x);
	void SetY(int y);
	void PlaneMove();
	void PlanePause();
	void AddPos(int x, int y);

	//BOSS
	void SetBoss(bool isBoss);

	void SetImageInfo(HBITMAP *images, int n);

private:
	int m_iDesX;
	int m_iDesY;
	int m_iNowPos;
	double m_iAngle;

	bool m_bMoveable;
	
	vector<POINT> posMap;

	//更新延迟
	unsigned int m_Delay;
	unsigned int m_ShowTime;

	//BOSS判定
	bool m_bIsBoss;

	//图像
	HBITMAP *m_PlaneImage;
	int m_iNowImageIndex;
	int m_iImageIndexMax;
};
