#include "stdafx.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "packet.h"
#include "guild.h"
#include "vector.h"
#include "questmanager.h"
#include "item.h"
#include "horsename_manager.h"
#include "locale_service.h"
#include "../../common/VnumHelper.h"
#include "mob_manager.h"

bool CHARACTER::StartRiding()
{
	if (IsDead() == true)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot ride while falling."));
		return false;
	}
	
	if (IsPolymorphed())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot ride a Horse as long as you are transformed."));
		return false;
	}

	LPITEM armor = GetWear(WEAR_BODY);

	if (armor && (armor->GetVnum() >= 11901 && armor->GetVnum() <= 11904))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot ride while wearing a Wedding Robe or a Smoking."));
		return false;
	}

	DWORD dwMountVnum = m_chHorse ? m_chHorse->GetRaceNum() : GetMyHorseVnum();

	if (false == CHorseRider::StartRiding())
	{
		if (GetHorseLevel() <= 0)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You do not have a Horse."));
		else if (GetHorseHealth() <= 0)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your Horse is dead."));
		else if (GetHorseStamina() <= 0)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The endurance of your Horse is too low."));

		return false;
	}

	HorseSummon(false);

	MountVnum(dwMountVnum);

	if(test_server)
		sys_log(0, "Ride Horse : %s ", GetName());

	return true;
}

bool CHARACTER::StopRiding()
{
	if (CHorseRider::StopRiding())
	{
		quest::CQuestManager::instance().Unmount(GetPlayerID());

		if (!IsDead() && !IsStun())
		{
			DWORD dwOldVnum = GetMountVnum();
			MountVnum(0);
			HorseSummon(true, false, dwOldVnum);
		}
		else
		{
			m_dwMountVnum = 0;
			ComputePoints();
			UpdatePacket();
		}

		PointChange(POINT_ST, 0);
		PointChange(POINT_DX, 0);
		PointChange(POINT_HT, 0);
		PointChange(POINT_IQ, 0);

		return true;
	}

	return false;
}

EVENTFUNC(horse_dead_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "horse_dead_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = info->ch;
	if (ch == NULL) {
		return 0;
	}
	ch->HorseSummon(false);
	return 0;
}

void CHARACTER::SetRider(LPCHARACTER ch)
{
	if (m_chRider)
		m_chRider->ClearHorseInfo();

	m_chRider = ch;

	if (m_chRider)
		m_chRider->SendHorseInfo();
}

LPCHARACTER CHARACTER::GetRider() const
{
	return m_chRider;
}


void CHARACTER::HorseSummon(bool bSummon, bool bFromFar, DWORD dwVnum, const char* pPetName)
{
	if ( bSummon )
	{
		if( m_chHorse != NULL )
			return;

		if (GetHorseLevel() <= 0)
			return;

		if (IsRiding())
			return;

		sys_log(0, "HorseSummon : %s lv:%d bSummon:%d fromFar:%d", GetName(), GetLevel(), bSummon, bFromFar);

		long x = GetX();
		long y = GetY();

		if (GetHorseHealth() <= 0)
			bFromFar = false;

		if (bFromFar)
		{
			x += (number(0, 1) * 2 - 1) * number(2000, 2500);
			y += (number(0, 1) * 2 - 1) * number(2000, 2500);
		}
		else
		{
			x += number(-100, 100);
			y += number(-100, 100);
		}

		m_chHorse = CHARACTER_MANAGER::instance().SpawnMob(
				(0 == dwVnum) ? GetMyHorseVnum() : dwVnum, 
				GetMapIndex(), 
				x, y,
				GetZ(), false, (int)(GetRotation()+180), false);

		if (!m_chHorse)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Calling the Horse failed."));
			return;
		}

		if (GetHorseHealth() <= 0)
		{
			m_chHorse->SetPosition(POS_DEAD);
			char_event_info* info = AllocEventInfo<char_event_info>();
			info->ch = this;
			m_chHorse->m_pkDeadEvent = event_create(horse_dead_event, info, PASSES_PER_SEC(60));
		}

		m_chHorse->SetLevel(GetHorseLevel());

		const char* pHorseName = CHorseNameManager::instance().GetHorseName(GetPlayerID());

		if (pHorseName != NULL && strlen(pHorseName) != 0)
		{
			m_chHorse->m_stName = pHorseName;
		}
		else
		{
			LPITEM pkMountCostume = GetWear(WEAR_COSTUME_MOUNT);
			if (pkMountCostume)
			{
				const CMob* pMob = CMobManager::instance().Get(pkMountCostume->GetValue(0));
				if (!pMob)
				{
					sys_err("Cannot find mount_vnum %d for mount_costume %d", pkMountCostume->GetValue(0), pkMountCostume->GetVnum());
				}
				else
				{
					m_chHorse->m_stName = GetName();
				}
			}
		}

		if (strlen(m_chHorse->m_stName.c_str()) == 0)
		{
			m_chHorse->m_stName = GetName();
		}

		if (!m_chHorse->Show(GetMapIndex(), x, y, GetZ()))
		{
			M2_DESTROY_CHARACTER(m_chHorse);
			sys_err("cannot show monster");
			m_chHorse = NULL;
			return;
		}

		if ((GetHorseHealth() <= 0))
		{
			TPacketGCDead pack;
			pack.header	= HEADER_GC_DEAD;
			pack.vid    = m_chHorse->GetVID();
			PacketAround(&pack, sizeof(pack));
		}

		m_chHorse->SetRider(this);
	}
	else
	{
		if (!m_chHorse)
			return;

		LPCHARACTER chHorse = m_chHorse;

		chHorse->SetRider(NULL);

		if (!bFromFar)
		{
			M2_DESTROY_CHARACTER(chHorse);
		}
		else
		{
			chHorse->SetNowWalking(false);
			float fx, fy;
			chHorse->SetRotation(GetDegreeFromPositionXY(chHorse->GetX(), chHorse->GetY(), GetX(), GetY())+180);
			GetDeltaByDegree(chHorse->GetRotation(), 3500, &fx, &fy);
			chHorse->Goto((long)(chHorse->GetX()+fx), (long) (chHorse->GetY()+fy));
			chHorse->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
		}

		m_chHorse = NULL;
	}
}

DWORD CHARACTER::GetMyHorseVnum() const
{
	int delta = 0;

	if (GetGuild())
	{
		++delta;

		if (GetGuild()->GetMasterPID() == GetPlayerID())
			++delta;
	}

	LPITEM pkCostumeMount = GetWear(WEAR_COSTUME_MOUNT);
	if (pkCostumeMount)
	{
		return pkCostumeMount->GetValue(0);
	}

	return c_aHorseStat[GetHorseLevel()].iNPCRace + delta;
}

void CHARACTER::HorseDie()
{
	CHorseRider::HorseDie();
	HorseSummon(false);
}

bool CHARACTER::ReviveHorse()
{
	if (CHorseRider::ReviveHorse())
	{
		HorseSummon(false);
		HorseSummon(true);
		return true;
	}
	return false;
}

void CHARACTER::ClearHorseInfo()
{
	if (!IsHorseRiding())
	{
		ChatPacket(CHAT_TYPE_COMMAND, "hide_horse_state");

		m_bSendHorseLevel = 0;
		m_bSendHorseHealthGrade = 0;
		m_bSendHorseStaminaGrade = 0;
	}

	m_chHorse = NULL;

}

void CHARACTER::SendHorseInfo()
{
	if (m_chHorse || IsHorseRiding())
	{
		int iHealthGrade;
		int iStaminaGrade;

		if (GetHorseHealth() == 0)
			iHealthGrade = 0;
		else if (GetHorseHealth() * 10 <= GetHorseMaxHealth() * 3) 
			iHealthGrade = 1;
		else if (GetHorseHealth() * 10 <= GetHorseMaxHealth() * 7) 
			iHealthGrade = 2;
		else
			iHealthGrade = 3;

		if (GetHorseStamina() * 10 <= GetHorseMaxStamina())
			iStaminaGrade = 0;
		else if (GetHorseStamina() * 10 <= GetHorseMaxStamina() * 3) 
			iStaminaGrade = 1;
		else if (GetHorseStamina() * 10 <= GetHorseMaxStamina() * 7) 
			iStaminaGrade = 2;
		else
			iStaminaGrade = 3;

		if (m_bSendHorseLevel != GetHorseLevel() || 
				m_bSendHorseHealthGrade != iHealthGrade || 
				m_bSendHorseStaminaGrade != iStaminaGrade)
		{
			ChatPacket(CHAT_TYPE_COMMAND, "horse_state %d %d %d", GetHorseLevel(), iHealthGrade, iStaminaGrade);

			m_bSendHorseLevel = GetHorseLevel();
			m_bSendHorseHealthGrade = iHealthGrade;
			m_bSendHorseStaminaGrade = iStaminaGrade;
		}
	}
}
		
bool CHARACTER::CanUseHorseSkill()
{
	if(IsRiding())
	{
		if (GetHorseGrade() == 3)
			return true;
		else
			return false;

		if(GetMountVnum())
		{
			if (GetMountVnum() >= 20209 && GetMountVnum() <= 20212)
				return true;
		}
		else
			return false;

	}

	return false;
}

void CHARACTER::SetHorseLevel(int iLevel)
{
	CHorseRider::SetHorseLevel(iLevel);
	SetSkillLevel(SKILL_HORSE, GetHorseLevel());
}

