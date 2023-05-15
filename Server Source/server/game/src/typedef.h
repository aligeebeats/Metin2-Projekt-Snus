#pragma once
#define itertype(v) decltype((v).begin())

class DESC;
typedef DESC* LPDESC;

class CLIENT_DESC;
typedef CLIENT_DESC* LPCLIENT_DESC;

class DESC_P2P;
typedef DESC_P2P* LPDESC_P2P;

class CHARACTER;
typedef CHARACTER* LPCHARACTER;
typedef std::vector<LPCHARACTER> CHARACTER_VECTOR;
typedef std::list<LPCHARACTER> CHARACTER_LIST;
typedef boost::unordered_set<LPCHARACTER> CHARACTER_SET;

class CItem;
typedef CItem* LPITEM;

namespace building {
class CObject;
typedef CObject* LPOBJECT;
}

typedef struct regen* LPREGEN;
typedef struct regen_exception* LPREGEN_EXCEPTION;

class CEntity;
typedef CEntity* LPENTITY;
typedef std::vector<LPENTITY> ENTITY_VECTOR;
typedef boost::unordered_set<LPENTITY> ENTITY_SET;

class SECTREE;
typedef SECTREE* LPSECTREE;
typedef std::list<LPSECTREE> LPSECTREE_LIST;

class SECTREE_MAP;
typedef SECTREE_MAP* LPSECTREE_MAP;

class CDungeon;
typedef CDungeon* LPDUNGEON;

class CParty;
typedef CParty* LPPARTY;

typedef struct pixel_position_s
{
	INT x, y, z;
} PIXEL_POSITION;

enum EEntityTypes
{
	ENTITY_CHARACTER,
	ENTITY_ITEM,
	ENTITY_OBJECT,
};

