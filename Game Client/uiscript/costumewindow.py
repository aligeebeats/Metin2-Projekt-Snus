import uiScriptLocale
import item

window = {
	"name" : "CostumeWindow",

	"x" : SCREEN_WIDTH - 175 - 140 - 4,
	"y" : SCREEN_HEIGHT - 37 - 565 - 18,

	"style" : ("movable", "float",),

	"width" : 140+4,
	"height" : 228+27,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : 140+4,
			"height" : 228+27,
			
			"children" :
			(
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 8,
					"y" : 7,

					"width" : 130,
					"color" : "yellow",

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":60, "y":3, "text":uiScriptLocale.COSTUME_WINDOW_TITLE, "text_horizontal_align":"center" },
					),
				},

				## Equipment Slot
				{
					"name" : "Costume_Base",
					"type" : "image",

					"x" : 13 + 2,
					"y" : 38 - 1,
					
					"image" : uiScriptLocale.LOCALE_UISCRIPT_PATH + "costume/costume_bg.jpg",

					"children" :
					(

						{
							"name" : "CostumeSlot",
							"type" : "slot",

							"x" : 3,
							"y" : 3,

							"width" : 127,
							"height" : 145 + 40,

							"slot" : (
										{"index":item.COSTUME_BODY, "x":61, "y":45+27, "width":32, "height":64},
										{"index":item.COSTUME_HAIR, "x":61, "y": 8+27, "width":32, "height":32},
										{"index":item.COSTUME_MOUNT, "x":61, "y":125+27, "width":32, "height":32},
										{"index":item.COSTUME_PET, "x":12, "y":125+27, "width":32, "height":32},
										{"index":item.COSTUME_WEAPON, "x":12, "y":13+27, "width":32, "height":96},
										{"index":item.COSTUME_ACCE, "x":62, "y":1, "width":32, "height":32},
										{"index":item.EQUIPMENT_PENDANT, "x":12, "y":1, "width":32, "height":32},
									),
						},
					),
				},

			),
		},
	),
}
