#pragma once
#include <Windows.h>

enum BULLET_STATE
{
	ALIVE,
	EXPLODE,
	DISAPPEAR,
};

enum BULLET_STEP
{
	STEP_1,
	STEP_2,
	STEP_3,
	STEP_4,
	STEP_5,
};

class Plane;

class Bullet
{
public:
	Bullet(void);
	Bullet(Bullet *);
	virtual ~Bullet(void);

	//成员设置和取得
	double GetX();
	double GetY();
	void SetX(double x);
	void SetY(double y);
	void SetPos(double x, double y);
	void SetDestPos(int x, int y);

	double GetSpeed();
	double GetFinalSpeed();
	double GetAcceleration();
	double GetDirection();
	void SetSpeed(double speed);
	void SetFinalSpeed(double speed);
	void SetAcceleration(double acceleration);
	void SetDirection(double direction);
	void SetDirectChange(double direct);

	void SetPauseTime(unsigned int time);

	int GetBulletType();
	void SetBulletType(int type);

	int GetState();
	void SetState(int state);

	int GetStep();
	void SetStep(int step);

	RECT& GetImpactBox();
	void SetImpactBox(int width, int height);
	int GetWidth();
	int GetHeight();

	void SetImage(HBITMAP image, int type);
	void SetOffset(int x, int y);

	void SetTarget(int target);

	//子弹移动
	virtual void BulletMove();

	//显示
	void ShowBullet();

private:
	//位置
	double m_iX;
	double m_iY;

	//子弹类型
	int m_BulletType;
	int m_ShowType;

	//速度&方向
	double m_iSpeed;
	double m_iDirection;
	double m_DirectChange;

	double m_iAcceleration;
	double m_iFinalSpeed;

	unsigned int m_Time;
	unsigned int m_PauseTime;

	int m_iTarget;
	int m_iDesX;
	int m_iDesY;
	double m_iAngle;

	//状态
	int m_iState;
	int m_iStep;

	//图像
	HBITMAP m_hImage;
	bool m_bMirror;
	unsigned int m_SwitchTime;

	//相对于图像的偏移量
	int m_iOffsetX;
	int m_iOffsetY;

	//碰撞
	RECT m_ImpactBox;
	int m_iWidth;
	int m_iHeight;
};
