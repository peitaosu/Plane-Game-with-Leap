#include ".\enemyplane.h"
#include "HeroPlane.h"
#include <math.h>
#include "Shooter.h"
#include "Bullet.h"
#include "Item.h"
#include "GameManager.h"

extern HDC g_hdc, g_hdcMem, g_hdcMask, g_hdcBuffer; //显示DC，内存DC，遮罩DC，缓冲DC
extern HBITMAP g_bmpMask;

extern int ScreenWidth;
extern int ScreenHeight;

extern GameManager g_Manager;

extern UINT g_Time;

EnemyPlane::EnemyPlane(void)
{
	m_bMoveable = true;
	m_iNowPos = 0;
	m_ShowTime = g_Time;

	m_bIsBoss = false;

	m_PlaneImage = NULL;

	AddItem(I_EVER, 10);
}

EnemyPlane::~EnemyPlane(void)
{
}

void EnemyPlane::SetDelay( unsigned int delay )
{
	m_Delay = delay;
}

void EnemyPlane::SetX( int x )
{
	if (m_iState != P_ALIVE)
	{
		return;
	}

	if (x < -m_iWidth || x > ScreenWidth)
	{
		m_iState = P_DISAPPEAR;
		return;
	}

	OffsetRect(&m_ImpactBox, x-m_iX, 0);
	m_iX = x;
}

void EnemyPlane::SetY( int y )
{
	if (m_iState != P_ALIVE)
	{
		return;
	}

	if (y < -m_iHeight || y > ScreenHeight)
	{
		m_iState = P_DISAPPEAR;
		return;
	}

	OffsetRect(&m_ImpactBox, 0, y-m_iY);
	m_iY = y;
}

void EnemyPlane::UpdatePlane()
{
	if (g_Time - m_ShowTime < m_Delay)
	{
		return;
	}

	switch (m_iState)
	{
	case P_ALIVE:
		if (abs(m_iDesX-m_iX)>m_iSpeed || abs(m_iDesY-m_iY)>m_iSpeed)
		{
			m_iAngle = atan((double)(m_iDesX-m_iX)/(double)(m_iDesY-m_iY));

			if (m_iDesY > m_iY)
			{
				SetX(m_iX + (int)(m_iSpeed*sin(m_iAngle)));
				SetY(m_iY + (int)(m_iSpeed*cos(m_iAngle)));
			}
			else if(m_iDesY < m_iY)
			{
				SetX(m_iX - (int)(m_iSpeed*sin(m_iAngle)));
				SetY(m_iY - (int)(m_iSpeed*cos(m_iAngle)));
			}
			else
			{
				if (m_iDesX > m_iX)
				{
					SetX(m_iX+m_iSpeed);
				}
				else
				{
					SetX(m_iX-m_iSpeed);
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
		}
		else
		{
			m_bMoveable = true;
			vector<Shooter*> *pShooters = NULL;
			if (!m_vShooterGroup.empty())
			{
				pShooters = &m_vShooterGroup[m_iNowShooterGroup];
				for (int i=0; i<(int)pShooters->size(); i++)
				{
					if (m_bFireCommand)
					{
						(*pShooters)[i]->SetCommand(true);
						m_bMoveable &= (*pShooters)[i]->Fire(false); 
					}
				}
			}

			if (m_bMoveable)
			{
				m_iNowPos++;

				if (m_iNowPos >= (int)posMap.size())
				{
					m_iNowPos = 0;
				}

				m_iDesX = posMap[m_iNowPos].x;
				m_iDesY = posMap[m_iNowPos].y;

				if (pShooters)
				{
					m_iNowShooterGroup = rand()%(int)m_vShooterGroup.size();

					for (int i=0; i<(int)pShooters->size(); i++)
					{
						(*pShooters)[i]->SetCommand(false);
						(*pShooters)[i]->Reload(); 
					}
				}
			}
		}

		{
			//判断与子弹的碰撞
			vector<Bullet*> *pBullets = g_Manager.GetMyBulletVector();
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
					g_Manager.AddScore(10);

					m_iHp--;
					if (m_iHp <= 0)
					{
						//弹出物品
						for (int i=0; i<(int)items.size(); i++)
						{
							if(!items[i]) continue;

							g_Manager.AddItemToVector(items[i]);
						}
						ClearItem();

						//飞机爆炸
						PlaySound(".\\Sound\\explosion.wav", NULL,SND_ASYNC|SND_NODEFAULT|SND_FILENAME);
						m_bExplodeOver = false;
						m_iState = P_EXPLODE;
						m_Time = g_Time;
						
						if (m_bIsBoss)
						{
							g_Manager.GetHeroPlane()->AddBoom();
							g_Manager.GetHeroPlane()->BoomStart(false);
						}
					}
				}
			}
		}
		break;

	case P_EXPLODE:
		if (!m_bExplodeOver)
		{
		}
		else
		{
			m_iState = P_DISAPPEAR;
		}
		break;

	case P_DISAPPEAR:
		break;
	}
}

void EnemyPlane::ShowPlane()
{
	if (g_Time - m_ShowTime < m_Delay)
	{
		return;
	}

	switch (m_iState)
	{
	case P_EXPLODE:
		PlaneExplode();
		break;

	default:
		if (m_PlaneImage)
		{
			BITMAP bitmap;
			GetObject(m_PlaneImage[m_iNowImageIndex], sizeof(bitmap), &bitmap);
			HPEN oldPen = (HPEN)SelectObject(g_hdcMem, m_PlaneImage[m_iNowImageIndex]);
			SetBkColor(g_hdcMem, RGB(0, 123, 139));

			g_bmpMask = CreateBitmap(bitmap.bmWidth, bitmap.bmHeight, 1, 1, NULL);
			HPEN oldMaskPen = (HPEN)SelectObject(g_hdcMask, g_bmpMask);
			BitBlt(g_hdcMask, 0, 0, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCCOPY);

			BitBlt(g_hdcBuffer, m_iX-m_iOffsetX, m_iY-m_iOffsetY, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCINVERT);
			BitBlt(g_hdcBuffer, m_iX-m_iOffsetX, m_iY-m_iOffsetY, bitmap.bmWidth, bitmap.bmHeight, g_hdcMask, 0, 0, SRCAND);
			BitBlt(g_hdcBuffer, m_iX-m_iOffsetX, m_iY-m_iOffsetY, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCINVERT);

			SelectObject(g_hdcMask, oldMaskPen);
			DeleteObject(g_bmpMask);
			SelectObject(g_hdcMem, oldPen);

			m_iNowImageIndex++;
			if (m_iNowImageIndex >= m_iImageIndexMax)
			{
				m_iNowImageIndex = 0;
			}
		}
		else
		{
			HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
			HPEN oldPen = (HPEN)SelectObject(g_hdcBuffer, hBrush);

			Rectangle(g_hdcBuffer, m_iX, m_iY, m_ImpactBox.right, m_ImpactBox.bottom);

			SelectObject(g_hdcBuffer, oldPen);
			DeleteObject(hBrush);
		}

		if (m_bIsBoss)
		{
			HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
			HPEN oldPen = (HPEN)SelectObject(g_hdcBuffer, hBrush);

			int length = (ScreenWidth-40)*m_iHp/m_iHpMax;
			Rectangle(g_hdcBuffer, 20, 50, 20+length, 55);

			SelectObject(g_hdcBuffer, oldPen);
			DeleteObject(hBrush);
		}
		break;
	}
}

void EnemyPlane::AddPos( int x, int y )
{
	POINT temp = {x, y};
	posMap.push_back(temp);
	m_iDesX = posMap[0].x;
	m_iDesY = posMap[0].y;
}

void EnemyPlane::PlaneMove()
{
	m_bMoveable = true;
}

void EnemyPlane::PlanePause()
{
	m_bMoveable = false;
}

void EnemyPlane::SetBoss( bool isBoss )
{
	m_bIsBoss = isBoss;
}

void EnemyPlane::SetImageInfo( HBITMAP *images, int n )
{
	m_PlaneImage = images;
	m_iNowImageIndex = 0;
	m_iImageIndexMax = n;
}