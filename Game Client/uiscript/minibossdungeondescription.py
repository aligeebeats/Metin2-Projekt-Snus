import uiScriptLocale

ROOT_PATH		= "d:/ymir work/ui/minigame/miniboss/"
PUBLIC_PATH		= "d:/ymir work/ui/public/"
WINDOW_WIDTH	= 392
WINDOW_HEIGHT	= 336
BOARD_WIDTH		= 362
BOARD_HEIGHT	= 250

window = {
	"name" : "MiniBossDungeonDescription", "style" : ("movable",),
	"x" : SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2, "y" : SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	"width" : WINDOW_WIDTH, "height" : WINDOW_HEIGHT,
	"children" :
	[
		{
			"name" : "board", "type" : "board_with_titlebar",
			"x" : 0, "y" : 0,
			"width" : WINDOW_WIDTH, "height" : WINDOW_HEIGHT,
			"title" : uiScriptLocale.MINI_BOSS_TITLE,
			"children" :
			[
				{
					"name" : "desc_board", "type" : "bar",
					"x" : 15, "y" : 40,
					"width" : BOARD_WIDTH, "height" : BOARD_HEIGHT,
					"children" :
					[
						{
							"name" : "right_line", "type" : "line",
							"x" : BOARD_WIDTH - 1, "y" : 0,
							"width" : 0, "height" : BOARD_HEIGHT,
							"color" : 0xffAAA6A1,
						},
						{
							"name" : "bottom_line", "type" : "line",
							"x" : 0, "y" : BOARD_HEIGHT - 1,
							"width" : BOARD_WIDTH, "height" : 0,
							"color" : 0xffAAA6A1,
						},
						{
							"name" : "left_line", "type" : "line",
							"x" : 0, "y" : 0,
							"width" : 0, "height" : BOARD_HEIGHT,
							"color" : 0xff2A2521,
						},
						{
							"name" : "top_line", "type" : "line",
							"x" : 0, "y" : 0,
							"width" : BOARD_WIDTH, "height" : 0,
							"color" : 0xff2A2521,
						},
					],
				},
				{
					"name" : "prev_button", "type" : "button",
					"x" : 17, "y" : WINDOW_HEIGHT - 33,
					"default_image"	: PUBLIC_PATH + "public_intro_btn/prev_btn_01.sub",
					"over_image"	: PUBLIC_PATH + "public_intro_btn/prev_btn_02.sub",
					"down_image"	: PUBLIC_PATH + "public_intro_btn/prev_btn_01.sub",
				},
				{
					"name" : "next_button", "type" : "button",
					"x" : 42, "y" : WINDOW_HEIGHT - 33,
					"default_image"	: PUBLIC_PATH + "public_intro_btn/next_btn_01.sub",
					"over_image"	: PUBLIC_PATH + "public_intro_btn/next_btn_02.sub",
					"down_image"	: PUBLIC_PATH + "public_intro_btn/next_btn_01.sub",
				},
				{
					"name" : "game_start_button", "type" : "button",
					"x" : WINDOW_WIDTH - 103, "y" : WINDOW_HEIGHT - 37,
					"default_image"	: ROOT_PATH + "btn_enter1.sub",
					"over_image"	: ROOT_PATH + "btn_enter2.sub",
					"down_image"	: ROOT_PATH + "btn_enter3.sub",
				},
			],
		},
	],
}
