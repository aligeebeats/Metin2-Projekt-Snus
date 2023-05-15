import localeInfo
import uiCommon
import uiRefine
import constInfo
import exception
import item
import player
import net
import snd
import grp
import app
import uiScriptLocale
import ui
import mouseModule
import dbg

class AcceWindow(ui.ScriptWindow):

	isLoaded = 0
	tooltip = None
	wndItem = None
	popupDialog = None
	questionDialog = None

	highlightedSlotList = []
	windowType = 0
	xAcceWindowStart = 0
	yAcceWindowStart = 0
	Cost = None

	interface = None
	
	def __init__(self, interface):
		ui.ScriptWindow.__init__(self)
		self.interface = interface
		

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		
	def __LoadWindow(self, type):
		if type == 0:
			try:
				pyScrLoader = ui.PythonScriptLoader()
				pyScrLoader.LoadScriptFile(self, "uiscript/Acce_AbsorbWindow.py")
			except:
				import exception
				exception.Abort("AcceWindow.LoadWindow.LoadObject")
		else:
			try:
				pyScrLoader = ui.PythonScriptLoader()
				pyScrLoader.LoadScriptFile(self, "uiscript/Acce_CombineWindow.py")
			except:
				import exception
				exception.Abort("AcceWindow.LoadWindow.LoadObject")

		try:
			self.wndItem = self.GetChild("AcceSlot")
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
			if type == 1:
				self.Cost = self.GetChild("Cost")
			self.GetChild("AcceptButton").SetEvent(self.Accept)
			self.GetChild("CancelButton").SetEvent(self.OnAcceCloseEvent)
			self.wndItem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
			self.wndItem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
			self.wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			self.wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			self.wndItem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
			self.wndItem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
			if self.Cost:
				self.Cost.SetText(localeInfo.ACCE_ABSORB_COST % (0))
		except:
			import exception
			exception.Abort("AcceWindow.LoadWindow.BindObject")

	def Destroy(self):
		self.tooltip = None
		self.wndItem = None
		self.popupDialog = None
		self.questionDialog = None

		self.highlightedSlotList = []
		self.windowType = 0
		self.xAcceWindowStart = 0
		self.yAcceWindowStart = 0
		self.Cost = None

		self.interface = None

	def Accept(self):
		net.SendAcceRefineAccept(self.windowType)

	def Open(self, type):
		self.__LoadWindow(type)
		self.windowType = type
		(x, y, z) = player.GetMainCharacterPosition()
		self.xAcceWindowStart = x
		self.yAcceWindowStart = y

		self.SetTop()
		self.SetCenterPosition()
		self.Show()

	def Close(self):
		net.SendAcceRefineCanCancel()
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()
		self.Hide()
		
	def SetItemToolTip(self, itemTooltip):
		self.tooltipItem = itemTooltip
		
	def __ShowToolTip(self, slotIndex):
		if self.tooltipItem:
			self.tooltipItem.SetAcceWindowItem(slotIndex)

	def OverInItem(self, slotIndex):
		slotIndex = slotIndex
		self.wndItem.SetUsableItem(False)
		self.__ShowToolTip(slotIndex)
		
	def OverOutItem(self):
		self.wndItem.SetUsableItem(False)
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OnAcceCloseEvent(self):
		self.Close()

	def OnCloseQuestionDialog(self):
		if not self.questionDialog:
			return
		
		self.questionDialog.Close()
		self.questionDialog = None
		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	def AutoSetItem(self, slotIndex):
		
		if not player.GetAcceItemID(0):
			targetSlot = 0
		elif not player.GetAcceItemID(1):
			targetSlot = 1
		else:
			return

		if self.windowType == 1 and player.IsAcceWindowEmpty():
			self.popupDialog = uiCommon.PopupDialog()
			self.popupDialog.SetText(localeInfo.ACCE_DEL_SERVEITEM)
			self.popupDialog.Show()
		elif self.windowType == 0 and player.IsAcceWindowEmpty():
			self.popupDialog = uiCommon.PopupDialog()
			self.popupDialog.SetText(localeInfo.ACCE_DEL_ABSORDITEM)
			self.popupDialog.Show()

		player.SetAcceActivedItemSlot(targetSlot, slotIndex)
		net.SendAcceRefineCheckIn(player.INVENTORY, slotIndex, targetSlot)

	## Slot Event
	def SelectEmptySlot(self, selectedSlotPos):
		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			
			attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
			if player.RESERVED_WINDOW == attachedInvenType:
				return
			
			if player.INVENTORY == attachedInvenType:
				player.SetAcceActivedItemSlot(selectedSlotPos, attachedSlotPos)
				#self.highlightedSlotList[selectedSlotPos] = attachedSlotPos
				if self.windowType == 1 and player.IsAcceWindowEmpty():
					self.popupDialog = uiCommon.PopupDialog()
					self.popupDialog.SetText(localeInfo.ACCE_DEL_SERVEITEM)
					self.popupDialog.Show()
				elif self.windowType == 0 and player.IsAcceWindowEmpty():
					self.popupDialog = uiCommon.PopupDialog()
					self.popupDialog.SetText(localeInfo.ACCE_DEL_ABSORDITEM)
					self.popupDialog.Show()
				net.SendAcceRefineCheckIn(attachedInvenType, attachedSlotPos, selectedSlotPos)
			
			mouseModule.mouseController.DeattachObject()

	def SelectItemSlot(self, selectedSlotPos):
		if not mouseModule.mouseController.isAttached():
			player.SetAcceActivedItemSlot(selectedSlotPos, -1)
			net.SendAcceRefineCheckOut(selectedSlotPos)

	def UseItemSlot(self, slotIndex):
		player.SetAcceActivedItemSlot(slotIndex, -1)
		net.SendAcceRefineCheckOut(slotIndex)
		mouseModule.mouseController.DeattachObject()
		self.OverOutItem()

	def RefreshAcceWindow(self):
		if self.wndItem:
			for i in xrange(3):
				self.wndItem.SetItemSlot(i, player.GetAcceItemID(i), 0)
				if self.windowType == 1:
					if i == 0:
						if player.GetAcceItemID(i) != 0:
							item.SelectItem(player.GetAcceItemID(i))
							self.Cost.SetText(localeInfo.ACCE_ABSORB_COST % (item.GetIBuyItemPrice()))
						else:
							self.Cost.SetText(localeInfo.ACCE_ABSORB_COST % (0))
			
			self.wndItem.RefreshSlot()

		if self.interface:
			self.interface.RefreshInventory()


	def __OnClosePopupDialog(self):
		pass
	
	def OnUpdate(self):
		USE_ACCEWINDOW_LIMIT_RANGE = 2500

		(x, y, z) = player.GetMainCharacterPosition()
		if abs(x - self.xAcceWindowStart) > USE_ACCEWINDOW_LIMIT_RANGE or abs(y - self.yAcceWindowStart) > USE_ACCEWINDOW_LIMIT_RANGE:
			self.Close()

	def OnPressEscapeKey(self):
		self.Close()

