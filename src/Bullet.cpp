#include ".\bullet.h"
#include "GameManager.h"
#include "Plane.h"
#include <math.h>

extern HDC g_hdc, g_hdcMem, g_hdcMask, g_hdcBuffer; //显示DC，内存DC，遮罩DC，缓冲DC
extern HBITMAP g_bmpMask;

extern HDC g_hdcBuffer;
extern int ScreenWidth;
extern int ScreenHeight;

extern UINT g_Time;

extern GameManager g_Manager;

const double PI = 3.1415926;

Bullet::Bullet(void)
{
	m_iX = 0;
	m_iY = 0;
	m_iState = ALIVE;
	m_iStep = STEP_1;
	m_Time = g_Time;
	m_SwitchTime = g_Time;
	m_iTarget = -1;
	m_DirectChange = -1;
}

Bullet::Bullet( Bullet *bullet)
{
	m_BulletType = bullet->m_BulletType;
	m_ShowType = bullet->m_ShowType;
	m_bMirror = bullet->m_bMirror;
	m_hImage = bullet->m_hImage;
	//m_hImage_2 = bullet->m_hImage_2;
	m_iDirection = bullet->m_iDirection;
	m_iHeight = bullet->m_iHeight;
	m_ImpactBox = bullet->m_ImpactBox;
	m_iSpeed = bullet->m_iSpeed;
	m_iState = bullet->m_iState;
	m_iWidth = bullet->m_iWidth;
	m_iX = bullet->m_iX;
	m_iY = bullet->m_iY;
	m_iStep = bullet->m_iStep;
	m_iFinalSpeed = bullet->m_iFinalSpeed;
	m_iAcceleration = bullet->m_iAcceleration;
	m_PauseTime = bullet->m_PauseTime;
	m_Time = g_Time;
	m_SwitchTime = g_Time;
	m_iOffsetX = bullet->m_iOffsetX;
	m_iOffsetY = bullet->m_iOffsetY;
	m_iTarget = -1;
	m_DirectChange = bullet->m_DirectChange;
}

Bullet::~Bullet(void)
{
}

double Bullet::GetX()
{
	return m_iX;
}

double Bullet::GetY()
{
	return m_iY;
}

void Bullet::SetX( double x )
{
	if (m_iState == DISAPPEAR)
	{
		return;
	}

	if (x < -m_iWidth || x > ScreenWidth)
	{
		m_iState = DISAPPEAR;
		return;
	}

	m_iX = x;
	SetRect(&m_ImpactBox, (int)m_iX, (int)m_iY, (int)m_iX+m_iWidth, (int)m_iY+m_iHeight);
}

void Bullet::SetY( double y )
{
	if (m_iState == DISAPPEAR)
	{
		return;
	}

	if (y < -m_iHeight || y > ScreenHeight)
	{
		m_iState = DISAPPEAR;
		return;
	}

	m_iY = y;
	SetRect(&m_ImpactBox, (int)m_iX, (int)m_iY, (int)m_iX+m_iWidth, (int)m_iY+m_iHeight);
}

void Bullet::SetPos( double x, double y )
{
	SetX(x);
	SetY(y);
}

void Bullet::SetDestPos( int x, int y )
{
	m_iDesX = x;
	m_iDesY = y;
}

double Bullet::GetSpeed()
{
	return m_iSpeed;
}

double Bullet::GetFinalSpeed()
{
	return m_iFinalSpeed;
}

double Bullet::GetAcceleration()
{
	return m_iAcceleration;
}

double Bullet::GetDirection()
{
	return m_iDirection;
}

void Bullet::SetSpeed( double speed )
{
	m_iSpeed = speed;
}

void Bullet::SetFinalSpeed( double speed )
{
	m_iFinalSpeed = speed;
}

void Bullet::SetAcceleration( double acceleration )
{
	m_iAcceleration = acceleration;
}

void Bullet::SetDirection( double direction )
{
	m_iDirection = direction;
}

void Bullet::SetDirectChange( double direct )
{
	m_DirectChange = direct;
}

void Bullet::SetPauseTime( unsigned int time )
{
	m_PauseTime = time;
}

int Bullet::GetState()
{
	return m_iState;
}

void Bullet::SetState( int state )
{
	m_iState = state;
}

int Bullet::GetStep()
{
	return m_iStep;
}

void Bullet::SetStep( int step )
{
	m_iStep = step;
}

int Bullet::GetBulletType()
{
	return m_BulletType;
}

void Bullet::SetBulletType( int type )
{
	m_BulletType = type;
}

RECT& Bullet::GetImpactBox()
{
	return m_ImpactBox;
}

void Bullet::SetImpactBox( int width, int height )
{
	m_iWidth = width;
	m_iHeight = height;
	SetRect(&m_ImpactBox, 0, 0, width, height);
}

int Bullet::GetWidth()
{
	return m_iWidth;
}

int Bullet::GetHeight()
{
	return m_iHeight;
}

void Bullet::SetImage( HBITMAP image, int type )
{
	m_hImage = image;
	m_ShowType = type;
	m_bMirror = false;
}

void Bullet::SetOffset( int x, int y )
{
	m_iOffsetX = x;
	m_iOffsetY = y;
}

void Bullet::SetTarget( int target )
{
	m_iTarget = target;
}

void Bullet::BulletMove()
{
	double radian;

	switch (m_BulletType)
	{
	case 0:	//匀速子弹
		radian = m_iDirection/180 * PI;

		SetX(m_iX + m_iSpeed*sin(radian));
		SetY(m_iY + m_iSpeed*cos(radian));
		break;

	case 1:	//暂停子弹
		if (m_iStep == STEP_1 && m_iSpeed > 0)
		{
			m_iSpeed -= m_iAcceleration;
			if (m_iSpeed <= 0)
			{
				m_iSpeed = 0;
				m_Time = g_Time;
				m_iStep = STEP_2;
			}
		} 
		else if (m_iStep == STEP_2)
		{
			 if (g_Time - m_Time > m_PauseTime)
			 {
				 m_iStep = STEP_3;
			 }
		}
		else if (m_iStep == STEP_3)
		{
			m_iSpeed += m_iAcceleration;
			if (m_iSpeed >= m_iFinalSpeed)
			{
				m_iSpeed = m_iFinalSpeed;
			}
		}

		radian = m_iDirection/180 * PI;

		SetX(m_iX + m_iSpeed*sin(radian));
		SetY(m_iY + m_iSpeed*cos(radian));
		break;

	case 2:	//暂停后变更方向
		if (m_iStep == STEP_1 && m_iSpeed > 0)
		{
			m_iSpeed -= m_iAcceleration;
			if (m_iSpeed <= 0)
			{
				m_iSpeed = 0;
				m_Time = g_Time;
				m_iStep = STEP_2;
			}
		} 
		else if (m_iStep == STEP_2)
		{
			if (g_Time - m_Time > m_PauseTime)
			{
				double temp;
				if (m_DirectChange == -1)
				{
					temp = rand()%180;
				}
				else
				{
					temp = m_DirectChange;
				}
				m_iDirection += temp;
				m_iStep = STEP_3;
			}
		}
		else if (m_iStep == STEP_3)
		{
			m_iSpeed += m_iAcceleration;
			if (m_iSpeed >= m_iFinalSpeed)
			{
				m_iSpeed = m_iFinalSpeed;
			}
		}

		radian = m_iDirection/180 * PI;

		SetX(m_iX + m_iSpeed*sin(radian));
		SetY(m_iY + m_iSpeed*cos(radian));
		break;

	case 3:	//追踪导弹
		switch (m_iStep)
		{
		case STEP_1:
			m_iAngle = m_iDirection/180 * PI;

			SetX(m_iX + m_iSpeed*sin(m_iAngle));
			SetY(m_iY + m_iSpeed*cos(m_iAngle));

			if (g_Time - m_Time > 500)
			{
				m_iStep = STEP_2;
			}
			break;

		case STEP_2:
			{
				vector<Plane *>* tempPlanes = g_Manager.GetPlaneVector();

				for (int i=1; i<(int)tempPlanes->size(); i++)
				{
					if ((*tempPlanes)[i])
					{
						m_iTarget = i;
						break;
					}
				}

				m_iStep = STEP_3;
			}
			break;

		case STEP_3:
			if (m_iTarget != -1)
			{
				Plane *target = (*g_Manager.GetPlaneVector())[m_iTarget];
				if (target)
				{
					m_iDesX = target->GetX();
					m_iDesY = target->GetY();

					m_iAngle = atan((double)(m_iDesX-m_iX)/(double)(m_iDesY-m_iY));
					m_iDirection = m_iAngle*180/PI;

					if (m_iDesY > m_iY)
					{
						SetX(m_iX + m_iSpeed*sin(m_iAngle));
						SetY(m_iY + m_iSpeed*cos(m_iAngle));
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
				}
				else
				{
					m_iTarget = -1;
				}	
			}
			else
			{
				m_iAngle = m_iDirection/180 * PI;

				SetX(m_iX + m_iSpeed*sin(m_iAngle));
				SetY(m_iY + m_iSpeed*cos(m_iAngle));
			}
			break;
		}
		break;
	}

}

void Bullet::ShowBullet()
{
	if (m_iState == DISAPPEAR)
	{
		return;
	}

	HPEN oldMaskPen;
	BITMAP bitmap;
	GetObject(m_hImage, sizeof(bitmap), &bitmap);
	HPEN oldPen = (HPEN)SelectObject(g_hdcMem, m_hImage);
	SetBkColor(g_hdcMem, RGB(0, 0, 0));

	switch (m_ShowType)
	{
	case 1:
		g_bmpMask = CreateBitmap(bitmap.bmWidth, bitmap.bmHeight, 1, 1, NULL);
		oldMaskPen = (HPEN)SelectObject(g_hdcMask, g_bmpMask);
		BitBlt(g_hdcMask, 0, 0, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCCOPY);

		BitBlt(g_hdcBuffer, (int)(m_iX-m_iOffsetX), (int)(m_iY-m_iOffsetY), bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCINVERT);
		BitBlt(g_hdcBuffer, (int)(m_iX-m_iOffsetX), (int)(m_iY-m_iOffsetY), bitmap.bmWidth, bitmap.bmHeight, g_hdcMask, 0, 0, SRCAND);
		BitBlt(g_hdcBuffer, (int)(m_iX-m_iOffsetX), (int)(m_iY-m_iOffsetY), bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCINVERT);
		break;

	case 2:
		g_bmpMask = CreateBitmap(bitmap.bmWidth, bitmap.bmHeight, 1, 1, NULL);
		oldMaskPen = (HPEN)SelectObject(g_hdcMask, g_bmpMask);
		BitBlt(g_hdcMask, 0, 0, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCCOPY);

		if (m_bMirror)
		{
			StretchBlt(g_hdcBuffer, (int)(m_iX-m_iOffsetX), (int)(m_iY-m_iOffsetY), bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, bitmap.bmHeight, bitmap.bmWidth, -bitmap.bmHeight, SRCINVERT);
			StretchBlt(g_hdcBuffer, (int)(m_iX-m_iOffsetX), (int)(m_iY-m_iOffsetY), bitmap.bmWidth, bitmap.bmHeight, g_hdcMask, 0, bitmap.bmHeight, bitmap.bmWidth, -bitmap.bmHeight, SRCAND);
			StretchBlt(g_hdcBuffer, (int)(m_iX-m_iOffsetX), (int)(m_iY-m_iOffsetY), bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, bitmap.bmHeight, bitmap.bmWidth, -bitmap.bmHeight, SRCINVERT);
		}
		else
		{
			BitBlt(g_hdcBuffer, (int)(m_iX-m_iOffsetX), (int)(m_iY-m_iOffsetY), bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCINVERT);
			BitBlt(g_hdcBuffer, (int)(m_iX-m_iOffsetX), (int)(m_iY-m_iOffsetY), bitmap.bmWidth, bitmap.bmHeight, g_hdcMask, 0, 0, SRCAND);
			BitBlt(g_hdcBuffer, (int)(m_iX-m_iOffsetX), (int)(m_iY-m_iOffsetY), bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCINVERT);
		}

		if ((g_Time - m_SwitchTime > 100) && (m_iStep != STEP_2))
		{
			m_bMirror = !m_bMirror;
			m_SwitchTime = g_Time;
		}	
		break;

	case 3:
		while (m_iDirection >=360)
		{
			m_iDirection -= 360;
		}
		while(m_iDirection < 0)
		{
			m_iDirection += 360;
		}
		int nowFrame = (int)(m_iDirection/45);
		g_bmpMask = CreateBitmap(bitmap.bmWidth>>3, bitmap.bmHeight, 1, 1, NULL);
		oldMaskPen = (HPEN)SelectObject(g_hdcMask, g_bmpMask);
		BitBlt(g_hdcMask, 0, 0, bitmap.bmWidth>>3, bitmap.bmHeight, g_hdcMem, nowFrame*(bitmap.bmWidth>>3), 0, SRCCOPY);

		BitBlt(g_hdcBuffer, (int)(m_iX-m_iOffsetX), (int)(m_iY-m_iOffsetY), bitmap.bmWidth>>3, bitmap.bmHeight, g_hdcMem, nowFrame*(bitmap.bmWidth>>3), 0, SRCINVERT);
		BitBlt(g_hdcBuffer, (int)(m_iX-m_iOffsetX), (int)(m_iY-m_iOffsetY), bitmap.bmWidth>>3, bitmap.bmHeight, g_hdcMask, 0, 0, SRCAND);
		BitBlt(g_hdcBuffer, (int)(m_iX-m_iOffsetX), (int)(m_iY-m_iOffsetY), bitmap.bmWidth>>3, bitmap.bmHeight, g_hdcMem, nowFrame*(bitmap.bmWidth>>3), 0, SRCINVERT);
		break;
	}

	SelectObject(g_hdcMask, oldMaskPen);
	DeleteObject(g_bmpMask);
	SelectObject(g_hdcMem, oldPen);
}