import uiScriptLocale

LOCALE_PATH = uiScriptLocale.WINDOWS_PATH
ROOT_PATH = "d:/ymir work/ui/game/fishing/"

# 278 : ����̹��� �ʺ�, 14 : ����
WINDOW_WIDTH = 278 + 14
# 226 : ����̹��� ����, 37 : ���Ʒ� ����(Ÿ��Ʋ) + 30 (Ÿ�̸� ����28 + ���� 1��)
WINDOW_HEIGHT = 226 + 37

window = {
	"name" : "FishingGameWindow",
	"style" : ("movable", "float", "limit", ),
		
	"x" : 0,
	"y" : 0,
	
	"width" : WINDOW_WIDTH,
	"height" : WINDOW_HEIGHT,
	
	"children" :
	[
		{
			"name" : "board",
			"type" : "board_with_titlebar",
			"style" : ("ltr", ),
			
			"x" : 0,
			"y" : 0,
			
			"width" : WINDOW_WIDTH,
			"height" : WINDOW_HEIGHT,
			"title" : "Fishing",
		
		
			"children" :
			(
				## BackGround Water
				{
					"name" : "fishing_background_water",
					"type" : "image",
					"style" : ("ltr", ),
					"x" : 7,
					"y" : 29,
					"image" : ROOT_PATH + "fishing_background_water.sub",
					
					"children" :
					(
						## BackGround Navigation Area
						{
							"name" : "fishing_water_navArea",
							"type" : "box",
							"style" : ("not_pick", "ltr", ),
							"x" : 15,
							"y" : 15,
							"width" : 278 - 30,
							"height" : 226 - 30,
							"color" : 0xFF35302D,
						},
						
						## Goal Circle
						{
							"name" : "fishing_goal_circle",
							"type" : "image",
							"style" : ("not_pick", "ltr", ),
							"x" : 68,
							"y" : 42,
							"image" : ROOT_PATH + "fishing_goal_circle.sub",
						},
					),
				},				
				
				
				## Goal Success Count
				{
					"name" : "fishing_goal_count",
					"type" : "expanded_image",
					"style" : ("ltr", ),
					"x" : 7 + 278 - 73,
					"y" : 29 + 5,
					"image" : ROOT_PATH + "fishing_goal_count.sub",
					
					"children" :
					(
						{ 
							"name" : "fishing_goal_count_text",
							"type" : "text",
							"style" : ("ltr", ),
							"x" : 45,
							"y" : 8, 
							"text" : "2/5",
							"text_horizontal_align" : "center" 	
						},
					),
				},
				
				# Debug Text
				{ 
					"name" : "debug_text_fish_pos",
					"type" : "text", 
					"x" : 7,
					"y" : 29 + 5 + 8, 
					"text" : "",
					"text_horizontal_align" : "center" 	
				},
				
				{ 
					"name" : "debug_text_circle_pos",
					"type" : "text", 
					"x" : 7,
					"y" : 29 + 5 + 8 + 10, 
					"text" : "",
					"text_horizontal_align" : "center" 	
				},
				
				{ 
					"name" : "debug_text_mouse_pos",
					"type" : "text", 
					"x" : 7,
					"y" : 29 + 5 + 8 + 20, 
					"text" : "",
					"text_horizontal_align" : "center" 	
				},
				
				## Timer Base
				{
					"name" : "fishing_timer_baseImg",
					"type" : "image",
					"x" : 7 + 28,
					"y" : 223,
					"image" : ROOT_PATH + "fishing_timer_base.sub",				
				},
				
				## Timer Gauge type expand_image ?
				{
					"name" : "fishing_timer_gauge",
					"type" : "expanded_image",
					"x" : 7 + 28,
					"y" : 223,
					"image" : ROOT_PATH + "fishing_timer_gauge.sub",
				},
								
			),
		},	
	],
}