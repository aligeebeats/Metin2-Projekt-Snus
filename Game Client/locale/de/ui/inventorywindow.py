import uiScriptlocale
import item
 
EQUIPMENT_START_INDEX = 180
 
window = {
	"name" : "InventoryWindow",
 
	## 600 - (width + ????? ?? ??? 24 px)
	"x" : SCREEN_WIDTH - 176,
	"y" : SCREEN_HEIGHT - 55 - 565,
 
	"style" : ("movable", "float",),
 
	"width" : 176,
	"height" : 585,
 
	"children" :
	(
		## Inventory, Equipment Slots
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),
 
			"x" : 0,
			"y" : 0,
 
			"width" : 176,
			"height" : 585,
 
			"children" :
			(
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),
 
					"x" : 8,
					"y" : 7,
 
					"width" : 161,
					"color" : "yellow",
 
					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":77, "y":3, "text":uiScriptlocale.INVENTORY_TITLE, "text_horizontal_align":"center" },
					),
				},
				## Equipment Slot
				{
					"name" : "Equipment_Base_Tab_0",
					"type" : "image",
 
					"x" : 0,
					"y" : 33,
 
					"image" : "d:/ymir work/ui/equipment_bg_with_ring.tga",
 
					"children" :
					(
 
						{
							"name" : "EquipmentSlot",
							"type" : "slot",
 
							"x" : 3 + 10,
							"y" : 3 ,
 
							"width" : 150,
							"height" : 182,
 
							"slot" : (
										{"index":EQUIPMENT_START_INDEX+0, "x":38, "y":35, "width":32, "height":64},
										{"index":EQUIPMENT_START_INDEX+1, "x":38, "y":2, "width":32, "height":32},
										{"index":EQUIPMENT_START_INDEX+2, "x":38, "y":145, "width":32, "height":32},
										{"index":EQUIPMENT_START_INDEX+3, "x":75, "y":67, "width":32, "height":32},
										{"index":EQUIPMENT_START_INDEX+4, "x":3, "y":3, "width":32, "height":96},
										{"index":EQUIPMENT_START_INDEX+5, "x":114, "y":67, "width":32, "height":32},
										{"index":EQUIPMENT_START_INDEX+6, "x":114, "y":35, "width":32, "height":32},
										{"index":EQUIPMENT_START_INDEX+7, "x":2, "y":145, "width":32, "height":32},
										{"index":EQUIPMENT_START_INDEX+8, "x":75, "y":145, "width":32, "height":32},
										{"index":EQUIPMENT_START_INDEX+9, "x":114, "y":2, "width":32, "height":32},
										{"index":EQUIPMENT_START_INDEX+10, "x":75, "y":35, "width":32, "height":32},
										{"index":item.EQUIPMENT_RING1, "x":1, "y":104, "width":32, "height":32},
										{"index":item.EQUIPMENT_RING2, "x":73, "y":104, "width":32, "height":32},
										{"index":item.EQUIPMENT_BELT, "x":38, "y":104, "width":32, "height":32},
									),
						},
						## Dragon Soul Button
						{
							"name" : "DSSButton",
							"type" : "button",
 
							"x" : 114+10,
							"y" : 107,
 
							"tooltip_text" : uiScriptlocale.TASKBAR_DRAGON_SOUL,
 
							"default_image" : "d:/ymir work/ui/dragonsoul/dss_inventory_button_01.tga",
							"over_image" : "d:/ymir work/ui/dragonsoul/dss_inventory_button_02.tga",
							"down_image" : "d:/ymir work/ui/dragonsoul/dss_inventory_button_03.tga",
						},
						## MallButton
						{
							"name" : "MallButton",
							"type" : "button",
 
							"x" : 118+10,
							"y" : 148,
 
							"tooltip_text" : uiScriptlocale.MALL_TITLE,
 
							"default_image" : "d:/ymir work/ui/game/TaskBar/Mall_Button_01.tga",
							"over_image" : "d:/ymir work/ui/game/TaskBar/Mall_Button_02.tga",
							"down_image" : "d:/ymir work/ui/game/TaskBar/Mall_Button_03.tga",
						},
					),
				},
  
				{
					"name" : "Equipment_Base_Tab_1",
					"type" : "image",
 
					"x" : 0,
					"y" : 33 ,
 
					"image" : "d:/ymir work/ui/special_equipment/equipment_bg_2.tga",
					"children" :
					(

						{
							"name" : "Costume_Slot",
							"type" : "slot",

							"x" : 3 +10,
							"y" : 3,

							"width" : 150,
							"height" : 145 + 40 ,

							"slot" : (
										{"index":item.COSTUME_BODY, "x":61, "y":76, "width":32, "height":64},
										{"index":item.COSTUME_HAIR, "x":61, "y": 38, "width":32, "height":32},
										{"index":item.COSTUME_MOUNT, "x":109, "y":62, "width":32, "height":32},
										{"index":item.COSTUME_PET, "x":109, "y":98, "width":32, "height":32},
										{"index":item.COSTUME_WEAPON, "x":10, "y":13+27+23, "width":32, "height":96},
										{"index":item.COSTUME_ACCE, "x":109, "y":62 - 36, "width":32, "height":32},
										{"index":item.EQUIPMENT_PENDANT, "x":109, "y":98 + 36, "width":32, "height":32},
									),
						},
					),
				},

				{
					"name" : "Equipment_Tab_01",
					"type" : "radio_button",
 
					"x" : 0,
					"y" : 70,
 
					"default_image" : "d:/ymir work/ui/game/windows/tab_button_small_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_small_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_small_03.sub",
 
					"children" :
					(
						{
							"name" : "Equipment_Tab_01_Print",
							"type" : "text",
 
							"x" : 0,
							"y" : 0,
 
							"all_align" : "center",
 
							"text" : "I",
						},
					),
				},
				{
					"name" : "Equipment_Tab_02",
					"type" : "radio_button",
 
					"x" : 0,
					"y" : 90,
 
					"default_image" : "d:/ymir work/ui/game/windows/tab_button_small_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_small_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_small_03.sub",
 
					"children" :
					(
						{
							"name" : "Equipment_Tab_02_Print",
							"type" : "text",
 
							"x" : 0,
							"y" : 0,
 
							"all_align" : "center",
 
							"text" : "II",
						},
					),
				},
				{
					"name" : "Inventory_Tab_01",
					"type" : "radio_button",
 
					"x" : 10,
					"y" : 33 + 189,
 
					"default_image" : "d:/ymir work/ui/game/windows/tab_button_large_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_large_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_large_03.sub",
					"tooltip_text" : uiScriptlocale.INVENTORY_PAGE_BUTTON_TOOLTIP_1,
 
					"children" :
					(
						{
							"name" : "Inventory_Tab_01_Print",
							"type" : "text",
 
							"x" : 0,
							"y" : 0,
 
							"all_align" : "center",
 
							"text" : "I",
						},
					),
				},
				{
					"name" : "Inventory_Tab_02",
					"type" : "radio_button",
 
					"x" : 10 + 78,
					"y" : 33 + 189,
 
					"default_image" : "d:/ymir work/ui/game/windows/tab_button_large_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_large_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_large_03.sub",
					"tooltip_text" : uiScriptlocale.INVENTORY_PAGE_BUTTON_TOOLTIP_2,
 
					"children" :
					(
						{
							"name" : "Inventory_Tab_02_Print",
							"type" : "text",
 
							"x" : 0,
							"y" : 0,
 
							"all_align" : "center",
 
							"text" : "II",
						},
					),
				},
				{
					"name" : "Inventory_Tab_03",
					"type" : "radio_button",
 
					"x" : 10,
					"y" : 33 + 210,
 
					"default_image" : "d:/ymir work/ui/game/windows/tab_button_large_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_large_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_large_03.sub",
					"tooltip_text" : uiScriptlocale.INVENTORY_PAGE_BUTTON_TOOLTIP_3,
 
					"children" :
					(
						{
							"name" : "Inventory_Tab_03_Print",
							"type" : "text",
 
							"x" : 0,
							"y" : 0,
 
							"all_align" : "center",
 
							"text" : "III",
						},
					),
				},
				{
					"name" : "Inventory_Tab_04",
					"type" : "radio_button",
 
					"x" : 10 + 78,
					"y" : 33 + 210,
 
					"default_image" : "d:/ymir work/ui/game/windows/tab_button_large_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_large_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_large_03.sub",
					"tooltip_text" : uiScriptlocale.INVENTORY_PAGE_BUTTON_TOOLTIP_4,
 
					"children" :
					(
						{
							"name" : "Inventory_Tab_04_Print",
							"type" : "text",
 
							"x" : 0,
							"y" : 0,
 
							"all_align" : "center",
 
							"text" : "IV",
						},
					),
				},             
 
				## Item Slot
				{
					"name" : "ItemSlot",
					"type" : "grid_table",
 
					"x" : 8,
					"y" : 264,
 
					"start_index" : 0,
					"x_count" : 5,
					"y_count" : 9,
					"x_step" : 32,
					"y_step" : 32,
 
					"image" : "d:/ymir work/ui/public/Slot_Base.sub"
				},
 
				## Print
				{
					"name":"Money_Slot",
					"type":"button",
 
					"x":8,
					"y":28,
 
					"horizontal_align":"center",
					"vertical_align":"bottom",
 
					"default_image" : "d:/ymir work/ui/public/parameter_slot_05.sub",
					"over_image" : "d:/ymir work/ui/public/parameter_slot_05.sub",
					"down_image" : "d:/ymir work/ui/public/parameter_slot_05.sub",
 
					"children" :
					(
						{
							"name":"Money_Icon",
							"type":"image",
 
							"x":-18,
							"y":2,
 
							"image":"d:/ymir work/ui/game/windows/money_icon.sub",
						},
 
						{
							"name" : "Money",
							"type" : "text",
 
							"x" : 3,
							"y" : 3,
 
							"horizontal_align" : "right",
							"text_horizontal_align" : "right",
 
							"text" : "123456789",
						},
					),
				},
 
			),
		},
	),
}