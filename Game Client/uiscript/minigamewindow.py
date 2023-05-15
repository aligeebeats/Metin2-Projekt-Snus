import uiScriptLocale
import app

RUMI_ROOT = "d:/ymir work/ui/minigame/rumi/"

if app.ENABLE_NOTICE_INGAME_EVENT:
	FIXED_VALUE_X = 2	#화살표 가 생김으로 생긴 보정갑
	FIXED_VALUE_Y = 15
else:
	FIXED_VALUE_X = 0
	FIXED_VALUE_Y = 0

window = {
	"name" : "MiniGameWindow",
	
	"x" : SCREEN_WIDTH - 246 - FIXED_VALUE_X,
	"y" : 15 - FIXED_VALUE_Y,
	
	"width" : 94 + FIXED_VALUE_X,
	"height" : 58 + FIXED_VALUE_Y,
	
	"children" :
	(
		{
			"name" : "mini_game_window",
			"type" : "window",
			
			"x" : FIXED_VALUE_X,
			"y" : 0,
			
			"width" : 94 + FIXED_VALUE_X,
			"height" : 58 + FIXED_VALUE_Y,
			
			"children" :
			(
				{
					"name" : "minigame_rumi_button",
					"type" : "button",
					
					"x" : 0,
					"y" : 0,
					
					"default_image" : RUMI_ROOT + "rumi_button_min.sub",
					"over_image" : RUMI_ROOT + "rumi_button_min.sub",
					"down_image" : RUMI_ROOT + "rumi_button_min.sub",
				},
				
				{
					"name" : "minigame_rumi_button_effect",
					"type" : "ani_image",
					"style" : ("attach",),
					
					"x" : 0,
					"y" : 0,
					
					"delay" : 8,

					"images" :
					(
						RUMI_ROOT + "rumi_button_effect/rumi_button_effect2.sub",
						RUMI_ROOT + "rumi_button_effect/rumi_button_effect3.sub",
						RUMI_ROOT + "rumi_button_effect/rumi_button_effect4.sub",
						RUMI_ROOT + "rumi_button_effect/rumi_button_effect5.sub",
						RUMI_ROOT + "rumi_button_effect/rumi_button_effect4.sub",
						RUMI_ROOT + "rumi_button_effect/rumi_button_effect3.sub",
						RUMI_ROOT + "rumi_button_effect/rumi_button_effect2.sub",
						RUMI_ROOT + "rumi_button_effect/rumi_button_effect1.sub",
						RUMI_ROOT + "rumi_button_effect/rumi_button_effect1.sub",
						RUMI_ROOT + "rumi_button_effect/rumi_button_effect1.sub",
					),
				},
			),
		},		
	),	
}
