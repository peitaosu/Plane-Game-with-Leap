#include ".\plane.h"
#include "Shooter.h"
#include "Item.h"
#include "GameManager.h"

extern GameManager g_Manager;

extern HDC g_hdc, g_hdcMem, g_hdcMask, g_hdcBuffer; //显示DC，内存DC，遮罩DC，缓冲DC
extern HBITMAP g_bmpMask;

extern int ScreenWidth;
extern int ScreenHeight;

extern UINT g_Time;

Plane::Plane(void)
{
	m_iX = 0;
	m_iY = 0;
	m_iState = P_ALIVE;
	m_InvincibleTime = 5000;
	m_iNowShooterGroup = 0;
	ClearShooter();
}

Plane::~Plane(void)
{
	//释放发射器
	for (int i=0; i<(int)m_vShooterGroup.size(); i++)
	{
		for (int j=0; j<(int)m_vShooterGroup[i].size(); j++)
		{
			if (!m_vShooterGroup[i][j]) continue;
			
			delete m_vShooterGroup[i][j];
			m_vShooterGroup[i][j] = NULL;
		}
	}

	//释放物品
	if (m_iState == P_DISAPPEAR)
	{
		for (int i=0; i<(int)items.size(); i++)
		{
			if(!items[i]) continue;

			delete items[i];
			items[i] = NULL;
		}
	}
}

int Plane::GetHp()
{
	return m_iHp;
}

int Plane::GetAtk()
{
	return m_iAtk;
}

int Plane::GetDef()
{
	return m_iDef;
}

int Plane::GetSpeed()
{
	return m_iSpeed;
}

void Plane::SetHp( int Hp )
{
	m_iHp = Hp;
	m_iHpMax = Hp;
}

void Plane::SetAtk( int Atk )
{
	m_iAtk = Atk;
}

void Plane::SetDef( int Def )
{
	m_iDef = Def;
}

void Plane::SetSpeed( int Speed )
{
	m_iSpeed = Speed;
}

int Plane::GetX()
{
	return m_iX;
}

int Plane::GetY()
{
	return m_iY;
}

int Plane::GetState()
{
	return m_iState;
}

void Plane::SetX( int x )
{
	if (x < 0 || x > ScreenWidth - m_iWidth)
	{
		return;
	}
	OffsetRect(&m_ImpactBox, x-m_iX, 0);
	m_iX = x;
}

void Plane::SetY( int y )
{
	if (y < 0 || y > ScreenHeight - m_iHeight)
	{
		return;
	}
	OffsetRect(&m_ImpactBox, 0, y-m_iY);
	m_iY = y;
}

void Plane::SetPos( int x, int y )
{
	SetX(x);
	SetY(y);
}

void Plane::SetState( int State )
{
	m_iState = State;
}

RECT& Plane::GetImpactBox()
{
	return m_ImpactBox;
}

void Plane::SetImpactBox( int width, int height )
{
	m_iWidth = width;
	m_iHeight = height;
	SetRect(&m_ImpactBox, 0, 0, width, height);
}

void Plane::SetImage( HBITMAP image )
{
	m_hImage = image;
}

void Plane::SetOffset( int x, int y )
{
	m_iOffsetX = x;
	m_iOffsetY = y;
}

BOOL Plane::IsImpact( RECT& ImpactBox )
{
	RECT impact;

	IntersectRect(&impact, &m_ImpactBox, &ImpactBox);
	
	if (!IsRectEmpty(&impact))
	{
		return TRUE;
	}

	return FALSE;
}

void Plane::SetFireCommand( bool command )
{
	m_bFireCommand = command;
}

void Plane::UpdatePlane()
{

}

void Plane::ShowPlane()
{

}

void Plane::AddShooter( vector<Shooter*> &shooters )
{
	m_vShooterGroup.push_back(shooters);
}

void Plane::ClearShooter()
{
	m_vShooterGroup.clear();
}

void Plane::AddItem( int ItemType, int ItemNum )
{
	for (int i=0; i<ItemNum; i++)
	{
		Item *tempItem = new Item;
		tempItem->SetItemType(ItemType);
		tempItem->SetState(I_ALIVE);
		tempItem->SetSpeed(15 + (rand()%5));
		tempItem->SetDirection(150 + (rand()%60));

		switch (ItemType)
		{
		case I_P_SMALL:
			tempItem->SetImpactBox(12, 12);
			break;

		case I_P_BIG:
			tempItem->SetImpactBox(16, 16);
			break;

		case I_BOOM:
			tempItem->SetImpactBox(16, 16);
			break;

		case I_LV_1:
			tempItem->SetImpactBox(16, 16);
			break;

		case I_POINT:
			tempItem->SetImpactBox(12, 12);
			break;

		case I_EVER:
			tempItem->SetSpeed(15);
			tempItem->SetDirection(90 + (rand()%180));
			tempItem->SetImpactBox(12, 12);
			break;	
		}
		tempItem->SetPos(m_iX, m_iY);

		items.push_back(tempItem);
	}
}

void Plane::ClearItem()
{
	items.clear();
}

void Plane::SetExplodeInfo( HBITMAP* images, int n )
{
	m_ExplodeImage = images;
	m_iNowExplodeIndex = 0;
	m_iExplodeIndexMax = n;
}

void Plane::PlaneExplode()
{
	BITMAP bitmap;
	GetObject(m_ExplodeImage[m_iNowExplodeIndex], sizeof(bitmap), &bitmap);
	HPEN oldPen = (HPEN)SelectObject(g_hdcMem, m_ExplodeImage[m_iNowExplodeIndex]);
	SetBkColor(g_hdcMem, RGB(0, 0, 0));

	g_bmpMask = CreateBitmap(bitmap.bmWidth, bitmap.bmHeight, 1, 1, NULL);
	HPEN oldMaskPen = (HPEN)SelectObject(g_hdcMask, g_bmpMask);
	BitBlt(g_hdcMask, 0, 0, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCCOPY);

	BitBlt(g_hdcBuffer, m_iX-25, m_iY-25, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCINVERT);
	BitBlt(g_hdcBuffer, m_iX-25, m_iY-25, bitmap.bmWidth, bitmap.bmHeight, g_hdcMask, 0, 0, SRCAND);
	BitBlt(g_hdcBuffer, m_iX-25, m_iY-25, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCINVERT);

	SelectObject(g_hdcMask, oldMaskPen);
	DeleteObject(g_bmpMask);
	SelectObject(g_hdcMem, oldPen);
	if (g_Time - m_Time > 50)
	{
		m_iNowExplodeIndex++;
		if (m_iNowExplodeIndex >= m_iExplodeIndexMax)
		{
			m_iNowExplodeIndex = 0;
			m_bExplodeOver = true;
		}
		m_Time = g_Time;
	}
}