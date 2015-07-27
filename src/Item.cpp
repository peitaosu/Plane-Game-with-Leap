#include ".\item.h"
#include "GameManager.h"
#include "HeroPlane.h"
#include <math.h>

extern int ScreenWidth;
extern int ScreenHeight;

extern HDC g_hdc, g_hdcMem, g_hdcMask, g_hdcBuffer; //œ‘ æDC£¨ƒ⁄¥ÊDC£¨’⁄’÷DC£¨ª∫≥ÂDC
extern HDC g_hdcBuffer;
extern HBITMAP g_bmpMask;

extern GameManager g_Manager;

const double PI = 3.1415926;

Item::Item(void)
{
	m_iX = 0;
	m_iY = 0;

	m_ItemStep = I_STEP_1;
}

Item::~Item(void)
{
}

int Item::GetItemType()
{
	return m_ItemType;
}

void Item::SetItemType( int type )
{
	m_ItemType = type;
}

RECT& Item::GetImpactBox()
{
	return m_ImpactBox;
}

void Item::SetImpactBox( int width, int height )
{
	SetRect(&m_ImpactBox, 0, 0, width, height);
}

int Item::GetX()
{
	return m_iX;
}

int Item::GetY()
{
	return m_iY;
}

void Item::SetX( int x )
{
	if (m_iState == I_DISAPPEAR)
	{
		return;
	}

	if (x < 0 || x > ScreenWidth)
	{
		return;
	}

	OffsetRect(&m_ImpactBox, x-m_iX, 0);
	m_iX = x;
}

void Item::SetY( int y )
{
	if (m_iState == I_DISAPPEAR)
	{
		return;
	}
	else if (y > ScreenHeight)
	{
		m_iState = I_DISAPPEAR;
		return;
	}
	OffsetRect(&m_ImpactBox, 0, y-m_iY);
	m_iY = y;
}

void Item::SetPos( int x, int y )
{
	SetX(x);
	SetY(y);
}

int Item::GetSpeed()
{
	return m_iSpeed;
}

void Item::SetSpeed( int speed )
{
	m_iSpeed = speed;
}

void Item::SetDirection( int direction )
{
	m_iAngle = (double)direction / 180 * PI;
}

int Item::GetState()
{
	return m_iState;
}

void Item::SetState( int state )
{
	m_iState = state;
}

void Item::ItemMove()
{
	switch (m_ItemStep)
	{
	case I_STEP_1:
		if (m_iSpeed > 0)
		{
			SetX(m_iX + (int)(m_iSpeed*sin(m_iAngle)));
			SetY(m_iY + (int)(m_iSpeed*cos(m_iAngle)));
			m_iSpeed--;
		}
		else
		{
			m_ItemStep = I_STEP_2;
		}
		break;

	case I_STEP_2:
		switch (m_ItemType)
		{
		case I_EVER:
			m_iDesX = g_Manager.GetHeroPlane()->GetX();
			m_iDesY = g_Manager.GetHeroPlane()->GetY();

			m_iAngle = atan((double)(m_iDesX-m_iX)/(double)(m_iDesY-m_iY));

			if (m_iDesY > m_iY)
			{
				SetX(m_iX + (int)(15*sin(m_iAngle)));
				SetY(m_iY + (int)(15*cos(m_iAngle)));
			}
			else if(m_iDesY < m_iY)
			{
				SetX(m_iX - (int)(15*sin(m_iAngle)));
				SetY(m_iY - (int)(15*cos(m_iAngle)));
			}
			else
			{
				if (m_iDesX > m_iX)
				{
					SetX(m_iX+15);
				}
				else
				{
					SetX(m_iX-15);
				}
			}
			break;

		default:
			if (m_iSpeed > -2)
			{
				m_iSpeed--;
			}

			SetY(m_iY - m_iSpeed);
			break;
		}
		break;
	}
}

void Item::ShowItem()
{
	BITMAP bitmap;
	GetObject(g_Manager.GetItemImage(m_ItemType), sizeof(bitmap), &bitmap);
	HPEN oldPen = (HPEN)SelectObject(g_hdcMem, g_Manager.GetItemImage(m_ItemType));

	switch (m_ItemType)
	{
	case I_EVER:
		{
			SetBkColor(g_hdcMem, RGB(0, 0, 0));

			g_bmpMask = CreateBitmap(bitmap.bmWidth, bitmap.bmHeight, 1, 1, NULL);
			HPEN oldMaskPen = (HPEN)SelectObject(g_hdcMask, g_bmpMask);
			BitBlt(g_hdcMask, 0, 0, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCCOPY);

			BitBlt(g_hdcBuffer, m_iX, m_iY, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCINVERT);
			BitBlt(g_hdcBuffer, m_iX, m_iY, bitmap.bmWidth, bitmap.bmHeight, g_hdcMask, 0, 0, SRCAND);
			BitBlt(g_hdcBuffer, m_iX, m_iY, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCINVERT);

			SelectObject(g_hdcMask, oldMaskPen);
			DeleteObject(g_bmpMask);
		}
		break;

	default:
		BitBlt(g_hdcBuffer, m_iX, m_iY, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCCOPY);
		break;
	}
	SelectObject(g_hdcMem, oldPen);

	//HBRUSH hBrush = CreateSolidBrush(RGB(0, 255, 0));
	//HPEN oldPen = (HPEN)SelectObject(g_hdcBuffer, hBrush);

	//Rectangle(g_hdcBuffer, m_iX, m_iY, m_ImpactBox.right, m_ImpactBox.bottom);

	//SelectObject(g_hdcBuffer, oldPen);
	//DeleteObject(hBrush);
}
