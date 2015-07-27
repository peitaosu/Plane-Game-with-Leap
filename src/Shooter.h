#pragma once
#include <Windows.h>

class Bullet;

class Shooter
{
public:
	Shooter(void);
	virtual ~Shooter(void);

	//成员设置和取得
	int GetX();
	int GetY();
	void SetX(int x);
	void SetY(int y);
	void SetPos(int x, int y);

	double GetDirection();
	double GetFireDirection();
	int GetSpeed();
	int GetFireSpeed();
	bool GetCommand();
	void SetDirection(double direcion);
	void SetFireDirection(double direction);
	void SetSpeed(int speed);
	void SetFireSpeed(int speed);
	void SetCommand(bool command);
	void SetDelay(UINT delay);
	void SetReloadMax(int max);

	int GetState();
	void SetState(int state);

	void SetBulletTemplate(Bullet &BulletTemplate);

	RECT& GetImpactBox();
	void SetImpactBox(int width, int height);

	void SetImage(LPCTSTR ImageName);

	//移动
	virtual void ShooterMove();

	//装弹
	void Reload();
	Bullet* ReloadBullet();

	//发射
	virtual bool Fire(bool camp);

	//显示
	void ShowShooter();

private:
	//位置
	int m_iX;
	int m_iY;

	//方向
	double m_iDirection;
	double m_iFireDirection;

	//状态
	int m_iState;

	//子弹模板
	Bullet *m_BulletTemplate;

	//速度
	UINT m_iDelay;
	UINT m_iShowTime;
	UINT m_Time;

	int m_iReloadMax;
	int m_iRemainBullets;

	int m_iSpeed;
	int m_iFireSpeed;
	bool m_bCommand;

	//图像
	HBITMAP m_hImage;

	//碰撞盒
	RECT m_ImpactBox;
};
