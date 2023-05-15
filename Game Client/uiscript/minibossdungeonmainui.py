import uiScriptLocale

ROOT_PATH		= "d:/ymir work/ui/minigame/miniboss/"
PATTERN_PATH	= "d:/ymir work/ui/pattern/"
PUBLIC_PATH		= "d:/ymir work/ui/public/"
WINDOW_WIDTH	= 328
WINDOW_HEIGHT	= 404
MAIN_WIDTH		= 306
MAIN_HEIGHT		= 309

# 공통
window = {
	"name" : "MiniBossDungeonMainUI", "style" : ("movable", "float",),
	"x" : SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2, "y" : SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	"width" : WINDOW_WIDTH, "height" : WINDOW_HEIGHT,
	"children" :
	[
		# 제목표시줄
		{
			"name" : "board", "type" : "board_with_titlebar",
			"x" : 0, "y" : 0,
			"width" : WINDOW_WIDTH, "height" : WINDOW_HEIGHT,
			"title" : uiScriptLocale.MINI_BOSS_TITLE,
		},
		# 메인 BG : 테두리 및 검은배경
		{
			"name" : "main_bg_window", "type" : "window", "style" : ("ltr",),
			"x" : 10, "y" : 58,
			"width" : MAIN_WIDTH, "height" : MAIN_HEIGHT,
			"children" :
			[
				{
					"name" : "left_top", "type" : "image",
					"x" : 0, "y" : 0,
					"image" : PATTERN_PATH + "border_A_left_top.tga",
				},
				{
					"name" : "right_top", "type" : "image",
					"x" : MAIN_WIDTH - 16, "y" : 0,
					"image" : PATTERN_PATH + "border_A_right_top.tga",
				},
				{
					"name" : "left_bottom", "type" : "image",
					"x" : 0, "y" : MAIN_HEIGHT - 16,
					"image" : PATTERN_PATH + "border_A_left_bottom.tga",
				},
				{
					"name" : "right_bottom", "type" : "image",
					"x" : MAIN_WIDTH - 16, "y" : MAIN_HEIGHT - 16,
					"image" : PATTERN_PATH + "border_A_right_bottom.tga",
				},
				{
					"name" : "top", "type" : "expanded_image",
					"x" : 16, "y" : 0,
					"image" : PATTERN_PATH + "border_A_top.tga",
					"rect" : [0.0, 0.0, 17, 0],
				},
				{
					"name" : "left", "type" : "expanded_image",
					"x" : 0, "y" : 16,
					"image" : PATTERN_PATH + "border_A_left.tga",
					"rect" : [0.0, 0.0, 0, 17],
				},
				{
					"name" : "right", "type" : "expanded_image",
					"x" : MAIN_WIDTH - 16, "y" : 16,
					"image" : PATTERN_PATH + "border_A_right.tga",
					"rect" : [0.0, 0.0, 0, 17],
				},
				{
					"name" : "bottom", "type" : "expanded_image",
					"x" : 16, "y" : MAIN_HEIGHT - 16,
					"image" : PATTERN_PATH + "border_A_bottom.tga",
					"rect" : [0.0, 0.0, 17, 0],
				},
				{
					"name" : "center", "type" : "expanded_image",
					"x" : 16, "y" : 16,
					"image" : PATTERN_PATH + "border_A_center.tga",
					"rect" : [0.0, 0.0, 17, 17],
				},
			],
		},
		# 탭 메뉴
		{
			"name" : "tab_window", "type" : "window",
			"x" : 37, "y" : 32,
			"width" :  252 , "height" : 21,
			"children" :
			[
				{
					"name" : "tab_menu_1", "type" : "image",
					"x" : 0, "y" : 0,
					"image" : ROOT_PATH + "tab_menu_button1.sub",
				},
				{
					"name" : "tab_menu_2", "type" : "image",
					"x" : 0, "y" : 0,
					"image" : ROOT_PATH + "tab_menu_button2.sub",
				},
				{
					"name" : "tab_menu_3", "type" : "image",
					"x" : 0, "y" : 0,
					"image" : ROOT_PATH + "tab_menu_button3.sub",
				},
				{
					"name" : "tab_menu_button_1", "type" : "radio_button",
					"x" : 10, "y" : 5,
					"width" : 70, "height" : 21,
				},
				{
					"name" : "tab_menu_button_2", "type" : "radio_button",
					"x" : 91, "y" : 5,
					"width" : 70, "height" : 21,
				},
				{
					"name" : "tab_menu_button_3", "type" : "radio_button",
					"x" : 172, "y" : 5,
					"width" : 70, "height" : 21,
				},
			],
		},
		# 도움말 버튼
		{
			"name" : "desc_button", "type" : "button",
			"x" : 10, "y" : 371,
			"text" : uiScriptLocale.MINI_BOSS_DUNGEON_DESC_BTN,
			"default_image"	: PUBLIC_PATH + "large_button_01.sub",
			"over_image"	: PUBLIC_PATH + "large_button_02.sub",
			"down_image"	: PUBLIC_PATH + "large_button_03.sub",
		},
		# 기여도 아이콘
		{
			"name" : "point_image", "type" : "image",
			"x" : 13, "y" : 370,
			"image" : ROOT_PATH + "boss_ranking/point_icon.sub",
		},
		# 기여도 텍스트
		{
			"name" : "point_text_bg", "type" : "image",
			"x" : 43, "y" : 373,
			"image" : ROOT_PATH + "boss_ranking/point_bg.sub",
			"children" :
			[
				{
					"name" : "point_text", "type" : "text",
					"x" : 0, "y" : 0,
					"text" : "0", "all_align" : "center",
				},
			],
		},
		# 젬스톤 아이콘
		{
			"name" : "gem_image", "type" : "image",
			"x" : 148, "y" : 376,
			"image" : ROOT_PATH + "boss_ranking/gem_icon.sub",
		},
		# 젬스톤 텍스트
		{
			"name" : "gem_text_bg", "type" : "image",
			"x" : 170, "y" : 373,
			"image" : ROOT_PATH + "boss_ranking/gem_bg.sub",
			"children" :
			[
				{
					"name" : "gem_text", "type" : "text",
					"x" : 0, "y" : 0,
					"text" : "0", "all_align" : "center",
				},
			],
		},		
		# 입장 버튼
		{
			"name" : "enter_button", "type" : "button",
			"x" : 228, "y" : 371,
			"default_image"	: ROOT_PATH + "btn_enter1.sub",
			"over_image"	: ROOT_PATH + "btn_enter2.sub",
			"down_image"	: ROOT_PATH + "btn_enter3.sub",
		},
	],
}

# 보스정보
window["children"] = window["children"] + [
	{
		"name" : "boss_info_page", "type" : "window",
		"x" : 10, "y" : 58,
		"width" : MAIN_WIDTH, "height" : MAIN_HEIGHT,
		"children" :
		[
			{
				"name" : "blue_count_bg", "type" : "image",
				"x" : 47, "y" : 6,
				"image" : ROOT_PATH + "boss_info/count_bg_blue.sub",
				"children" :
				[
					{
						"name" : "blue_count_text", "type" : "text",
						"x" : 0, "y" : 0,
						"text" : "0/1", "all_align" : "center",
						"r" : 91.0 / 255.0, "g" : 207.0 / 255.0, "b" : 1.0, "a" : 1.0,
					},
				],
			},
			{
				"name" : "blue_time_bg", "type" : "image",
				"x" : 87, "y" : 6,
				"image" : ROOT_PATH + "boss_info/time_bg_blue.sub",
				"children" :
				[
					{
						"name" : "blue_time", "type" : "text",
						"x" : 0, "y" : 0,
						"text" : "00:00", "all_align" : "center",
						"r" : 91.0 / 255.0, "g" : 207.0 / 255.0, "b" : 1.0, "a" : 1.0,
					},
				],
			},
			{
				"name" : "red_count_bg", "type" : "image",
				"x" : 199, "y" : 6,
				"image" : ROOT_PATH + "boss_info/count_bg_red.sub",
				"children" :
				[
					{
						"name" : "red_count_text", "type" : "text",
						"x" : 0, "y" : 0,
						"text" : "0/1", "all_align" : "center",
						"r" : 1.0, "g" : 194.0 / 255.0, "b" : 91.0 / 255.0, "a" : 1.0,
					},
				],
			},
			{
				"name" : "red_time_bg", "type" : "image",
				"x" : 239, "y" : 6,
				"image" : ROOT_PATH + "boss_info/time_bg_red.sub",
				"children" :
				[
					{
						"name" : "red_time", "type" : "text",
						"x" : 0, "y" : 0,
						"text" : "00:00", "all_align" : "center",
						"r" : 1.0, "g" : 194.0 / 255.0, "b" : 91.0 / 255.0, "a" : 1.0,
					},
				],
			},
			{
				"name" : "line", "type" : "expanded_image",
				"x" : 152, "y" : 6, "vertical_align" : "top", 
				"x_scale" : 1.0, "y_scale" : 4.0,
				"image" : ROOT_PATH + "boss_info/line.sub",
			},
		],
	},
]

# 랭킹
window["children"] = window["children"] + [
	{
		"name" : "ranking_page", "type" : "window",
		"x" : 10, "y" : 58,
		"width" : MAIN_WIDTH, "height" : MAIN_HEIGHT,
		"children" :
		[
			# 타이틀
			{
				"name" : "ranking_title", "type" : "image",
				"x" : 3, "y" : 3,
				"image" : ROOT_PATH + "boss_ranking/title.sub",
				"children" :
				[
					{
						"name" : "title_number_window", "type" : "window",
						"x" : 11, "y" : 0,
						"width" : 44, "height" : 18,
						"children" :
						[
							{
								"name" : "title_number", "type" : "text",
								"x" : 0, "y" : 0,
								"text" : uiScriptLocale.MINI_BOSS_DUNGEON_TITLE_NUM, "all_align" : "center",
							},
						],
					},
					{
						"name" : "title_name_window", "type" : "window",
						"x" : 71, "y" : 0,
						"width" : 115, "height" : 18,
						"children" :
						[
							{
								"name" : "title_name", "type" : "text",
								"x" : 0, "y" : 0,
								"text" : uiScriptLocale.MINI_BOSS_DUNGEON_TITLE_NAME, "all_align" : "center",
							},
						],
					},
					{
						"name" : "title_point_window", "type" : "window",
						"x" : 211, "y" : 0,
						"width" : 62, "height" : 18,
						"children" :
						[
							{
								"name" : "title_point", "type" : "text",
								"x" : 0, "y" : 0,
								"text" : uiScriptLocale.MINI_BOSS_DUNGEON_TITLE_POINT, "all_align" : "center",
							},
						],
					},
				],
			},
			# 공백
			{
				"name" : "ranking_space", "type" : "image",
				"x" : 153, "y" : 273,
				"image" : ROOT_PATH + "boss_ranking/ranking_space.sub",
			},
			# 나의 랭킹
			{
				"name" : "ranking_list", "type" : "image",
				"x" : 5, "y" : 280,
				"image" : ROOT_PATH + "boss_ranking/list2.sub",
				"children" :
				[
					{
						"name" : "ranking_number_window", "type" : "window",
						"x" : 11, "y" : 2,
						"width" : 44, "height" : 18,
						"children" :
						[
							{
								"name" : "ranking_number", "type" : "text",
								"x" : 0, "y" : 0,
								"text" : "", "all_align" : "center",
							},
						],
					},
					{
						"name" : "ranking_name_window", "type" : "window",
						"x" : 71, "y" : 2,
						"width" : 115, "height" : 18,
						"children" :
						[
							{
								"name" : "ranking_name", "type" : "text",
								"x" : 0, "y" : 0,
								"text" : "", "all_align" : "center",
							},
						],
					},
					{
						"name" : "ranking_point_window", "type" : "window",
						"x" : 209, "y" : 2,
						"width" : 62, "height" : 18,
						"children" :
						[
							{
								"name" : "ranking_point", "type" : "text",
								"x" : 0, "y" : 0,
								"text" : "", "all_align" : "center",
							},
						],
					},
				],
			},
		],
	},
]

# 반복되는거 for문으로
for j in xrange(len(window["children"])):
	# 보스정보
	if window["children"][j]["name"] == "boss_info_page":
		for i in xrange(12):	#보스 12마리
			window["children"][j]["children"] = window["children"][j]["children"] + [
				{
					"name" : "blue_icon_%d" % i, "type" : "image",
					"x" : 11, "y" : 27 + 23 * i,
					"image" : ROOT_PATH + "boss_info/icon_on_%d.sub" % (i + 1),
				},
				{
					"name" : "red_icon_%d" % i, "type" : "image",
					"x" : 163, "y" : 27 + 23 * i,
					"image" : ROOT_PATH + "boss_info/icon_on_%d.sub" % (i + 1),
				},
				{
					"name" : "blue_gauge_bg_%d" % i, "type" : "image",
					"x" : 44, "y" : 33 + 23 * i,
					"image" : ROOT_PATH + "boss_info/gauge_bg.sub",
					"children" :
					[
						{
							"name" : "blue_gauge_%d" % i, "type" : "expanded_image",
							"x" : 8, "y" : 2,
							"image" : ROOT_PATH + "boss_info/gauge_blue.sub",
						},
					],
				},
				{
					"name" : "red_gauge_bg_%d" % i, "type" : "image",
					"x" : 196, "y" : 33 + 23 * i,
					"image" : ROOT_PATH + "boss_info/gauge_bg.sub",
					"children" :
					[
						{
							"name" : "red_gauge_%d" % i, "type" : "expanded_image",
							"x" : 8, "y" : 2,
							"image" : ROOT_PATH + "boss_info/gauge_red.sub",
						},
					],
				},
			]
	# 랭킹
	elif window["children"][j]["name"] == "ranking_page":
		for i in xrange(10):	#1~10위
			window["children"][j]["children"] = window["children"][j]["children"] + [
				{
					"name" : "ranking_list_%d" % i, "type" : "image",
					"x" : 5, "y" : 24 + 25 * i,
					"image" : ROOT_PATH + "boss_ranking/list1.sub",
					"children" :
					[
						{
							"name" : "ranking_number_window_%d" % i, "type" : "window",
							"x" : 11, "y" : 2,
							"width" : 44, "height" : 18,
							"children" :
							[
								{
									"name" : "ranking_number_%d" % i, "type" : "text",
									"x" : 0, "y" : 0,
									"text" : "", "all_align" : "center",
								},
							],
						},
						{
							"name" : "ranking_name_window_%d" % i, "type" : "window",
							"x" : 71, "y" : 2,
							"width" : 115, "height" : 18,
							"children" :
							[
								{
									"name" : "ranking_name_%d" % i, "type" : "text",
									"x" : 0, "y" : 0,
									"text" : "", "all_align" : "center",
								},
							],
						},
						{
							"name" : "ranking_point_window_%d" % i, "type" : "window",
							"x" : 209, "y" : 2,
							"width" : 62, "height" : 18,
							"children" :
							[
								{
									"name" : "ranking_point_%d" % i, "type" : "text",
									"x" : 0, "y" : 0,
									"text" : "", "all_align" : "center",
								},
							],
						},
					],
				},
			]
