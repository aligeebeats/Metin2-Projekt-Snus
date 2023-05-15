import uiScriptlocale

window = {
	"name" : "ExpandTaskBar",

	"x" : SCREEN_WIDTH/2 - 5,
	"y" : SCREEN_HEIGHT - 74,

	"width" : 37*2,
	"height" : 37,

	"children" :
	(
		{
			"name" : "ExpanedTaskBar_Board",
			"type" : "window",
			
			"x" : 0,
			"y" : 0,
			
			"width" : 66,
			"height" : 32,
			
			"children" :
			(
				{
					"name" : "DragonSoulButton",
					"type" : "button",

					"x" : 0,
					"y" : 0,

					"tooltip_text" : uiScriptlocale.TASKBAR_DISABLE,
							
					"default_image" : "d:/ymir work/ui/dragonsoul/DragonSoul_Button_01.tga",
					"over_image" : "d:/ymir work/ui/dragonsoul/DragonSoul_Button_02.tga",
					"down_image" : "d:/ymir work/ui/dragonsoul/DragonSoul_Button_03.tga",
				},
			),
		},
	),
}