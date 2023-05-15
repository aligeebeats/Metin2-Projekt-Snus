#pragma once
#pragma pack(push)
#pragma pack(8)
#pragma pack(pop)

#include "../gamelib/RaceData.h"

typedef BYTE TPacketHeader;

enum
{
	HEADER_CG_LOGIN								= 1,
	HEADER_CG_ATTACK							= 2,
	HEADER_CG_CHAT								= 3,
	HEADER_CG_PLAYER_CREATE						= 4,
	HEADER_CG_PLAYER_DESTROY					= 5,
	HEADER_CG_PLAYER_SELECT						= 6,
	HEADER_CG_CHARACTER_MOVE					= 7,
	HEADER_CG_SYNC_POSITION  					= 8,
	HEADER_CG_DIRECT_ENTER						= 9,
	HEADER_CG_ENTERGAME							= 10,
	HEADER_CG_ITEM_USE							= 11,
	HEADER_CG_ITEM_DROP							= 12,
	HEADER_CG_ITEM_MOVE							= 13,
	HEADER_CG_ITEM_PICKUP						= 15,
	HEADER_CG_QUICKSLOT_ADD                     = 16,
	HEADER_CG_QUICKSLOT_DEL                     = 17,
	HEADER_CG_QUICKSLOT_SWAP                    = 18,
	HEADER_CG_WHISPER							= 19,
	HEADER_CG_ITEM_DROP2                        = 20,
	HEADER_CG_ITEM_DESTROY						= 21,
	HEADER_CG_ON_CLICK							= 26,
	HEADER_CG_EXCHANGE							= 27,
    HEADER_CG_CHARACTER_POSITION                = 28,
    HEADER_CG_SCRIPT_ANSWER						= 29,
	HEADER_CG_QUEST_INPUT_STRING				= 30,
    HEADER_CG_QUEST_CONFIRM                     = 31,
	HEADER_CG_PVP								= 41,
    HEADER_CG_SHOP								= 50,
	HEADER_CG_FLY_TARGETING						= 51,
	HEADER_CG_USE_SKILL							= 52,
    HEADER_CG_ADD_FLY_TARGETING                 = 53,
	HEADER_CG_SHOOT								= 54,
	HEADER_CG_MYSHOP                            = 55, 
#ifdef ENABLE_TARGET_INFO
	HEADER_CG_TARGET_INFO_LOAD					= 59,
#endif
	HEADER_CG_ITEM_USE_TO_ITEM					= 60,
    HEADER_CG_TARGET                            = 61,
	HEADER_CG_WARP								= 65, 
    HEADER_CG_SCRIPT_BUTTON						= 66,
    HEADER_CG_MESSENGER                         = 67,
    HEADER_CG_MALL_CHECKOUT                     = 69,
    HEADER_CG_SAFEBOX_CHECKIN                   = 70,
    HEADER_CG_SAFEBOX_CHECKOUT                  = 71,
    HEADER_CG_PARTY_INVITE                      = 72,
    HEADER_CG_PARTY_INVITE_ANSWER               = 73,
    HEADER_CG_PARTY_REMOVE                      = 74,
    HEADER_CG_PARTY_SET_STATE                   = 75,
    HEADER_CG_PARTY_USE_SKILL                   = 76,
    HEADER_CG_SAFEBOX_ITEM_MOVE                 = 77,
	HEADER_CG_PARTY_PARAMETER                   = 78,
	HEADER_CG_GUILD								= 80,
	HEADER_CG_ANSWER_MAKE_GUILD					= 81,
    HEADER_CG_FISHING                           = 82,
    HEADER_CG_GIVE_ITEM                         = 83,
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	HEADER_CG_BUFF_NPC_CREATE					= 86,
	HEADER_CG_BUFF_NPC_ACTION					= 87,
#endif
    HEADER_CG_EMPIRE                            = 90,
    HEADER_CG_REFINE                            = 96,
#ifdef ENABLE_HUNTING_SYSTEM
	HEADER_CG_SEND_HUNTING_ACTION				= 99,
#endif
	HEADER_CG_MARK_LOGIN						= 100,
	HEADER_CG_MARK_CRCLIST						= 101,
	HEADER_CG_MARK_UPLOAD						= 102,
	HEADER_CG_MARK_IDXLIST						= 104,
	HEADER_CG_CRC_REPORT						= 103,
	HEADER_CG_HACK								= 105,
    HEADER_CG_CHANGE_NAME                       = 106,
    HEADER_CG_LOGIN2                            = 109,
	HEADER_CG_DUNGEON							= 110,
	HEADER_CG_LOGIN3							= 111,
	HEADER_CG_GUILD_SYMBOL_UPLOAD				= 112,
	HEADER_CG_GUILD_SYMBOL_CRC					= 113,
	HEADER_CG_SCRIPT_SELECT_ITEM				= 114,
	HEADER_CG_LOGIN4							= 115,
#ifdef ENABLE_SWITCHBOT
	HEADER_CG_SWITCHBOT							= 171,
#endif
	HEADER_CG_DRAGON_SOUL_REFINE				= 205,
	HEADER_CG_STATE_CHECKER						= 206,
	HEADER_CG_ACCE								= 207,
#ifdef _IMPROVED_PACKET_ENCRYPTION_
	HEADER_CG_KEY_AGREEMENT						= 0xfb,
#endif
#ifdef ENABLE_ITEMSHOP
	HEADER_CG_BUY_ITEMSHOP_ITEM					= 240,
	HEADER_CG_PROMOTION							= 241,
#endif
	HEADER_CG_TIME_SYNC							= 0xfc,
	HEADER_CG_PONG								= 0xfe,
    HEADER_CG_HANDSHAKE                         = 0xff,

	HEADER_GC_CHARACTER_ADD						= 1,
	HEADER_GC_CHARACTER_DEL						= 2,
	HEADER_GC_CHARACTER_MOVE					= 3,
	HEADER_GC_CHAT								= 4,
	HEADER_GC_SYNC_POSITION 					= 5,
	HEADER_GC_LOGIN_SUCCESS						= 6,
	HEADER_GC_LOGIN_FAILURE						= 7,
	HEADER_GC_PLAYER_CREATE_SUCCESS				= 8,
	HEADER_GC_PLAYER_CREATE_FAILURE				= 9,
	HEADER_GC_PLAYER_DELETE_SUCCESS				= 10,
	HEADER_GC_PLAYER_DELETE_WRONG_SOCIAL_ID		= 11,
	HEADER_GC_STUN								= 13,
	HEADER_GC_DEAD								= 14,
	HEADER_GC_MAIN_CHARACTER					= 15,
	HEADER_GC_PLAYER_POINTS						= 16,
	HEADER_GC_PLAYER_POINT_CHANGE				= 17,
	HEADER_GC_CHANGE_SPEED						= 18,
	HEADER_GC_CHARACTER_UPDATE                  = 19,
	HEADER_GC_ITEM_SET							= 20,
	HEADER_GC_ITEM_SET2							= 21,
	HEADER_GC_ITEM_USE							= 22,
	HEADER_GC_ITEM_DROP							= 23,
	HEADER_GC_ITEM_UPDATE						= 25,
	HEADER_GC_ITEM_GROUND_ADD					= 26,
	HEADER_GC_ITEM_GROUND_DEL					= 27,
    HEADER_GC_QUICKSLOT_ADD                     = 28,
    HEADER_GC_QUICKSLOT_DEL                     = 29,
    HEADER_GC_QUICKSLOT_SWAP                    = 30,
	HEADER_GC_ITEM_OWNERSHIP					= 31,
	HEADER_GC_ITEM_UNBIND_TIME					= 33,
	HEADER_GC_WHISPER							= 34,
	HEADER_GC_ALERT								= 35,
	HEADER_GC_MOTION							= 36,
	HEADER_GC_SHOP							    = 38,
	HEADER_GC_SHOP_SIGN							= 39,
	HEADER_GC_PVP								= 41,
	HEADER_GC_EXCHANGE							= 42,
    HEADER_GC_CHARACTER_POSITION                = 43,
	HEADER_GC_PING								= 44,
	HEADER_GC_SCRIPT							= 45,
    HEADER_GC_QUEST_CONFIRM                     = 46,
#ifdef ENABLE_HUNTING_SYSTEM
	HEADER_GC_HUNTING_OPEN_MAIN					= 52,
	HEADER_GC_HUNTING_OPEN_SELECT				= 53,
	HEADER_GC_HUNTING_OPEN_REWARD				= 54,
	HEADER_GC_HUNTING_UPDATE					= 55,
	HEADER_GC_HUNTING_RECIVE_RAND_ITEMS			= 56,
#endif
#ifdef ENABLE_TARGET_INFO
	HEADER_GC_TARGET_INFO						= 58,
#endif
	HEADER_GC_MOUNT								= 61,
	HEADER_GC_OWNERSHIP                         = 62, 
    HEADER_GC_TARGET                            = 63,
	HEADER_GC_WARP								= 65, 
	HEADER_GC_ADD_FLY_TARGETING                 = 69,
	HEADER_GC_CREATE_FLY						= 70,
	HEADER_GC_FLY_TARGETING						= 71,
	HEADER_GC_SKILL_COOLTIME_END				= 73,
    HEADER_GC_MESSENGER                         = 74,
	HEADER_GC_GUILD								= 75,
	HEADER_GC_SKILL_LEVEL_NEW					= 76,
    HEADER_GC_PARTY_INVITE                      = 77,
    HEADER_GC_PARTY_ADD                         = 78,
    HEADER_GC_PARTY_UPDATE                      = 79,
    HEADER_GC_PARTY_REMOVE                      = 80,
    HEADER_GC_QUEST_INFO                        = 81,
    HEADER_GC_REQUEST_MAKE_GUILD                = 82,
	HEADER_GC_PARTY_PARAMETER                   = 83,
    HEADER_GC_SAFEBOX_MONEY_CHANGE              = 84,
    HEADER_GC_SAFEBOX_SET                       = 85,
    HEADER_GC_SAFEBOX_DEL                       = 86,
    HEADER_GC_SAFEBOX_WRONG_PASSWORD            = 87,
    HEADER_GC_SAFEBOX_SIZE                      = 88,
    HEADER_GC_FISHING                           = 89,
    HEADER_GC_EMPIRE                            = 90,
    HEADER_GC_PARTY_LINK                        = 91,
    HEADER_GC_PARTY_UNLINK                      = 92,
    HEADER_GC_REFINE_INFORMATION                = 95,
	HEADER_GC_OBSERVER_ADD						= 96,
	HEADER_GC_OBSERVER_REMOVE					= 97,
	HEADER_GC_OBSERVER_MOVE						= 98,
	HEADER_GC_VIEW_EQUIP                        = 99,
	HEADER_GC_MARK_BLOCK						= 100,
	HEADER_GC_MARK_DIFF_DATA                    = 101,
	HEADER_GC_MARK_IDXLIST						= 102,
    HEADER_GC_TIME                              = 106,
    HEADER_GC_CHANGE_NAME                       = 107,
	HEADER_GC_DUNGEON							= 110,
	HEADER_GC_WALK_MODE							= 111, 
	HEADER_GC_CHANGE_SKILL_GROUP				= 112,
	HEADER_GC_MAIN_CHARACTER2_EMPIRE			= 113,
    HEADER_GC_SEPCIAL_EFFECT                    = 114,
	HEADER_GC_NPC_POSITION						= 115,
    HEADER_GC_CHARACTER_UPDATE2                 = 117,
    HEADER_GC_LOGIN_KEY                         = 118,
    HEADER_GC_REFINE_INFORMATION_NEW            = 119,
    HEADER_GC_CHARACTER_ADD2                    = 120,
    HEADER_GC_CHANNEL                           = 121,
    HEADER_GC_MALL_OPEN                         = 122,
	HEADER_GC_TARGET_UPDATE                     = 123,
	HEADER_GC_TARGET_DELETE                     = 124,
	HEADER_GC_TARGET_CREATE_NEW                 = 125,
	HEADER_GC_AFFECT_ADD                        = 126,
	HEADER_GC_AFFECT_REMOVE                     = 127,
    HEADER_GC_MALL_SET                          = 128,
    HEADER_GC_MALL_DEL                          = 129,
	HEADER_GC_LAND_LIST                         = 130,
	HEADER_GC_LOVER_INFO						= 131,
	HEADER_GC_LOVE_POINT_UPDATE					= 132,
	HEADER_GC_GUILD_SYMBOL_DATA					= 133,
    HEADER_GC_DIG_MOTION                        = 134,
	HEADER_GC_DAMAGE_INFO						= 135,
	HEADER_GC_CHAR_ADDITIONAL_INFO				= 136,
	HEADER_GC_MAIN_CHARACTER3_BGM				= 137,
	HEADER_GC_MAIN_CHARACTER4_BGM_VOL			= 138,
    HEADER_GC_AUTH_SUCCESS                      = 150,
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	HEADER_GC_BUFF_NPC_ACTION					= 164,
	HEADER_GC_BUFF_NPC_USE_SKILL				= 165,
#endif
#ifdef ENABLE_SWITCHBOT
	HEADER_GC_SWITCHBOT							= 171,
#endif
	HEADER_GC_SPECIFIC_EFFECT					= 208,
	HEADER_GC_DRAGON_SOUL_REFINE				= 209,
	HEADER_GC_RESPOND_CHANNELSTATUS				= 210,
	HEADER_GC_ACCE								= 211,

#if defined(__BL_DROP_DESTROY_TIME__)
	HEADER_GC_ITEM_DESTROY_TIME					= 212,
#endif

#ifdef _IMPROVED_PACKET_ENCRYPTION_
	HEADER_GC_KEY_AGREEMENT_COMPLETED			= 0xfa,
	HEADER_GC_KEY_AGREEMENT						= 0xfb,
#endif
#ifdef ENABLE_ITEMSHOP
	HEADER_GC_ITEMSHOP							= 240,
#endif
	HEADER_GC_HANDSHAKE_OK						= 0xfc,
	HEADER_GC_PHASE								= 0xfd,
    HEADER_GC_BINDUDP                           = 0xfe,
    HEADER_GC_HANDSHAKE                         = 0xff,
};

enum
{
	ID_MAX_NUM = 30,
	PASS_MAX_NUM = 16,
	CHAT_MAX_NUM = 128,
	PATH_NODE_MAX_NUM = 64,
	SHOP_SIGN_MAX_LEN = 32,
	PLAYER_PER_ACCOUNT = 4,
	PLAYER_ITEM_SLOT_MAX_NUM = 20,
	QUICKSLOT_MAX_LINE = 4,
	QUICKSLOT_MAX_COUNT_PER_LINE = 8,
	QUICKSLOT_MAX_COUNT = QUICKSLOT_MAX_LINE * QUICKSLOT_MAX_COUNT_PER_LINE,
	QUICKSLOT_MAX_NUM = 36,
	SHOP_HOST_ITEM_MAX_NUM = 40,
	METIN_SOCKET_COUNT = 6,
	PARTY_AFFECT_SLOT_MAX_NUM = 7,
	GUILD_GRADE_NAME_MAX_LEN = 8,
	GUILD_NAME_MAX_LEN = 12,
	GUILD_GRADE_COUNT = 15,
	GULID_COMMENT_MAX_LEN = 50,
	MARK_CRC_NUM = 8*8,
	MARK_DATA_SIZE = 16*12,
	SYMBOL_DATA_SIZE = 128*256,
	QUEST_INPUT_STRING_MAX_NUM = 64,
	PRIVATE_CODE_LENGTH = 8,
	REFINE_MATERIAL_MAX_NUM = 5,
	WEAR_MAX_NUM = 11,
	SHOP_TAB_NAME_MAX = 32,
	SHOP_TAB_COUNT_MAX = 3,
};

#pragma pack(push)
#pragma pack(1)

typedef struct command_mark_login
{
    BYTE    header;
    DWORD   handle;
    DWORD   random_key;
} TPacketCGMarkLogin;

typedef struct command_mark_upload
{
    BYTE    header;
    DWORD   gid;
    BYTE    image[16*12*4];
} TPacketCGMarkUpload;

typedef struct command_mark_idxlist
{
    BYTE    header;
} TPacketCGMarkIDXList;

typedef struct command_mark_crclist
{
    BYTE    header;
    BYTE    imgIdx;
    DWORD   crclist[80];
} TPacketCGMarkCRCList;

typedef struct packet_mark_idxlist
{
    BYTE    header;
	DWORD	bufSize;
    WORD    count;
} TPacketGCMarkIDXList;

typedef struct packet_mark_block
{
    BYTE    header;
    DWORD   bufSize;
	BYTE	imgIdx;
    DWORD   count;
} TPacketGCMarkBlock;

typedef struct command_symbol_upload
{
	BYTE	header;
	WORD	size;
	DWORD	handle;
} TPacketCGSymbolUpload;

typedef struct command_symbol_crc
{
	BYTE	header;
	DWORD	dwGuildID;
	DWORD	dwCRC;
	DWORD	dwSize;
} TPacketCGSymbolCRC;

typedef struct packet_symbol_data
{
    BYTE header;
    WORD size;
    DWORD guild_id;
} TPacketGCGuildSymbolData;

typedef struct packet_observer_add
{
	BYTE	header;
	DWORD	vid;
	WORD	x;
	WORD	y;
} TPacketGCObserverAdd;

typedef struct packet_observer_move
{
	BYTE	header;
	DWORD	vid;
	WORD	x;
	WORD	y;
} TPacketGCObserverMove;


typedef struct packet_observer_remove
{
	BYTE	header;
	DWORD	vid;	
} TPacketGCObserverRemove;

typedef struct command_checkin
{
	BYTE header;
	char name[ID_MAX_NUM+1];
	char pwd[PASS_MAX_NUM+1];
} TPacketCGCheckin;

typedef struct command_login
{
    BYTE header;
    char name[ID_MAX_NUM + 1];
    char pwd[PASS_MAX_NUM + 1];
} TPacketCGLogin;

typedef struct command_login2
{
	BYTE	header;
	char	name[ID_MAX_NUM + 1];
	DWORD	login_key;
    DWORD	adwClientKey[4];
} TPacketCGLogin2;

typedef struct command_login3
{
    BYTE	header;
    char	name[ID_MAX_NUM + 1];
    char	pwd[PASS_MAX_NUM + 1];
    DWORD	adwClientKey[4];
	char    clientVersion[8];
} TPacketCGLogin3;

typedef struct command_direct_enter
{
    BYTE        bHeader;
    char        login[ID_MAX_NUM + 1];
    char        passwd[PASS_MAX_NUM + 1];
    BYTE        index;
} TPacketCGDirectEnter;

typedef struct command_player_select
{
	BYTE	header;
	BYTE	player_index;
} TPacketCGSelectCharacter;

typedef struct command_attack
{
	BYTE	header;
	BYTE	bType;
	DWORD	dwVictimVID;
} TPacketCGAttack;

typedef struct command_chat
{
	BYTE	header;
	WORD	length;
	BYTE	type;
} TPacketCGChat;

typedef struct command_whisper
{
    BYTE        bHeader;
    WORD        wSize;
    char        szNameTo[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGWhisper;

enum EBattleMode
{
	BATTLEMODE_ATTACK = 0,
	BATTLEMODE_DEFENSE = 1,
};

typedef struct command_EnterFrontGame
{
	BYTE header;
} TPacketCGEnterFrontGame;

typedef struct command_item_use
{
	BYTE header;
	TItemPos pos;
#ifdef ENABLE_OPEN_MULTIPLE
	WORD wCount;
#endif
} TPacketCGItemUse;

typedef struct command_item_use_to_item
{
	BYTE header;
	TItemPos source_pos;
	TItemPos target_pos;
} TPacketCGItemUseToItem;

typedef struct command_item_drop
{
	BYTE  header;
	TItemPos pos;
	DWORD elk;
} TPacketCGItemDrop;

typedef struct command_item_drop2
{
    BYTE        header;
    TItemPos pos;
    DWORD       gold;
    WORD        count;
} TPacketCGItemDrop2;

typedef struct command_item_destroy
{
	BYTE		header;
	TItemPos	pos;
} TPacketCGItemDestroy;

typedef struct command_item_move
{
	BYTE header;
	TItemPos pos;
	TItemPos change_pos;
	WORD num;
} TPacketCGItemMove;

typedef struct command_item_pickup
{
	BYTE header;
	DWORD vid;
} TPacketCGItemPickUp;

typedef struct command_quickslot_add
{
    BYTE        header;
    BYTE        pos;
	TQuickSlot	slot;
}TPacketCGQuickSlotAdd;

typedef struct command_quickslot_del
{
    BYTE        header;
    BYTE        pos;
}TPacketCGQuickSlotDel;

typedef struct command_quickslot_swap
{
    BYTE        header;
    BYTE        pos;
    BYTE        change_pos;
}TPacketCGQuickSlotSwap;

typedef struct command_on_click
{
	BYTE		header;
	DWORD		vid;
} TPacketCGOnClick;


enum
{
    SHOP_SUBHEADER_CG_END,
	SHOP_SUBHEADER_CG_BUY,
	SHOP_SUBHEADER_CG_SELL,
	SHOP_SUBHEADER_CG_SELL2,
};

typedef struct command_shop
{
	BYTE        header;
	BYTE		subheader;
} TPacketCGShop;

enum
{
	EXCHANGE_SUBHEADER_CG_START,
	EXCHANGE_SUBHEADER_CG_ITEM_ADD,
	EXCHANGE_SUBHEADER_CG_ITEM_DEL,
	EXCHANGE_SUBHEADER_CG_ELK_ADD,
	EXCHANGE_SUBHEADER_CG_ACCEPT,
	EXCHANGE_SUBHEADER_CG_CANCEL,
};

typedef struct command_exchange
{
	BYTE		header;
	BYTE		subheader;
	long long		arg1;
	BYTE		arg2;
	TItemPos	Pos;
#if defined(ITEM_CHECKINOUT_UPDATE)
	bool		SelectPosAuto;
#endif
} TPacketCGExchange;

typedef struct command_position
{   
    BYTE        header;
    BYTE        position;
} TPacketCGPosition;

typedef struct command_script_answer
{
    BYTE        header;
	BYTE		answer;
} TPacketCGScriptAnswer;

typedef struct command_script_button
{
    BYTE        header;
	unsigned int			idx;
} TPacketCGScriptButton;

typedef struct command_target
{
    BYTE        header;
    DWORD       dwVID;
} TPacketCGTarget;

typedef struct command_move
{
	BYTE		bHeader;
	BYTE		bFunc;
	BYTE		bArg;
	BYTE		bRot;
	LONG		lX;
	LONG		lY;
	DWORD		dwTime;	
} TPacketCGMove;

typedef struct command_sync_position_element 
{ 
    DWORD       dwVID; 
    long        lX; 
    long        lY; 
} TPacketCGSyncPositionElement; 

typedef struct command_sync_position
{ 
    BYTE        bHeader;
	WORD		wSize;
} TPacketCGSyncPosition; 

typedef struct command_fly_targeting
{
	BYTE		bHeader;
	DWORD		dwTargetVID;
	long		lX;
	long		lY;
} TPacketCGFlyTargeting;

typedef struct packet_fly_targeting
{
    BYTE        bHeader;
	DWORD		dwShooterVID;
	DWORD		dwTargetVID;
	long		lX;
	long		lY;
} TPacketGCFlyTargeting;

typedef struct packet_shoot
{   
    BYTE		bHeader;                
    BYTE		bType;
} TPacketCGShoot;

typedef struct command_warp
{
	BYTE			bHeader;
} TPacketCGWarp;

enum
{
	MESSENGER_SUBHEADER_GC_LIST,
	MESSENGER_SUBHEADER_GC_LOGIN,
	MESSENGER_SUBHEADER_GC_LOGOUT,
	MESSENGER_SUBHEADER_GC_INVITE,
	MESSENGER_SUBHEADER_GC_REMOVE_FRIEND,
};

typedef struct packet_messenger
{
    BYTE header;
    WORD size;
    BYTE subheader;
} TPacketGCMessenger;

typedef struct packet_messenger_list_offline
{
    BYTE connected;
	BYTE length;
} TPacketGCMessengerListOffline;

enum
{
	MESSENGER_CONNECTED_STATE_OFFLINE,
	MESSENGER_CONNECTED_STATE_ONLINE,
};

typedef struct packet_messenger_list_online
{
    BYTE connected;
	BYTE length;
} TPacketGCMessengerListOnline;

typedef struct packet_messenger_login
{
	BYTE length;
} TPacketGCMessengerLogin;

typedef struct packet_messenger_logout
{
	BYTE length;
} TPacketGCMessengerLogout;

enum
{
    MESSENGER_SUBHEADER_CG_ADD_BY_VID,
    MESSENGER_SUBHEADER_CG_ADD_BY_NAME,
    MESSENGER_SUBHEADER_CG_REMOVE,
};

typedef struct command_messenger
{
    BYTE header;
    BYTE subheader;
} TPacketCGMessenger;

typedef struct command_messenger_remove
{
	BYTE length;
} TPacketCGMessengerRemove;

enum
{
	SAFEBOX_MONEY_STATE_SAVE,
	SAFEBOX_MONEY_STATE_WITHDRAW,
};

typedef struct command_safebox_money
{
    BYTE        bHeader;
    BYTE        bState;
    DWORD       dwMoney;
} TPacketCGSafeboxMoney;

typedef struct command_safebox_checkout
{
    BYTE        bHeader;
    BYTE        bSafePos;
    TItemPos	ItemPos;
#if defined(ITEM_CHECKINOUT_UPDATE)
	bool		SelectPosAuto;
#endif
} TPacketCGSafeboxCheckout;

typedef struct command_safebox_checkin
{
    BYTE        bHeader;
    BYTE        bSafePos;
    TItemPos	ItemPos;
#if defined(ITEM_CHECKINOUT_UPDATE)
	bool		SelectPosAuto;
#endif
} TPacketCGSafeboxCheckin;

typedef struct command_mall_checkout
{
    BYTE        bHeader;
    BYTE        bMallPos;
    TItemPos	ItemPos;
#if defined(ITEM_CHECKINOUT_UPDATE)
	bool		SelectPosAuto;
#endif
} TPacketCGMallCheckout;

typedef struct command_use_skill
{
    BYTE                bHeader;
    DWORD               dwVnum;
	DWORD				dwTargetVID;
} TPacketCGUseSkill;

typedef struct command_party_invite
{
    BYTE header;
    DWORD vid;
} TPacketCGPartyInvite;

typedef struct command_party_invite_answer
{
    BYTE header;
    DWORD leader_pid;
    BYTE accept;
} TPacketCGPartyInviteAnswer;

typedef struct command_party_remove
{
    BYTE header;
    DWORD pid;
} TPacketCGPartyRemove;

typedef struct command_party_set_state
{
    BYTE byHeader;
    DWORD dwVID;
	BYTE byState;
    BYTE byFlag;
} TPacketCGPartySetState;

typedef struct packet_party_link
{
    BYTE header;
    DWORD pid;
    DWORD vid;
} TPacketGCPartyLink;

typedef struct packet_party_unlink
{
    BYTE header;
    DWORD pid;
	DWORD vid;
} TPacketGCPartyUnlink;

typedef struct command_party_use_skill
{
    BYTE byHeader;
	BYTE bySkillIndex;
    DWORD dwTargetVID;
} TPacketCGPartyUseSkill;

enum
{
	GUILD_SUBHEADER_CG_ADD_MEMBER,
	GUILD_SUBHEADER_CG_REMOVE_MEMBER,
	GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME,
	GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY,
	GUILD_SUBHEADER_CG_OFFER,
	GUILD_SUBHEADER_CG_POST_COMMENT,
	GUILD_SUBHEADER_CG_DELETE_COMMENT,
	GUILD_SUBHEADER_CG_REFRESH_COMMENT,
	GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE,
	GUILD_SUBHEADER_CG_USE_SKILL,
	GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL,
	GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER,
	GUILD_SUBHEADER_CG_CHARGE_GSP,
	GUILD_SUBHEADER_CG_DEPOSIT_MONEY,
	GUILD_SUBHEADER_CG_WITHDRAW_MONEY,
};

typedef struct command_guild
{
    BYTE byHeader;
	BYTE bySubHeader;
} TPacketCGGuild;

typedef struct command_guild_answer_make_guild
{
	BYTE header;
	char guild_name[GUILD_NAME_MAX_LEN+1];
} TPacketCGAnswerMakeGuild; 

typedef struct command_give_item
{
	BYTE byHeader;
	DWORD dwTargetVID;
	TItemPos ItemPos;
	WORD byItemCount;
} TPacketCGGiveItem;

typedef struct SPacketCGHack
{
    BYTE        bHeader;
    char        szBuf[255 + 1];
} TPacketCGHack;

typedef struct command_dungeon
{
	BYTE		bHeader;
	WORD		size;
} TPacketCGDungeon;

typedef struct SShopItemTable
{
    DWORD		vnum;
    WORD		count;
    TItemPos	pos;
    long long	price;
    BYTE		display_pos;
	long alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
	DWORD 		price_type = 1;
	DWORD 		price_vnum = 0;
	SShopItemTable() {
		memset(&alSockets, 0, sizeof(alSockets));
		memset(&aAttr, 0, sizeof(aAttr));
	}
} TShopItemTable;

typedef struct SPacketCGMyShop
{
    BYTE        bHeader;
    char        szSign[SHOP_SIGN_MAX_LEN + 1];
    WORD        wCount;
} TPacketCGMyShop;

typedef struct SPacketCGRefine
{
	BYTE		header;
	WORD        pos;
	BYTE		type;
} TPacketCGRefine;

typedef struct SPacketCGChangeName
{
    BYTE header;
    BYTE index;
    char name[CHARACTER_NAME_MAX_LEN+1];
} TPacketCGChangeName;

typedef struct command_crc_report
{
	BYTE header;
	BYTE byPackMode;
	DWORD dwBinaryCRC32;
	DWORD dwProcessCRC32;
	DWORD dwRootPackCRC32;
} TPacketCGCRCReport;

enum EPartyExpDistributionType
{
    PARTY_EXP_DISTRIBUTION_NON_PARITY,
    PARTY_EXP_DISTRIBUTION_PARITY,
};

typedef struct command_party_parameter
{
    BYTE        bHeader;
    BYTE        bDistributeMode;
} TPacketCGPartyParameter;

typedef struct command_quest_input_string
{
    BYTE        bHeader;
    char		szString[QUEST_INPUT_STRING_MAX_NUM+1];
} TPacketCGQuestInputString;

typedef struct command_quest_confirm
{
    BYTE header;
    BYTE answer;
    DWORD requestPID;
} TPacketCGQuestConfirm;

typedef struct command_script_select_item
{
    BYTE header;
    DWORD selection;
} TPacketCGScriptSelectItem;

enum EPhase
{
    PHASE_CLOSE,
    PHASE_HANDSHAKE,
    PHASE_LOGIN,
    PHASE_SELECT,
    PHASE_LOADING,
    PHASE_GAME,
    PHASE_DEAD,
	PHASE_DBCLIENT_CONNECTING,
    PHASE_DBCLIENT,
    PHASE_P2P,
    PHASE_AUTH,
};

typedef struct packet_phase
{
    BYTE        header;
    BYTE        phase;
} TPacketGCPhase;

typedef struct packet_blank
{
	BYTE		header;
} TPacketGCBlank;

typedef struct packet_blank_dynamic
{
	BYTE		header;
	WORD		size;
} TPacketGCBlankDynamic;

typedef struct packet_header_handshake
{
	BYTE		header;
	DWORD		dwHandshake;
	DWORD		dwTime;
	LONG		lDelta;
} TPacketGCHandshake;

typedef struct packet_header_bindudp
{
	BYTE		header;
	DWORD		addr;
	WORD		port;
} TPacketGCBindUDP;

typedef struct packet_header_dynamic_size
{
	BYTE		header;
	WORD		size;
} TDynamicSizePacketHeader;

typedef struct SSimplePlayerInformation
{
    DWORD               dwID;
    char                szName[CHARACTER_NAME_MAX_LEN + 1];
    BYTE                byJob;
    BYTE                byLevel;
    DWORD               dwPlayMinutes;
    BYTE                byST, byHT, byDX, byIQ;
    WORD                wMainPart;
    BYTE                bChangeName;
	WORD				wHairPart;
	DWORD				dwAccePart;
    BYTE                bDummy[4];
	long				x, y;
	LONG				lAddr;
	WORD				wPort;
	BYTE				bySkillGroup;
} TSimplePlayerInformation;

typedef struct packet_login_success
{
	BYTE						header;
	TSimplePlayerInformation	akSimplePlayerInformation[PLAYER_PER_ACCOUNT];
    DWORD						guild_id[PLAYER_PER_ACCOUNT];
    char						guild_name[PLAYER_PER_ACCOUNT][GUILD_NAME_MAX_LEN+1];
	DWORD handle;
	DWORD random_key;
} TPacketGCLoginSuccess;

enum { LOGIN_STATUS_MAX_LEN = 8 };
typedef struct packet_login_failure
{
	BYTE	header;
	char	szStatus[LOGIN_STATUS_MAX_LEN + 1];
} TPacketGCLoginFailure;

typedef struct command_player_create
{
	BYTE        header;
	BYTE        index;
	char        name[CHARACTER_NAME_MAX_LEN + 1];
	WORD        job;
	BYTE		shape;
	BYTE		CON;
	BYTE		INT;
	BYTE		STR;
	BYTE		DEX;
} TPacketCGCreateCharacter;

typedef struct command_player_create_success
{
    BYTE						header;
    BYTE						bAccountCharacterSlot;
    TSimplePlayerInformation	kSimplePlayerInfomation;
} TPacketGCPlayerCreateSuccess;

typedef struct command_create_failure
{
	BYTE	header;
	BYTE	bType;
} TPacketGCCreateFailure;

typedef struct command_player_delete
{
	BYTE        header;
	BYTE        index;
	char		szPrivateCode[PRIVATE_CODE_LENGTH];
} TPacketCGDestroyCharacter;

typedef struct packet_player_delete_success
{
	BYTE        header;
	BYTE        account_index;
} TPacketGCDestroyCharacterSuccess;

enum
{
	ADD_CHARACTER_STATE_DEAD   = (1 << 0),
	ADD_CHARACTER_STATE_SPAWN  = (1 << 1),
	ADD_CHARACTER_STATE_GUNGON = (1 << 2),
	ADD_CHARACTER_STATE_KILLER = (1 << 3),
	ADD_CHARACTER_STATE_PARTY  = (1 << 4),
};

enum EPKModes
{
	PK_MODE_PEACE,
	PK_MODE_REVENGE,
	PK_MODE_FREE,
	PK_MODE_PROTECT,
	PK_MODE_GUILD,
	PK_MODE_MAX_NUM,
};

enum ECharacterEquipmentPart
{
	CHR_EQUIPPART_ARMOR,
	CHR_EQUIPPART_WEAPON,
	CHR_EQUIPPART_HEAD,
	CHR_EQUIPPART_HAIR,
	CHR_EQUIPPART_ACCE,
	CHR_EQUIPPART_NUM,		
};

typedef struct packet_char_additional_info
{
	BYTE    header;
	DWORD   dwVID;
	char    name[CHARACTER_NAME_MAX_LEN + 1];
	DWORD   dwPart[CHR_EQUIPPART_NUM];
	BYTE	bEmpire;
	DWORD   dwGuildID;
	DWORD   dwLevel;
	short   sAlignment;
	BYTE    bPKMode;
	DWORD   dwMountVnum;
	DWORD	dwArrow;
	BYTE	byAcceDrainRate;
} TPacketGCCharacterAdditionalInfo;

typedef struct packet_add_char
{
    BYTE        header;
    DWORD       dwVID;
    float       angle;
    long        x;
    long        y;
    long        z;
	BYTE		bType;
    WORD        wRaceNum;
    WORD		wMovingSpeed;
    BYTE        bAttackSpeed;
    BYTE        bStateFlag;
    DWORD       dwAffectFlag[2];
} TPacketGCCharacterAdd;

typedef struct packet_add_char2
{
    BYTE        header;
    DWORD       dwVID;
    char        name[CHARACTER_NAME_MAX_LEN + 1];
    float       angle;
    long        x;
    long        y;
    long        z;
	BYTE		bType;
    WORD        wRaceNum;
    DWORD       dwPart[CHR_EQUIPPART_NUM];
    WORD		wMovingSpeed;
    BYTE        bAttackSpeed;
    BYTE        bStateFlag;
    DWORD       dwAffectFlag[2];
    BYTE        bEmpire;
    DWORD       dwGuild;
    short       sAlignment;
	DWORD		dwLevel;
	BYTE		bPKMode;
	DWORD		dwMountVnum;
	DWORD		dwArrow;
	BYTE		byAcceDrainRate;
} TPacketGCCharacterAdd2;

typedef struct packet_update_char
{
    BYTE        header;
    DWORD       dwVID;
    DWORD       dwPart[CHR_EQUIPPART_NUM];
    WORD		wMovingSpeed;
	BYTE		bAttackSpeed;
    BYTE        bStateFlag;
    DWORD       dwAffectFlag[2];
	DWORD		dwGuildID;
    short       sAlignment;
	BYTE		bPKMode;
	DWORD		dwMountVnum;
	DWORD		dwArrow;
	BYTE		byAcceDrainRate;
} TPacketGCCharacterUpdate;

typedef struct packet_del_char
{
	BYTE	header;
    DWORD	dwVID;
} TPacketGCCharacterDelete;

typedef struct packet_GlobalTime
{
	BYTE	header;
	float	GlobalTime;
} TPacketGCGlobalTime;

enum EChatType
{
	CHAT_TYPE_TALKING,
	CHAT_TYPE_INFO,
	CHAT_TYPE_NOTICE,
	CHAT_TYPE_PARTY,
	CHAT_TYPE_GUILD,
	CHAT_TYPE_COMMAND,
	CHAT_TYPE_SHOUT,
	CHAT_TYPE_WHISPER,
	CHAT_TYPE_BIG_NOTICE,
	CHAT_TYPE_MAX_NUM,
};

typedef struct packet_chatting
{
	BYTE	header;
	WORD	size;
	BYTE	type;
	DWORD	dwVID;
	BYTE	bEmpire;
} TPacketGCChat;

typedef struct packet_whisper  
{   
    BYTE        bHeader;                        
    WORD        wSize;
    BYTE        bType;
    char        szNameFrom[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCWhisper;

typedef struct packet_stun
{
	BYTE		header;
	DWORD		vid;
} TPacketGCStun;

typedef struct packet_dead
{
	BYTE		header;
	DWORD		vid;
} TPacketGCDead;

typedef struct packet_main_character
{
    BYTE        header;
    DWORD       dwVID;
	WORD		wRaceNum;
    char        szName[CHARACTER_NAME_MAX_LEN + 1];
    long        lX, lY, lZ;
	BYTE		bySkillGroup;
} TPacketGCMainCharacter;

typedef struct packet_main_character2_empire
{
    BYTE        header;
    DWORD       dwVID;
	WORD		wRaceNum;
    char        szName[CHARACTER_NAME_MAX_LEN + 1];
    long        lX, lY, lZ;
	BYTE		byEmpire;
	BYTE		bySkillGroup;
} TPacketGCMainCharacter2_EMPIRE;

typedef struct packet_main_character3_bgm
{
	enum
	{
		MUSIC_NAME_MAX_LEN = 24,
	};
    BYTE        header;
    DWORD       dwVID;
	WORD		wRaceNum;
    char        szUserName[CHARACTER_NAME_MAX_LEN + 1];
	char        szBGMName[MUSIC_NAME_MAX_LEN + 1];
    long        lX, lY, lZ;
	BYTE		byEmpire;
	BYTE		bySkillGroup;
} TPacketGCMainCharacter3_BGM;

typedef struct packet_main_character4_bgm_vol
{
	enum
	{
		MUSIC_NAME_MAX_LEN = 24,
	};
    BYTE        header;
    DWORD       dwVID;
	WORD		wRaceNum;
    char        szUserName[CHARACTER_NAME_MAX_LEN + 1];
	char        szBGMName[MUSIC_NAME_MAX_LEN + 1];
	float		fBGMVol;
    long        lX, lY, lZ;
	BYTE		byEmpire;
	BYTE		bySkillGroup;
} TPacketGCMainCharacter4_BGM_VOL;

enum EPointTypes
{
    POINT_NONE,
    POINT_LEVEL,
    POINT_VOICE,
    POINT_EXP,
    POINT_NEXT_EXP,
    POINT_HP,
    POINT_MAX_HP,
    POINT_SP,
    POINT_MAX_SP,
    POINT_STAMINA,
    POINT_MAX_STAMINA,
    POINT_GOLD,
    POINT_ST,
    POINT_HT,
    POINT_DX,
    POINT_IQ,
    POINT_ATT_POWER,
    POINT_ATT_SPEED,
    POINT_EVADE_RATE,
    POINT_MOV_SPEED,
    POINT_DEF_GRADE,
	POINT_CASTING_SPEED,
	POINT_MAGIC_ATT_GRADE,
    POINT_MAGIC_DEF_GRADE,
    POINT_EMPIRE_POINT,
    POINT_LEVEL_STEP,
    POINT_STAT,
	POINT_SUB_SKILL,
	POINT_SKILL,
	POINT_MIN_ATK,
	POINT_MAX_ATK,
    POINT_PLAYTIME,
    POINT_HP_REGEN,
    POINT_SP_REGEN,
    POINT_BOW_DISTANCE,
    POINT_HP_RECOVERY,
    POINT_SP_RECOVERY,
    POINT_POISON_PCT,
    POINT_STUN_PCT,
    POINT_SLOW_PCT,
    POINT_CRITICAL_PCT,
    POINT_PENETRATE_PCT,
    POINT_CURSE_PCT,
    POINT_ATTBONUS_HUMAN,
    POINT_ATTBONUS_ANIMAL,
    POINT_ATTBONUS_ORC,
    POINT_ATTBONUS_MILGYO,
    POINT_ATTBONUS_UNDEAD,
    POINT_ATTBONUS_DEVIL,
    POINT_ATTBONUS_INSECT,
    POINT_ATTBONUS_FIRE,
    POINT_ATTBONUS_ICE,
    POINT_ATTBONUS_DESERT,
	POINT_ATTBONUS_MONSTER,
	POINT_ATTBONUS_WARRIOR,
	POINT_ATTBONUS_ASSASSIN,
	POINT_ATTBONUS_SURA,
	POINT_ATTBONUS_SHAMAN,
	POINT_ATTBONUS_TREE,
	POINT_RESIST_WARRIOR,
	POINT_RESIST_ASSASSIN,
	POINT_RESIST_SURA,
	POINT_RESIST_SHAMAN,
    POINT_STEAL_HP,
    POINT_STEAL_SP,
    POINT_MANA_BURN_PCT,
    POINT_DAMAGE_SP_RECOVER,
    POINT_BLOCK,
    POINT_DODGE,
    POINT_RESIST_SWORD,
    POINT_RESIST_TWOHAND,
    POINT_RESIST_DAGGER,
    POINT_RESIST_BELL,
    POINT_RESIST_FAN,
    POINT_RESIST_BOW,
    POINT_RESIST_FIRE,
    POINT_RESIST_ELEC,
    POINT_RESIST_MAGIC,
    POINT_RESIST_WIND,
    POINT_REFLECT_MELEE,
    POINT_REFLECT_CURSE,
    POINT_POISON_REDUCE,
    POINT_KILL_SP_RECOVER,
    POINT_EXP_DOUBLE_BONUS,
    POINT_GOLD_DOUBLE_BONUS,
    POINT_ITEM_DROP_BONUS,
    POINT_POTION_BONUS,
    POINT_KILL_HP_RECOVER,
    POINT_IMMUNE_STUN,
    POINT_IMMUNE_SLOW,
    POINT_IMMUNE_FALL,
    POINT_PARTY_ATT_GRADE,
    POINT_PARTY_DEF_GRADE,
    POINT_ATT_BONUS,
    POINT_DEF_BONUS,
    POINT_ATT_GRADE_BONUS,
    POINT_DEF_GRADE_BONUS,
    POINT_MAGIC_ATT_GRADE_BONUS,
    POINT_MAGIC_DEF_GRADE_BONUS,
    POINT_RESIST_NORMAL_DAMAGE,
	POINT_STAT_RESET_COUNT = 112,
    POINT_HORSE_SKILL = 113,
	POINT_MALL_ATTBONUS,
	POINT_MALL_DEFBONUS,
	POINT_MALL_EXPBONUS,
	POINT_MALL_ITEMBONUS,
	POINT_MALL_GOLDBONUS,
    POINT_MAX_HP_PCT,
    POINT_MAX_SP_PCT,
	POINT_SKILL_DAMAGE_BONUS,
	POINT_NORMAL_HIT_DAMAGE_BONUS,
    POINT_SKILL_DEFEND_BONUS,
    POINT_NORMAL_HIT_DEFEND_BONUS,
    POINT_PC_BANG_EXP_BONUS,
	POINT_PC_BANG_DROP_BONUS,
	POINT_ENERGY = 128,
	POINT_ENERGY_END_TIME = 129,
	POINT_COSTUME_ATTR_BONUS = 130,
	POINT_MAGIC_ATT_BONUS_PER = 131,
	POINT_MELEE_MAGIC_ATT_BONUS_PER = 132,
	POINT_RESIST_ICE = 133,
	POINT_RESIST_EARTH = 134,
	POINT_RESIST_DARK = 135,
	POINT_RESIST_CRITICAL = 136,
	POINT_RESIST_PENETRATE = 137,
#ifdef ENABLE_WOLFMAN
	POINT_BLEEDING_REDUCE,
	POINT_BLEEDING_PCT,
	POINT_ATTBONUS_WOLFMAN,
	POINT_RESIST_WOLFMAN,
	POINT_RESIST_CLAW,
#endif
	POINT_RESIST_MAGIC_REDUCTION,
	POINT_RESIST_HUMAN,
	POINT_ATTBONUS_ELEC,
	POINT_ATTBONUS_WIND,
	POINT_ATTBONUS_EARTH,
	POINT_ATTBONUS_DARK,
	POINT_MIN_WEP = 200,
	POINT_MAX_WEP,
	POINT_MIN_MAGIC_WEP,
	POINT_MAX_MAGIC_WEP,
	POINT_HIT_RATE,
};

typedef struct packet_points
{
    BYTE        header;
    long long	points[POINT_MAX_NUM];
} TPacketGCPoints;

typedef struct packet_point_change
{
    int         header;

	DWORD		dwVID;
	BYTE		Type;

	long    long	amount;
    long long		value;
} TPacketGCPointChange;

typedef struct packet_motion
{
	BYTE		header;
	DWORD		vid;
	DWORD		victim_vid;
	WORD		motion;
} TPacketGCMotion;

typedef struct packet_set_item
{
	BYTE		header;
	TItemPos	Cell;
	DWORD		vnum;
	WORD		count;
	long		alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
    TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
} TPacketGCItemSet;

typedef struct packet_set_item2
{
	BYTE		header;
	TItemPos	Cell;
	DWORD		vnum;
	WORD		count;
	DWORD		flags;
	DWORD		anti_flags;
	bool		highlight;
	long		alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
    TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
} TPacketGCItemSet2;

typedef struct packet_item_del
{
    BYTE        header;
    BYTE        pos;
} TPacketGCItemDel;

typedef struct packet_use_item
{
	BYTE		header;
	TItemPos	Cell;
	DWORD		ch_vid;
	DWORD		victim_vid;

	DWORD		vnum;
} TPacketGCItemUse;

typedef struct packet_update_item
{
	BYTE		header;
	TItemPos	Cell;
	WORD		count;
	long		alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
    TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
} TPacketGCItemUpdate;

typedef struct packet_ground_add_item
{
    BYTE	bHeader;
    long	lX;
	long	lY;
	long	lZ;
    DWORD	dwVID;
    DWORD	dwVnum;
	DWORD	dwCount;
} TPacketGCItemGroundAdd;

typedef struct packet_ground_del_item
{
	BYTE		header;
	DWORD		vid;
} TPacketGCItemGroundDel;

typedef struct packet_item_ownership
{
    BYTE        bHeader;
    DWORD       dwVID;
    char        szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCItemOwnership;

#if defined(__BL_DROP_DESTROY_TIME__)
typedef struct packet_item_destroy_time
{
	BYTE	bHeader;
	DWORD	dwVID;
	short	sDestroySeconds;
} TPacketGCItemDestroyTime;
#endif

typedef struct packet_quickslot_add
{
    BYTE        header;
    BYTE        pos;
	TQuickSlot	slot;
} TPacketGCQuickSlotAdd;

typedef struct packet_quickslot_del
{
    BYTE        header;
    BYTE        pos;
} TPacketGCQuickSlotDel;

typedef struct packet_quickslot_swap
{
    BYTE        header;
    BYTE        pos;
    BYTE        change_pos;
} TPacketGCQuickSlotSwap;

typedef struct packet_shop_start
{
	struct packet_shop_item		items[SHOP_HOST_ITEM_MAX_NUM];
} TPacketGCShopStart;

typedef struct packet_shop_start_ex
{
	typedef struct sub_packet_shop_tab 
	{
		char name[SHOP_TAB_NAME_MAX];
		BYTE coin_type;
		packet_shop_item items[SHOP_HOST_ITEM_MAX_NUM];
	} TSubPacketShopTab;
	DWORD owner_vid;
	BYTE shop_tab_count;
} TPacketGCShopStartEx;


typedef struct packet_shop_update_item
{
	BYTE						pos;
	struct packet_shop_item		item;
} TPacketGCShopUpdateItem;

typedef struct packet_shop_update_price
{
	long long	iElkAmount;
} TPacketGCShopUpdatePrice;

enum EPacketShopSubHeaders
{
	SHOP_SUBHEADER_GC_START,
    SHOP_SUBHEADER_GC_END,
	SHOP_SUBHEADER_GC_UPDATE_ITEM,
	SHOP_SUBHEADER_GC_UPDATE_PRICE,
    SHOP_SUBHEADER_GC_OK,
    SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY,
    SHOP_SUBHEADER_GC_SOLDOUT,
    SHOP_SUBHEADER_GC_INVENTORY_FULL,
    SHOP_SUBHEADER_GC_INVALID_POS,
	SHOP_SUBHEADER_GC_SOLD_OUT,
	SHOP_SUBHEADER_GC_START_EX,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_EX,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_ITEM,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_EXP,
};

typedef struct packet_shop
{
	BYTE        header;
	WORD		size;
	BYTE        subheader;
} TPacketGCShop;

typedef struct packet_exchange
{
    BYTE        header;
    BYTE        subheader;
    BYTE        is_me;
    long long	arg1;
    TItemPos	arg2;
    DWORD       arg3;
#ifdef WJ_ENABLE_TRADABLE_ICON
	TItemPos       arg4;
#endif
	long		alValues[ITEM_SOCKET_SLOT_MAX_NUM];
    TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
} TPacketGCExchange;

enum
{
    EXCHANGE_SUBHEADER_GC_START,
    EXCHANGE_SUBHEADER_GC_ITEM_ADD,
	EXCHANGE_SUBHEADER_GC_ITEM_DEL,
    EXCHANGE_SUBHEADER_GC_ELK_ADD,
    EXCHANGE_SUBHEADER_GC_ACCEPT,
    EXCHANGE_SUBHEADER_GC_END,
    EXCHANGE_SUBHEADER_GC_ALREADY,
    EXCHANGE_SUBHEADER_GC_LESS_ELK,
};

typedef struct packet_position
{
    BYTE        header;
	DWORD		vid;
    BYTE        position;
} TPacketGCPosition;

typedef struct packet_ping
{
	BYTE		header;
} TPacketGCPing;

typedef struct packet_pong
{
	BYTE		bHeader;
} TPacketCGPong;

typedef struct packet_script
{
    BYTE		header;
    WORD        size;
	BYTE		skin;
    WORD        src_size;
} TPacketGCScript;

typedef struct packet_target
{
    BYTE	header;
    DWORD	dwVID;
    BYTE	bHPPercent;
	int		iMinHP;
	int		iMaxHP;
} TPacketGCTarget;

#ifdef ENABLE_TARGET_INFO
typedef struct packet_target_info
{
	BYTE	header;
	DWORD	dwVID;
	DWORD	race;
	DWORD	dwVnum;
	WORD	count;
} TPacketGCTargetInfo;

typedef struct packet_target_info_load
{
	BYTE header;
	DWORD dwVID;
} TPacketCGTargetInfoLoad;
#endif

typedef struct packet_damage_info
{
	BYTE header;
	DWORD dwVID;
	BYTE flag;
	int  damage;
} TPacketGCDamageInfo;

typedef struct packet_mount
{
    BYTE        header;
    DWORD       vid;
    DWORD       mount_vid;
    BYTE        pos;
	DWORD		_x, _y;
} TPacketGCMount;

typedef struct packet_change_speed
{
	BYTE		header;
	DWORD		vid;
	WORD		moving_speed;
} TPacketGCChangeSpeed;

typedef struct packet_move
{	
	BYTE		bHeader;
	BYTE		bFunc;
	BYTE		bArg;
	BYTE		bRot;
	DWORD		dwVID;
	LONG		lX;
	LONG		lY;
	DWORD		dwTime;
	DWORD		dwDuration;
} TPacketGCMove;

enum
{
	QUEST_SEND_IS_BEGIN         = 1 << 0,
    QUEST_SEND_TITLE            = 1 << 1,
    QUEST_SEND_CLOCK_NAME       = 1 << 2,
    QUEST_SEND_CLOCK_VALUE      = 1 << 3,
    QUEST_SEND_COUNTER_NAME     = 1 << 4,
    QUEST_SEND_COUNTER_VALUE    = 1 << 5,
	QUEST_SEND_ICON_FILE		= 1 << 6,
};

typedef struct packet_quest_info
{
	BYTE header;
	WORD size;
	WORD index;
	BYTE flag;
} TPacketGCQuestInfo;

typedef struct packet_quest_confirm
{
    BYTE header;
    char msg[64+1];
    long timeout;
    DWORD requestPID;
} TPacketGCQuestConfirm;

typedef struct packet_attack
{
    BYTE        header;
    DWORD       dwVID;
    DWORD       dwVictimVID;
    BYTE        bType;
} TPacketGCAttack;

typedef struct packet_c2c
{
	BYTE		header;
	WORD		wSize;
} TPacketGCC2C;

typedef struct packetd_sync_position_element 
{ 
    DWORD       dwVID; 
    long        lX; 
    long        lY; 
} TPacketGCSyncPositionElement; 

typedef struct packetd_sync_position
{ 
    BYTE        bHeader; 
    WORD		wSize;
} TPacketGCSyncPosition; 

typedef struct packet_ownership 
{ 
    BYTE                bHeader; 
    DWORD               dwOwnerVID; 
    DWORD               dwVictimVID; 
} TPacketGCOwnership; 

#define	SKILL_MAX_NUM 255
typedef struct SPlayerSkill
{
	BYTE bMasterType;
	BYTE bLevel;
	time_t tNextRead;
} TPlayerSkill;

typedef struct packet_skill_level_new
{
	BYTE bHeader;
	TPlayerSkill skills[SKILL_MAX_NUM];
} TPacketGCSkillLevelNew;

typedef struct packet_fly
{
    BYTE        bHeader;
    BYTE        bType;
    DWORD       dwStartVID;
    DWORD       dwEndVID;
} TPacketGCCreateFly;

enum EPVPModes
{
	PVP_MODE_NONE,
    PVP_MODE_AGREE,
    PVP_MODE_FIGHT,
    PVP_MODE_REVENGE,
};

typedef struct packet_pvp
{
	BYTE		header;
	DWORD		dwVIDSrc;
	DWORD		dwVIDDst;
	BYTE		bMode;
} TPacketGCPVP;

typedef struct packet_skill_cooltime_end
{
	BYTE		header;
	BYTE		bSkill;
} TPacketGCSkillCoolTimeEnd;

typedef struct packet_warp
{
	BYTE			bHeader;
	LONG			lX;
	LONG			lY;
	LONG			lAddr;
	WORD			wPort;
} TPacketGCWarp;

typedef struct packet_party_invite
{
    BYTE header;
    DWORD leader_pid;
} TPacketGCPartyInvite;

typedef struct packet_party_add
{
    BYTE header;
    DWORD pid;
    char name[CHARACTER_NAME_MAX_LEN+1];
} TPacketGCPartyAdd;

typedef struct packet_party_update
{
    BYTE header;
    DWORD pid;
    BYTE state;
    BYTE percent_hp;
    short affects[PARTY_AFFECT_SLOT_MAX_NUM];
} TPacketGCPartyUpdate;

typedef struct packet_party_remove
{
    BYTE header;
    DWORD pid;
} TPacketGCPartyRemove;

typedef TPacketCGSafeboxCheckout TPacketGCSafeboxCheckout;
typedef TPacketCGSafeboxCheckin TPacketGCSafeboxCheckin;

typedef struct packet_safebox_wrong_password
{
    BYTE        bHeader;
} TPacketGCSafeboxWrongPassword;

typedef struct packet_safebox_size
{
	BYTE bHeader;
	BYTE bSize;
} TPacketGCSafeboxSize; 

typedef struct packet_safebox_money_change
{
    BYTE bHeader;
    DWORD dwMoney;
} TPacketGCSafeboxMoneyChange;

typedef struct command_empire
{
    BYTE        bHeader;
    BYTE        bEmpire;
} TPacketCGEmpire;

typedef struct packet_empire
{
    BYTE        bHeader;
    BYTE        bEmpire;
} TPacketGCEmpire;

enum
{
	FISHING_SUBHEADER_GC_START,
	FISHING_SUBHEADER_GC_STOP,
	FISHING_SUBHEADER_GC_REACT,
	FISHING_SUBHEADER_GC_SUCCESS,
	FISHING_SUBHEADER_GC_FAIL,
    FISHING_SUBHEADER_GC_FISH,
};

typedef struct packet_fishing
{
    BYTE header;
    BYTE subheader;
    DWORD info;
    BYTE dir;
} TPacketGCFishing;

typedef struct paryt_parameter
{
    BYTE        bHeader;
    BYTE        bDistributeMode;
} TPacketGCPartyParameter;

enum
{
    GUILD_SUBHEADER_GC_LOGIN,
	GUILD_SUBHEADER_GC_LOGOUT,
	GUILD_SUBHEADER_GC_LIST,
	GUILD_SUBHEADER_GC_GRADE,
	GUILD_SUBHEADER_GC_ADD,
	GUILD_SUBHEADER_GC_REMOVE,
	GUILD_SUBHEADER_GC_GRADE_NAME,
	GUILD_SUBHEADER_GC_GRADE_AUTH, 
	GUILD_SUBHEADER_GC_INFO,
	GUILD_SUBHEADER_GC_COMMENTS,
    GUILD_SUBHEADER_GC_CHANGE_EXP,
    GUILD_SUBHEADER_GC_CHANGE_MEMBER_GRADE,
	GUILD_SUBHEADER_GC_SKILL_INFO,
	GUILD_SUBHEADER_GC_CHANGE_MEMBER_GENERAL,
	GUILD_SUBHEADER_GC_GUILD_INVITE,
    GUILD_SUBHEADER_GC_WAR,
    GUILD_SUBHEADER_GC_GUILD_NAME,
    GUILD_SUBHEADER_GC_GUILD_WAR_LIST,
    GUILD_SUBHEADER_GC_GUILD_WAR_END_LIST,
    GUILD_SUBHEADER_GC_WAR_POINT,
	GUILD_SUBHEADER_GC_MONEY_CHANGE,
};

typedef struct packet_guild
{
    BYTE header;
    WORD size;
    BYTE subheader;
} TPacketGCGuild;

enum
{
    GUILD_AUTH_ADD_MEMBER       = (1 << 0),
    GUILD_AUTH_REMOVE_MEMBER    = (1 << 1),
    GUILD_AUTH_NOTICE           = (1 << 2),
    GUILD_AUTH_SKILL            = (1 << 3),
};

typedef struct packet_guild_sub_grade
{
	char grade_name[GUILD_GRADE_NAME_MAX_LEN+1];
	BYTE auth_flag;
} TPacketGCGuildSubGrade;

typedef struct packet_guild_sub_member
{
	DWORD pid;
	BYTE byGrade;
	BYTE byIsGeneral;
	BYTE byJob;
	BYTE byLevel;
	DWORD dwOffer;
	BYTE byNameFlag;
} TPacketGCGuildSubMember;

typedef struct packet_guild_sub_info
{
    WORD member_count;
    WORD max_member_count;
	DWORD guild_id;
    DWORD master_pid;
    DWORD exp;
    BYTE level;
    char name[GUILD_NAME_MAX_LEN+1];
	long long gold;
	BYTE hasLand;
} TPacketGCGuildInfo;

enum EGuildWarState
{
    GUILD_WAR_NONE,
    GUILD_WAR_SEND_DECLARE,
    GUILD_WAR_REFUSE,
    GUILD_WAR_RECV_DECLARE,
    GUILD_WAR_WAIT_START,
    GUILD_WAR_CANCEL,
    GUILD_WAR_ON_WAR,
    GUILD_WAR_END,
    GUILD_WAR_DURATION = 2*60*60,
};

typedef struct packet_guild_war
{
    DWORD       dwGuildSelf;
    DWORD       dwGuildOpp;
    BYTE        bType;
    BYTE        bWarState;
} TPacketGCGuildWar;

typedef struct SPacketGuildWarPoint
{
    DWORD dwGainGuildID;
    DWORD dwOpponentGuildID;
    long lPoint;
} TPacketGuildWarPoint;

enum
{
	DUNGEON_SUBHEADER_GC_TIME_ATTACK_START = 0,
	DUNGEON_SUBHEADER_GC_DESTINATION_POSITION = 1,
};

typedef struct packet_dungeon
{
	BYTE		bHeader;
    WORD		size;
    BYTE		subheader;
} TPacketGCDungeon;

typedef struct SPacketGCShopSign
{
    BYTE        bHeader;
    DWORD       dwVID;
    char        szSign[SHOP_SIGN_MAX_LEN + 1];
} TPacketGCShopSign;

typedef struct SPacketGCTime
{
    BYTE        bHeader;
    time_t      time;
} TPacketGCTime;

enum
{
    WALKMODE_RUN,
    WALKMODE_WALK,
};

typedef struct SPacketGCWalkMode
{
    BYTE        header;
    DWORD       vid;
    BYTE        mode;
} TPacketGCWalkMode;

typedef struct SPacketGCChangeSkillGroup
{
    BYTE        header;
    BYTE        skill_group;
} TPacketGCChangeSkillGroup;

struct TMaterial
{
    DWORD vnum;
    DWORD count;
};

typedef struct SRefineTable
{
    DWORD src_vnum;
    DWORD result_vnum;
    BYTE material_count;
    long long cost;
    int prob;
    TMaterial materials[REFINE_MATERIAL_MAX_NUM];
} TRefineTable;

typedef struct SPacketGCRefineInformation
{
	BYTE			header;
	WORD            pos;
	TRefineTable	refine_table;
} TPacketGCRefineInformation;

typedef struct SPacketGCRefineInformationNew
{
	BYTE			header;
	BYTE			type;
	WORD            pos;
	TRefineTable	refine_table;
} TPacketGCRefineInformationNew;

enum SPECIAL_EFFECT
{
	SE_NONE,
	SE_HPUP_RED,
	SE_SPUP_BLUE,
	SE_SPEEDUP_GREEN,
	SE_DXUP_PURPLE,
	SE_CRITICAL,
	SE_PENETRATE,
	SE_BLOCK,
	SE_DODGE,
	SE_CHINA_FIREWORK,
	SE_SPIN_TOP,
	SE_SUCCESS,
	SE_FAIL,
	SE_FR_SUCCESS,    
    SE_LEVELUP_ON_14_FOR_GERMANY,
    SE_LEVELUP_UNDER_15_FOR_GERMANY,
    SE_PERCENT_DAMAGE1,
    SE_PERCENT_DAMAGE2,
    SE_PERCENT_DAMAGE3,    
	SE_AUTO_HPUP,
	SE_AUTO_SPUP,
	SE_EQUIP_RAMADAN_RING,
	SE_EQUIP_HALLOWEEN_CANDY,
	SE_EQUIP_HAPPINESS_RING,
	SE_EQUIP_LOVE_PENDANT,
	SE_ACCE_SUCESS_ABSORB,
	SE_ACCE_EQUIP,
	SE_ACCE_BACK,
};

typedef struct SPacketGCSpecialEffect
{
    BYTE header;
    BYTE type;
    DWORD vid;
} TPacketGCSpecialEffect;

typedef struct SPacketGCNPCPosition
{
    BYTE header;
	WORD size;
    WORD count;
} TPacketGCNPCPosition;

struct TNPCPosition
{
    BYTE bType;
    char name[CHARACTER_NAME_MAX_LEN+1];
    long x;
    long y;
};

typedef struct SPacketGCChangeName
{
    BYTE header;
    DWORD pid;
    char name[CHARACTER_NAME_MAX_LEN+1];
} TPacketGCChangeName;

enum EBlockAction
{
    BLOCK_EXCHANGE              = (1 << 0),
    BLOCK_PARTY_INVITE          = (1 << 1),
    BLOCK_GUILD_INVITE          = (1 << 2),
    BLOCK_WHISPER               = (1 << 3),
    BLOCK_MESSENGER_INVITE      = (1 << 4),
    BLOCK_PARTY_REQUEST         = (1 << 5),
};

typedef struct packet_login_key
{
	BYTE	bHeader;
	DWORD	dwLoginKey;
} TPacketGCLoginKey;

typedef struct packet_auth_success
{
    BYTE        bHeader;
    DWORD       dwLoginKey;
    BYTE        bResult;
} TPacketGCAuthSuccess;

typedef struct packet_channel
{
    BYTE header;
    BYTE channel;
} TPacketGCChannel;

typedef struct SEquipmentItemSet
{
	DWORD   vnum;
	WORD    count;
	long    alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
} TEquipmentItemSet;

typedef struct packet_view_equip
{
    BYTE header;
	DWORD dwVID;
	TEquipmentItemSet equips[WEAR_MAX_NUM];
} TPacketGCViewEquip;

typedef struct
{
    DWORD       dwID;
    long        x, y;
    long        width, height;
    DWORD       dwGuildID;
} TLandPacketElement;

typedef struct packet_land_list
{
    BYTE        header;
    WORD        size;
} TPacketGCLandList;

typedef struct
{
    BYTE        bHeader;
    long        lID;
    char        szTargetName[32+1];
} TPacketGCTargetCreate;

enum
{
	CREATE_TARGET_TYPE_NONE,
	CREATE_TARGET_TYPE_LOCATION,
	CREATE_TARGET_TYPE_CHARACTER,
};

typedef struct
{
	BYTE		bHeader;
	long		lID;
	char		szTargetName[32+1];
	DWORD		dwVID;
	BYTE		byType;
} TPacketGCTargetCreateNew;

typedef struct
{
    BYTE        bHeader;
    long        lID;
    long        lX, lY;
} TPacketGCTargetUpdate;

typedef struct
{
    BYTE        bHeader;
    long        lID;
} TPacketGCTargetDelete;

typedef struct
{
    DWORD       dwType;
    BYTE        bPointIdxApplyOn;
    long        lApplyValue;
    DWORD       dwFlag;
    long        lDuration;
    long        lSPCost;
} TPacketAffectElement;

typedef struct 
{
    BYTE bHeader;
    TPacketAffectElement elem;
} TPacketGCAffectAdd;

typedef struct
{
    BYTE bHeader;
    DWORD dwType;
    BYTE bApplyOn;
} TPacketGCAffectRemove;

typedef struct packet_mall_open
{
	BYTE bHeader;
	BYTE bSize;
} TPacketGCMallOpen;

typedef struct packet_lover_info
{
	BYTE bHeader;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE byLovePoint;
} TPacketGCLoverInfo;

typedef struct packet_love_point_update
{
	BYTE bHeader;
	BYTE byLovePoint;
} TPacketGCLovePointUpdate;

typedef struct packet_dig_motion
{
    BYTE header;
    DWORD vid;
    DWORD target_vid;
	WORD count;
} TPacketGCDigMotion;

typedef struct SPacketGCOnTime
{
    BYTE header;
    int ontime;
} TPacketGCOnTime;

typedef struct SPacketGCResetOnTime
{
    BYTE header;
} TPacketGCResetOnTime;

typedef struct packet_state
{
	BYTE			bHeader;
	BYTE			bFunc;
	BYTE			bArg;
	BYTE			bRot;
	DWORD			dwVID;
	DWORD			dwTime;
	TPixelPosition	kPPos;
} TPacketCCState;

typedef struct packet_autoban_quiz
{
    BYTE bHeader;
	BYTE bDuration;
    BYTE bCaptcha[64*32];
    char szQuiz[256];
} TPacketGCAutoBanQuiz;

#ifdef _IMPROVED_PACKET_ENCRYPTION_
struct TPacketKeyAgreement
{
	static const int MAX_DATA_LEN = 256;
	BYTE bHeader;
	WORD wAgreedLength;
	WORD wDataLength;
	BYTE data[MAX_DATA_LEN];
};

struct TPacketKeyAgreementCompleted
{
	BYTE bHeader;
	BYTE data[3];
};
#endif

typedef struct SPacketGCSpecificEffect
{
	BYTE header;
	DWORD vid;
	char effect_file[128];
} TPacketGCSpecificEffect;

enum EDragonSoulRefineWindowRefineType
{
	DragonSoulRefineWindow_UPGRADE,
	DragonSoulRefineWindow_IMPROVEMENT,
	DragonSoulRefineWindow_REFINE,
};

enum EPacketCGDragonSoulSubHeaderType
{
	DS_SUB_HEADER_OPEN,
	DS_SUB_HEADER_CLOSE,
	DS_SUB_HEADER_DO_UPGRADE,
	DS_SUB_HEADER_DO_IMPROVEMENT,
	DS_SUB_HEADER_DO_REFINE,
	DS_SUB_HEADER_REFINE_FAIL,
	DS_SUB_HEADER_REFINE_FAIL_MAX_REFINE,
	DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL,
	DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MONEY,
	DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL,
	DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL,
	DS_SUB_HEADER_REFINE_SUCCEED,
};

typedef struct SPacketCGDragonSoulRefine
{
	SPacketCGDragonSoulRefine() : header (HEADER_CG_DRAGON_SOUL_REFINE), bSubType(0)
	{}
	BYTE header;
	BYTE bSubType;
	TItemPos ItemGrid[DS_REFINE_WINDOW_MAX_NUM];
} TPacketCGDragonSoulRefine;

typedef struct SPacketGCDragonSoulRefine
{
	SPacketGCDragonSoulRefine() : header(HEADER_GC_DRAGON_SOUL_REFINE), bSubType(0)
	{}
	BYTE header;
	BYTE bSubType;
	TItemPos Pos;
} TPacketGCDragonSoulRefine;

typedef struct SChannelStatus
{
	short nPort;
	BYTE bStatus;
} TChannelStatus;

enum
{
	ACCE_SUBHEADER_GC_SET_ITEM,
	ACCE_SUBHEADER_GC_CLEAR_SLOT,
	ACCE_SUBHEADER_GC_CLEAR_ALL,
};

enum
{
	ACCE_SUBHEADER_CG_REFINE_CHECKIN,
	ACCE_SUBHEADER_CG_REFINE_CHECKOUT,
	ACCE_SUBHEADER_CG_REFINE_ACCEPT,
	ACCE_SUBHEADER_CG_REFINE_CANCEL,

};

typedef struct packet_acce
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketGCAcce;

typedef struct command_acce
{
	BYTE header;
	BYTE subheader;
} TPacketCGAcce;

typedef struct command_acce_checkout
{
	BYTE        bAccePos;
} TPacketCGAcceCheckout;

typedef struct command_acce_checkin
{
	BYTE        bAccePos;
	TItemPos ItemPos;
} TPacketCGAcceCheckin;

typedef struct command_acce_accept
{
	BYTE windowType;
} TPacketCGAcceRefineAccept;

#ifdef ENABLE_SWITCHBOT
enum ECGSwitchbotSubheader
{
	SUBHEADER_CG_SWITCHBOT_START,
	SUBHEADER_CG_SWITCHBOT_STOP,
};

struct TPacketCGSwitchbot
{
	BYTE header;
	int size;
	BYTE subheader;
	BYTE slot;
};

enum EGCSwitchbotSubheader
{
	SUBHEADER_GC_SWITCHBOT_UPDATE,
	SUBHEADER_GC_SWITCHBOT_UPDATE_ITEM,
	SUBHEADER_GC_SWITCHBOT_SEND_ATTRIBUTE_INFORMATION,
};

struct TPacketGCSwitchbot
{
	BYTE header;
	int size;
	BYTE subheader;
	BYTE slot;
};

struct TSwitchbotUpdateItem
{
	BYTE	slot;
	BYTE	vnum;
	BYTE	count;
	long	alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
};
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
typedef struct SPacketCGBuffNPCCreate
{
	BYTE bHeader;
	BYTE	bAction;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE	bSex;
}TPacketCGBuffNPCCreate;

typedef struct SPacketCGBuffNPCAction
{
	BYTE	bHeader;
	BYTE	bAction;
	DWORD	dValue0;
	DWORD	dValue1;
	DWORD	dValue2;
	DWORD	dValue3;
} TPacketCGBuffNPCAction;

typedef struct SPacketGCBuffNPCAction
{
	BYTE	bHeader;
	BYTE	bAction;
	DWORD	dValue0;
	DWORD	dValue1;
	DWORD	dValue2;
	DWORD	dValue3;
} TPacketGCBuffNPCAction;

typedef struct SPacketGCBuffNPCUseSkill
{
	BYTE	bHeader;
	DWORD	dSkillVnum;
	DWORD	dVid;
	DWORD	dSkillLevel;
}TPacketGCBuffNPCUseSkill;
#endif

#ifdef ENABLE_HUNTING_SYSTEM
typedef struct SPacketCGHuntingAction
{
	BYTE	bHeader;
	BYTE	bAction;
	DWORD	dValue;
} TPacketGCHuntingAction;

typedef struct SPacketCGOpenWindowHuntingMain
{
	BYTE	bHeader;
	DWORD	dLevel;
	DWORD	dMonster;
	DWORD	dCurCount;
	DWORD	dDestCount;
	DWORD	dMoneyMin;
	DWORD	dMoneyMax;
	DWORD	dExpMin;
	DWORD	dExpMax;
	DWORD	dRaceItem;
	DWORD	dRaceItemCount;
} TPacketGCOpenWindowHuntingMain;

typedef struct SPacketCGOpenWindowHuntingSelect
{
	BYTE	bHeader;
	DWORD	dLevel;
	BYTE	bType;
	DWORD	dMonster;
	DWORD	dCount;
	DWORD	dMoneyMin;
	DWORD	dMoneyMax;
	DWORD	dExpMin;
	DWORD	dExpMax;
	DWORD	dRaceItem;
	DWORD	dRaceItemCount;
} TPacketGCOpenWindowHuntingSelect;

typedef struct SPacketGCOpenWindowReward
{
	BYTE	bHeader;
	DWORD	dLevel;
	DWORD	dReward;
	DWORD	dRewardCount;
	DWORD	dRandomReward;
	DWORD	dRandomRewardCount;
	DWORD	dMoney;
	BYTE	bExp;
} TPacketGCOpenWindowReward;

typedef struct SPacketGCUpdateHunting
{
	BYTE	bHeader;
	DWORD	dCount;
} TPacketGCUpdateHunting;

typedef struct SPacketGCReciveRandomItems
{
	BYTE	bHeader;
	BYTE	bWindow;
	DWORD	dItemVnum;
	DWORD	dItemCount;
} TPacketGCReciveRandomItems;
#endif

#ifdef ENABLE_ITEMSHOP
enum
{
	ITEMSHOP_CATEGORY_MAX_LEN = 24,
	ITEMSHOP_ICON_MAX_LEN = 24,
	ITEMSHOP_HASH_MAX_LEN = 32,
	PROMOTION_CODE_MAX_LEN = 48,
};

typedef struct SItemshopCategoryInfo
{
	char					category[ITEMSHOP_CATEGORY_MAX_LEN + 1];
	char					icon[ITEMSHOP_ICON_MAX_LEN + 1];
}TItemshopCategoryInfo;

typedef struct SPacketGCitemshopCategorySize
{
	DWORD size;
} TPacketGCitemshopCategorySize;

typedef struct SPacketGCItemshopItemSize
{
	DWORD size;
} TPacketGCItemshopItemSize;

typedef struct SPacketGCItemshopInfo
{
	BYTE header;
	DWORD subheader;
} TPacketGCItemshopInfo;

enum
{
	SUBHEADER_ITEMSHOP_REFRESH_ITEMS,
	SUBHEADER_ITEMSHOP_REFRESH_COINS,
	SUBHEADER_ITEMSHOP_REFRESH_SINGLE_ITEM,
	SUBHEADER_ITEMSHOP_REMOVE_SINGLE_ITEM,
	SUBHEADER_ITEMSHOP_ADD_SINGLE_ITEM,
	SUBHEADER_PROMOTION_CODE_REWARDS,
};

typedef struct SItemshopItemTable
{
	char					hash[ITEMSHOP_HASH_MAX_LEN + 1];
	unsigned long long		ullPrice;
	BYTE					byDiscountPercent;
	long long				llLimitCount;
	BYTE					byCategory;
	DWORD					dwVnum;
	WORD					wCount;
	time_t					end_time;
	long					alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute	aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
	BYTE					weight;
} TItemshopItemTable;

typedef struct SPromotionItemTable
{
	DWORD					dwVnum;
	WORD					wCount;
	long					alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute	aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
} TPromotionItemTable;

typedef struct SPacketCGBuyItemshopItem
{
	BYTE	header;
	char	hash[ITEMSHOP_HASH_MAX_LEN + 1];
	WORD	wCount;
} TPacketCGBuyItemshopItem;

typedef struct SPacketCGRedeemPromotionCode
{
	BYTE	header;
	char	promotion_code[PROMOTION_CODE_MAX_LEN + 1];
} TPacketCGRedeemPromotionCode;
#endif

#pragma pack(pop)
