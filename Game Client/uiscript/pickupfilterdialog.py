import uiScriptLocale

BOARD_WIDTH = 300
BOARD_HEIGHT = 220

DROPS_LINE_X = 15
DROPS_LINE_X_2 = DROPS_LINE_X + 100
DROPS_LINE_X_3 = DROPS_LINE_X_2 + 100

DEFAULT_BTN_IMAGE = "d:/ymir work/ui/public/middle_button_01.sub"
OVER_BTN_IMAGE = "d:/ymir work/ui/public/middle_button_02.sub"
DOWN_BTN_IMAGE = "d:/ymir work/ui/public/middle_button_03.sub"

window = {
	"name" : "PickupOptionDialog",
	"style" : ("movable", "float",),

	"x" : 0,
	"y" : 0,

	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board_with_titlebar",

			"x" : 0,
			"y" : 0,

			"width" : BOARD_WIDTH,
			"height" : BOARD_HEIGHT,
			
			"title" : uiScriptLocale.PICKUP_FILTER_TITLE,

			"children" :
			(
				{
					"name" : "new_bar",
					"type" : "horizontalbar",

					"x": DROPS_LINE_X,
					"y": 39,
					
					"width" : BOARD_WIDTH - DROPS_LINE_X * 2,
				},
				{
					"name" : "pickup_mode",
					"type" : "text",

					"x": BOARD_WIDTH / 2 -DROPS_LINE_X,
					"y": 40,
					
					"text" : uiScriptLocale.PICKUP_FILTER_MODE_LABEL,
				},
				{
					"name" : "pickup_mode_single",
					"type" : "radio_button",

					"x" : BOARD_WIDTH / 4 - DROPS_LINE_X,
					"y" : 65,

					"text" : uiScriptLocale.PICKUP_FILTER_MODE_SINGLE_LABEL,

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
				},
				{
					"name" : "pickup_mode_all",
					"type" : "radio_button",

					"x" : BOARD_WIDTH / 2 + DROPS_LINE_X-5,
					"y" : 65,

					"text" : uiScriptLocale.PICKUP_FILTER_MODE_ALL_LABEL,

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
				},
				{
					"name" : "new_bar2",
					"type" : "horizontalbar",

					"x": DROPS_LINE_X,
					"y": 99,
					
					"width" : BOARD_WIDTH - DROPS_LINE_X * 2,
				},
				{
					"name" : "drops_ignore",
					"type" : "text",

					"x": BOARD_WIDTH / 2 - DROPS_LINE_X - 7,
					"y" : 100,

					"text" : uiScriptLocale.PICKUP_FILTER_IGNORE_LABEL,
				},

				{
					"name" : "drops_ignore_weapon",
					"type" : "toggle_button",

					"x" : DROPS_LINE_X + 8,
					"y" : 122,

					"text" : uiScriptLocale.PICKUP_FILTER_IGNORE_WEAPON_LABEL,

					"default_image" : DEFAULT_BTN_IMAGE,
					"over_image" : OVER_BTN_IMAGE,
					"down_image" : DOWN_BTN_IMAGE,
				},

				{
					"name" : "drops_ignore_armor",
					"type" : "toggle_button",

					"x" : DROPS_LINE_X_2 + 38,
					"y" : 122,

					"text" : uiScriptLocale.PICKUP_FILTER_IGNORE_ARMOR_LABEL,

					"default_image" : DEFAULT_BTN_IMAGE,
					"over_image" : OVER_BTN_IMAGE,
					"down_image" : DOWN_BTN_IMAGE,
				},
				{
					"name" : "drops_ignore_head",
					"type" : "toggle_button",

					"x" : DROPS_LINE_X + 8,
					"y" : 152,

					"text" : uiScriptLocale.PICKUP_FILTER_IGNORE_HEAD_LABEL,

					"default_image" : DEFAULT_BTN_IMAGE,
					"over_image" : OVER_BTN_IMAGE,
					"down_image" : DOWN_BTN_IMAGE,
				},
				{
					"name" : "drops_ignore_shield",
					"type" : "toggle_button",

					"x" : DROPS_LINE_X_3 + 3,
					"y" : 152,

					"text" : uiScriptLocale.PICKUP_FILTER_IGNORE_SHIELD_LABEL,

					"default_image" : DEFAULT_BTN_IMAGE,
					"over_image" : OVER_BTN_IMAGE,
					"down_image" : DOWN_BTN_IMAGE,
				},
				{
					"name" : "drops_ignore_wrist",
					"type" : "toggle_button",

					"x" : DROPS_LINE_X_2 - 27,
					"y" : 122,

					"text" : uiScriptLocale.PICKUP_FILTER_IGNORE_WRIST_LABEL,

					"default_image" : DEFAULT_BTN_IMAGE,
					"over_image" : OVER_BTN_IMAGE,
					"down_image" : DOWN_BTN_IMAGE,
				},
				{
					"name" : "drops_ignore_foots",
					"type" : "toggle_button",

					"x" : DROPS_LINE_X_3 + 3,
					"y" : 122,

					"text" : uiScriptLocale.PICKUP_FILTER_IGNORE_FOOTS_LABEL,

					"default_image" : DEFAULT_BTN_IMAGE,
					"over_image" : OVER_BTN_IMAGE,
					"down_image" : DOWN_BTN_IMAGE,
				},
				
				{
					"name" : "drops_ignore_neck",
					"type" : "toggle_button",

					"x" : DROPS_LINE_X_2 - 27,
					"y" : 152,

					"text" : uiScriptLocale.PICKUP_FILTER_IGNORE_NECK_LABEL,

					"default_image" : DEFAULT_BTN_IMAGE,
					"over_image" : OVER_BTN_IMAGE,
					"down_image" : DOWN_BTN_IMAGE,
				},
				
				{
					"name" : "drops_ignore_ear",
					"type" : "toggle_button",

					"x" : DROPS_LINE_X_2 + 38,
					"y" : 152,

					"text" : uiScriptLocale.PICKUP_FILTER_IGNORE_EAR_LABEL,

					"default_image" : DEFAULT_BTN_IMAGE,
					"over_image" : OVER_BTN_IMAGE,
					"down_image" : DOWN_BTN_IMAGE,
				},
				
				{
					"name" : "drops_ignore_etc",
					"type" : "toggle_button",

					"x" : DROPS_LINE_X_2 + 3,
					"y" : 182,

					"text" : uiScriptLocale.PICKUP_FILTER_IGNORE_ETC_LABEL,

					"default_image" : DEFAULT_BTN_IMAGE,
					"over_image" : OVER_BTN_IMAGE,
					"down_image" : DOWN_BTN_IMAGE,
				},
			),
		},
	),
}
