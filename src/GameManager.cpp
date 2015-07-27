#include ".\gamemanager.h"
#include "Music.h"
#include "Plane.h"
#include "Shooter.h"
#include "Bullet.h"
#include "Item.h"
#include "HeroPlane.h"
#include "EnemyPlane.h"
#include "PlaneGuard.h"
#include <fstream>
#include "resource.h"

extern HDC g_hdc, g_hdcMem, g_hdcMask, g_hdcBuffer; //显示DC，内存DC，遮罩DC，缓冲DC
extern HBITMAP g_bmpMask;

extern BOOL g_GamePause;

extern int ScreenWidth;
extern int ScreenHeight;
extern HWND hWnd;
extern HINSTANCE g_hInstance;

extern UINT g_Time;
extern Music g_Music;

extern LRESULT CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);

GameManager::GameManager(void)
{
	//选项初始化
	m_MenuStrings[0].assign("START");
	m_MenuStrings[1].assign("BGM");
	m_MenuStrings[2].assign("RANK");
	m_MenuStrings[3].assign("EXIT");

	m_MusicStrings[0].assign("01. 永夜抄　～ Eastern Night");
	m_MusicStrings[1].assign("02. 幻視の夜　～ Ghostly Eyes");
	m_MusicStrings[2].assign("03. 蠢々秋月　～ Mooned Insect");
	m_MusicStrings[3].assign("04. 夜雀の歌声　～ Night Bird");
	m_MusicStrings[4].assign("05. もう歌しか聞こえない");
	m_MusicStrings[5].assign("06. 懐かしき東方の血　～ Old World");
	m_MusicStrings[6].assign("07. プレインエイジア");
	m_MusicStrings[7].assign("08. 永夜の報い　～ Imperishable Night");
	m_MusicStrings[8].assign("09. 少女綺想曲　～ Dream Battle");
	m_MusicStrings[9].assign("10. 恋色マスタースパーク");
	m_MusicStrings[10].assign("11. シンデレラケージ　～ Kagome-Kagome");
	m_MusicStrings[11].assign("12. 狂気の瞳　～ Invisible Full Moon");
	m_MusicStrings[12].assign("13. ヴォヤージュ1969");
	m_MusicStrings[13].assign("14. 千年幻想郷　～ History of the Moon");
	m_MusicStrings[14].assign("15. 竹取飛翔　～ Lunatic Princess");
	m_MusicStrings[15].assign("16. ヴォヤージュ1970");
	m_MusicStrings[16].assign("17. エクステンドアッシュ　～ 蓬莱人");
	m_MusicStrings[17].assign("18. 月まで届け、不死の煙");
	m_MusicStrings[18].assign("19. 月見草");
	m_MusicStrings[19].assign("20. Eternal Dream　～ 幽玄の槭樹");
	m_MusicStrings[20].assign("21. 東方妖怪小町");

	//元素控制
	planes.clear();
	shooters.clear();
	myBullets.clear();
	ememyBullets.clear();
	items.clear();

	//资源载入
	ReadRecord();
	LoadGameImage();

	//状态跳转
	GotoMenu();

	m_Time = g_Time;
	m_ScriptMode = INIT;
	m_Process = 0;
	m_bProcessOver = true;

	m_iScore = 0;
}

GameManager::~GameManager(void)
{
	ClearAll();
}

void GameManager::ClearAll()
{
	for (int i=0; i<(int)planes.size(); i++)
	{
		delete planes[i];
	}

	for (int i=0; i<(int)shooters.size(); i++)
	{
		delete shooters[i];
	}

	for (int i=0; i<(int)myBullets.size(); i++)
	{
		delete myBullets[i];
	}

	for (int i=0; i<(int)ememyBullets.size(); i++)
	{
		delete ememyBullets[i];
	}

	for (int i=0; i<(int)items.size(); i++)
	{
		delete items[i];
	}

	planes.clear();
	shooters.clear();
	myBullets.clear();
	ememyBullets.clear();
	items.clear();
}

template<class T>
void GameManager::SetVectorEmpty( vector<T> &pool )
{
	pool.clear();
}

HeroPlane* GameManager::GetHeroPlane()
{
	return heroPlane;
}

void GameManager::UpdateGame()
{
	//进度控制
	m_bProcessOver = true;

	for (int i=1; i<(int)planes.size(); i++)
	{
		if (planes[i])
		{
			m_bProcessOver = false;
			break;
		}
	}
	if (m_bProcessOver)
	{
		m_Process++;
	}

	for (int i=0; i<(int)planes.size(); i++)
	{
		if (!planes[i])
		{
			continue;
		}
		planes[i]->UpdatePlane();

		if (planes[i]->GetState() == P_DISAPPEAR)
		{
			delete planes[i];
			planes[i] = NULL;
		}
	}

	for (int i=0; i<(int)shooters.size(); i++)
	{
		if (!shooters[i])
		{
			continue;
		}
		shooters[i]->ShooterMove();
	}

	for (int i=0; i<(int)myBullets.size(); i++)
	{
		if (!myBullets[i])
		{
			continue;
		}

		myBullets[i]->BulletMove();
		if (myBullets[i]->GetState() == DISAPPEAR)
		{
			delete myBullets[i];
			myBullets[i] = NULL;
		}
	}

	for (int i=0; i<(int)ememyBullets.size(); i++)
	{
		if (!ememyBullets[i])
		{
			continue;
		}
		ememyBullets[i]->BulletMove();
		if (ememyBullets[i]->GetState() == DISAPPEAR)
		{
			delete ememyBullets[i];
			ememyBullets[i] = NULL;
		}
	}

	for (int i=0; i<(int)items.size(); i++)
	{
		if (!items[i])
		{
			continue;
		}
		items[i]->ItemMove();
		if (items[i]->GetState() == I_DISAPPEAR)
		{
			delete items[i];
			items[i] = NULL;
		}
	}
}

void GameManager::ShowGame()
{
	ShowBackground();

	for (int i=0; i<(int)shooters.size(); i++)
	{
		if (!shooters[i])
		{
			continue;
		}
		shooters[i]->ShowShooter();
	}

	for (int i=0; i<(int)planes.size(); i++)
	{
		if (!planes[i])
		{
			continue;
		}
		planes[i]->ShowPlane();
	}

	for (int i=0; i<(int)myBullets.size(); i++)
	{
		if (!myBullets[i])
		{
			continue;
		}
		myBullets[i]->ShowBullet();
	}

	for (int i=0; i<(int)ememyBullets.size(); i++)
	{
		if (!ememyBullets[i])
		{
			continue;
		}
		ememyBullets[i]->ShowBullet();
	}

	for (int i=0; i<(int)items.size(); i++)
	{
		if (!items[i])
		{
			continue;
		}
		items[i]->ShowItem();
	}

	ShowUI();
}

vector<Bullet*>* GameManager::GetMyBulletVector()
{
	return &myBullets;
}

vector<Bullet*>* GameManager::GetEmemyBulletVector()
{
	return &ememyBullets;
}

vector<Plane*>* GameManager::GetPlaneVector()
{
	return &planes;
}

vector<Shooter*>* GameManager::GetShooterVector()
{
	return &shooters;
}

vector<Item*>* GameManager::GetItemVector()
{
	return &items;
}

void GameManager::AddBulletToVector( Bullet* bullet, bool type )
{
	if (type)
	{
		for (int i=0; i<(int)myBullets.size(); i++)
		{
			if (!myBullets[i])
			{
				myBullets[i] = bullet;
				return;
			}
		}

		myBullets.push_back(bullet);
	} 
	else
	{
		for (int i=0; i<(int)ememyBullets.size(); i++)
		{
			if (!ememyBullets[i])
			{
				ememyBullets[i] = bullet;
				return;
			}
		}

		ememyBullets.push_back(bullet);
	}
}

int GameManager::AddPlaneToVector( Plane* plane )
{
	int i;
	for (i=0; i<(int)planes.size(); i++)
	{
		if (!planes[i])
		{
			planes[i] = plane;
			return i;
		}
	}

	planes.push_back(plane);
	return i;
}

void GameManager::AddShooterToVector( Shooter* shooter )
{
	for (int i=0; i<(int)shooters.size(); i++)
	{
		if (!shooters[i])
		{
			shooters[i] = shooter;
			return;
		}
	}

	shooters.push_back(shooter);
}

void GameManager::AddItemToVector( Item* item )
{
	for (int i=0; i<(int)items.size(); i++)
	{
		if (!items[i])
		{
			items[i] = item;
			return;
		}
	}

	items.push_back(item);
}

void GameManager::GameScript()
{
	vector<Shooter*> tempVector;
	switch (m_ScriptMode)
	{
	case INIT:
		{
			//玩家主控飞机
			heroPlane = new HeroPlane;
			heroPlane->SetImage(m_HeroImage);
			heroPlane->SetBoomInfo(m_Boom_1, 5);
			heroPlane->SetInvincibleEffect(m_InvincibleEffect, 30);
			heroPlane->SetLevelUpEffect(m_LevelUpEffect);
			heroPlane->SetOffset(8, 4);;
			heroPlane->SetExplodeInfo(m_Crash, 8);
			heroPlane->SetLifeMax(7);
			heroPlane->SetBombMax(7);
			heroPlane->SetPowerMax(10);
			heroPlane->SetSpeed(4);
			heroPlane->SetImpactBox(10, 10);
			heroPlane->SetPos(ScreenWidth>>1, ScreenHeight-50);

			AddPlaneToVector(heroPlane);

			//创建子弹模板
			Bullet tempBullet_1;//匀速子弹
			tempBullet_1.SetImage(m_Bullet_2[8], 1);
			tempBullet_1.SetOffset(5, 5);
			tempBullet_1.SetImpactBox(5, 5);
			tempBullet_1.SetSpeed(15);
			tempBullet_1.SetBulletType(0);

			Bullet tempBullet_2;//追踪导弹
			tempBullet_2.SetImage(m_Bullet_2[5], 1);
			tempBullet_2.SetOffset(5, 5);
			tempBullet_2.SetImpactBox(5, 5);
			tempBullet_2.SetSpeed(10);
			tempBullet_2.SetBulletType(3);

			//创建发射器

			//LV_1
			Shooter* tempShooter = new Shooter;
			tempShooter->SetFireDirection(180);
			tempShooter->SetFireSpeed(100);
			tempShooter->SetPos(heroPlane->GetX(), heroPlane->GetY());
			tempShooter->SetReloadMax(-1);
			tempShooter->SetDelay(0);
			tempShooter->SetBulletTemplate(tempBullet_1);
			tempVector.push_back(tempShooter);

			//LV_2
			tempShooter = new Shooter;
			tempShooter->SetFireDirection(150);
			tempShooter->SetFireSpeed(500);
			tempShooter->SetPos(heroPlane->GetX(), heroPlane->GetY());
			tempShooter->SetReloadMax(-1);
			tempShooter->SetDelay(0);
			tempShooter->SetBulletTemplate(tempBullet_2);
			tempVector.push_back(tempShooter);

			tempShooter = new Shooter;
			tempShooter->SetFireDirection(210);
			tempShooter->SetFireSpeed(500);
			tempShooter->SetPos(heroPlane->GetX(), heroPlane->GetY());
			tempShooter->SetReloadMax(-1);
			tempShooter->SetDelay(0);
			tempShooter->SetBulletTemplate(tempBullet_2);
			tempVector.push_back(tempShooter);

			//LV_3
			tempShooter = new Shooter;
			tempShooter->SetFireDirection(165);
			tempShooter->SetFireSpeed(100);
			tempShooter->SetPos(heroPlane->GetX(), heroPlane->GetY());
			tempShooter->SetReloadMax(-1);
			tempShooter->SetDelay(0);
			tempShooter->SetBulletTemplate(tempBullet_1);
			tempVector.push_back(tempShooter);

			tempShooter = new Shooter;
			tempShooter->SetFireDirection(195);
			tempShooter->SetFireSpeed(100);
			tempShooter->SetPos(heroPlane->GetX(), heroPlane->GetY());
			tempShooter->SetReloadMax(-1);
			tempShooter->SetDelay(0);
			tempShooter->SetBulletTemplate(tempBullet_1);
			tempVector.push_back(tempShooter);

			//LV_4
			tempShooter = new Shooter;
			tempShooter->SetFireDirection(120);
			tempShooter->SetFireSpeed(500);
			tempShooter->SetPos(heroPlane->GetX(), heroPlane->GetY());
			tempShooter->SetReloadMax(-1);
			tempShooter->SetDelay(0);
			tempShooter->SetBulletTemplate(tempBullet_2);
			tempVector.push_back(tempShooter);

			tempShooter = new Shooter;
			tempShooter->SetFireDirection(240);
			tempShooter->SetFireSpeed(500);
			tempShooter->SetPos(heroPlane->GetX(), heroPlane->GetY());
			tempShooter->SetReloadMax(-1);
			tempShooter->SetDelay(0);
			tempShooter->SetBulletTemplate(tempBullet_2);
			tempVector.push_back(tempShooter);

			heroPlane->AddShooter(tempVector);

			m_ScriptMode = STAGE_1;
			//SetBGImage(m_BG_yuki, 31);
			//heroPlane->PowerUp(50);
			m_Process++;
			m_bProcessOver = true;
		}
		//break;

	case STAGE_1:
		if (m_bProcessOver)
		{
			switch (m_Process)
			{
			case 1:
				{
					char buffer[256] = {0};
					sprintf(buffer, ".\\Music\\%s.mid", m_MusicStrings[1].c_str());
					g_Music.lmciMidiStop();
					g_Music.lmciMidiPlay(buffer);
				}

				SetBGImage(m_BG_ame, 12);

				for (int i=0; i<5; i++)
				{
					EnemyPlane* tempPlane = new EnemyPlane;
					tempPlane->SetImageInfo(m_Iku, 9);
					tempPlane->SetOffset(0, 5);
					tempPlane->SetHp(2);
					tempPlane->SetExplodeInfo(m_Crash, 8);
					tempPlane->SetSpeed(5);
					tempPlane->SetImpactBox(20, 20);
					tempPlane->SetPos(-10, 50);
					tempPlane->AddItem(I_P_SMALL, 1);
					tempPlane->AddItem(I_POINT, 2);
					tempPlane->AddPos(50+60*i, 150);
					tempPlane->AddPos(500, 400);
					tempPlane->SetDelay(i*1000);

					//创建子弹模板
					Bullet tempBullet;
					tempBullet.SetImage(m_Bullet_1[4], 1);
					tempBullet.SetOffset(5, 5);
					tempBullet.SetImpactBox(5, 5);
					tempBullet.SetSpeed(12);
					tempBullet.SetFinalSpeed(3);
					tempBullet.SetAcceleration(1);
					tempBullet.SetPauseTime(1000);
					tempBullet.SetBulletType(1);

					//创建发射器
					tempVector.clear();
					for (int j=0; j<360; j+=20)
					{
						Shooter* tempShooter = new Shooter;
						tempShooter->SetFireDirection(j);
						tempShooter->SetFireSpeed(500);
						tempShooter->SetPos(tempPlane->GetX(), tempPlane->GetY());
						tempShooter->SetReloadMax(2);
						tempShooter->SetDelay(1000-2*j);
						tempShooter->SetBulletTemplate(tempBullet);

						tempVector.push_back(tempShooter);
					}

					tempPlane->AddShooter(tempVector);

					AddPlaneToVector(tempPlane);
				}
				break;

			case 2:
				for (int i=0; i<5; i++)
				{
					EnemyPlane* tempPlane = new EnemyPlane;
					tempPlane->SetImageInfo(m_Iku, 9);
					tempPlane->SetOffset(0, 5);
					tempPlane->SetHp(2);
					tempPlane->SetExplodeInfo(m_Crash, 8);
					tempPlane->SetSpeed(5);
					tempPlane->SetImpactBox(20, 20);
					tempPlane->SetPos(ScreenWidth-10, 50);
					tempPlane->AddItem(I_POINT, 2);
					tempPlane->AddItem(I_P_SMALL, 1);
					tempPlane->AddPos(ScreenWidth-60*(i+1), 150);
					tempPlane->AddPos(-100, 400);
					tempPlane->SetDelay(i*1000);

					//创建子弹模板
					Bullet tempBullet;
					tempBullet.SetImage(m_Bullet_1[3], 1);
					tempBullet.SetOffset(5, 5);
					tempBullet.SetImpactBox(5, 5);
					tempBullet.SetSpeed(12);
					tempBullet.SetFinalSpeed(3);
					tempBullet.SetAcceleration(1);
					tempBullet.SetPauseTime(1000);
					tempBullet.SetBulletType(1);

					//创建发射器
					tempVector.clear();
					for (int j=0; j<360; j+=20)
					{
						Shooter* tempShooter = new Shooter;
						tempShooter->SetFireDirection(j);
						tempShooter->SetFireSpeed(500);
						tempShooter->SetPos(tempPlane->GetX(), tempPlane->GetY());
						tempShooter->SetReloadMax(2);
						tempShooter->SetDelay(1000-2*j);
						tempShooter->SetBulletTemplate(tempBullet);

						tempVector.push_back(tempShooter);
					}

					tempPlane->AddShooter(tempVector);

					AddPlaneToVector(tempPlane);
				}
				break;

			case 3:
				for (int i=0; i<5; i++)
				{
					EnemyPlane* tempPlane = new EnemyPlane;
					tempPlane->SetImageInfo(m_Komachi, 8);
					tempPlane->SetOffset(10, 5);
					tempPlane->SetHp(2);
					tempPlane->SetExplodeInfo(m_Crash, 8);
					tempPlane->SetSpeed(8);
					tempPlane->SetImpactBox(20, 20);
					tempPlane->SetPos(60*(i+1), -10);
					tempPlane->AddItem(I_P_SMALL, 1);
					tempPlane->AddItem(I_POINT, 2);
					tempPlane->AddPos(60*(i+1), 150+rand()%100);
					tempPlane->AddPos(60*(i+1), -50);
					tempPlane->SetDelay(rand()%2000+500);

					//创建子弹模板
					Bullet tempBullet;
					tempBullet.SetImage(m_Bullet_1[4], 1);
					tempBullet.SetOffset(5, 5);
					tempBullet.SetImpactBox(5, 5);
					tempBullet.SetSpeed(2);
					tempBullet.SetBulletType(0);

					//创建发射器
					tempVector.clear();
					for (int j=0; j<360; j+=20)
					{
						Shooter* tempShooter = new Shooter;
						tempShooter->SetFireDirection(j);
						tempShooter->SetFireSpeed(500);
						tempShooter->SetPos(tempPlane->GetX(), tempPlane->GetY());
						tempShooter->SetReloadMax(3);
						tempShooter->SetDelay(0);
						tempShooter->SetBulletTemplate(tempBullet);

						tempVector.push_back(tempShooter);
					}

					tempPlane->AddShooter(tempVector);

					AddPlaneToVector(tempPlane);
				}
				break;

			case 4:
				for (int i=0; i<5; i++)
				{
					EnemyPlane* tempPlane = new EnemyPlane;
					tempPlane->SetImageInfo(m_Marisa, 10);
					tempPlane->SetOffset(1, 5);
					tempPlane->SetHp(2);
					tempPlane->SetExplodeInfo(m_Crash, 8);
					tempPlane->SetSpeed(8);
					tempPlane->SetImpactBox(20, 20);
					tempPlane->SetPos(-10, 50*(i+1));
					tempPlane->AddItem(I_P_SMALL, 1);
					tempPlane->AddItem(I_POINT, 2);
					tempPlane->AddPos(150+rand()%100, 50*(i+1));
					tempPlane->AddPos(-50, 50*(i+1));
					tempPlane->SetDelay(rand()%2000+500);

					//创建子弹模板
					Bullet tempBullet;
					tempBullet.SetImage(m_Bullet_1[5], 1);
					tempBullet.SetOffset(5, 5);
					tempBullet.SetImpactBox(5, 5);
					tempBullet.SetSpeed(2);
					tempBullet.SetBulletType(0);

					//创建发射器
					tempVector.clear();
					for (int j=0; j<360; j+=20)
					{
						Shooter* tempShooter = new Shooter;
						tempShooter->SetFireDirection(j);
						tempShooter->SetFireSpeed(500);
						tempShooter->SetPos(tempPlane->GetX(), tempPlane->GetY());
						tempShooter->SetReloadMax(2);
						tempShooter->SetDelay(0);
						tempShooter->SetBulletTemplate(tempBullet);

						tempVector.push_back(tempShooter);
					}

					tempPlane->AddShooter(tempVector);

					AddPlaneToVector(tempPlane);
				}

				for (int i=0; i<5; i++)
				{
					EnemyPlane* tempPlane = new EnemyPlane;
					tempPlane->SetImageInfo(m_Yuyuko, 10);
					tempPlane->SetOffset(1, 10);
					tempPlane->SetExplodeInfo(m_Crash, 8);
					tempPlane->SetHp(2);
					tempPlane->SetSpeed(8);
					tempPlane->SetImpactBox(20, 20);
					tempPlane->SetPos(ScreenWidth-10, 50*(i+1));
					tempPlane->AddItem(I_P_SMALL, 1);
					tempPlane->AddItem(I_POINT, 2);
					tempPlane->AddPos(150+rand()%100, 50*(i+1));
					tempPlane->AddPos(ScreenWidth+50, 50*(i+1));
					tempPlane->SetDelay(rand()%2000+500);

					//创建子弹模板
					Bullet tempBullet;
					tempBullet.SetImage(m_Bullet_1[6], 1);
					tempBullet.SetOffset(5, 5);
					tempBullet.SetImpactBox(5, 5);
					tempBullet.SetSpeed(2);
					tempBullet.SetBulletType(0);

					//创建发射器
					tempVector.clear();
					for (int j=0; j<360; j+=20)
					{
						Shooter* tempShooter = new Shooter;
						tempShooter->SetFireDirection(j);
						tempShooter->SetFireSpeed(500);
						tempShooter->SetPos(tempPlane->GetX(), tempPlane->GetY());
						tempShooter->SetReloadMax(2);
						tempShooter->SetDelay(0);
						tempShooter->SetBulletTemplate(tempBullet);

						tempVector.push_back(tempShooter);
					}

					tempPlane->AddShooter(tempVector);

					AddPlaneToVector(tempPlane);
				}
				break;

			case 5:		//BOSS
				{
					EnemyPlane* tempPlane = new EnemyPlane;
					tempPlane->SetImageInfo(m_Remilia, 8);
					tempPlane->SetExplodeInfo(m_Crash, 8);
					tempPlane->SetBoss(true);
					tempPlane->SetHp(500);
					tempPlane->SetSpeed(5);
					tempPlane->SetImpactBox(20, 20);
					tempPlane->SetOffset(25, 20);
					tempPlane->SetPos(-10, -10);
					tempPlane->AddItem(I_POINT, 5);
					tempPlane->AddItem(I_P_SMALL, 3);
					tempPlane->AddItem(I_P_BIG, 2);
					tempPlane->AddItem(I_BOOM, 1);
					tempPlane->AddItem(I_LV_1, 1);
					tempPlane->AddPos(100, 100);
					tempPlane->AddPos(300, 100);
					tempPlane->AddPos(300, 200);
					tempPlane->AddPos(100, 200);
					tempPlane->SetDelay(rand()%2000+500);
					
					//发射器组---------1
					//创建子弹模板
					Bullet tempBullet;
					tempBullet.SetImage(m_Bullet_1[7], 1);
					tempBullet.SetOffset(5, 5);
					tempBullet.SetImpactBox(5, 5);
					tempBullet.SetSpeed(2);
					tempBullet.SetBulletType(0);

					//创建发射器
					tempVector.clear();
					for (int j=0; j<360; j+=10)
					{
						Shooter* tempShooter = new Shooter;
						tempShooter->SetFireDirection(j);
						tempShooter->SetFireSpeed(500);
						tempShooter->SetPos(tempPlane->GetX(), tempPlane->GetY());
						tempShooter->SetReloadMax(5);
						tempShooter->SetDelay(0);
						tempShooter->SetBulletTemplate(tempBullet);

						tempVector.push_back(tempShooter);
					}

					tempPlane->AddShooter(tempVector);

					//发射器组---------2
					//创建子弹模板
					tempBullet.SetImage(m_Bullet_3[1], 3);
					tempBullet.SetOffset(5, 4);
					tempBullet.SetImpactBox(6, 8);
					tempBullet.SetSpeed(20);
					tempBullet.SetFinalSpeed(3);
					tempBullet.SetAcceleration(1);
					tempBullet.SetPauseTime(2000);
					tempBullet.SetBulletType(2);

					//创建发射器
					tempVector.clear();
					for (int j=0; j<360; j+=10)
					{
						Shooter* tempShooter = new Shooter;
						tempShooter->SetFireDirection(j);
						tempShooter->SetFireSpeed(500);
						tempShooter->SetPos(tempPlane->GetX(), tempPlane->GetY());
						tempShooter->SetReloadMax(3);
						tempShooter->SetDelay(1000-2*j);
						tempShooter->SetBulletTemplate(tempBullet);

						tempVector.push_back(tempShooter);
					}

					tempPlane->AddShooter(tempVector);

					//发射器组---------3
					//创建子弹模板
					tempBullet.SetOffset(5, 5);
					tempBullet.SetImage(m_Bullet_1[3], 1);
					tempBullet.SetImpactBox(5, 5);
					tempBullet.SetSpeed(20);
					tempBullet.SetFinalSpeed(3);
					tempBullet.SetAcceleration(1);
					tempBullet.SetPauseTime(3000);
					tempBullet.SetBulletType(1);

					//创建发射器
					tempVector.clear();
					for (int j=0; j<360; j+=10)
					{
						Shooter* tempShooter = new Shooter;
						tempShooter->SetFireDirection(j);
						tempShooter->SetFireSpeed(200);
						tempShooter->SetPos(tempPlane->GetX(), tempPlane->GetY());
						tempShooter->SetReloadMax(5);
						tempShooter->SetDelay(1000-2*j);
						tempShooter->SetBulletTemplate(tempBullet);

						tempVector.push_back(tempShooter);
					}

					tempPlane->AddShooter(tempVector);
					
					int boss = AddPlaneToVector(tempPlane);

					for (int i=0; i<4; i++)
					{

						PlaneGuard* tempPlaneGuard = new PlaneGuard;
						tempPlaneGuard->SetImpactBox(14, 16);
						tempPlaneGuard->SetOffset(10, 8);
						tempPlaneGuard->SetGuardImageInfo(m_GuardImage[i], 10);
						tempPlaneGuard->SetImpactable(false);
						tempPlaneGuard->SetTarget(boss, 50, 90*i);
						tempPlaneGuard->SetSpeed(4);

						//创建子弹模板
						Bullet tempBullet;
						tempBullet.SetImage(m_Bullet_3[2], 3);
						tempBullet.SetOffset(5, 5);
						tempBullet.SetImpactBox(5, 5);
						tempBullet.SetSpeed(3);
						tempBullet.SetBulletType(0);

						tempVector.clear();
						for (int j=0; j<4; j++)
						{
							Shooter* tempShooter = new Shooter;
							tempShooter->SetFireDirection(j*90);
							tempShooter->SetFireSpeed(100);
							tempShooter->SetPos(tempPlaneGuard->GetX(), tempPlaneGuard->GetY());
							tempShooter->SetReloadMax(1);
							tempShooter->SetDelay(500);
							tempShooter->SetBulletTemplate(tempBullet);

							tempVector.push_back(tempShooter);
						}
						tempPlaneGuard->AddShooter(tempVector);

						tempVector.clear();
						for (int j=0; j<4; j++)
						{
							Shooter* tempShooter = new Shooter;
							tempShooter->SetFireDirection(j*90-45);
							tempShooter->SetFireSpeed(100);
							tempShooter->SetPos(tempPlaneGuard->GetX(), tempPlaneGuard->GetY());
							tempShooter->SetReloadMax(1);
							tempShooter->SetDelay(500);
							tempShooter->SetBulletTemplate(tempBullet);

							tempVector.push_back(tempShooter);
						}
						tempPlaneGuard->AddShooter(tempVector);
	
						//AddPlaneToVector(tempPlaneGuard);////
					}
					
				}
				break;

			case 6:
				{
					bool MissionComplete = true;
					for (int i=0; i<(int)items.size(); i++)
					{
						if (items[i])
						{
							MissionComplete = false;
						}
					}

					if (MissionComplete)
					{
						heroPlane->SetPos(ScreenWidth>>1, ScreenHeight-50);
						m_ScriptMode = STAGE_2;
						m_Process = 0;
					}
					else
					{
						m_Process--;
					}
				}
				break;
			}
			m_bProcessOver = false;
		}
		break;

	case STAGE_2:
		if (m_bProcessOver)
		{
			switch (m_Process)
			{
			case 1:
				{
					char buffer[256] = {0};
					sprintf(buffer, ".\\Music\\%s.mid", m_MusicStrings[2].c_str());
					g_Music.lmciMidiStop();
					g_Music.lmciMidiPlay(buffer);
				}

				SetBGImage(m_BG_yuki, 31);

				for (int i=0; i<10; i++)
				{
					EnemyPlane *tempPlane = new EnemyPlane;
					tempPlane->SetImageInfo(m_Iku, 9);
					tempPlane->SetOffset(0, 5);
					tempPlane->SetHp(2);
					tempPlane->SetExplodeInfo(m_Crash, 8);
					tempPlane->SetSpeed(6);
					tempPlane->SetImpactBox(20, 20);
					tempPlane->SetPos(-10, 200);
					tempPlane->AddItem(I_POINT, 1);
					tempPlane->AddPos(500, 200);
					tempPlane->SetDelay((i+9)*300);

					AddPlaneToVector(tempPlane);
				}
				break;

			case 2:
				for (int i=0; i<10; i++)
				{
					EnemyPlane *tempPlane = new EnemyPlane;
					tempPlane->SetImageInfo(m_Marisa, 10);
					tempPlane->SetOffset(1, 5);
					tempPlane->SetHp(2);
					tempPlane->SetExplodeInfo(m_Crash, 8);
					tempPlane->SetSpeed(6);
					tempPlane->SetImpactBox(20, 20);
					tempPlane->SetPos(ScreenWidth-10, 150);
					tempPlane->AddItem(I_POINT, 1);
					tempPlane->AddPos(-100, 150);
					tempPlane->SetDelay((i+2)*300);

					AddPlaneToVector(tempPlane);
				}
				break;

			case 3:
				for (int i=0; i<5; i++)
				{
					EnemyPlane *tempPlane = new EnemyPlane;
					tempPlane->SetImageInfo(m_Yuyuko, 10);
					tempPlane->SetOffset(1, 10);
					tempPlane->SetHp(10);
					tempPlane->SetExplodeInfo(m_Crash, 8);
					tempPlane->SetSpeed(6);
					tempPlane->SetImpactBox(20, 20);
					tempPlane->SetPos(-10, 200);
					tempPlane->AddItem(I_P_SMALL, 1);
					tempPlane->AddItem(I_POINT, 1);
					tempPlane->AddPos(70*(i+1), 200);
					tempPlane->AddPos(500, 200);
					tempPlane->SetDelay((i+1)*500);
					
					//环形用
					Bullet tempBullet_1;
					tempBullet_1.SetImage(m_Bullet_1[i], 1);
					tempBullet_1.SetOffset(5, 5);
					tempBullet_1.SetImpactBox(5, 5);
					tempBullet_1.SetSpeed(3);
					tempBullet_1.SetBulletType(0);

					tempVector.clear();
					for (int j=0; j<360; j+=20)
					{
						Shooter *tempShooter = new Shooter;
						tempShooter->SetFireDirection(j);
						tempShooter->SetFireSpeed(1000);
						tempShooter->SetPos(tempPlane->GetX(), tempPlane->GetY());
						tempShooter->SetReloadMax(-1);
						tempShooter->SetDelay(0);
						tempShooter->SetBulletTemplate(tempBullet_1);
						tempVector.push_back(tempShooter);
					}
					tempPlane->AddShooter(tempVector);

					AddPlaneToVector(tempPlane);
				}
				break;

			case 4:
				for (int i=0; i<5; i++)
				{
					EnemyPlane *tempPlane = new EnemyPlane;
					tempPlane->SetImageInfo(m_Iku, 9);
					tempPlane->SetOffset(0, 5);
					tempPlane->SetHp(10);
					tempPlane->SetExplodeInfo(m_Crash, 8);
					tempPlane->SetSpeed(6);
					tempPlane->SetImpactBox(20, 20);
					tempPlane->SetPos(ScreenWidth-10, 150);
					tempPlane->AddItem(I_P_SMALL, 1);
					tempPlane->AddItem(I_POINT, 1);
					tempPlane->AddPos(70*(i+1), 150);
					tempPlane->AddPos(-100, 150);
					tempPlane->SetDelay((i+1)*500);

					//环形用
					Bullet tempBullet_1;
					tempBullet_1.SetImage(m_Bullet_1[7-i], 1);
					tempBullet_1.SetOffset(5, 5);
					tempBullet_1.SetImpactBox(5, 5);
					tempBullet_1.SetSpeed(3);
					tempBullet_1.SetBulletType(0);

					tempVector.clear();
					for (int j=0; j<360; j+=20)
					{
						Shooter *tempShooter = new Shooter;
						tempShooter->SetFireDirection(j);
						tempShooter->SetFireSpeed(1000);
						tempShooter->SetPos(tempPlane->GetX(), tempPlane->GetY());
						tempShooter->SetReloadMax(-1);
						tempShooter->SetDelay(0);
						tempShooter->SetBulletTemplate(tempBullet_1);
						tempVector.push_back(tempShooter);
					}
					tempPlane->AddShooter(tempVector);
					
					AddPlaneToVector(tempPlane);
				}
				break;

			case 5:
				for (int i=0; i<5; i++)
				{
					EnemyPlane *tempPlane = new EnemyPlane;
					tempPlane->SetImageInfo(m_Marisa, 10);
					tempPlane->SetOffset(1, 5);
					tempPlane->SetHp(10);
					tempPlane->SetExplodeInfo(m_Crash, 8);
					tempPlane->SetSpeed(6);
					tempPlane->SetImpactBox(20, 20);
					tempPlane->SetPos(70*(i+1), -10);
					tempPlane->AddItem(I_POINT, 2);
					tempPlane->AddItem(I_P_SMALL, 1);
					tempPlane->AddPos(70*(i+1), 200);
					tempPlane->AddPos(70*(i+1), -100);
					tempPlane->SetDelay((i+1)*500);

					//环形用
					Bullet tempBullet_1;
					tempBullet_1.SetImage(m_Bullet_1[i+1], 1);
					tempBullet_1.SetOffset(5, 5);
					tempBullet_1.SetImpactBox(5, 5);
					tempBullet_1.SetSpeed(3);
					tempBullet_1.SetBulletType(0);

					tempVector.clear();
					for (int j=0; j<360; j+=20)
					{
						Shooter *tempShooter = new Shooter;
						tempShooter->SetFireDirection(j);
						tempShooter->SetFireSpeed(1000);
						tempShooter->SetPos(tempPlane->GetX(), tempPlane->GetY());
						tempShooter->SetReloadMax(-1);
						tempShooter->SetDelay(0);
						tempShooter->SetBulletTemplate(tempBullet_1);
						tempVector.push_back(tempShooter);
					}
					tempPlane->AddShooter(tempVector);

					AddPlaneToVector(tempPlane);					
				}
				break;

			case 6:
				for (int i=0; i<2; i++)
				{
					EnemyPlane *tempPlane = new EnemyPlane;
					tempPlane->SetImageInfo(m_Komachi, 8);
					tempPlane->SetOffset(10, 5);
					tempPlane->SetHp(30);
					tempPlane->SetExplodeInfo(m_Crash, 8);
					tempPlane->SetSpeed(6);
					tempPlane->SetImpactBox(20, 20);
					tempPlane->SetPos((ScreenWidth/3)*(i+1), -10);
					tempPlane->AddItem(P_BOOM, 1);
					tempPlane->AddItem(I_POINT, 2);
					tempPlane->AddItem(I_P_SMALL, 1);
					tempPlane->AddPos((ScreenWidth/3)*(i+1), 200);
					tempPlane->AddPos((ScreenWidth/3)*(i+1), -100);
					tempPlane->SetDelay(1000);

					//环形用
					Bullet tempBullet_1;
					tempBullet_1.SetImage(m_Bullet_1[6-i], 1);
					tempBullet_1.SetOffset(5, 5);
					tempBullet_1.SetImpactBox(5, 5);
					tempBullet_1.SetSpeed(3);
					tempBullet_1.SetBulletType(0);

					//直线形用
					Bullet tempBullet_2;
					tempBullet_2.SetImage(m_Bullet_3[1], 3);
					tempBullet_2.SetOffset(5, 5);
					tempBullet_2.SetImpactBox(5, 5);
					tempBullet_2.SetSpeed(4);
					tempBullet_2.SetBulletType(0);

					tempVector.clear();
					for (int j=0; j<360; j+=20)
					{
						Shooter *tempShooter = new Shooter;
						tempShooter->SetFireDirection(j);
						tempShooter->SetFireSpeed(1000);
						tempShooter->SetPos(tempPlane->GetX(), tempPlane->GetY());
						tempShooter->SetReloadMax(-1);
						tempShooter->SetDelay(0);
						tempShooter->SetBulletTemplate(tempBullet_1);
						tempVector.push_back(tempShooter);
					}
					tempPlane->AddShooter(tempVector);
					tempVector.clear();

					PlaneGuard *tempPlaneGuard = new PlaneGuard;
					tempPlaneGuard->SetImpactBox(14, 16);
					tempPlaneGuard->SetOffset(10, 8);
					tempPlaneGuard->SetGuardImageInfo(m_GuardImage[i], 10);
					tempPlaneGuard->SetImpactable(false);
					tempPlaneGuard->SetTarget(AddPlaneToVector(tempPlane), 50, 45*i);
					tempPlaneGuard->SetSpeed(4);

					for (int j=0; j<4; j++)
					{
						Shooter *tempShooter = new Shooter;
						tempShooter->SetFireDirection(j*90);
						tempShooter->SetFireSpeed(500);
						tempShooter->SetPos(tempPlaneGuard->GetX(), tempPlaneGuard->GetY());
						tempShooter->SetReloadMax(-1);
						tempShooter->SetDelay(0);
						tempShooter->SetBulletTemplate(tempBullet_2);
						tempVector.push_back(tempShooter);
					}

					tempPlaneGuard->AddShooter(tempVector);
					AddPlaneToVector(tempPlaneGuard);
				}
				break;

			case 7:
				{
					EnemyPlane* tempPlane = new EnemyPlane;
					tempPlane->SetImageInfo(m_Remilia, 8);
					tempPlane->SetOffset(25, 20);
					tempPlane->SetExplodeInfo(m_Crash, 8);
					tempPlane->SetBoss(true);
					tempPlane->SetHp(1000);
					tempPlane->SetSpeed(5);
					tempPlane->SetImpactBox(20, 20);
					tempPlane->SetPos(-10, -10);
					tempPlane->AddItem(I_POINT, 5);
					tempPlane->AddItem(I_P_SMALL, 3);
					tempPlane->AddItem(I_P_BIG, 2);
					tempPlane->AddItem(I_BOOM, 1);
					tempPlane->AddItem(I_LV_1, 1);
					tempPlane->AddPos(ScreenWidth/3, 100);
					tempPlane->AddPos(ScreenWidth/5*4, 200);
					tempPlane->AddPos(ScreenWidth/5, 200);
					tempPlane->AddPos(ScreenWidth/3*2, 100);
					tempPlane->AddPos(ScreenWidth>>1, 300);
					tempPlane->SetDelay(500);

					//子弹模板-1
					Bullet tempBullet_1;
					tempBullet_1.SetOffset(5, 5);
					tempBullet_1.SetImage(m_Bullet_3[3], 3);
					tempBullet_1.SetImpactBox(5, 5);
					tempBullet_1.SetSpeed(15);
					tempBullet_1.SetFinalSpeed(2);
					tempBullet_1.SetAcceleration(1);
					tempBullet_1.SetPauseTime(2000);
					tempBullet_1.SetBulletType(1);

					//子弹模板-2
					Bullet tempBullet_2;
					tempBullet_2.SetOffset(5, 5);
					tempBullet_2.SetImage(m_Bullet_3[0], 3);
					tempBullet_2.SetImpactBox(5, 5);
					tempBullet_2.SetSpeed(15);
					tempBullet_2.SetFinalSpeed(2);
					tempBullet_2.SetAcceleration(1);
					tempBullet_2.SetPauseTime(2000);
					tempBullet_2.SetBulletType(2);

					//子弹模板-3
					Bullet tempBullet_3;
					tempBullet_3.SetImage(m_Bullet_3[1], 3);
					tempBullet_3.SetOffset(5, 5);
					tempBullet_3.SetImpactBox(5, 5);
					tempBullet_3.SetSpeed(2);
					tempBullet_3.SetBulletType(0);

					//发射器组-1
					tempVector.clear();
					for (int i=0; i<360; i+=10)
					{
						Shooter *tempShooter = new Shooter;
						tempShooter->SetFireDirection(i);
						tempShooter->SetFireSpeed(100);
						tempShooter->SetPos(tempPlane->GetX(), tempPlane->GetY());
						tempShooter->SetReloadMax(3);
						tempShooter->SetDelay(i);
						tempShooter->SetBulletTemplate(tempBullet_1);
						tempVector.push_back(tempShooter);
					}
					tempPlane->AddShooter(tempVector);

					//发射器组-2
					tempVector.clear();
					for (int i=0; i<360; i+=10)
					{
						Shooter *tempShooter = new Shooter;
						tempShooter->SetFireDirection(i);
						tempShooter->SetFireSpeed(300);
						tempShooter->SetPos(tempPlane->GetX(), tempPlane->GetY());
						tempShooter->SetReloadMax(2);
						tempShooter->SetDelay(1000-i*2);
						tempBullet_2.SetImage(m_Bullet_3[1], 3);
						tempBullet_2.SetSpeed(15);
						tempBullet_2.SetPauseTime(1000);
						tempBullet_2.SetDirectChange(90);
						tempShooter->SetBulletTemplate(tempBullet_2);
						tempVector.push_back(tempShooter);
					}
					for (int i=0; i<360; i+=10)
					{
						Shooter *tempShooter = new Shooter;
						tempShooter->SetFireDirection(i);
						tempShooter->SetFireSpeed(300);
						tempShooter->SetPos(tempPlane->GetX(), tempPlane->GetY());
						tempShooter->SetReloadMax(2);
						tempShooter->SetDelay(1000-i*2);
						tempBullet_2.SetImage(m_Bullet_3[4], 3);
						tempBullet_2.SetSpeed(15);
						tempBullet_2.SetPauseTime(1000);
						tempBullet_2.SetDirectChange(270);
						tempShooter->SetBulletTemplate(tempBullet_2);
						tempVector.push_back(tempShooter);
					}
					tempPlane->AddShooter(tempVector);

					//发射器组-3
					tempVector.clear();
					for (int i=0; i<4; i++)
					{
						Shooter *tempShooter = new Shooter;
						tempShooter->SetFireDirection(90*i);
						tempShooter->SetFireSpeed(100);
						tempShooter->SetPos(tempPlane->GetX(), tempPlane->GetY());
						tempShooter->SetReloadMax(5);
						tempShooter->SetDelay(2000);
						tempBullet_2.SetImage(m_Bullet_3[5], 3);
						tempBullet_2.SetSpeed(20);
						tempBullet_2.SetPauseTime(2000);
						tempBullet_2.SetDirectChange(90);
						tempShooter->SetBulletTemplate(tempBullet_2);
						tempVector.push_back(tempShooter);
					}
					for (int i=0; i<4; i++)
					{
						Shooter *tempShooter = new Shooter;
						tempShooter->SetFireDirection(90*i);
						tempShooter->SetFireSpeed(100);
						tempShooter->SetPos(tempPlane->GetX(), tempPlane->GetY());
						tempShooter->SetReloadMax(5);
						tempShooter->SetDelay(2000);
						tempBullet_2.SetImage(m_Bullet_3[0], 3);
						tempBullet_2.SetSpeed(20);
						tempBullet_2.SetPauseTime(2000);
						tempBullet_2.SetDirectChange(270);
						tempShooter->SetBulletTemplate(tempBullet_2);
						tempVector.push_back(tempShooter);
					}
					tempPlane->AddShooter(tempVector);

					int boss = AddPlaneToVector(tempPlane);

					//PlaneGuard
					for (int i=0; i<3; i++)
					{
						PlaneGuard *tempPlaneGuard = new PlaneGuard;
						tempPlaneGuard->SetImpactBox(14, 16);
						tempPlaneGuard->SetOffset(10, 8);
						tempPlaneGuard->SetGuardImageInfo(m_GuardImage[i], 10);
						tempPlaneGuard->SetImpactable(false);
						tempPlaneGuard->SetTarget(boss, 80, 120*i);
						tempPlaneGuard->SetSpeed(4);

						tempVector.clear();
						for (int j=0; j<3; j++)
						{
							Shooter *tempShooter = new Shooter;
							tempShooter->SetFireDirection(j*120);
							tempShooter->SetFireSpeed(1000);
							tempShooter->SetPos(tempPlaneGuard->GetX(), tempPlaneGuard->GetY());
							tempShooter->SetReloadMax(-1);
							tempShooter->SetDelay(300*j);
							tempBullet_3.SetImage(m_Bullet_1[7-i], 1);
							tempShooter->SetBulletTemplate(tempBullet_3);
							tempVector.push_back(tempShooter);
						}

						tempPlaneGuard->AddShooter(tempVector);
						//AddPlaneToVector(tempPlaneGuard);
					}

					//捣乱的

					//左边的
					for (int i=0; i<5; i++)
					{
						PlaneGuard *tempPlaneGuard = new PlaneGuard;
						tempPlaneGuard->SetImpactBox(20, 20);
						tempPlaneGuard->SetMoveAction(false);
						tempPlaneGuard->SetImpactable(false);
						tempPlaneGuard->SetPos(0, (i+1)*(ScreenHeight/6));
						tempPlaneGuard->SetTarget(boss, 0, 0);
						tempPlaneGuard->SetSpeed(4);

						tempVector.clear();
						Shooter *tempShooter = new Shooter;
						tempShooter->SetFireDirection(90);
						tempShooter->SetFireSpeed(2000+rand()%1000);
						tempShooter->SetPos(tempPlaneGuard->GetX(), tempPlaneGuard->GetY());
						tempShooter->SetReloadMax(-1);
						tempShooter->SetDelay(rand()%5000);
						tempBullet_3.SetSpeed(2);
						tempBullet_3.SetImage(m_Bullet_2[i], 2);
						tempShooter->SetBulletTemplate(tempBullet_3);
						tempVector.push_back(tempShooter);

						tempPlaneGuard->AddShooter(tempVector);
						AddPlaneToVector(tempPlaneGuard);
					}
					//右边的
					for (int i=0; i<4; i++)
					{
						PlaneGuard *tempPlaneGuard = new PlaneGuard;
						tempPlaneGuard->SetImpactBox(20, 20);
						tempPlaneGuard->SetMoveAction(false);
						tempPlaneGuard->SetImpactable(false);
						tempPlaneGuard->SetPos(ScreenWidth-10, (i+1)*(ScreenHeight/5));
						tempPlaneGuard->SetTarget(boss, 0, 0);
						tempPlaneGuard->SetSpeed(4);

						tempVector.clear();
						Shooter *tempShooter = new Shooter;
						tempShooter->SetFireDirection(270);
						tempShooter->SetFireSpeed(2000+rand()%1000);
						tempShooter->SetPos(tempPlaneGuard->GetX(), tempPlaneGuard->GetY());
						tempShooter->SetReloadMax(-1);
						tempShooter->SetDelay(rand()%5000);
						tempBullet_3.SetSpeed(2);
						tempBullet_3.SetImage(m_Bullet_2[i+5], 2);
						tempShooter->SetBulletTemplate(tempBullet_3);
						tempVector.push_back(tempShooter);

						tempPlaneGuard->AddShooter(tempVector);
						AddPlaneToVector(tempPlaneGuard);
					}
				}
				break;

			case 8:
				{
					bool MissionComplete = true;
					for (int i=0; i<(int)items.size(); i++)
					{
						if (items[i])
						{
							MissionComplete = false;
						}
					}

					if (MissionComplete)
					{
						m_ScriptMode = END;
					}
					m_Process--;
				}
				break;
			}
			m_bProcessOver = false;
		}
		break;

	case END:
		if (m_iScore > m_Record[19].record)
		{
			g_GamePause = TRUE;
			DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_NEWRECORD), hWnd, (DLGPROC)DialogProc);
		}
		else
		{
			GotoMenu();
		}
		ClearAll();
		break;
	}
}

void GameManager::SetScriptMode( int mode )
{
	m_ScriptMode = mode;
}

void GameManager::ShowBackground()
{
	BITMAP bitmap;
	GetObject(m_BGImage[m_iNowBGIndex], sizeof(bitmap), &bitmap);
	HPEN oldPen = (HPEN)SelectObject(g_hdcMem, m_BGImage[m_iNowBGIndex]);
	//BitBlt(g_hdcBuffer, 0, 0, ScreenWidth, ScreenHeight, g_hdcMem, 0, 0, SRCCOPY);
	StretchBlt(g_hdcBuffer, 0, 0, ScreenWidth, ScreenHeight, g_hdcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);

	if (g_Time - m_Time > 50)
	{
		m_iNowBGIndex++;
		if (m_iNowBGIndex >= m_iGBIndexMax)
		{
			m_iNowBGIndex = 0;
		}
		m_Time = g_Time;
	}
	SelectObject(g_hdcMem, oldPen);
}

void GameManager::ShowUI()
{
	COLORREF oldColor;

	SetBkMode(g_hdcBuffer, TRANSPARENT);
	oldColor = SetTextColor(g_hdcBuffer, RGB(255, 255, 255));
	TextOut(g_hdcBuffer, 5, 8, "Life:", 5);
	TextOut(g_hdcBuffer, 5, 28, "Boom:", 5);
	SetTextColor(g_hdcBuffer, oldColor);

	//LIFE
	for (int i=0; i<heroPlane->GetLifeNum(); i++)
	{
		BITMAP bitmap;
		GetObject(m_Bullet_2[1], sizeof(bitmap), &bitmap);
		HPEN oldPen = (HPEN)SelectObject(g_hdcMem, m_Bullet_2[1]);
		SetBkColor(g_hdcMem, RGB(0, 0, 0));

		g_bmpMask = CreateBitmap(bitmap.bmWidth, bitmap.bmHeight, 1, 1, NULL);
		HPEN oldMaskPen = (HPEN)SelectObject(g_hdcMask, g_bmpMask);
		BitBlt(g_hdcMask, 0, 0, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCCOPY);

		BitBlt(g_hdcBuffer, 50+i*12, 10, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCINVERT);
		BitBlt(g_hdcBuffer, 50+i*12, 10, bitmap.bmWidth, bitmap.bmHeight, g_hdcMask, 0, 0, SRCAND);
		BitBlt(g_hdcBuffer, 50+i*12, 10, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCINVERT);

		SelectObject(g_hdcMask, oldMaskPen);
		DeleteObject(g_bmpMask);
		SelectObject(g_hdcMem, oldPen);
	}

	//BOOM
	for (int i=0; i<heroPlane->GetBombNum(); i++)
	{
		BITMAP bitmap;
		GetObject(m_Bullet_2[2], sizeof(bitmap), &bitmap);
		HPEN oldPen = (HPEN)SelectObject(g_hdcMem, m_Bullet_2[2]);
		SetBkColor(g_hdcMem, RGB(0, 0, 0));

		g_bmpMask = CreateBitmap(bitmap.bmWidth, bitmap.bmHeight, 1, 1, NULL);
		HPEN oldMaskPen = (HPEN)SelectObject(g_hdcMask, g_bmpMask);
		BitBlt(g_hdcMask, 0, 0, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCCOPY);

		BitBlt(g_hdcBuffer, 50+i*12, 30, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCINVERT);
		BitBlt(g_hdcBuffer, 50+i*12, 30, bitmap.bmWidth, bitmap.bmHeight, g_hdcMask, 0, 0, SRCAND);
		BitBlt(g_hdcBuffer, 50+i*12, 30, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCINVERT);

		SelectObject(g_hdcMask, oldMaskPen);
		DeleteObject(g_bmpMask);
		SelectObject(g_hdcMem, oldPen);
	}

	//SCORE
	PaintNumber(m_iScore, 280, 8, 10);
}

void GameManager::AddScore( int num )
{
	m_iScore += num;
}

int GameManager::GetGameStatus()
{
	return m_iStatus;
}

void GameManager::SetGameStatus( int status )
{
	m_iStatus = status;
}

void GameManager::KeyProcess( WPARAM wParam )
{
	switch (m_iStatus)
	{
	case S_MENU:
		switch (wParam)
		{
		case VK_UP:
			PreFocus();
			break;

		case VK_DOWN:
			NextFocus();
			break;

		case VK_RETURN:
			g_Music.lmciMidiStop();
			switch (m_iFocus)
			{
			case 0:	//开始游戏
				m_Process = 0;
				m_bProcessOver = true;

				m_iScore = 0;
				m_ScriptMode = INIT;
				m_iStatus = S_GAME;
				break;

			case 1:	//音乐欣赏
				m_iFocus = 0;
				m_iSelectionMax = 21;
				m_iStatus = S_MUSIC;
				break;

			case 2:	//排行榜
				m_iStatus = S_RANK;
				break;

			case 3:	//退出游戏
				m_iStatus = S_END;
				break;
			}
			break;

		case VK_ESCAPE:
			m_iFocus = m_iSelectionMax - 1;
			break;
		}
		break;

	case S_MUSIC:
		switch (wParam)
		{
		case VK_UP:
			PreFocus();
			break;

		case VK_DOWN:
			NextFocus();
			break;

		case VK_RETURN:
			{
				g_Music.lmciMidiStop();
				char buffer[256] = {0};
				sprintf(buffer, ".\\Music\\%s.mid", m_MusicStrings[m_iFocus].c_str());
				g_Music.lmciMidiPlay(buffer);
			}
			break;

		case VK_ESCAPE:
			GotoMenu();
			break;
		}
		break;

	case S_RANK:
		switch (wParam)
		{
		case VK_ESCAPE:
			GotoMenu();
			break;
		}
		break;
	}
}

void GameManager::ShowMenu()
{
	//LOGFONT tempFont = {0};
	//背景
	ShowBackground();

	//选项
	int textY = 300;
	COLORREF oldColor;

	SetBkMode(g_hdcBuffer, TRANSPARENT);

	for (int i=0; i<m_iSelectionMax; i++)
	{
		if (i == m_iFocus)
		{
			oldColor = SetTextColor(g_hdcBuffer, RGB(255, 135, 139));
			TextOut(g_hdcBuffer, 150, textY, m_MenuStrings[i].c_str(), (int)m_MenuStrings[i].length());
			textY += 30;
			SetTextColor(g_hdcBuffer, oldColor);
		}
		else
		{
			oldColor = SetTextColor(g_hdcBuffer, RGB(14, 84, 141));
			TextOut(g_hdcBuffer, 150, textY, m_MenuStrings[i].c_str(), (int)m_MenuStrings[i].length());
			textY += 30;
			SetTextColor(g_hdcBuffer, oldColor);
		}
	}
}

void GameManager::ShowMusicRoom()
{
	//背景
	ShowBackground();

	//选项
	int textY = 50;
	COLORREF oldColor;

	SetBkMode(g_hdcBuffer, TRANSPARENT);

	for (int i=0; i<m_iSelectionMax; i++)
	{
		if (i == m_iFocus)
		{
			oldColor = SetTextColor(g_hdcBuffer, RGB(255, 0, 0));
			TextOut(g_hdcBuffer, 60, textY, m_MusicStrings[i].c_str(), (int)m_MusicStrings[i].length());
			textY += 20;
			SetTextColor(g_hdcBuffer, oldColor);
		}
		else
		{
			oldColor = SetTextColor(g_hdcBuffer, RGB(255, 255, 255));
			TextOut(g_hdcBuffer, 50, textY, m_MusicStrings[i].c_str(), (int)m_MusicStrings[i].length());
			textY += 20;
			SetTextColor(g_hdcBuffer, oldColor);
		}
	}
}

void GameManager::ShowRank()
{
	ReadRecord();

	//背景
	ShowBackground();

	//选项
	int textY = 100;
	
	SetBkMode(g_hdcBuffer, TRANSPARENT);
	COLORREF oldColor;
	for (int i=0; i<20; i++)
	{
		//char buffer[16] = {0};
		//itoa(m_Record[i].record, buffer, 10);
		//TextOut(g_hdcBuffer, 300, textY, buffer, (int)strlen(buffer));
		oldColor = SetTextColor(g_hdcBuffer, RGB(255, 255, 255));
		TextOut(g_hdcBuffer, 80, textY, m_Record[i].name.c_str(), (int)m_Record[i].name.length());
		SetTextColor(g_hdcBuffer, oldColor);
		PaintNumber(m_Record[i].record, 200, textY, 10);
		textY += 20;
	}
}

void GameManager::PreFocus()
{
	m_iFocus--;
	if (m_iFocus < 0)
	{
		m_iFocus = m_iSelectionMax - 1;
	}
}

void GameManager::NextFocus()
{
	m_iFocus++;
	if (m_iFocus >= m_iSelectionMax)
	{
		m_iFocus = 0;
	}
}

void GameManager::GotoMenu()
{
	g_Music.lmciMidiStop();
	char buffer[256] = {0};
	sprintf(buffer, ".\\Music\\%s.mid", m_MusicStrings[0].c_str());
	g_Music.lmciMidiPlay(buffer);
	m_iFocus = 0;
	m_iSelectionMax = 4;

	SetBGImage(m_BG_sakura, 1);

	m_iStatus = S_MENU;
}

void GameManager::WriteNewRecord()
{
	ofstream os("record.txt", ios::binary);

	for (int i=0; i<20; i++)
	{
		WriteString(m_Record[i].name, os);
		WriteInt(m_Record[i].record, os);
	}

	os.close();
}

void GameManager::ReadRecord()
{
	char buffer[1024];
	ifstream is("record.txt", ios::binary);

	if(!is)
	{
		for (int i=0; i<20; i++)
		{
			m_Record[i].name = "AAA";
			m_Record[i].record = 0;
		}
		WriteNewRecord();
	}
	else
	{
		is.read(buffer, sizeof(buffer));
		is.close();

		string data(buffer);

		for (int i=0; i<20; i++)
		{
			ReadString(m_Record[i].name, data);
			ReadInt(m_Record[i].record, data);
		}
	}
}

void GameManager::WriteInt( int val, ofstream& os )
{
	os<<val<<',';
}

void GameManager::ReadInt( int& val, string& data )
{
	int pos = (int)data.find_first_of(',');

	if (pos != string::npos)
	{
		string result = data.substr(0, pos);
		val = atoi(result.c_str());
		data = data.substr(pos+1);
	}
}

void GameManager::WriteString( string& val, ofstream& os )
{
	os<<val<<',';
}

void GameManager::ReadString( string& val, string& data )
{
	int pos = (int)data.find_first_of(',');

	if (pos != string::npos)
	{
		val = data.substr(0, pos);

		data = data.substr(pos+1);
	}
}

void GameManager::AddNewRecord( char* name )
{
	string tempString(name);
	if (!tempString.length())
	{
		tempString.assign("DEFAULT");
	}

	for (int i=0; i<20; i++)
	{
		if (m_iScore > m_Record[i].record)
		{
			for (int j=19; j>i; j--)
			{
				m_Record[j] = m_Record[j-1];
			}
			m_Record[i].record = m_iScore;
			m_Record[i].name = tempString;
			WriteNewRecord();
			break;
		}
	}
}

void GameManager::LoadGameImage()
{
	//背景图片

	//sakura
	m_BG_sakura[0] = (HBITMAP)LoadImage(NULL, ".\\Image\\back.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	//ame
	for (int i=0; i<12; i++)
	{
		char buffer[64] = {0};
		if (i<10)
		{
			sprintf(buffer, ".\\Image\\ame\\ame0%d.bmp", i);
		}
		else
		{
			sprintf(buffer, ".\\Image\\ame\\ame%d.bmp", i);
		}
		
		m_BG_ame[i] = (HBITMAP)LoadImage(NULL, buffer, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}

	//yuki
	for (int i=0; i<31; i++)
	{
		char buffer[64] = {0};
		if (i<10)
		{
			sprintf(buffer, ".\\Image\\yuki\\yuki00%d.bmp", i);
		}
		else
		{
			sprintf(buffer, ".\\Image\\yuki\\yuki0%d.bmp", i);
		}

		m_BG_yuki[i] = (HBITMAP)LoadImage(NULL, buffer, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}

	m_iNowBGIndex = 0;

	//角色图片
	m_HeroImage = (HBITMAP)LoadImage(NULL, ".\\Image\\hero.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	//敌方角色

	//remilia
	for (int i=0; i<8; i++)
	{
		char buffer[64] = {0};
		sprintf(buffer, ".\\Image\\EnemyPlane\\remilia\\stand00%d.bmp", i);
		m_Remilia[i] = (HBITMAP)LoadImage(NULL, buffer, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}

	//iku
	for (int i=0; i<9; i++)
	{
		char buffer[64] = {0};
		sprintf(buffer, ".\\Image\\EnemyPlane\\iku\\stand00%d.bmp", i);
		m_Iku[i] = (HBITMAP)LoadImage(NULL, buffer, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}

	//komachi
	for (int i=0; i<8; i++)
	{
		char buffer[64] = {0};
		sprintf(buffer, ".\\Image\\EnemyPlane\\komachi\\stand00%d.bmp", i);
		m_Komachi[i] = (HBITMAP)LoadImage(NULL, buffer, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}

	//marisa
	for (int i=0; i<10; i++)
	{
		char buffer[64] = {0};
		sprintf(buffer, ".\\Image\\EnemyPlane\\marisa\\stand00%d.bmp", i);
		m_Marisa[i] = (HBITMAP)LoadImage(NULL, buffer, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}

	//yoyoko
	for (int i=0; i<10; i++)
	{
		char buffer[64] = {0};
		sprintf(buffer, ".\\Image\\EnemyPlane\\yuyuko\\stand00%d.bmp", i);
		m_Yuyuko[i] = (HBITMAP)LoadImage(NULL, buffer, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}

	//子弹图片

	//球形子弹
	for (int i=0; i<8; i++)
	{
		char buffer[64] = {0};
		
		sprintf(buffer, ".\\Image\\Bullet_1\\BulletAb00%d.bmp", i);
		
		m_Bullet_1[i] = (HBITMAP)LoadImage(NULL, buffer, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}

	//星形子弹
	for (int i=0; i<9; i++)
	{
		char buffer[64] = {0};

		sprintf(buffer, ".\\Image\\Bullet_2\\bulletDa00%d.bmp", i);

		m_Bullet_2[i] = (HBITMAP)LoadImage(NULL, buffer, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}

	//有方向子弹
	for (int i=0; i<7; i++)
	{
		char buffer[64] = {0};

		sprintf(buffer, ".\\Image\\Bullet_3\\bulletAc00%d.bmp", i);

		m_Bullet_3[i] = (HBITMAP)LoadImage(NULL, buffer, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}

	//爆炸
	for (int i=0; i<8; i++)
	{
		char buffer[64] = {0};

		sprintf(buffer, ".\\Image\\crash\\crashAa00%d.bmp", i);

		m_Crash[i] = (HBITMAP)LoadImage(NULL, buffer, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}

	//数字
	m_Number = (HBITMAP)LoadImage(NULL, ".\\Image\\number.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	//BOOM
	for (int i=0; i<5; i++)
	{
		char buffer[64] = {0};

		sprintf(buffer, ".\\Image\\Boom_1\\objectDa00%d.bmp", i);

		m_Boom_1[i] = (HBITMAP)LoadImage(NULL, buffer, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}

	//无敌特效
	for (int i=0; i<30; i++)
	{
		char buffer[64] = {0};
		if (i<10)
		{
			sprintf(buffer, ".\\Image\\effect\\spellBulletCa00%d.bmp", i);
		}
		else
		{
			sprintf(buffer, ".\\Image\\effect\\spellBulletCa0%d.bmp", i);
		}

		m_InvincibleEffect[i] = (HBITMAP)LoadImage(NULL, buffer, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}

	//LevelUp
	for (int i=0; i<4; i++)
	{
		char buffer[64] = {0};
		sprintf(buffer, ".\\Image\\LevelUp\\LEVEL00%d.bmp", i);
		m_LevelUpEffect[i] = (HBITMAP)LoadImage(NULL, buffer, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}

	//物品
	m_ItemImage[0] = (HBITMAP)LoadImage(NULL, ".\\Image\\Items\\p_small.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	m_ItemImage[1] = (HBITMAP)LoadImage(NULL, ".\\Image\\Items\\p_big.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	m_ItemImage[2] = (HBITMAP)LoadImage(NULL, ".\\Image\\Items\\boom.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	m_ItemImage[3] = (HBITMAP)LoadImage(NULL, ".\\Image\\Items\\lv_1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	m_ItemImage[4] = (HBITMAP)LoadImage(NULL, ".\\Image\\Items\\point.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	m_ItemImage[5] = (HBITMAP)LoadImage(NULL, ".\\Image\\Items\\ever.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	//PlaneGuard
	for (int i=0; i<5; i++)
	{
		for (int j=0; j<10; j++)
		{
			char buffer[64] = {0};
			sprintf(buffer, ".\\Image\\PlaneGuard\\spellBulletF%c00%d.bmp", 'a'+i, j);
			m_GuardImage[i][j] = (HBITMAP)LoadImage(NULL, buffer, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		}
	}
}

void GameManager::SetBGImage( HBITMAP* bgImage, int n )
{
	m_BGImage = bgImage;
	m_iNowBGIndex = 0;
	m_iGBIndexMax = n;
}

void GameManager::PaintNumber( int num, int x, int y, int col )
{
	//转化字符串
	char buffer[32] = {0};
	char result[32] = {0};
	itoa(num, buffer, 10);

	int nowX = x;


	//补零
	if (col > (int)strlen(buffer))
	{
		for (int i=0; i<col-(int)strlen(buffer); i++)
		{
			result[i] = '0';
		}
	}

	strcat(result, buffer);

	//描画数字
	for (int i=0; i<(int)strlen(result); i++)
	{
		BITMAP bitmap;
		GetObject(m_Number, sizeof(bitmap), &bitmap);
		HPEN oldPen = (HPEN)SelectObject(g_hdcMem, m_Number);
		SetBkColor(g_hdcMem, RGB(0, 255, 0));

		g_bmpMask = CreateBitmap(bitmap.bmWidth, bitmap.bmHeight, 1, 1, NULL);
		HPEN oldMaskPen = (HPEN)SelectObject(g_hdcMask, g_bmpMask);
		BitBlt(g_hdcMask, 0, 0, bitmap.bmWidth, bitmap.bmHeight, g_hdcMem, 0, 0, SRCCOPY);

		BitBlt(g_hdcBuffer, nowX, y, 11, bitmap.bmHeight, g_hdcMem, 11*(result[i]-'0'), 0, SRCINVERT);
		BitBlt(g_hdcBuffer, nowX, y, 11, bitmap.bmHeight, g_hdcMask, 11*(result[i]-'0'), 0, SRCAND);
		BitBlt(g_hdcBuffer, nowX, y, 11, bitmap.bmHeight, g_hdcMem, 11*(result[i]-'0'), 0, SRCINVERT);

		SelectObject(g_hdcMask, oldMaskPen);
		DeleteObject(g_bmpMask);
		SelectObject(g_hdcMem, oldPen);

		nowX += 11;
	}
}

HBITMAP GameManager::GetItemImage( int itemType )
{
	return m_ItemImage[itemType];
}