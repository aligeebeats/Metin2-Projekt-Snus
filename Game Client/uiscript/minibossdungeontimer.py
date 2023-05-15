window = {
	"name" : "MiniBossDungeonTimer",
	"x" : SCREEN_WIDTH - 136, "y" : 35,
	"width" : 78 * 1.2, "height" : 25 * 1.2,
	"children" :
	[
		{
			"name" : "TimerBG", "type" : "expanded_image", "style" : ("ltr",),
			"x" : 0, "y" : 0,
			"x_scale" : 1.2, "y_scale" : 1.2,
			"image" : "d:/ymir work/ui/minigame/miniboss/time_bg_red.sub",
			"children" :
			[
				{
					"name" : "LeftTime", "type" : "text",
					"x" : 13, "y" : 0,
					"text" : "00:00", "all_align" : "center",
					"r" : 1.0, "g" : 194.0 / 255.0, "b" : 91.0 / 255.0, "a" : 1.0,
					"fontsize" : "LARGE",
				},
			],
		},
	],
}
