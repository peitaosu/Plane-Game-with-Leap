#include ".\planeguard.h"
#include <math.h>
#include "Shooter.h"
#include "GameManager.h"

extern HDC g_hdc, g_hdcMem, g_hdcMask, g_hdcBuffer; //显示DC，内存DC，遮罩DC，缓冲DC
extern HBITMAP g_bmpMask;

extern GameManager g_Manager;

const double PI = 3.1415926;

PlaneGuard::PlaneGuard(void)
{
	m_bMoveable = true;
	m_GuardImage = NULL;
}

PlaneGuard::~PlaneGuard(void)
{
}

void PlaneGuard::SetX( int x )
{
	OffsetRect(&m_ImpactBox, x-m_iX, 0);
	m_iX = x;
}

void PlaneGuard::SetY( int y )
{
	OffsetRect(&m_ImpactBox, 0, y-m_iY);
	m_iY = y;
}

void PlaneGuard::SetTarget( int target, double distance, double direction )
{
	m_Target = target;
	m_Distance = distance;
	m_iDirection = direction;
}

void PlaneGuard::SetImpactable( bool flag )
{
	m_bImpactable = flag;
}

void PlaneGuard::UpdatePlane()
{
	Plane *target = (*g_Manager.GetPlaneVector())[m_Target];

	if (!target)
	{
		m_iState = P_DISAPPEAR;
		return;
	}

	if (m_bMoveable)
	{
		m_iDirection += m_iSpeed;
		if (m_iDirection >= 360)
		{
			m_iDirection -= 360;
		}
		m_Angle = m_iDirection/180 * PI;

		SetX(target->GetX() + (int)(m_Distance*sin(m_Angle)));
		SetY(target->GetY() + (int)(m_Distance*cos(m_Angle)));

		//更新发射器的位置
		for (int i=0; i<(int)m_vShooterGroup.size(); i++)
		{
			for (int j=0; j<(int)m_vShooterGroup[i].size(); j++)
			{
				m_vShooterGroup[i][j]->SetPos(m_iX, m_iY);
			}
		}
	}

	//发射
	m_bFireOver = true;
	vector<Shooter*> *pShooters = &m_vShooterGroup[m_iNowShooterGroup];
	for (int i=0; i<(int)pShooters->size(); i++)
	{
		if (m_bFireCommand)
		{
			(*pShooters)[i]->SetCommand(true);
			m_bFireOver &= (*pShooters)[i]->Fire(false); 
		}
	}

	if (m_bFireOver)
	{
		m_iNowShooterGroup++;
		if (m_iNowShooterGroup >= (int)pShooters->size())
		{
			m_iNowShooterGroup = 0;
		}

		for (int i=0; i<(int)pShooters->size(); i++)
		{
			(*pShooters)[i]->SetCommand(false);
			(*pShooters)[i]->Reload(); 
		}
	}

	if (m_bImpactable)
	{
		//碰撞部分
	}
}

void PlaneGuard::ShowPlane()
{
	if (!m_GuardImage)
	{
		return;
	}

	BITMAP bitmap;
	GetObject(m_GuardImage[m_iNowImageIndex], sizeof(bitmap), &bitmap);
	HPEN oldPen = (HPEN)SelectObject(g_hdcMem, m_GuardImage[m_iNowImageIndex]);
	SetBkColor(g_hdcMem, RGB(0, 0, 0));

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

void PlaneGuard::SetGuardImageInfo( HBITMAP *images, int n )
{
	m_GuardImage = images;
	m_iNowImageIndex = 0;
	m_iImageIndexMax = n;
}

void PlaneGuard::SetMoveAction( bool moveable )
{
	m_bMoveable = moveable;
}