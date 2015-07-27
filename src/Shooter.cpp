#include ".\shooter.h"
#include "Bullet.h"
#include "GameManager.h"

extern GameManager g_Manager;
extern UINT g_Time;

Shooter::Shooter(void)
{
	m_Time = g_Time;
	m_iShowTime = g_Time;
	m_bCommand = false;
}

Shooter::~Shooter(void)
{
	delete m_BulletTemplate;
}

int Shooter::GetX()
{
	return m_iX;
}

int Shooter::GetY()
{
	return m_iY;
}

void Shooter::SetX( int x )
{
	OffsetRect(&m_ImpactBox, x-m_iX, 0);
	m_iX = x;
}

void Shooter::SetY( int y )
{
	OffsetRect(&m_ImpactBox, 0, y-m_iY);
	m_iY = y;
}

void Shooter::SetPos( int x, int y )
{
	OffsetRect(&m_ImpactBox, x-m_iX, y-m_iY);
	m_iX = x;
	m_iY = y;
}

double Shooter::GetDirection()
{
	return m_iDirection;
}

double Shooter::GetFireDirection()
{
	return m_iFireDirection;
}

int Shooter::GetSpeed()
{
	return m_iSpeed;
}

int Shooter::GetFireSpeed()
{
	return m_iFireSpeed;
}

bool Shooter::GetCommand()
{
	return m_bCommand;
}

void Shooter::SetDirection( double direcion )
{
	m_iDirection = direcion;
}

void Shooter::SetFireDirection( double direction )
{
	m_iFireDirection = direction;
}

void Shooter::SetSpeed( int speed )
{
	m_iSpeed = speed;
}

void Shooter::SetFireSpeed( int speed )
{
	m_iFireSpeed = speed;
}

void Shooter::SetCommand( bool command )
{
	if (m_bCommand != command)
	{
		m_iShowTime = g_Time;
	}

	m_bCommand = command;
}

int Shooter::GetState()
{
	return m_iState;
}

void Shooter::SetState( int state )
{
	m_iState = state;
}

RECT& Shooter::GetImpactBox()
{
	return m_ImpactBox;
}

void Shooter::SetImpactBox( int width, int height )
{
	SetRect(&m_ImpactBox, 0, 0, width, height);
}

void Shooter::SetImage( LPCTSTR ImageName )
{

}

void Shooter::SetDelay( UINT delay )
{
	m_iDelay = delay;
}

void Shooter::SetReloadMax( int max )
{
	m_iReloadMax = max;
	m_iRemainBullets = max;
}

void Shooter::Reload()
{
	m_iRemainBullets = m_iReloadMax;
}

void Shooter::ShooterMove()
{

}

bool Shooter::Fire(bool camp)
{
	if (g_Time - m_iShowTime < m_iDelay)
	{
		return false;
	}

	if (m_bCommand && m_iRemainBullets && g_Time-m_Time>=(UINT)m_iFireSpeed)
	{
		Bullet* temp;

		temp = ReloadBullet();
		g_Manager.AddBulletToVector(temp, camp);

		m_Time = g_Time;
		if (m_iRemainBullets > 0)
		{
			m_iRemainBullets--;
		}
	}

	if (m_iRemainBullets == 0)
	{
		return true;
	}

	return false;
}

void Shooter::ShowShooter()
{

}

Bullet* Shooter::ReloadBullet()
{
	Bullet *temp;

	temp = new Bullet(m_BulletTemplate);
	temp->SetDirection(m_iFireDirection);
	temp->SetPos(m_iX, m_iY);
	
	switch (temp->GetBulletType())
	{
	case 0:
	case 3:
		break;

	case 1:
	case 2:
		temp->SetSpeed(temp->GetSpeed() - (temp->GetWidth()>>1)*(m_iReloadMax-m_iRemainBullets));
		break;
	}

	return temp;
}

void Shooter::SetBulletTemplate( Bullet &BulletTemplate )
{
	m_BulletTemplate = new Bullet;
	*m_BulletTemplate = BulletTemplate;
}