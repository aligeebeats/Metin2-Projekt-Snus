import uiScriptLocale

ROOT_PATH		= "d:/ymir work/ui/minigame/findm/"
WINDOW_WIDTH	= 456
WINDOW_HEIGHT	= 338

window = {
	"name" : "MiniGameFindMMainUI", "style" : ("movable", "float",),
	"x" : SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2, "y" : SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	"width" : WINDOW_WIDTH, "height" : WINDOW_HEIGHT,
	"children" :
	[
		# 제목표시줄
		{
			"name" : "board", "type" : "board_with_titlebar",
			"x" : 0, "y" : 0,
			"width" : WINDOW_WIDTH, "height" : WINDOW_HEIGHT,
			"title" : uiScriptLocale.MINI_GAME_FINDM_TITLE,
		},
		# 메인 BG
		{
			"name" : "main_bg", "type" : "image", "style" : ("ltr",),
			"x" : 10, "y" : 32,
			"image" : ROOT_PATH + "main_bg.sub",
			"children" :
			[
				# 시간
				{
					"name" : "time_window", "type" : "window",
					"x" : 169, "y" : 9,
					"width" : 98, "height" : 21,
					"children" :
					[
						{
							"name" : "time", "type" : "text",
							"x" : 0, "y" : 0,
							"text" : "00:00",  "all_align" : "center",
						},
					],
				}
			],
		},
		# 힌트
		{
			"name" : "hint", "type" : "button",
			"x" : 11, "y" : 303,
			"text" : uiScriptLocale.MINI_GAME_FINDM_HINT_BTN,
			"default_image"	: ROOT_PATH + "hint_button_01.sub",
			"over_image"	: ROOT_PATH + "hint_button_02.sub",
			"down_image"	: ROOT_PATH + "hint_button_03.sub",
		},
		# 힌트 갯수
		{
			"name" : "hint_count_image", "type" : "image", "style" : ("ltr",),
			"x" : 104, "y" : 304,
			"image" : ROOT_PATH + "hint_count.sub",
			"children" :
			[
				{
					"name" : "hint_count_text_window", "type" : "window",
					"x" : 22, "y" : 3,
					"width" : 34, "height" : 16,
					"children" :
					[
						{
							"name" : "hint_count", "type" : "text",
							"x" : 3, "y" : 0,
							"text" : "0",
							"vertical_align" : "center", "horizontal_align" : "right",
							"text_vertical_align" : "center", "text_horizontal_align" : "right",
						},
					],
				},
			],
		},
		# 카운트다운
		{
			"name" : "count_down_window", type : "window",
			"x" : 203, "y" : 306,
			"width" : 55, "height" : 18,
			"children" :
			[
				{
					"name" : "count_down_desc", "type" : "image",
					"x" : 0, "y" : 0,
					"image" : "d:/ymir work/ui/pattern/q_mark_02.tga",
				},
				{
					"name" : "count_down_image", "type" : "image",
					"x" : 20, "y" : 0,
					"image" : ROOT_PATH + "try_count.sub",
					"children" :
					[
						{
							"name" : "count_down", "type" : "text",
							"x" : 0, "y" : 0,
							"text" : "00",
							"all_align" : "center",
						},
					],
				},
			],
		},
		# 시도횟수
		{
			"name" : "try_count_window", type : "window",
			"x" : 349, "y" : 306,
			"width" : 55, "height" : 18,
			"children" :
			[
				{
					"name" : "try_count_title", "type" : "text",
					"x" : 2, "y" : 0,
					"text" : uiScriptLocale.MINI_GAME_FINDM_TRY_COUNT_TEXT,
					"vertical_align" : "center", "horizontal_align" : "right",
					"text_vertical_align" : "center", "text_horizontal_align" : "right",
				},
				{
					"name" : "try_count_image", "type" : "image",
					"x" : 57, "y" : 0,
					"image" : ROOT_PATH + "try_count.sub",
					"children" :
					[
						{
							"name" : "try_count", "type" : "text",
							"x" : 4, "y" : 0,
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
