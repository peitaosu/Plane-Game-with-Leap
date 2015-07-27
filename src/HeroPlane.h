#pragma once
#include "plane.h"

enum POWER_LEVEL
{
	P_LV_1,
	P_LV_2,
	P_LV_3,
	P_LV_MAX,
};

class HeroPlane :
	public Plane
{
public:
	HeroPlane(void);
	virtual ~HeroPlane(void);

	int GetLifeNum();
	int GetBombNum();
	int GetPower();

	void SetLifeMax(int LifeMax);
	void AddBoom();
	void SetBombMax(int BombMax);
	void SetPowerMax(int PowerMax);

	//等级变更
	void PowerUp(int power);
	void SetLevelUpEffect(HBITMAP *images);

	//更新显示
	void UpdatePlane();
	void ShowPlane();

	//必杀
	void BoomStart(bool anima);
	void SetBoomInfo(HBITMAP* images, int n);

	//无敌
	void SetInvincibleEffect(HBITMAP* images, int n);

private:
	int m_iLifeNum;
	int m_iLifeMax;
	int m_iBombNum;
	int m_iBombMax;

	int m_iPower;
	int m_iPowerMax;

	int m_iLevel;

	int m_iBoomStep;
	HBITMAP *m_BoomImage;
	int m_iNowBoomIndex;
	int m_iBoomIndexMax;
	bool m_bAnimation;

	HBITMAP *m_InvincibleEffect;
	int m_iNowEffectIndex;
	int m_iEffectIndexMax;

	HBITMAP *m_LevelUp;
	bool m_bIsLevelChange;
	int m_iLevelUpOffset;

	//飞行方向
	int m_iDirection;
};
