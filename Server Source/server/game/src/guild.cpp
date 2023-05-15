#include "stdafx.h"
#include "utils.h"
#include "config.h"
#include "char.h"
#include "packet.h"
#include "desc_client.h"
#include "buffer_manager.h"
#include "char_manager.h"
#include "db.h"
#include "guild.h"
#include "guild_manager.h"
#include "affect.h"
#include "p2p.h"
#include "questmanager.h"
#include "building.h"
#include "locale_service.h"
#include "log.h"
#include "questmanager.h"

	SGuildMember::SGuildMember(LPCHARACTER ch, BYTE grade, DWORD offer_exp)
: pid(ch->GetPlayerID()), grade(grade), is_general(0), job(ch->GetJob()), level(ch->GetLevel()), offer_exp(offer_exp), name(ch->GetName())
{}
	SGuildMember::SGuildMember(DWORD pid, BYTE grade, BYTE is_general, BYTE job, BYTE level, DWORD offer_exp, char* name)
: pid(pid), grade(grade), is_general(is_general), job(job), level(level), offer_exp(offer_exp), name(name)
{}

namespace 
{
	struct FGuildNameSender
	{
		FGuildNameSender(DWORD id, const char* guild_name) : id(id), name(guild_name)
		{
			p.header = HEADER_GC_GUILD;
			p.subheader = GUILD_SUBHEADER_GC_GUILD_NAME;
			p.size = sizeof(p) + sizeof(DWORD) + GUILD_NAME_MAX_LEN;
		}

		void operator() (LPCHARACTER ch)
		{
			LPDESC d = ch->GetDesc();

			if (d)
			{
				d->BufferedPacket(&p, sizeof(p));
				d->BufferedPacket(&id, sizeof(id));
				d->Packet(name, GUILD_NAME_MAX_LEN);
			}
		}

		DWORD id;
		const char * name;
		TPacketGCGuild p;
	};
}

CGuild::CGuild(TGuildCreateParameter & cp)
{
	Initialize();

	m_general_count = 0;

	m_iMemberCountBonus = 0;

	strlcpy(m_data.name, cp.name, sizeof(m_data.name));
	m_data.master_pid = cp.master->GetPlayerID();
	strlcpy(m_data.grade_array[0].grade_name, LC_TEXT("Leader"), sizeof(m_data.grade_array[0].grade_name));
	m_data.grade_array[0].auth_flag = GUILD_AUTH_ADD_MEMBER | GUILD_AUTH_REMOVE_MEMBER | GUILD_AUTH_NOTICE | GUILD_AUTH_USE_SKILL;

	for (int i = 1; i < GUILD_GRADE_COUNT; ++i)
	{
		strlcpy(m_data.grade_array[i].grade_name, LC_TEXT("Member"), sizeof(m_data.grade_array[i].grade_name));
		m_data.grade_array[i].auth_flag = 0;
	}

	std::unique_ptr<SQLMsg> pmsg (DBManager::instance().DirectQuery(
				"INSERT INTO guild%s(name, master, sp, level, exp, skill_point, skill) "
				"VALUES('%s', %u, 1000, 1, 0, 0, '\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0')", 
				get_table_postfix(), m_data.name, m_data.master_pid));

	m_data.guild_id = pmsg->Get()->uiInsertID;

	for (int i = 0; i < GUILD_GRADE_COUNT; ++i)
	{
		DBManager::instance().Query("INSERT INTO guild_grade%s VALUES(%u, %d, '%s', %d)", 
				get_table_postfix(),
				m_data.guild_id, 
				i + 1, 
				m_data.grade_array[i].grade_name, 
				m_data.grade_array[i].auth_flag);
	}

	ComputeGuildPoints();
	m_data.power	= m_data.max_power;
	m_data.ladder_point	= 0;
	db_clientdesc->DBPacket(HEADER_GD_GUILD_CREATE, 0, &m_data.guild_id, sizeof(DWORD));

	TPacketGuildSkillUpdate guild_skill;
	guild_skill.guild_id = m_data.guild_id;
	guild_skill.amount = 0;
	guild_skill.skill_point = 0;
	memset(guild_skill.skill_levels, 0, GUILD_SKILL_COUNT);

	db_clientdesc->DBPacket(HEADER_GD_GUILD_SKILL_UPDATE, 0, &guild_skill, sizeof(guild_skill));

	CHARACTER_MANAGER::instance().for_each_pc(FGuildNameSender(GetID(), GetName()));
	RequestAddMember(cp.master, GUILD_LEADER_GRADE);
}

void CGuild::Initialize()
{
	memset(&m_data, 0, sizeof(m_data));
	m_data.level = 1;

	for (int i = 0; i < GUILD_SKILL_COUNT; ++i)
		abSkillUsable[i] = true;

	m_iMemberCountBonus = 0;
}

CGuild::~CGuild()
{
}

void CGuild::RequestAddMember(LPCHARACTER ch, int grade)
{
	if (ch->GetGuild())
		return;

	TPacketGDGuildAddMember gd;

	if (m_member.find(ch->GetPlayerID()) != m_member.end())
	{
		sys_err("Already a member in guild %s[%d]", ch->GetName(), ch->GetPlayerID());
		return;
	}

	gd.dwPID = ch->GetPlayerID();
	gd.dwGuild = GetID();
	gd.bGrade = grade;

	db_clientdesc->DBPacket(HEADER_GD_GUILD_ADD_MEMBER, 0, &gd, sizeof(TPacketGDGuildAddMember));
}

void CGuild::AddMember(TPacketDGGuildMember * p)
{
	TGuildMemberContainer::iterator it;

	if ((it = m_member.find(p->dwPID)) == m_member.end())
		m_member.insert(std::make_pair(p->dwPID, TGuildMember(p->dwPID, p->bGrade, p->isGeneral, p->bJob, p->bLevel, p->dwOffer, p->szName)));
	else
	{
		TGuildMember & r_gm = it->second;
		r_gm.pid = p->dwPID;
		r_gm.grade = p->bGrade;
		r_gm.job = p->bJob;
		r_gm.offer_exp = p->dwOffer;
		r_gm.is_general = p->isGeneral;
	}

	CGuildManager::instance().Link(p->dwPID, this);

	SendListOneToAll(p->dwPID);

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(p->dwPID);

	sys_log(0, "GUILD: AddMember PID %u, grade %u, job %u, level %u, offer %u, name %s ptr %p",
			p->dwPID, p->bGrade, p->bJob, p->bLevel, p->dwOffer, p->szName, get_pointer(ch));

	if (ch)
		LoginMember(ch);
	else
		P2PLoginMember(p->dwPID);
}

bool CGuild::RequestRemoveMember(DWORD pid)
{
	TGuildMemberContainer::iterator it;

	if ((it = m_member.find(pid)) == m_member.end())
		return false;

	if (it->second.grade == GUILD_LEADER_GRADE)
		return false;

	TPacketGuild gd_guild;

	gd_guild.dwGuild = GetID();
	gd_guild.dwInfo = pid;

	db_clientdesc->DBPacket(HEADER_GD_GUILD_REMOVE_MEMBER, 0, &gd_guild, sizeof(TPacketGuild));
	return true;
}

bool CGuild::RemoveMember(DWORD pid)
{
	sys_log(0, "Receive Guild P2P RemoveMember");
	TGuildMemberContainer::iterator it;

	if ((it = m_member.find(pid)) == m_member.end())
		return false;

	if (it->second.grade == GUILD_LEADER_GRADE)
		return false;

	if (it->second.is_general)
		m_general_count--;

	m_member.erase(it);
	SendOnlineRemoveOnePacket(pid);

	CGuildManager::instance().Unlink(pid);

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(pid);

	if (ch)
	{
		m_memberOnline.erase(ch);
		ch->SetGuild(NULL);
	}

	return true;
}

void CGuild::P2PLoginMember(DWORD pid)
{
	if (m_member.find(pid) == m_member.end())
	{
		sys_err("GUILD [%d] is not a memeber of guild.", pid);
		return;
	}

	m_memberP2POnline.insert(pid);

	for (auto it = m_memberOnline.begin(); it!=m_memberOnline.end();++it)
		SendLoginPacket(*it, pid);
}

void CGuild::LoginMember(LPCHARACTER ch)
{
	if (m_member.find(ch->GetPlayerID()) == m_member.end())
	{
		sys_err("GUILD %s[%d] is not a memeber of guild.", ch->GetName(), ch->GetPlayerID());
		return;
	}

	ch->SetGuild(this);

	for (auto it = m_memberOnline.begin(); it!=m_memberOnline.end();++it)
		SendLoginPacket(*it, ch);

	m_memberOnline.insert(ch);

	SendAllGradePacket(ch);
	SendGuildInfoPacket(ch);
	SendListPacket(ch);
	SendSkillInfoPacket(ch);
	SendEnemyGuild(ch);
}

void CGuild::P2PLogoutMember(DWORD pid)
{
	if (m_member.find(pid)==m_member.end())
	{
		sys_err("GUILD [%d] is not a memeber of guild.", pid);
		return;
	}

	m_memberP2POnline.erase(pid);

	for (auto it = m_memberOnline.begin(); it!=m_memberOnline.end();++it)
	{
		SendLogoutPacket(*it, pid);
	}
}

void CGuild::LogoutMember(LPCHARACTER ch)
{
	if (m_member.find(ch->GetPlayerID())==m_member.end())
	{
		sys_err("GUILD %s[%d] is not a memeber of guild.", ch->GetName(), ch->GetPlayerID());
		return;
	}

	m_memberOnline.erase(ch);

	for (auto it = m_memberOnline.begin(); it!=m_memberOnline.end();++it)
	{
		SendLogoutPacket(*it, ch);
	}
}

void CGuild::SendOnlineRemoveOnePacket(DWORD pid)
{
	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack)+4;
	pack.subheader = GUILD_SUBHEADER_GC_REMOVE;

	TEMP_BUFFER buf;
	buf.write(&pack,sizeof(pack));
	buf.write(&pid, sizeof(pid));

	for (auto it = m_memberOnline.begin(); it!=m_memberOnline.end();++it)
	{
		LPDESC d = (*it)->GetDesc();

		if (d)
			d->Packet(buf.read_peek(), buf.size());
	}
}

void CGuild::SendAllGradePacket(LPCHARACTER ch)
{
	LPDESC d = ch->GetDesc();
	if (!d)
		return;

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack)+1+GUILD_GRADE_COUNT*(sizeof(TGuildGrade)+1);
	pack.subheader = GUILD_SUBHEADER_GC_GRADE;

	TEMP_BUFFER buf;

	buf.write(&pack, sizeof(pack));
	BYTE n = 15;
	buf.write(&n, 1);

	for (int i=0;i<GUILD_GRADE_COUNT;i++)
	{
		BYTE j = i+1;
		buf.write(&j, 1);
		buf.write(&m_data.grade_array[i], sizeof(TGuildGrade));
	}

	d->Packet(buf.read_peek(), buf.size());
}

void CGuild::SendListOneToAll(LPCHARACTER ch)
{
	SendListOneToAll(ch->GetPlayerID());
}

void CGuild::SendListOneToAll(DWORD pid)
{

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(TPacketGCGuild);
	pack.subheader = GUILD_SUBHEADER_GC_LIST;

	pack.size += sizeof(TGuildMemberPacketData);

	char c[CHARACTER_NAME_MAX_LEN+1];
	memset(c, 0, sizeof(c));

	auto cit = m_member.find(pid);
	if (cit == m_member.end())
		return;

	for (auto it = m_memberOnline.begin(); it!= m_memberOnline.end(); ++it)
	{
		LPDESC d = (*it)->GetDesc();
		if (!d) 
			continue;

		TEMP_BUFFER buf;

		buf.write(&pack, sizeof(pack));

		cit->second._dummy = 1;

		buf.write(&(cit->second), sizeof(DWORD) * 3 +1);
		buf.write(cit->second.name.c_str(), cit->second.name.length());
		buf.write(c, CHARACTER_NAME_MAX_LEN + 1 - cit->second.name.length());
		d->Packet(buf.read_peek(), buf.size());
	}
}

void CGuild::SendListPacket(LPCHARACTER ch)
{
	LPDESC d;
	if (!(d=ch->GetDesc()))
		return;

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(TPacketGCGuild);
	pack.subheader = GUILD_SUBHEADER_GC_LIST;

	pack.size += sizeof(TGuildMemberPacketData) * m_member.size();

	TEMP_BUFFER buf;

	buf.write(&pack,sizeof(pack));

	char c[CHARACTER_NAME_MAX_LEN+1];

	for (auto it = m_member.begin(); it != m_member.end(); ++it)
	{
		it->second._dummy = 1;

		buf.write(&(it->second), sizeof(DWORD)*3+1);

		strlcpy(c, it->second.name.c_str(), MIN(sizeof(c), it->second.name.length() + 1));

		buf.write(c, CHARACTER_NAME_MAX_LEN+1 );

		if ( test_server )
			sys_log(0 ,"name %s job %d  ", it->second.name.c_str(), it->second.job );
	}

	d->Packet(buf.read_peek(), buf.size());

	for (auto it = m_memberOnline.begin(); it != m_memberOnline.end(); ++it)
	{
		SendLoginPacket(ch, *it);
	}

	for (auto it = m_memberP2POnline.begin(); it != m_memberP2POnline.end(); ++it)
	{
		SendLoginPacket(ch, *it);
	}

}

void CGuild::SendLoginPacket(LPCHARACTER ch, LPCHARACTER chLogin)
{
	SendLoginPacket(ch, chLogin->GetPlayerID());
}

void CGuild::SendLoginPacket(LPCHARACTER ch, DWORD pid)
{
	if (!ch->GetDesc())
		return;

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack)+4;
	pack.subheader = GUILD_SUBHEADER_GC_LOGIN;

	TEMP_BUFFER buf;

	buf.write(&pack, sizeof(pack));

	buf.write(&pid, 4);

	ch->GetDesc()->Packet(buf.read_peek(), buf.size());
}

void CGuild::SendLogoutPacket(LPCHARACTER ch, LPCHARACTER chLogout)
{
	SendLogoutPacket(ch, chLogout->GetPlayerID());
}

void CGuild::SendLogoutPacket(LPCHARACTER ch, DWORD pid)
{
	if (!ch->GetDesc())
		return;

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack)+4;
	pack.subheader = GUILD_SUBHEADER_GC_LOGOUT;

	TEMP_BUFFER buf;

	buf.write(&pack, sizeof(pack));
	buf.write(&pid, 4);

	ch->GetDesc()->Packet(buf.read_peek(), buf.size());
}

void CGuild::LoadGuildMemberData(SQLMsg* pmsg)
{
	if (pmsg->Get()->uiNumRows == 0)
		return;

	m_general_count = 0;

	m_member.clear();

	for (uint i = 0; i < pmsg->Get()->uiNumRows; ++i)
	{
		MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);

		DWORD pid = strtoul(row[0], (char**) NULL, 10);
		BYTE grade = (BYTE) strtoul(row[1], (char**) NULL, 10);
		BYTE is_general = 0;

		if (row[2] && *row[2] == '1')
			is_general = 1;

		DWORD offer = strtoul(row[3], (char**) NULL, 10);
		BYTE level = (BYTE)strtoul(row[4], (char**) NULL, 10);
		BYTE job = (BYTE)strtoul(row[5], (char**) NULL, 10);
		char * name = row[6];

		if (is_general)
			m_general_count++;

		m_member.insert(std::make_pair(pid, TGuildMember(pid, grade, is_general, job, level, offer, name)));
		CGuildManager::instance().Link(pid, this);
	}
}

void CGuild::LoadGuildGradeData(SQLMsg* pmsg)
{
	for (uint i = 0; i < pmsg->Get()->uiNumRows; ++i)
	{
		MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);
		BYTE grade = 0;
		str_to_number(grade, row[0]);
		char * name = row[1];
		DWORD auth = strtoul(row[2], NULL, 10);

		if (grade >= 1 && grade <= 15)
		{
			strlcpy(m_data.grade_array[grade-1].grade_name, name, sizeof(m_data.grade_array[grade-1].grade_name));
			m_data.grade_array[grade-1].auth_flag = auth;
		}
	}
}
void CGuild::LoadGuildData(SQLMsg* pmsg)
{
	if (pmsg->Get()->uiNumRows == 0)
	{
		sys_err("Query failed: getting guild data %s", pmsg->stQuery.c_str());
		return;
	}

	MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);
	m_data.master_pid = strtoul(row[0], (char **)NULL, 10);
	m_data.level = (BYTE)strtoul(row[1], (char **)NULL, 10);
	m_data.exp = strtoul(row[2], (char **)NULL, 10);
	strlcpy(m_data.name, row[3], sizeof(m_data.name));

	m_data.skill_point = (BYTE) strtoul(row[4], (char **) NULL, 10);
	if (row[5])
		thecore_memcpy(m_data.abySkill, row[5], sizeof(BYTE) * GUILD_SKILL_COUNT);
	else
		memset(m_data.abySkill, 0, sizeof(BYTE) * GUILD_SKILL_COUNT);

	m_data.power = MAX(0, strtoul(row[6], (char **) NULL, 10));

	str_to_number(m_data.ladder_point, row[7]);

	if (m_data.ladder_point < 0)
		m_data.ladder_point = 0;

	str_to_number(m_data.win, row[8]);
	str_to_number(m_data.draw, row[9]);
	str_to_number(m_data.loss, row[10]);
	str_to_number(m_data.gold, row[11]);

	ComputeGuildPoints();
}

void CGuild::Load(DWORD guild_id)
{
	Initialize();

	m_data.guild_id = guild_id;

	DBManager::instance().FuncQuery(std::bind(&CGuild::LoadGuildData, this, std::placeholders::_1),
		"SELECT master, level, exp, name, skill_point, skill, sp, ladder_point, win, draw, loss, gold FROM guild%s WHERE id = %u", get_table_postfix(), m_data.guild_id);

	sys_log(0, "GUILD: loading guild id %12s %u", m_data.name, guild_id);

	DBManager::instance().FuncQuery(std::bind(&CGuild::LoadGuildGradeData, this, std::placeholders::_1),
		"SELECT grade, name, auth+0 FROM guild_grade%s WHERE guild_id = %u", get_table_postfix(), m_data.guild_id);

	DBManager::instance().FuncQuery(std::bind(&CGuild::LoadGuildMemberData, this, std::placeholders::_1),
		"SELECT pid, grade, is_general, offer, level, job, name FROM guild_member%s, player%s WHERE guild_id = %u and pid = id", get_table_postfix(), get_table_postfix(), guild_id);
}

void CGuild::SaveLevel()
{
	DBManager::instance().Query("UPDATE guild%s SET level=%d, exp=%u, skill_point=%d WHERE id = %u", get_table_postfix(), m_data.level,m_data.exp, m_data.skill_point,m_data.guild_id);
}

void CGuild::SendDBSkillUpdate(int amount)
{
	TPacketGuildSkillUpdate guild_skill;
	guild_skill.guild_id = m_data.guild_id;
	guild_skill.amount = amount;
	guild_skill.skill_point = m_data.skill_point;
	thecore_memcpy(guild_skill.skill_levels, m_data.abySkill, sizeof(BYTE) * GUILD_SKILL_COUNT);

	db_clientdesc->DBPacket(HEADER_GD_GUILD_SKILL_UPDATE, 0, &guild_skill, sizeof(guild_skill));
}

void CGuild::SaveSkill()
{
	char text[GUILD_SKILL_COUNT * 2 + 1];

	DBManager::instance().EscapeString(text, sizeof(text), (const char *) m_data.abySkill, sizeof(m_data.abySkill));
	DBManager::instance().Query("UPDATE guild%s SET sp = %d, skill_point=%d, skill='%s' WHERE id = %u",
			get_table_postfix(), m_data.power, m_data.skill_point, text, m_data.guild_id);
}

TGuildMember* CGuild::GetMember(DWORD pid)
{
	auto it = m_member.find(pid);
	if (it==m_member.end())
		return NULL;

	return &it->second;
}

DWORD CGuild::GetMemberPID(const std::string& strName)
{
	for (auto iter = m_member.begin();
			iter != m_member.end(); iter++ )
	{
		if ( iter->second.name == strName ) return iter->first;
	}

	return 0;
}

void CGuild::__P2PUpdateGrade(SQLMsg* pmsg)
{
	if (pmsg->Get()->uiNumRows)
	{
		MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);
		
		int grade = 0;
		const char* name = row[1];
		int auth = 0;

		str_to_number(grade, row[0]);
		str_to_number(auth, row[2]);

		if (grade <= 0)
			return;

		grade--;

		if (0 != strcmp(m_data.grade_array[grade].grade_name, name))
		{
			strlcpy(m_data.grade_array[grade].grade_name, name, sizeof(m_data.grade_array[grade].grade_name));

			TPacketGCGuild pack;
			
			pack.header = HEADER_GC_GUILD;
			pack.size = sizeof(pack);
			pack.subheader = GUILD_SUBHEADER_GC_GRADE_NAME;

			TOneGradeNamePacket pack2;

			pack.size += sizeof(pack2);
			pack2.grade = grade + 1;
			strlcpy(pack2.grade_name, name, sizeof(pack2.grade_name));

			TEMP_BUFFER buf;

			buf.write(&pack,sizeof(pack));
			buf.write(&pack2,sizeof(pack2));

			for (auto it = m_memberOnline.begin(); it!=m_memberOnline.end(); ++it)
			{
				LPDESC d = (*it)->GetDesc();

				if (d)
					d->Packet(buf.read_peek(), buf.size());
			}
		}

		if (m_data.grade_array[grade].auth_flag != auth)
		{
			m_data.grade_array[grade].auth_flag = auth;

			TPacketGCGuild pack;
			pack.header = HEADER_GC_GUILD;
			pack.size = sizeof(pack);
			pack.subheader = GUILD_SUBHEADER_GC_GRADE_AUTH;

			TOneGradeAuthPacket pack2;
			pack.size+=sizeof(pack2);
			pack2.grade = grade+1;
			pack2.auth = auth;

			TEMP_BUFFER buf;
			buf.write(&pack,sizeof(pack));
			buf.write(&pack2,sizeof(pack2));

			for (auto it = m_memberOnline.begin(); it!=m_memberOnline.end(); ++it)
			{
				LPDESC d = (*it)->GetDesc();
				if (d)
				{
					d->Packet(buf.read_peek(), buf.size());
				}
			}
		}
	}
}

void CGuild::P2PChangeGrade(BYTE grade)
{
	DBManager::instance().FuncQuery(std::bind(&CGuild::__P2PUpdateGrade, this, std::placeholders::_1),
		"SELECT grade, name, auth+0 FROM guild_grade%s WHERE guild_id = %u and grade = %d", get_table_postfix(), m_data.guild_id, grade);
}

namespace 
{
	struct FSendChangeGrade
	{
		BYTE grade;
		TPacketGuild p;

		FSendChangeGrade(DWORD guild_id, BYTE grade) : grade(grade)
		{
			p.dwGuild = guild_id;
			p.dwInfo = grade;
		}

		void operator()()
		{
			db_clientdesc->DBPacket(HEADER_GD_GUILD_CHANGE_GRADE, 0, &p, sizeof(p));
		}
	};
}

void CGuild::ChangeGradeName(BYTE grade, const char* grade_name)
{
	if (grade == 1)
		return;

	if (grade < 1 || grade > 15)
	{
		sys_err("Wrong guild grade value %d", grade);
		return;
	}

	if (strlen(grade_name) > GUILD_NAME_MAX_LEN)
		return;

	if (!*grade_name)
		return;

	char text[GUILD_NAME_MAX_LEN * 2 + 1];

	DBManager::instance().EscapeString(text, sizeof(text), grade_name, strlen(grade_name));
	DBManager::instance().FuncAfterQuery(FSendChangeGrade(GetID(), grade), "UPDATE guild_grade%s SET name = '%s' where guild_id = %u and grade = %d", get_table_postfix(), text, m_data.guild_id, grade);

	grade--;
	strlcpy(m_data.grade_array[grade].grade_name, grade_name, sizeof(m_data.grade_array[grade].grade_name));

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack);
	pack.subheader = GUILD_SUBHEADER_GC_GRADE_NAME;

	TOneGradeNamePacket pack2;
	pack.size+=sizeof(pack2);
	pack2.grade = grade+1;
	strlcpy(pack2.grade_name,grade_name, sizeof(pack2.grade_name));

	TEMP_BUFFER buf;
	buf.write(&pack,sizeof(pack));
	buf.write(&pack2,sizeof(pack2));

	for (auto it = m_memberOnline.begin(); it!=m_memberOnline.end(); ++it)
	{
		LPDESC d = (*it)->GetDesc();

		if (d)
			d->Packet(buf.read_peek(), buf.size());
	}
}

void CGuild::ChangeGradeAuth(BYTE grade, BYTE auth)
{
	if (grade == 1)
		return;

	if (grade < 1 || grade > 15)
	{
		sys_err("Wrong guild grade value %d", grade);
		return;
	}

	DBManager::instance().FuncAfterQuery(FSendChangeGrade(GetID(),grade), "UPDATE guild_grade%s SET auth = %d where guild_id = %u and grade = %d", get_table_postfix(), auth, m_data.guild_id, grade);

	grade--;

	m_data.grade_array[grade].auth_flag=auth;

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack);
	pack.subheader = GUILD_SUBHEADER_GC_GRADE_AUTH;

	TOneGradeAuthPacket pack2;
	pack.size += sizeof(pack2);
	pack2.grade = grade + 1;
	pack2.auth = auth;

	TEMP_BUFFER buf;
	buf.write(&pack, sizeof(pack));
	buf.write(&pack2, sizeof(pack2));

	for (auto it = m_memberOnline.begin(); it != m_memberOnline.end(); ++it)
	{
		LPDESC d = (*it)->GetDesc();

		if (d)
			d->Packet(buf.read_peek(), buf.size());
	}
}

void CGuild::SendGuildInfoPacket(LPCHARACTER ch)
{
	LPDESC d = ch->GetDesc();

	if (!d)
		return;

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(TPacketGCGuild) + sizeof(TPacketGCGuildInfo);
	pack.subheader = GUILD_SUBHEADER_GC_INFO;

	TPacketGCGuildInfo pack_sub;

	memset(&pack_sub, 0, sizeof(TPacketGCGuildInfo));
	pack_sub.member_count = GetMemberCount(); 
	pack_sub.max_member_count = GetMaxMemberCount();
	pack_sub.guild_id = m_data.guild_id;
	pack_sub.master_pid = m_data.master_pid;
	pack_sub.exp	= m_data.exp;
	pack_sub.level	= m_data.level;
	strlcpy(pack_sub.name, m_data.name, sizeof(pack_sub.name));
	pack_sub.gold	= m_data.gold;
	pack_sub.has_land	= HasLand();

	sys_log(0, "GMC guild_name %s", m_data.name);
	sys_log(0, "GMC master %d", m_data.master_pid);

	d->BufferedPacket(&pack, sizeof(TPacketGCGuild));
	d->Packet(&pack_sub, sizeof(TPacketGCGuildInfo));
}

bool CGuild::OfferExp(LPCHARACTER ch, int amount)
{
	auto cit = m_member.find(ch->GetPlayerID());

	if (cit == m_member.end())
		return false;

	if (m_data.exp+amount < m_data.exp)
		return false;

	if (amount < 0)
		return false;

	if (ch->GetExp() < (DWORD) amount)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> Provided Experience is larger then left Experience."));
		return false;
	}

	if (ch->GetExp() - (DWORD) amount > ch->GetExp())
	{
		sys_err("Wrong guild offer amount %d by %s[%u]", amount, ch->GetName(), ch->GetPlayerID());
		return false;
	}

	ch->PointChange(POINT_EXP, -amount);

	TPacketGuildExpUpdate guild_exp;
	guild_exp.guild_id = GetID();
	guild_exp.amount = amount / 100;
	db_clientdesc->DBPacket(HEADER_GD_GUILD_EXP_UPDATE, 0, &guild_exp, sizeof(guild_exp));
	GuildPointChange(POINT_EXP, amount / 100, true);

	cit->second.offer_exp += amount / 100;
	cit->second._dummy = 0;

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;

	for (auto it = m_memberOnline.begin(); it != m_memberOnline.end(); ++it)
	{
		LPDESC d = (*it)->GetDesc();
		if (d)
		{
			pack.subheader = GUILD_SUBHEADER_GC_LIST;
			pack.size = sizeof(pack) + 13;
			d->BufferedPacket(&pack, sizeof(pack));
			d->Packet(&(cit->second), sizeof(DWORD) * 3 + 1);
		}
	}

	SaveMember(ch->GetPlayerID());

	TPacketGuildChangeMemberData gd_guild;

	gd_guild.guild_id = GetID();
	gd_guild.pid = ch->GetPlayerID();
	gd_guild.offer = cit->second.offer_exp;
	gd_guild.level = ch->GetLevel();
	gd_guild.grade = cit->second.grade;

	db_clientdesc->DBPacket(HEADER_GD_GUILD_CHANGE_MEMBER_DATA, 0, &gd_guild, sizeof(gd_guild));
	return true;
}

void CGuild::Disband()
{
	sys_log(0, "GUILD: Disband %s:%u", GetName(), GetID());

	for (auto it = m_memberOnline.begin(); it != m_memberOnline.end(); ++it)
	{
		LPCHARACTER ch = *it;
		ch->SetGuild(NULL);
		SendOnlineRemoveOnePacket(ch->GetPlayerID());
		ch->SetQuestFlag("guild_manage.new_withdraw_time", get_global_time());
	}

	for (auto it = m_member.begin(); it != m_member.end(); ++it)
	{
		CGuildManager::instance().Unlink(it->first);
	}

}

void CGuild::RequestDisband(DWORD pid)
{
	if (m_data.master_pid != pid)
		return;

	TPacketGuild gd_guild;
	gd_guild.dwGuild = GetID();
	gd_guild.dwInfo = 0;
	db_clientdesc->DBPacket(HEADER_GD_GUILD_DISBAND, 0, &gd_guild, sizeof(TPacketGuild));
	building::CManager::instance().ClearLandByGuildID(GetID());
}

void CGuild::AddComment(LPCHARACTER ch, const std::string& str)
{
	if (str.length() > GUILD_COMMENT_MAX_LEN || str.length() == 0)
	{
		return;
	}

	if (m_guildPostCommentPulse > thecore_pulse())
	{
		int deltaInSeconds = ((m_guildPostCommentPulse / PASSES_PER_SEC(1)) - (thecore_pulse() / PASSES_PER_SEC(1)));
		int minutes = deltaInSeconds / 60;
		int seconds = (deltaInSeconds - (minutes * 60));

		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can write another comment in %02d minutes and %02d seconds!"), minutes, seconds);
		return;
	}

	char text[GUILD_COMMENT_MAX_LEN * 2 + 1];
	DBManager::instance().EscapeString(text, sizeof(text), str.c_str(), str.length());

	DBManager::instance().FuncAfterQuery(std::bind(&CGuild::RefreshCommentForce, this, ch->GetPlayerID()),
		"INSERT INTO guild_comment%s(guild_id, name, notice, content, time) VALUES(%u, '%s', %d, '%s', NOW())",
		get_table_postfix(), m_data.guild_id, ch->GetName(), (str[0] == '!') ? 1 : 0, text);

	m_guildPostCommentPulse = thecore_pulse() + PASSES_PER_SEC(5*60);
}

void CGuild::DeleteComment(LPCHARACTER ch, DWORD comment_id)
{
	SQLMsg * pmsg;

	if (GetMember(ch->GetPlayerID())->grade == GUILD_LEADER_GRADE)
		pmsg = DBManager::instance().DirectQuery("DELETE FROM guild_comment%s WHERE id = %u AND guild_id = %u",get_table_postfix(), comment_id, m_data.guild_id);
	else
		pmsg = DBManager::instance().DirectQuery("DELETE FROM guild_comment%s WHERE id = %u AND guild_id = %u AND name = '%s'",get_table_postfix(), comment_id, m_data.guild_id, ch->GetName());

	if (pmsg->Get()->uiAffectedRows == 0 || pmsg->Get()->uiAffectedRows == (uint32_t)-1)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> The message cannot be deleted."));
	else
		RefreshCommentForce(ch->GetPlayerID());

	M2_DELETE(pmsg);
}

void CGuild::RefreshComment(LPCHARACTER ch)
{
	RefreshCommentForce(ch->GetPlayerID());
}

void CGuild::RefreshCommentForce(DWORD player_id)
{
	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(player_id);
	if (ch == NULL) {
		return;
	}

	std::unique_ptr<SQLMsg> pmsg (DBManager::instance().DirectQuery("SELECT id, name, content FROM guild_comment%s WHERE guild_id = %u ORDER BY notice DESC, id DESC LIMIT %d", get_table_postfix(), m_data.guild_id, GUILD_COMMENT_MAX_COUNT));

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack)+1;
	pack.subheader = GUILD_SUBHEADER_GC_COMMENTS;

	WORD count = pmsg->Get()->uiNumRows;

	LPDESC d = ch->GetDesc();

	if (!d) 
		return;

	pack.size += (sizeof(DWORD)+CHARACTER_NAME_MAX_LEN+1+GUILD_COMMENT_MAX_LEN+1)*(WORD)count;
	d->BufferedPacket(&pack,sizeof(pack));
	d->BufferedPacket(&count, 1);
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	char szContent[GUILD_COMMENT_MAX_LEN + 1];
	memset(szName, 0, sizeof(szName));
	memset(szContent, 0, sizeof(szContent));

	for (uint i = 0; i < pmsg->Get()->uiNumRows; i++)
	{
		MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);
		DWORD id = strtoul(row[0], NULL, 10);

		strlcpy(szName, row[1], sizeof(szName));
		strlcpy(szContent, row[2], sizeof(szContent));

		d->BufferedPacket(&id, sizeof(id));
		d->BufferedPacket(szName, sizeof(szName));

		if (i == pmsg->Get()->uiNumRows - 1)
			d->Packet(szContent, sizeof(szContent));
		else
			d->BufferedPacket(szContent, sizeof(szContent));
	}
}

bool CGuild::ChangeMemberGeneral(DWORD pid, BYTE is_general)
{
	if (is_general && GetGeneralCount() >= GetMaxGeneralCount())
		return false;

	auto it = m_member.find(pid);
	if (it == m_member.end())
	{
		return true;
	}

	is_general = is_general?1:0;

	if (it->second.is_general == is_general)
		return true;

	if (is_general)
		++m_general_count;
	else
		--m_general_count;

	it->second.is_general = is_general;

	auto itOnline = m_memberOnline.begin();

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack)+5;
	pack.subheader = GUILD_SUBHEADER_GC_CHANGE_MEMBER_GENERAL;

	while (itOnline != m_memberOnline.end())
	{
		LPDESC d = (*(itOnline++))->GetDesc();

		if (!d)
			continue;

		d->BufferedPacket(&pack, sizeof(pack));
		d->BufferedPacket(&pid, sizeof(pid));
		d->Packet(&is_general, sizeof(is_general));
	}

	SaveMember(pid);
	return true;
}

void CGuild::ChangeMemberGrade(DWORD pid, BYTE grade)
{
	if (grade == 1)
		return;

	auto it = m_member.find(pid);

	if (it == m_member.end())
		return;

	it->second.grade = grade;

	auto itOnline = m_memberOnline.begin();

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack)+5;
	pack.subheader = GUILD_SUBHEADER_GC_CHANGE_MEMBER_GRADE;

	while (itOnline != m_memberOnline.end())
	{
		LPDESC d = (*(itOnline++))->GetDesc();

		if (!d)
			continue;

		d->BufferedPacket(&pack, sizeof(pack));
		d->BufferedPacket(&pid, sizeof(pid));
		d->Packet(&grade, sizeof(grade));
	}

	SaveMember(pid);

	TPacketGuildChangeMemberData gd_guild;

	gd_guild.guild_id = GetID();
	gd_guild.pid = pid;
	gd_guild.offer = it->second.offer_exp;
	gd_guild.level = it->second.level;
	gd_guild.grade = grade;

	db_clientdesc->DBPacket(HEADER_GD_GUILD_CHANGE_MEMBER_DATA, 0, &gd_guild, sizeof(gd_guild));
}

void CGuild::SkillLevelUp(DWORD dwVnum)
{
	DWORD dwRealVnum = dwVnum - GUILD_SKILL_START;

	if (dwRealVnum >= GUILD_SKILL_COUNT)
		return;

	CSkillProto* pkSk = CSkillManager::instance().Get(dwVnum);

	if (!pkSk)
	{
		sys_err("There is no such guild skill by number %u", dwVnum);
		return;
	}

	if (m_data.abySkill[dwRealVnum] >= pkSk->bMaxLevel)
		return;

	if (m_data.skill_point <= 0)
		return;
	m_data.skill_point --;

	m_data.abySkill[dwRealVnum] ++;

	ComputeGuildPoints();
	SaveSkill();
	SendDBSkillUpdate();
	for_each(m_memberOnline.begin(), m_memberOnline.end(), std::bind(&CGuild::SendSkillInfoPacket, this, std::placeholders::_1));

	sys_log(0, "Guild SkillUp: %s %d level %d type %u", GetName(), pkSk->dwVnum, m_data.abySkill[dwRealVnum], pkSk->dwType);
}

void CGuild::UseSkill(DWORD dwVnum, LPCHARACTER ch, DWORD pid)
{
	LPCHARACTER victim = NULL;

	if (!GetMember(ch->GetPlayerID()) || !HasGradeAuth(GetMember(ch->GetPlayerID())->grade, GUILD_AUTH_USE_SKILL))
		return;

	sys_log(0,"GUILD_USE_SKILL : cname(%s), skill(%d)", ch ? ch->GetName() : "", dwVnum);

	DWORD dwRealVnum = dwVnum - GUILD_SKILL_START;

	if (!ch->CanMove())
		return;

	if (dwRealVnum >= GUILD_SKILL_COUNT)
		return;

	CSkillProto* pkSk = CSkillManager::instance().Get(dwVnum);

	if (!pkSk)
	{
		sys_err("There is no such guild skill by number %u", dwVnum);
		return;
	}

	if (m_data.abySkill[dwRealVnum] == 0)
		return;

	if ((pkSk->dwFlag & SKILL_FLAG_SELFONLY))
	{
		if (ch->FindAffect(pkSk->dwVnum))
			return;

		victim = ch;
	}

	if (ch->IsAffectFlag(AFF_REVIVE_INVISIBLE))
		ch->RemoveAffect(AFFECT_REVIVE_INVISIBLE);

	if (ch->IsAffectFlag(AFF_EUNHYUNG))
		ch->RemoveAffect(SKILL_EUNHYUNG);

	double k =1.0*m_data.abySkill[dwRealVnum]/pkSk->bMaxLevel;
	pkSk->kSPCostPoly.SetVar("k", k);
	int iNeededSP = (int) pkSk->kSPCostPoly.Eval();

	if (GetSP() < iNeededSP)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> Not enough Dragon ghost. (%d, %d)"), GetSP(), iNeededSP);
		return;
	}

	pkSk->kCooldownPoly.SetVar("k", k);
	int iCooltime = (int) pkSk->kCooldownPoly.Eval();

	if (!abSkillUsable[dwRealVnum])
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> You cannot use Guild Skills yet."));
		return;
	}

	{
		TPacketGuildUseSkill p;
		p.dwGuild = GetID();
		p.dwSkillVnum = pkSk->dwVnum;
		p.dwCooltime = iCooltime;
		db_clientdesc->DBPacket(HEADER_GD_GUILD_USE_SKILL, 0, &p, sizeof(p));
	}
	abSkillUsable[dwRealVnum] = false;

	if (test_server)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> %d Skills used (%d, %d) to %u"), dwVnum, GetSP(), iNeededSP, pid);

	switch (dwVnum)
	{
		case GUILD_SKILL_TELEPORT:
			SendDBSkillUpdate(-iNeededSP);
			if ((victim = (CHARACTER_MANAGER::instance().FindByPID(pid))))
				ch->WarpSet(victim->GetX(), victim->GetY());
			else
			{
				if (m_memberP2POnline.find(pid) != m_memberP2POnline.end())
				{
					CCI * pcci = P2P_MANAGER::instance().FindByPID(pid);

					if (pcci->bChannel != g_bChannel)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> The player is in channel %d. (Current channel: %d)"), pcci->bChannel, g_bChannel);
					}
					else
					{
						TPacketGGFindPosition p;
						p.header = HEADER_GG_FIND_POSITION;
						p.dwFromPID = ch->GetPlayerID();
						p.dwTargetPID = pid;
						pcci->pkDesc->Packet(&p, sizeof(TPacketGGFindPosition));
						if (test_server) ch->ChatPacket(CHAT_TYPE_PARTY, "sent find position packet for guild teleport");
					}
				}
				else
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> The player is not online."));
			}
			break;

		default:
			{

				if (!UnderAnyWar())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> This Guild Skill can be used in war only."));
					return;
				}

				SendDBSkillUpdate(-iNeededSP);

				for (auto it = m_memberOnline.begin(); it != m_memberOnline.end(); ++it)
				{
					LPCHARACTER victim = *it;
					victim->RemoveAffect(dwVnum);
					ch->ComputeSkill(dwVnum, victim, m_data.abySkill[dwRealVnum]);
				}
			}
			break;
	}
}

void CGuild::SendSkillInfoPacket(LPCHARACTER ch) const
{
	LPDESC d = ch->GetDesc();

	if (!d)
		return;

	TPacketGCGuild pack;

	pack.header		= HEADER_GC_GUILD;
	pack.size		= sizeof(pack) + 6 + GUILD_SKILL_COUNT;
	pack.subheader	= GUILD_SUBHEADER_GC_SKILL_INFO;

	d->BufferedPacket(&pack, sizeof(pack));
	d->BufferedPacket(&m_data.skill_point,	1);
	d->BufferedPacket(&m_data.abySkill,		GUILD_SKILL_COUNT);
	d->BufferedPacket(&m_data.power,		2);
	d->Packet(&m_data.max_power,	2);
}

void CGuild::ComputeGuildPoints()
{
	m_data.max_power = GUILD_BASE_POWER + (m_data.level-1) * GUILD_POWER_PER_LEVEL;

	m_data.power = MINMAX(0, m_data.power, m_data.max_power);
}

int CGuild::GetSkillLevel(DWORD vnum)
{
	DWORD dwRealVnum = vnum - GUILD_SKILL_START;

	if (dwRealVnum >= GUILD_SKILL_COUNT)
		return 0;

	return m_data.abySkill[dwRealVnum];
}

void CGuild::UpdateSkill(BYTE skill_point, BYTE* skill_levels)
{
	m_data.skill_point = skill_point;
	thecore_memcpy(m_data.abySkill, skill_levels, sizeof(BYTE) * GUILD_SKILL_COUNT);
	ComputeGuildPoints();
}

static DWORD __guild_levelup_exp(int level)
{
	return guild_exp_table[level];
}

void CGuild::GuildPointChange(BYTE type, int amount, bool save)
{
	switch (type)
	{
		case POINT_SP:
			m_data.power += amount;

			m_data.power = MINMAX(0, m_data.power, m_data.max_power);

			if (save)
			{
				SaveSkill();
			}

			for_each(m_memberOnline.begin(), m_memberOnline.end(), std::bind(&CGuild::SendSkillInfoPacket, this, std::placeholders::_1));
			break;

		case POINT_EXP:
			if (amount < 0 && m_data.exp < (DWORD) - amount)
			{
				m_data.exp = 0;
			}
			else
			{
				m_data.exp += amount;

				while (m_data.exp >= __guild_levelup_exp(m_data.level))
				{

					if (m_data.level < GUILD_MAX_LEVEL)
					{
						m_data.exp -= __guild_levelup_exp(m_data.level);
						++m_data.level;
						++m_data.skill_point;

						if (m_data.level > GUILD_MAX_LEVEL)
							m_data.level = GUILD_MAX_LEVEL;

						ComputeGuildPoints();
						GuildPointChange(POINT_SP, m_data.max_power-m_data.power);

						if (save)
							ChangeLadderPoint(GUILD_LADDER_POINT_PER_LEVEL);

						for_each(m_memberOnline.begin(), m_memberOnline.end(), std::bind(&CGuild::SendGuildInfoPacket, this, std::placeholders::_1));
					}

					if (m_data.level == GUILD_MAX_LEVEL)
					{
						m_data.exp = 0;
					}
				}
			}

			TPacketGCGuild pack;
			pack.header = HEADER_GC_GUILD;
			pack.size = sizeof(pack)+5;
			pack.subheader = GUILD_SUBHEADER_GC_CHANGE_EXP;

			TEMP_BUFFER buf;
			buf.write(&pack,sizeof(pack));
			buf.write(&m_data.level,1);
			buf.write(&m_data.exp,4);

			for (auto it = m_memberOnline.begin(); it != m_memberOnline.end(); ++it)
			{
				LPDESC d = (*it)->GetDesc();

				if (d)
					d->Packet(buf.read_peek(), buf.size());
			}

			if (save)
				SaveLevel();

			break;
	}
}

void CGuild::SkillRecharge()
{
}

void CGuild::SaveMember(DWORD pid)
{
	auto it = m_member.find(pid);

	if (it == m_member.end())
		return;

	DBManager::instance().Query(
			"UPDATE guild_member%s SET grade = %d, offer = %u, is_general = %d WHERE pid = %u and guild_id = %u",
			get_table_postfix(), it->second.grade, it->second.offer_exp, it->second.is_general, pid, m_data.guild_id);
}

void CGuild::LevelChange(DWORD pid, BYTE level)
{
	auto cit = m_member.find(pid);

	if (cit == m_member.end())
		return;

	cit->second.level = level;

	TPacketGuildChangeMemberData gd_guild;

	gd_guild.guild_id = GetID();
	gd_guild.pid = pid;
	gd_guild.offer = cit->second.offer_exp;
	gd_guild.grade = cit->second.grade;
	gd_guild.level = level;

	db_clientdesc->DBPacket(HEADER_GD_GUILD_CHANGE_MEMBER_DATA, 0, &gd_guild, sizeof(gd_guild));

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	cit->second._dummy = 0;

	for (auto it = m_memberOnline.begin(); it != m_memberOnline.end(); ++it)
	{
		LPDESC d = (*it)->GetDesc();

		if (d)
		{
			pack.subheader = GUILD_SUBHEADER_GC_LIST;
			pack.size = sizeof(pack) + 13;
			d->BufferedPacket(&pack, sizeof(pack));
			d->Packet(&(cit->second), sizeof(DWORD) * 3 + 1);
		}
	}
}

void CGuild::ChangeMemberData(DWORD pid, DWORD offer, BYTE level, BYTE grade)
{
	auto cit = m_member.find(pid);

	if (cit == m_member.end())
		return;

	cit->second.offer_exp = offer;
	cit->second.level = level;
	cit->second.grade = grade;
	cit->second._dummy = 0;

	TPacketGCGuild pack;
	memset(&pack, 0, sizeof(pack));
	pack.header = HEADER_GC_GUILD;

	for (auto it = m_memberOnline.begin(); it != m_memberOnline.end(); ++it)
	{
		LPDESC d = (*it)->GetDesc();
		if (d)
		{
			pack.subheader = GUILD_SUBHEADER_GC_LIST;
			pack.size = sizeof(pack) + 13;
			d->BufferedPacket(&pack, sizeof(pack));
			d->Packet(&(cit->second), sizeof(DWORD) * 3 + 1);
		}
	}
}

namespace
{
	struct FGuildChat
	{
		const char* c_pszText;

		FGuildChat(const char* c_pszText)
			: c_pszText(c_pszText)
			{}

		void operator()(LPCHARACTER ch)
		{
			ch->ChatPacket(CHAT_TYPE_GUILD, "%s", c_pszText);
		}
	};
}

void CGuild::P2PChat(const char* c_pszText)
{
	std::for_each(m_memberOnline.begin(), m_memberOnline.end(), FGuildChat(c_pszText));
}

void CGuild::Chat(const char* c_pszText)
{
	std::for_each(m_memberOnline.begin(), m_memberOnline.end(), FGuildChat(c_pszText));

	TPacketGGGuild p1;
	TPacketGGGuildChat p2;

	p1.bHeader = HEADER_GG_GUILD;
	p1.bSubHeader = GUILD_SUBHEADER_GG_CHAT;
	p1.dwGuild = GetID();
	strlcpy(p2.szText, c_pszText, sizeof(p2.szText));

	P2P_MANAGER::instance().Send(&p1, sizeof(TPacketGGGuild));
	P2P_MANAGER::instance().Send(&p2, sizeof(TPacketGGGuildChat));
}

LPCHARACTER CGuild::GetMasterCharacter()
{ 
	return CHARACTER_MANAGER::instance().FindByPID(GetMasterPID()); 
}

void CGuild::Packet(const void* buf, int size)
{
	for (auto it = m_memberOnline.begin(); it!=m_memberOnline.end();++it)
	{
		LPDESC d = (*it)->GetDesc();

		if (d)
			d->Packet(buf, size);
	}
}

int CGuild::GetTotalLevel() const
{
	int total = 0;

	for (auto it = m_member.begin(); it != m_member.end(); ++it)
	{
		total += it->second.level;
	}

	return total;
}

bool CGuild::ChargeSP(LPCHARACTER ch, int iSP)
{
	long long gold = iSP * 100;

	if (gold < iSP || ch->GetGold() < gold)
		return false;

	int iRemainSP = m_data.max_power - m_data.power;

	if (iSP > iRemainSP)
	{
		iSP = iRemainSP;
		gold = iSP * 100;
	}

	ch->PointChange(POINT_GOLD, -gold);

	SendDBSkillUpdate(iSP);
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> %u Dragon ghost restored."), iSP);
	}
	return true;
}

void CGuild::SkillUsableChange(DWORD dwSkillVnum, bool bUsable)
{
	DWORD dwRealVnum = dwSkillVnum - GUILD_SKILL_START;

	if (dwRealVnum >= GUILD_SKILL_COUNT)
		return; 

	abSkillUsable[dwRealVnum] = bUsable;
	sys_log(0, "CGuild::SkillUsableChange(guild=%s, skill=%d, usable=%d)", GetName(), dwSkillVnum, bUsable);
}

void CGuild::SetMemberCountBonus(int iBonus)
{
	m_iMemberCountBonus = iBonus;
	sys_log(0, "GUILD_IS_FULL_BUG : Bonus set to %d(val:%d)", iBonus, m_iMemberCountBonus);
}

void CGuild::BroadcastMemberCountBonus()
{
	TPacketGGGuild p1;

	p1.bHeader = HEADER_GG_GUILD;
	p1.bSubHeader = GUILD_SUBHEADER_GG_SET_MEMBER_COUNT_BONUS;
	p1.dwGuild = GetID();

	P2P_MANAGER::instance().Send(&p1, sizeof(TPacketGGGuild));
	P2P_MANAGER::instance().Send(&m_iMemberCountBonus, sizeof(int));
}

int CGuild::GetMaxMemberCount()
{
	if ( m_iMemberCountBonus < 0 || m_iMemberCountBonus > 18 )
		m_iMemberCountBonus = 0;

	return 32 + 2 * (m_data.level-1) + m_iMemberCountBonus;
}

void CGuild::AdvanceLevel(int iLevel)
{
	if (m_data.level == iLevel)
		return;

	m_data.level = MIN(GUILD_MAX_LEVEL, iLevel);
}

void CGuild::RequestDepositMoney(LPCHARACTER ch, long long lldGold)
{
	if (false==ch->CanDeposit())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> Please use after a while."));
		return;
	}

	if (ch->GetGold() < lldGold)
		return;


	ch->PointChange(POINT_GOLD, -lldGold);

	TPacketGDGuildMoney p;
	p.dwGuild = GetID();
	p.lldGold = lldGold;
	db_clientdesc->DBPacket(HEADER_GD_GUILD_DEPOSIT_MONEY, 0, &p, sizeof(p));

	ch->UpdateDepositPulse();
	sys_log(0, "GUILD: DEPOSIT %s:%u player %s[%u] gold %lld", GetName(), GetID(), ch->GetName(), ch->GetPlayerID(), lldGold);
}

void CGuild::RequestWithdrawMoney(LPCHARACTER ch, long long lldGold)
{
	if (false==ch->CanDeposit())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> Please use after a while."));
		return;
	}

	if (ch->GetPlayerID() != GetMasterPID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> Only the Guild master can take out Yang."));
		return;
	}

	if (m_data.gold < lldGold)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> You do not have enough Yang."));
		return;
	}

	TPacketGDGuildMoney p;
	p.dwGuild = GetID();
	p.lldGold = lldGold;
	db_clientdesc->DBPacket(HEADER_GD_GUILD_WITHDRAW_MONEY, 0, &p, sizeof(p));

	ch->UpdateDepositPulse();
}

void CGuild::RecvMoneyChange(long long lldGold)
{
	m_data.gold = lldGold;

	TPacketGCGuild p;
	p.header = HEADER_GC_GUILD;
	p.size = sizeof(p) + sizeof(int);
	p.subheader = GUILD_SUBHEADER_GC_MONEY_CHANGE;

	for (auto it = m_memberOnline.begin(); it != m_memberOnline.end(); ++it)
	{
		LPCHARACTER ch = *it;
		LPDESC d = ch->GetDesc();
		d->BufferedPacket(&p, sizeof(p));
		d->Packet(&lldGold, sizeof(long long));
	}
}

void CGuild::RecvWithdrawMoneyGive(long long lldChangeGold)
{
	LPCHARACTER ch = GetMasterCharacter();

	if (ch)
	{
		ch->PointChange(POINT_GOLD, lldChangeGold);
		sys_log(0, "GUILD: WITHDRAW %s:%u player %s[%u] gold %d", GetName(), GetID(), ch->GetName(), ch->GetPlayerID(), lldChangeGold);
	}

	TPacketGDGuildMoneyWithdrawGiveReply p;
	p.dwGuild = GetID();
	p.lldChangeGold = lldChangeGold;
	p.bGiveSuccess = ch ? 1 : 0;
	db_clientdesc->DBPacket(HEADER_GD_GUILD_WITHDRAW_MONEY_GIVE_REPLY, 0, &p, sizeof(p));
}

bool CGuild::HasLand()
{
	return building::CManager::instance().FindLandByGuild(GetID()) != NULL;
}


EVENTINFO(TInviteGuildEventInfo)
{
	DWORD	dwInviteePID;
	DWORD	dwGuildID;

	TInviteGuildEventInfo()
	: dwInviteePID( 0 )
	, dwGuildID( 0 )
	{
	}
};

EVENTFUNC( GuildInviteEvent )
{
	TInviteGuildEventInfo *pInfo = dynamic_cast<TInviteGuildEventInfo*>( event->info );

	if ( pInfo == NULL )
	{
		sys_err( "GuildInviteEvent> <Factor> Null pointer" );
		return 0;
	}

	CGuild* pGuild = CGuildManager::instance().FindGuild( pInfo->dwGuildID );

	if ( pGuild ) 
	{
		sys_log( 0, "GuildInviteEvent %s", pGuild->GetName() );
		pGuild->InviteDeny( pInfo->dwInviteePID );
	}

	return 0;
}

void CGuild::Invite( LPCHARACTER pchInviter, LPCHARACTER pchInvitee )
{
	if (quest::CQuestManager::instance().GetPCForce(pchInviter->GetPlayerID())->IsRunning() == true)
	{
	    return;
	}

	
	if (quest::CQuestManager::instance().GetPCForce(pchInvitee->GetPlayerID())->IsRunning() == true)
		return;

	if ( pchInvitee->IsBlockMode( BLOCK_GUILD_INVITE ) ) 
	{
		pchInviter->ChatPacket( CHAT_TYPE_INFO, LC_TEXT("<Guild> The player declines the Guild invitation.") );
		return;
	} 
	else if ( !HasGradeAuth( GetMember( pchInviter->GetPlayerID() )->grade, GUILD_AUTH_ADD_MEMBER ) ) 
	{
		pchInviter->ChatPacket( CHAT_TYPE_INFO, LC_TEXT("<Guild> You cannot invite someone into the Guild.") );
		return;
	} 
	else if ( pchInvitee->GetEmpire() != pchInviter->GetEmpire() ) 
	{
		pchInviter->ChatPacket( CHAT_TYPE_INFO, LC_TEXT("<Guild> You cannot invite players from another Kingdom into the Guild.") );
		return;
	}

	GuildJoinErrCode errcode = VerifyGuildJoinableCondition( pchInvitee );
	switch ( errcode ) 
	{
		case GERR_NONE: break;
		case GERR_WITHDRAWPENALTY:
						pchInviter->ChatPacket( CHAT_TYPE_INFO, 
								LC_TEXT("<Guild> The player can be invited again after %d days."), 
								quest::CQuestManager::instance().GetEventFlag( "guild_withdraw_delay" ) );
						return;
		case GERR_COMMISSIONPENALTY:
						pchInviter->ChatPacket( CHAT_TYPE_INFO, 
								LC_TEXT("<Guild> After the rearrangement you can invite members again after %d days."), 
								quest::CQuestManager::instance().GetEventFlag( "guild_disband_delay") );
						return;
		case GERR_ALREADYJOIN:	pchInviter->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> This person is already a member of another Guild.")); return;
		case GERR_GUILDISFULL:	pchInviter->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> The Guild capacity reached its upper limit.")); return;
		case GERR_GUILD_IS_IN_WAR : pchInviter->ChatPacket( CHAT_TYPE_INFO, LC_TEXT("<Guild> The Guild is at war.") ); return;
		case GERR_INVITE_LIMIT : pchInviter->ChatPacket( CHAT_TYPE_INFO, LC_TEXT("<Guild> You can't invite more people.") ); return;

		default: sys_err( "ignore guild join error(%d)", errcode ); return;
	}

	if ( m_GuildInviteEventMap.end() != m_GuildInviteEventMap.find( pchInvitee->GetPlayerID() ) )
		return;

	TInviteGuildEventInfo* pInfo = AllocEventInfo<TInviteGuildEventInfo>();
	pInfo->dwInviteePID = pchInvitee->GetPlayerID();
	pInfo->dwGuildID = GetID();

	m_GuildInviteEventMap.insert(EventMap::value_type(pchInvitee->GetPlayerID(), event_create(GuildInviteEvent, pInfo, PASSES_PER_SEC(10))));
	DWORD gid = GetID();

	TPacketGCGuild p;
	p.header	= HEADER_GC_GUILD;
	p.size	= sizeof(p) + sizeof(DWORD) + GUILD_NAME_MAX_LEN + 1;
	p.subheader	= GUILD_SUBHEADER_GC_GUILD_INVITE;

	TEMP_BUFFER buf;
	buf.write( &p, sizeof(p) );
	buf.write( &gid, sizeof(DWORD) );
	buf.write( GetName(), GUILD_NAME_MAX_LEN + 1 );

	pchInvitee->GetDesc()->Packet( buf.read_peek(), buf.size() );
}

void CGuild::InviteAccept( LPCHARACTER pchInvitee )
{
	auto itFind = m_GuildInviteEventMap.find( pchInvitee->GetPlayerID() );
	if ( itFind == m_GuildInviteEventMap.end() ) 
	{
		sys_log( 0, "GuildInviteAccept from not invited character(invite guild: %s, invitee: %s)", GetName(), pchInvitee->GetName() );
		return;
	}

	event_cancel( &itFind->second );
	m_GuildInviteEventMap.erase( itFind );

	GuildJoinErrCode errcode = VerifyGuildJoinableCondition( pchInvitee );
	switch ( errcode ) 
	{
		case GERR_NONE: break;
		case GERR_WITHDRAWPENALTY:
						pchInvitee->ChatPacket( CHAT_TYPE_INFO, 
								LC_TEXT("<Guild> The player can be invited again after %d days."), 
								quest::CQuestManager::instance().GetEventFlag( "guild_withdraw_delay" ) );
						return;
		case GERR_COMMISSIONPENALTY:
						pchInvitee->ChatPacket( CHAT_TYPE_INFO, 
								LC_TEXT("<Guild> After the rearrangement you can invite members again after %d days."), 
								quest::CQuestManager::instance().GetEventFlag( "guild_disband_delay") );
						return;
		case GERR_ALREADYJOIN:	pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> This person is already a member of another Guild.")); return;
		case GERR_GUILDISFULL:	pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> The Guild capacity reached its upper limit.")); return;
		case GERR_GUILD_IS_IN_WAR : pchInvitee->ChatPacket( CHAT_TYPE_INFO, LC_TEXT("<Guild> The Guild is at war.") ); return;
		case GERR_INVITE_LIMIT : pchInvitee->ChatPacket( CHAT_TYPE_INFO, LC_TEXT("<Guild> You can't invite more people.") ); return;

		default: sys_err( "ignore guild join error(%d)", errcode ); return;
	}

	RequestAddMember( pchInvitee, 15 );
}

void CGuild::InviteDeny( DWORD dwPID )
{
	auto itFind = m_GuildInviteEventMap.find( dwPID );
	if ( itFind == m_GuildInviteEventMap.end() ) 
	{
		sys_log( 0, "GuildInviteDeny from not invited character(invite guild: %s, invitee PID: %d)", GetName(), dwPID );
		return;
	}

	event_cancel( &itFind->second );
	m_GuildInviteEventMap.erase( itFind );
}

CGuild::GuildJoinErrCode CGuild::VerifyGuildJoinableCondition( const LPCHARACTER pchInvitee )
{
	if ( get_global_time() - pchInvitee->GetQuestFlag( "guild_manage.new_withdraw_time" )
			< CGuildManager::instance().GetWithdrawDelay() )
		return GERR_WITHDRAWPENALTY;
	else if ( get_global_time() - pchInvitee->GetQuestFlag( "guild_manage.new_disband_time" )
			< CGuildManager::instance().GetDisbandDelay() )
		return GERR_COMMISSIONPENALTY;
	else if ( pchInvitee->GetGuild() )
		return GERR_ALREADYJOIN;
	else if ( GetMemberCount() >= GetMaxMemberCount() )
	{
		sys_log(1, "GuildName = %s, GetMemberCount() = %d, GetMaxMemberCount() = %d (32 + MAX(level(%d)-10, 0) * 2 + bonus(%d)", 
				GetName(), GetMemberCount(), GetMaxMemberCount(), m_data.level, m_iMemberCountBonus);
		return GERR_GUILDISFULL;
	}
	else if ( UnderAnyWar() != 0 )
	{
		return GERR_GUILD_IS_IN_WAR;
	}

	return GERR_NONE;
}

bool CGuild::ChangeMasterTo(DWORD dwPID)
{
	if ( GetMember(dwPID) == NULL ) return false;

	TPacketChangeGuildMaster p;
	p.dwGuildID = GetID();
	p.idFrom = GetMasterPID();
	p.idTo = dwPID;

	db_clientdesc->DBPacket(HEADER_GD_REQ_CHANGE_GUILD_MASTER, 0, &p, sizeof(p));

	return true;
}

void CGuild::SendGuildDataUpdateToAllMember(SQLMsg* pmsg)
{
	auto iter = m_memberOnline.begin();

	for (; iter != m_memberOnline.end(); iter++ )
	{
		SendGuildInfoPacket(*iter);
		SendAllGradePacket(*iter);
	}
}

