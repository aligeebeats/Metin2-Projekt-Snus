import ui
import player
import mouseModule
import net
import app
import snd
import item
import player
import chat
import grp
import uiScriptLocale
import uiRefine
import uiAttachMetin
import uiPickMoney
import uiCommon
import uiPrivateShopBuilder # 개인상점 열동안 ItemMove 방지
import localeInfo
import constInfo
import ime
import wndMgr
import dbg
if app.ITEM_CHECKINOUT_UPDATE:
	import exchange

ITEM_MALL_BUTTON_ENABLE = TRUE
ITEM_FLAG_APPLICABLE = 1 << 14

		
class BeltInventoryWindow(ui.ScriptWindow):

	def __init__(self, wndInventory):
		import exception

		if not wndInventory:
			exception.Abort("wndInventory parameter must be set to InventoryWindow")
			return						
			 	 
		ui.ScriptWindow.__init__(self)

		self.isLoaded = 0
		self.wndInventory = wndInventory;
		
		self.wndBeltInventoryLayer = None
		self.wndBeltInventorySlot = None
		self.expandBtn = None
		self.minBtn = None

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self, openBeltSlot = FALSE):
		self.__LoadWindow()
		self.RefreshSlot()

		ui.ScriptWindow.Show(self)
		
		if openBeltSlot:
			self.OpenInventory()
		else:
			self.CloseInventory()

	def Close(self):
		self.Hide()

	def IsOpeningInventory(self):
		return self.wndBeltInventoryLayer.IsShow()
		
	def OpenInventory(self):
		self.wndBeltInventoryLayer.Show()
		self.expandBtn.Hide()
		self.AdjustPositionAndSize()

	def CloseInventory(self):
		self.wndBeltInventoryLayer.Hide()
		self.expandBtn.Show()
		self.AdjustPositionAndSize()

	def GetBasePosition(self):
		x, y = self.wndInventory.GetGlobalPosition()
		if app.ENABLE_SPECIAL_INVENTORY:
			return x - 148, y + 380
		else:
			return x - 148, y + 241
		
	def AdjustPositionAndSize(self):
		bx, by = self.GetBasePosition()

		if self.IsOpeningInventory():
			self.SetPosition(bx, by)
			self.SetSize(self.ORIGINAL_WIDTH, self.GetHeight())
		else:
			self.SetPosition(bx + 138, by);
			self.SetSize(10, self.GetHeight())

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/BeltInventoryWindow.py")
		except:
			import exception
			exception.Abort("CostumeWindow.LoadWindow.LoadObject")

		try:
			self.ORIGINAL_WIDTH = self.GetWidth()
			wndBeltInventorySlot = self.GetChild("BeltInventorySlot")
			self.wndBeltInventoryLayer = self.GetChild("BeltInventoryLayer")
			self.expandBtn = self.GetChild("ExpandBtn")
			self.minBtn = self.GetChild("MinimizeBtn")
			
			self.expandBtn.SetEvent(ui.__mem_func__(self.OpenInventory))
			self.minBtn.SetEvent(ui.__mem_func__(self.CloseInventory))

			for i in xrange(item.BELT_INVENTORY_SLOT_COUNT):
				slotNumber = item.BELT_INVENTORY_SLOT_START + i							
				wndBeltInventorySlot.SetCoverButton(slotNumber,	"d:/ymir work/ui/game/quest/slot_button_01.sub",\
												"d:/ymir work/ui/game/quest/slot_button_01.sub",\
												"d:/ymir work/ui/game/quest/slot_button_01.sub",\
												"d:/ymir work/ui/game/belt_inventory/slot_disabled.tga", FALSE, FALSE)									
			
		except:
			import exception
			exception.Abort("CostumeWindow.LoadWindow.BindObject")

		## Equipment
		wndBeltInventorySlot.SetOverInItemEvent(ui.__mem_func__(self.wndInventory.OverInItem))
		wndBeltInventorySlot.SetOverOutItemEvent(ui.__mem_func__(self.wndInventory.OverOutItem))
		wndBeltInventorySlot.SetUnselectItemSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
		wndBeltInventorySlot.SetUseSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))						
		wndBeltInventorySlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.wndInventory.SelectEmptySlot))
		wndBeltInventorySlot.SetSelectItemSlotEvent(ui.__mem_func__(self.wndInventory.SelectItemSlot))

		self.wndBeltInventorySlot = wndBeltInventorySlot

	def RefreshSlot(self):
		getItemVNum=player.GetItemIndex
		
		for i in xrange(item.BELT_INVENTORY_SLOT_COUNT):
			slotNumber = item.BELT_INVENTORY_SLOT_START + i
			self.wndBeltInventorySlot.SetItemSlot(slotNumber, getItemVNum(slotNumber), player.GetItemCount(slotNumber))
			self.wndBeltInventorySlot.SetAlwaysRenderCoverButton(slotNumber, TRUE)
			
			avail = "0"
			
			if player.IsAvailableBeltInventoryCell(slotNumber):
				self.wndBeltInventorySlot.EnableCoverButton(slotNumber)				
			else:
				self.wndBeltInventorySlot.DisableCoverButton(slotNumber)				

		self.wndBeltInventorySlot.RefreshSlot()

if app.ENABLE_SPECIAL_INVENTORY:
	class InventoryTypeTabWindow(ui.ScriptWindow):

		IMG_PATH = "d:/ymir work/ui/special_inventory/"
		IMG_WIDTH = 23
		IMG_HEIGHT = 23
		Y_SPACE = 3

		TAB_INVENTORY = 0
		TAB_SKILLBOOK = 1
		TAB_STONE = 2
		TAB_MATERIAL = 3

		def __init__(self, wndInventory):
			if not wndInventory:
				exception.Abort("wndInventory parameter must be set to InventoryWindow")
				return	

			ui.ScriptWindow.__init__(self)
			self.wndInventory = wndInventory

			self.buttonTab = []
			self.isLoaded = False

			self.__LoadWindow()
			self.AdjustPosition()

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def Destroy(self):
			self.ClearDictionary()

		def __LoadWindow(self):
			if self.isLoaded:
				return

			buttons = [
				{ 
					"text" : uiScriptLocale.INVENTORY_TITLE, 
					"up_visual" : self.IMG_PATH + "tab_inventory_01_normal.png" ,
					"over_visual" : self.IMG_PATH + "tab_inventory_01_normal.png",
					"down_visual" : self.IMG_PATH + "tab_inventory_03_active.png",
				},
				{ 
					"text" : uiScriptLocale.SPECIAL_INVENTORY_SKILLBOOK_TITLE, 
					"up_visual" : self.IMG_PATH + "tab_skillbook_01_normal.png" , 
					"over_visual" : self.IMG_PATH + "tab_skillbook_01_normal.png", 
					"down_visual" : self.IMG_PATH + "tab_skillbook_03_active.png",
				},
				{ 
					"text" : uiScriptLocale.SPECIAL_INVENTORY_STONE_TITLE, 
					"up_visual" : self.IMG_PATH + "tab_stone_01_normal.png" , 
					"over_visual" : self.IMG_PATH + "tab_stone_01_normal.png", 
					"down_visual" : self.IMG_PATH + "tab_stone_03_active.png",
				},
				{ 
					"text" : uiScriptLocale.SPECIAL_INVENTORY_MATERIAL_TITLE, 
					"up_visual" : self.IMG_PATH + "tab_material_01_normal.png" , 
					"over_visual" : self.IMG_PATH + "tab_material_01_normal.png", 
					"down_visual" : self.IMG_PATH + "tab_material_03_active.png",
				},
			]

			self.SetSize(self.IMG_WIDTH, (self.IMG_HEIGHT + self.Y_SPACE) * len(buttons) - self.Y_SPACE)

			i = 0
			for button in buttons:
				tab = ui.RadioButton()
				tab.SetParent(self)
				tab.SetToolTipText(button["text"])
				tab.SetUpVisual(button["up_visual"])
				tab.SetOverVisual(button["over_visual"])
				tab.SetDownVisual(button["down_visual"])
				tab.SetPosition(0, (self.IMG_HEIGHT + self.Y_SPACE) * i)
				tab.Show()

				self.buttonTab.append(tab)
				i += 1

			self.isLoaded = True

		def __TabExists(self, i):
			if i > len(self.buttonTab)+1:
				return False	

			return True

		def SetTabEvent(self, i, event):
			if not self.__TabExists(i):
				return

			self.buttonTab[i].SetEvent(event)

		def SetUp(self, i):
			if not self.__TabExists(i):
				return	

			self.buttonTab[i].SetUp()

		def Down(self, i):
			if not self.__TabExists(i):
				return	

			self.buttonTab[i].Down()		

		def AdjustPosition(self):
			x, y = self.wndInventory.GetGlobalPosition()
			self.SetPosition(x - self.IMG_WIDTH + 1, y + 270)	

class EquipmentTypeTabWindow(ui.ScriptWindow):

	IMG_PATH = "d:/ymir work/ui/special_equipment/"
	IMG_WIDTH = 25#20
	IMG_HEIGHT = 22#18
	Y_SPACE = 3

	TAB_EQUIPMENT = 0
	TAB_COSTUME = 1

	def __init__(self, wndInventory):
		if not wndInventory:
			exception.Abort("wndInventory parameter must be set to InventoryWindow")
			return	

		ui.ScriptWindow.__init__(self)
		self.wndInventory = wndInventory

		self.buttonTab = []
		self.isLoaded = False

		self.__LoadWindow()
		self.AdjustPosition()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.ClearDictionary()

	def __LoadWindow(self):
		if self.isLoaded:
			return

		buttons = [
			{ 
				"text" :localeInfo.TARGET_BUTTON_VIEW_EQUIPMENT,
				"up_visual" : self.IMG_PATH + "tab_0_default.png" ,
				"over_visual" : self.IMG_PATH + "tab_0_default.png",
				"down_visual" : self.IMG_PATH + "tab_0_down.png",
			},
			{ 
				"text" :localeInfo.CATEGORY_COSTUMES,
				"up_visual" : self.IMG_PATH + "tab_1_default.png" , 
				"over_visual" : self.IMG_PATH + "tab_1_default.png", 
				"down_visual" : self.IMG_PATH + "tab_1_down.png",
			},
		]

		self.SetSize(self.IMG_WIDTH, (self.IMG_HEIGHT + self.Y_SPACE) * len(buttons) - self.Y_SPACE)

		i = 0
		for button in buttons:
			tab = ui.RadioButton()
			tab.SetParent(self)
			tab.SetToolTipText2(button["text"])
			tab.SetUpVisual(button["up_visual"])
			tab.SetOverVisual(button["over_visual"])
			tab.SetDownVisual(button["down_visual"])
		#	tab.SetPosition(0, (self.IMG_HEIGHT + self.Y_SPACE) * i)
			tab.SetPosition(0, (self.IMG_HEIGHT) * i)
			tab.Show()

			self.buttonTab.append(tab)
			i += 1

		self.isLoaded = True

	def __TabExists(self, i):
		if i > len(self.buttonTab)+1:
			return False	

		return True

	def SetTabEvent(self, i, event):
		if not self.__TabExists(i):
			return

		self.buttonTab[i].SetEvent(event)

	def SetUp(self, i):
		if not self.__TabExists(i):
			return	

		self.buttonTab[i].SetUp()

	def Down(self, i):
		if not self.__TabExists(i):
			return	

		self.buttonTab[i].Down()		

	def AdjustPosition(self):
		x, y = self.wndInventory.GetGlobalPosition()
		self.SetPosition(x - self.IMG_WIDTH + 3, y + 32 )#35

class InventoryWindow(ui.ScriptWindow):

	USE_TYPE_TUPLE = ("USE_CLEAN_SOCKET", "USE_CHANGE_ATTRIBUTE", "USE_ADD_ATTRIBUTE", "USE_ADD_ATTRIBUTE2", "USE_ADD_ACCESSORY_SOCKET", "USE_PUT_INTO_ACCESSORY_SOCKET", "USE_PUT_INTO_BELT_SOCKET", "USE_PUT_INTO_RING_SOCKET", "USE_SPECIAL", "USE_CHANGE_COSTUME_ATTR", "USE_RESET_COSTUME_ATTR")

	questionDialog = None
	tooltipItem = None
	wndBelt = None
	
	if app.ENABLE_SPECIAL_INVENTORY:
		wndInventoryTypeTab = None
		inventoryTypeIndex = 0
	
	interface = None
	
	if app.WJ_ENABLE_TRADABLE_ICON:
		bindWnds = []
	
	if app.HIGHLIGHT_SLOT:
		liHighlightedItems = []
	
	wndEquipmentTypeTab = None
	equipmentTypeIndex = 0
	dlgPickMoney = None
	
	sellingSlotNumber = -1
	isLoaded = 0
	isOpenedCostumeWindowWhenClosingInventory = 0		# 인벤토리 닫을 때 코스츔이 열려있었는지 여부-_-; 네이밍 ㅈㅅ
	isOpenedBeltWindowWhenClosingInventory = 0		# 인벤토리 닫을 때 벨트 인벤토리가 열려있었는지 여부-_-; 네이밍 ㅈㅅ

	acceSlots = []

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.isOpenedBeltWindowWhenClosingInventory = 0		# 인벤토리 닫을 때 벨트 인벤토리가 열려있었는지 여부-_-; 네이밍 ㅈㅅ

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.__LoadWindow()

		ui.ScriptWindow.Show(self)

		# 인벤토리를 닫을 때 코스츔이 열려있었다면 인벤토리를 열 때 코스츔도 같이 열도록 함.
		if self.isOpenedCostumeWindowWhenClosingInventory and self.wndCostume:
			self.wndCostume.Show() 

		# 인벤토리를 닫을 때 벨트 인벤토리가 열려있었다면 같이 열도록 함.
		if self.wndBelt:
			self.wndBelt.Show(self.isOpenedBeltWindowWhenClosingInventory)
			
		if app.ENABLE_SPECIAL_INVENTORY and self.wndInventoryTypeTab:
			self.wndInventoryTypeTab.Show()
			
		if self.wndEquipmentTypeTab:
			self.wndEquipmentTypeTab.Show()

	def BindInterfaceClass(self, interface):
		self.interface = interface
		
	if app.WJ_ENABLE_TRADABLE_ICON:
		def BindWindow(self, wnd):
			self.bindWnds.append(wnd)
		
	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()

			if ITEM_MALL_BUTTON_ENABLE:
				pyScrLoader.LoadScriptFile(self, uiScriptLocale.LOCALE_UISCRIPT_PATH + "InventoryWindow.py")
			else:
				pyScrLoader.LoadScriptFile(self, "UIScript/InventoryWindow.py")
		except:
			import exception
			exception.Abort("InventoryWindow.LoadWindow.LoadObject")

		try:
			wndItem = self.GetChild("ItemSlot")
			wndEquip = self.GetChild("EquipmentSlot")
			# wndEquip1 = self.GetChild("Artefact_Slot")
			wndEquip1 = self.GetChild("Costume_Slot")
			# wndEquip4 = self.GetChild("Shining_Slot")
			self.wndEquipmentType0 = self.GetChild("Equipment_Base_Tab_0")
			self.wndEquipmentType1 = self.GetChild("Equipment_Base_Tab_1")
			# self.wndEquipmentType2 = self.GetChild("Equipment_Base_Tab_2")
			# self.wndEquipmentType3 = self.GetChild("Equipment_Base_Tab_3")
			# self.wndEquipmentType4 = self.GetChild("Equipment_Base_Tab_4")
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
			self.wndMoney = self.GetChild("Money")
			self.wndMoneySlot = self.GetChild("Money_Slot")
			self.mallButton = self.GetChild2("MallButton")
			self.DSSButton = self.GetChild2("DSSButton")

			self.inventoryTab = []
			self.inventoryTab.append(self.GetChild("Inventory_Tab_01"))
			self.inventoryTab.append(self.GetChild("Inventory_Tab_02"))
			self.inventoryTab.append(self.GetChild("Inventory_Tab_03"))
			self.inventoryTab.append(self.GetChild("Inventory_Tab_04"))
			
			self.equipmentTab = []
			self.equipmentTab.append(self.GetChild("Equipment_Tab_01"))
			self.equipmentTab.append(self.GetChild("Equipment_Tab_02"))

			self.wndEquipmentTypeTab = EquipmentTypeTabWindow(self)
			# Belt Inventory Window
			
#			if app.ENABLE_INVENTORY_SORT:
#				self.sortButton = self.GetChild("SortButton")
#				self.sortButton.SetEvent(self.OnSortInventory)	
			
			self.wndBelt = None
				
			if app.ENABLE_SPECIAL_INVENTORY:
				self.wndInventoryTypeTab = InventoryTypeTabWindow(self)
			
			self.wndBelt = BeltInventoryWindow(self)

			self.dlgQuestion = uiCommon.QuestionDialog2()
			self.dlgQuestion.Close()

		except:
			import exception
			exception.Abort("InventoryWindow.LoadWindow.BindObject")

		## Item
		wndItem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		wndItem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		wndItem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndItem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		## Equipment
		wndEquip.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		wndEquip.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		wndEquip.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndEquip.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndEquip.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		wndEquip.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		## Costume
		wndEquip1.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		wndEquip1.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		wndEquip1.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndEquip1.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndEquip1.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		wndEquip1.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		## PickMoneyDialog
		dlgPickMoney = uiPickMoney.PickMoneyDialog()
		dlgPickMoney.LoadDialog()
		dlgPickMoney.Hide()

		## RefineDialog
		self.refineDialog = uiRefine.RefineDialog()
		self.refineDialog.Hide()

		## AttachMetinDialog
		if app.WJ_ENABLE_TRADABLE_ICON:  
			self.attachMetinDialog = uiAttachMetin.AttachMetinDialog(self)
			self.BindWindow(self.attachMetinDialog)
		else:
			self.attachMetinDialog = uiAttachMetin.AttachMetinDialog()
		self.attachMetinDialog.Hide()

		## MoneySlot
		self.wndMoneySlot.SetEvent(ui.__mem_func__(self.OpenPickMoneyDialog))

		self.inventoryTab[0].SetEvent(lambda arg=0: self.SetInventoryPage(arg))
		self.inventoryTab[1].SetEvent(lambda arg=1: self.SetInventoryPage(arg))
		self.inventoryTab[2].SetEvent(lambda arg=2: self.SetInventoryPage(arg))
		self.inventoryTab[3].SetEvent(lambda arg=3: self.SetInventoryPage(arg))
		self.inventoryTab[0].Down()
		
		if app.ENABLE_SPECIAL_INVENTORY:
			self.wndInventoryTypeTab.SetTabEvent(InventoryTypeTabWindow.TAB_INVENTORY,	lambda arg = 0 : self.SetInventoryType(arg))
			self.wndInventoryTypeTab.SetTabEvent(InventoryTypeTabWindow.TAB_SKILLBOOK,	lambda arg = 1 : self.SetInventoryType(arg))
			self.wndInventoryTypeTab.SetTabEvent(InventoryTypeTabWindow.TAB_STONE,		lambda arg = 2 : self.SetInventoryType(arg))
			self.wndInventoryTypeTab.SetTabEvent(InventoryTypeTabWindow.TAB_MATERIAL,	lambda arg = 3 : self.SetInventoryType(arg))
			self.wndInventoryTypeTab.Down(InventoryTypeTabWindow.TAB_INVENTORY)
		
		self.inventoryPageIndex = 0

		self.wndEquipmentTypeTab.SetTabEvent(EquipmentTypeTabWindow.TAB_EQUIPMENT,	lambda arg = 0 : self.SetEquipmentType(arg))
		self.wndEquipmentTypeTab.SetTabEvent(EquipmentTypeTabWindow.TAB_COSTUME,	lambda arg = 1 : self.SetEquipmentType(arg))
		self.wndEquipmentTypeTab.Down(EquipmentTypeTabWindow.TAB_EQUIPMENT)
		self.equipmentTab[0].SetEvent(lambda arg=0: self.SetEquipmentPage(arg))
		self.equipmentTab[1].SetEvent(lambda arg=1: self.SetEquipmentPage(arg))
		self.equipmentTab[0].Down()
		self.equipmentTab[1].Hide()
		self.equipmentTab[0].Hide()
		self.equipmentPageIndex = 0

		self.wndItem = wndItem
		self.wndEquip = wndEquip
		self.wndEquip1 = wndEquip1
		self.dlgPickMoney = dlgPickMoney

		# MallButton
		if self.mallButton:
			self.mallButton.SetEvent(ui.__mem_func__(self.ClickMallButton))

		if self.DSSButton:
			#self.DSSButton.Hide()
			self.DSSButton.SetEvent(ui.__mem_func__(self.ClickDSSButton)) 
		

 		#####

		## Refresh
		self.SetInventoryPage(0)
		self.SetEquipmentPage(0)
		if app.ENABLE_SPECIAL_INVENTORY:
			self.SetInventoryType(0)
		self.SetEquipmentType(0)
		self.RefreshItemSlot()
		self.RefreshStatus()

	if app.ENABLE_INVENTORY_SORT:
		def OnSortInventory(self):
			self.questionDialog = uiCommon.QuestionDialog()
			self.questionDialog.SetText(localeInfo.INVENTORY_SORT_QUESTION)
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.AcceptSortInventory))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.CancelSortInventory))
			self.questionDialog.Open()
			self.questionDialog.inventoryTypeIndex = self.inventoryTypeIndex

		def CancelSortInventory(self):
			self.OnCloseQuestionDialog()

		def AcceptSortInventory(self):
			if self.questionDialog:
				if self.questionDialog.inventoryTypeIndex == player.INVENTORY_TYPE_INVENTORY:
					net.SendChatPacket("/sort_inventory")
				else:
					net.SendChatPacket("/sort_special_inventory %i" % (self.questionDialog.inventoryTypeIndex-1))
	
				self.questionDialog.Close()

			self.questionDialog = None

	def Destroy(self):
		self.ClearDictionary()

		self.dlgPickMoney.Destroy()
		self.dlgPickMoney = 0

		self.refineDialog.Destroy()
		self.refineDialog = 0

		self.attachMetinDialog.Destroy()
		self.attachMetinDialog = 0

		self.tooltipItem = None
		self.wndItem = 0
		self.wndEquip = 0
		self.wndEquip1 = 0
		self.dlgPickMoney = 0
		self.wndMoney = 0
		self.wndMoneySlot = 0
		self.questionDialog = None
		self.mallButton = None
		self.DSSButton = None
		self.interface = None
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.bindWnds = []

			
		if self.wndBelt:
			self.wndBelt.Destroy()
			self.wndBelt = None
			
		if app.ENABLE_SPECIAL_INVENTORY and self.wndInventoryTypeTab:
			self.inventoryTypeIndex = 0
			self.wndInventoryTypeTab.Destroy()
			self.wndInventoryTypeTab = None
			
		if self.wndEquipmentTypeTab:
			self.equipmentTypeIndex = 0
			self.wndEquipmentTypeTab.Destroy()
			self.wndEquipmentTypeTab = None
		self.inventoryTab = []
		self.equipmentTab = []

		self.acceSlots = []

	def Hide(self):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			self.OnCloseQuestionDialog()
			return
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()

 
		if self.wndBelt:
			self.isOpenedBeltWindowWhenClosingInventory = self.wndBelt.IsOpeningInventory()		# 인벤토리 창이 닫힐 때 벨트 인벤토리도 열려 있었는가?
			print "Is Opening Belt Inven?? ", self.isOpenedBeltWindowWhenClosingInventory
			self.wndBelt.Close()
  		if self.wndEquipmentTypeTab:
			self.wndEquipmentTypeTab.Hide()
  
		if self.dlgPickMoney:
			self.dlgPickMoney.Close()
		
		if app.ENABLE_SPECIAL_INVENTORY and self.wndInventoryTypeTab:
			self.wndInventoryTypeTab.Hide()
		
		wndMgr.Hide(self.hWnd)
		
	
	def Close(self):
		self.Hide()

	def SetInventoryPage(self, page):
		self.inventoryTab[self.inventoryPageIndex].SetUp()
		self.inventoryPageIndex = page
		self.inventoryTab[self.inventoryPageIndex].Down()
		if app.ENABLE_SPECIAL_INVENTORY:	
			if self.inventoryTypeIndex == player.INVENTORY_TYPE_INVENTORY:
				self.RefreshBagSlotWindow()
			else:
				self.RefreshSpecialInventory()
		else:
			self.RefreshBagSlotWindow()
	
	def SetEquipmentPage(self, page):
		self.equipmentPageIndex = page
		self.equipmentTab[1-page].SetUp()
		self.RefreshEquipSlotWindow()

	if app.ENABLE_SPECIAL_INVENTORY:
		def SetInventoryType(self, type):
			self.wndInventoryTypeTab.SetUp(self.inventoryTypeIndex)
			self.inventoryTypeIndex = int(type)
			self.wndInventoryTypeTab.Down(self.inventoryTypeIndex)
			
			if self.inventoryTypeIndex == player.INVENTORY_TYPE_INVENTORY:
				self.RefreshBagSlotWindow()
			else:
				self.RefreshSpecialInventory()
				
		def GetInventoryType(self):
			return self.inventoryTypeIndex
				
		def __SpecialInventoryLocalSlotPosToGlobalSlotPos(self, local):

			if player.IsEquipmentSlot(local) or player.IsCostumeSlot(local):
				return local

			start = player.SPECIAL_INVENTORY_SLOT_START
			slotCountPerType = (player.SPECIAL_INVENTORY_PAGE_SIZE * player.INVENTORY_PAGE_COUNT)
			slotStartForType = start + slotCountPerType * (self.inventoryTypeIndex-1)
			slotPageStart = slotStartForType + (player.SPECIAL_INVENTORY_PAGE_SIZE * self.inventoryPageIndex)

			return slotPageStart + local
				
		def RefreshSpecialInventory(self):
			for i in xrange(player.SPECIAL_INVENTORY_PAGE_SIZE):
				slotNumber = self.__SpecialInventoryLocalSlotPosToGlobalSlotPos(i)
				
				itemCount = player.GetItemCount(slotNumber)
				if 0 == itemCount:
					self.wndItem.ClearSlot(i)
					continue
				elif 1 == itemCount:
					itemCount = 0
					
				itemVnum = player.GetItemIndex(slotNumber)

				if app.HIGHLIGHT_SLOT:
					if itemVnum == 0 and slotNumber in self.liHighlightedItems:
						self.liHighlightedItems.remove(slotNumber)

					self.wndItem.DeactivateSlot(slotNumber)

				self.wndItem.SetItemSlot(i, itemVnum, itemCount)

#				if app.BL_ENABLE_PICKUP_ITEM_EFFECT and not constInfo.IS_AUTO_POTION(itemVnum):
#					if not slotNumber in self.listHighlightedSlot:
#						self.wndItem.DeactivateSlot(i)

				self.wndItem.DeactivateSlot(slotNumber)

			if app.HIGHLIGHT_SLOT:
				self.__RefreshHighlights()

			self.wndItem.RefreshSlot()

#			if app.WJ_ENABLE_TRADABLE_ICON:
#				map(lambda wnd:wnd.RefreshLockedSlot(), self.bindWnds)
#				self.RefreshMarkSlots()

	def SetEquipmentType(self, type):
		self.wndEquipmentTypeTab.SetUp(self.equipmentTypeIndex)
		self.equipmentTypeIndex = int(type)
		self.wndEquipmentTypeTab.Down(self.equipmentTypeIndex)
		buttons = [
			{0 : self.wndEquipmentType0,},
			{1 : self.wndEquipmentType1,},
		]
		i = 0
		for button in buttons:
			if self.equipmentTypeIndex == i:
				buttons[i][i].Show()
			else:
				buttons[i][i].Hide()
			i = i+1
		self.RefreshEquipSlotWindow()
	def ClickMallButton(self):
		print "click_mall_button"
		net.SendChatPacket("/click_mall")

	# DSSButton
	def ClickDSSButton(self):
		print "click_dss_button"
		self.interface.ToggleDragonSoulWindow()

	def OpenPickMoneyDialog(self):

		if mouseModule.mouseController.isAttached():

			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			if player.SLOT_TYPE_SAFEBOX == mouseModule.mouseController.GetAttachedType():

				if player.ITEM_MONEY == mouseModule.mouseController.GetAttachedItemIndex():
					net.SendSafeboxWithdrawMoneyPacket(mouseModule.mouseController.GetAttachedItemCount())
					snd.PlaySound("sound/ui/money.wav")

			mouseModule.mouseController.DeattachObject()

		else:
			curMoney = player.GetElk()

			if curMoney <= 0:
				return

			self.dlgPickMoney.SetTitleName(localeInfo.PICK_MONEY_TITLE)
			self.dlgPickMoney.SetAcceptEvent(ui.__mem_func__(self.OnPickMoney))
			self.dlgPickMoney.Open(curMoney)
			self.dlgPickMoney.SetMax(7)

	def OnPickMoney(self, money):
		mouseModule.mouseController.AttachMoney(self, player.SLOT_TYPE_INVENTORY, money)

	def OnPickItem(self, count):
		itemSlotIndex = self.dlgPickMoney.itemGlobalSlotIndex
		selectedItemVNum = player.GetItemIndex(itemSlotIndex)
		mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum, count)

	def __InventoryLocalSlotPosToGlobalSlotPos(self, local):
		if player.IsEquipmentSlot(local) or player.IsCostumeSlot(local) or player.IsBeltInventorySlot(local):
			return local

		return self.inventoryPageIndex*player.INVENTORY_PAGE_SIZE + local

	def GetInventoryPageIndex(self):
		return self.inventoryPageIndex

	if app.WJ_ENABLE_TRADABLE_ICON:
		def RefreshMarkSlots(self, localIndex=None):
			if not self.interface:
				return

			onTopWnd = self.interface.GetOnTopWindow()
			if localIndex:
				if app.ENABLE_SPECIAL_INVENTORY:
					if self.inventoryTypeIndex == player.INVENTORY_TYPE_INVENTORY:
						slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(localIndex)
					else:
						slotNumber = self.__SpecialInventoryLocalSlotPosToGlobalSlotPos(localIndex)
				else:
					slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(localIndex)
					
#				if onTopWnd == player.ON_TOP_WND_NONE:
#					self.wndItem.SetUsableSlotOnTopWnd(localIndex)

				if onTopWnd == player.ON_TOP_WND_SHOP:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_SELL):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)

				elif onTopWnd == player.ON_TOP_WND_EXCHANGE:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_GIVE):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)

				elif onTopWnd == player.ON_TOP_WND_PRIVATE_SHOP:
					if player.IsAntiFlagBySlot(slotNumber, item.ITEM_ANTIFLAG_MYSHOP):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)

				elif onTopWnd == player.ON_TOP_WND_SAFEBOX:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_SAFEBOX):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)

				return

			for i in xrange(player.INVENTORY_PAGE_SIZE):
				if app.ENABLE_SPECIAL_INVENTORY:
					if self.inventoryTypeIndex == player.INVENTORY_TYPE_INVENTORY:
						slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)
					else:
						slotNumber = self.__SpecialInventoryLocalSlotPosToGlobalSlotPos(i)
				else:
					slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)	
						
#				if onTopWnd == player.ON_TOP_WND_NONE:
#					self.wndItem.SetUsableSlotOnTopWnd(i)

				if onTopWnd == player.ON_TOP_WND_SHOP:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_SELL):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)

				elif onTopWnd == player.ON_TOP_WND_EXCHANGE:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_GIVE):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)

				elif onTopWnd == player.ON_TOP_WND_PRIVATE_SHOP:
					if player.IsAntiFlagBySlot(slotNumber, item.ITEM_ANTIFLAG_MYSHOP):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)

				elif onTopWnd == player.ON_TOP_WND_SAFEBOX:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_SAFEBOX):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)

	if app.HIGHLIGHT_SLOT:
		def HighlightSlot(self, slot):
			if not slot in self.liHighlightedItems:
				self.liHighlightedItems.append(slot)
		
		def __RefreshHighlights(self):
			for i in xrange(player.INVENTORY_PAGE_SIZE):
				if app.ENABLE_SPECIAL_INVENTORY:
					if self.inventoryTypeIndex == player.INVENTORY_TYPE_INVENTORY:
						slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)
					else:
						slotNumber = self.__SpecialInventoryLocalSlotPosToGlobalSlotPos(i)
				else:
					slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)

				if slotNumber in self.liHighlightedItems:
					self.wndItem.ActivateSlot(i)

#	def RefreshBagSlotWindow(self):
#		getItemVNum=player.GetItemIndex
#		getItemCount=player.GetItemCount
#		setItemVNum=self.wndItem.SetItemSlot
#		
#		for i in xrange(player.INVENTORY_PAGE_SIZE):
#			slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)
#
#			itemCount = getItemCount(slotNumber)
#			if 0 == itemCount:
#				self.wndItem.ClearSlot(i)
#				continue
#			elif 1 == itemCount:
#				itemCount = 0
#				
#			itemVnum = getItemVNum(slotNumber)
#			setItemVNum(i, itemVnum, itemCount)
#			
#			
#			if slotNumber in self.acceSlots:
#				self.wndItem.ActivateSlot(i, 28.0 / 255.0, 255.0 / 255.0, 8.0 / 255.0)
#			else:
#				self.wndItem.DeactivateSlot(i)
#
#			if constInfo.IS_AUTO_POTION(itemVnum):
#				metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]	
#				
#				if slotNumber >= player.INVENTORY_PAGE_SIZE*self.inventoryPageIndex:
#					slotNumber -= player.INVENTORY_PAGE_SIZE*self.inventoryPageIndex
#					
#				isActivated = 0 != metinSocket[0]
#				
#				if isActivated:
#					self.wndItem.ActivateSlot(slotNumber)
#					potionType = 0;
#					if constInfo.IS_AUTO_POTION_HP(itemVnum):
#						potionType = player.AUTO_POTION_TYPE_HP
#					elif constInfo.IS_AUTO_POTION_SP(itemVnum):
#						potionType = player.AUTO_POTION_TYPE_SP						
#					
#					usedAmount = int(metinSocket[1])
#					totalAmount = int(metinSocket[2])					
#					player.SetAutoPotionInfo(potionType, isActivated, (totalAmount - usedAmount), totalAmount, self.__InventoryLocalSlotPosToGlobalSlotPos(i))
#					
#				else:
#					self.wndItem.DeactivateSlot(slotNumber)
#
#		self.wndItem.RefreshSlot()
#
#		if self.wndBelt:
#			self.wndBelt.RefreshSlot()

	def RefreshBagSlotWindow(self):
		getItemVNum=player.GetItemIndex
		getItemCount=player.GetItemCount
		setItemVNum=self.wndItem.SetItemSlot

		for i in xrange(player.INVENTORY_PAGE_SIZE):
			slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)

			itemCount = getItemCount(slotNumber)
			# itemCount == 0이면 소켓을 비운다.
			if 0 == itemCount:
				self.wndItem.ClearSlot(i)
				continue
			elif 1 == itemCount:
				itemCount = 0

			itemVnum = getItemVNum(slotNumber)
			setItemVNum(i, itemVnum, itemCount)

			if app.HIGHLIGHT_SLOT:
				if itemVnum == 0 and slotNumber in self.liHighlightedItems:
					self.liHighlightedItems.remove(slotNumber)

				self.wndItem.DeactivateSlot(slotNumber)

				## 자동물약 (HP: #72723 ~ #72726, SP: #72727 ~ #72730) 특수처리 - 아이템인데도 슬롯에 활성화/비활성화 표시를 위한 작업임 - [hyo]
				if constInfo.IS_AUTO_POTION(itemVnum):
					# metinSocket - [0] : 활성화 여부, [1] : 사용한 양, [2] : 최대 용량
					metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]

					isActivated = 0 != metinSocket[0]

					if isActivated:
						self.wndItem.ActivateSlot(i)
						potionType = 0;
						if constInfo.IS_AUTO_POTION_HP(itemVnum):
							potionType = player.AUTO_POTION_TYPE_HP
						elif constInfo.IS_AUTO_POTION_SP(itemVnum):
							potionType = player.AUTO_POTION_TYPE_SP

						usedAmount = int(metinSocket[1])
						totalAmount = int(metinSocket[2])
						player.SetAutoPotionInfo(potionType, isActivated, (totalAmount - usedAmount), totalAmount, self.__InventoryLocalSlotPosToGlobalSlotPos(i))

					else:
						self.wndItem.DeactivateSlot(i)

			elif itemVnum >= 53001 and itemVnum <= 53256:
				metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]# <!> globalSlotNumber may be different <!>
				isActivated = 0 != metinSocket[1]
				if isActivated:
					self.wndItem.ActivateSlot(i)
				else:
					self.wndItem.DeactivateSlot(i)

			if app.WJ_ENABLE_TRADABLE_ICON:
				self.RefreshMarkSlots(i)

		if app.HIGHLIGHT_SLOT:
			self.__RefreshHighlights()

		self.wndItem.RefreshSlot()

		if self.wndBelt:
			self.wndBelt.RefreshSlot()

#		if app.WJ_ENABLE_TRADABLE_ICON:
#			map(lambda wnd:wnd.RefreshLockedSlot(), self.bindWnds)

	def RefreshEquipSlotWindow(self):
		getItemVNum=player.GetItemIndex
		getItemCount=player.GetItemCount
		setItemVNum=self.wndEquip.SetItemSlot
		for i in xrange(player.EQUIPMENT_PAGE_COUNT):
			slotNumber = player.EQUIPMENT_SLOT_START + i
			itemCount = getItemCount(slotNumber)
			if itemCount <= 1:
				itemCount = 0
			setItemVNum(slotNumber, getItemVNum(slotNumber), itemCount)

	##	setItemVNum(item.COSTUME_SLOT_ACCE, getItemVNum(item.COSTUME_SLOT_ACCE), 0)

		self.wndEquip.RefreshSlot()

		for i in xrange(item.COSTUME_SLOT_COUNT):
			slotNumber = item.COSTUME_SLOT_START + i
			self.wndEquip1.SetItemSlot(slotNumber, getItemVNum(slotNumber), 0)
		self.wndEquip1.RefreshSlot()

	def RefreshItemSlot(self):
		if app.ENABLE_SPECIAL_INVENTORY:
			if self.inventoryTypeIndex == player.INVENTORY_TYPE_INVENTORY:
				self.RefreshBagSlotWindow()
			else:
				self.RefreshSpecialInventory()	
		else:
			self.RefreshBagSlotWindow()
		self.RefreshEquipSlotWindow()

	def RefreshStatus(self):
		money = player.GetElk()
		self.wndMoney.SetText(localeInfo.NumberToMoneyStringOld(money))

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem

	def SellItem(self):
		if self.sellingSlotitemIndex == player.GetItemIndex(self.sellingSlotNumber):
			if self.sellingSlotitemCount == player.GetItemCount(self.sellingSlotNumber):
				## 용혼석도 팔리게 하는 기능 추가하면서 인자 type 추가
				net.SendShopSellPacketNew(self.sellingSlotNumber, self.questionDialog.count, player.INVENTORY)
				snd.PlaySound("sound/ui/money.wav")
		self.OnCloseQuestionDialog()

	def OnDetachMetinFromItem(self):
		if None == self.questionDialog:
			return
			
		#net.SendItemUseToItemPacket(self.questionDialog.sourcePos, self.questionDialog.targetPos)		
		self.__SendUseItemToItemPacket(self.questionDialog.sourcePos, self.questionDialog.targetPos)
		self.OnCloseQuestionDialog()

	def OnCloseQuestionDialog(self):
		if not self.questionDialog:
			return
		
		self.questionDialog.Close()
		self.questionDialog = None
		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	## Slot Event
	def SelectEmptySlot(self, selectedSlotPos):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		if app.ENABLE_SPECIAL_INVENTORY:
			if self.inventoryTypeIndex == player.INVENTORY_TYPE_INVENTORY:
				selectedSlotPos = self.__InventoryLocalSlotPosToGlobalSlotPos(selectedSlotPos)
			else:
				selectedSlotPos = self.__SpecialInventoryLocalSlotPosToGlobalSlotPos(selectedSlotPos)
		else:
			selectedSlotPos = self.__InventoryLocalSlotPosToGlobalSlotPos(selectedSlotPos)	

		if mouseModule.mouseController.isAttached():

			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemCount = mouseModule.mouseController.GetAttachedItemCount()
			attachedItemIndex = mouseModule.mouseController.GetAttachedItemIndex()

			if player.SLOT_TYPE_INVENTORY == attachedSlotType:
				itemCount = player.GetItemCount(attachedSlotPos)
				attachedCount = mouseModule.mouseController.GetAttachedItemCount()
				self.__SendMoveItemPacket(attachedSlotPos, selectedSlotPos, attachedCount)

				if item.IsRefineScroll(attachedItemIndex):
					self.wndItem.SetUseMode(FALSE)

			elif app.ENABLE_SWITCHBOT and player.SLOT_TYPE_SWITCHBOT == attachedSlotType:
				attachedCount = mouseModule.mouseController.GetAttachedItemCount()
				net.SendItemMovePacket(player.SWITCHBOT, attachedSlotPos, player.INVENTORY, selectedSlotPos, attachedCount)

			elif player.SLOT_TYPE_PRIVATE_SHOP == attachedSlotType:
				mouseModule.mouseController.RunCallBack("INVENTORY")

			elif player.SLOT_TYPE_SHOP == attachedSlotType:
				net.SendShopBuyPacket(attachedSlotPos)

			elif player.SLOT_TYPE_SAFEBOX == attachedSlotType:

				if player.ITEM_MONEY == attachedItemIndex:
					net.SendSafeboxWithdrawMoneyPacket(mouseModule.mouseController.GetAttachedItemCount())
					snd.PlaySound("sound/ui/money.wav")

				else:
					net.SendSafeboxCheckoutPacket(attachedSlotPos, selectedSlotPos)

			elif player.SLOT_TYPE_MALL == attachedSlotType:
				net.SendMallCheckoutPacket(attachedSlotPos, selectedSlotPos)

			elif app.ENABLE_ASLAN_BUFF_NPC_SYSTEM and player.SLOT_TYPE_BUFF_EQUIPMENT == attachedSlotType:
				attachedCount = mouseModule.mouseController.GetAttachedItemCount()
				net.SendItemMovePacket(player.BUFF_EQUIPMENT, attachedSlotPos, player.INVENTORY, selectedSlotPos, attachedCount)

			mouseModule.mouseController.DeattachObject()

	def SelectItemSlot(self, itemSlotIndex):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		if app.ENABLE_SPECIAL_INVENTORY:
			if self.inventoryTypeIndex == player.INVENTORY_TYPE_INVENTORY:
				itemSlotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(itemSlotIndex)
			else:
				itemSlotIndex = self.__SpecialInventoryLocalSlotPosToGlobalSlotPos(itemSlotIndex)
		else:
			itemSlotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(itemSlotIndex)	

		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemVID = mouseModule.mouseController.GetAttachedItemIndex()

			if player.SLOT_TYPE_INVENTORY == attachedSlotType:
				self.__DropSrcItemToDestItemInInventory(attachedItemVID, attachedSlotPos, itemSlotIndex)

			mouseModule.mouseController.DeattachObject()

		else:

			curCursorNum = app.GetCursor()
			if app.SELL == curCursorNum:
				self.__SellItem(itemSlotIndex)
				
			elif app.BUY == curCursorNum:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SHOP_BUY_INFO)

			elif app.IsPressed(app.DIK_LALT):
				link = player.GetItemLink(itemSlotIndex)
				ime.PasteString(link)

			elif app.IsPressed(app.DIK_LSHIFT):
				itemCount = player.GetItemCount(itemSlotIndex)
				
				if itemCount > 1:
					self.dlgPickMoney.SetTitleName(localeInfo.PICK_ITEM_TITLE)
					self.dlgPickMoney.SetAcceptEvent(ui.__mem_func__(self.OnPickItem))
					self.dlgPickMoney.Open(itemCount)
					self.dlgPickMoney.itemGlobalSlotIndex = itemSlotIndex
				#else:
					#selectedItemVNum = player.GetItemIndex(itemSlotIndex)
					#mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum)

			elif app.IsPressed(app.DIK_LCONTROL):
				itemIndex = player.GetItemIndex(itemSlotIndex)

				if TRUE == item.CanAddToQuickSlotItem(itemIndex):
					player.RequestAddToEmptyLocalQuickSlot(player.SLOT_TYPE_INVENTORY, itemSlotIndex)
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.QUICKSLOT_REGISTER_DISABLE_ITEM)

			else:
				selectedItemVNum = player.GetItemIndex(itemSlotIndex)
				itemCount = player.GetItemCount(itemSlotIndex)
				mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum, itemCount)
				
				if self.__IsUsableItemToItem(selectedItemVNum, itemSlotIndex):				
					self.wndItem.SetUseMode(TRUE)
				else:					
					self.wndItem.SetUseMode(FALSE)

				snd.PlaySound("sound/ui/pick.wav")

	def __DropSrcItemToDestItemInInventory(self, srcItemVID, srcItemSlotPos, dstItemSlotPos):
		if srcItemSlotPos == dstItemSlotPos:
			return
	         	
		elif item.IsRefineScroll(srcItemVID):
			self.RefineItem(srcItemSlotPos, dstItemSlotPos)
			self.wndItem.SetUseMode(FALSE)

		elif item.IsMetin(srcItemVID):
			self.AttachMetinToItem(srcItemSlotPos, dstItemSlotPos)

		elif item.IsDetachScroll(srcItemVID):
			self.DetachMetinFromItem(srcItemSlotPos, dstItemSlotPos)

		elif item.IsKey(srcItemVID):
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)			

		elif (player.GetItemFlags(srcItemSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)

		elif item.GetUseType(srcItemVID) in self.USE_TYPE_TUPLE:
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)			

		else:
			#snd.PlaySound("sound/ui/drop.wav")

			## 이동시킨 곳이 장착 슬롯일 경우 아이템을 사용해서 장착 시킨다 - [levites]
			if player.IsEquipmentSlot(dstItemSlotPos):

				## 들고 있는 아이템이 장비일때만
				if item.IsEquipmentVID(srcItemVID):
					self.__UseItem(srcItemSlotPos)

			else:
				self.__SendMoveItemPacket(srcItemSlotPos, dstItemSlotPos, 0)
				#net.SendItemMovePacket(srcItemSlotPos, dstItemSlotPos, 0)

	def __SellItem(self, itemSlotPos):
		if not player.IsEquipmentSlot(itemSlotPos):
			self.sellingSlotNumber = itemSlotPos
			itemIndex = player.GetItemIndex(itemSlotPos)
			itemCount = player.GetItemCount(itemSlotPos)

			self.sellingSlotitemIndex = itemIndex
			self.sellingSlotitemCount = itemCount

			item.SelectItem(itemIndex)

			if item.IsAntiFlag(item.ANTIFLAG_SELL):
				popup = uiCommon.PopupDialog()
				popup.SetText(localeInfo.SHOP_CANNOT_SELL_ITEM)
				popup.SetAcceptEvent(self.__OnClosePopupDialog)
				popup.Open()
				self.popup = popup
				return

			itemPrice = item.GetISellItemPrice()

			if item.Is1GoldItem():
				itemPrice = itemCount / itemPrice
			else:
				itemPrice = itemPrice * itemCount

			item.GetItemName(itemIndex)
			itemName = item.GetItemName()

			self.questionDialog = uiCommon.QuestionDialog()
			self.questionDialog.SetText(localeInfo.DO_YOU_SELL_ITEM(itemName, itemCount, itemPrice))
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.SellItem))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
			self.questionDialog.Open()
			self.questionDialog.count = itemCount
		
			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def __OnClosePopupDialog(self):
		self.pop = None

	def RefineItem(self, scrollSlotPos, targetSlotPos):

		scrollIndex = player.GetItemIndex(scrollSlotPos)
		targetIndex = player.GetItemIndex(targetSlotPos)

		if player.REFINE_OK != player.CanRefine(scrollIndex, targetSlotPos):
			return

		constInfo.AUTO_REFINE_TYPE = 1
		constInfo.AUTO_REFINE_DATA["ITEM"][0] = scrollSlotPos
		constInfo.AUTO_REFINE_DATA["ITEM"][1] = targetSlotPos

		self.__SendUseItemToItemPacket(scrollSlotPos, targetSlotPos)
		return

		result = player.CanRefine(scrollIndex, targetSlotPos)

		if player.REFINE_ALREADY_MAX_SOCKET_COUNT == result:
			#snd.PlaySound("sound/ui/jaeryun_fail.wav")
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NO_MORE_SOCKET)

		elif player.REFINE_NEED_MORE_GOOD_SCROLL == result:
			#snd.PlaySound("sound/ui/jaeryun_fail.wav")
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NEED_BETTER_SCROLL)

		elif player.REFINE_CANT_MAKE_SOCKET_ITEM == result:
			#snd.PlaySound("sound/ui/jaeryun_fail.wav")
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_SOCKET_DISABLE_ITEM)

		elif player.REFINE_NOT_NEXT_GRADE_ITEM == result:
			#snd.PlaySound("sound/ui/jaeryun_fail.wav")
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_UPGRADE_DISABLE_ITEM)

		elif player.REFINE_CANT_REFINE_METIN_TO_EQUIPMENT == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_EQUIP_ITEM)

		if player.REFINE_OK != result:
			return

		self.refineDialog.Open(scrollSlotPos, targetSlotPos)

	def DetachMetinFromItem(self, scrollSlotPos, targetSlotPos):
		scrollIndex = player.GetItemIndex(scrollSlotPos)
		targetIndex = player.GetItemIndex(targetSlotPos)

		if not player.CanDetach(scrollIndex, targetSlotPos):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_METIN_INSEPARABLE_ITEM)
			return

		self.questionDialog = uiCommon.QuestionDialog()
		self.questionDialog.SetText(localeInfo.REFINE_DO_YOU_SEPARATE_METIN)
		self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnDetachMetinFromItem))
		self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
		self.questionDialog.Open()
		self.questionDialog.sourcePos = scrollSlotPos
		self.questionDialog.targetPos = targetSlotPos

	def AttachMetinToItem(self, metinSlotPos, targetSlotPos):
		metinIndex = player.GetItemIndex(metinSlotPos)
		targetIndex = player.GetItemIndex(targetSlotPos)

		item.SelectItem(metinIndex)
		itemName = item.GetItemName()

		result = player.CanAttachMetin(metinIndex, targetSlotPos)

		if player.ATTACH_METIN_NOT_MATCHABLE_ITEM == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_CAN_NOT_ATTACH(itemName))

		if player.ATTACH_METIN_NO_MATCHABLE_SOCKET == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NO_SOCKET(itemName))

		elif player.ATTACH_METIN_NOT_EXIST_GOLD_SOCKET == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NO_GOLD_SOCKET(itemName))

		elif player.ATTACH_METIN_CANT_ATTACH_TO_EQUIPMENT == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_EQUIP_ITEM)

		if player.ATTACH_METIN_OK != result:
			return

		self.attachMetinDialog.Open(metinSlotPos, targetSlotPos)


		
	def OverOutItem(self):
		self.wndItem.SetUsableItem(FALSE)
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OverInItem(self, overSlotPos):
		if app.ENABLE_SPECIAL_INVENTORY:
			if self.inventoryTypeIndex == player.INVENTORY_TYPE_INVENTORY:
				overSlotPosGlobal = self.__InventoryLocalSlotPosToGlobalSlotPos(overSlotPos)
			else:
				overSlotPosGlobal = self.__SpecialInventoryLocalSlotPosToGlobalSlotPos(overSlotPos)
		else:
			overSlotPos = self.__InventoryLocalSlotPosToGlobalSlotPos(overSlotPos)	
		self.wndItem.SetUsableItem(FALSE)

		if app.HIGHLIGHT_SLOT and overSlotPosGlobal in self.liHighlightedItems:	
			self.liHighlightedItems.remove(overSlotPosGlobal)
			self.wndItem.DeactivateSlot(overSlotPos)

		if mouseModule.mouseController.isAttached():
			attachedItemType = mouseModule.mouseController.GetAttachedType()
			if player.SLOT_TYPE_INVENTORY == attachedItemType:

				attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
				attachedItemVNum = mouseModule.mouseController.GetAttachedItemIndex()

				if app.HIGHLIGHT_SLOT:
					if attachedItemVNum==player.ITEM_MONEY: # @fixme005
						pass
					elif self.__CanUseSrcItemToDstItem(attachedItemVNum, attachedSlotPos, overSlotPosGlobal):
						self.wndItem.SetUsableItem(True)
						self.ShowToolTip(overSlotPosGlobal)
						return
				else:
					if attachedItemVNum==player.ITEM_MONEY: # @fixme005
						pass
					elif self.__CanUseSrcItemToDstItem(attachedItemVNum, attachedSlotPos, overSlotPos):
						self.wndItem.SetUsableItem(True)
						self.ShowToolTip(overSlotPos)
						return
				
#				if self.__CanUseSrcItemToDstItem(attachedItemVNum, attachedSlotPos, overSlotPos):
#					self.wndItem.SetUsableItem(TRUE)
#					self.ShowToolTip(overSlotPos)
#					return
				
		if app.HIGHLIGHT_SLOT:
			self.ShowToolTip(overSlotPosGlobal)
		else:
			self.ShowToolTip(overSlotPos)


	def __IsUsableItemToItem(self, srcItemVNum, srcSlotPos):
		"다른 아이템에 사용할 수 있는 아이템인가?"

		if item.IsRefineScroll(srcItemVNum):
			return TRUE
		elif item.IsMetin(srcItemVNum):
			return TRUE
		elif item.IsDetachScroll(srcItemVNum):
			return TRUE
		elif item.IsKey(srcItemVNum):
			return TRUE
		elif (player.GetItemFlags(srcSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			return TRUE
		else:
			if item.GetUseType(srcItemVNum) in self.USE_TYPE_TUPLE:
				return TRUE
			
		return FALSE

	def __CanUseSrcItemToDstItem(self, srcItemVNum, srcSlotPos, dstSlotPos):
		"대상 아이템에 사용할 수 있는가?"
		if srcSlotPos == dstSlotPos:
			return FALSE

		if item.IsRefineScroll(srcItemVNum):
			if player.REFINE_OK == player.CanRefine(srcItemVNum, dstSlotPos):
				return TRUE
		elif item.IsMetin(srcItemVNum):
			if player.ATTACH_METIN_OK == player.CanAttachMetin(srcItemVNum, dstSlotPos):
				return TRUE
		elif item.IsDetachScroll(srcItemVNum):
			if player.DETACH_METIN_OK == player.CanDetach(srcItemVNum, dstSlotPos):
				return TRUE
		elif item.IsKey(srcItemVNum):
			if player.CanUnlock(srcItemVNum, dstSlotPos):
				return TRUE
		elif (player.GetItemFlags(srcSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			return TRUE

		else:
			useType=item.GetUseType(srcItemVNum)

			if "USE_CLEAN_SOCKET" == useType:
				if self.__CanCleanBrokenMetinStone(dstSlotPos):
					return TRUE
			elif "USE_CHANGE_ATTRIBUTE" == useType:
				if self.__CanChangeItemAttrList(dstSlotPos):
					return TRUE
			elif "USE_ADD_ATTRIBUTE" == useType:
				if self.__CanAddItemAttr(dstSlotPos):
					return TRUE
			elif "USE_ADD_ATTRIBUTE2" == useType:
				if self.__CanAddItemAttr(dstSlotPos):
					return TRUE
			elif "USE_ADD_ACCESSORY_SOCKET" == useType:
				if self.__CanAddAccessorySocket(dstSlotPos):
					return TRUE
			elif "USE_PUT_INTO_ACCESSORY_SOCKET" == useType:
				if self.__CanPutAccessorySocket(dstSlotPos, srcItemVNum):
					return TRUE
			elif "USE_SPECIAL" == useType:
				if self.__CanAddSpecialItem(dstSlotPos):
					return TRUE
			elif "USE_PUT_INTO_BELT_SOCKET" == useType:
				dstItemVNum = player.GetItemIndex(dstSlotPos)
				print "USE_PUT_INTO_BELT_SOCKET", srcItemVNum, dstItemVNum

				item.SelectItem(dstItemVNum)
		
				if item.ITEM_TYPE_BELT == item.GetItemType():
					return TRUE
			elif "USE_CHANGE_COSTUME_ATTR" == useType:
				if self.__CanChangeCostumeAttrList(dstSlotPos):
					return TRUE
			elif "USE_RESET_COSTUME_ATTR" == useType:
				if self.__CanResetCostumeAttr(dstSlotPos):
					return TRUE

		return FALSE

	def __CanCleanBrokenMetinStone(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return FALSE

		item.SelectItem(dstItemVNum)
		
		if item.ITEM_TYPE_WEAPON != item.GetItemType():
			return FALSE

		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemMetinSocket(dstSlotPos, i) == constInfo.ERROR_METIN_STONE:
				return TRUE

		return FALSE

	def __CanChangeItemAttrList(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return FALSE

		item.SelectItem(dstItemVNum)
		
		if not item.GetItemType() in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR):	 
			return FALSE

		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemAttribute(dstSlotPos, i) != 0:
				return TRUE

		return FALSE

	def __CanPutAccessorySocket(self, dstSlotPos, mtrlVnum):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return FALSE

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_ARMOR:
			return FALSE

		if not item.GetItemSubType() in (item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR):
			return FALSE

		curCount = player.GetItemMetinSocket(dstSlotPos, 0)
		maxCount = player.GetItemMetinSocket(dstSlotPos, 1)

		if mtrlVnum != constInfo.GET_ACCESSORY_MATERIAL_VNUM(dstItemVNum, item.GetItemSubType()):
			return FALSE
		
		if curCount>=maxCount:
			return FALSE

		return TRUE

	def __CanAddAccessorySocket(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return FALSE

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_ARMOR:
			return FALSE

		if not item.GetItemSubType() in (item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR):
			return FALSE

		curCount = player.GetItemMetinSocket(dstSlotPos, 0)
		maxCount = player.GetItemMetinSocket(dstSlotPos, 1)
		
		ACCESSORY_SOCKET_MAX_SIZE = 3
		if maxCount >= ACCESSORY_SOCKET_MAX_SIZE:
			return FALSE

		return TRUE

	def __CanAddItemAttr(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return FALSE

		item.SelectItem(dstItemVNum)
		
		if not item.GetItemType() in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR):	 
			return FALSE

		attrCount = 0
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemAttribute(dstSlotPos, i) != 0:
				attrCount += 1

		if attrCount<4:
			return TRUE
								
		return FALSE

	def __CanAddSpecialItem(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return FALSE

		item.SelectItem(dstItemVNum)
		
		if item.GetItemType() == item.ITEM_TYPE_COSTUME and item.GetItemSubType() == item.COSTUME_ACCE:
			return TRUE

		return FALSE

	def __CanChangeCostumeAttrList(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_COSTUME:   
			return False

		if not item.GetItemSubType() in (item.COSTUME_TYPE_BODY, item.COSTUME_TYPE_HAIR):
			return False

		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemAttribute(dstSlotPos, i) != 0:
				return True

		return False

	def __CanResetCostumeAttr(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_COSTUME:   
			return False	   

		if not item.GetItemSubType() in (item.COSTUME_TYPE_BODY, item.COSTUME_TYPE_HAIR, item.COSTUME_TYPE_WEAPON):  
			return False

		attrCount = 0
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemAttribute(dstSlotPos, i) != 0:
				attrCount += 1

		if attrCount<2:
			return True

		return False

	def ShowToolTip(self, slotIndex):
		if None != self.tooltipItem:
			self.tooltipItem.SetInventoryItem(slotIndex)

	def OnTop(self):
		if None != self.tooltipItem:
			self.tooltipItem.SetTop()
			
#		if app.WJ_ENABLE_TRADABLE_ICON:
#			map(lambda wnd:wnd.RefreshLockedSlot(), self.bindWnds)
#			self.RefreshMarkSlots()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

	def AppendAcceSlot(self, slotIndex):
		if slotIndex in self.acceSlots:
			return

		self.acceSlots.append(slotIndex)

	def RemoveAcceSlot(self, slotIndex):
		self.acceSlots.remove(slotIndex)

	def UseItemSlot(self, slotIndex):
		curCursorNum = app.GetCursor()
		if app.SELL == curCursorNum:
			return

		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			return

		if app.ENABLE_SPECIAL_INVENTORY:
			if self.inventoryTypeIndex == player.INVENTORY_TYPE_INVENTORY:		
				slotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(slotIndex)
			else:
				slotIndex = self.__SpecialInventoryLocalSlotPosToGlobalSlotPos(slotIndex)
		else:
			slotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(slotIndex)

		if self.wndDragonSoulRefine.IsShow():
			self.wndDragonSoulRefine.AutoSetItem((player.INVENTORY, slotIndex), 1)
			return

		if app.ITEM_CHECKINOUT_UPDATE:
			if self.wndSafeBox.IsShow() and slotIndex < player.EQUIPMENT_SLOT_START:
				net.SendSafeboxCheckinPacket(slotIndex)
				return
			if exchange.isTrading() and slotIndex < player.EQUIPMENT_SLOT_START:
				net.SendExchangeItemAddPacket(player.INVENTORY, slotIndex, -1)
				return

		if self.interface and self.interface.AcceOpen():
			self.interface.AcceAutoSetItem(slotIndex)
			self.RefreshBagSlotWindow()
			return

		self.__UseItem(slotIndex)
		mouseModule.mouseController.DeattachObject()
		self.OverOutItem()

	def __UseItem(self, slotIndex):
		ItemVNum = player.GetItemIndex(slotIndex)
		item.SelectItem(ItemVNum)

		if app.ENABLE_CHEST_DROP:
			if app.IsPressed(app.DIK_LCONTROL):
				if item.HasDropInfo(ItemVNum) and self.interface:
					self.interface.OpenChestDropWindow(ItemVNum)
				return

		if item.IsFlag(item.ITEM_FLAG_CONFIRM_WHEN_USE):
			self.questionDialog = uiCommon.QuestionDialog()
			self.questionDialog.SetText(localeInfo.INVENTORY_REALLY_USE_ITEM)
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnAccept))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnCancel))
			self.questionDialog.Open()
			self.questionDialog.slotIndex = slotIndex
		
			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)
		else:
			self.__SendUseItemPacket(slotIndex)
			#net.SendItemUsePacket(slotIndex)	

	def __UseItemQuestionDialog_OnCancel(self):
		self.OnCloseQuestionDialog()

	def __UseItemQuestionDialog_OnAccept(self):
		self.__SendUseItemPacket(self.questionDialog.slotIndex)
		self.OnCloseQuestionDialog()		

	def __SendUseItemToItemPacket(self, srcSlotPos, dstSlotPos):
		# 개인상점 열고 있는 동안 아이템 사용 방지
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
			return

		net.SendItemUseToItemPacket(srcSlotPos, dstSlotPos)

	def __SendUseItemPacket(self, slotPos):
		# 개인상점 열고 있는 동안 아이템 사용 방지
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
			return

		net.SendItemUsePacket(slotPos)
	
	def __SendMoveItemPacket(self, srcSlotPos, dstSlotPos, srcItemCount):
		# 개인상점 열고 있는 동안 아이템 사용 방지
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_ITEM_FAILURE_PRIVATE_SHOP)
			return

		net.SendItemMovePacket(srcSlotPos, dstSlotPos, long(srcItemCount))
	
	def SetDragonSoulRefineWindow(self, wndDragonSoulRefine):
		self.wndDragonSoulRefine = wndDragonSoulRefine

	if app.ITEM_CHECKINOUT_UPDATE:
		def SetSafeboxWindow(self, wndSafeBox):
			self.wndSafeBox = wndSafeBox

	def OnMoveWindow(self, x, y):
		if self.wndBelt:
			self.wndBelt.AdjustPositionAndSize()

		if app.ENABLE_SPECIAL_INVENTORY and self.wndInventoryTypeTab:
			self.wndInventoryTypeTab.AdjustPosition()

		if self.wndEquipmentTypeTab:
			self.wndEquipmentTypeTab.AdjustPosition()
