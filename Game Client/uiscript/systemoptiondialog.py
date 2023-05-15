import uiScriptLocale
import app

ROOT_PATH = "d:/ymir work/ui/public/"

TEMPORARY_X = +13
TEXT_TEMPORARY_X = -10
BUTTON_TEMPORARY_X = 5
PVP_X = -10

window = {
	"name" : "SystemOptionDialog",
	"style" : ("movable", "float",),

	"x" : 0,
	"y" : 0,

	"width" : 305,
	"height" : 255,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : 305,
			"height" : 255,

			"children" :
			(
				## Title
				{
					"name" : "titlebar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 8,
					"y" : 8,

					"width" : 284,
					"color" : "gray",

					"children" :
					(
						{ 
						"name":"titlename", "type":"text", "x":0, "y":3, 
						"horizontal_align":"center", "text_horizontal_align":"center",
						"text": uiScriptLocale.SYSTEMOPTION_TITLE, 
						 },
					),
				},

				
				## Music
				{
					"name" : "music_name",
					"type" : "text",

					"x" : 30,
					"y" : 75,

					"text" : uiScriptLocale.OPTION_MUSIC,
				},
				
				{
					"name" : "music_volume_controller",
					"type" : "sliderbar",

					"x" : 110,
					"y" : 75,
				},
				
				{
					"name" : "bgm_button",
					"type" : "button",

					"x" : 20,
					"y" : 100,

					"text" : uiScriptLocale.OPTION_MUSIC_CHANGE,

					"default_image" : ROOT_PATH + "Middle_Button_01.sub",
					"over_image" : ROOT_PATH + "Middle_Button_02.sub",
					"down_image" : ROOT_PATH + "Middle_Button_03.sub",
				},
				
				{
					"name" : "bgm_file",
					"type" : "text",

					"x" : 100,
					"y" : 102,

					"text" : uiScriptLocale.OPTION_MUSIC_DEFAULT_THEMA,
				},
				
				## Sound
				{
					"name" : "sound_name",
					"type" : "text",

					"x" : 30,
					"y" : 50,

					"text" : uiScriptLocale.OPTION_SOUND,
				},
				
				{
					"name" : "sound_volume_controller",
					"type" : "sliderbar",

					"x" : 110,
					"y" : 50,
				},	

				## 카메라
				{
					"name" : "camera_mode",
					"type" : "text",

					"x" : 40 + TEXT_TEMPORARY_X,
					"y" : 135+2,

					"text" : uiScriptLocale.OPTION_CAMERA_DISTANCE,
				},
				
				{
					"name" : "camera_short",
					"type" : "radio_button",

					"x" : 110,
					"y" : 135,

					"text" : uiScriptLocale.OPTION_CAMERA_DISTANCE_SHORT,

					"default_image" : ROOT_PATH + "Middle_Button_01.sub",
					"over_image" : ROOT_PATH + "Middle_Button_02.sub",
					"down_image" : ROOT_PATH + "Middle_Button_03.sub",
				},
				
				{
					"name" : "camera_long",
					"type" : "radio_button",

					"x" : 110+70,
					"y" : 135,

					"text" : uiScriptLocale.OPTION_CAMERA_DISTANCE_LONG,

					"default_image" : ROOT_PATH + "Middle_Button_01.sub",
					"over_image" : ROOT_PATH + "Middle_Button_02.sub",
					"down_image" : ROOT_PATH + "Middle_Button_03.sub",
				},

				{
					"name" : "fog_mode",
					"type" : "text",

					"x" : 30,
					"y" : 160+2,

					"text" : uiScriptLocale.OPTION_FOG,
				},
				
				{
					"name" : "fog_on",
					"type" : "radio_button",

					"x" : 110,
					"y" : 160,

					"text" : uiScriptLocale.OPTION_FOG_ON,

					"default_image" : ROOT_PATH + "small_Button_01.sub",
					"over_image" : ROOT_PATH + "small_Button_02.sub",
					"down_image" : ROOT_PATH + "small_Button_03.sub",
				},
				
				{
					"name" : "fog_off",
					"type" : "radio_button",

					"x" : 110+50,
					"y" : 160,

					"text" : uiScriptLocale.OPTION_FOG_OFF,
					
					"default_image" : ROOT_PATH + "small_Button_01.sub",
					"over_image" : ROOT_PATH + "small_Button_02.sub",
					"down_image" : ROOT_PATH + "small_Button_03.sub",
				},

				## 타일 가속
				{
					"name" : "tiling_mode",
					"type" : "text",

					"x" : 40 + TEXT_TEMPORARY_X,
					"y" : 185+2,

					"text" : uiScriptLocale.OPTION_TILING,
				},
				
				{
					"name" : "tiling_cpu",
					"type" : "radio_button",

					"x" : 110,
					"y" : 185,

					"text" : uiScriptLocale.OPTION_TILING_CPU,

					"default_image" : ROOT_PATH + "small_Button_01.sub",
					"over_image" : ROOT_PATH + "small_Button_02.sub",
					"down_image" : ROOT_PATH + "small_Button_03.sub",
				},
				
				{
					"name" : "tiling_gpu",
					"type" : "radio_button",

					"x" : 110+50,
					"y" : 185,

					"text" : uiScriptLocale.OPTION_TILING_GPU,

					"default_image" : ROOT_PATH + "small_Button_01.sub",
					"over_image" : ROOT_PATH + "small_Button_02.sub",
					"down_image" : ROOT_PATH + "small_Button_03.sub",
				},
				
				{
					"name" : "tiling_apply",
					"type" : "button",

					"x" : 110+100,
					"y" : 185,

					"text" : uiScriptLocale.OPTION_TILING_APPLY,

					"default_image" : ROOT_PATH + "middle_Button_01.sub",
					"over_image" : ROOT_PATH + "middle_Button_02.sub",
					"down_image" : ROOT_PATH + "middle_Button_03.sub",
				},
				{
					"name" : "FOV_TYPE",
					"type" : "text",

					"x" : 40 + TEXT_TEMPORARY_X,
					"y" : 210+2,

					"text" : uiScriptLocale.GRAPHICONOFF_FOV_LEVEL, 
				},
				
				{
					"name" : "FOV_TYPE1",
					"type" : "radio_button",

					"x" : 112,
					"y" : 210,

					"text" :  uiScriptLocale.GRAPHICONOFF_FOV_LEVEL1,

					"default_image" : ROOT_PATH + "minimize_empty_button_01.sub",
					"over_image" : ROOT_PATH + "minimize_empty_button_02.sub",
					"down_image" : ROOT_PATH + "minimize_empty_button_03.sub",
				},
				
				{
					"name" : "FOV_TYPE2",
					"type" : "radio_button",

					"x" : 112 + 20,
					"y" : 210,

					"text" :  uiScriptLocale.GRAPHICONOFF_FOV_LEVEL2,

					"default_image" : ROOT_PATH + "minimize_empty_button_01.sub",
					"over_image" : ROOT_PATH + "minimize_empty_button_02.sub",
					"down_image" : ROOT_PATH + "minimize_empty_button_03.sub",
				},
				
				{
					"name" : "FOV_TYPE3",
					"type" : "radio_button",

					"x" : 112 + 40,
					"y" : 210,

					"text" :  uiScriptLocale.GRAPHICONOFF_FOV_LEVEL3,

					"default_image" : ROOT_PATH + "minimize_empty_button_01.sub",
					"over_image" : ROOT_PATH + "minimize_empty_button_02.sub",
					"down_image" : ROOT_PATH + "minimize_empty_button_03.sub",
				},
				
				{
					"name" : "FOV_TYPE4",
					"type" : "radio_button",

					"x" : 112 + 60,
					"y" : 210,

					"text" :  uiScriptLocale.GRAPHICONOFF_FOV_LEVEL4,

					"default_image" : ROOT_PATH + "minimize_empty_button_01.sub",
					"over_image" : ROOT_PATH + "minimize_empty_button_02.sub",
					"down_image" : ROOT_PATH + "minimize_empty_button_03.sub",
				},
				
				{
					"name" : "FOV_TYPE5",
					"type" : "radio_button",

					"x" : 112 + 80,
					"y" : 210,

					"text" :  uiScriptLocale.GRAPHICONOFF_FOV_LEVEL5,

					"default_image" : ROOT_PATH + "minimize_empty_button_01.sub",
					"over_image" : ROOT_PATH + "minimize_empty_button_02.sub",
					"down_image" : ROOT_PATH + "minimize_empty_button_03.sub",
				},
				
				{
					"name" : "fovlevel_apply",
					"type" : "button",

					"x" : 110+105,
					"y" : 210,

					"text" : uiScriptLocale.GRAPHICONOFF_FOV_APPLY,

					"default_image" : ROOT_PATH + "middle_Button_01.sub",
					"over_image" : ROOT_PATH + "middle_Button_02.sub",
					"down_image" : ROOT_PATH + "middle_Button_03.sub",
				},
			),
		},
	),
}
