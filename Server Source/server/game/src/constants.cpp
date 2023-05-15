#include "stdafx.h"
#include "char.h"

TJobInitialPoints JobInitialPoints[JOB_MAX_NUM] = 
{
	{ 6, 4, 3, 3, 600, 200, 40, 20, 36, 44, 18, 22, 800, 5, 1, 3 },
	{ 4, 3, 6, 3, 650, 200, 40, 20, 36, 44, 18, 22, 800, 5, 1, 3 },
	{ 5, 3, 3, 5, 650, 200, 40, 20, 36, 44, 18, 22, 800, 5, 1, 3 },
	{ 3, 4, 3, 6, 700, 200, 40, 20, 36, 44, 18, 22, 800, 5, 1, 3 },
#ifdef ENABLE_WOLFMAN
	{ 2, 6, 6, 2, 600, 200, 40, 20, 36, 44, 18, 22, 800, 5, 1, 3 },
#endif
};

const TMobRankStat MobRankStats[MOB_RANK_MAX_NUM] =
{
	{  20,  },
	{  20,  },
	{  25,  },
	{  30,  },
	{  50,  },
	{ 100,  }
};

TBattleTypeStat BattleTypeStats[BATTLE_TYPE_MAX_NUM] =
{
	{	  0,	  0,	  0,	-10	},
	{	 10,	-20,	-10,	-15	},
	{	 -5,	 -5,	 10,	 10	},
	{	  0,	  0,	  0,	  0	},
	{	 10,	-10,	  0,	-15	},
	{	-10,	 10,	-10,	  0	},
	{	 20,	-20,	  0,	-10	},
	{	-20,	 20,	-10,	  0	},
};

const DWORD exp_table[PLAYER_EXP_TABLE_MAX + 1] =
{
	0,
	100,
	150,
	260,
	380,
	600,
	1300,
	3300,
	5700,
	8700,
	12800,
	18000,
	25000,
	36000,
	52000,
	73000,
	100000,
	125000,
	160000,
	220000,
	280000,
	370000,
	540000,
	670000,
	880000,
	1000000,
	1237000,
	1418000,
	1624000,
	1857000,
	2122000,
	2421000,
	2761000,
	3145000,
	3580000,
	4073000,
	4632000,
	5194000,
	5717000,
	6264000,
	6837000,
	7600000,
	8274000,
	8990000,
	9753000,
	10560000,
	11410000,
	12320000,
	13270000,
	14280000,
	15340000,
	16870000,
	18960000,
	19980000,
	21420000,
	22930000,
	24530000,
	26200000,
	27960000,
	29800000,
	32780000,
	36060000,
	39670000,
	43640000,
	48000000,
	52800000,
	58080000,
	63890000,
	70280000,
	77310000,
	85040000,
	93540000,
	102900000,
	113200000,
	124500000,
	137000000,
	150700000,
	165700000,
	236990000,
	260650000,
	286780000,
	315380000,
	346970000,
	381680000,
	419770000,
	461760000,
	508040000,
	558740000,
	614640000,
	676130000,
	743730000,
	1041222000,
	1145344200,
	1259878620,
	1385866482,
	1524453130,
	1676898443,
	1844588288,
	2029047116,
	2100000000,
	2100000000,
	2100000000,		
	2100000000,		
	2100000000,		
	2100000000,		
	2100000000,
	2100000000,		
	2100000000,		
	2100000000,		
	2100000000,		
	2100000000,	
	2100000000,		
	2100000000,		
	2100000000,		
	2100000000,		
	2100000000,
	2100000000,		
	2100000000,		
	2100000000,		
	2100000000,		
	2100000000,
	2500000000,	
	2500000000,	
	2500000000,	
	2500000000,	
	2500000000,	
	2500000000,	
	2500000000,	
	2500000000,	
	2500000000,	
	2500000000,
	2500000000,	
	2500000000,	
	2500000000,	
	2500000000,	
	2500000000,	
	2500000000,	
	2500000000,	
	2500000000,	
	2500000000,	
	2500000000,
	2500000000,	
	2500000000,	
	2500000000,	
	2500000000,	
	2500000000,	
	2500000000,	
	2500000000,	
	2500000000,	
	2500000000,	
	2500000000,
};

const int aiPercentByDeltaLevForBoss[MAX_EXP_DELTA_OF_LEV] =
{
	1,
	3,
	5,
	7,
	15,
	30,
	60,
	90,
	91,
	92,
	93,
	94,
	95,
	97,
	99,
	100,
	105,
	110,
	115,
	120,
	125,
	130,
	135,
	140,
	145,
	150,
	155,
	160,
	165,
	170,
	180
};

const int aiPercentByDeltaLev[MAX_EXP_DELTA_OF_LEV] =
{
	1,
	5,
	10,
	20,
	30,
	50,
	70,
	80,
	85,
	90,
	92,
	94,
	96,
	98,
	100, 
	100,
	105,
	110,
	115,
	120,
	125,
	130,
	135,
	140,
	145,
	150,
	155,
	160,
	165,
	170,
	180,
};

const DWORD party_exp_distribute_table[PLAYER_MAX_LEVEL_CONST + 1] = 
{
	0,
	10,		10,		10,		10,		15,		15,		20,		25,		30,		40,
	50,		60,		80,		100,	120,	140,	160,	184,	210,	240,
	270,	300,	330,	360,	390,	420,	450,	480,	510,	550,
	600,	640,	700,	760,	820,	880,	940,	1000,	1100,	1180,
	1260,	1320,	1380,	1440,	1500,	1560,	1620,	1680,	1740,	1800,
	1860,	1920,	2000,	2100,	2200,	2300,	2450,	2600,	2750,	2900,
	3050,	3200,	3350,	3500,	3650,	3800,	3950,	4100,	4250,	4400,
	4600,	4800,	5000,	5200,	5400,	5600,	5800,	6000,	6200,	6400,
	6600,	6900,	7100,	7300,	7600,	7800,	8000,	8300,	8500,	8800,
	9000,	9000,	9000,	9000,	9000,	9000,	9000,	9000,	9000,	9000,
	10000,	10000,	10000,	10000,	10000,	10000,	10000,	10000,	10000,	10000,
	12000,	12000,	12000,	12000,	12000,	12000,	12000,	12000,	12000,	12000,
};

Coord aArroundCoords[ARROUND_COORD_MAX_NUM] = 
{
	{	     0,	      0	    },
	{        0,      50     },
	{       35,     35      },
	{       50,     -0      },
	{       35,     -35     },
	{       0,      -50     },
	{       -35,    -35     },
	{       -50,    0       },
	{       -35,    35      },
	{       0,      100     },
	{       71,     71      },
	{       100,    -0      },
	{       71,     -71     },
	{       0,      -100    },
	{       -71,    -71     },
	{       -100,   0       },
	{       -71,    71      },
	{       0,      150     },
	{       106,    106     },
	{       150,    -0      },
	{       106,    -106    },
	{       0,      -150    },
	{       -106,   -106    },
	{       -150,   0       },
	{       -106,   106     },
	{       0,      200     },
	{       141,    141     },
	{       200,    -0      },
	{       141,    -141    },
	{       0,      -200    },
	{       -141,   -141    },
	{       -200,   0       },
	{       -141,   141     },
	{       0,      250     },
	{       177,    177     },
	{       250,    -0      },
	{       177,    -177    },
	{       0,      -250    },
	{       -177,   -177    },
	{       -250,   0       },
	{       -177,   177     },
	{       0,      300     },
	{       212,    212     },
	{       300,    -0      },
	{       212,    -212    },
	{       0,      -300    },
	{       -212,   -212    },
	{       -300,   0       },
	{       -212,   212     },
	{       0,      350     },
	{       247,    247     },
	{       350,    -0      },
	{       247,    -247    },
	{       0,      -350    },
	{       -247,   -247    },
	{       -350,   0       },
	{       -247,   247     },
	{       0,      400     },
	{       283,    283     },
	{       400,    -0      },
	{       283,    -283    },
	{       0,      -400    },
	{       -283,   -283    },
	{       -400,   0       },
	{       -283,   283     },
	{       0,      450     },
	{       318,    318     },
	{       450,    -0      },
	{       318,    -318    },
	{       0,      -450    },
	{       -318,   -318    },
	{       -450,   0       },
	{       -318,   318     },
	{       0,      500     },
	{       354,    354     },
	{       500,    -0      },
	{       354,    -354    },
	{       0,      -500    },
	{       -354,   -354    },
	{       -500,   0       },
	{       -354,   354     },
	{       0,      550     },
	{       389,    389     },
	{       550,    -0      },
	{       389,    -389    },
	{       0,      -550    },
	{       -389,   -389    },
	{       -550,   0       },
	{       -389,   389     },
	{       0,      600     },
	{       424,    424     },
	{       600,    -0      },
	{       424,    -424    },
	{       0,      -600    },
	{       -424,   -424    },
	{       -600,   0       },
	{       -424,   424     },
	{       0,      650     },
	{       460,    460     },
	{       650,    -0      },
	{       460,    -460    },
	{       0,      -650    },
	{       -460,   -460    },
	{       -650,   0       },
	{       -460,   460     },
	{       0,      700     },
	{       495,    495     },
	{       700,    -0      },
	{       495,    -495    },
	{       0,      -700    },
	{       -495,   -495    },
	{       -700,   0       },
	{       -495,   495     },
	{       0,      750     },
	{       530,    530     },
	{       750,    -0      },
	{       530,    -530    },
	{       0,      -750    },
	{       -530,   -530    },
	{       -750,   0       },
	{       -530,   530     },
	{       0,      800     },
	{       566,    566     },
	{       800,    -0      },
	{       566,    -566    },
	{       0,      -800    },
	{       -566,   -566    },
	{       -800,   0       },
	{       -566,   566     },
	{       0,      850     },
	{       601,    601     },
	{       850,    -0      },
	{       601,    -601    },
	{       0,      -850    },
	{       -601,   -601    },
	{       -850,   0       },
	{       -601,   601     },
	{       0,      900     },
	{       636,    636     },
	{       900,    -0      },
	{       636,    -636    },
	{       0,      -900    },
	{       -636,   -636    },
	{       -900,   0       },
	{       -636,   636     },
	{       0,      950     },
	{       672,    672     },
	{       950,    -0      },
	{       672,    -672    },
	{       0,      -950    },
	{       -672,   -672    },
	{       -950,   0       },
	{       -672,   672     },
	{       0,      1000    },
	{       707,    707     },
	{       1000,   -0      },
	{       707,    -707    },
	{       0,      -1000   },
	{       -707,   -707    },
	{       -1000,  0       },
	{       -707,   707     },
};

const DWORD guild_exp_table[GUILD_MAX_LEVEL+1] =
{
	0,
	6000UL,
	18000UL,
	36000UL,
	64000UL,
	94000UL,
	130000UL,
	172000UL,
	220000UL,
	274000UL,
	334000UL,
	400000UL,
	600000UL,
	840000UL,
	1120000UL,
	1440000UL,
	1800000UL,
	2600000UL,
	3200000UL,
	4000000UL,
	16800000UL
};

const int aiMobEnchantApplyIdx[MOB_ENCHANTS_MAX_NUM] =
{
	APPLY_CURSE_PCT,
	APPLY_SLOW_PCT,
	APPLY_POISON_PCT,
	APPLY_STUN_PCT,
	APPLY_CRITICAL_PCT,
	APPLY_PENETRATE_PCT,
};

const int aiMobResistsApplyIdx[MOB_RESISTS_MAX_NUM] =
{
	APPLY_RESIST_SWORD,
	APPLY_RESIST_TWOHAND,
	APPLY_RESIST_DAGGER,
	APPLY_RESIST_BELL,
	APPLY_RESIST_FAN,
	APPLY_RESIST_BOW,
#ifdef ENABLE_WOLFMAN
	APPLY_RESIST_CLAW,
#endif
	APPLY_RESIST_FIRE,
	APPLY_RESIST_ELEC,
	APPLY_RESIST_MAGIC,
	APPLY_RESIST_WIND,
	APPLY_POISON_REDUCE,
#ifdef ENABLE_WOLFMAN
	APPLY_BLEEDING_REDUCE,
#endif
};

const int aiSocketPercentByQty[5][4] =
{
	{  0,  0,  0,  0 },
	{  3,  0,  0,  0 },
	{ 10,  1,  0,  0 },
	{ 15, 10,  1,  0 },
	{ 20, 15, 10,  1 }
};

const int aiWeaponSocketQty[WEAPON_NUM_TYPES] =
{
	// 3,
	// 3,
	// 3,
	// 3,
	// 3,
	// 3,
#ifdef ENABLE_EXTENDED_SOCKETS
	6, // WEAPON_SWORD,
	6, // WEAPON_DAGGER,
	6, // WEAPON_BOW,
	6, // WEAPON_TWO_HANDED,
	6, // WEAPON_BELL,
	6, // WEAPON_FAN,
#else
	3, // WEAPON_SWORD,
	3, // WEAPON_DAGGER,
	3, // WEAPON_BOW,
	3, // WEAPON_TWO_HANDED,
	3, // WEAPON_BELL,
	3, // WEAPON_FAN,
#endif
	0,
	0,
#ifdef ENABLE_WOLFMAN
	3,
#endif
	0,
};

const int aiArmorSocketQty[ARMOR_NUM_TYPES] =
{
#ifdef ENABLE_EXTENDED_SOCKETS
	6, // ARMOR_BODY,
#else
	3, // ARMOR_BODY,
#endif
	1, // ARMOR_HEAD,
	1, // ARMOR_SHIELD,
	0, // ARMOR_WRIST,
	0, // ARMOR_FOOTS,
	0  // ARMOR_ACCESSORY
};

TItemAttrMap g_map_itemAttr;
TItemAttrMap g_map_itemRare;

const TApplyInfo aApplyInfo[MAX_APPLY_NUM] =
{
	{ POINT_NONE,						},
	{ POINT_MAX_HP,						},
	{ POINT_MAX_SP,						},
	{ POINT_HT,							},
	{ POINT_IQ,							},
	{ POINT_ST,							},
	{ POINT_DX,							},
	{ POINT_ATT_SPEED,					},
	{ POINT_MOV_SPEED,					},
	{ POINT_CASTING_SPEED,				},
	{ POINT_HP_REGEN,					},
	{ POINT_SP_REGEN,					},
	{ POINT_POISON_PCT,					},
	{ POINT_STUN_PCT,					},
	{ POINT_SLOW_PCT,					},
	{ POINT_CRITICAL_PCT,				},
	{ POINT_PENETRATE_PCT,				},
	{ POINT_ATTBONUS_HUMAN,				},
	{ POINT_ATTBONUS_ANIMAL,	        },
	{ POINT_ATTBONUS_ORC,				},
	{ POINT_ATTBONUS_MILGYO,	        },
	{ POINT_ATTBONUS_UNDEAD,	        },
	{ POINT_ATTBONUS_DEVIL,				},
	{ POINT_STEAL_HP,					},
	{ POINT_STEAL_SP,					},
	{ POINT_MANA_BURN_PCT,				},
	{ POINT_DAMAGE_SP_RECOVER,	        },
	{ POINT_BLOCK,						},
	{ POINT_DODGE,						},
	{ POINT_RESIST_SWORD,				},
	{ POINT_RESIST_TWOHAND,				},
	{ POINT_RESIST_DAGGER,				},
	{ POINT_RESIST_BELL,				},
	{ POINT_RESIST_FAN,					},
	{ POINT_RESIST_BOW,					},
	{ POINT_RESIST_FIRE,				},
	{ POINT_RESIST_ELEC,				},
	{ POINT_RESIST_MAGIC,				},
	{ POINT_RESIST_WIND,				},
	{ POINT_REFLECT_MELEE,				},
	{ POINT_REFLECT_CURSE,				},
	{ POINT_POISON_REDUCE,				},
	{ POINT_KILL_SP_RECOVER,	        },
	{ POINT_EXP_DOUBLE_BONUS,	        },
	{ POINT_GOLD_DOUBLE_BONUS,	        },
	{ POINT_ITEM_DROP_BONUS,	        },
	{ POINT_POTION_BONUS,				},
	{ POINT_KILL_HP_RECOVERY,	        },
	{ POINT_IMMUNE_STUN,				},
	{ POINT_IMMUNE_SLOW,				},
	{ POINT_IMMUNE_FALL,				},
	{ POINT_NONE,						},
	{ POINT_BOW_DISTANCE,				},
	{ POINT_ATT_GRADE_BONUS,	        },
	{ POINT_DEF_GRADE_BONUS,	        },
	{ POINT_MAGIC_ATT_GRADE_BONUS,      },
	{ POINT_MAGIC_DEF_GRADE_BONUS,      },
	{ POINT_CURSE_PCT,					},
	{ POINT_MAX_STAMINA					},
	{ POINT_ATTBONUS_WARRIOR			},
	{ POINT_ATTBONUS_ASSASSIN			},
	{ POINT_ATTBONUS_SURA				},
	{ POINT_ATTBONUS_SHAMAN				},
	{ POINT_ATTBONUS_MONSTER			},
	{ POINT_MALL_ATTBONUS				},
	{ POINT_MALL_DEFBONUS				},
	{ POINT_MALL_EXPBONUS				},
	{ POINT_MALL_ITEMBONUS				},
	{ POINT_MALL_GOLDBONUS				},
	{ POINT_MAX_HP_PCT					},
	{ POINT_MAX_SP_PCT					},
	{ POINT_SKILL_DAMAGE_BONUS			},
	{ POINT_NORMAL_HIT_DAMAGE_BONUS		},
	{ POINT_SKILL_DEFEND_BONUS			},
	{ POINT_NORMAL_HIT_DEFEND_BONUS		},
	{ POINT_PC_BANG_EXP_BONUS			},
	{ POINT_PC_BANG_DROP_BONUS			},
	{ POINT_NONE,						},
	{ POINT_RESIST_WARRIOR,				},
	{ POINT_RESIST_ASSASSIN,			},
	{ POINT_RESIST_SURA,				},
	{ POINT_RESIST_SHAMAN,				},
	{ POINT_ENERGY						},
	{ POINT_DEF_GRADE					},
	{ POINT_COSTUME_ATTR_BONUS			},
	{ POINT_MAGIC_ATT_BONUS_PER			},
	{ POINT_MELEE_MAGIC_ATT_BONUS_PER	},
	{ POINT_RESIST_ICE,					},
	{ POINT_RESIST_EARTH,				},
	{ POINT_RESIST_DARK,				},
	{ POINT_RESIST_CRITICAL,			},
	{ POINT_RESIST_PENETRATE,			},
#ifdef ENABLE_WOLFMAN
	{ POINT_BLEEDING_REDUCE,			},
	{ POINT_BLEEDING_PCT,				},
	{ POINT_ATTBONUS_WOLFMAN,			},
	{ POINT_RESIST_WOLFMAN,				},
	{ POINT_RESIST_CLAW,				},
#endif
	{ POINT_NONE,						},
	{ POINT_RESIST_MAGIC_REDUCTION,		},
	{ POINT_RESIST_HUMAN,				},
	{ POINT_ENCHANT_ELEC,				},
	{ POINT_ENCHANT_FIRE,				},
	{ POINT_ENCHANT_ICE,				},
	{ POINT_ENCHANT_WIND,				},
	{ POINT_ENCHANT_EARTH,				},
	{ POINT_ENCHANT_DARK,				},
};

const int aiItemMagicAttributePercentHigh[ITEM_ATTRIBUTE_MAX_LEVEL] =
{
	30, 40, 20, 8, 2
};

const int aiItemMagicAttributePercentLow[ITEM_ATTRIBUTE_MAX_LEVEL] =
{
	50, 40, 10, 0, 0
};

const int aiItemAttributeAddPercent[ITEM_ATTRIBUTE_MAX_NUM] =
{
	100, 80, 60, 50, 30, 0, 0, 
};

const int aiExpLossPercents[PLAYER_EXP_TABLE_MAX + 1] =
{
	0,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 4,
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	4, 4, 4, 4, 4, 4, 4, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

const int aiSkillBookCountForLevelUp[10] =
{
	3, 3, 3, 3, 3, 4, 4, 5, 5, 6
};

const int aiGrandMasterSkillBookCountForLevelUp[10] =
{
	3, 3, 5, 5, 7, 7, 10, 10, 10, 20, 
};

const int aiGrandMasterSkillBookMinCount[10] = 
{
	1, 1, 1, 2,  2,  3,  3,  4,  5,  6
};

const int aiGrandMasterSkillBookMaxCount[10] = 
{
	5,  7,  9, 11, 13, 15, 20, 25, 30, 35
};

const int CHN_aiPartyBonusExpPercentByMemberCount[9] =
{
	0, 0, 12, 18, 26, 40, 53, 70, 100
};

const int aiChainLightningCountBySkillLevel[SKILL_MAX_LEVEL+1] =
{
	0,
	2,
	2,
	2,
	2,
	2,
	2,
	2,
	2,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	3,
	4,
	4,
	4,
	4,
	4,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
	5,
};

const SStoneDropInfo aStoneDrop[STONE_INFO_MAX_NUM] =
{
	{8005,	60,	{30,	30,	30,	9,	1}	},
	{8006,	60,	{28,	29,	31,	11,	1}	},
	{8007,	60,	{24,	29,	32,	13,	2}	},
	{8008,	60,	{22,	28,	33,	15,	2}	},
	{8009,	60,	{21,	27,	33,	17,	2}	},
	{8010,	60,	{18,	26,	34,	20,	2}	},
	{8011,	60,	{14,	26,	35,	22,	3}	},
	{8012,	60,	{10,	26,	37,	24,	3}	},
	{8013,	60,	{2,		26,	40,	29,	3}	},
	{8014,	60,	{0,		26,	41,	30,	3}	},
};

const char * c_apszEmpireNames[EMPIRE_MAX_NUM] =
{
	"All Kingdoms",
	"Pandemonia Kingdom",
	"Asmodia Kingdom",
	"Elgoria Kingdom"
};

const char * c_apszPrivNames[MAX_PRIV_NUM] =
{
	"",
	"Item drop rate",
	"Yang drop rate",
	"µ· ´ë¹ÚÀÌ ³ª¿Ã È®·ü",
	"Experience rate",
};

const int aiPolymorphPowerByLevel[SKILL_MAX_LEVEL + 1] =
{
	10,
	11,
	11,
	12,
	13,
	13,
	14,
	15,
	16,
	17,
	18,
	19,
	20,
	22,
	23,
	24,
	26,
	27,
	29,
	31,
	33,
	35,
	37,
	39,
	41,
	44,
	46,
	49,
	52,
	55,
	59,
	62,
	66,
	70,
	74,
	79,
	84,
	89,
	94,
	100,
};

TGuildWarInfo KOR_aGuildWarInfo[GUILD_WAR_TYPE_MAX_NUM] =
{
	{ 0,	0,	0,		0,	0,	0	},
	{ 110,	0,	100,	50,	0,	100	},
	{ 111,	0,	100,    50,	0,	10	},
};

const int aiAccessorySocketAddPct[ITEM_ACCESSORY_SOCKET_MAX_NUM] =
{
	50, 50, 50
};

const int aiAccessorySocketEffectivePct[ITEM_ACCESSORY_SOCKET_MAX_NUM + 1] = 
{
	0, 10, 20, 40
};

const int aiAccessorySocketDegradeTime[ITEM_ACCESSORY_SOCKET_MAX_NUM + 1] =
{
	0, 3600 * 24, 3600 * 12, 3600 * 6
};

const int aiAccessorySocketPutPct[ITEM_ACCESSORY_SOCKET_MAX_NUM + 1] =
{
	90, 80, 70, 0
};

#include "../../common/length.h"

typedef struct SValueName
{
    const char *	c_pszName;
    long		lValue;
} TValueName;

TValueName c_aApplyTypeNames[] =
{
    { "STR",						APPLY_STR						},
    { "DEX",						APPLY_DEX						},
    { "CON",						APPLY_CON						},
    { "INT",						APPLY_INT						},
    { "MAX_HP",						APPLY_MAX_HP					},
    { "MAX_SP",						APPLY_MAX_SP					},
    { "MAX_STAMINA",				APPLY_MAX_STAMINA				},
    { "POISON_REDUCE",				APPLY_POISON_REDUCE				},
	{ "EXP_DOUBLE_BONUS",			APPLY_EXP_DOUBLE_BONUS			},
    { "GOLD_DOUBLE_BONUS",			APPLY_GOLD_DOUBLE_BONUS			},
    { "ITEM_DROP_BONUS",			APPLY_ITEM_DROP_BONUS			},
    { "HP_REGEN",					APPLY_HP_REGEN					},
    { "SP_REGEN",					APPLY_SP_REGEN					},
    { "ATTACK_SPEED",				APPLY_ATT_SPEED					},
    { "MOVE_SPEED",					APPLY_MOV_SPEED					},
    { "CAST_SPEED",					APPLY_CAST_SPEED				},
    { "ATT_BONUS",					APPLY_ATT_GRADE_BONUS			},
    { "DEF_BONUS",					APPLY_DEF_GRADE_BONUS			},
    { "MAGIC_ATT_GRADE",			APPLY_MAGIC_ATT_GRADE			},
    { "MAGIC_DEF_GRADE",			APPLY_MAGIC_DEF_GRADE			},
    { "SKILL",						APPLY_SKILL						},
    { "ATTBONUS_ANIMAL",			APPLY_ATTBONUS_ANIMAL			},
    { "ATTBONUS_UNDEAD",			APPLY_ATTBONUS_UNDEAD			},
    { "ATTBONUS_DEVIL",				APPLY_ATTBONUS_DEVIL			},
    { "ATTBONUS_HUMAN",				APPLY_ATTBONUS_HUMAN			},
    { "ADD_BOW_DISTANCE",			APPLY_BOW_DISTANCE				},
    { "DODGE",						APPLY_DODGE						},
    { "BLOCK",						APPLY_BLOCK						},
    { "RESIST_SWORD",				APPLY_RESIST_SWORD				},
    { "RESIST_TWOHAND",				APPLY_RESIST_TWOHAND			},
    { "RESIST_DAGGER",				APPLY_RESIST_DAGGER				},
    { "RESIST_BELL",				APPLY_RESIST_BELL				},
    { "RESIST_FAN",					APPLY_RESIST_FAN				},
    { "RESIST_BOW",					APPLY_RESIST_BOW				},
    { "RESIST_FIRE",				APPLY_RESIST_FIRE				},
    { "RESIST_ELEC",				APPLY_RESIST_ELEC				},
    { "RESIST_MAGIC",				APPLY_RESIST_MAGIC				},
    { "RESIST_WIND",				APPLY_RESIST_WIND				},
	{ "REFLECT_MELEE",				APPLY_REFLECT_MELEE				},
	{ "REFLECT_CURSE",				APPLY_REFLECT_CURSE				},
    { "RESIST_ICE",					APPLY_RESIST_ICE				},
    { "RESIST_EARTH",				APPLY_RESIST_EARTH				},
    { "RESIST_DARK",				APPLY_RESIST_DARK				},
    { "RESIST_CRITICAL",			APPLY_ANTI_CRITICAL_PCT			},
    { "RESIST_PENETRATE",			APPLY_ANTI_PENETRATE_PCT		},
    { "POISON",						APPLY_POISON_PCT				},
    { "SLOW",						APPLY_SLOW_PCT					},
    { "STUN",						APPLY_STUN_PCT					},
    { "STEAL_HP",					APPLY_STEAL_HP					},
    { "STEAL_SP",					APPLY_STEAL_SP					},
    { "MANA_BURN_PCT",				APPLY_MANA_BURN_PCT				},
    { "CRITICAL",					APPLY_CRITICAL_PCT				},
    { "PENETRATE",					APPLY_PENETRATE_PCT				},
    { "KILL_SP_RECOVER",			APPLY_KILL_SP_RECOVER			},
    { "KILL_HP_RECOVER",			APPLY_KILL_HP_RECOVER			},
    { "PENETRATE_PCT",				APPLY_PENETRATE_PCT				},
    { "CRITICAL_PCT",				APPLY_CRITICAL_PCT				},
    { "POISON_PCT",					APPLY_POISON_PCT				},
    { "STUN_PCT",					APPLY_STUN_PCT					},
    { "ATT_BONUS_TO_WARRIOR",		APPLY_ATTBONUS_WARRIOR			},
    { "ATT_BONUS_TO_ASSASSIN",		APPLY_ATTBONUS_ASSASSIN			},
    { "ATT_BONUS_TO_SURA",			APPLY_ATTBONUS_SURA				},
    { "ATT_BONUS_TO_SHAMAN",		APPLY_ATTBONUS_SHAMAN			},
    { "ATT_BONUS_TO_MONSTER",		APPLY_ATTBONUS_MONSTER			},
    { "ATT_BONUS_TO_MOB",			APPLY_ATTBONUS_MONSTER			},
    { "MALL_ATTBONUS",				APPLY_MALL_ATTBONUS				},
    { "MALL_EXPBONUS",				APPLY_MALL_EXPBONUS				},
    { "MALL_DEFBONUS",				APPLY_MALL_DEFBONUS				},  
    { "MALL_ITEMBONUS",				APPLY_MALL_ITEMBONUS			},
    { "MALL_GOLDBONUS",				APPLY_MALL_GOLDBONUS			},
    { "MAX_HP_PCT",					APPLY_MAX_HP_PCT				},
    { "MAX_SP_PCT",					APPLY_MAX_SP_PCT				},
    { "SKILL_DAMAGE_BONUS",			APPLY_SKILL_DAMAGE_BONUS		},
    { "NORMAL_HIT_DAMAGE_BONUS",	APPLY_NORMAL_HIT_DAMAGE_BONUS	},
    { "SKILL_DEFEND_BONUS",			APPLY_SKILL_DEFEND_BONUS		},
    { "NORMAL_HIT_DEFEND_BONUS",	APPLY_NORMAL_HIT_DEFEND_BONUS	},
    { "RESIST_WARRIOR",				APPLY_RESIST_WARRIOR			},
    { "RESIST_ASSASSIN",			APPLY_RESIST_ASSASSIN			},
    { "RESIST_SURA",				APPLY_RESIST_SURA				},
    { "RESIST_SHAMAN",				APPLY_RESIST_SHAMAN				},
    { "INFINITE_AFFECT_DURATION",	0x1FFFFFFF						},
	{ "ENERGY",						APPLY_ENERGY					},
	{ "COSTUME_ATTR_BONUS",			APPLY_COSTUME_ATTR_BONUS		},
	{ "MAGIC_ATTBONUS_PER",			APPLY_MAGIC_ATTBONUS_PER		},
    { "MELEE_MAGIC_ATTBONUS_PER",	APPLY_MELEE_MAGIC_ATTBONUS_PER	},
#ifdef ENABLE_WOLFMAN
	{ "BLEEDING_REDUCE",			APPLY_BLEEDING_REDUCE			},
	{ "BLEEDING_PCT",				APPLY_BLEEDING_PCT				},
	{ "ATT_BONUS_TO_WOLFMAN",		APPLY_ATTBONUS_WOLFMAN			},
	{ "RESIST_WOLFMAN",				APPLY_RESIST_WOLFMAN			},
	{ "RESIST_CLAW",				APPLY_RESIST_CLAW				},
#endif
	{"RESIST_MAGIC_REDUCTION",		APPLY_RESIST_MAGIC_REDUCTION	},
	{"RESIST_HUMAN",				APPLY_RESIST_HUMAN				},
	{"ENCHANT_ELECT",				APPLY_ENCHANT_ELECT				},
	{"ENCHANT_FIRE",				APPLY_ENCHANT_FIRE				},
	{"ENCHANT_ICE",					APPLY_ENCHANT_ICE				},
	{"ENCHANT_WIND",				APPLY_ENCHANT_WIND				},
	{"ENCHANT_EARTH",				APPLY_ENCHANT_EARTH				},
	{"ENCHANT_DARK",				APPLY_ENCHANT_DARK				},
    { NULL,							0								}
};

long FN_get_apply_type(const char *apply_type_string)
{
	TValueName	*value_name;
	for (value_name = c_aApplyTypeNames; value_name->c_pszName; ++value_name)
	{
		if (0==strcasecmp(value_name->c_pszName, apply_type_string))
			return value_name->lValue;
	}
	return 0;
}

#ifdef ENABLE_HUNTING_SYSTEM
const DWORD THuntingMissions[HUNTING_MISSION_COUNT+1][2][2] =
{
	// mob1 count1 , mob2 count2,
	{{0, 0}, {0, 0}}, // None
	{{171, 5}, {172, 3}},		// Lv1
	{{171, 10}, {172, 5}},		// Lv2
	{{171, 20}, {172, 10}},	// Lv3
	{{172, 15}, {173, 5}},		// Lv4
	{{173, 10}, {174, 10}},	// Lv5
	{{174, 20}, {178, 10}},	// Lv6
	{{178, 10}, {175, 5}},		// Lv7
	{{178, 20}, {175, 10}},	// Lv8
	{{175, 15}, {179, 5}},		// Lv9
	{{175, 20}, {179, 10}},	// Lv10
	
	{{179, 10}, {180, 5}},		// Lv11
	{{180, 15}, {176, 10}},	// Lv12
	{{176, 20}, {181, 5}},		// Lv13
	{{181, 15}, {177, 5}},		// Lv14
	{{181, 20}, {177, 10}},	// Lv15
	{{177, 15}, {184, 5}},		// Lv16
	{{177, 20}, {184, 10}},	// Lv17
	{{184, 10}, {182, 10}},	// Lv18
	{{182, 20}, {183, 10}},	// Lv19
	{{183, 20}, {352, 15}},	// Lv20
	
	{{352, 10}, {185, 10}},	// Lv21
	{{185, 25}, {354, 10}},	// Lv22
	{{354, 20}, {451, 40}},	// Lv23
	{{451, 60}, {402, 80}},	// Lv24
	{{551, 80}, {454, 20}},	// Lv25
	{{552, 80}, {456, 20}},	// Lv26
	{{456, 30}, {554, 20}},	// Lv27
	{{651, 35}, {554, 30}},	// Lv28
	{{651, 40}, {652, 30}},	// Lv29
	{{652, 40}, {2102, 30}},	// Lv30
	
	{{652, 50}, {2102, 45}},	// Lv31
	{{653, 25}, {2051, 40}},	// Lv32
	{{751, 35}, {2103, 30}},	// Lv33
	{{751, 40}, {2103, 40}},	// Lv34
	{{752, 40}, {2052, 30}},	// Lv35
	{{754, 20}, {2106, 20}},	// Lv36
	{{773, 30}, {2003, 20}},	// Lv37
	{{774, 40}, {2004, 20}},	// Lv38
	{{756, 40}, {2005, 30}},	// Lv39
	{{757, 40}, {2158, 20}},	// Lv40
	
	{{931, 40}, {5123, 25}},	// Lv41
	{{932, 30}, {5123, 30}},	// Lv42
	{{932, 40}, {2031, 35}},	// Lv43
	{{933, 40}, {2031, 30}},	// Lv44
	{{771, 50}, {2032, 45}},	// Lv45
	{{772, 30}, {5124, 30}},	// Lv46
	{{933, 35}, {5125, 30}},	// Lv47
	{{934, 40}, {5125, 35}},	// Lv48
	{{773, 40}, {2033, 45}},	// Lv49
	{{774, 40}, {5126, 20}},	// Lv50
	
	{{775, 50}, {5126, 30}},	// Lv51
	{{934, 45}, {2034, 45}},	// Lv52
	{{934, 50}, {2034, 50}},	// Lv53
	{{776, 40}, {1001, 30}},	// Lv54
	{{777, 40}, {1301, 35}},	// Lv55
	{{935, 50}, {1002, 30}},	// Lv56
	{{935, 60}, {1002, 40}},	// Lv57
	{{936, 45}, {1303, 40}},	// Lv58
	{{936, 50}, {1303, 45}},	// Lv59
	{{937, 45}, {1003, 40}},	// Lv60
	
	{{1004, 50}, {2061, 60}},	// Lv61
	{{1305, 45}, {2131, 55}},	// Lv62
	{{1305, 50}, {1101, 45}},	// Lv63
	{{2062, 50}, {1102, 45}},	// Lv64
	{{1104, 40}, {2063, 40}},	// Lv65
	{{2301, 50}, {1105, 45}},	// Lv66
	{{2301, 55}, {1105, 50}},	// Lv67
	{{1106, 50}, {1061, 50}},	// Lv68
	{{1107, 45}, {1061, 50}},	// Lv69
	{{2302, 55}, {2201, 55}},	// Lv70
	
	{{2303, 55}, {2202, 55}},	// Lv71
	{{2303, 60}, {2202, 60}},	// Lv72
	{{2304, 55}, {1063, 55}},	// Lv73
	{{2305, 50}, {1063, 55}},	// Lv74
	{{1064, 50}, {2204, 50}},	// Lv75
	{{2205, 45}, {1035, 50}},	// Lv76
	{{2311, 50}, {1068, 50}},	// Lv77
	{{1070, 50}, {1066, 55}},	// Lv78
	{{1069, 50}, {1070, 50}},	// Lv79
	{{1071, 50}, {2312, 55}},	// Lv80
	
	{{1071, 55}, {2312, 50}},	// Lv81
	{{2313, 55}, {2314, 45}},	// Lv82
	{{2313, 55}, {2314, 45}},	// Lv83
	{{1131, 60}, {2315, 50}},	// Lv84
	{{1132, 60}, {2315, 45}},	// Lv85
	{{1132, 60}, {1135, 50}},	// Lv86
	{{1132, 60}, {1135, 50}},	// Lv87
	{{1133, 60}, {1136, 50}},	// Lv88
	{{1133, 60}, {1137, 50}},	// Lv89
	{{1132, 60}, {1137, 50}},	// Lv80
};

const DWORD THuntingRewardItem[HUNTING_MISSION_COUNT+1][2][4][2] =
{
	/*
	{ mission_type 1 = { warrior {vnum, count} | assassine {vnum, count} | sura {vnum, count} | shaman {vnum, count} } }
	{ mission_type 2 = { warrior {vnum, count} | assassine {vnum, count} | sura {vnum, count} | shaman {vnum, count} } }
	*/
	
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},										// None
	{{{13, 1}, {13, 1}, {13, 1}, {7003, 1}}, {{3003, 1}, {1003, 1}, {13, 1}, {5003, 1}}},									// Lv1 - Waffen
	{{{11203, 1}, {11403, 1}, {11603, 1}, {11803, 1}}, {{11205, 1}, {11405, 1}, {11605, 1}, {11805, 1}}},			// Lv2 - Rustungen
	{{{13003, 1}, {13003, 1}, {13003, 1}, {13003, 1}}, {{13005, 1}, {13005, 1}, {13005, 1}, {13005, 1}}},			// Lv3 - Schild
	{{{12203, 1}, {12343, 1}, {12483, 1}, {12623, 1}}, {{12205, 1}, {12345, 1}, {12485, 1}, {12625, 1}}},			// Lv4 - Helm
	{{{14005, 1}, {14005, 1}, {14005, 1}, {14005, 1}}, {{14025, 1}, {14025, 1}, {14025, 1}, {14025, 1}}},			// Lv5 - Armband
	{{{16005, 1}, {16005, 1}, {16005, 1}, {16005, 1}}, {{16025, 1}, {16025, 1}, {16025, 1}, {16025, 1}}},			// Lv6 - Halskette
	{{{17005, 1}, {17005, 1}, {17005, 1}, {17005, 1}}, {{17025, 1}, {17025, 1}, {17025, 1}, {17025, 1}}},			// Lv7 - Ohrringe
	{{{15005, 1}, {15005, 1}, {15005, 1}, {15005, 1}}, {{15025, 1}, {15025, 1}, {15025, 1}, {15025, 1}}},			// Lv8 - Schuhe
	{{{11213, 1}, {11413, 1}, {11613, 1}, {11813, 1}}, {{11215, 1}, {11415, 1}, {11615, 1}, {11815, 1}}},			// Lv2 - Rustungen Lv9er
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv10
	
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv11
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv12
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv13
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv14
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv15
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv16
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv17
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv18
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv19
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv20
	
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv21
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv22
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv23
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv24
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv25
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv26
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv27
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv28
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv29
	{{{296, 1}, {296, 1}, {296, 1}, {7166, 1}}, {{3216, 1}, {1176, 1}, {296, 1}, {5116, 1}}},							// Lv30 - 30er Waffen
	
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv31
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv32
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv33
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv34
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv35
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv36
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv37
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv38
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv39
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv40
	
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv41
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv42
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv43
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv44
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv45
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv46
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv47
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv48
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv49
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv50
	
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv51
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv52
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv53
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv54
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv55
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv56
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv57
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv58
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv59
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv60
	
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv61
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv62
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv63
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv64
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv65
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv66
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv67
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv68
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv69
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv70
	
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv71
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv72
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv73
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv74
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv75
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv76
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv77
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv78
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv79
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv80
	
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv81
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv82
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv83
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv84
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv85
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv86
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv87
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv88
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv89
	{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},															// Lv90
};

const SHuntingRewardMoney THuntingRewardMoney[HUNTING_MONEY_TABLE_SIZE] =
{
	/*
	Note: The total of all Chances in Level = 100
	from_level, to_level {amount, chance} | {amount, chance} | {amount, chance} | {amount, chance} | {amount, chance}
	*/

	{ 1, 10,	{10000, 20000, 30000, 40000, 50000}},						// Lv1-10
	{11, 20,	{20000, 30000, 50000, 70000, 100000}},						// Lv11-20
	{21, 30,	{20000, 40000, 60000, 90000, 120000}},						// Lv21-30
	{31, 40,	{50000, 80000, 120000, 150000, 200000}},					// Lv31-40
	{41, 50,	{100000, 150000, 200000, 300000, 400000}},				// Lv41-50
	{51, 60,	{200000, 300000, 400000, 500000, 750000}},				// Lv51-60
	{61, 70,	{500000, 750000, 1000000, 1250000, 1500000}},			// Lv61-70
	{71, 80,	{1000000, 1500000, 2000000, 2500000, 3500000}},		// Lv71-80
	{81, 90,	{3000000, 4000000, 5000000, 7500000, 10000000}}, 	// Lv81-90
};

const SHuntingRewardEXP THuntingRewardEXP[HUNTING_EXP_TABLE_SIZE] =
{
	/*
	Note: percent exp from each level
	from_level, to_level {percent_min, percent_max}
	*/

	{ 1, 10, {10, 25}}, 	// Lv1-10
	{11, 20, {10, 20}},	// Lv11-20
	{21, 30, {10, 15}},	// Lv21-30
	{31, 40, {5, 15}},	// Lv31-40
	{41, 50, {5, 12}},	// Lv41-50
	{51, 60, {3, 10}},	// Lv51-60
	{61, 70, {3, 8}},		// Lv61-70
	{71, 80, {3, 5}}, 	// Lv71-80
	{81, 90, {2, 5}}, 	// Lv81-90
};

const DWORD THuntingRandomItem_01_20[6][2] =
{
	{27102, 3},	// Gruener Trank G
	{27105, 3},	// Violetter Trank G
	{71151, 5},	// Gruener Zauber
	{71152, 2},	// Gruene Staerke
	{70038, 25},	// Tapferkeitsumhang
	{25040, 2},	// Segensschriftrolle
};

const DWORD THuntingRandomItem_21_40[13][2] =
{
	{25040, 3},	// Segensschriftrolle
	{25041, 1},	// Magisches Metall
	{55401, 1},	// Affen-Ei
	{55402, 1},	// Spinnen-Ei
	{50300, 3},	// Fertigkeitsbuch
	{70048, 1},	// Fluchtlingsumhang
	{70049, 1},	// Lucys Ring
	{70050, 1},	// Symbol d. weisen Kaisers
	{70051, 1},	// Handschuh weiser Kaiser
	{71027, 1},	// Drachengott-Leben
	{71028, 1},	// Drachengott-Angriff
	{71029, 1},	// Drachengott-Intelligenz
	{71030, 1},	// Drachengott-Verteidigung
};

const DWORD THuntingRandomItem_41_60[13][2] =
{
	{25040, 3},	// Segensschriftrolle
	{25041, 1},	// Magisches Metall
	{55401, 1},	// Affen-Ei
	{55402, 1},	// Spinnen-Ei
	{50300, 3},	// Fertigkeitsbuch
	{70048, 1},	// Fluchtlingsumhang
	{70049, 1},	// Lucys Ring
	{70050, 1},	// Symbol d. weisen Kaisers
	{70051, 1},	// Handschuh weiser Kaiser
	{71027, 1},	// Drachengott-Leben
	{71028, 1},	// Drachengott-Angriff
	{71029, 1},	// Drachengott-Intelligenz
	{71030, 1},	// Drachengott-Verteidigung
};

const DWORD THuntingRandomItem_61_80[13][2] =
{
	{25040, 3},	// Segensschriftrolle
	{25041, 1},	// Magisches Metall
	{55401, 1},	// Affen-Ei
	{55402, 1},	// Spinnen-Ei
	{50300, 3},	// Fertigkeitsbuch
	{70048, 1},	// Fluchtlingsumhang
	{70049, 1},	// Lucys Ring
	{70050, 1},	// Symbol d. weisen Kaisers
	{70051, 1},	// Handschuh weiser Kaiser
	{71027, 1},	// Drachengott-Leben
	{71028, 1},	// Drachengott-Angriff
	{71029, 1},	// Drachengott-Intelligenz
	{71030, 1},	// Drachengott-Verteidigung
};

const DWORD THuntingRandomItem_81_90[13][2] =
{
	{25040, 3},	// Segensschriftrolle
	{25041, 1},	// Magisches Metall
	{55401, 1},	// Affen-Ei
	{55402, 1},	// Spinnen-Ei
	{50300, 3},	// Fertigkeitsbuch
	{70048, 1},	// Fluchtlingsumhang
	{70049, 1},	// Lucys Ring
	{70050, 1},	// Symbol d. weisen Kaisers
	{70051, 1},	// Handschuh weiser Kaiser
	{71027, 1},	// Drachengott-Leben
	{71028, 1},	// Drachengott-Angriff
	{71029, 1},	// Drachengott-Intelligenz
	{71030, 1},	// Drachengott-Verteidigung
};
#endif

