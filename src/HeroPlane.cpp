#include ".\heroplane.h"
#include "Shooter.h"
#include "Bullet.h"
#include "Item.h"
#include "GameManager.h"

extern HDC g_hdc, g_hdcMem, g_hdcMask, g_hdcBuffer; //显示DC，内存DC，遮罩DC，缓冲DC
extern HBITMAP g_bmpMask;

extern int ScreenWidth;
extern int ScreenHeight;

extern UINT g_Time;

extern GameManager g_Manager;

//按键状态
extern BOOL LEFT_PRESSED, RIGHT_PRESSED, UP_PRESSED, DOWN_PRESSED, FIRE_PRESSED;

HeroPlane::HeroPlane(void)
{
	m_iBoomStep = STEP_1;
	m_iDirection = 0;
}

HeroPlane::~HeroPlane(void)
{
}

int HeroPlane::GetBombNum()
{
	return m_iBombNum;
}

int HeroPlane::GetLifeNum()
{
	return m_iLifeNum;
}

int HeroPlane::GetPower()
{
	return m_iPower;
}

void HeroPlane::SetLifeMax( int LifeMax )
{
	m_iLifeMax = LifeMax;
	m_iLifeNum = LifeMax;
}

void HeroPlane::AddBoom()
{
	m_iBombNum++;
	if (m_iBombNum > m_iBombMax)
	{
		m_iBombNum = m_iBombMax;
	}
}

void HeroPlane::SetBombMax( int BombMax )
{
	m_iBombMax = BombMax;
	m_iBombNum = 3;
}

void HeroPlane::SetPowerMax( int PowerMax )
{
	m_iPowerMax = PowerMax;
	m_iPower = 1;
	m_iLevel = P_LV_1;
}

void HeroPlane::UpdatePlane()
{
	switch (m_iState)
	{
	case P_BOOM:
		switch (m_iBoomStep)
		{
		case STEP_1:
			{
				vector<Plane*> *tempPlane = g_Manager.GetPlaneVector();

				for (int i=1; i<(int)tempPlane->size(); i++)
				{
					if(!(*tempPlane)[i])	continue;

					(*tempPlane)[i]->SetFireCommand(false);
				}
				m_Time = g_Time;	//无敌时间开始
				m_iBoomStep = STEP_2;
			}			
			break;

		case STEP_2:
			{
				vector<Bullet*> *tempBullet = g_Manager.GetEmemyBulletVector();

				for (int i=0; i<(int)tempBullet->size(); i++)
				{
					if(!(*tempBullet)[i])	continue;

					(*tempBullet)[i]->SetSpeed(15);
					(*tempBullet)[i]->SetBulletType(3);
					(*tempBullet)[i]->SetStep(STEP_3);
					(*tempBullet)[i]->SetTarget(0);
				}

				m_iBoomStep = STEP_3;
			}
			break;

		case STEP_3:
			{
				vector<Bullet*> *tempBullet = g_Manager.GetEmemyBulletVector();

				bool bBoomOver = true;
				for (int i=0; i<(int)tempBullet->size(); i++)
				{
					if ((*tempBullet)[i])
					{
						bBoomOver = false;
						break;
					}
				}

				if (bBoomOver)
				{
					m_iBoomStep = STEP_4;
				}
			}
			break;

		case STEP_4:
			{
				vector<Plane*> *tempPlane = g_Manager.GetPlaneVector();

				for (int i=1; i<(int)tempPlane->size(); i++)
				{
					if(!(*tempPlane)[i])	continue;

					(*tempPlane)[i]->SetFireCommand(true);
				}

				m_iNowBoomIndex = 0;
				m_iState = P_ALIVE;
			}
			break;
		}
	case P_INVINCIBLE:
		if (g_Time - m_Time >= m_InvincibleTime)
		{
			m_iState = P_ALIVE;
		}
	case P_ALIVE:
		if (LEFT_PRESSED && !RIGHT_PRESSED)
		{
			m_iDirection = 1;
			SetX(m_iX - m_iSpeed);
		}

		if (RIGHT_PRESSED && !LEFT_PRESSED)
		{
			m_iDirection = -1;
			SetX(m_iX + m_iSpeed);
		}

		if (!LEFT_PRESSED && !RIGHT_PRESSED)
		{
			m_iDirection = 0;
		}

		if (UP_PRESSED && !DOWN_PRESSED)
		{
			SetY(m_iY - m_iSpeed);
		}

		if (DOWN_PRESSED && !UP_PRESSED)
		{
			SetY(m_iY + m_iSpeed);
		}

		if (FIRE_PRESSED)
		{
			for (int i=0; i<(m_iLevel<<1)+1; i++)
			{
				m_vShooterGroup[0][i]->SetCommand(true);
				m_vShooterGroup[0][i]->Fire(true);
			}
		}
		else
		{
			for (int i=0; i<(int)m_vShooterGroup[0].size(); i++)
			{
				m_vShooterGroup[0][i]->SetCommand(false);
			}
		}

		//更新发射器的位置
		for (int i=0; i<(int)m_vShooterGroup.size(); i++)
		{
			for (int j=0; j<(int)m_vShooterGroup[i].size(); j++)
			{
				m_vShooterGroup[i][j]->SetPos(m_iX, m_iY);
			}
		}

		//更新所携带物品的位置
		for (int i=0; i<(int)items.size(); i++)
		{
			items[i]->SetPos(m_iX, m_iY);
		}

		{
			//检测与子弹的碰撞
			vector<Bullet*> *pBullets = g_Manager.GetEmemyBulletVector();
			for (int i=0; i<(int)pBullets->size(); i++)
			{
				if (!(*pBullets)[i])
				{
					continue;
				}

				if (IsImpact((*pBullets)[i]->GetImpactBox()))
				{
					delete (*pBullets)[i];
					(*pBullets)[i] = NULL;

					//飞机爆炸
					if (m_iState == P_ALIVE)
					{
						PlaySound(".\\Sound\\explosion.wav", NULL,SND_ASYNC|SND_NODEFAULT|SND_FILENAME);
						m_iState = P_EXPLODE;
						m_Time = g_Time;
						m_bExplodeOver = false;
						return;
					}
				}
			}

			//检测与物品的碰撞
			vector<Item*> *pItems = g_Manager.GetItemVector();
			for (int i=0; i<(int)pItems->size(); i++)
			{
				if (!(*pItems)[i])
				{
					continue;
				}

				if (IsImpact((*pItems)[i]->GetImpactBox()))
				{
					(*pItems)[i]->SetState(I_DISAPPEAR);
	
					//飞机获得物品
					switch ((*pItems)[i]->GetItemType())
					{
					case I_P_SMALL:
						PowerUp(1);
						break;

					case I_P_BIG:
						PowerUp(2);
						break;

					case I_BOOM:
						m_iBombNum++;
						if (m_iBombNum > m_iBombMax)
						{
							g_Manager.AddScore(300);
							m_iBombNum = m_iBombMax;
						}
						break;

					case I_LV_1:
						m_iLifeNum++;
						if (m_iLifeNum > m_iLifeMax)
						{
							g_Manager.AddScore(500);
							m_iLifeNum = m_iLifeMax;
						}
						break;

					case I_POINT:
						g_Manager.AddScore(100);
						break;
					}
				}
			}
		}
		break;

	case P_EXPLODE:
		//播放爆炸动画
		if (!m_bExplodeOver)
		{
			
		} 
		else
		{
			//爆炸结束
			m_iLifeNum--;
			
			if (m_iLifeNum)
			{
				m_iBombNum = 3;
				PowerUp(-1);
				SetPos(ScreenWidth>>1, ScreenHeight - 50);
				m_iState = P_INVINCIBLE;
				m_Time = g_Time;	//无敌时间开始
			}
			else
			{
				g_Manager.SetScriptMode(END);
			}
		}
		break;
	}
}

void HeroPlane::ShowPlane()
{
	switch (m_iState)
	{
	case P_EXPLODE:
		PlaneExplode();
		break;

	case P_BOOM:
		if (m_bAnimation && m_iNowBoomIndex < m_iBoomIndexMax)
		{
			BITMAP bitmap;
			GetObject(m_BoomImage[m_iNowBoomIndex], sizeof(bitmap), &bitmap);
			HPEN oldPen = (HPEN)SelectObject(g_hdcMem, m_BoomImage[m_iNowBoomIndex]);
			SetBkColor(g_hdcMem, RGB(0, 0, 0));

			g_bmpMask = CreateBitmap(bitmap.bmWidth, bitmap.bmHeight, 1, 1, NULL);
			HPEN oldMaskPen = (HPEN)SelectObject(g_hdcMask, g_bmpMask);
			BitBlt(g_hdcMask, 0, 0, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCCOPY);

			BitBlt(g_hdcBuffer, (ScreenWidth-bitmap.bmWidth)>>1, (ScreenHeight-bitmap.bmHeight)>>1, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCINVERT);
			BitBlt(g_hdcBuffer, (ScreenWidth-bitmap.bmWidth)>>1, (ScreenHeight-bitmap.bmHeight)>>1, bitmap.bmWidth, bitmap.bmHeight, g_hdcMask, 0, 0, SRCAND);
			BitBlt(g_hdcBuffer, (ScreenWidth-bitmap.bmWidth)>>1, (ScreenHeight-bitmap.bmHeight)>>1, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCINVERT);

			SelectObject(g_hdcMask, oldMaskPen);
			DeleteObject(g_bmpMask);
			SelectObject(g_hdcMem, oldPen);

			//m_iNowBoomIndex++;
			//if (m_iNowBoomIndex >= m_iBoomIndexMax)
			//{
			//	m_iNowBoomIndex = 0;
			//}
			if (g_Time - m_Time > 100)
			{
				m_iNowBoomIndex++;
				m_Time = g_Time;
			}
		}
		//NO BREAK

	case P_INVINCIBLE:
		{
			BITMAP bitmap;
			GetObject(m_InvincibleEffect[m_iNowEffectIndex], sizeof(bitmap), &bitmap);
			HPEN oldPen = (HPEN)SelectObject(g_hdcMem, m_InvincibleEffect[m_iNowEffectIndex]);
			SetBkColor(g_hdcMem, RGB(0, 0, 0));

			g_bmpMask = CreateBitmap(bitmap.bmWidth, bitmap.bmHeight, 1, 1, NULL);
			HPEN oldMaskPen = (HPEN)SelectObject(g_hdcMask, g_bmpMask);
			BitBlt(g_hdcMask, 0, 0, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCCOPY);

			BitBlt(g_hdcBuffer, m_iX-25, m_iY-20, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCINVERT);
			BitBlt(g_hdcBuffer, m_iX-25, m_iY-20, bitmap.bmWidth, bitmap.bmHeight, g_hdcMask, 0, 0, SRCAND);
			BitBlt(g_hdcBuffer, m_iX-25, m_iY-20, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCINVERT);

			SelectObject(g_hdcMask, oldMaskPen);
			DeleteObject(g_bmpMask);
			SelectObject(g_hdcMem, oldPen);

			m_iNowEffectIndex++;
			if (m_iNowEffectIndex >= m_iEffectIndexMax)
			{
				m_iNowEffectIndex = 0;
			}
		}
		//NO BREAK

	default:
		BITMAP bitmap;
		GetObject(m_hImage, sizeof(bitmap), &bitmap);
		HPEN oldPen = (HPEN)SelectObject(g_hdcMem, m_hImage);
		SetBkColor(g_hdcMem, RGB(32, 156, 0));

		int frameWidth = bitmap.bmWidth/3;
		int frameHeight = bitmap.bmHeight;
		g_bmpMask = CreateBitmap(frameWidth, frameHeight, 1, 1, NULL);
		HPEN oldMaskPen = (HPEN)SelectObject(g_hdcMask, g_bmpMask);
		BitBlt(g_hdcMask, 0, 0, frameWidth, frameHeight, g_hdcMem, frameWidth*(1+m_iDirection), 0, SRCCOPY);

		BitBlt(g_hdcBuffer, m_iX-m_iOffsetX, m_iY-m_iOffsetY, frameWidth, frameHeight, g_hdcMem, frameWidth*(1+m_iDirection), 0, SRCINVERT);
		BitBlt(g_hdcBuffer, m_iX-m_iOffsetX, m_iY-m_iOffsetY, frameWidth, frameHeight, g_hdcMask, 0, 0, SRCAND);
		BitBlt(g_hdcBuffer, m_iX-m_iOffsetX, m_iY-m_iOffsetY, frameWidth, frameHeight, g_hdcMem, frameWidth*(1+m_iDirection), 0, SRCINVERT);

		SelectObject(g_hdcMask, oldMaskPen);
		DeleteObject(g_bmpMask);
		SelectObject(g_hdcMem, oldPen);

		if (m_bIsLevelChange)
		{
			GetObject(m_LevelUp[m_iLevel], sizeof(bitmap), &bitmap);
			HPEN oldPen = (HPEN)SelectObject(g_hdcMem, m_LevelUp[m_iLevel]);
			SetBkColor(g_hdcMem, RGB(255, 255, 255));

			g_bmpMask = CreateBitmap(bitmap.bmWidth, bitmap.bmHeight, 1, 1, NULL);
			HPEN oldMaskPen = (HPEN)SelectObject(g_hdcMask, g_bmpMask);
			BitBlt(g_hdcMask, 0, 0, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCCOPY);

			BitBlt(g_hdcBuffer, m_iX-25, m_iY-m_iLevelUpOffset, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCINVERT);
			BitBlt(g_hdcBuffer, m_iX-25, m_iY-m_iLevelUpOffset, bitmap.bmWidth, bitmap.bmHeight, g_hdcMask, 0, 0, SRCAND);
			BitBlt(g_hdcBuffer, m_iX-25, m_iY-m_iLevelUpOffset, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCINVERT);

			SelectObject(g_hdcMask, oldMaskPen);
			DeleteObject(g_bmpMask);
			SelectObject(g_hdcMem, oldPen);

			m_iLevelUpOffset++;
			if (m_iLevelUpOffset > 25)
			{
				m_bIsLevelChange = false;
			}
		}
		break;
	}

	//HBRUSH hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
	//HPEN oldPen = (HPEN)SelectObject(g_hdcBuffer, hBrush);

	//Rectangle(g_hdcBuffer, m_iX, m_iY, m_ImpactBox.right, m_ImpactBox.bottom);

	//SelectObject(g_hdcBuffer, oldPen);
	//DeleteObject(hBrush);
}

void HeroPlane::BoomStart(bool anima)
{
	if (m_iBombNum > 0 && (m_iState == P_ALIVE || m_iState == P_INVINCIBLE))
	{
		m_iState = P_BOOM;
		m_bAnimation = anima;
		m_iBoomStep = STEP_1;
		m_iBombNum--;
	}
}

void HeroPlane::SetBoomInfo( HBITMAP* images, int n )
{
	m_BoomImage = images;
	m_iNowBoomIndex = 0;
	m_iBoomIndexMax = n;
}

void HeroPlane::SetInvincibleEffect( HBITMAP* images, int n )
{
	m_InvincibleEffect = images;
	m_iNowEffectIndex = 0;
	m_iEffectIndexMax = n;
}

void HeroPlane::PowerUp( int power )
{
	int tempLevel = m_iLevel;

	m_iPower += power;
	if (m_iPower < 1)
	{
		m_iPower = 1;
	}
	if (m_iPower > m_iPowerMax)
	{
		g_Manager.AddScore(200);
		m_iPower = m_iPowerMax;
	}

	if (m_iPower >= 9)
	{
		m_iLevel = P_LV_MAX;
	}
	else if (m_iPower >= 6)
	{
		m_iLevel = P_LV_3;
	}
	else if (m_iPower >= 3)
	{
		m_iLevel = P_LV_2;
	}
	else
	{
		m_iLevel = P_LV_1;
	}

	if (m_iLevel != tempLevel)
	{
		m_iLevelUpOffset = 10;
		m_bIsLevelChange = true;
	}
}

void HeroPlane::SetLevelUpEffect( HBITMAP *images )
{
	m_LevelUp = images;
}