import uiScriptLocale

ROOT = "d:/ymir work/ui/game/"
PUBLIC_PATH = "d:/ymir work/ui/public/"

WINDOW_WIDTH	= 352
WINDOW_HEIGHT	= 384

BOARD_WIDTH		= 322
BOARD_HEIGHT	= 280

window = {
	"name" : "RumiWaitingPage",
	"style" : ("movable",),
	
	"x" : SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y" : SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	
	"width" : WINDOW_WIDTH,
	"height" : WINDOW_HEIGHT,
	
	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),
			
			"x" : 0,
			"y" : 0,
			
			"width" : WINDOW_WIDTH,
			"height" : WINDOW_HEIGHT,
			
			"children" :
			(
				{
					"name" : "titlebar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 0,
					"y" : 0,

					"width" : WINDOW_WIDTH,
					"color" : "yellow",

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":0, "y":0, "text": uiScriptLocale.MINI_GAME_RUMI_TITLE, "all_align":"center" },
					),
				},
		
				{
					"name" : "desc_board",
					"type" : "bar",
					
					"x" : 15,
					"y" : 30,

					"width" : BOARD_WIDTH,
					"height" : BOARD_HEIGHT,
					
					"children" :
					(
						{
							"name" : "right_line",
							"type" : "line",

							"x" : BOARD_WIDTH-1,
							"y" : 0,

							"width" : 0,
							"height" : BOARD_HEIGHT,

							"color" : 0xffAAA6A1,
						},
						
						{
							"name" : "bottom_line",
							"type" : "line",

							"x" : 0,
							"y" : BOARD_HEIGHT-1,

							"width" : BOARD_WIDTH,
							"height" : 0,

							"color" : 0xffAAA6A1,
						},
						{
							"name" : "left_line",
							"type" : "line",

							"x" : 0,
							"y" : 0,

							"width" : 0,
							"height" : BOARD_HEIGHT,

							"color" : 0xff2A2521,
						},
						{
							"name" : "top_line",
							"type" : "line",

							"x" : 0,
							"y" : 0,

							"width" : BOARD_WIDTH,
							"height" : 0,

							"color" : 0xff2A2521,
						},
						
					),
				
				},
				
				{
					"name" : "prev_button",
					"type" : "button",

					"x" : WINDOW_WIDTH -30 -20 -20 -10,
					"y" : 60,
					
					"vertical_align" : "bottom",
					
					"default_image" : PUBLIC_PATH + "public_intro_btn/prev_btn_01.sub",
					"over_image" : PUBLIC_PATH + "public_intro_btn/prev_btn_02.sub",
					"down_image" : PUBLIC_PATH + "public_intro_btn/prev_btn_01.sub",
				},
				
				{
					"name" : "next_button",
					"type" : "button",

					"x" : WINDOW_WIDTH - 30 -20,
					"y" : 60,
					
					"vertical_align" : "bottom",
					
					"default_image" : PUBLIC_PATH + "public_intro_btn/next_btn_01.sub",
					"over_image" : PUBLIC_PATH + "public_intro_btn/next_btn_02.sub",
					"down_image" : PUBLIC_PATH + "public_intro_btn/next_btn_01.sub",
				},
				
				{
					"name" : "game_start_button",
					"type" : "button",
					
					"x" : 40,
					"y" : 40,
					
					"text" : uiScriptLocale.MINI_GAME_RUMI_START_TEXT,
					
					"vertical_align" : "bottom",
					"horizontal_align" : "left",
					
					
					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
				},
				
				{
					"name" : "confirm_check_button_text_window",
					"type" : "window",
					"style" : ("attach",),
					
					"x" : 275,
					"y" : 348,
					
					"width" : 29,
					"height" : 18,
					
					"children" :
					(
						{
							"name" : "confirm_check_button_text",
							"type" : "text",
							
							"x" : 0,
							"y" : 0,
							
							"text_horizontal_align" : "right",
							
							"text" : uiScriptLocale.MINI_GAME_RUMI_DISCARD_TEXT,
						},
					),
				},
				
				{
					"name" : "confirm_check_button",
					"type" : "expanded_image",
					
					"x" : 285,
					"y" : 344,
					
					"image" : "d:/ymir work/ui/public/Parameter_Slot_07.sub",
				},
				{
					"name" : "check_image",
					"type" : "expanded_image",
					"style" : ("not_pick",),
					
					"x" : 290,
					"y" : 344,
					
					"image" : "d:/ymir work/ui/public/check_image.sub",
				},
				
			),
		},
	),	
}
