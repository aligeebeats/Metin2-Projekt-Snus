#include "StdAfx.h"
#include "PythonNonPlayer.h"

#include "InstanceBase.h"
#include "PythonCharacterManager.h"

PyObject * nonplayerGetEventType(PyObject * poSelf, PyObject * poArgs)
{
	int iVirtualNumber;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualNumber))
		return Py_BuildException();

	BYTE iType = CPythonNonPlayer::Instance().GetEventType(iVirtualNumber);

	return Py_BuildValue("i", iType);
}

PyObject * nonplayerGetEventTypeByVID(PyObject * poSelf, PyObject * poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	BYTE iType = CPythonNonPlayer::Instance().GetEventTypeByVID(iVirtualID);

	return Py_BuildValue("i", iType);
}

PyObject * nonplayerGetLevelByVID(PyObject * poSelf, PyObject * poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (!pInstance)
		return Py_BuildValue("i", -1);

	const CPythonNonPlayer::TMobTable * pMobTable = CPythonNonPlayer::Instance().GetTable(pInstance->GetVirtualNumber());

	if (!pMobTable)
		return Py_BuildValue("i", -1);

	float fAverageLevel = pMobTable->bLevel;
	fAverageLevel = floor(fAverageLevel + 0.5f);
	return Py_BuildValue("i", int(fAverageLevel));
}

PyObject * nonplayerGetGradeByVID(PyObject * poSelf, PyObject * poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (!pInstance)
		return Py_BuildValue("i", -1);

	const CPythonNonPlayer::TMobTable * pMobTable = CPythonNonPlayer::Instance().GetTable(pInstance->GetVirtualNumber());

	if (!pMobTable)
		return Py_BuildValue("i", -1);

	return Py_BuildValue("i", pMobTable->bRank);
}


PyObject * nonplayerGetMonsterName(PyObject * poSelf, PyObject * poArgs)
{
	int iVNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVNum))
		return Py_BuildException();

	CPythonNonPlayer& rkNonPlayer=CPythonNonPlayer::Instance();
	return Py_BuildValue("s", rkNonPlayer.GetMonsterName(iVNum));
}

PyObject * nonplayerLoadNonPlayerData(PyObject * poSelf, PyObject * poArgs)
{
	char * szFileName;
	if(!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	CPythonNonPlayer::Instance().LoadNonPlayerData(szFileName);
	return Py_BuildNone();
}

#ifdef ENABLE_TARGET_INFO
PyObject* nonplayerGetMonsterMaxHP(PyObject* poSelf, PyObject* poArgs)
{
	int race;
	if (!PyTuple_GetInteger(poArgs, 0, &race))
		return Py_BuildException();

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();

	return Py_BuildValue("i", rkNonPlayer.GetMonsterMaxHP(race));
}

PyObject* nonplayerGetMonsterRaceFlag(PyObject* poSelf, PyObject* poArgs)
{
	int race;
	if (!PyTuple_GetInteger(poArgs, 0, &race))
		return Py_BuildException();

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();

	return Py_BuildValue("i", rkNonPlayer.GetMonsterRaceFlag(race));
}

PyObject* nonplayerGetMonsterLevel(PyObject* poSelf, PyObject* poArgs)
{
	int race;
	if (!PyTuple_GetInteger(poArgs, 0, &race))
		return Py_BuildException();

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();

	return Py_BuildValue("i", rkNonPlayer.GetMonsterLevel(race));
}

PyObject* nonplayerGetMonsterDamage(PyObject* poSelf, PyObject* poArgs)
{
	int race;
	if (!PyTuple_GetInteger(poArgs, 0, &race))
		return Py_BuildException();

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();

	DWORD dmg1 = rkNonPlayer.GetMonsterDamage1(race);
	DWORD dmg2 = rkNonPlayer.GetMonsterDamage2(race);

	return Py_BuildValue("ii", dmg1, dmg2);
}

PyObject* nonplayerGetMonsterExp(PyObject* poSelf, PyObject* poArgs)
{
	int race;
	if (!PyTuple_GetInteger(poArgs, 0, &race))
		return Py_BuildException();

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();

	return Py_BuildValue("i", rkNonPlayer.GetMonsterExp(race));
}

PyObject* nonplayerGetMonsterDamageMultiply(PyObject* poSelf, PyObject* poArgs)
{
	int race;
	if (!PyTuple_GetInteger(poArgs, 0, &race))
		return Py_BuildException();

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();

	return Py_BuildValue("f", rkNonPlayer.GetMonsterDamageMultiply(race));
}

PyObject* nonplayerGetMonsterST(PyObject* poSelf, PyObject* poArgs)
{
	int race;
	if (!PyTuple_GetInteger(poArgs, 0, &race))
		return Py_BuildException();

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();

	return Py_BuildValue("i", rkNonPlayer.GetMonsterST(race));
}

PyObject* nonplayerGetMonsterDX(PyObject* poSelf, PyObject* poArgs)
{
	int race;
	if (!PyTuple_GetInteger(poArgs, 0, &race))
		return Py_BuildException();

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();

	return Py_BuildValue("i", rkNonPlayer.GetMonsterDX(race));
}

PyObject* nonplayerIsMonsterStone(PyObject* poSelf, PyObject* poArgs)
{
	int race;
	if (!PyTuple_GetInteger(poArgs, 0, &race))
		return Py_BuildException();

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();

	return Py_BuildValue("i", rkNonPlayer.IsMonsterStone(race) ? 1 : 0);
}

PyObject* nonplayerGetMonsterResist(PyObject* poSelf, PyObject* poArgs)
{
	int race;
	if (!PyTuple_GetInteger(poArgs, 0, &race))
		return Py_BuildException();

	BYTE bResist;
	if (!PyTuple_GetByte(poArgs, 1, &bResist))
		return Py_BuildException();

	CPythonNonPlayer& rkNonPlayer = CPythonNonPlayer::Instance();

	return Py_BuildValue("i", rkNonPlayer.GetMonsterResist(race, bResist));
}
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
PyObject* nonplayerGetRaceNumByVID(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (!pInstance)
		return Py_BuildValue("i", -1);

	const CPythonNonPlayer::TMobTable* pMobTable = CPythonNonPlayer::Instance().GetTable(pInstance->GetVirtualNumber());

	if (!pMobTable)
		return Py_BuildValue("i", -1);

	return Py_BuildValue("i", pMobTable->dwVnum);
}
#endif
void initNonPlayer()
{
	static PyMethodDef s_methods[] =
	{
		{ "GetEventType",				nonplayerGetEventType,				METH_VARARGS },
		{ "GetEventTypeByVID",			nonplayerGetEventTypeByVID,			METH_VARARGS },
		{ "GetLevelByVID",				nonplayerGetLevelByVID,				METH_VARARGS },
		{ "GetGradeByVID",				nonplayerGetGradeByVID,				METH_VARARGS },
		{ "GetMonsterName",				nonplayerGetMonsterName,			METH_VARARGS },
#ifdef ENABLE_TARGET_INFO
		{ "GetMonsterMaxHP",			nonplayerGetMonsterMaxHP,			METH_VARARGS },
		{ "GetMonsterRaceFlag",			nonplayerGetMonsterRaceFlag,		METH_VARARGS },
		{ "GetMonsterLevel",			nonplayerGetMonsterLevel,			METH_VARARGS },
		{ "GetMonsterDamage",			nonplayerGetMonsterDamage,			METH_VARARGS },
		{ "GetMonsterExp",				nonplayerGetMonsterExp,				METH_VARARGS },
		{ "GetMonsterDamageMultiply",	nonplayerGetMonsterDamageMultiply,	METH_VARARGS },
		{ "GetMonsterST",				nonplayerGetMonsterST,				METH_VARARGS },
		{ "GetMonsterDX",				nonplayerGetMonsterDX,				METH_VARARGS },
		{ "IsMonsterStone",				nonplayerIsMonsterStone,			METH_VARARGS },
		{ "GetMonsterResist",			nonplayerGetMonsterResist,			METH_VARARGS },
		
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
		{ "GetRaceNumByVID",			nonplayerGetRaceNumByVID,			METH_VARARGS },
#endif
		{ "LoadNonPlayerData",			nonplayerLoadNonPlayerData,			METH_VARARGS },

		{ NULL,							NULL,								NULL		 },
	};

	PyObject * poModule = Py_InitModule("nonplayer", s_methods);

	PyModule_AddIntConstant(poModule, "ON_CLICK_EVENT_NONE",		CPythonNonPlayer::ON_CLICK_EVENT_NONE);
	PyModule_AddIntConstant(poModule, "ON_CLICK_EVENT_BATTLE",		CPythonNonPlayer::ON_CLICK_EVENT_BATTLE);
	PyModule_AddIntConstant(poModule, "ON_CLICK_EVENT_SHOP",		CPythonNonPlayer::ON_CLICK_EVENT_SHOP);
	PyModule_AddIntConstant(poModule, "ON_CLICK_EVENT_TALK",		CPythonNonPlayer::ON_CLICK_EVENT_TALK);
	PyModule_AddIntConstant(poModule, "ON_CLICK_EVENT_VEHICLE",		CPythonNonPlayer::ON_CLICK_EVENT_VEHICLE);

	PyModule_AddIntConstant(poModule, "PAWN", 0);
	PyModule_AddIntConstant(poModule, "S_PAWN", 1);
	PyModule_AddIntConstant(poModule, "KNIGHT", 2);
	PyModule_AddIntConstant(poModule, "S_KNIGHT", 3);
	PyModule_AddIntConstant(poModule, "BOSS", 4);
	PyModule_AddIntConstant(poModule, "KING", 5);

#ifdef ENABLE_TARGET_INFO
	PyModule_AddIntConstant(poModule, "MOB_RESIST_SWORD", CPythonNonPlayer::MOB_RESIST_SWORD);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_TWOHAND", CPythonNonPlayer::MOB_RESIST_TWOHAND);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_DAGGER", CPythonNonPlayer::MOB_RESIST_DAGGER);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_BELL", CPythonNonPlayer::MOB_RESIST_BELL);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_FAN", CPythonNonPlayer::MOB_RESIST_FAN);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_BOW", CPythonNonPlayer::MOB_RESIST_BOW);
#ifdef ENABLE_WOLFMAN
	PyModule_AddIntConstant(poModule, "MOB_RESIST_CLAW", CPythonNonPlayer::MOB_RESIST_CLAW);
#endif
	PyModule_AddIntConstant(poModule, "MOB_RESIST_FIRE", CPythonNonPlayer::MOB_RESIST_FIRE);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_ELECT", CPythonNonPlayer::MOB_RESIST_ELECT);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_MAGIC", CPythonNonPlayer::MOB_RESIST_MAGIC);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_WIND", CPythonNonPlayer::MOB_RESIST_WIND);
	PyModule_AddIntConstant(poModule, "MOB_RESIST_POISON", CPythonNonPlayer::MOB_RESIST_POISON);
#ifdef ENABLE_WOLFMAN
	PyModule_AddIntConstant(poModule, "MOB_RESIST_BLEEDING", CPythonNonPlayer::MOB_RESIST_BLEEDING);
#endif
#endif
}