#pragma once
#include <Windows.h>

enum ITEM_TYPE
{
	I_P_SMALL,
	I_P_BIG,
	I_BOOM,
	I_LV_1,
	I_POINT,
	I_EVER,
};

enum ITEM_STATE
{
	I_ALIVE,
	I_DISAPPEAR,
};

enum ITEM_STEP
{
	I_STEP_1,
	I_STEP_2,
};

class Item
{
public:
	Item(void);
	~Item(void);

	//成员取得和设置
	int GetItemType();
	void SetItemType(int type);

	RECT& GetImpactBox();
	void SetImpactBox(int width, int height);

	int GetX();
	int GetY();
	void SetX(int x);
	void SetY(int y);
	void SetPos(int x, int y);

	int GetSpeed();
	void SetSpeed(int speed);

	void SetDirection(int direction);

	int GetState();
	void SetState(int state);

	//移动
	void ItemMove();

	//显示
	void ShowItem();

private:
	int m_ItemType;
	int m_ItemStep;

	RECT m_ImpactBox;
	int m_iX;
	int m_iY;

	int m_iSpeed;

	int m_iState;

	int m_iDesX;
	int m_iDesY;
	double m_iAngle;
};
