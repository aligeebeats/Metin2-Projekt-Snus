import uiScriptLocale

LOCALE_PATH = uiScriptLocale.WINDOWS_PATH
ROOT = "d:/ymir work/ui/game/"

ROOT_PATH = "d:/ymir work/ui/game/monster_card/"
BUTTON_PATH = "d:/ymir work/ui/game/monster_card/button/"
PATTERN_PATH = "d:/ymir work/ui/pattern/"
CARD_PATH = "d:/ymir work/ui/game/monster_card/card/"

BG_WIDTH	= 800
BG_HEIGHT	= 564

## TEXT COLOR
GOLD_COLOR	= 0xFFFEE3AE
WHITE_COLOR = 0xFFFFFFFF

TAB_MENU_WIDTH	= 511
TAB_MENU_HEIGHT = 43
TAB_MEMU_BUTTON_WIDTH = 108
TAB_MEMU_BUTTON_HEIGHT = 30

MAIN_WINDOW_WIDTH	= 756
MAIN_WINDOW_HEIGHT	= 469

PATTERN_X_COUNT = 45
PATTERN_Y_COUNT = 27

PATTERN2_X_COUNT = 40
PATTERN2_Y_COUNT = 10

WAIT_CARD_WINDOW_WIDTH	= 682
WAIT_CARD_WINDOW_HEIGHT = 204
WAIT_CARD_COVER_ALPHA = 0.5

ILLUSTRATED_CARD_GAP = 29

STAR_WINDOW_WIDTH = 15 * 5 + 1 * 4

MODEL_VIEW_BG_WIDTH		= 246
MODEL_VIEW_BG_HEIGHT	= 443
MODEL_VIEW_X_COUNT	= 13
MODEL_VIEW_Y_COUNT	= 25

window = {
	"name" : "MonsterCardWindow",
	"style" : ("movable", "float",),
	
	"x" : 0,
	"y" : 0,

	"width" : BG_WIDTH,
	"height" : BG_HEIGHT,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board_with_titlebar",

			"x" : 0,
			"y" : 0,

			"width" : BG_WIDTH,
			"height" : BG_HEIGHT,
			
			"title" : uiScriptLocale.KEYCHANGE_MONSTER_CARD_WINDOW,
		},
		
		## main bg
		{
			"name" : "main_bg_window",
			"type" : "window",
			
			"style" : ("attach", "ltr",),

			"x" : 21,
			"y" : 75,
			
			"width" : MAIN_WINDOW_WIDTH,
			"height" : MAIN_WINDOW_HEIGHT,
			
			"children" :
			(
				## LeftTop 1
				{
					"name" : "LeftTop",
					"type" : "image",
					"style" : ("ltr",),
					
					"x" : 0,
					"y" : 0,
					"image" : PATTERN_PATH + "border_A_left_top.tga",
				},
				## RightTop 2
				{
					"name" : "RightTop",
					"type" : "image",
					"style" : ("ltr",),
					
					"x" : MAIN_WINDOW_WIDTH - 16,
					"y" : 0,
					"image" : PATTERN_PATH + "border_A_right_top.tga",
				},
				## LeftBottom 3
				{
					"name" : "LeftBottom",
					"type" : "image",
					"style" : ("ltr",),
					
					"x" : 0,
					"y" : MAIN_WINDOW_HEIGHT - 16,
					"image" : PATTERN_PATH + "border_A_left_bottom.tga",
				},
				## RightBottom 4
				{
					"name" : "RightBottom",
					"type" : "image",
					"style" : ("ltr",),
					
					"x" : MAIN_WINDOW_WIDTH - 16,
					"y" : MAIN_WINDOW_HEIGHT - 16,
					"image" : PATTERN_PATH + "border_A_right_bottom.tga",
				},
				## topcenterImg 5
				{
					"name" : "TopCenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 16,
					"y" : 0,
					"image" : PATTERN_PATH + "border_A_top.tga",
					"rect" : (0.0, 0.0, PATTERN_X_COUNT, 0),
				},
				## leftcenterImg 6
				{
					"name" : "LeftCenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 0,
					"y" : 16,
					"image" : PATTERN_PATH + "border_A_left.tga",
					"rect" : (0.0, 0.0, 0, PATTERN_Y_COUNT),
				},
				## rightcenterImg 7
				{
					"name" : "RightCenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : MAIN_WINDOW_WIDTH - 16,
					"y" : 16,
					"image" : PATTERN_PATH + "border_A_right.tga",
					"rect" : (0.0, 0.0, 0, PATTERN_Y_COUNT),
				},
				## bottomcenterImg 8
				{
					"name" : "BottomCenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 16,
					"y" : MAIN_WINDOW_HEIGHT - 16,
					"image" : PATTERN_PATH + "border_A_bottom.tga",
					"rect" : (0.0, 0.0, PATTERN_X_COUNT, 0),
				},
				## centerImg
				{
					"name" : "CenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 16,
					"y" : 16,
					"image" : PATTERN_PATH + "border_A_center.tga",
					"rect" : (0.0, 0.0, PATTERN_X_COUNT, PATTERN_Y_COUNT),
				},
			),
		},
		
		{
			"name" : "tab_window",
			"type" : "window",
			
			"x" : 21,
			"y" : 35,
			"width" : TAB_MENU_WIDTH,
			"height" : TAB_MENU_HEIGHT,
			
			"children" :
			(
				## tab image
				{
					"name" : "tab_menu_1",
					"type" : "image",
					
					"x" : 0,
					"y" : 0,

					"image" : BUTTON_PATH+"tab/tab_menu_button1.sub",
				},
				{
					"name" : "tab_menu_2",
					"type" : "image",
					
					"x" : 0,
					"y" : 0,

					"image" : BUTTON_PATH+"tab/tab_menu_button2.sub",
				},
				{
					"name" : "tab_menu_3",
					"type" : "image",
					
					"x" : 0,
					"y" : 0,

					"image" : BUTTON_PATH+"tab/tab_menu_button3.sub",
				},
				
				## RadioButton
				{
					"name" : "tab_menu_button_1",
					"type" : "radio_button",

					"x" : 18,
					"y" : 10,

					"width" : TAB_MEMU_BUTTON_WIDTH,
					"height" : TAB_MEMU_BUTTON_HEIGHT,
				},
				{
					"name" : "tab_menu_button_2",
					"type" : "radio_button",

					"x" : 139,
					"y" : 10,

					"width" : TAB_MEMU_BUTTON_WIDTH,
					"height" : TAB_MEMU_BUTTON_HEIGHT,
				},
				{
					"name" : "tab_menu_button_3",
					"type" : "radio_button",

					"x" : 260,
					"y" : 10,

					"width" : TAB_MEMU_BUTTON_WIDTH,
					"height" : TAB_MEMU_BUTTON_HEIGHT,
				},
			),			
		},
		
		## Page Area
		{
			"name" : "mission_page",
			"type" : "window",
			"style" : ("attach", "ltr",),

			"x" : 21,
			"y" : 75,

			"width" : MAIN_WINDOW_WIDTH,
			"height" : MAIN_WINDOW_HEIGHT,
			
			"children" :
			(
				## mission_alter bg pattern
				{
					"name" : "mission_alter_bg_window",
					"type" : "window",
					"style" : ("attach", "ltr",),

					"x" : 17,
					"y" : 10,

					"width" : 720,
					"height" : 18,
					"children" :
					(
						{
							"name" : "MissionAlterLeftImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 0,
							"image" : PATTERN_PATH + "border_c_left.tga",
						},
						{
							"name" : "MissionAlterCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 21,
							"y" : 0,
							"image" : PATTERN_PATH + "border_c_middle.tga",
							"rect" : (0.0, 0.0, 32, 0),
						},
						{
							"name" : "MissionAlterRightImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 720-21,
							"y" : 0,
							"image" : PATTERN_PATH + "border_c_right.tga",
						},
						## Text
						{
							"name" : "MissionAlterText",
							"type" : "text",
							"x" : 0,
							"y" : 1,
							"all_align" : "center",
							"text" : "1 단계 미션",
						},
					),
				},
				
				## wait card bg pattern
				{
					"name" : "wait_card_bg_window",
					"type" : "window",
					"style" : ("attach", "ltr",),

					"x" : 37,	#58 - 21
					"y" : 221,	#296 - 75

					"width" : WAIT_CARD_WINDOW_WIDTH,
					"height" : WAIT_CARD_WINDOW_HEIGHT,
					"children" :
					(
						## LeftTop 1
						{
							"name" : "WC_LeftTop",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 0,
							"image" : PATTERN_PATH + "border_B_left_top.tga",
						},
						## RightTop 2
						{
							"name" : "WC_RightTop",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : WAIT_CARD_WINDOW_WIDTH - 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_B_right_top.tga",
						},
						## LeftBottom 3
						{
							"name" : "WC_LeftBottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : WAIT_CARD_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_B_left_bottom.tga",
						},
						## RightBottom 4
						{
							"name" : "WC_RightBottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : WAIT_CARD_WINDOW_WIDTH - 16,
							"y" : WAIT_CARD_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_B_right_bottom.tga",
						},
						## topcenterImg 5
						{
							"name" : "WC_TopCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_B_top.tga",
							"rect" : (0.0, 0.0, PATTERN2_X_COUNT, 0),
						},
						## leftcenterImg 6
						{
							"name" : "WC_LeftCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 16,
							"image" : PATTERN_PATH + "border_B_left.tga",
							"rect" : (0.0, 0.0, 0, PATTERN2_Y_COUNT),
						},
						## rightcenterImg 7
						{
							"name" : "WC_RightCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : WAIT_CARD_WINDOW_WIDTH - 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_B_right.tga",
							"rect" : (0.0, 0.0, 0, PATTERN2_Y_COUNT),
						},
						## bottomcenterImg 8
						{
							"name" : "WC_BottomCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : WAIT_CARD_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_B_bottom.tga",
							"rect" : (0.0, 0.0, PATTERN2_X_COUNT, 0),
						},
						## centerImg
						{
							"name" : "WC_CenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_B_center.tga",
							"rect" : (0.0, 0.0, PATTERN2_X_COUNT, PATTERN2_Y_COUNT),
						},
						
						## wait card window
						{
							"name" : "wait_card_window",
							"type" : "window",
							"style" : ("attach",),

							"x" : 20,
							"y" : 15,

							"width" : 69*8 + 13*7,
							"height" : 84*2 + 6,
						},
					),
				},
				
				{
					"name" : "wait_card_alpha_bg_window",
					"type" : "window",
					"style" : ("attach", "ltr",),

					"x" : 37,	#58 - 21
					"y" : 221,	#296 - 75

					"width" : WAIT_CARD_WINDOW_WIDTH,
					"height" : WAIT_CARD_WINDOW_HEIGHT,
					"children" :
					(
						## LeftTop 1
						{
							"name" : "WC_alpha_LeftTop",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 0,
							"image" : PATTERN_PATH + "border_B_left_top.tga",
							"alpha" : WAIT_CARD_COVER_ALPHA
						},
						## RightTop 2
						{
							"name" : "WC_alpha_RightTop",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : WAIT_CARD_WINDOW_WIDTH - 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_B_right_top.tga",
							"alpha" : WAIT_CARD_COVER_ALPHA
						},
						## LeftBottom 3
						{
							"name" : "WC_alpha_LeftBottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : WAIT_CARD_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_B_left_bottom.tga",
							"alpha" : WAIT_CARD_COVER_ALPHA
						},
						## RightBottom 4
						{
							"name" : "WC_alpha_RightBottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : WAIT_CARD_WINDOW_WIDTH - 16,
							"y" : WAIT_CARD_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_B_right_bottom.tga",
							"alpha" : WAIT_CARD_COVER_ALPHA
						},
						## topcenterImg 5
						{
							"name" : "WC_alpha_TopCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_B_top.tga",
							"rect" : (0.0, 0.0, PATTERN2_X_COUNT, 0),
							"alpha" : WAIT_CARD_COVER_ALPHA
						},
						## leftcenterImg 6
						{
							"name" : "WC_alpha_LeftCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 16,
							"image" : PATTERN_PATH + "border_B_left.tga",
							"rect" : (0.0, 0.0, 0, PATTERN2_Y_COUNT),
							"alpha" : WAIT_CARD_COVER_ALPHA
						},
						## rightcenterImg 7
						{
							"name" : "WC_alpha_RightCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : WAIT_CARD_WINDOW_WIDTH - 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_B_right.tga",
							"rect" : (0.0, 0.0, 0, PATTERN2_Y_COUNT),
							"alpha" : WAIT_CARD_COVER_ALPHA
						},
						## bottomcenterImg 8
						{
							"name" : "WC_alpha_BottomCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : WAIT_CARD_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_B_bottom.tga",
							"rect" : (0.0, 0.0, PATTERN2_X_COUNT, 0),
							"alpha" : WAIT_CARD_COVER_ALPHA
						},
						## centerImg
						{
							"name" : "WC_alpha_CenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_B_center.tga",
							"rect" : (0.0, 0.0, PATTERN2_X_COUNT, PATTERN2_Y_COUNT),
							"alpha" : WAIT_CARD_COVER_ALPHA
						},
					),
				},
				
				## selected card image
				## 초상화0
				{
					"name" : "selected_img0",
					"type" : "image",
					
					"x" : 180 - 21,
					"y" : 147 - 75,
					
					"image" : ROOT_PATH + "empty_card.sub",
				},
				## 미션 클리어 0
				{
					"name" : "selected_clear_img0",
					"type" : "image",
					
					"x" : 244 - 21,
					"y" : 233 - 75,
					
					"image" : ROOT_PATH + "clear_img.sub",
				},
				## 프레임 0
				{
					"name" : "selected_frame0",
					"type" : "image",
					"style" : ("attach",),
					
					"x" : 167 - 21,
					"y" : 136 - 75,
					"image" : ROOT_PATH + "card_frame.sub",
				},
				## 출현지역0
				{
					"name" : "selected_area_window0",
					"type" : "window",
					"style" : ("attach",),
					"x" : 177 - 21,
					"y" : 268 - 75,
					"width" : 98,
					"height" : 21,
					"children" :
					(
						{ "name" : "selected_area0", "type" : "image", "x" : 0, "y" : 0, "image" : ROOT_PATH + "area_bg.sub", },						
					),
				},
				## 초상화1
				{
					"name" : "selected_img1",
					"type" : "image",
					
					"x" : 354 - 21,
					"y" : 147 - 75,
					
					"image" : ROOT_PATH + "empty_card.sub",
				},
				## 미션 클리어 1
				{
					"name" : "selected_clear_img1",
					"type" : "image",
					
					"x" : 418 - 21,
					"y" : 233 - 75,
					
					"image" : ROOT_PATH + "clear_img.sub",
				},
				## 출현지역1
				{
					"name" : "selected_area_window1",
					"type" : "window",
					"style" : ("attach",),
					"x" : 351 - 21,
					"y" : 268 - 75,
					"width" : 98,
					"height" : 21,
					"children" :
					(
						{ "name" : "selected_area1", "type" : "image", "x" : 0, "y" : 0, "image" : ROOT_PATH + "area_bg.sub", },
					),
				},
				## 프레임 1
				{
					"name" : "selected_frame1",
					"type" : "image",
					"x" : 341 - 21,
					"y" : 136 - 75,
					"image" : ROOT_PATH + "card_frame.sub",
				},
				## 초상화2
				{
					"name" : "selected_img2",
					"type" : "image",
					
					"x" : 528 - 21,
					"y" : 147 - 75,
					
					"image" : ROOT_PATH + "empty_card.sub",
				},
				## 미션 클리어 2
				{
					"name" : "selected_clear_img2",
					"type" : "image",
					
					"x" : 592 - 21,
					"y" : 233 - 75,
					
					"image" : ROOT_PATH + "clear_img.sub",
				},
				## 출현지역2
				{
					"name" : "selected_area_window2",
					"type" : "window",
					"style" : ("attach",),
					"x" : 525 - 21,
					"y" : 268 - 75,
					"width" : 98,
					"height" : 21,
					"children" :
					(
						{ "name" : "selected_area2", "type" : "image", "x" : 0, "y" : 0, "image" : ROOT_PATH + "area_bg.sub", },
					),
				},
				## 프레임 2
				{
					"name" : "selected_frame2",
					"type" : "image",
					"x" : 515 - 21,
					"y" : 136 - 75,
					"image" : ROOT_PATH + "card_frame.sub",
				},
						
				## Buttons
				{
					## 미션 받기
					"name" : "recive_mission_button",
					"type" : "button",

					"x" : 534 - 21,
					"y" : 111 - 75,

					"default_image" : BUTTON_PATH + "recive_mission/recive_mission_button_default.sub",
					"over_image" : BUTTON_PATH + "recive_mission/recive_mission_button_over.sub",
					"down_image" : BUTTON_PATH + "recive_mission/recive_mission_button_down.sub",
				},
				{
					## 카드 배치
					"name" : "shuffle_card_button",
					"type" : "button",

					"x" : 655 - 21,
					"y" : 111 - 75,

					"default_image" : BUTTON_PATH + "shuffle_card/shuffle_card_button_default.sub",
					"over_image" : BUTTON_PATH + "shuffle_card/shuffle_card_button_over.sub",
					"down_image" : BUTTON_PATH + "shuffle_card/shuffle_card_button_down.sub",
				},
				{
					## 카드 받기
					"name" : "reward_card_button",
					"type" : "button",

					"x" : 534 - 21,
					"y" : 507 - 75,

					"default_image" : BUTTON_PATH + "reward_card/reward_card_default.sub",
					"over_image" : BUTTON_PATH + "reward_card/reward_card_over.sub",
					"down_image" : BUTTON_PATH + "reward_card/reward_card_down.sub",
				},
				{
					## 초기화
					"name" : "mission_init_button",
					"type" : "button",

					"x" : 655 - 21,
					"y" : 507 - 75,

					"default_image" : BUTTON_PATH + "mission_init/mission_init_button_default.sub",
					"over_image" : BUTTON_PATH + "mission_init/mission_init_button_over.sub",
					"down_image" : BUTTON_PATH + "mission_init/mission_init_button_down.sub",
				},
				{
					## 물음표
					"name" : "init_question_button",
					"type" : "button",

					"x" : 655 - 21 + 96 + 1,
					"y" : 507 - 75 + 5,

					"default_image" : "d:/ymir work/ui/pattern/q_mark_01.tga",
					"over_image" : "d:/ymir work/ui/pattern/q_mark_02.tga",
					"down_image" : "d:/ymir work/ui/pattern/q_mark_01.tga",
				},
			),
		},
		{
			"name" : "illustration_page",
			"type" : "window",
			"style" : ("attach", "ltr", ),

			"x" : 21,
			"y" : 75,

			"width" : MAIN_WINDOW_WIDTH,
			"height" : MAIN_WINDOW_HEIGHT,
			"children" :
			(
				## 초상화 window
				{
					"name" : "illustrated_window",
					"type" : "window",
					
					"x" : 0,
					"y" : 0,
					"width" : 92*4 + 29*3,
					"height" : 351,
				},
				
				## 등급00 window
				{
					"name" : "star_window00",
					"type" : "window",
					"x" : 48 - 21,
					"y" : 247 - 75,
					"width" : STAR_WINDOW_WIDTH,
					"height" : 15,
				},
				## 등급01 window
				{
					"name" : "star_window01",
					"type" : "window",
					"x" : 169 - 21,
					"y" : 247 - 75,
					"width" : STAR_WINDOW_WIDTH,
					"height" : 15,
				},
				## 등급02 window
				{
					"name" : "star_window02",
					"type" : "window",
					"x" : 290 - 21,
					"y" : 247 - 75,
					"width" : STAR_WINDOW_WIDTH,
					"height" : 15,
				},
				## 등급03 window
				{
					"name" : "star_window03",
					"type" : "window",
					"x" : 411 - 21,
					"y" : 247 - 75,
					"width" : STAR_WINDOW_WIDTH,
					"height" : 15,
				},
				## 등급10 window
				{
					"name" : "star_window10",
					"type" : "window",
					"x" : 48 - 21,
					"y" : 448 - 75,
					"width" : STAR_WINDOW_WIDTH,
					"height" : 15,
				},
				## 등급11 window
				{
					"name" : "star_window11",
					"type" : "window",
					"x" : 169 - 21,
					"y" : 448 - 75,
					"width" : STAR_WINDOW_WIDTH,
					"height" : 15,
				},
				## 등급12 window
				{
					"name" : "star_window12",
					"type" : "window",
					"x" : 290 - 21,
					"y" : 448 - 75,
					"width" : STAR_WINDOW_WIDTH,
					"height" : 15,
				},
				## 등급13 window
				{
					"name" : "star_window13",
					"type" : "window",
					"x" : 411 - 21,
					"y" : 448 - 75,
					"width" : STAR_WINDOW_WIDTH,
					"height" : 15,
				},
				
				## 출현지역00
				{
					"name" : "illustrated_area00",
					"type" : "image",
					"x" : 39 - 21,
					"y" : 268 - 75,
					"image" : ROOT_PATH + "area_bg.sub",
				},
				## 출현지역01
				{
					"name" : "illustrated_area01",
					"type" : "image",
					"x" : 39 - 21 + 98 + 23,
					"y" : 268 - 75,
					"image" : ROOT_PATH + "area_bg.sub",
				},
				## 출현지역02
				{
					"name" : "illustrated_area02",
					"type" : "image",
					"x" : 39 - 21 + 98*2 + 23*2,
					"y" : 268 - 75,
					"image" : ROOT_PATH + "area_bg.sub",
				},
				## 출현지역03
				{
					"name" : "illustrated_area03",
					"type" : "image",
					"x" : 39 - 21 + 98*3 + 23*3,
					"y" : 268 - 75,
					"image" : ROOT_PATH + "area_bg.sub",
				},
				## 출현지역10
				{
					"name" : "illustrated_area10",
					"type" : "image",
					"x" : 39 - 21,
					"y" : 469 - 75,
					"image" : ROOT_PATH + "area_bg.sub",
				},
				## 출현지역11
				{
					"name" : "illustrated_area11",
					"type" : "image",
					"x" : 39 - 21 + 98 + 23,
					"y" : 469 - 75,
					"image" : ROOT_PATH + "area_bg.sub",
				},
				## 출현지역12
				{
					"name" : "illustrated_area12",
					"type" : "image",
					"x" : 39 - 21 + 98*2 + 23*2,
					"y" : 469 - 75,
					"image" : ROOT_PATH + "area_bg.sub",
				},
				## 출현지역13
				{
					"name" : "illustrated_area13",
					"type" : "image",
					"x" : 39 - 21 + 98*3 + 23*3,
					"y" : 469 - 75,
					"image" : ROOT_PATH + "area_bg.sub",
				},
				
				## 모델 뷰
				{
					"name" : "model_view_window",
					"type" : "window",
					"style" : ("attach", "ltr",),
					
					"x" : 515 - 21,
					"y" : 88 - 75,
					"width" : MODEL_VIEW_BG_WIDTH,
					"height" : MODEL_VIEW_BG_HEIGHT,
					
					"children" :
					(
						## LeftTop 1
						{
							"name" : "MV_LeftTop",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_left_top.tga",
						},
						## RightTop 2
						{
							"name" : "MV_RightTop",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : MODEL_VIEW_BG_WIDTH - 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_right_top.tga",
						},
						## LeftBottom 3
						{
							"name" : "MV_LeftBottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : MODEL_VIEW_BG_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_left_bottom.tga",
						},
						## RightBottom 4
						{
							"name" : "MV_RightBottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : MODEL_VIEW_BG_WIDTH - 16,
							"y" : MODEL_VIEW_BG_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_right_bottom.tga",
						},
						## topcenterImg 5
						{
							"name" : "MV_TopCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_top.tga",
							"rect" : (0.0, 0.0, MODEL_VIEW_X_COUNT, 0),
						},
						## leftcenterImg 6
						{
							"name" : "MV_LeftCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_left.tga",
							"rect" : (0.0, 0.0, 0, MODEL_VIEW_Y_COUNT),
						},
						## rightcenterImg 7
						{
							"name" : "MV_RightCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : MODEL_VIEW_BG_WIDTH - 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_right.tga",
							"rect" : (0.0, 0.0, 0, MODEL_VIEW_Y_COUNT),
						},
						## bottomcenterImg 8
						{
							"name" : "MV_BottomCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : MODEL_VIEW_BG_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_bottom.tga",
							"rect" : (0.0, 0.0, MODEL_VIEW_X_COUNT, 0),
						},
						## centerImg
						{
							"name" : "MV_CenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_center.tga",
							"rect" : (0.0, 0.0, MODEL_VIEW_X_COUNT, MODEL_VIEW_Y_COUNT),
						},
						
						## name
						{
							"name" : "MV_name",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 3,
							"y" : 3,
							"image" : ROOT_PATH + "model_view_name.sub",
						},
						## name text window
						{
							"name" : "MV_name_text_window", "type" : "window", "style" : ("attach",), "x" : 3, "y" : 3, "width" : 240, "height" : 23,
							"children" :
							(
								{ "name" : "MV_name_text", "type" : "text", "x" : 0, "y" : 0, "all_align" : "center", "text" : "Lv7 늑대", },
							),
						},
						## model view bg
						{
							"name" : "model_view",
							"type" : "render_target",

							"x" : 3,
							"y" : 26,
							
							"width" : 240,
							"height" : 306,

							"index" : 0,
						},
						## 상 버튼
						{
							"name" : "mv_up_camera_button",
							"type" : "button",
							"x" : 3 + 80 -1 - 17,
							"y" : 26 + 306 - 18,

							"default_image" : BUTTON_PATH + "up_camera/up_camera_button_default.sub",
							"over_image" 	: BUTTON_PATH + "up_camera/up_camera_button_over.sub",
							"down_image" 	: BUTTON_PATH + "up_camera/up_camera_button_down.sub",
						},
						## 하 버튼
						{
							"name" : "mv_down_camera_button",
							"type" : "button",
							"x" : 3 + 80,
							"y" : 26 + 306 - 18,

							"default_image" : BUTTON_PATH + "down_camera/down_camera_button_default.sub",
							"over_image" 	: BUTTON_PATH + "down_camera/down_camera_button_over.sub",
							"down_image" 	: BUTTON_PATH + "down_camera/down_camera_button_down.sub",
						},
						## 왼쪽 회전 버튼
						{
							"name" : "mv_left_rotation_button",
							"type" : "button",
							"x" : 3 + 80 + 17 + 1,
							"y" : 26 + 306 - 18,

							"default_image" : BUTTON_PATH + "left_rotation/left_rotation_button_default.sub",
							"over_image" 	: BUTTON_PATH + "left_rotation/left_rotation_button_over.sub",
							"down_image" 	: BUTTON_PATH + "left_rotation/left_rotation_button_down.sub",
						},
						## 오른쪽 회전 버튼
						{
							"name" : "mv_right_rotation_button", 
							"type" : "button",
							"x" : 3 + 80 + 17 + 1 + 17 + 1,
							"y" : 26 + 306 - 18,

							"default_image" : BUTTON_PATH + "right_rotation/right_rotation_button_default.sub",
							"over_image" 	: BUTTON_PATH + "right_rotation/right_rotation_button_over.sub",
							"down_image" 	: BUTTON_PATH + "right_rotation/right_rotation_button_down.sub",
						},
						## 초기화 버튼
						{
							"name" : "mv_reset_button",
							"type" : "button",
							"x" : 3 + 80 + 17 + 1 + 17 + 1 + 17 + 1,
							"y" : 26 + 306 - 18,

							"default_image" : BUTTON_PATH + "mv_reset/mv_reset_button_default.sub",
							"over_image" 	: BUTTON_PATH + "mv_reset/mv_reset_button_over.sub",
							"down_image" 	: BUTTON_PATH + "mv_reset/mv_reset_button_down.sub",
						},
						## 확대
						{
							"name" : "mv_zoomin_button",
							"type" : "button",
							"x" : 3 + 80 + 17 + 1 + 17 + 1 + 17 + 1 + 17 + 1,
							"y" : 26 + 306 - 18,

							"default_image" : BUTTON_PATH + "zoomin/zoomin_rotation_button_default.sub",
							"over_image" 	: BUTTON_PATH + "zoomin/zoomin_rotation_button_over.sub",
							"down_image" 	: BUTTON_PATH + "zoomin/zoomin_rotation_button_down.sub",
						},
						## 축소
						{
							"name" : "mv_zoomout_button", 
							"type" : "button",
							"x" : 3 + 80 + 17 + 1 + 17 + 1 + 17 + 1 + 17 + 1 + 17 + 1,
							"y" : 26 + 306 - 18,

							"default_image" : BUTTON_PATH + "zoomout/zoomin_rotation_button_default.sub",
							"over_image" 	: BUTTON_PATH + "zoomout/zoomin_rotation_button_over.sub",
							"down_image" 	: BUTTON_PATH + "zoomout/zoomin_rotation_button_down.sub",
						},
						## count bg
						{
							"name" : "MV_count_img",
							"type" : "expanded_image",
							"x" : 3,
							"y" : 332,
							"image" : ROOT_PATH + "model_view_name.sub",
						},
						
						## count window
						{
							"name" : "MV_count_window",
							"type" : "window",
							"style" : ("ltr",),
							
							"x" : 26,
							"y" : 333,
							"width" : 196,
							"height" : 21,
							"children" :
							(
								{
									"name" : "MV_countLeftImg",
									"type" : "expanded_image",
									"style" : ("ltr",),
									
									"x" : 0,
									"y" : 0,
									"image" : PATTERN_PATH + "border_c_left.tga",
								},
								{
									"name" : "MV_countCenterImg",
									"type" : "expanded_image",
									"style" : ("ltr",),
									
									"x" : 21,
									"y" : 0,
									"image" : PATTERN_PATH + "border_c_middle.tga",
									"rect" : (0.0, 0.0, 7, 0),
								},
								{
									"name" : "MV_countImg",
									"type" : "expanded_image",
									"style" : ("ltr",),
									
									"x" : 196-21,
									"y" : 0,
									"image" : PATTERN_PATH + "border_c_right.tga",
								},
								{ "name" : "MV_countText", "type" : "text", "x" : 0, "y" : 0, "all_align" : "center", "text" : "누적 등록 횟수 : 9999", },
							),
						},
						## 승급 버튼
						{
							"name" : "promotion_button",
							"type" : "button",

							"x" : 540 - 515,
							"y" : 450 - 88,

							"default_image" : BUTTON_PATH + "promotion/promotion_button_default.sub",
							"over_image" : BUTTON_PATH + "promotion/promotion_button_over.sub",
							"down_image" : BUTTON_PATH + "promotion/promotion_button_down.sub",
						},
						## 교환 버튼
						{
							"name" : "exchange_button",
							"type" : "button",

							"x" : 648 - 515,
							"y" : 450 - 88,

							"default_image" : BUTTON_PATH + "trade_card/trade_card_button_default.sub",
							"over_image" : BUTTON_PATH + "trade_card/trade_card_button_over.sub",
							"down_image" : BUTTON_PATH + "trade_card/trade_card_button_down.sub",
						},
						## 모션 버튼
						{
							"name" : "motion_button",
							"type" : "button",

							"x" : 540 - 515,
							"y" : 474 - 88,

							"default_image" : BUTTON_PATH + "motion/motion_button_default.sub",
							"over_image" : BUTTON_PATH + "motion/motion_button_over.sub",
							"down_image" : BUTTON_PATH + "motion/motion_button_down.sub",
						},
						## 변신 버튼
						{
							"name" : "poly_button",
							"type" : "button",

							"x" : 648 - 515,
							"y" : 474 - 88,

							"default_image" : BUTTON_PATH + "poly/poly_button_default.sub",
							"over_image" : BUTTON_PATH + "poly/poly_button_over.sub",
							"down_image" : BUTTON_PATH + "poly/poly_button_down.sub",
						},
						## 이동 버튼
						{
							"name" : "warp_button",
							"type" : "button",

							"x" : 540 - 515,
							"y" : 498 - 88,

							"default_image" : BUTTON_PATH + "warp/warp_button_default.sub",
							"over_image" : BUTTON_PATH + "warp/warp_button_over.sub",
							"down_image" : BUTTON_PATH + "warp/warp_button_down.sub",
						},
						## 소환 버튼
						{
							"name" : "summon_button",
							"type" : "button",

							"x" : 648 - 515,
							"y" : 498 - 88,

							"default_image" : BUTTON_PATH + "summon/summon_button_default.sub",
							"over_image" : BUTTON_PATH + "summon/summon_button_over.sub",
							"down_image" : BUTTON_PATH + "summon/summon_button_down.sub",
						},
					),
				},
				
				##width 461
				## 이전 * 10 버튼 13*10
				{
					"name" : "first_prev_button",
					"type" : "button",

					"x" : 39 -21 + 231 -16 -(32*2) -(3*2) -3 -10 -10 -13,
					"y" : 509 - 75 + 3,

					"default_image" : "d:/ymir work/ui/privatesearch/private_first_prev_btn_01.sub",
					"over_image" 	: "d:/ymir work/ui/privatesearch/private_first_prev_btn_02.sub",
					"down_image" 	: "d:/ymir work/ui/privatesearch/private_first_prev_btn_01.sub",
				},
				## 이전 버튼 10*10
				{
					"name" : "prev_button",
					"type" : "button",
					"x" : 39 -21 + 231 -16 - 32 -32 - 6-3 -10,
					"y" : 434 + 3,

					"default_image" : "d:/ymir work/ui/privatesearch/private_prev_btn_01.sub",
					"over_image" 	: "d:/ymir work/ui/privatesearch/private_prev_btn_02.sub",
					"down_image" 	: "d:/ymir work/ui/privatesearch/private_prev_btn_01.sub",
				},
				{
					"name" : "page_button0",
					"type" : "button",
					"x" : 39 -21 + 231 -16 - 32 -32 - 6,
					"y" : 434,

					"text" : "1",

					"default_image" : "d:/ymir work/ui/privatesearch/private_pagenumber_00.sub",
					"over_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_01.sub",
					"down_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_02.sub",
				},
				{
					"name" : "page_button1",
					"type" : "button",
					"x" : 39 -21 + 231 -16 - 32 - 3,
					"y" : 434,

					"text" : "2",

					"default_image" : "d:/ymir work/ui/privatesearch/private_pagenumber_00.sub",
					"over_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_01.sub",
					"down_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_02.sub",
				},
				{
					"name" : "page_button2",
					"type" : "button",
					"x" : 39 -21 + 231 -16,
					"y" : 434,
					
					"text" : "3",

					"default_image" : "d:/ymir work/ui/privatesearch/private_pagenumber_00.sub",
					"over_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_01.sub",
					"down_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_02.sub",
				},
				{
					"name" : "page_button3",
					"type" : "button",
					"x" : 39 -21 + 231 + 16 + 3,
					"y" : 434,

					"text" : "4",

					"default_image" : "d:/ymir work/ui/privatesearch/private_pagenumber_00.sub",
					"over_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_01.sub",
					"down_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_02.sub",
				},
				{
					"name" : "page_button4",
					"type" : "button",
					"x" : 39 -21 + 231 + 16 + 3 +32 +3,
					"y" : 434,

					"text" : "5",

					"default_image" : "d:/ymir work/ui/privatesearch/private_pagenumber_00.sub",
					"over_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_01.sub",
					"down_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_02.sub",
				},
				## 다음 버튼 10*10
				{
					"name" : "next_button", 
					"type" : "button",
					"x" : 39 -21 + 231 + 16 + 3 +32 +3 +32 +3,
					"y" : 434 + 3,

					"default_image" : "d:/ymir work/ui/privatesearch/private_next_btn_01.sub",
					"over_image" 	: "d:/ymir work/ui/privatesearch/private_next_btn_02.sub",
					"down_image" 	: "d:/ymir work/ui/privatesearch/private_next_btn_01.sub",
				},
				## 다음 * 10 버튼 13*10
				{
					"name" : "last_next_button", 
					"type" : "button",
					"x" : 39 -21 + 231 + 16 +(32 * 2) + (3*2) +3 + 10 +10,
					"y" : 509 - 75 + 3,

					"default_image" : "d:/ymir work/ui/privatesearch/private_last_next_btn_01.sub",
					"over_image" 	: "d:/ymir work/ui/privatesearch/private_last_next_btn_02.sub",
					"down_image" 	: "d:/ymir work/ui/privatesearch/private_last_next_btn_01.sub",
				},
			),
		},
	),
}