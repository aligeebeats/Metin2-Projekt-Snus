#include "StdAfx.h"
#include "PythonNetworkStream.h"
#include "NetworkActorManager.h"
#include "PythonBackground.h"
#include "PythonApplication.h"
#include "AbstractPlayer.h"
#include "../gamelib/ActorInstance.h"

void CPythonNetworkStream::__GlobalPositionToLocalPosition(LONG& rGlobalX, LONG& rGlobalY)
{
	CPythonBackground&rkBgMgr=CPythonBackground::Instance();
	rkBgMgr.GlobalPositionToLocalPosition(rGlobalX, rGlobalY);
}

void CPythonNetworkStream::__LocalPositionToGlobalPosition(LONG& rLocalX, LONG& rLocalY)
{
	CPythonBackground&rkBgMgr=CPythonBackground::Instance();
	rkBgMgr.LocalPositionToGlobalPosition(rLocalX, rLocalY);
}

bool CPythonNetworkStream::__CanActMainInstance()
{
	CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
	CInstanceBase* pkInstMain=rkChrMgr.GetMainInstancePtr();
	if (!pkInstMain)
		return false;

	return pkInstMain->CanAct();
}

void CPythonNetworkStream::__ClearNetworkActorManager()
{
	m_rokNetActorMgr->Destroy();
}

void __SetWeaponPower(IAbstractPlayer& rkPlayer)
{
	DWORD dwWeaponID = rkPlayer.GetItemIndex(TItemPos(INVENTORY, c_Equipment_Weapon));
	DWORD minPower=0;
	DWORD maxPower=0;
	DWORD minMagicPower=0;
	DWORD maxMagicPower=0;
	DWORD addPower=0;

	CItemData* pkWeapon;
	if (CItemManager::Instance().GetItemDataPointer(CPythonPlayer::Instance().GetItemIndex(TItemPos(INVENTORY, c_Equipment_Weapon)), &pkWeapon))
	{
		if (pkWeapon->GetType()==CItemData::ITEM_TYPE_WEAPON)
		{
			minPower=pkWeapon->GetValue(3);
			maxPower=pkWeapon->GetValue(4);
			minMagicPower=pkWeapon->GetValue(1);
			maxMagicPower=pkWeapon->GetValue(2);
			addPower=pkWeapon->GetValue(5);
		}
	}

	const TItemData* acce = CPythonPlayer::instance().GetItemData(TItemPos(INVENTORY, c_Costume_Slot_Acce));
	if (acce)
	{
		BYTE drainPct = acce->alSockets[0];
		DWORD itemInAcce = acce->alSockets[1];

		if (drainPct > 0 && itemInAcce > 0)
		{
			CItemData* pkItemInAcce;
			if (CItemManager::Instance().GetItemDataPointer(itemInAcce, &pkItemInAcce) && pkItemInAcce->GetType() == CItemData::ITEM_TYPE_WEAPON)
			{
				minPower += (int)(float(pkItemInAcce->GetValue(3)) / 100.0f * float(drainPct));
				maxPower += (int)(float(pkItemInAcce->GetValue(4)) / 100.0f * float(drainPct));
				minMagicPower += (int)(float(pkItemInAcce->GetValue(1)) / 100.0f * float(drainPct));
				maxMagicPower += (int)(float(pkItemInAcce->GetValue(2)) / 100.0f * float(drainPct));
				addPower += (int)(float(pkItemInAcce->GetValue(5)) / 100.0f * float(drainPct));
			}
		}
	}

	rkPlayer.SetWeaponPower(minPower, maxPower, minMagicPower, maxMagicPower, addPower);
}

bool IsInvisibleRace(WORD raceNum)
{
	switch(raceNum)
	{
	case 20025:
	case 20038:
	case 20039:
		return true;
	default:
		return false;
	}
}

static SNetworkActorData s_kNetActorData;


bool CPythonNetworkStream::RecvCharacterAppendPacket()
{
	TPacketGCCharacterAdd chrAddPacket;
	if (!Recv(sizeof(chrAddPacket), &chrAddPacket))
		return false;

	__GlobalPositionToLocalPosition(chrAddPacket.x, chrAddPacket.y);

	SNetworkActorData kNetActorData;
	kNetActorData.m_bType=chrAddPacket.bType;
	kNetActorData.m_dwMovSpd=chrAddPacket.wMovingSpeed;
	kNetActorData.m_dwAtkSpd=chrAddPacket.bAttackSpeed;
	kNetActorData.m_dwRace=chrAddPacket.wRaceNum;
	
	kNetActorData.m_dwStateFlags=chrAddPacket.bStateFlag;
	kNetActorData.m_dwVID=chrAddPacket.dwVID;
	kNetActorData.m_fRot=chrAddPacket.angle;

	kNetActorData.m_stName="";

	kNetActorData.m_stName="";
	kNetActorData.m_kAffectFlags.CopyData(0, sizeof(chrAddPacket.dwAffectFlag[0]), &chrAddPacket.dwAffectFlag[0]);
	kNetActorData.m_kAffectFlags.CopyData(32, sizeof(chrAddPacket.dwAffectFlag[1]), &chrAddPacket.dwAffectFlag[1]);
	
	kNetActorData.SetPosition(chrAddPacket.x, chrAddPacket.y);

	kNetActorData.m_sAlignment=0;
	kNetActorData.m_byPKMode=0;	
	kNetActorData.m_dwGuildID=0;
	kNetActorData.m_dwEmpireID=0;
	kNetActorData.m_dwArmor=0;
	kNetActorData.m_dwWeapon=0;
	kNetActorData.m_dwHair=0;	
	kNetActorData.m_dwAcce = 0;
	kNetActorData.m_byAcceDrainRate = 0;
	kNetActorData.m_dwMountVnum=0;
	kNetActorData.m_dwArrow = 0;

	kNetActorData.m_dwLevel = 0;

	if(kNetActorData.m_bType != CActorInstance::TYPE_PC &&
		kNetActorData.m_bType != CActorInstance::TYPE_NPC
		&& kNetActorData.m_bType != CActorInstance::TYPE_MOUNT
		&& kNetActorData.m_bType != CActorInstance::TYPE_PET)
	{
		const char * c_szName;
		CPythonNonPlayer& rkNonPlayer=CPythonNonPlayer::Instance();
		if (rkNonPlayer.GetName(kNetActorData.m_dwRace, &c_szName))
			kNetActorData.m_stName = c_szName;

		__RecvCharacterAppendPacket(&kNetActorData);
	}
	else
	{
		s_kNetActorData = kNetActorData;
	}

	return true;
}

bool CPythonNetworkStream::RecvCharacterAdditionalInfo()
{
	TPacketGCCharacterAdditionalInfo chrInfoPacket;
	if (!Recv(sizeof(chrInfoPacket), &chrInfoPacket))
		return false;

	
	SNetworkActorData kNetActorData = s_kNetActorData;
	if (IsInvisibleRace(kNetActorData.m_dwRace))
		return true;

	if(kNetActorData.m_dwVID == chrInfoPacket.dwVID)
	{
		const char* c_szNpcClientName;
		if ((kNetActorData.m_bType == CActorInstance::TYPE_NPC && kNetActorData.m_dwRace != 30000) || kNetActorData.m_bType == CActorInstance::TYPE_PET || kNetActorData.m_bType == CActorInstance::TYPE_MOUNT)
		{
			if (kNetActorData.m_dwRace > 34000 && kNetActorData.m_dwRace < 35000 || kNetActorData.m_dwRace > 20100 && kNetActorData.m_dwRace < 20125 || kNetActorData.m_dwRace > 20201 && kNetActorData.m_dwRace < 20270 || kNetActorData.m_dwRace == 10 || kNetActorData.m_dwRace == 11)
			{
				char szPetName[CHARACTER_NAME_MAX_LEN];
				if(kNetActorData.m_dwRace == 10 || kNetActorData.m_dwRace == 11)
					sprintf(szPetName, "%s", chrInfoPacket.name);
				else if (CPythonNonPlayer::Instance().GetName(kNetActorData.m_dwRace, &c_szNpcClientName))
					sprintf(szPetName, "%s - %s", chrInfoPacket.name, c_szNpcClientName);
				else
					sprintf(szPetName, "%s", chrInfoPacket.name);

				kNetActorData.m_stName = szPetName;
			}
			else
			{
				if (CPythonNonPlayer::Instance().GetName(kNetActorData.m_dwRace, &c_szNpcClientName))
					kNetActorData.m_stName = c_szNpcClientName;
				else
					kNetActorData.m_stName = chrInfoPacket.name;
			}
		}
		else
		{
			kNetActorData.m_stName = chrInfoPacket.name;
		}
		kNetActorData.m_dwGuildID = chrInfoPacket.dwGuildID;
		kNetActorData.m_dwLevel = chrInfoPacket.dwLevel;
		kNetActorData.m_sAlignment=chrInfoPacket.sAlignment;	
		kNetActorData.m_byPKMode=chrInfoPacket.bPKMode;	
		kNetActorData.m_dwGuildID=chrInfoPacket.dwGuildID;
		kNetActorData.m_dwEmpireID=chrInfoPacket.bEmpire;
		kNetActorData.m_dwArmor=chrInfoPacket.dwPart[CHR_EQUIPPART_ARMOR];
		kNetActorData.m_dwWeapon=chrInfoPacket.dwPart[CHR_EQUIPPART_WEAPON];
		kNetActorData.m_dwHair=chrInfoPacket.dwPart[CHR_EQUIPPART_HAIR];	
		kNetActorData.m_dwAcce = chrInfoPacket.dwPart[CHR_EQUIPPART_ACCE];
		kNetActorData.m_byAcceDrainRate = chrInfoPacket.byAcceDrainRate;
		kNetActorData.m_dwMountVnum=chrInfoPacket.dwMountVnum;
		kNetActorData.m_dwArrow = chrInfoPacket.dwArrow;
		__RecvCharacterAppendPacket(&kNetActorData);
	}
	else
	{
		TraceError("TPacketGCCharacterAdditionalInfo name=%s vid=%d race=%d Error",chrInfoPacket.name,chrInfoPacket.dwVID,kNetActorData.m_dwRace);
	}
	return true;
}

bool CPythonNetworkStream::RecvCharacterAppendPacketNew()
{
	TraceError("TPacketGCCharacterAdd2는 쓰지 않는 패킷입니다.");
	TPacketGCCharacterAdd2 chrAddPacket;
	if (!Recv(sizeof(chrAddPacket), &chrAddPacket))
		return false;
	if(IsInvisibleRace(chrAddPacket.wRaceNum))
		return true;

	__GlobalPositionToLocalPosition(chrAddPacket.x, chrAddPacket.y);

	SNetworkActorData kNetActorData;
	kNetActorData.m_dwLevel = 0;
	kNetActorData.m_bType=chrAddPacket.bType;
	kNetActorData.m_dwGuildID=chrAddPacket.dwGuild;
	kNetActorData.m_dwEmpireID=chrAddPacket.bEmpire;	
	kNetActorData.m_dwMovSpd=chrAddPacket.wMovingSpeed;
	kNetActorData.m_dwAtkSpd=chrAddPacket.bAttackSpeed;
	kNetActorData.m_dwRace=chrAddPacket.wRaceNum;
	kNetActorData.m_dwArmor=chrAddPacket.dwPart[CHR_EQUIPPART_ARMOR];
	kNetActorData.m_dwWeapon=chrAddPacket.dwPart[CHR_EQUIPPART_WEAPON];
	kNetActorData.m_dwHair=chrAddPacket.dwPart[CHR_EQUIPPART_HAIR];
	kNetActorData.m_dwAcce = chrAddPacket.dwPart[CHR_EQUIPPART_ACCE];
	kNetActorData.m_byAcceDrainRate = chrAddPacket.byAcceDrainRate;
	kNetActorData.m_dwStateFlags=chrAddPacket.bStateFlag;
	kNetActorData.m_dwVID=chrAddPacket.dwVID;
	kNetActorData.m_dwMountVnum=chrAddPacket.dwMountVnum;
	kNetActorData.m_dwArrow = chrAddPacket.dwArrow;
	kNetActorData.m_fRot=chrAddPacket.angle;
	kNetActorData.m_kAffectFlags.CopyData(0, sizeof(chrAddPacket.dwAffectFlag[0]), &chrAddPacket.dwAffectFlag[0]);
	kNetActorData.m_kAffectFlags.CopyData(32, sizeof(chrAddPacket.dwAffectFlag[1]), &chrAddPacket.dwAffectFlag[1]);
	kNetActorData.SetPosition(chrAddPacket.x, chrAddPacket.y);
	kNetActorData.m_sAlignment=chrAddPacket.sAlignment;
	kNetActorData.m_byPKMode=chrAddPacket.bPKMode;
	kNetActorData.m_stName=chrAddPacket.name;
	__RecvCharacterAppendPacket(&kNetActorData);

	return true;
}

bool CPythonNetworkStream::RecvCharacterUpdatePacket()
{
	TPacketGCCharacterUpdate chrUpdatePacket;
	if (!Recv(sizeof(chrUpdatePacket), &chrUpdatePacket))
		return false;

	SNetworkUpdateActorData kNetUpdateActorData;
	kNetUpdateActorData.m_dwGuildID=chrUpdatePacket.dwGuildID;
	kNetUpdateActorData.m_dwMovSpd=chrUpdatePacket.wMovingSpeed;
	kNetUpdateActorData.m_dwAtkSpd=chrUpdatePacket.bAttackSpeed;
	kNetUpdateActorData.m_dwArmor=chrUpdatePacket.dwPart[CHR_EQUIPPART_ARMOR];
	kNetUpdateActorData.m_dwWeapon=chrUpdatePacket.dwPart[CHR_EQUIPPART_WEAPON];
	kNetUpdateActorData.m_dwHair=chrUpdatePacket.dwPart[CHR_EQUIPPART_HAIR];
	kNetUpdateActorData.m_dwAcce = chrUpdatePacket.dwPart[CHR_EQUIPPART_ACCE];
	kNetUpdateActorData.m_byAcceDrainRate = chrUpdatePacket.byAcceDrainRate;
	kNetUpdateActorData.m_dwVID=chrUpdatePacket.dwVID;	
	kNetUpdateActorData.m_kAffectFlags.CopyData(0, sizeof(chrUpdatePacket.dwAffectFlag[0]), &chrUpdatePacket.dwAffectFlag[0]);
	kNetUpdateActorData.m_kAffectFlags.CopyData(32, sizeof(chrUpdatePacket.dwAffectFlag[1]), &chrUpdatePacket.dwAffectFlag[1]);
	kNetUpdateActorData.m_sAlignment=chrUpdatePacket.sAlignment;
	kNetUpdateActorData.m_byPKMode=chrUpdatePacket.bPKMode;
	kNetUpdateActorData.m_dwStateFlags=chrUpdatePacket.bStateFlag;
	kNetUpdateActorData.m_dwMountVnum=chrUpdatePacket.dwMountVnum;
	kNetUpdateActorData.m_dwArrow = chrUpdatePacket.dwArrow;
	__RecvCharacterUpdatePacket(&kNetUpdateActorData);

	return true;
}

void CPythonNetworkStream::__RecvCharacterAppendPacket(SNetworkActorData * pkNetActorData)
{
	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	if (rkPlayer.IsMainCharacterIndex(pkNetActorData->m_dwVID))
	{
		rkPlayer.SetRace(pkNetActorData->m_dwRace);

		__SetWeaponPower(rkPlayer);

		if (rkPlayer.NEW_GetMainActorPtr())
		{
			CPythonBackground::Instance().Update(pkNetActorData->m_lCurX, pkNetActorData->m_lCurY, 0.0f);
			CPythonCharacterManager::Instance().Update();

			{
				std::string strMapName = CPythonBackground::Instance().GetWarpMapName();
				if (strMapName == "metin2_map_deviltower1")
					__ShowMapName(pkNetActorData->m_lCurX, pkNetActorData->m_lCurY);
			}
		}
		else
		{
			__ShowMapName(pkNetActorData->m_lCurX, pkNetActorData->m_lCurY);
		}
	}

	m_rokNetActorMgr->AppendActor(*pkNetActorData);

	if (GetMainActorVID()==pkNetActorData->m_dwVID)
	{
		rkPlayer.SetTarget(0);
		if (m_bComboSkillFlag)
			rkPlayer.SetComboSkillFlag(m_bComboSkillFlag);

		__SetGuildID(pkNetActorData->m_dwGuildID);
	}
}

void CPythonNetworkStream::__RecvCharacterUpdatePacket(SNetworkUpdateActorData * pkNetUpdateActorData)
{
	m_rokNetActorMgr->UpdateActor(*pkNetUpdateActorData);

	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	if (rkPlayer.IsMainCharacterIndex(pkNetUpdateActorData->m_dwVID))
	{
		__SetGuildID(pkNetUpdateActorData->m_dwGuildID);
		__SetWeaponPower(rkPlayer);

		__RefreshStatus();
		__RefreshAlignmentWindow();
		__RefreshEquipmentWindow();
		__RefreshInventoryWindow();
	}
	else
	{
		rkPlayer.NotifyCharacterUpdate(pkNetUpdateActorData->m_dwVID);
	}
}

bool CPythonNetworkStream::RecvCharacterDeletePacket()
{
	TPacketGCCharacterDelete chrDelPacket;

	if (!Recv(sizeof(chrDelPacket), &chrDelPacket))
	{
		TraceError("CPythonNetworkStream::RecvCharacterDeletePacket - Recv Error");
		return false;
	}

	m_rokNetActorMgr->RemoveActor(chrDelPacket.dwVID);

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], 
		"BINARY_PrivateShop_Disappear", 
		Py_BuildValue("(i)", chrDelPacket.dwVID)
	);

	return true;
}


bool CPythonNetworkStream::RecvCharacterMovePacket()
{
	TPacketGCMove kMovePacket;
	if (!Recv(sizeof(TPacketGCMove), &kMovePacket))
	{
		Tracen("CPythonNetworkStream::RecvCharacterMovePacket - PACKET READ ERROR");
		return false;
	}

	__GlobalPositionToLocalPosition(kMovePacket.lX, kMovePacket.lY);

	SNetworkMoveActorData kNetMoveActorData;
	kNetMoveActorData.m_dwArg=kMovePacket.bArg;
	kNetMoveActorData.m_dwFunc=kMovePacket.bFunc;
	kNetMoveActorData.m_dwTime=kMovePacket.dwTime;
	kNetMoveActorData.m_dwVID=kMovePacket.dwVID;
	kNetMoveActorData.m_fRot=kMovePacket.bRot*5.0f;
	kNetMoveActorData.m_lPosX=kMovePacket.lX;
	kNetMoveActorData.m_lPosY=kMovePacket.lY;
	kNetMoveActorData.m_dwDuration=kMovePacket.dwDuration;

	m_rokNetActorMgr->MoveActor(kNetMoveActorData);

	return true;
}

bool CPythonNetworkStream::RecvOwnerShipPacket()
{
	TPacketGCOwnership kPacketOwnership;

	if (!Recv(sizeof(kPacketOwnership), &kPacketOwnership))
		return false;

	m_rokNetActorMgr->SetActorOwner(kPacketOwnership.dwOwnerVID, kPacketOwnership.dwVictimVID);

	return true;
}

bool CPythonNetworkStream::RecvSyncPositionPacket()
{
	TPacketGCSyncPosition kPacketSyncPos;
	if (!Recv(sizeof(kPacketSyncPos), &kPacketSyncPos))
		return false;

	TPacketGCSyncPositionElement kSyncPos;

	UINT uSyncPosCount=(kPacketSyncPos.wSize-sizeof(kPacketSyncPos))/sizeof(kSyncPos);
	for (UINT iSyncPos=0; iSyncPos<uSyncPosCount; ++iSyncPos)
	{		
		if (!Recv(sizeof(TPacketGCSyncPositionElement), &kSyncPos))
			return false;

		__GlobalPositionToLocalPosition(kSyncPos.lX, kSyncPos.lY);
		m_rokNetActorMgr->SyncActor(kSyncPos.dwVID, kSyncPos.lX, kSyncPos.lY);
	}

	return true;
}
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
bool CPythonNetworkStream::RecvBuffNPCUseSkill()
{
	TPacketGCBuffNPCUseSkill packet;
	if (!Recv(sizeof(TPacketGCBuffNPCUseSkill), &packet)) {
		Tracen("CPythonNetworkStream::RecvBuffNPCUseSkill - PACKET READ ERROR");
		return false;
	}
	CInstanceBase* pkInstClone = CPythonCharacterManager::Instance().GetInstancePtr(packet.dVid);
	DWORD dwSkillIndex = packet.dSkillVnum;
	CPythonSkill::TSkillData* pSkillData;
	if (!CPythonSkill::Instance().GetSkillData(dwSkillIndex, &pSkillData))
		return false;

	DWORD dSkillGrade;
	if (packet.dSkillLevel < 20)
		dSkillGrade = 0;
	else if (packet.dSkillLevel >= 20 && packet.dSkillLevel < 30)
		dSkillGrade = 1;
	else if (packet.dSkillLevel >= 30 && packet.dSkillLevel < 40)
		dSkillGrade = 2;
	else if (packet.dSkillLevel >= 40)
		dSkillGrade = 3;

	DWORD dwMotionIndex = pSkillData->GetSkillMotionIndex(dSkillGrade);
	if (!pkInstClone->NEW_UseSkill(dwSkillIndex, dwMotionIndex, 1, false))
	{
		Tracenf("CPythonPlayer::UseBuffNPCSkill(%d) - ERROR", dwSkillIndex);
	}

	return true;
}
#endif