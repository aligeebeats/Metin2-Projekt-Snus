import uiScriptLocale

WINDOW_WIDTH	= 240
WINDOW_HEIGHT	= 180

window = {
	"name" : "MiniGameFindMUpgrade", "style" : ("movable", "float",),
	"x" : SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2, "y" : SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	"width" : WINDOW_WIDTH, "height" : WINDOW_HEIGHT,
	"children" :
	[
		{
			"name" : "Board", "type" : "board",
			"x" : 0, "y" : 0,
			"width" : WINDOW_WIDTH, "height" : WINDOW_HEIGHT,
			"children" :
			[
				# Title Bar
				{
					"name" : "TitleBar", "type" : "titlebar",
					"x" : 8, "y" : 7,
					"width" : WINDOW_WIDTH - 16, "color" : "red",
					"title" : uiScriptLocale.LUCKY_BOX_TITLE_NAME,
					"children" :
					[
						{
							"name" : "TitleName", "type" : "text",
							"x" : 0, "y" : 0,
							"text" : uiScriptLocale.MINI_GAME_FINDM_UPGRADE_TITLE,
							"all_align" : "center",
						},
					],
				},
				# Item Bar
				{
					"name" : "RewardBar", "type" : "horizontalbar",
					"x" : 10, "y" : 40,
					"width" : WINDOW_WIDTH - 20,
					"children" :
					[
						{
							"name" : "RewardBarName", "type" : "text",
							"x" : 5, "y" : 2,
							"text" : uiScriptLocale.MINI_GAME_FINDM_UPGRADE_BAR1,
						},
					]
				},
				# Item Slot
				{					
					"name" : "RewardItemSlot", "type" : "slot",
					"x" : 15, "y" : 70,
					"width" : 32, "height" : 32,
					"image" : "d:/ymir work/ui/public/Slot_Base.sub",
					"slot" :
					[
						{ "index" : 0, "x" : 0, "y" : 0, "width" : 32, "height" : 32, },
					],
				},
				# Get Button
				{
					"name" : "GetButton", "type" : "button",
					"x" : 51, "y" : 63,
					"text" : uiScriptLocale.MINI_GAME_FINDM_UPGRADE_RECV,
					"default_image"	: "d:/ymir work/ui/event/long_button_01.sub",
					"over_image"	: "d:/ymir work/ui/event/long_button_02.sub",
					"down_image"	: "d:/ymir work/ui/event/long_button_03.sub",
				},
				# Upgrade Button
				{
					"name" : "UpgradeButton", "type" : "button",
					"x" : 51, "y" : 89,
					"text" : uiScriptLocale.MINI_GAME_FINDM_UPGRADE_RETRY,
					"default_image"	: "d:/ymir work/ui/event/long_button_01.sub",
					"over_image"	: "d:/ymir work/ui/event/long_button_02.sub",
					"down_image"	: "d:/ymir work/ui/event/long_button_03.sub",
				},
				# NeedMoney Bar
				{
					"name" : "NeedMoneyBar", "type" : "horizontalbar",
					"x" : 10, "y" : 121,
					"width" : WINDOW_WIDTH - 20,
					"children" :
					[
						{
							"name" : "NeedMoneyBarName", "type" : "text",
							"x" : 5, "y" : 2,
							"text" : uiScriptLocale.MINI_GAME_FINDM_UPGRADE_BAR2,
						},
					]
				},
				# NeedMoney
				{
					"name" : "NeedMoneySlot", "type" : "expanded_image",
					"x" : 52, "y" : 145,
					"x_scale" : 1.92, "y_scale" : 1.0,
					"image" : "d:/ymir work/ui/public/gold_slot.sub",
					"children" :
					[
						{
							"name" : "NeedMoney", "type" : "text",
							"x" : 5, "y" : 0,
							"text" : "0",
							"vertical_align" : "center", "horizontal_align" : "right",
							"text_vertical_align" : "center", "text_horizontal_align" : "right",
						},
					],
				},
			],
		},
	],
}
