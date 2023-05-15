import uiScriptLocale
import localeInfo
import constInfo
import grp

IMG_PATH = "d:/ymir work/ui/aslan/buffnpc/"

BOARD_WIDTH = 306
BOARD_HEIGHT = 180

window = {
	"name" : "BuffNPCSelectSkillWindow",

	"x" : (SCREEN_WIDTH / 2) - (BOARD_WIDTH / 2),
	"y" : (SCREEN_HEIGHT / 2) - (BOARD_HEIGHT / 2) ,

	"style" : ("movable", "float",),

	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,

	"children" :
	(
		{
			"name" : "board", "type" : "board_with_titlebar",
			"x" : 0, "y" : 0, "width" : BOARD_WIDTH, "height" : BOARD_HEIGHT, "title" : uiScriptLocale.ASLAN_BUFF_TITLE_LEARN_GRANDMASTER,
			"children" :
			(
				{ 
					"name" : "skill_select_board", "type" : "image", "x" : 10, "y" : 35, "image" : IMG_PATH + "board_skill.sub",
					"children" :
					(
						{ "name" : "skill_board_title", "type" : "text", "x" : 0, "y" : 12, "text": uiScriptLocale.ASLAN_BUFF_SELECT_SKILL, "horizontal_align" : "center", "text_horizontal_align" : "center", "text_vertical_align" : "center"},
						
						{ "name" : "img_title", "type" : "image", "x" : 0, "y" : 31, "horizontal_align" : "center", "image" : IMG_PATH + "title_large.sub", "children" : ( { "name" : "text_info", "type" : "text", "x" : 0, "y" : -1, "text": "", "all_align" : "center" },) },
						
						{
							"name" : "skill_slot_icons", "type" : "window", "x" : 15, "y" : 58, "width" : 256, "height" : 32,
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
							"name" : "skill_slots", "type" : "slot", "x" : 15, "y" : 58, "width" : 256, "height" : 32,
							"slot" :	(
								{"index": 0, "x":  0, "y":  0, "width":32, "height":32},
								{"index": 1, "x": 46, "y":  0, "width":32, "height":32},
								{"index": 2, "x": 91, "y":  0, "width":32, "height":32},
								{"index": 3, "x":134, "y":  0, "width":32, "height":32},
								{"index": 4, "x":179, "y":  0, "width":32, "height":32},
								{"index": 5, "x":224, "y":  0, "width":32, "height":32},
							),
						},
						{
							"name" : "skill_slot_cover", "type" : "window", "style" : ("not_pick",), "x" : 15, "y" : 58, "width" : 256, "height" : 32,
							"children":
							(
								{ "name" : "skill_slot_cover_0", "type" : "image", "style" : ("not_pick",), "x" : 0, "y" : 0, "image" : IMG_PATH + "slot_cover_green.sub"},
								{ "name" : "skill_slot_cover_1", "type" : "image", "style" : ("not_pick",), "x" : 46, "y" : 0, "image" : IMG_PATH + "slot_cover_green.sub"},
								{ "name" : "skill_slot_cover_2", "type" : "image", "style" : ("not_pick",), "x" : 91, "y" : 0, "image" : IMG_PATH + "slot_cover_green.sub"},
								{ "name" : "skill_slot_cover_3", "type" : "image", "style" : ("not_pick",), "x" : 134, "y" : 0, "image" : IMG_PATH + "slot_cover_green.sub"},
								{ "name" : "skill_slot_cover_4", "type" : "image", "style" : ("not_pick",), "x" : 179, "y" : 0, "image" : IMG_PATH + "slot_cover_green.sub"},
								{ "name" : "skill_slot_cover_5", "type" : "image", "style" : ("not_pick",), "x" : 224, "y" : 0, "image" : IMG_PATH + "slot_cover_green.sub"},
							),
						},
						{
							"name" : "skill_slot_cover_empty", "type" : "window", "style" : ("not_pick",), "x" : 15, "y" : 58, "width" : 256, "height" : 32,
							"children":
							(
								{ "name" : "skill_slot_cover_empty_0", "type" : "image", "style" : ("not_pick",), "x" : 0+1, "y" : 0+1, "image" : IMG_PATH + "slot_cover_empty.sub"},
								{ "name" : "skill_slot_cover_empty_1", "type" : "image", "style" : ("not_pick",), "x" : 46+1, "y" : 0+1, "image" : IMG_PATH + "slot_cover_empty.sub"},
								{ "name" : "skill_slot_cover_empty_2", "type" : "image", "style" : ("not_pick",), "x" : 91+1, "y" : 0+1, "image" : IMG_PATH + "slot_cover_empty.sub"},
								{ "name" : "skill_slot_cover_empty_3", "type" : "image", "style" : ("not_pick",), "x" : 134+1, "y" : 0+1, "image" : IMG_PATH + "slot_cover_empty.sub"},
								{ "name" : "skill_slot_cover_empty_4", "type" : "image", "style" : ("not_pick",), "x" : 179+1, "y" : 0+1, "image" : IMG_PATH + "slot_cover_empty.sub"},
								{ "name" : "skill_slot_cover_empty_5", "type" : "image", "style" : ("not_pick",), "x" : 224+1, "y" : 0+1, "image" : IMG_PATH + "slot_cover_empty.sub"},
							),
						},
						{
							"name" : "accept_button", "type" : "button", "x" : 0 , "y" : 100, "horizontal_align" : "center",
							"default_image" : IMG_PATH + "button_accept_normal.sub", "over_image" : IMG_PATH + "button_accept_hover.sub", "down_image" : IMG_PATH + "button_accept_down.sub",
						},
					),
				},
			)
		},
	),
}