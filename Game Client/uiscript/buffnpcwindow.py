import uiScriptLocale
import localeInfo
import constInfo
import player
import grp

IMG_PATH = "d:/ymir work/ui/aslan/buffnpc/"
SKILL_IMG_PATH = "d:/ymir work/ui/skill/shaman/"
PUBLIC_IMG_PATH = "d:/ymir work/ui/public/"
GAME_IMG_PATH = "d:/ymir work/ui/game/windows/"

BOARD_WIDTH = 436
BOARD_HEIGHT = 278

## TEXT COLOR
GOLD_COLOR	= 0xFFFEE3AE
WHITE_COLOR = 0xFFFFFFFF

BUFF_EQUIPMENT_START_INDEX = player.BUFF_EQUIPMENT_SLOT_START

window = {
	"name" : "BuffNPCWindow",

	# "x" : (SCREEN_WIDTH / 2) - (BOARD_WIDTH / 2),
	# "y" : (SCREEN_HEIGHT / 2) - (BOARD_HEIGHT / 2) ,
	"x" : 100,
	"y" : SCREEN_HEIGHT - BOARD_HEIGHT - 150 ,

	"style" : ("movable", "float",),

	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,

	"children" :
	(
		{
			"name" : "board", "type" : "board_with_titlebar", "x" : 0, "y" : 0, "width" : BOARD_WIDTH, "height" : BOARD_HEIGHT,
			"title" : uiScriptLocale.ASLAN_BUFF_TITLE_MAIN_BOARD,
			"children" :
			(
				
				## Page 01
				{
					"name" : "Page_01", "type" : "window", "style" : ("attach",), "x" : 0, "y" : 30, "width" : BOARD_WIDTH, "height" : BOARD_HEIGHT-30,
					"children":
					(
						{ "name" : "face_icon_empty", "type" : "image", "x" : 15, "y" : 9, "image" : IMG_PATH + "icon_face_empty.tga"},
						{ "name" : "face_icon_0", "type" : "image", "x" : 15, "y" : 9, "image" : "icon/face/shaman_m.tga" },
						{ "name" : "face_icon_1", "type" : "image", "x" : 15, "y" : 9, "image" : "icon/face/shaman_w.tga" },
						{ "name" : "face_icon_overlay", "type" : "image", "x" : 11, "y" : 5, "image" : IMG_PATH + "face_slot.sub"},
						
						{
							"name" : "summon_button", "type" : "button", "x" : 302, "y" : 8, "text" : uiScriptLocale.ASLAN_BUFF_BUTTON_SUMMON, "text_color" : GOLD_COLOR,
							"default_image" : IMG_PATH + "button_02_normal.sub", "over_image" : IMG_PATH + "button_02_hover.sub", "down_image" : IMG_PATH + "button_02_down.sub",
						},
						{
							"name" : "export_button", "type" : "button", "x" : 302, "y" : 34, "text" : uiScriptLocale.ASLAN_BUFF_BUTTON_EXPORT, "text_color" : GOLD_COLOR,
							"default_image" : IMG_PATH + "button_02_normal.sub", "over_image" : IMG_PATH + "button_02_hover.sub", "down_image" : IMG_PATH + "button_02_down.sub",
						},
						{ 
							"name" : "info_board", "type" : "image", "x" : 68, "y" : 8, "image" : IMG_PATH + "board_info.sub",
							"children" :
							(
								{ "name" : "buff_name", "type" : "text", "x" : 49, "y" : 33, "text_horizontal_align" : "center", "text_vertical_align" : "center"},
								{ "name" : "buff_level", "type" : "text", "x" : 115, "y" : 33, "text_horizontal_align" : "center", "text_vertical_align" : "center"},
								{ "name" : "exp_hover_info", "type" : "window", "x" : 145, "y" : 25, "width" : 75, "height" : 18},
								
								{ "name" : "exp_gauge_01", "type" : "expanded_image", "x" : 147 + 0, "y" : 25, "image" : IMG_PATH + "exp_full.sub" },
								{ "name" : "exp_gauge_02", "type" : "expanded_image", "x" : 147 + 18, "y" : 25, "image" : IMG_PATH + "exp_full.sub" },
								{ "name" : "exp_gauge_03", "type" : "expanded_image", "x" : 147 + 36, "y" : 25, "image" : IMG_PATH + "exp_full.sub" },
								{ "name" : "exp_gauge_04", "type" : "expanded_image", "x" : 147 + 54, "y" : 25, "image" : IMG_PATH + "exp_full.sub" },
							),
						},
						
						{ 
							"name" : "skill_board", "type" : "image", "x" : 11, "y" : 60, "image" : IMG_PATH + "board_skill.sub",
							"children" :
							(
								{ "name" : "skill_board_title", "type" : "text", "x" : 0, "y" : 12, "text": uiScriptLocale.ASLAN_BUFF_TITLE_SKILLS, "horizontal_align" : "center", "text_horizontal_align" : "center", "text_vertical_align" : "center"},
								
								{ "name" : "img_title_rest_skill_points", "type" : "image", "x" : 26, "y" : 36, "image" : IMG_PATH + "title_large.sub", "children" : ( { "name" : "skill_points", "type" : "text", "x" : 0, "y" : -1, "all_align" : "center",},) },
								{ "name" : "img_title_int_points", "type" : "image", "x" : 186, "y" : 36, "image" : IMG_PATH + "title_small.sub", "children" : ( { "name" : "int_points", "type" : "text", "x" : 0, "y" : -1, "all_align" : "center",},) },
								
								{
									"name" : "skill_slot_icons", "type" : "window", "x" : 15, "y" : 65, "width" : 256, "height" : 32,
									"children":
									(
										{ "name" : "skill_slot_icon_0", "type" : "image", "x" : 0, "y" : 0, "image" : IMG_PATH + "slot.sub"},
										{ "name" : "skill_slot_icon_1", "type" : "image", "x" : 46, "y" : 0, "image" : IMG_PATH + "slot.sub"},
										{ "name" : "skill_slot_icon_2", "type" : "image", "x" : 91, "y" : 0, "image" : IMG_PATH + "slot.sub"},
										{ "name" : "skill_slot_icon_3", "type" : "image", "x" : 134, "y" : 0, "image" : IMG_PATH + "slot.sub"},
										{ "name" : "skill_slot_icon_4", "type" : "image", "x" : 179, "y" : 0, "image" : IMG_PATH + "slot.sub"},
										{ "name" : "skill_slot_icon_5", "type" : "image", "x" : 224, "y" : 0, "image" : IMG_PATH + "slot.sub"},
									),
								},
								## Active Slot
								{
									"name" : "skill_slots", "type" : "slot", "x" : 15, "y" : 65, "width" : 256, "height" : 32,
									"slot" :	(
										{"index": 0, "x":  0, "y":  0, "width":32, "height":32},
										{"index": 1, "x": 46, "y":  0, "width":32, "height":32},
										{"index": 2, "x": 91, "y":  0, "width":32, "height":32},
										{"index": 3, "x":134, "y":  0, "width":32, "height":32},
										{"index": 4, "x":179, "y":  0, "width":32, "height":32},
										{"index": 5, "x":224, "y":  0, "width":32, "height":32},
									),
								},
								{ "name" : "skill_active_icon_0", "type" : "image", "x" : 24, "y" : 106, "image" : IMG_PATH + "skill_is_activate.sub"},
								{ "name" : "skill_active_icon_1", "type" : "image", "x" : 70, "y" : 106, "image" : IMG_PATH + "skill_is_activate.sub"},
								{ "name" : "skill_active_icon_2", "type" : "image", "x" : 115, "y" : 106, "image" : IMG_PATH + "skill_is_activate.sub"},
								{ "name" : "skill_active_icon_3", "type" : "image", "x" : 158, "y" : 106, "image" : IMG_PATH + "skill_is_activate.sub"},
								{ "name" : "skill_active_icon_4", "type" : "image", "x" : 203, "y" : 106, "image" : IMG_PATH + "skill_is_activate.sub"},
								{ "name" : "skill_active_icon_5", "type" : "image", "x" : 248, "y" : 106, "image" : IMG_PATH + "skill_is_activate.sub"},
							),
						},
						
						{ 
							"name" : "eq_board", "type" : "image", "x" : 305, "y" : 61, "image" : IMG_PATH + "board_equipment.sub",
							"children" :
							(
								{
									"name" : "equipment_slots", "type" : "slot", "x" : 0, "y" : 0, "width" : 115, "height" : 178,
									"slot" :	(
										{"index": BUFF_EQUIPMENT_START_INDEX+0, "x":  15, "y":  60, "width":32, "height":32},
										{"index": BUFF_EQUIPMENT_START_INDEX+1, "x": 64, "y": 24, "width":32, "height":32},
										{"index": BUFF_EQUIPMENT_START_INDEX+2, "x": 64, "y":  60, "width":32, "height":64},
										{"index": BUFF_EQUIPMENT_START_INDEX+3, "x":15, "y":  11, "width":32, "height":32},
									),
								},
							),
						},

					),
				},
				## Page 01
				{
					"name" : "Page_02", "type" : "window", "style" : ("attach",), "x" : 0, "y" : 30, "width" : BOARD_WIDTH, "height" : BOARD_HEIGHT-30,
					"children":
					(
						{ 
							"name" : "common_board", "type" : "image", "x" : 11, "y" : 6, "image" : IMG_PATH + "board_common.sub",
							"children" :
							(
								{ "name" : "common_board_title", "type" : "text", "x" : 0, "y" : 12, "text": uiScriptLocale.ASLAN_BUFF_TITLE_IMPROVE_CHANGE, "horizontal_align" : "center", "text_horizontal_align" : "center", "text_vertical_align" : "center"},
								{ "name" : "arrow_left_button", "type" : "button", "x" : 26, "y" : 49, "default_image" : IMG_PATH + "arrow_left_normal.sub", "over_image" : IMG_PATH + "arrow_left_hover.sub", "down_image" : IMG_PATH + "arrow_left_down.sub",},
								{ "name" : "arrow_right_button", "type" : "button", "x" : 79, "y" : 49, "default_image" : IMG_PATH + "arrow_right_normal.sub", "over_image" : IMG_PATH + "arrow_right_hover.sub", "down_image" : IMG_PATH + "arrow_right_down.sub",},
						
								## Active Slot
								{ "name" : "preview_upgrade_skill_slot", "type" : "slot", "x" : 43, "y" : 39, "width" : 32, "height" : 32, "slot" :	( {"index": 0, "x":  0, "y":  0, "width":32, "height":32}, ), },
								{
									"name" : "upgrade_skill_slots", "type" : "slot", "x" : 105, "y" : 39, "width" : 136, "height" : 32,
									"slot" :	(
										{"index": BUFF_EQUIPMENT_START_INDEX+4, "x": 0, "y":  0, "width":32, "height":32},
										{"index": BUFF_EQUIPMENT_START_INDEX+5, "x": 52, "y": 0, "width":32, "height":32},
										{"index": BUFF_EQUIPMENT_START_INDEX+6, "x": 104, "y":  0, "width":32, "height":32},
									),
								},
								{
									"name" : "upgrade_skill_button", "type" : "button", "x" : 259, "y" : 42, "text" : uiScriptLocale.ASLAN_BUFF_BUTTON_TEXT_USE, "text_color" : GOLD_COLOR,
									"default_image" : IMG_PATH + "button_01_normal.sub", "over_image" : IMG_PATH + "button_01_hover.sub", "down_image" : IMG_PATH + "button_01_down.sub",
								},
								
								{ "name" : "change_name_title", "type" : "text", "x" : 75, "y" : 100, "text": uiScriptLocale.ASLAN_BUFF_CHANGE_NAME, "text_horizontal_align" : "center", "text_vertical_align" : "center", "color" : GOLD_COLOR,},
								{ "name" : "change_sex_title", "type" : "text", "x" : 204, "y" : 100, "text": uiScriptLocale.ASLAN_BUFF_CHANGE_SEX, "text_horizontal_align" : "center", "text_vertical_align" : "center", "color" : GOLD_COLOR,},
								{ "name" : "reset_skill_title", "type" : "text", "x" : 333, "y" : 100, "text": uiScriptLocale.ASLAN_BUFF_RESET_SKILL, "text_horizontal_align" : "center", "text_vertical_align" : "center", "color" : GOLD_COLOR,},
								
								{
									"name" : "change_slots", "type" : "slot", "x" : 59, "y" : 116, "width" : 290, "height" : 32,
									"slot" :	(
										{"index": BUFF_EQUIPMENT_START_INDEX+7, "x":  0, "y":  0, "width":32, "height":32},
										{"index": BUFF_EQUIPMENT_START_INDEX+8, "x": 129, "y": 0, "width":32, "height":32},
										{"index": BUFF_EQUIPMENT_START_INDEX+9, "x": 258, "y":  0, "width":32, "height":32},
									),
								},
								
								{
									"name" : "change_name_button", "type" : "button", "x" : 30, "y" : 153, "text" : uiScriptLocale.ASLAN_BUFF_BUTTON_TEXT_USE, "text_color" : GOLD_COLOR,
									"default_image" : IMG_PATH + "button_03_normal.sub", "over_image" : IMG_PATH + "button_03_hover.sub", "down_image" : IMG_PATH + "button_03_down.sub",
								},
								{
									"name" : "change_sex_button", "type" : "button", "x" : 159, "y" : 153, "text" : uiScriptLocale.ASLAN_BUFF_BUTTON_TEXT_USE, "text_color" : GOLD_COLOR,
									"default_image" : IMG_PATH + "button_03_normal.sub", "over_image" : IMG_PATH + "button_03_hover.sub", "down_image" : IMG_PATH + "button_03_down.sub",
								},
								{
									"name" : "reset_skill_button", "type" : "button", "x" : 288, "y" : 153, "text" : uiScriptLocale.ASLAN_BUFF_BUTTON_TEXT_USE, "text_color" : GOLD_COLOR,
									"default_image" : IMG_PATH + "button_03_normal.sub", "over_image" : IMG_PATH + "button_03_hover.sub", "down_image" : IMG_PATH + "button_03_down.sub",
								},
							),
						},
					),
				},
				{
					"name" : "TabControl", "type" : "window", "x" : 3, "y" : BOARD_HEIGHT - 48, "width" : BOARD_WIDTH, "height" : 48,
					"children" :
					(
						## Tab
						{ "name" : "Tab_01", "type" : "image", "x" : 0, "y" : 0, "width" : BOARD_WIDTH, "height" : 45, "image" : IMG_PATH + "tab_0.sub",},
						{ "name" : "Tab_02", "type" : "image", "x" : 0, "y" : 0, "width" : BOARD_WIDTH, "height" : 45, "image" : IMG_PATH + "tab_1.sub",},
						## Button
						{ "name" : "Tab_Button_01", "type" : "button", "x" : 8, "y" : 7, "width" : 81, "height" : 32, },
						{ "name" : "Tab_Button_02", "type" : "button", "x" : 102, "y" : 7, "width" : 81, "height" : 32, },
					 ),	
				},
			)
		},
	),
}