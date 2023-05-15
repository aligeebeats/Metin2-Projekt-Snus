#include "stdafx.h"
#ifndef _WIN32
#include <md5.h>
#else
#include "../../libthecore/src/xmd5.h"
#endif
#include "utils.h"
#include "config.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "motion.h"
#include "packet.h"
#include "affect.h"
#include "pvp.h"
#include "start_position.h"
#include "party.h"
#include "guild_manager.h"
#include "p2p.h"
#include "dungeon.h"
#include "messenger_manager.h"
#include "war_map.h"
#include "questmanager.h"
#include "item_manager.h"
#include "mob_manager.h"
#include "item.h"
#include "buffer_manager.h"
#include "unique_item.h"
#include "log.h"
#include "../../common/VnumHelper.h"

extern int g_server_id;

extern int g_nPortalLimitTime;

ACMD(do_user_horse_ride)
{
	if (ch->IsObserverMode())
		return;

	if (ch->IsDead() || ch->IsStun())
		return;

	if (ch->IsHorseRiding() == false)
	{
		if (ch->GetMountVnum())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are already riding a mount."));
			return;
		}

		if (ch->GetHorse() == NULL)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please call your Horse first."));
			return;
		}

		ch->StartRiding();
	}
	else
	{
		ch->StopRiding();
	}
}

ACMD(do_user_horse_back)
{
	if (ch->GetHorse() != NULL)
	{
		ch->HorseSummon(false);
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You sent back your Horse."));
	}
	else if (ch->IsHorseRiding() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to get off your Horse."));
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please call your Horse first."));
	}
}

ACMD(do_user_horse_feed)
{
	if (ch->GetMyShop())
		return;

	if (ch->GetHorse() == NULL)
	{
		if (ch->IsHorseRiding() == false)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please call your Horse first."));
		else
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot feed your Horse while sitting on it."));
		return;
	}

	DWORD dwFood = ch->GetHorseGrade() + 50054 - 1;

	if (ch->CountSpecifyItem(dwFood) > 0)
	{
		ch->RemoveSpecifyItem(dwFood, 1);
		ch->FeedHorse();
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You gave %s%s to a Horse."), ITEM_MANAGER::instance().GetTable(dwFood)->szLocaleName, "");
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need %s."), ITEM_MANAGER::instance().GetTable(dwFood)->szLocaleName);
	}
}

#define MAX_REASON_LEN		128

EVENTINFO(TimedEventInfo)
{
	DynamicCharacterPtr ch;
	int		subcmd;
	int         	left_second;
	char		szReason[MAX_REASON_LEN];

	TimedEventInfo() 
	: ch()
	, subcmd( 0 )
	, left_second( 0 )
	{
		::memset( szReason, 0, MAX_REASON_LEN );
	}
};

struct SendDisconnectFunc
{
	void operator()(LPDESC d)
	{
		if(d->GetCharacter())
		{
			d->GetCharacter()->SaveReal();
			DWORD pid = d->GetCharacter()->GetPlayerID();
			db_clientdesc->DBPacketHeader(HEADER_GD_FLUSH_CACHE, 0, sizeof(DWORD));
			db_clientdesc->Packet(&pid, sizeof(DWORD));

			if(d->GetCharacter()->GetGMLevel() == GM_PLAYER)
			{
				d->GetCharacter()->ChatPacket(CHAT_TYPE_COMMAND, "quit Shutdown(SendDisconnectFunc)");
			}
		}
	}
};

struct DisconnectFunc
{
	void operator () (LPDESC d)
	{
		if (d->GetType() == DESC_TYPE_CONNECTOR)
			return;

		if (d->IsPhase(PHASE_P2P))
			return;

		if (d->GetCharacter())
			d->GetCharacter()->Disconnect("Shutdown(DisconnectFunc)");

		d->SetPhase(PHASE_CLOSE);
	}
};

EVENTINFO(shutdown_event_data)
{
	int seconds;

	shutdown_event_data()
	: seconds( 0 )
	{
	}
};

EVENTFUNC(shutdown_event)
{
	shutdown_event_data* info = dynamic_cast<shutdown_event_data*>( event->info );

	if ( info == NULL )
	{
		sys_err( "shutdown_event> <Factor> Null pointer" );
		return 0;
	}

	int *pSec = & (info->seconds);

	if (*pSec < 0)
	{
		sys_log(0, "shutdown_event sec %d", *pSec);

		if (--*pSec == -10)
		{
			const DESC_MANAGER::DESC_SET & c_set_desc = DESC_MANAGER::instance().GetClientSet();
			std::for_each(c_set_desc.begin(), c_set_desc.end(), DisconnectFunc());
			return passes_per_sec;
		}
		else if (*pSec < -10)
		{
			return 0;
		}

		return passes_per_sec;
	}
	else if (*pSec == 0)
	{
		const DESC_MANAGER::DESC_SET& c_set_desc = DESC_MANAGER::instance().GetClientSet();
		std::for_each(c_set_desc.begin(), c_set_desc.end(), SendDisconnectFunc());
		g_bNoMoreClient = true;
		--*pSec;
		return passes_per_sec;
	}
	else
	{
		char buf[64];
		snprintf(buf, sizeof(buf), LC_TEXT("%d seconds until shutting off."), *pSec);
		SendNotice(buf);

		--*pSec;
		return passes_per_sec;
	}
}

void Shutdown(int iSec)
{
	if (g_bNoMoreClient)
	{
		thecore_shutdown();
		return;
	}

	CWarMapManager::instance().OnShutdown();

	char buf[64];
	snprintf(buf, sizeof(buf), LC_TEXT("The Game will be closed after %d Seconds."), iSec);
	SendNotice(buf);

	shutdown_event_data* info = AllocEventInfo<shutdown_event_data>();
	info->seconds = iSec;

	event_create(shutdown_event, info, 1);
}

ACMD(do_shutdown)
{
	if (NULL == ch)
	{
		return;
	}
	TPacketGGShutdown p;
	p.bHeader = HEADER_GG_SHUTDOWN;
	P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGShutdown));

#ifndef _IMPROVED_PACKET_ENCRYPTION_
	TPacketShutdown client_packet;
	client_packet.state = true;
	db_clientdesc->DBPacket(HEADER_GD_SHUTDOWN, 0, &client_packet, sizeof(client_packet));
#endif

	Shutdown(10);
}

EVENTFUNC(timed_event)
{
	TimedEventInfo * info = dynamic_cast<TimedEventInfo *>( event->info );
	
	if ( info == NULL )
	{
		sys_err( "timed_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;
	if (ch == NULL) {
		return 0;
	}
	LPDESC d = ch->GetDesc();

	if (info->left_second <= 0)
	{
		ch->m_pkTimedEvent = NULL;

		switch (info->subcmd)
		{
			case SCMD_LOGOUT:
			case SCMD_QUIT:
			case SCMD_PHASE_SELECT:
				{
					TPacketNeedLoginLogInfo acc_info;
					acc_info.dwPlayerID = ch->GetDesc()->GetAccountTable().id;
					db_clientdesc->DBPacket( HEADER_GD_VALID_LOGOUT, 0, &acc_info, sizeof(acc_info) );
				}
				break;
		}

		switch (info->subcmd)
		{
			case SCMD_LOGOUT:
				if (d)
					d->SetPhase(PHASE_CLOSE);
				break;

			case SCMD_QUIT:
				ch->ChatPacket(CHAT_TYPE_COMMAND, "quit");
				break;

			case SCMD_PHASE_SELECT:
				{
					ch->Disconnect("timed_event - SCMD_PHASE_SELECT");

					if (d)
					{
						d->SetPhase(PHASE_SELECT);
					}
				}
				break;
		}

		return 0;
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d seconds until shutting off."), info->left_second);
		--info->left_second;
	}

	return PASSES_PER_SEC(1);
}

ACMD(do_cmd)
{
	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your Logout was cancelled."));
		event_cancel(&ch->m_pkTimedEvent);
		return;
	}

	switch (subcmd)
	{
		case SCMD_LOGOUT:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Back to Login Window. Please wait."));
			break;

		case SCMD_QUIT:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You will be disconnected from the server. Please wait."));
			break;

		case SCMD_PHASE_SELECT:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Changing character. Please wait."));
			break;
	}

	int nExitLimitTime = 10;

	if (ch->IsHack(false, true, nExitLimitTime) && (!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
	{
		return;
	}
	
	switch (subcmd)
	{
		case SCMD_LOGOUT:
		case SCMD_QUIT:
		case SCMD_PHASE_SELECT:
			{
				TimedEventInfo* info = AllocEventInfo<TimedEventInfo>();

				{
					if (ch->IsPosition(POS_FIGHTING))
						info->left_second = 10;
					else
						info->left_second = 3;
				}

				info->ch		= ch;
				info->subcmd		= subcmd;
				strlcpy(info->szReason, argument, sizeof(info->szReason));

				ch->m_pkTimedEvent	= event_create(timed_event, info, 1);
			}
			break;
	}
}

ACMD(do_mount)
{
}

ACMD(do_fishing)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	ch->SetRotation(atof(arg1));
	ch->fishing();
}

ACMD(do_console)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ConsoleEnable");
}

ACMD(do_restart)
{
	if (false == ch->IsDead())
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");
		ch->StartRecoveryEvent();
		return;
	}

	if (NULL == ch->m_pkDeadEvent)
		return;

	int iTimeToDead = (event_time(ch->m_pkDeadEvent) / 0);

	if (subcmd != SCMD_RESTART_TOWN && (!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
	{
		if (!test_server)
		{
			if (ch->IsHack())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No new start possible.(%d seconds left) "), iTimeToDead - (0 - g_nPortalLimitTime));
				return;
			}

			if (iTimeToDead > 170)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No new start possible.(%d seconds left) "), iTimeToDead - 0);
				return;
			}
		}
	}

	if (subcmd == SCMD_RESTART_TOWN)
	{
		if (ch->IsHack())
		{
			if ((!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No new start possible.(%d seconds left) "), iTimeToDead - (0 - g_nPortalLimitTime));
				return;
			}
		}

		if (iTimeToDead > 173)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Still no new start possible in the city. (%d seconds left)"), iTimeToDead - 0);
			return;
		}
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");

	ch->GetDesc()->SetPhase(PHASE_GAME);
	ch->SetPosition(POS_STANDING);
	ch->StartRecoveryEvent();

	if (ch->GetDungeon())
		ch->GetDungeon()->UseRevive(ch);

	if (ch->GetWarMap() && !ch->IsObserverMode())
	{
		CWarMap * pMap = ch->GetWarMap();
		DWORD dwGuildOpponent = pMap ? pMap->GetGuildOpponent(ch) : 0;

		if (dwGuildOpponent)
		{
			switch (subcmd)
			{
				case SCMD_RESTART_TOWN:
					sys_log(0, "do_restart: restart town");
					PIXEL_POSITION pos;

					if (CWarMapManager::instance().GetStartPosition(ch->GetMapIndex(), ch->GetGuild()->GetID() < dwGuildOpponent ? 0 : 1, pos))
						ch->Show(ch->GetMapIndex(), pos.x, pos.y);
					else
						ch->ExitToSavedLocation();

					ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
					ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
					ch->ReviveInvisible(5);
					break;

				case SCMD_RESTART_HERE:
					sys_log(0, "do_restart: restart here");
					ch->RestartAtSamePos();
					ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
					ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
					ch->ReviveInvisible(5);
					break;
			}

			return;
		}
	}

	switch (subcmd)
	{
		case SCMD_RESTART_TOWN:
			sys_log(0, "do_restart: restart town");
			PIXEL_POSITION pos;

			if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(ch->GetMapIndex(), ch->GetEmpire(), pos))
				ch->WarpSet(pos.x, pos.y);
			else
				ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));

			ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
			ch->DeathPenalty(1);
			break;

		case SCMD_RESTART_HERE:
			sys_log(0, "do_restart: restart here");
			ch->RestartAtSamePos();
			ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
			ch->DeathPenalty(0);
			ch->ReviveInvisible(5);
			break;
	}

	if (ch->GetWear(WEAR_COSTUME_MOUNT))
	{
		DWORD dwOldVnum = ch->GetMountVnum();
		ch->HorseSummon(true, false, dwOldVnum);
	}
}

#define MAX_STAT 90

ACMD(do_stat_reset)
{
	ch->PointChange(POINT_STAT_RESET_COUNT, 12 - ch->GetPoint(POINT_STAT_RESET_COUNT));
}

ACMD(do_stat_minus)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change your state as long as you are transformed."));
		return;
	}

	if (ch->GetPoint(POINT_STAT_RESET_COUNT) <= 0)
		return;

	if (!strcmp(arg1, "st"))
	{
		if (ch->GetRealPoint(POINT_ST) <= JobInitialPoints[ch->GetJob()].st)
			return;

		ch->SetRealPoint(POINT_ST, ch->GetRealPoint(POINT_ST) - 1);
		ch->SetPoint(POINT_ST, ch->GetPoint(POINT_ST) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_ST, 0);
	}
	else if (!strcmp(arg1, "dx"))
	{
		if (ch->GetRealPoint(POINT_DX) <= JobInitialPoints[ch->GetJob()].dx)
			return;

		ch->SetRealPoint(POINT_DX, ch->GetRealPoint(POINT_DX) - 1);
		ch->SetPoint(POINT_DX, ch->GetPoint(POINT_DX) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_DX, 0);
	}
	else if (!strcmp(arg1, "ht"))
	{
		if (ch->GetRealPoint(POINT_HT) <= JobInitialPoints[ch->GetJob()].ht)
			return;

		ch->SetRealPoint(POINT_HT, ch->GetRealPoint(POINT_HT) - 1);
		ch->SetPoint(POINT_HT, ch->GetPoint(POINT_HT) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_HT, 0);
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (!strcmp(arg1, "iq"))
	{
		if (ch->GetRealPoint(POINT_IQ) <= JobInitialPoints[ch->GetJob()].iq)
			return;

		ch->SetRealPoint(POINT_IQ, ch->GetRealPoint(POINT_IQ) - 1);
		ch->SetPoint(POINT_IQ, ch->GetPoint(POINT_IQ) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_IQ, 0);
		ch->PointChange(POINT_MAX_SP, 0);
	}
	else
		return;

	ch->PointChange(POINT_STAT, +1);
	ch->PointChange(POINT_STAT_RESET_COUNT, -1);
	ch->ComputePoints();
}

ACMD(do_stat)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change your state as long as you are transformed."));
		return;
	}

	if (ch->GetPoint(POINT_STAT) <= 0)
		return;

	BYTE idx = 0;
	
	if (!strcmp(arg1, "st"))
		idx = POINT_ST;
	else if (!strcmp(arg1, "dx"))
		idx = POINT_DX;
	else if (!strcmp(arg1, "ht"))
		idx = POINT_HT;
	else if (!strcmp(arg1, "iq"))
		idx = POINT_IQ;
	else
		return;

	if (ch->GetRealPoint(idx) >= MAX_STAT)
		return;

	ch->SetRealPoint(idx, ch->GetRealPoint(idx) + 1);
	ch->SetPoint(idx, ch->GetPoint(idx) + 1);
	ch->ComputePoints();
	ch->PointChange(idx, 0);

	if (idx == POINT_IQ)
	{
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (idx == POINT_HT)
	{
		ch->PointChange(POINT_MAX_SP, 0);
	}

	ch->PointChange(POINT_STAT, -1);
	ch->ComputePoints();
}

ACMD(do_pvp)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(vid);

	if (!pkVictim)
		return;

	if (pkVictim->IsNPC())
		return;

	CPVPManager::instance().Insert(ch, pkVictim);
}

ACMD(do_guildskillup)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (!ch->GetGuild())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> It does not belong to the Guild."));
		return;
	}

	CGuild* g = ch->GetGuild();
	TGuildMember* gm = g->GetMember(ch->GetPlayerID());
	if (gm->grade == GUILD_LEADER_GRADE)
	{
		DWORD vnum = 0;
		str_to_number(vnum, arg1);
		g->SkillLevelUp(vnum);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> You cannot change the Level of the Guild Skills."));
	}
}

ACMD(do_skillup)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vnum = 0;
	str_to_number(vnum, arg1);

	if (true == ch->CanUseSkill(vnum))
	{
		ch->SkillLevelUp(vnum);
	}
	else
	{
		switch(vnum)
		{
			case SKILL_HORSE_WILDATTACK:
			case SKILL_HORSE_CHARGE:
			case SKILL_HORSE_ESCAPE:
			case SKILL_HORSE_WILDATTACK_RANGE:

			case SKILL_7_A_ANTI_TANHWAN:
			case SKILL_7_B_ANTI_AMSEOP:
			case SKILL_7_C_ANTI_SWAERYUNG:
			case SKILL_7_D_ANTI_YONGBI:

			case SKILL_8_A_ANTI_GIGONGCHAM:
			case SKILL_8_B_ANTI_YEONSA:
			case SKILL_8_C_ANTI_MAHWAN:
			case SKILL_8_D_ANTI_BYEURAK:

			case SKILL_ADD_HP:
			case SKILL_RESIST_PENETRATE:
				ch->SkillLevelUp(vnum);
				break;
		}
	}
}

ACMD(do_safebox_close)
{
	ch->CloseSafebox();
}

ACMD(do_safebox_password)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	ch->ReqSafeboxLoad(arg1);
}

ACMD(do_safebox_change_password)
{
	char arg1[256];
	char arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || strlen(arg1)>6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Storages> You entered a wrong password."));
		return;
	}

	if (!*arg2 || strlen(arg2)>6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Storages> You entered a wrong password."));
		return;
	}

	TSafeboxChangePasswordPacket p;

	p.dwID = ch->GetDesc()->GetAccountTable().id;
	strlcpy(p.szOldPassword, arg1, sizeof(p.szOldPassword));
	strlcpy(p.szNewPassword, arg2, sizeof(p.szNewPassword));

	db_clientdesc->DBPacket(HEADER_GD_SAFEBOX_CHANGE_PASSWORD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}

ACMD(do_mall_password)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1 || strlen(arg1) > 6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Storages> You entered a wrong password."));
		return;
	}

	int iPulse = thecore_pulse();

	if (ch->GetMall())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Storages> The storage is already open."));
		return;
	}

	if (iPulse - ch->GetMallLoadTime() < passes_per_sec * 10)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Storages> You have to wait 10 seconds to open the storage again."));
		return;
	}

	ch->SetMallLoadTime(iPulse);

	TSafeboxLoadPacket p;
	p.dwID = ch->GetDesc()->GetAccountTable().id;
	strlcpy(p.szLogin, ch->GetDesc()->GetAccountTable().login, sizeof(p.szLogin));
	strlcpy(p.szPassword, arg1, sizeof(p.szPassword));

	db_clientdesc->DBPacket(HEADER_GD_MALL_LOAD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}

ACMD(do_mall_close)
{
	if (ch->GetMall())
	{
		ch->SetMallLoadTime(thecore_pulse());
		ch->CloseMall();
		ch->Save();
	}
}

ACMD(do_ungroup)
{
	if (!ch->GetParty())
		return;

	if (!CPartyManager::instance().IsEnablePCParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> The server cannot execute the Group request."));
		return;
	}

	if (ch->GetDungeon())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot leave a Group while you are in a dungeon."));
		return;
	}

	LPPARTY pParty = ch->GetParty();

	if (pParty->GetMemberCount() == 2)
	{
		CPartyManager::instance().DeleteParty(pParty);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You left the Group."));
		pParty->Quit(ch->GetPlayerID());
	}
}

ACMD(do_close_shop)
{
	if (ch->GetMyShop())
	{
		ch->CloseMyShop();
		return;
	}
}

ACMD(do_set_walk_mode)
{
	ch->SetNowWalking(true);
	ch->SetWalking(true);
}

ACMD(do_set_run_mode)
{
	ch->SetNowWalking(false);
	ch->SetWalking(false);
}

ACMD(do_war)
{
	CGuild * g = ch->GetGuild();

	if (!g)
		return;

	if (g->UnderAnyWar())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> Your Guild is already participating in another war."));
		return;
	}

	char arg1[256], arg2[256];
	int type = GUILD_WAR_TYPE_FIELD;
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
		return;

	if (*arg2)
	{
	str_to_number(type, arg2);
 
	if (type >= GUILD_WAR_TYPE_MAX_NUM)
		type = GUILD_WAR_TYPE_FIELD;
		 
	if(type < 0)
		return;
	}

	DWORD gm_pid = g->GetMasterPID();

	if (gm_pid != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> You are not entitled to declare a Guild war."));
		return;
	}

	CGuild * opp_g = CGuildManager::instance().FindGuildByName(arg1);

	if (!opp_g)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> This Guild does not exist."));
		return;
	}

	switch (g->GetGuildWarState(opp_g->GetID()))
	{
		case GUILD_WAR_NONE:
			{
				if (opp_g->UnderAnyWar())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> This Guild is already participating in a war."));
					return;
				}

				long long lldWarPrice = KOR_aGuildWarInfo[type].lldWarPrice;

				if (g->GetGuildMoney() < lldWarPrice)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> There is not enough Yang to participate in a Guild war."));
					return;
				}

				if (opp_g->GetGuildMoney() < lldWarPrice)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> The Guild does not have enough Yang to participate in a Guild war."));
					return;
				}
			}
			break;

		case GUILD_WAR_SEND_DECLARE:
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You already declared war on this Guild."));
				return;
			}
			break;

		case GUILD_WAR_RECV_DECLARE:
			{
				if (opp_g->UnderAnyWar())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> This Guild is already participating in a war."));
					g->RequestRefuseWar(opp_g->GetID());
					return;
				}
			}
			break;

		case GUILD_WAR_RESERVE:
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> This Guild is already noted for another Guild war."));
				return;
			}
			break;

		case GUILD_WAR_END:
			return;

		default:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> This Guild is already participating in another war."));
			g->RequestRefuseWar(opp_g->GetID());
			return;
	}

	if (!g->CanStartWar(type))
	{
		if (g->GetLadderPoint() == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> Rank is too low to participate in a Guild war."));
			sys_log(0, "GuildWar.StartError.NEED_LADDER_POINT");
		}
		else if (g->GetMemberCount() < GUILD_WAR_MIN_MEMBER_COUNT)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> At least %d players have to participate in a Guild war."), GUILD_WAR_MIN_MEMBER_COUNT);
			sys_log(0, "GuildWar.StartError.NEED_MINIMUM_MEMBER[%d]", GUILD_WAR_MIN_MEMBER_COUNT);
		}
		else
		{
			sys_log(0, "GuildWar.StartError.UNKNOWN_ERROR");
		}
		return;
	}

	if (!opp_g->CanStartWar(GUILD_WAR_TYPE_FIELD))
	{
		if (opp_g->GetLadderPoint() == 0)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> The Guild does not have enough Rank Points to participate in a Guild war."));
		else if (opp_g->GetMemberCount() < GUILD_WAR_MIN_MEMBER_COUNT)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> The Guild does not have enough members to participate in a Guild war."));
		return;
	}

	do
	{
		if (g->GetMasterCharacter() != NULL)
			break;

		CCI *pCCI = P2P_MANAGER::instance().FindByPID(g->GetMasterPID());

		if (pCCI != NULL)
			break;

		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> The Guild master is not online."));
		g->RequestRefuseWar(opp_g->GetID());
		return;

	} while (false);

	do
	{
		if (opp_g->GetMasterCharacter() != NULL)
			break;
		
		CCI *pCCI = P2P_MANAGER::instance().FindByPID(opp_g->GetMasterPID());
		
		if (pCCI != NULL)
			break;

		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> The Guild master is not online."));
		g->RequestRefuseWar(opp_g->GetID());
		return;

	} while (false);

	g->RequestDeclareWar(opp_g->GetID(), type);
}

ACMD(do_nowar)
{
	CGuild* g = ch->GetGuild();
	if (!g)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD gm_pid = g->GetMasterPID();

	if (gm_pid != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> You are not entitled to declare a Guild war."));
		return;
	}

	CGuild* opp_g = CGuildManager::instance().FindGuildByName(arg1);

	if (!opp_g)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> This Guild does not exist."));
		return;
	}

	g->RequestRefuseWar(opp_g->GetID());
}

ACMD(do_pkmode)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	BYTE mode = 0;
	str_to_number(mode, arg1);

	if (mode == PK_MODE_PROTECT)
		return;

	if (ch->GetLevel() < PK_PROTECT_LEVEL && mode != 0)
		return;

	ch->SetPKMode(mode);
}

ACMD(do_messenger_auth)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
		return;

	char answer = LOWER(*arg1);

	if (!MessengerManager::instance().AuthToAdd(ch->GetName(), arg2, answer == 'y' ? false : true))
		return;

	if (answer != 'y')
	{
		LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg2);

		if (tch)
			tch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Messenger> %s rejected your request."), ch->GetName());
	}
}

ACMD(do_setblockmode)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		BYTE flag = 0;
		str_to_number(flag, arg1);
		ch->SetBlockMode(flag);
	}
}

ACMD(do_setpickupmode)
{
	char arg1[256];
	char arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (*arg1 && *arg2 && isdigit(*arg1) && isdigit(*arg2))
	{
		BYTE mode;
		WORD flag;

		str_to_number(mode, arg1);
		str_to_number(flag, arg2);

		ch->SetPickupMode(mode);
		ch->SetPickupBlockFlag(flag);
	}
}

ACMD(do_unmount)
{
	if (true == ch->UnEquipSpecialRideUniqueItem())
	{
		ch->RemoveAffect(AFFECT_MOUNT);
		ch->RemoveAffect(AFFECT_MOUNT_BONUS);

		if (ch->IsHorseRiding())
		{
			ch->StopRiding(); 
		}
	}
	else
	{
		ch->ChatPacket( CHAT_TYPE_INFO, LC_TEXT("Your inventory is full."));
	}

}

ACMD(do_view_equip)
{
	if (ch->GetGMLevel() <= GM_PLAYER)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		DWORD vid = 0;
		str_to_number(vid, arg1);
		LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

		if (!tch)
			return;

		if (!tch->IsPC())
			return;

		tch->SendEquipment(ch);
	}
}

ACMD(do_party_request)
{
	if (ch->GetParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot accept the invitation because you are already in the Group."));
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		if (!ch->RequestToParty(tch))
			ch->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

ACMD(do_party_request_accept)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		ch->AcceptToParty(tch);
}

ACMD(do_party_request_deny)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		ch->DenyToParty(tch);
}

struct GotoInfo
{
	std::string 	st_name;

	BYTE 	empire;
	int 	mapIndex;
	DWORD 	x, y;

	GotoInfo()
	{
		st_name 	= "";
		empire 		= 0;
		mapIndex 	= 0;

		x = 0;
		y = 0;
	}

	GotoInfo(const GotoInfo& c_src)
	{
		__copy__(c_src);
	}

	void operator = (const GotoInfo& c_src)
	{
		__copy__(c_src);
	}

	void __copy__(const GotoInfo& c_src)
	{
		st_name 	= c_src.st_name;
		empire 		= c_src.empire;
		mapIndex 	= c_src.mapIndex;

		x = c_src.x;
		y = c_src.y;
	}
};

ACMD(do_inventory)
{
	int	index = 0;
	int	count		= 1;

	char arg1[256];
	char arg2[256];

	LPITEM	item;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: inventory <start_index> <count>");
		return;
	}

	if (!*arg2)
	{
		index = 0;
		str_to_number(count, arg1);
	}
	else
	{
		str_to_number(index, arg1); index = MIN(index, INVENTORY_MAX_NUM);
		str_to_number(count, arg2); count = MIN(count, INVENTORY_MAX_NUM);
	}

	for (int i = 0; i < count; ++i)
	{
		if (index >= INVENTORY_MAX_NUM)
			break;

		item = ch->GetInventoryItem(index);

		ch->ChatPacket(CHAT_TYPE_INFO, "inventory [%d] = %s",
						index, item ? item->GetName() : "<NONE>");
		++index;
	}
}

ACMD(do_gift)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "gift");
}

ACMD(do_cube)
{
	if (!ch->CanDoCube())
		return;

	int cube_index = 0, inven_index = 0;
	const char *line;

	char arg1[256], arg2[256], arg3[256];

	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));

	if (0 == arg1[0])
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: cube open");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube close");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube add <inveltory_index>");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube delete <cube_index>");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube list");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube cancel");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube make [all]");
		return;
	}

	const std::string& strArg1 = std::string(arg1);

	if (strArg1 == "r_info")
	{
		if (0 == arg2[0])
			Cube_request_result_list(ch);
		else
		{
			if (isdigit(*arg2))
			{
				int listIndex = 0, requestCount = 1;
				str_to_number(listIndex, arg2);

				if (0 != arg3[0] && isdigit(*arg3))
					str_to_number(requestCount, arg3);

				Cube_request_material_info(ch, listIndex, requestCount);
			}
		}

		return;
	}

	switch (LOWER(arg1[0]))
	{
		case 'o':
			Cube_open(ch);
			break;

		case 'c':
			Cube_close(ch);
			break;

		case 'l':
			Cube_show_list(ch);
			break;

		case 'a':
			{
				if (0 == arg2[0] || !isdigit(*arg2) ||
					0 == arg3[0] || !isdigit(*arg3))
					return;

				str_to_number(cube_index, arg2);
				str_to_number(inven_index, arg3);
				Cube_add_item (ch, cube_index, inven_index);
			}
			break;

		case 'd':
			{
				if (0 == arg2[0] || !isdigit(*arg2))
					return;

				str_to_number(cube_index, arg2);
				Cube_delete_item (ch, cube_index);
			}
			break;

		case 'm':
			Cube_make(ch);
			break;

		default:
			return;
	}
}

ACMD(do_in_game_mall)
{
	char country_code[3];
	country_code[0] = 'd';
	country_code[1] = 'e';
	country_code[2] = '\0';

	char buf[512 + 1];
	char sas[33];
	MD5_CTX ctx;
	const char sas_key[] = "GF9001";

	snprintf(buf, sizeof(buf), "%u%u%s", ch->GetPlayerID(), ch->GetAID(), sas_key);

	MD5Init(&ctx);
	MD5Update(&ctx, (const unsigned char*)buf, strlen(buf));
	MD5End(&ctx, sas);

	snprintf(buf, sizeof(buf), "mall http://%s", g_strWebMallURL.c_str());
	ch->ChatPacket(CHAT_TYPE_COMMAND, buf);
}

ACMD(do_click_mall)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeMallPassword");
}

ACMD(do_ride)
{
	if (ch->IsDead() || ch->IsStun())
		return;

	if (ch->IsPolymorphed() == true)
		return;

	{
		if (ch->IsHorseRiding())
		{
			ch->StopRiding();
			return;
		}

		if (ch->GetMountVnum())
		{
			do_unmount(ch, NULL, 0, 0);
			return;
		}
	}

	if (ch->GetHorse() != NULL)
	{
		ch->StartRiding();
		return;
	}

	if (ch->GetWear(WEAR_COSTUME_MOUNT))
	{
		ch->HorseSummon(true);
		ch->StartRiding();
		return;
	}
	else
	{
		for (BYTE i = 0; i < INVENTORY_MAX_NUM; ++i)
		{
			LPITEM item = ch->GetInventoryItem(i);
			if (NULL == item)
			{
				continue;
			}

			if (item->IsCostumeMountItem())
			{
				ch->EquipItem(item);
				ch->HorseSummon(true);
				ch->StartRiding();
				return;
			}
		}
	}

	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please call your Horse first."));
}

#ifdef ENABLE_INVENTORY_SORT
bool sortByType(CItem* a, CItem* b)
{
	return (a->GetType() < b->GetType());
}

bool sortBySubType(CItem* a, CItem* b)
{
	return (a->GetSubType() < b->GetSubType());
}

bool sortByVnum(CItem* a, CItem* b)
{
	return (a->GetVnum() < b->GetVnum());
}

bool sortBySocket(CItem* a, CItem* b)
{
	return (a->GetSocket(0) < b->GetSocket(0));
}

ACMD(do_sort_inventory)
{
	if (!ch->CanHandleItem())
	{
		return;
	}

	std::vector<CItem*> collectItems;
	int totalSize = 0;

	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		LPITEM item = ch->GetInventoryItem(i);

		if (item)
		{
			totalSize += item->GetSize();
			collectItems.push_back(item);
		}
	}

	if (totalSize - 3 >= INVENTORY_MAX_NUM)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("INVENTORY_FULL_CANNOT_SORT"));
		return;
	}

	for (std::vector<CItem*>::iterator it = collectItems.begin(); it != collectItems.end(); ++it)
	{
		LPITEM item = *it;
		item->RemoveFromCharacter();
	}

	std::sort(collectItems.begin(), collectItems.end(), sortByType);
	std::sort(collectItems.begin(), collectItems.end(), sortBySubType);
	std::sort(collectItems.begin(), collectItems.end(), sortByVnum);
	std::sort(collectItems.begin(), collectItems.end(), sortBySocket);

	for (std::vector<CItem*>::iterator iit = collectItems.begin(); iit < collectItems.end(); ++iit)
	{
		LPITEM sortedItem = *iit;
		if (sortedItem)
		{
			DWORD dwCount = sortedItem->GetCount();

			if (sortedItem->IsStackable() && !IS_SET(sortedItem->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
			{
				for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
				{
					LPITEM item2 = ch->GetInventoryItem(i);

					if (!item2)
					{
						continue;
					}

					if (item2->GetVnum() == sortedItem->GetVnum())
					{
						int j;

						for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
						{
							if (item2->GetSocket(j) != sortedItem->GetSocket(j))
							{
								break;
							}
						}

						if (j != ITEM_SOCKET_MAX_NUM)
						{
							continue;
						}

						DWORD dwCount2 = MIN(MAX_ITEM_STACK - item2->GetCount(), dwCount);
						dwCount -= dwCount2;

						item2->SetCount(item2->GetCount() + dwCount2);

						if (dwCount == 0)
						{
							M2_DESTROY_ITEM(sortedItem);
							break;
						}
						else
						{
							sortedItem->SetCount(dwCount);
						}
					}
				}
			}

			if (dwCount > 0)
			{
#ifdef ENABLE_SPECIAL_INVENTORY
				int cell = ch->GetEmptyInventory(sortedItem);
#else
				int cell = ch->GetEmptyInventory(sortedItem->GetSize());
#endif
				sortedItem->AddToCharacter(ch, TItemPos(INVENTORY, cell));
			}
		}
	}
}

#ifdef ENABLE_SPECIAL_INVENTORY

ACMD(do_sort_special_inventory)
{
	if (!ch->CanHandleItem())
	{
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	BYTE specialInventoryType = atoi(arg1);

	std::vector<CItem*> collectItems;

	int startIndex = SPECIAL_INVENTORY_SLOT_START + (SPECIAL_INVENTORY_PAGE_SIZE * INVENTORY_PAGE_NUM) * specialInventoryType;
	int endIndex = SPECIAL_INVENTORY_SLOT_START + (SPECIAL_INVENTORY_PAGE_SIZE * INVENTORY_PAGE_NUM) * (specialInventoryType + 1);

	for (int i = startIndex; i < endIndex; ++i)
	{
		LPITEM item = ch->GetInventoryItem(i);

		if (item)
		{
			collectItems.push_back(item);
		}
	}

	for (std::vector<CItem*>::iterator it = collectItems.begin(); it != collectItems.end(); ++it)
	{
		LPITEM item = *it;
		item->RemoveFromCharacter();
	}

	std::sort(collectItems.begin(), collectItems.end(), sortByType);
	std::sort(collectItems.begin(), collectItems.end(), sortBySubType);
	std::sort(collectItems.begin(), collectItems.end(), sortByVnum);
	std::sort(collectItems.begin(), collectItems.end(), sortBySocket);

	for (std::vector<CItem*>::iterator iit = collectItems.begin(); iit < collectItems.end(); ++iit)
	{
		LPITEM sortedItem = *iit;
		if (sortedItem)
		{
			DWORD dwCount = sortedItem->GetCount();

			if (sortedItem->IsStackable() && !IS_SET(sortedItem->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
			{
				for (int i = startIndex; i < endIndex; ++i)
				{
					LPITEM item2 = ch->GetInventoryItem(i);

					if (!item2)
					{
						continue;
					}

					if (item2->GetVnum() == sortedItem->GetVnum())
					{
						int j;

						for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
						{
							if (item2->GetSocket(j) != sortedItem->GetSocket(j))
							{
								break;
							}
						}

						if (j != ITEM_SOCKET_MAX_NUM)
						{
							continue;
						}

						DWORD dwCount2 = MIN(MAX_ITEM_STACK - item2->GetCount(), dwCount);
						dwCount -= dwCount2;

						item2->SetCount(item2->GetCount() + dwCount2);

						if (dwCount == 0)
						{
							M2_DESTROY_ITEM(sortedItem);
							break;
						}
						else
						{
							sortedItem->SetCount(dwCount);
						}
					}
				}
			}

			if (dwCount > 0)
			{
				int cell = ch->GetEmptyInventory(sortedItem);
				sortedItem->AddToCharacter(ch, TItemPos(INVENTORY, cell));
			}
		}
	}
}

#endif
#endif

ACMD(do_stat_val)
{
	char	arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	int val = 0;
	str_to_number(val, arg2);

	if (!*arg1 || val <= 0)
	{
		return;
	}

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change your state as long as you are transformed."));
		return;
	}

	if (ch->GetPoint(POINT_STAT) <= 0)
	{
		return;
	}

	BYTE idx = 0;

	if (!strcmp(arg1, "st"))
		idx = POINT_ST;
	else if (!strcmp(arg1, "dx"))
		idx = POINT_DX;
	else if (!strcmp(arg1, "ht"))
		idx = POINT_HT;
	else if (!strcmp(arg1, "iq"))
		idx = POINT_IQ;
	else
		return;

	if (ch->GetRealPoint(idx) >= MAX_STAT)
		return;

	if (val > ch->GetPoint(POINT_STAT))
		val = ch->GetPoint(POINT_STAT);

	if (ch->GetRealPoint(idx) + val > MAX_STAT)
		val = MAX_STAT - ch->GetRealPoint(idx);

	ch->SetRealPoint(idx, ch->GetRealPoint(idx) + val);
	ch->SetPoint(idx, ch->GetPoint(idx) + val);
	ch->ComputePoints();
	ch->PointChange(idx, 0);

	if (idx == POINT_IQ)
	{
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (idx == POINT_HT)
	{
		ch->PointChange(POINT_MAX_SP, 0);
	}

	ch->PointChange(POINT_STAT, -val);
	ch->ComputePoints();
}
