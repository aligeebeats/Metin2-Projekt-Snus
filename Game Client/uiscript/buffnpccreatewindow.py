import uiScriptLocale
import localeInfo
import grp

IMG_PATH = "d:/ymir work/ui/aslan/buffnpc/"

BOARD_WIDTH = 200
BOARD_HEIGHT = 420

window = {
	"name" : "BuffNPCCreateWindow",

	"x" : (SCREEN_WIDTH / 2) - (BOARD_WIDTH / 2),
	"y" : (SCREEN_HEIGHT / 2) - (BOARD_HEIGHT / 2) ,

	"style" : ("movable", "float",),

	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,

	"children" :
	(
		{
			"name" : "board", "type" : "board_with_titlebar",
			"x" : 0, "y" : 0, "width" : BOARD_WIDTH, "height" : BOARD_HEIGHT, "title" : uiScriptLocale.ASLAN_BUFF_TITLE_CREATE_BOARD,
			"children" :
			(
				{ "name" : "render_target_background", "type" : "thinboard", "x" : 10, "y" : 35, "width" : 180, "height" : 230, },
				
				{ 
					"name" : "config_board", "type" : "image", "x" : 10, "y" : 35 + 230 + 5, "image" : IMG_PATH + "board_config_create.sub",
					"children" :
					(
						{ "name" : "text_name", "type" : "text", "x" : 0, "y" : 13, "text": uiScriptLocale.ASLAN_BUFF_INPUT_NAME_TITLE, "horizontal_align" : "center", "text_horizontal_align" : "center", "text_vertical_align" : "center"},
						{ "name" : "editline_name", "type" : "editline", "x" : 28, "y" : 30, "width" : 127, "height" : 16, "input_limit" : 14,},
						
						{
							"name" : "button_shaman_m", "type" : "radio_button", "x" : 37, "y" : 60,
							"default_image" : IMG_PATH + "button_shaman_m_down.sub",
							"over_image" : IMG_PATH + "button_shaman_m_hover.sub",
							"down_image" : IMG_PATH + "button_shaman_m_normal.sub",
						},
						{
							"name" : "button_shaman_w", "type" : "radio_button", "x" : 105, "y" : 60,
							"default_image" : IMG_PATH + "button_shaman_w_down.sub",
							"over_image" : IMG_PATH + "button_shaman_w_hover.sub",
							"down_image" : IMG_PATH + "button_shaman_w_normal.sub",
						},
					),
				},
				
				{
					"name" : "accept_button", "type" : "button", "x" : 10 + 8 , "y" : BOARD_HEIGHT - 35,

					"default_image" : IMG_PATH + "button_accept_normal.sub",
					"over_image" : IMG_PATH + "button_accept_hover.sub",
					"down_image" : IMG_PATH + "button_accept_down.sub",
				},
				
				{
					"name" : "cancel_button", "type" : "button", "x" : 10 + 96 , "y" : BOARD_HEIGHT - 35,

					"default_image" : IMG_PATH + "button_cancel_normal.sub",
					"over_image" : IMG_PATH + "button_cancel_hover.sub",
					"down_image" : IMG_PATH + "button_cancel_down.sub",
				},
			)
		},
	),
}