#pragma once
#include <Windows.h>
#include <vector>

using namespace std;

enum PLANE_STATUS
{
	P_ALIVE,
	P_DISAPPEAR,
	P_EXPLODE,
	P_INVINCIBLE,
	P_BOOM,
};

class Shooter;
class Item;

class Plane
{
public:
	Plane(void);
	Plane(RECT& ImpactBox);
	virtual ~Plane(void);

	//成员设置取得
	int GetHp();
	int GetAtk();
	int GetDef();
	int GetSpeed();
	void SetHp(int Hp);
	void SetAtk(int Atk);
	void SetDef(int Def);
	void SetSpeed(int Speed);

	int GetX();
	int GetY();
	int GetState();
	virtual void SetX(int x);
	virtual void SetY(int y);
	void SetPos(int x, int y);
	void SetState(int State);

	RECT& GetImpactBox();
	void SetImpactBox(int width, int height);

	void SetImage(HBITMAP image);
	void SetOffset(int x, int y);

	void AddShooter(vector<Shooter*> &shooters);
	void ClearShooter();

	//开火设定
	void SetFireCommand(bool command);

	//物品管理
	void AddItem(int ItemType, int ItemNum);
	void ClearItem();

	//碰撞判断
	BOOL IsImpact(RECT& ImpactBox);
	void SetExplodeInfo(HBITMAP* images, int n);
	void PlaneExplode();

	//更新显示
	virtual void UpdatePlane();
	virtual void ShowPlane();

protected:
	//属性
	int m_iHp;
	int m_iHpMax;
	int m_iAtk;
	int m_iDef;
	int m_iSpeed;

	//状态,位置
	int m_iX;
	int m_iY;
	int m_iState;

	//碰撞信息
	RECT m_ImpactBox;
	int m_iWidth;
	int m_iHeight;

	//发射器
	int m_iNowShooterGroup;
	vector< vector<Shooter*> > m_vShooterGroup;

	//开火指令
	bool m_bFireCommand;

	//图像
	HBITMAP m_hImage;

	//爆炸
	HBITMAP *m_ExplodeImage;
	int m_iNowExplodeIndex;
	int m_iExplodeIndexMax;
	bool m_bExplodeOver;

	//相对于图像的偏移量
	int m_iOffsetX;
	int m_iOffsetY;

	//计时器
	unsigned int m_Time;
	unsigned int m_InvincibleTime;

	//物品掉落
	vector<Item*> items;

	//必杀
};
