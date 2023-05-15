import uiScriptLocale

SMALL_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_01.sub"
LARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_03.sub"
XLARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_04.sub"

window = {
	"name" : "GuildLandDealDialog",
	"style" : ("movable","float",),
	
	"x" : SCREEN_WIDTH/2 - 250,
	"y" : SCREEN_HEIGHT/2 - 40,
	
	"width" : 280,
	"height" : 120,
	
	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			
			"x" : 0,
			"y" : 0,
			
			"width" : 230,
			"height" : 135,
			
			"children" :
			(
				## Ÿ��Ʋ ��
				{
					"name" : "titlebar",
					"type" : "titlebar",
					"style" : ("attach",),
					
					"x" : 8,
					"y" : 8,
					
					"width" : 215,
					"color" : "gray",
					
					"children" :
					(
						{
							"name" : "titlename", "type" : "text", "x" : 0, "y" : 3,
							"text" : uiScriptLocale.GUILD_LANDDEAL_VOTE,
							"horizontal_align" : "center", "text_horizontal_align" : "center"
						},
					),
				},
				
				## �ŷ� ���
				{"name" : "DealGuildName", "type" : "text", "x" : 20, "y" : 40, "text" : uiScriptLocale.GUILD_LANDDEAL_VOTE_DEALGUILDNAME,	},
				## �ŷ� ��� �̸� ����
				{
					"name" : "dealguildname_slot",
					"type" : "image",

					"x" : 100,
					"y" : 38,
					
					"image" : XLARGE_VALUE_FILE,

					"children" :
					(
						{
							"name" : "dealguildname_slot",
							"type" : "editline",

							"x" : 3,
							"y" : 2,

							"width" : 80,
							"height" : 18,

							"input_limit" : 18,
							"only_number" : 0,

							"text" : "",
						},
					),
				},		
				## �ŷ� �����
				{"name" : "DealGuildMoney", "type" : "text", "x" : 20, "y" : 70, "text" : uiScriptLocale.GUILD_LANDDEAL_VOTE_DEALGOLD,},
				## �ŷ� ����� ����
				{
					"name" : "dealguildmoney_slot",
					"type" : "image",

					"x" : 100,
					"y" : 68,
					
					"image" : XLARGE_VALUE_FILE,

					"children" :
					(
						{
							"name" : "dealguildmoney_slot",
							"type" : "editline",

							"x" : 3,
							"y" : 2,

							"width" : 80,
							"height" : 18,

							"input_limit" : 9,
							"only_number" : 1,

							"text" : "",
						},
					),
				},					
				## Ȯ�� ��ư
				{
					"name" : "Lbutton",
					"type" : "button",

					"x" : -35,
					"y" : 100,

					"width" : 61,
					"height" : 50,

					"horizontal_align" : "center",
					"text" : uiScriptLocale.OK,

					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				},
				## ��� ��ư
				{
					"name" : "Rbutton",
					"type" : "button",

					"x" : 35,
					"y" : 100,

					"width" : 61,
					"height" : 50,

					"horizontal_align" : "center",
					"text" : uiScriptLocale.CANCEL,

					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				},
			),
		},
	),
}