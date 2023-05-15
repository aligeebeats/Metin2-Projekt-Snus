import dbg
import constInfo
import app
import ime
import item
import grp
import net
import ui
from uiToolTip import ItemToolTip
import player
import mouseModule
import localeInfo
import wndMgr
import cPickle as pickle
import switchbot

COLOR_INACTIVE = grp.GenerateColor(1.0, 0.0, 0.0, 0.2)
COLOR_ACTIVE   = grp.GenerateColor(1.0, 0.6, 0.1, 0.2)
COLOR_FINISHED = grp.GenerateColor(0.0, 1.0, 0.0, 0.2)
COLOR_SELECTED = grp.GenerateColor(1.0, 1.0, 1.0, 1.0)

SKILL_DAMAGE_BONUS_MAX = 20
NORMAL_HIT_DAMAGE_BONUS_MAX = 50
ADDON_TYPES = {item.APPLY_SKILL_DAMAGE_BONUS : SKILL_DAMAGE_BONUS_MAX, item.APPLY_NORMAL_HIT_DAMAGE_BONUS : NORMAL_HIT_DAMAGE_BONUS_MAX}

BOARD_WIDTH = 400
INPUT_HEIGHT = 27

BORDER_SPACE = 8
BORDER_SPACE_TOP = 30

ITEM_TAB_NAME_BOX_HEIGHT = 20
ITEM_TAB_SPACE = 2
ITEM_TAB_WIDTH = (BOARD_WIDTH - BORDER_SPACE * 2) / switchbot.SLOT_COUNT + 1 - ITEM_TAB_SPACE
	
TOP_PANEL_HEIGHT = (BORDER_SPACE * 2) + (3 * 32) + ITEM_TAB_NAME_BOX_HEIGHT
TOP_PANEL_WIDTH = BOARD_WIDTH - (BORDER_SPACE * 2)

MAIN_PANEL_WIDTH = BOARD_WIDTH - BORDER_SPACE * 2
MAIN_PANEL_HEIGHT = INPUT_HEIGHT * 6 #1 AlternativeRow 5 Attribute

BOT_PANEL_HEIGHT = 40
BOT_PANEL_WIDTH = BOARD_WIDTH - BORDER_SPACE * 2

ATTR_SLOT_COUNT = 5

BOARD_HEIGHT = BORDER_SPACE_TOP + TOP_PANEL_HEIGHT + MAIN_PANEL_HEIGHT + BOT_PANEL_HEIGHT + BORDER_SPACE

def GetAffectString(affectType, affectValue):
	if not affectType:
		return None

	try:
		return ItemToolTip.AFFECT_DICT[affectType](affectValue)
	except TypeError:
		return "UNKNOWN_VALUE[{}] {}".format(affectType, affectValue)
	except KeyError:
		return "UNKNOWN_TYPE[{}] {}".format(affectType, affectValue)


class SwitchbotItemTab(ui.ThinBoardCircle):
	def __init__(self, slot_num):
		ui.ThinBoardCircle.__init__(self)
		
		self.slot_num = slot_num
		
		self.__Initialize()
		self.__LoadItemTab()
		self.__BindEvents()
		
	def __del__(self):
		ui.ThinBoardCircle.__del__(self)
		
	def __Initialize(self):	
		self.tooltipItem = None
		self.onSelectEvent = None
		
		self.statusBar = None
		self.itemSlot = None
		self.selectButton = None
		
		self.lastSelectAlpha = 255
		self.lastSelectAlphaDir = 1
		
	def __LoadItemTab(self):	
		self.SetSize(ITEM_TAB_WIDTH, TOP_PANEL_HEIGHT)
	
		self.statusBar = ui.Bar()
		self.statusBar.SetParent(self)
		self.statusBar.SetSize(ITEM_TAB_WIDTH - 4, TOP_PANEL_HEIGHT - 4)
		self.statusBar.SetPosition(2, 2)
		
		self.selectBox = ui.Box()
		self.selectBox.SetParent(self)
		self.selectBox.SetPosition(1, 1)
		self.selectBox.SetSize(ITEM_TAB_WIDTH - 2, TOP_PANEL_HEIGHT - 2)
		self.selectBox.SetColor(COLOR_SELECTED)
			
		self.selectButton = ui.RadioButton()
		self.selectButton.SetParent(self)
		self.selectButton.SetUpVisual("d:/ymir work/ui/switchbot/btn_small_01.sub")
		self.selectButton.SetOverVisual("d:/ymir work/ui/switchbot/btn_small_02.sub")
		self.selectButton.SetDownVisual("d:/ymir work/ui/switchbot/btn_small_03.sub")
		self.selectButton.SetDisableVisual("d:/ymir work/ui/switchbot/btn_small_01.sub")
		self.selectButton.SetWindowHorizontalAlignCenter()
		self.selectButton.SetPosition(0, 5)
		self.selectButton.SetText(str(self.slot_num+1))
		self.selectButton.Show()
	
		self.itemSlot = ui.GridSlotWindow()
		self.itemSlot.SetParent(self)
		self.itemSlot.ArrangeSlot(0, 1, 3, 32, 32, 0, 0)
		self.itemSlot.SetSlotBaseImage("d:/ymir work/ui/public/slot_base.sub", 1.0, 1.0, 1.0, 1.0)
		self.itemSlot.SetWindowHorizontalAlignCenter()
		self.itemSlot.SetPosition(0, 30)
		self.itemSlot.Show()
	
	def __BindEvents(self):			
		self.itemSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.itemSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		self.itemSlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		self.itemSlot.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		self.itemSlot.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		self.itemSlot.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))	
		
		self.selectButton.SetEvent(ui.__mem_func__(self.__OnSelectButtonClick))
		
	def Destroy(self):
		self.__Initialize()
		
	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem
		
	def OnPlacedItem(self, slot_num):
		pass
		
	def OverOutItem(self):
		self.itemSlot.SetUsableItem(False)
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()			
			
	def OverInItem(self, overSlotPos):
		self.itemSlot.SetUsableItem(True)

		if self.IsEmpty():
			return

		self.tooltipItem.ClearToolTip()

		itemVnum = player.GetItemIndex(player.SWITCHBOT, self.slot_num)
		metinSlot = [player.GetItemMetinSocket(player.SWITCHBOT, self.slot_num, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
		attrSlot = [player.GetItemAttribute(player.SWITCHBOT, self.slot_num, i) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]
		
		self.tooltipItem.AddItemData(itemVnum, metinSlot, attrSlot)	
		self.tooltipItem.ShowToolTip()
		
	def SelectEmptySlot(self, selectedSlotPos):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		if mouseModule.mouseController.isAttached():

			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemCount = mouseModule.mouseController.GetAttachedItemCount()
			attachedItemIndex = mouseModule.mouseController.GetAttachedItemIndex()

			if player.SLOT_TYPE_INVENTORY == attachedSlotType:
				attachedCount = mouseModule.mouseController.GetAttachedItemCount()
				net.SendItemMovePacket(player.INVENTORY, attachedSlotPos, player.SWITCHBOT, self.slot_num, long(attachedItemCount))
			elif player.SLOT_TYPE_SWITCHBOT == attachedSlotType:
				attachedCount = mouseModule.mouseController.GetAttachedItemCount()
				net.SendItemMovePacket(player.SWITCHBOT, attachedSlotPos, player.SWITCHBOT, self.slot_num, long(attachedItemCount))
				
			mouseModule.mouseController.DeattachObject()

			self.OnPlacedItem(self.slot_num)
		
	def SelectItemSlot(self, itemSlotIndex):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return
			
		if switchbot.IsActive(self.slot_num):
			return
		
		if mouseModule.mouseController.isAttached():
			mouseModule.mouseController.DeattachObject()
		else:

			if app.IsPressed(app.DIK_LALT):
				link = player.GetItemLink(player.SWITCHBOT, self.slot_num)
				ime.PasteString(link)
			else:
				itemVnum = player.GetItemIndex(player.SWITCHBOT, self.slot_num)
				itemCount = player.GetItemCount(player.SWITCHBOT, self.slot_num)
				mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_SWITCHBOT, self.slot_num, itemVnum, itemCount)
				
	def UseItemSlot(self, slotIndex):
		curCursorNum = app.GetCursor()
		if app.SELL == curCursorNum:
			return

		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		net.SendItemUsePacket(player.SWITCHBOT, self.slot_num)
		mouseModule.mouseController.DeattachObject()
		self.OverOutItem()
			
	def RefreshItemSlot(self):	
		if self.IsEmpty():
			switchbot.ClearSlot(self.slot_num)
			self.itemSlot.ClearSlot(0)
			self.statusBar.Hide()
		else:			
			itemCount = player.GetItemCount(player.SWITCHBOT, self.slot_num)
			itemVnum = player.GetItemIndex(player.SWITCHBOT, self.slot_num)
			if itemCount == 1:
				itemCount = 0
			
			self.itemSlot.SetItemSlot(0, itemVnum, itemCount)
		
			self.statusBar.Show()
			if self.IsActive():
				self.itemSlot.ActivateSlot(0)
				self.statusBar.SetColor(COLOR_ACTIVE)
			else:
				self.itemSlot.DeactivateSlot(0)
				
				if self.IsFinished():
					self.statusBar.SetColor(COLOR_FINISHED)
				else:
					self.statusBar.SetColor(COLOR_INACTIVE)
		
		self.itemSlot.RefreshSlot()
		
	def IsEmpty(self):
		itemVnum = player.GetItemIndex(player.SWITCHBOT, self.slot_num)	
		return not itemVnum
		
	def IsActive(self):
		return switchbot.IsActive(self.slot_num)
		
	def IsFinished(self):
		return switchbot.IsFinished(self.slot_num)
		
	def GetSlotNum(self):
		return self.slot_num
		
	def SetOnSelectEvent(self, event):
		self.onSelectEvent = event
		
	def __OnSelectButtonClick(self):
		if self.onSelectEvent:
			self.onSelectEvent(self.slot_num)
			
	def SetUp(self):
		if self.selectButton:
			self.selectButton.SetUp()
			self.selectBox.Hide()
			
	def Down(self):
		if self.selectButton:
			self.selectButton.Down()
			
			self.lastSelectAlpha = 255
			self.selectBox.Show()
			
	def OnUpdate(self):
		if self.selectBox.IsShow():
			rate = 3
			
			self.lastSelectAlpha -= (rate * self.lastSelectAlphaDir)
		
			if self.lastSelectAlpha <= 0:
				self.lastSelectAlpha = 0
				self.lastSelectAlphaDir = -1
			elif self.lastSelectAlpha >= 255:
				self.lastSelectAlpha = 255
				self.lastSelectAlphaDir = 1
				
			self.selectBox.SetColor(grp.GenerateColor(1.0, 1.0, 1.0, float(self.lastSelectAlpha) / 255.0))
			
class SwitchbotAttribute(ui.Window):
	VALUE_WIDTH = 50
	
	typeInput = None
	typeTextLine = None
	searchTypeButton = None
	deleteAttributeButton = None
	valueInput = None
	valueEditLine = None	
	
	def __init__(self):
		ui.Window.__init__(self)	
		self.__Initialize()
		
	def __del__(self):
		ui.Window.__del__(self)
		
	def __Initialize(self):
		self.selectedSlot = -1
		self.alternative = 0
		self.index = -1
		
		self.attributeType = 0
		self.attributeValue = 0
		
		self.searchEvent = None
		self.attributeMaxValue = 0	
	
		self.typeInput = None
		self.typeTextLine = None
		self.searchTypeButton = None
		self.deleteAttributeButton = None
		self.valueInput = None
		self.valueEditLine = None
		
	def Create(self, width, index):
		self.index = index
		self.SetSize(width, INPUT_HEIGHT)
		
		self.typeInput = ui.ThinBoardCircle()
		self.typeInput.SetParent(self)
		self.typeInput.SetSize(width - self.VALUE_WIDTH, INPUT_HEIGHT)
		self.typeInput.SetPosition(0, 0)
		self.typeInput.Show()
		
		self.typeTextLine = ui.TextLine()
		self.typeTextLine.SetParent(self.typeInput)
		self.typeTextLine.SetText(localeInfo.SWITCHBOT_EMPTY_ATTR)
		self.typeTextLine.SetVerticalAlignCenter()
		self.typeTextLine.SetWindowVerticalAlignCenter()
		self.typeTextLine.SetHorizontalAlignCenter()
		self.typeTextLine.SetWindowHorizontalAlignCenter()
		self.typeTextLine.SetPosition(-20, -1)
		self.typeTextLine.Show()
			
		self.searchTypeButton = ui.Button()
		self.searchTypeButton.SetParent(self.typeInput)
		self.searchTypeButton.SetUpVisual("d:/ymir work/ui/switchbot/search_attr_01.sub")
		self.searchTypeButton.SetOverVisual("d:/ymir work/ui/switchbot/search_attr_02.sub")
		self.searchTypeButton.SetDownVisual("d:/ymir work/ui/switchbot/search_attr_03.sub")
		self.searchTypeButton.SetDisableVisual("d:/ymir work/ui/switchbot/search_attr_03.sub")
		self.searchTypeButton.SetWindowHorizontalAlignRight()
		self.searchTypeButton.SetWindowVerticalAlignCenter()
		self.searchTypeButton.SetPosition(24 * 2 + 2, 1)
		self.searchTypeButton.SetEvent(ui.__mem_func__(self.__OnClickSearchButton))
		self.searchTypeButton.Show()
		
		self.deleteAttributeButton = ui.Button()
		self.deleteAttributeButton.SetParent(self.typeInput)
		self.deleteAttributeButton.SetUpVisual("d:/ymir work/ui/switchbot/delete_attr_01.sub")
		self.deleteAttributeButton.SetOverVisual("d:/ymir work/ui/switchbot/delete_attr_02.sub")
		self.deleteAttributeButton.SetDownVisual("d:/ymir work/ui/switchbot/delete_attr_03.sub")
		self.deleteAttributeButton.SetDisableVisual("d:/ymir work/ui/switchbot/delete_attr_03.sub")
		self.deleteAttributeButton.SetWindowHorizontalAlignRight()
		self.deleteAttributeButton.SetWindowVerticalAlignCenter()
		self.deleteAttributeButton.SetPosition(24 + 2, 1)
		self.deleteAttributeButton.SetToolTipText(localeInfo.SWITCHBOT_CLEAR_ATTR)
		self.deleteAttributeButton.SetEvent(ui.__mem_func__(self.__OnClickClearButton))
		self.deleteAttributeButton.Show()		
		
		self.valueInput = ui.ThinBoardCircle()
		self.valueInput.SetParent(self)
		self.valueInput.SetSize(self.VALUE_WIDTH, INPUT_HEIGHT)
		self.valueInput.SetPosition(width - self.VALUE_WIDTH, 0)
		self.valueInput.Show()
		
		self.valueEditLine = ui.EditLine()
		self.valueEditLine.SetParent(self.valueInput)
		self.valueEditLine.SetSize(self.VALUE_WIDTH, 18)
		self.valueEditLine.SetMax(4)
		self.valueEditLine.SetNumberMode()
		self.valueEditLine.SetText("0")
		self.valueEditLine.SetPosition(10, 7)
		self.valueEditLine.Show()
		self.valueEditLine.OnIMEUpdate = self.__OnEditLineIMEUpdate
		self.valueEditLine.SetEscapeEvent(ui.__mem_func__(self.__OnEscapeEvent))
		
	def Destroy(self):
		self.__Initialize()
		
	def Hide(self):
		ui.Window.Hide(self)
		
		if self.valueEditLine:
			self.valueEditLine.KillFocus()
			
	def RefreshAttributeRow(self, slot_num, alternative, index):
		self.selectedSlot = slot_num
		self.alternative = alternative
		self.index = index	
	
		if switchbot.IsActive(slot_num):
			self.searchTypeButton.Disable()
			self.deleteAttributeButton.Disable()
			self.valueEditLine.Hide()
		else:
			self.searchTypeButton.Enable()
			self.deleteAttributeButton.Enable()	
			self.valueEditLine.Show()
			
		(type, value) = switchbot.GetAttribute(slot_num, alternative, index)
		if not type:
			self.SetAttribute(localeInfo.SWITCHBOT_EMPTY_ATTR, 0)
			return
			
		affectString = GetAffectString(type, value)
		if not affectString:
			return
			
		self.SetAttribute(affectString, value)

	def __OnEditLineIMEUpdate(self):
		ui.EditLine.OnIMEUpdate(self.valueEditLine)
		
		text = self.valueEditLine.GetText()
		if len(text) > 0 and text.isdigit():
			(curType, curValue) = switchbot.GetAttribute(self.selectedSlot, self.alternative, self.index)
			
			maxValue = 0
			if not ADDON_TYPES.has_key(curType):
				maxValue = switchbot.GetAttributeMaxValue(self.selectedSlot, curType)
				
			value = int(text)
			
			if value == curValue:
				return
			
			if maxValue != 0 and value > maxValue:
				value = maxValue
				self.valueEditLine.SetText(str(value))
				
			switchbot.SetAttribute(self.selectedSlot, self.alternative, self.index, curType, value)
			self.typeTextLine.SetText(GetAffectString(curType, value))
			self.__OnAttributeUpdate()
		
	def SetSearchEvent(self, event):
		self.searchEvent= event
		
	def __OnClickSearchButton(self):
		if self.searchEvent:
			self.searchEvent(self.index)
		
	def SetAttributeUpdateEvent(self, event):
		self.attributeUpdateEvent = event		
		
	def __OnAttributeUpdate(self):
		if self.attributeUpdateEvent:
			self.attributeUpdateEvent(self.index)
			
	def SetEscapeEvent(self, event):
		self.escapeEvent = event			
			
	def __OnEscapeEvent(self):
		if self.escapeEvent:
			self.escapeEvent()
			
	def __OnClickClearButton(self):
		self.SetAttribute(localeInfo.SWITCHBOT_EMPTY_ATTR, 0)
		switchbot.SetAttribute(self.selectedSlot, self.alternative, self.index, 0, 0)
		self.__OnAttributeUpdate()		
			
	def SetAttribute(self, applyname, maxValue):
		self.typeTextLine.SetText(applyname)
		self.valueEditLine.SetText(str(maxValue))
		
	def GetIndex(self):
		return self.index
					
class SwitchbotWindow(ui.BoardWithTitleBar):
	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)

		try:
			self.__Initialize()
			self.__LoadWindow()
			self.__BindEvents()
		except:
			import exception
			exception.Abort("SwitchbotWindow.__init__")
	
	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def __Initialize(self):		
		self.selectedSlot = 0
		self.alternative = 0
		
		self.alternativeButtons = {}
		self.itemTabs = {}
		self.attrDict = {}
		
		self.attributeSearchWindow = None
		self.attrSearchIdx = -1
		
		self.configurationSaveWindow = None
		
		self.topPanel = None
		self.mainPanel = None
		self.botPanel = None
		
		self.startButton = None
		self.stopButton = None

	def __LoadWindow(self):
		self.AddFlag("movable")
		self.SetSize(BOARD_WIDTH, BOARD_HEIGHT)
		self.SetTitleName(localeInfo.SWITCHBOT_TITLE)
		
		self.topPanel = ui.ThinBoardCircle()
		self.topPanel.SetParent(self)
		self.topPanel.SetSize(TOP_PANEL_WIDTH, TOP_PANEL_HEIGHT)
		self.topPanel.SetPosition(BORDER_SPACE, BORDER_SPACE_TOP)
		self.topPanel.Show()
		
		for i in xrange(switchbot.SLOT_COUNT):	
			itemTab = SwitchbotItemTab(i)
			itemTab.SetParent(self.topPanel)
			itemTab.SetPosition((ITEM_TAB_WIDTH + ITEM_TAB_SPACE) * i, 0)
			itemTab.SetOnSelectEvent(ui.__mem_func__(self.SelectSlot))
			itemTab.OnPlacedItem = self.__OnPlacedItem
			itemTab.Show()
			
			self.itemTabs[i] = itemTab
		
		self.mainPanel = ui.ThinBoardCircle()
		self.mainPanel.SetParent(self)
		self.mainPanel.SetSize(MAIN_PANEL_WIDTH, MAIN_PANEL_HEIGHT)
		self.mainPanel.SetPosition(BORDER_SPACE, BORDER_SPACE_TOP + TOP_PANEL_HEIGHT)
		self.mainPanel.Show()
		
		self.alternativePanel = ui.ThinBoardCircle()
		self.alternativePanel.SetParent(self.mainPanel)
		self.alternativePanel.SetSize(MAIN_PANEL_WIDTH, INPUT_HEIGHT)
		self.alternativePanel.SetPosition(0, 0)
		self.alternativePanel.Show()
		
		self.alternativeTextLine = ui.TextLine()
		self.alternativeTextLine.SetParent(self.alternativePanel)
		self.alternativeTextLine.SetPosition(5, -1)
		self.alternativeTextLine.SetVerticalAlignCenter()
		self.alternativeTextLine.SetWindowVerticalAlignCenter()
		self.alternativeTextLine.SetText("Alternativen:")
		self.alternativeTextLine.Show()
		
		for i in xrange(switchbot.ALTERNATIVE_COUNT):
			alternativeButton = ui.RadioButton()
			alternativeButton.SetParent(self.alternativePanel)
			alternativeButton.SetUpVisual("d:/ymir work/ui/switchbot/btn_small_01.sub")
			alternativeButton.SetOverVisual("d:/ymir work/ui/switchbot/btn_small_02.sub")
			alternativeButton.SetDownVisual("d:/ymir work/ui/switchbot/btn_small_03.sub")
			alternativeButton.SetDisableVisual("d:/ymir work/ui/switchbot/btn_small_03.sub")
			alternativeButton.SetWindowVerticalAlignCenter()
			alternativeButton.SetPosition(80 + alternativeButton.GetWidth() * i, 0)
			alternativeButton.SetText(str(i+1))
			alternativeButton.SetEvent(lambda arg = i : self.SelectAlternative(arg))
			alternativeButton.Show()
			
			self.alternativeButtons[i] = alternativeButton
		
		for i in xrange(ATTR_SLOT_COUNT):
			attr = SwitchbotAttribute()
			attr.SetParent(self.mainPanel)
			attr.Create(MAIN_PANEL_WIDTH, i)
			attr.SetPosition(0, INPUT_HEIGHT + (INPUT_HEIGHT * i))
			attr.SetSearchEvent(ui.__mem_func__(self.__OnSearchAttribute))
			attr.SetAttributeUpdateEvent(ui.__mem_func__(self.__OnAttributeUpdate))
			attr.SetEscapeEvent(ui.__mem_func__(self.Close))
			attr.Show()
			
			self.attrDict[i] = attr
			
		self.botPanel = ui.ThinBoardCircle()
		self.botPanel.SetParent(self)
		self.botPanel.SetSize(BOT_PANEL_WIDTH, BOT_PANEL_HEIGHT)
		self.botPanel.SetPosition(BORDER_SPACE, BORDER_SPACE_TOP + TOP_PANEL_HEIGHT + MAIN_PANEL_HEIGHT)
		self.botPanel.Show()			
		
		self.saveButton = ui.Button()
		self.saveButton.SetParent(self.botPanel)
		self.saveButton.SetUpVisual("d:/ymir work/ui/switchbot/save_attr_01.sub")
		self.saveButton.SetOverVisual("d:/ymir work/ui/switchbot/save_attr_02.sub")
		self.saveButton.SetDownVisual("d:/ymir work/ui/switchbot/save_attr_03.sub")
		self.saveButton.SetDisableVisual("d:/ymir work/ui/switchbot/save_attr_03.sub")
		self.saveButton.SetWindowVerticalAlignCenter()
		self.saveButton.SetPosition(BORDER_SPACE, 0)
		self.saveButton.SetToolTipText(localeInfo.SWITCHBOT_SAVE_BUTTON)
		self.saveButton.Show()			
		
		self.startButton = ui.RadioButton()
		self.startButton.SetParent(self.botPanel)
		self.startButton.SetUpVisual("d:/ymir work/ui/switchbot/btn_big_01.sub")
		self.startButton.SetOverVisual("d:/ymir work/ui/switchbot/btn_big_02.sub")
		self.startButton.SetDownVisual("d:/ymir work/ui/switchbot/btn_big_03.sub")
		self.startButton.SetDisableVisual("d:/ymir work/ui/switchbot/btn_big_03.sub")
		self.startButton.SetWindowVerticalAlignCenter()
		self.startButton.SetWindowHorizontalAlignCenter()
		self.startButton.SetPosition(-self.startButton.GetWidth() / 2, 0)
		self.startButton.SetText(localeInfo.SWITCHBOT_START)
		self.startButton.Show()	
		
		self.stopButton = ui.RadioButton()
		self.stopButton.SetParent(self.botPanel)
		self.stopButton.SetUpVisual("d:/ymir work/ui/switchbot/btn_big_01.sub")
		self.stopButton.SetOverVisual("d:/ymir work/ui/switchbot/btn_big_02.sub")
		self.stopButton.SetDownVisual("d:/ymir work/ui/switchbot/btn_big_03.sub")
		self.stopButton.SetDisableVisual("d:/ymir work/ui/switchbot/btn_big_03.sub")
		self.stopButton.SetWindowVerticalAlignCenter()
		self.stopButton.SetWindowHorizontalAlignCenter()
		self.stopButton.SetPosition(self.stopButton.GetWidth() / 2 + 20, 0)
		self.stopButton.SetText(localeInfo.SWITCHBOT_STOP)
		self.stopButton.Show()		
		
		self.SelectSlot(0)
			
	def __BindEvents(self):
		self.SetCloseEvent(ui.__mem_func__(self.Close))
		
		self.startButton.SetEvent(lambda arg = True : self.SetActive(arg))
		self.stopButton.SetEvent(lambda arg = False : self.SetActive(arg))
		self.saveButton.SetEvent(ui.__mem_func__(self.OpenSaveConfiguration))

	def Destroy(self):
		self.__Initialize()

	def Open(self):
		self.Show()
		self.SetTop()
		self.SetCenterPosition()
		self.RefreshSwitchbotWindow()

	def Close(self):
		if self.attributeSearchWindow:
			self.attributeSearchWindow.Close()
			
		if self.configurationSaveWindow:
			self.configurationSaveWindow.Close()
			
		self.Hide()
		return True
		
	def __OnPlacedItem(self, slot_num):
		self.SelectSlot(slot_num)
		
	def SelectSlot(self, selectedSlot):
		self.itemTabs[self.selectedSlot].SetUp()	
		self.selectedSlot = selectedSlot
		self.itemTabs[self.selectedSlot].Down()
		
		self.SelectAlternative(0)
		
	def GetSelectedSlot(self):
		return self.selectedSlot
		
	def SelectAlternative(self, alternative):
		self.alternativeButtons[self.alternative].SetUp()	
		self.alternative = alternative
		self.alternativeButtons[self.alternative].Down()
		
		self.RefreshSwitchbotWindow()
		
	def SetActive(self, active):
		if self.selectedSlot >= switchbot.SLOT_COUNT:
			return
			
		if active and self.IsActiveSlot():
			return
			
		if not active and not self.IsActiveSlot():
			return
				
		if active:
			switchbot.Start(self.selectedSlot)
		else:
			switchbot.Stop(self.selectedSlot)
			
	def OpenSaveConfiguration(self):
		if not self.configurationSaveWindow:
			self.configurationSaveWindow = ConfigurationWindow(self)
				
		if self.configurationSaveWindow.IsShow():
			self.configurationSaveWindow.Close()
		else:
			self.configurationSaveWindow.Open()
			
	def SetItemToolTip(self, tooltipItem):
		for i in xrange(switchbot.SLOT_COUNT):
			self.itemTabs[i].SetItemToolTip(tooltipItem)
			
	def __AnyAttributeConfigured(self):
		for i in xrange(switchbot.ALTERNATIVE_COUNT):
			for j in xrange(ATTR_SLOT_COUNT):
				(type, value) = switchbot.GetAttribute(self.selectedSlot, i, j)
				if type and value:
					return True
				
		return False
		
	def RefreshSwitchbotWindow(self):
		self.__RefreshItemSlots()
		self.__RefreshAlternatives()
		self.__RefreshAttributeRows()	
		self.__RefreshButtons()
		
	def RefreshSwitchbotItem(self, slot):
		itemTab = self.itemTabs.get(slot, None)
		if not itemTab:
			return
			
		itemTab.RefreshItemSlot()
		
	def __RefreshItemSlots(self):
		for i in xrange(switchbot.SLOT_COUNT):
			itemTab = self.itemTabs.get(i, None)
			if not itemTab:
				continue
				
			itemTab.RefreshItemSlot()
			
	def __RefreshAlternatives(self):
		if self.IsEmptySlot():
			self.alternativePanel.Hide()
		else:
			self.alternativePanel.Show()
			
	def __RefreshAttributeRows(self):
		for i in xrange(ATTR_SLOT_COUNT):
			row = self.attrDict.get(i, None)
			if not row:
				continue
				
			(applyType, applyValue) = player.GetItemAttribute(player.SWITCHBOT, self.selectedSlot, i)
			if applyType and applyValue:	
				row.Show()
			else:
				row.Hide()
			
			row.RefreshAttributeRow(self.selectedSlot, self.alternative, i)		
			
	def __RefreshButtons(self):
		if self.IsEmptySlot() or not self.__AnyAttributeConfigured():
			self.startButton.Disable()
			self.stopButton.Disable()
			return
			
		self.startButton.Enable()
		self.stopButton.Enable()
		
		if self.IsActiveSlot():
			self.stopButton.SetUp()
			self.startButton.Down()
		else:
			self.stopButton.Down()
			self.startButton.SetUp()
			
	def __ClearAttributes(self):
		
	
		for index, row in self.attrDict.iteritems():
			for alternative in xrange(switchbot.ALTERNATIVE_COUNT):
				row.RefreshAttributeRow(self.selectedSlot, alternative, index)
		
	def __OnAttributeUpdate(self, attrIdx):
		self.__RefreshButtons()

	def __OnSearchAttribute(self, attrIdx):
		if self.attributeSearchWindow:
			self.attributeSearchWindow.Close()
			del self.attributeSearchWindow
			
		self.attrSearchIdx = attrIdx
			
		wnd = SearchListWindow()
		width = 260
		height = 260
		wnd.MakeWindow(width, height)
		wnd.SetTitleName(localeInfo.SWITCHBOT_SELECT_ATTR)
		wnd.SetAcceptArgsType("KEY_VALUE")

		(mouseX, mouseY) = wndMgr.GetMousePosition()

		if mouseX + width/2 > wndMgr.GetScreenWidth():
			xPos = wndMgr.GetScreenWidth() - width
		elif mouseX - width/2 < 0:
			xPos = 0
		else:
			xPos = mouseX - width/2
			
		if mouseY + height/2 > wndMgr.GetScreenHeight():
			yPos = wndMgr.GetScreenHeight() - height
		elif mouseY - height/2 < 0:
			yPos = 0
		else:
			yPos = mouseY - height/2

		wnd.SetPosition(xPos, yPos)	
				
		firstAttrType = player.GetItemAttribute(player.SWITCHBOT, self.selectedSlot, 0)[0]
		if firstAttrType in ADDON_TYPES.keys():
			for addonType, maxValue in ADDON_TYPES.iteritems():
				affectString = GetAffectString(addonType, maxValue)
				if not affectString:
					continue
				
				wnd.AppendItem(addonType, affectString)
		
		attributes = switchbot.GetAttributesForSet(self.selectedSlot)
		
		selectedAttributes = [switchbot.GetAttribute(self.selectedSlot, self.alternative, attrIndex)[0] for attrIndex in xrange(ATTR_SLOT_COUNT)]
		
		for (type, value) in attributes:
			if type in selectedAttributes:
				continue
				
			affectString = GetAffectString(type, value)
			if not affectString:
				continue
				
			wnd.AppendItem(type, affectString)
		
		wnd.SetAcceptEvent(ui.__mem_func__(self.__OnAcceptSearch))
		wnd.SetCancelEvent(ui.__mem_func__(self.__OnCancelSearch))
		wnd.SetTop()
		wnd.Open()

		self.attributeSearchWindow = wnd
		
	def __OnAcceptSearch(self, key, value):
		if self.attrSearchIdx < 0 or self.attrSearchIdx >= ATTR_SLOT_COUNT:
			return
			
		attrRow = self.attrDict.get(self.attrSearchIdx, None)
		if not attrRow:
			return
			
		maxValue = ADDON_TYPES.get(key, 0)
		if not maxValue:
			maxValue = switchbot.GetAttributeMaxValue(self.selectedSlot, key)
			
		attrRow.SetAttribute(value, maxValue)
		switchbot.SetAttribute(self.selectedSlot, self.alternative, self.attrSearchIdx, key, maxValue)
		
		self.__RefreshButtons()
		
	def SetSavedAttribute(self, slot, alternative, attrIndex, attrType, attrValue):
		if attrIndex < 0 or attrIndex >= ATTR_SLOT_COUNT:
			return
			
		attrRow = self.attrDict.get(attrIndex, None)
		if not attrRow:
			return
			
		if slot >= switchbot.SLOT_COUNT:
			return
			
		if switchbot.IsActive(slot):
			return
			
		var = switchbot.GetAttributesForSet(slot)
		attributes = [var[i][0] for i in xrange(len(var))]
		
		firstAttrType = player.GetItemAttribute(player.SWITCHBOT, slot, 0)[0]
		if firstAttrType in ADDON_TYPES.keys():
			for addon in ADDON_TYPES.keys():
				attributes.append(addon)
		
		if not attrType in attributes:
			switchbot.SetAttribute(slot, alternative, attrIndex, 0, 0)
		else:
			switchbot.SetAttribute(slot, alternative, attrIndex, attrType, attrValue)
			
		self.RefreshSwitchbotWindow()
		
	def IsEmptySlot(self):
		slot = self.itemTabs.get(self.selectedSlot, None)
		if not slot:
			return True
			
		return slot.IsEmpty()
		
	def IsActiveSlot(self):
		slot = self.itemTabs.get(self.selectedSlot, None)
		if not slot:
			return False
			
		return slot.IsActive()
		
	def GetSlotAttributeCount(self):
		slot = self.itemTabs.get(self.selectedSlot, None)
		if not slot:
			return 0
			
		return slot.GetAttributeCount()
		
	def OnMoveWindow(self, x, y):
		if self.configurationSaveWindow:
			self.configurationSaveWindow.AdjustPosition()
		
	def __OnCancelSearch(self):
		pass
		
	def OnPressEscapeKey(self):
		self.Close()
		return True

class SearchListWindow(ui.BoardWithTitleBar):
	class SearchListItem(ui.Window):
		LIST_ITEM_HEIGHT = 12
	
		def __init__(self, key, value):
			ui.Window.__init__(self)
			
			self.key = key
			self.value = value
			self.isDown = False
			self.onClickEvent = None	

			self.textLine = ui.TextLine()
			self.textLine.AddFlag("not_pick")
			self.textLine.SetParent(self)
			self.textLine.SetVerticalAlignCenter()
			self.textLine.SetWindowVerticalAlignCenter()
			self.textLine.SetPosition(10, -1)
			self.textLine.SetText(str(value))
			self.textLine.UpdateRect()
			self.textLine.Show()
				
		def __del__(self):
			ui.Window.__del__(self)
			
			self.key = None
			self.value = None
			
			self.onClickEvent = None
			
			self.textLine = 0
			
		def OnMouseLeftButtonUp(self):
			if not self.isDown:
				self.__OnClick()
				self.Down()
				
		def OnRender(self):
			if self.IsIn() or self.isDown:
				self.RenderHoverOrSelected()
				
		def RenderHoverOrSelected(self):
			x, y = self.GetGlobalPosition()
			grp.SetColor(grp.GenerateColor(0.0, 0.0, 0.7, 0.7))
			grp.RenderBar(x, y, self.GetWidth(), self.GetHeight())
				
		def SetUp(self):
			self.isDown = False
			
		def Down(self):
			self.isDown = True
			
		def IsDown(self):
			return self.isDown
			
		def GetKey(self):
			return self.key
			
		def GetValue(self):
			return self.value
			
		def GetText(self):	
			return self.textLine.GetText()
			
		def MatchText(self, text):
			return text in self.GetText()
			
		def SetOnClickEvent(self, event):
			self.onClickEvent = ui.__mem_func__(event)
			
		def __OnClick(self):
			if self.onClickEvent:
				self.onClickEvent(self.key, self.value)
			
	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.AddFlag("attach")
		self.AddFlag("movable")
		
		self.__Initialize()
		
	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)
		self.__Initialize()
		
	def __Initialize(self):
		self.listItems = {}
		self.acceptEvent = None
		self.acceptArgsType = "VALUE"
		self.cancelEvent = None
		
	def MakeWindow(self, width, height, maxInput = 50):
		self.SetSize(width, height)
		
		self.searchInput = ui.ThinBoardCircle()
		self.searchInput.SetParent(self)
		self.searchInput.SetSize(width - (BORDER_SPACE*2), INPUT_HEIGHT)
		self.searchInput.SetPosition(BORDER_SPACE, BORDER_SPACE_TOP)
		self.searchInput.Show()
		
		self.searchEditLine = ui.EditLine()
		self.searchEditLine.SetParent(self.searchInput)
		self.searchEditLine.SetPosition(10, 8)
		self.searchEditLine.SetSize(self.searchInput.GetWidth() - 20, self.searchInput.GetHeight() - 16)
		self.searchEditLine.SetMax(maxInput)
		self.searchEditLine.SetReturnEvent(ui.__mem_func__(self.__OnAccept))
		self.searchEditLine.SetEscapeEvent(ui.__mem_func__(self.Close))
		self.searchEditLine.OnIMEUpdate = self.__OnSearchIMEUpdate
		self.searchEditLine.Show()
		
		containerHeight = height - (BORDER_SPACE_TOP + INPUT_HEIGHT + 30)
		
		self.scrollBar = ui.ScrollBar()
		self.scrollBar.SetParent(self)
		self.scrollBar.SetWindowHorizontalAlignRight()
		self.scrollBar.SetPosition(23, BORDER_SPACE_TOP + INPUT_HEIGHT + 2)
		self.scrollBar.SetScrollBarSize(containerHeight - 4)	
		
		self.container = ui.ThinBoardCircle()
		self.container.SetParent(self)
		self.container.SetPosition(BORDER_SPACE, BORDER_SPACE_TOP + INPUT_HEIGHT)
		self.container.SetSize(width - (BORDER_SPACE*2), containerHeight)
		self.container.Show()
		
		listBoxHeight = self.container.GetHeight()
		listBoxWidth = self.container.GetWidth()
		
		self.listBox = ui.ListBoxEx()
		self.listBox.SetParent(self.container)
		self.listBox.SetPosition(2, 2)
		self.listBox.SetSize(listBoxWidth, listBoxHeight)
		self.listBox.SetItemStep(self.SearchListItem.LIST_ITEM_HEIGHT)
		self.listBox.SetViewItemCount(self.__GetViewItemCount())
		self.listBox.SetItemSize(listBoxWidth, self.SearchListItem.LIST_ITEM_HEIGHT)
		self.listBox.SetScrollBar(self.scrollBar)
		self.listBox.Show()
		
		buttonWidth = width / 2 - 20
		
		self.acceptButton = ui.Button()
		self.acceptButton.SetParent(self)
		self.acceptButton.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
		self.acceptButton.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
		self.acceptButton.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
		self.acceptButton.SetDisableVisual("d:/ymir work/ui/public/large_button_03.sub")
		self.acceptButton.SetWindowVerticalAlignBottom()
		self.acceptButton.SetWindowHorizontalAlignCenter()
		self.acceptButton.SetPosition(-buttonWidth / 2 - 5, 29)
		self.acceptButton.SetText(localeInfo.UI_ACCEPT)
		self.acceptButton.SetEvent(ui.__mem_func__(self.__OnAccept))
		self.acceptButton.Show()
		
		self.cancelButton = ui.Button()
		self.cancelButton.SetParent(self)
		self.cancelButton.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
		self.cancelButton.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
		self.cancelButton.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
		self.cancelButton.SetDisableVisual("d:/ymir work/ui/public/large_button_03.sub")
		self.cancelButton.SetWindowVerticalAlignBottom()
		self.cancelButton.SetWindowHorizontalAlignCenter()
		self.cancelButton.SetPosition(buttonWidth / 2 - 5, 29)
		self.cancelButton.SetText(localeInfo.UI_CANCEL)
		self.cancelButton.SetEvent(ui.__mem_func__(self.__OnCancel))
		self.cancelButton.Show()
		
		self.__AdjustScrollbar()
		
	def __OnSearchIMEUpdate(self):
		ui.EditLine.OnIMEUpdate(self.searchEditLine)
		self.__RefreshListBox()
		
	def __GetViewItemCount(self):
		return self.container.GetHeight() / self.SearchListItem.LIST_ITEM_HEIGHT
		
	def __AdjustScrollbar(self):
		if len(self.listItems) > self.__GetViewItemCount():
			listBoxWidth = self.GetWidth() - (BORDER_SPACE*2) - 20
			self.container.SetSize(listBoxWidth, self.container.GetHeight())
			self.listBox.SetSize(listBoxWidth - 4, self.listBox.GetHeight() - 4)
			self.listBox.SetItemSize(listBoxWidth - 4, self.SearchListItem.LIST_ITEM_HEIGHT)
			attachmentCount = len(self.listItems)
			scrollBarHeight = min(float(self.__GetViewItemCount()) / float(attachmentCount), 1.0)
			self.scrollBar.SetMiddleBarSize(scrollBarHeight)
			self.scrollBar.Show()
		else:
			listBoxWidth = self.GetWidth() - (BORDER_SPACE*2)
			self.container.SetSize(listBoxWidth, self.container.GetHeight())
			self.listBox.SetSize(listBoxWidth - 4, self.listBox.GetHeight() - 4)
			self.listBox.SetItemSize(listBoxWidth - 4, self.SearchListItem.LIST_ITEM_HEIGHT)
			self.scrollBar.Hide()
		
	def RemoveAllItems(self):
		self.listItems = {}
		self.listBox.RemoveAllItems()		
		
	def AppendItem(self, key, value = ""):
		if self.listItems.has_key(key):
			return
			
		if not value:
			value = key
			
		self.listItems[key] = { "value" : value, "item" : None, }
		self.__RefreshListBox()
		
	def __RefreshListBox(self):
		self.listBox.RemoveAllItems()
		
		self.__AdjustScrollbar()
		
		for key, dict in self.listItems.items():
			listItem = dict["item"]
			if not self.searchEditLine.GetText() or self.searchEditLine.GetText().lower() in dict["value"].lower():
				listItem = self.SearchListItem(key, dict["value"])
				listItem.SetOnClickEvent(self.__OnClickListItem)
				self.listBox.AppendItem(listItem)
			else:
				del listItem
			
	def GetSelectedItem(self):
		for listItem in self.listBox.GetItems():
			if listItem.IsDown():
				return listItem
				
		return None
		
	def SetCancelEvent(self, event):
		self.cancelEvent = event
		
	def SetAcceptEvent(self, event):
		self.acceptEvent = event
		
	def SetAcceptArgsType(self, type):
		self.acceptArgsType = type
		
	def __OnClickListItem(self, key, value):
		curItem = self.GetSelectedItem()
		if curItem:
			curItem.SetUp()
				
	def __OnCancel(self):
		if self.cancelEvent:
			self.cancelEvent()
			
		self.Close()
			
	def __OnAccept(self):
		selItem = self.GetSelectedItem()
		if not selItem:
			if len(self.listBox.GetItems()) == 1:
				selItem = self.listBox.GetItems()[0]
			else:
				return
	
		if self.acceptEvent:
			if self.acceptArgsType == "VALUES":
				self.acceptEvent(selItem.GetValue())
			elif self.acceptArgsType == "KEY":
				self.acceptEvent(selItem.GetKey())
			elif self.acceptArgsType == "KEY_VALUE":
				self.acceptEvent(selItem.GetKey(), selItem.GetValue())
			
		self.Close()
		
	def OnPressEscapeKey(self):
		self.Close()
		return True
		
	def Open(self):
		self.Show()
		self.searchEditLine.SetFocus()
		
	def Close(self):
		self.searchEditLine.SetText("")
		self.searchEditLine.KillFocus()
		self.Hide()		

class ConfigurationWindow(ui.BoardWithTitleBar):
	BOARD_WIDTH = 250

	class ConfigurationLine(ui.Window):
		LIST_ITEM_HEIGHT = 26
	
		id = 0
		name = ""
	
		loadEvent = None
		deleteEvent = None
	
		def __init__(self, width):
			ui.Window.__init__(self)
			
			self.SetSize(width, 26)
			
			self.nameTextLine = ui.TextLine()
			self.nameTextLine.SetParent(self)
			self.nameTextLine.SetVerticalAlignCenter()
			self.nameTextLine.SetWindowVerticalAlignCenter()
			self.nameTextLine.SetPosition(5, -1)
			self.nameTextLine.Show()
				
			self.loadButton = ui.Button()
			self.loadButton.SetParent(self)
			self.loadButton.SetUpVisual("d:/ymir work/ui/switchbot/load_attr_01.sub")
			self.loadButton.SetOverVisual("d:/ymir work/ui/switchbot/load_attr_02.sub")
			self.loadButton.SetDownVisual("d:/ymir work/ui/switchbot/load_attr_03.sub")
			self.loadButton.SetDisableVisual("d:/ymir work/ui/switchbot/load_attr_03.sub")
			self.loadButton.SetWindowHorizontalAlignRight()
			self.loadButton.SetWindowVerticalAlignCenter()
			self.loadButton.SetPosition(24 * 2 + 2, 1)
			self.loadButton.SetToolTipText(localeInfo.SWITCHBOT_LOAD_CONFIGURATION)
			self.loadButton.SetEvent(ui.__mem_func__(self.__OnClickLoadButton))
			self.loadButton.Show()
			
			self.deleteButton = ui.Button()
			self.deleteButton.SetParent(self)
			self.deleteButton.SetUpVisual("d:/ymir work/ui/switchbot/delete_attr_01.sub")
			self.deleteButton.SetOverVisual("d:/ymir work/ui/switchbot/delete_attr_02.sub")
			self.deleteButton.SetDownVisual("d:/ymir work/ui/switchbot/delete_attr_03.sub")
			self.deleteButton.SetDisableVisual("d:/ymir work/ui/switchbot/delete_attr_03.sub")
			self.deleteButton.SetWindowHorizontalAlignRight()
			self.deleteButton.SetWindowVerticalAlignCenter()
			self.deleteButton.SetPosition(24 + 2, 1)
			self.deleteButton.SetToolTipText(localeInfo.SWITCHBOT_CLEAR_CONFIGURATION)
			self.deleteButton.SetEvent(ui.__mem_func__(self.__OnClickDeleteButton))
			self.deleteButton.Show()
			
		def __del__(self):
			ui.Window.__del__(self)
			
		def Load(self, id, name):
			self.id = id
			self.name = name
			self.nameTextLine.SetText(name)
			
		def SetLoadEvent(self, event):
			self.loadEvent = event
			
		def SetDeleteEvent(self, event):
			self.deleteEvent = event
			
		def __OnClickLoadButton(self):
			if self.loadEvent:
				self.loadEvent(self.id)
			
		def __OnClickDeleteButton(self):
			if self.deleteEvent:
				self.deleteEvent(self.id)
	
	wndSwitchbot = None
	
	nameInput = None
	valueEditLine = None
	
	dict = {}
	
	listItems = {}
	
	def __init__(self, wndSwitchbot=None):
		ui.BoardWithTitleBar.__init__(self)
		
		if not wndSwitchbot:
			import exception
			exception.Abort("ConfigurationWindow.__init__ Cannot initialize without binding to Switchbot window.")
			
		self.wndSwitchbot = wndSwitchbot
		
		self.__LoadWindow()
		
	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)
		
	def Destroy(self):
		self.saveButton = None
		self.nameInput = None
		self.valueEditLine = None
		self.scrollBar = None
		self.container = None
		self.listBox = None
		
		self.listItems = {}
		
	def __LoadWindow(self):
		self.SetSize(self.BOARD_WIDTH, BOARD_HEIGHT)
		self.SetTitleName(localeInfo.SWITCHBOT_CONFIGURATION_TITLE)
		self.SetCloseEvent(ui.__mem_func__(self.Close))
		self.AddFlag("attach")
		# self.AddFlag("movable")
	
		self.saveButton = ui.Button()
		self.saveButton.SetParent(self)
		self.saveButton.SetUpVisual("d:/ymir work/ui/switchbot/btn_small_01.sub")
		self.saveButton.SetOverVisual("d:/ymir work/ui/switchbot/btn_small_02.sub")
		self.saveButton.SetDownVisual("d:/ymir work/ui/switchbot/btn_small_03.sub")
		self.saveButton.SetDisableVisual("d:/ymir work/ui/switchbot/btn_small_01.sub")
		self.saveButton.SetWindowHorizontalAlignRight()
		self.saveButton.SetPosition(self.saveButton.GetWidth() + BORDER_SPACE, BORDER_SPACE_TOP + 3)
		self.saveButton.SetText(localeInfo.SWITCHBOT_SAVE)
		self.saveButton.SetEvent(ui.__mem_func__(self.__OnSave))
		self.saveButton.Show()
	
		self.nameInput = ui.ThinBoardCircle()
		self.nameInput.SetParent(self)
		self.nameInput.SetSize(self.BOARD_WIDTH - (BORDER_SPACE * 2) - self.saveButton.GetWidth() - 3, INPUT_HEIGHT)
		self.nameInput.SetPosition(BORDER_SPACE, BORDER_SPACE_TOP)
		self.nameInput.Show()
		
		self.nameEditLine = ui.EditLine()
		self.nameEditLine.SetParent(self.nameInput)
		self.nameEditLine.SetSize(self.nameInput.GetWidth() - 20, 18)
		self.nameEditLine.SetMax(32)
		self.nameEditLine.SetPosition(10, 7)
		self.nameEditLine.Show()
		self.nameEditLine.SetEscapeEvent(ui.__mem_func__(self.__OnEscapeEvent))
		
		height = BOARD_HEIGHT - BORDER_SPACE_TOP - INPUT_HEIGHT - BORDER_SPACE
		
		self.scrollBar = ui.ScrollBar()
		self.scrollBar.SetParent(self)
		self.scrollBar.SetWindowHorizontalAlignRight()
		self.scrollBar.SetPosition(23, BORDER_SPACE_TOP + INPUT_HEIGHT + 2)
		self.scrollBar.SetScrollBarSize(height - 4)	
		
		self.container = ui.ThinBoardCircle()
		self.container.SetParent(self)
		self.container.SetPosition(BORDER_SPACE, BORDER_SPACE_TOP + INPUT_HEIGHT)
		self.container.SetSize(BOARD_WIDTH - (BORDER_SPACE*2), height)
		self.container.Show()
		
		listBoxHeight = self.container.GetHeight()
		listBoxWidth = self.container.GetWidth()
		
		self.listBox = ui.ListBoxEx()
		self.listBox.SetParent(self.container)
		self.listBox.SetPosition(2, 2)
		self.listBox.SetSize(listBoxWidth, listBoxHeight)
		self.listBox.SetItemStep(self.ConfigurationLine.LIST_ITEM_HEIGHT + 1)
		self.listBox.SetViewItemCount(self.__GetViewItemCount())
		self.listBox.SetItemSize(listBoxWidth, self.ConfigurationLine.LIST_ITEM_HEIGHT)
		self.listBox.SetScrollBar(self.scrollBar)
		self.listBox.Show()
		
		self.__AdjustScrollbar()
		
	def LoadConfigurations(self):
		try:
			with open('switchbot.pkl', 'rb') as f:
				self.dict = pickle.load(f)
		except IOError:
			self.dict = {}	
		
		self.listBox.RemoveAllItems()
			
		lineWidth = self.listBox.GetWidth()
		if len(self.dict) > self.__GetViewItemCount():	
			lineWidth -= 20
			
		self.__AdjustScrollbar()
			
		for key in self.dict.iterkeys():
			config = self.dict.get(key, None)
			if not config:
				continue
				
			line = self.ConfigurationLine(lineWidth)
			line.Load(key, config["name"])
			line.SetLoadEvent(ui.__mem_func__(self.__OnApplyConfiguration))
			line.SetDeleteEvent(ui.__mem_func__(self.__OnDeleteConfiguration))
			self.listBox.AppendItem(line)
			
			self.listItems[key] = line
			
	def SaveConfigurations(self):
		with open('switchbot.pkl', 'wb') as f:
			pickle.dump(self.dict, f)
			
	def __OnApplyConfiguration(self, key):
		config = self.dict.get(key, None)
		if not config:
			return
			
		if not self.wndSwitchbot:
			return
			
		selectedSlot = self.wndSwitchbot.GetSelectedSlot()			
		for alternativeIdx in xrange(len(config["alternatives"])):
			alternative = config["alternatives"][alternativeIdx]
			for attrIdx in xrange(len(alternative)):
				(attrType, attrValue) = alternative[attrIdx]
				self.wndSwitchbot.SetSavedAttribute(selectedSlot, alternativeIdx, attrIdx, attrType, attrValue)
		
	def __OnDeleteConfiguration(self, key):
		config = self.dict.get(key, None)
		if not config:
			return
			
		del self.listItems[key]
		del self.dict[key]
		self.SaveConfigurations()
		self.LoadConfigurations()
			
	def Open(self):
		self.Show()
		self.LoadConfigurations()
		self.AdjustPosition()
	
	def Close(self):
		self.Hide()
		
	def AdjustPosition(self):
		if not self.wndSwitchbot:
			return 
			
		(x, y) = self.wndSwitchbot.GetGlobalPosition()
		self.SetPosition(x - self.GetWidth(), y)
		
	def __GetViewItemCount(self):
		return self.container.GetHeight() / (self.ConfigurationLine.LIST_ITEM_HEIGHT + 1)
		
	def __AdjustScrollbar(self):
		if len(self.dict) > self.__GetViewItemCount():
			listBoxWidth = self.GetWidth() - (BORDER_SPACE*2) - 20
			self.container.SetSize(listBoxWidth, self.container.GetHeight())
			self.listBox.SetSize(listBoxWidth - 4, self.listBox.GetHeight() - 4)
			self.listBox.SetItemSize(listBoxWidth - 4, self.ConfigurationLine.LIST_ITEM_HEIGHT)
			attachmentCount = len(self.dict)
			scrollBarHeight = min(float(self.__GetViewItemCount()) / float(attachmentCount), 1.0)
			self.scrollBar.SetMiddleBarSize(scrollBarHeight)
			self.scrollBar.Show()
		else:
			listBoxWidth = self.GetWidth() - (BORDER_SPACE*2)
			self.container.SetSize(listBoxWidth, self.container.GetHeight())
			self.listBox.SetSize(listBoxWidth - 4, self.listBox.GetHeight() - 4)
			self.listBox.SetItemSize(listBoxWidth - 4, self.ConfigurationLine.LIST_ITEM_HEIGHT)
			self.scrollBar.Hide()
		
	def __OnEscapeEvent(self):
		self.Close()
		
	def __GetNewKey(self):
		key = 0
		while self.dict.has_key(key):
			key += 1
			
		return key
		
	def __BuildDict(self, slot, name):	
		key = self.__GetNewKey()
		alternatives = []
		for alternative in xrange(switchbot.ALTERNATIVE_COUNT):
			attributes = [switchbot.GetAttribute(slot, alternative, i) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]
			alternatives.append(attributes)
		
		new_dict = {
			"name" : name,
			"alternatives" : alternatives,
		}
		
		return (key, new_dict)
		
	def __OnSave(self):
		if not self.wndSwitchbot:
			return
			
		selectedSlot = self.wndSwitchbot.GetSelectedSlot()
		name = self.nameEditLine.GetText()
		
		if not name:
			return
		
		(key, new_dict) = self.__BuildDict(selectedSlot, name)
		self.dict[key] = new_dict
		
		self.nameEditLine.SetText("")
		
		self.SaveConfigurations()
		self.LoadConfigurations()
		
