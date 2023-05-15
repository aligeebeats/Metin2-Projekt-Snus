##
## Interface
##
import constInfo
import systemSetting
import wndMgr
import chat
import app
import player
import uiTaskBar
import uiCharacter
import uiInventory
import uiDragonSoul
import uiChat
import uiMessenger
import guild
import net
import snd
import ui
import uiHelp
import uiWhisper
import uiPointReset
import uiShop
import uiExchange
import uiSystem
import uiRestart
import uiToolTip
import uiMiniMap
import uiParty
import uiSafebox
import uiGuild
import uiQuest
import uiPrivateShopBuilder
import uiCommon
import uiRefine
import uiEquipmentDialog
import uiGameButton
import uiTip
import uiCube
import miniMap
# ACCESSORY_REFINE_ADD_METIN_STONE
import uiselectitem
# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE
import uiScriptLocale

import event
import localeInfo
import uiacce as uiAcce
if app.ENABLE_CHEST_DROP:
	import uiChestDropInfo

if app.ENABLE_HUNTING_SYSTEM:
	import uiHunting

if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
	import uiBuffNPC

if app.ENABLE_SWITCHBOT:
	import uiSwitchbot

if app.ENABLE_ITEMSHOP:
	import uiitemshop
	import net

IsQBHide = 0
class Interface(object):
	CHARACTER_STATUS_TAB = 1
	CHARACTER_SKILL_TAB = 2
	
	def __init__(self):
		systemSetting.SetInterfaceHandler(self)
		self.windowOpenPosition = 0
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.onTopWindow = player.ON_TOP_WND_NONE
		self.dlgWhisperWithoutTarget = None
		self.inputDialog = None
		self.tipBoard = None
		self.bigBoard = None

		# ITEM_MALL
		self.mallPageDlg = None
		# END_OF_ITEM_MALL

		self.wndWeb = None
		self.wndTaskBar = None
		self.wndCharacter = None
		self.wndInventory = None
		self.wndExpandedTaskBar = None
		self.wndDragonSoul = None
		self.wndDragonSoulRefine = None
		self.wndChat = None
		self.wndMessenger = None
		self.wndMiniMap = None
		self.wndGuild = None

		if app.ENABLE_CHEST_DROP:
			self.wndChestDropInfo = None

		if app.ENABLE_SWITCHBOT:
			self.wndSwitchbot = None

		if app.ENABLE_HUNTING_SYSTEM:
			self.wndHunting = None
			self.wndHuntingSelect = None
			self.wndHuntingReward = None

		if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
			self.wndBuffNPCWindow = None
			self.wndBuffNPCCreateWindow = None

		if app.ENABLE_ITEMSHOP:
			self.wndItemshop = None

		self.wndGuildBuilding = None
		self.wndAcce = None

		self.listGMName = {}
		self.wndQuestWindow = {}
		self.wndQuestWindowNewKey = 0
		self.privateShopAdvertisementBoardDict = {}
		self.guildScoreBoardDict = {}
		self.equipmentDialogDict = {}
		event.SetInterfaceWindow(self)

	def __del__(self):
		systemSetting.DestroyInterfaceHandler()
		event.SetInterfaceWindow(None)

	################################
	## Make Windows & Dialogs
	def __MakeUICurtain(self):
		wndUICurtain = ui.Bar("TOP_MOST")
		wndUICurtain.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
		wndUICurtain.SetColor(0x77000000)
		wndUICurtain.Hide()
		self.wndUICurtain = wndUICurtain

	def __MakeMessengerWindow(self):
		self.wndMessenger = uiMessenger.MessengerWindow()

		from _weakref import proxy
		self.wndMessenger.SetWhisperButtonEvent(lambda n,i=proxy(self):i.OpenWhisperDialog(n))
		self.wndMessenger.SetGuildButtonEvent(ui.__mem_func__(self.ToggleGuildWindow))

	def __MakeGuildWindow(self):
		self.wndGuild = uiGuild.GuildWindow()

	def __MakeChatWindow(self):
		
		wndChat = uiChat.ChatWindow()
		
		wndChat.SetSize(wndChat.CHAT_WINDOW_WIDTH, 0)
		wndChat.SetPosition(wndMgr.GetScreenWidth()/2 - wndChat.CHAT_WINDOW_WIDTH/2, wndMgr.GetScreenHeight() - wndChat.EDIT_LINE_HEIGHT - 37)
		wndChat.SetHeight(200)
		wndChat.Refresh()
		wndChat.Show()

		self.wndChat = wndChat
		self.wndChat.BindInterface(self)
		self.wndChat.SetSendWhisperEvent(ui.__mem_func__(self.OpenWhisperDialogWithoutTarget))
		self.wndChat.SetOpenChatLogEvent(ui.__mem_func__(self.ToggleChatLogWindow))

	def __MakeTaskBar(self):
		wndTaskBar = uiTaskBar.TaskBar()
		wndTaskBar.LoadWindow()
		self.wndTaskBar = wndTaskBar
		self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_CHARACTER, ui.__mem_func__(self.ToggleCharacterWindowStatusPage))
		self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_INVENTORY, ui.__mem_func__(self.ToggleInventoryWindow))
		self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_MESSENGER, ui.__mem_func__(self.ToggleMessenger))
		self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_SYSTEM, ui.__mem_func__(self.ToggleSystemDialog))
		if app.ENABLE_HUNTING_SYSTEM:
			self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_HUNTING, ui.__mem_func__(self.ToggleHuntingWindow))
		if uiTaskBar.TaskBar.IS_EXPANDED:
			self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_EXPAND, ui.__mem_func__(self.ToggleExpandedButton))
			self.wndExpandedTaskBar = uiTaskBar.ExpandedTaskBar()
			self.wndExpandedTaskBar.LoadWindow()
			self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_DRAGON_SOUL, ui.__mem_func__(self.ToggleDragonSoulWindow))

		else:
			self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_CHAT, ui.__mem_func__(self.ToggleChat))
		
		self.wndEnergyBar = None
		wndEnergyBar = uiTaskBar.EnergyBar()
		wndEnergyBar.LoadWindow()
		self.wndEnergyBar = wndEnergyBar	

	def __MakeParty(self):
		wndParty = uiParty.PartyWindow()
		wndParty.Hide()
		self.wndParty = wndParty

	def __MakeGameButtonWindow(self):
		wndGameButton = uiGameButton.GameButtonWindow()
		wndGameButton.SetTop()
		wndGameButton.Show()
		wndGameButton.SetButtonEvent("STATUS", ui.__mem_func__(self.__OnClickStatusPlusButton))
		wndGameButton.SetButtonEvent("SKILL", ui.__mem_func__(self.__OnClickSkillPlusButton))
		wndGameButton.SetButtonEvent("QUEST", ui.__mem_func__(self.__OnClickQuestButton))
		wndGameButton.SetButtonEvent("HELP", ui.__mem_func__(self.__OnClickHelpButton))
		wndGameButton.SetButtonEvent("BUILD", ui.__mem_func__(self.__OnClickBuildButton))

		self.wndGameButton = wndGameButton

	def __IsChatOpen(self):
		return TRUE
		
	def __MakeWindows(self):
		wndCharacter = uiCharacter.CharacterWindow()
		wndInventory = uiInventory.InventoryWindow()
		wndInventory.BindInterfaceClass(self)
		wndDragonSoul = uiDragonSoul.DragonSoulWindow()	
		wndDragonSoulRefine = uiDragonSoul.DragonSoulRefineWindow()
 
		wndMiniMap = uiMiniMap.MiniMap()
		wndSafebox = uiSafebox.SafeboxWindow()
		
		if app.WJ_ENABLE_TRADABLE_ICON:
			wndSafebox.BindInterface(self)
		
		if app.ENABLE_CHEST_DROP:
			wndSafebox.BindInterface(self)
		
		# ITEM_MALL
		wndMall = uiSafebox.MallWindow()
		self.wndMall = wndMall
		# END_OF_ITEM_MALL

		if self.wndAcce:
			self.wndAcce.Destroy()

		wndChatLog = uiChat.ChatLogWindow()
		wndChatLog.BindInterface(self)
		
		self.wndCharacter = wndCharacter
		self.wndInventory = wndInventory
		self.wndDragonSoul = wndDragonSoul
		self.wndDragonSoulRefine = wndDragonSoulRefine
		self.wndMiniMap = wndMiniMap
		self.wndSafebox = wndSafebox
		self.wndChatLog = wndChatLog
		
		self.wndDragonSoul.SetDragonSoulRefineWindow(self.wndDragonSoulRefine)
		self.wndDragonSoulRefine.SetInventoryWindows(self.wndInventory, self.wndDragonSoul)
		self.wndInventory.SetDragonSoulRefineWindow(self.wndDragonSoulRefine)

		if app.ENABLE_CHEST_DROP:
			self.wndChestDropInfo = uiChestDropInfo.ChestDropInfoWindow()

		if app.ENABLE_SWITCHBOT:
			self.wndSwitchbot = uiSwitchbot.SwitchbotWindow()

		if app.ITEM_CHECKINOUT_UPDATE:
			self.wndInventory.SetSafeboxWindow(self.wndSafebox)

		if app.ENABLE_HUNTING_SYSTEM:
			self.wndHunting = uiHunting.HuntingWindow()
			self.wndHuntingSelect = uiHunting.HuntingSelectWindow()
			self.wndHuntingReward = uiHunting.HuntingRewardWindow()

		if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
			self.wndBuffNPCWindow = uiBuffNPC.BuffNPCWindow()
			self.wndBuffNPCCreateWindow = uiBuffNPC.BuffNPCCreateWindow()

	def __MakeDialogs(self):
		self.dlgExchange = uiExchange.ExchangeDialog()
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.dlgExchange.BindInterface(self)
			self.dlgExchange.SetInven(self.wndInventory)
			self.wndInventory.BindWindow(self.dlgExchange)
		self.dlgExchange.LoadDialog()
		self.dlgExchange.SetCenterPosition()
		self.dlgExchange.Hide()

		self.dlgPointReset = uiPointReset.PointResetDialog()
		self.dlgPointReset.LoadDialog()
		self.dlgPointReset.Hide()

		self.dlgShop = uiShop.ShopDialog()
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.dlgShop.BindInterface(self)
		self.dlgShop.LoadDialog()
		if app.ENABLE_CHEST_DROP:
			self.dlgShop.BindInterface(self)
		self.dlgShop.Hide()

		self.dlgRestart = uiRestart.RestartDialog()
		self.dlgRestart.LoadDialog()
		self.dlgRestart.Hide()

		self.dlgSystem = uiSystem.SystemDialog()
		self.dlgSystem.LoadDialog()
		self.dlgSystem.SetOpenHelpWindowEvent(ui.__mem_func__(self.OpenHelpWindow))

		self.dlgSystem.Hide()

		self.dlgPassword = uiSafebox.PasswordDialog()
		self.dlgPassword.Hide()

		self.hyperlinkItemTooltip = uiToolTip.HyperlinkItemToolTip()
		self.hyperlinkItemTooltip.Hide()

		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.Hide()

		self.tooltipSkill = uiToolTip.SkillToolTip()
		self.tooltipSkill.Hide()

		self.privateShopBuilder = uiPrivateShopBuilder.PrivateShopBuilder()
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.privateShopBuilder.BindInterface(self)
			self.privateShopBuilder.SetInven(self.wndInventory)
			self.wndInventory.BindWindow(self.privateShopBuilder)
		self.privateShopBuilder.Hide()

		self.dlgRefineNew = uiRefine.RefineDialogNew()
		if app.WJ_ENABLE_TRADABLE_ICON:
#			self.dlgRefineNew.SetInven(self.wndInventory)
			self.wndInventory.BindWindow(self.dlgRefineNew)
		self.dlgRefineNew.Hide()

	def __MakeHelpWindow(self):
		self.wndHelp = uiHelp.HelpWindow()
		self.wndHelp.LoadDialog()
		self.wndHelp.SetCloseEvent(ui.__mem_func__(self.CloseHelpWindow))
		self.wndHelp.Hide()

	def __MakeTipBoard(self):
		self.tipBoard = uiTip.TipBoard()
		self.tipBoard.Hide()

		self.bigBoard = uiTip.BigBoard()
		self.bigBoard.Hide()

	def __MakeWebWindow(self):
		if constInfo.IN_GAME_SHOP_ENABLE:
			import uiWeb
			self.wndWeb = uiWeb.WebWindow()
			self.wndWeb.LoadWindow()
			self.wndWeb.Hide()

	def __MakeCubeWindow(self):
		self.wndCube = uiCube.CubeWindow()
		self.wndCube.LoadWindow()
		self.wndCube.Hide()

	def __MakeCubeResultWindow(self):
		self.wndCubeResult = uiCube.CubeResultWindow()
		self.wndCubeResult.LoadWindow()
		self.wndCubeResult.Hide()

	# ACCESSORY_REFINE_ADD_METIN_STONE
	def __MakeItemSelectWindow(self):
		self.wndItemSelect = uiselectitem.SelectItemWindow()
		self.wndItemSelect.Hide()
	# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE

	if app.ENABLE_ITEMSHOP:
		def __MakeItemshop(self):
			self.wndItemshop = uiitemshop.ItemshopWindow()
			self.wndItemshop.Hide()

	def MakeInterface(self):
		self.__MakeMessengerWindow()
		self.__MakeGuildWindow()
		self.__MakeChatWindow()
		self.__MakeParty()
		self.__MakeWindows()
		self.__MakeDialogs()

		self.__MakeUICurtain()
		self.__MakeTaskBar()
		self.__MakeGameButtonWindow()
		self.__MakeHelpWindow()
		self.__MakeTipBoard()
		self.__MakeWebWindow()
		self.__MakeCubeWindow()
		self.__MakeCubeResultWindow()
		
		
		# ACCESSORY_REFINE_ADD_METIN_STONE
		self.__MakeItemSelectWindow()
		# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE

		if app.ENABLE_ITEMSHOP:
			self.__MakeItemshop()

		self.questButtonList = []
		self.whisperButtonList = []
		self.whisperDialogDict = {}
		self.privateShopAdvertisementBoardDict = {}

		self.wndInventory.SetItemToolTip(self.tooltipItem)
		self.wndDragonSoul.SetItemToolTip(self.tooltipItem)
		self.wndDragonSoulRefine.SetItemToolTip(self.tooltipItem)
		self.wndSafebox.SetItemToolTip(self.tooltipItem)
		self.wndCube.SetItemToolTip(self.tooltipItem)
		self.wndCubeResult.SetItemToolTip(self.tooltipItem)

		if app.ENABLE_SWITCHBOT:
			self.wndSwitchbot.SetItemToolTip(self.tooltipItem)

		# ITEM_MALL
		self.wndMall.SetItemToolTip(self.tooltipItem)
		# END_OF_ITEM_MALL

		self.wndCharacter.SetSkillToolTip(self.tooltipSkill)
		self.wndTaskBar.SetItemToolTip(self.tooltipItem)
		self.wndTaskBar.SetSkillToolTip(self.tooltipSkill)
		self.wndGuild.SetSkillToolTip(self.tooltipSkill)

		if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
			self.wndBuffNPCWindow.SetSkillToolTip(self.tooltipSkill)

		# ACCESSORY_REFINE_ADD_METIN_STONE
		self.wndItemSelect.SetItemToolTip(self.tooltipItem)
		# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE

		self.dlgShop.SetItemToolTip(self.tooltipItem)
		self.dlgExchange.SetItemToolTip(self.tooltipItem)
		self.privateShopBuilder.SetItemToolTip(self.tooltipItem)

		self.__InitWhisper()
		self.DRAGON_SOUL_IS_QUALIFIED = FALSE

	def MakeHyperlinkTooltip(self, hyperlink):
		tokens = hyperlink.split(":")
		if tokens and len(tokens):
			type = tokens[0]
			if "item" == type:
				self.hyperlinkItemTooltip.SetHyperlinkItem(tokens)

	## Make Windows & Dialogs
	################################

	def Close(self):
		if self.dlgWhisperWithoutTarget:
			self.dlgWhisperWithoutTarget.Destroy()
			del self.dlgWhisperWithoutTarget

		if uiQuest.QuestDialog.__dict__.has_key("QuestCurtain"):
			uiQuest.QuestDialog.QuestCurtain.Close()

		if self.wndQuestWindow:
			for key, eachQuestWindow in self.wndQuestWindow.items():
				eachQuestWindow.nextCurtainMode = -1
				eachQuestWindow.CloseSelf()
				eachQuestWindow = None
		self.wndQuestWindow = {}

		if app.ENABLE_SWITCHBOT:
			if self.wndSwitchbot:
				self.wndSwitchbot.Destroy()

		if self.wndChat:
			self.wndChat.Destroy()

		if self.wndTaskBar:
			self.wndTaskBar.Destroy()
		
		if self.wndExpandedTaskBar:
			self.wndExpandedTaskBar.Destroy()
			
		if self.wndEnergyBar:
			self.wndEnergyBar.Destroy()

		if self.wndCharacter:
			self.wndCharacter.Hide()#fix
			self.wndCharacter.Destroy()

		if self.wndInventory:
			self.wndInventory.Hide()#fix
			self.wndInventory.Destroy()

		if app.ENABLE_ITEMSHOP and self.wndItemshop:
			self.wndItemshop.Hide()
			self.wndItemshop.Destroy()

		if self.wndDragonSoul:
			self.wndDragonSoul.Destroy()

		if self.wndDragonSoulRefine:
			self.wndDragonSoulRefine.Destroy()

		if self.dlgExchange:
			self.dlgExchange.Destroy()

		if self.dlgPointReset:
			self.dlgPointReset.Destroy()

		if self.dlgShop:
			self.dlgShop.Destroy()

		if self.dlgRestart:
			self.dlgRestart.Destroy()

		if self.dlgSystem:
			self.dlgSystem.Destroy()

		if self.dlgPassword:
			self.dlgPassword.Destroy()

		if self.wndMiniMap:
			self.wndMiniMap.Destroy()

		if self.wndSafebox:
			self.wndSafebox.Destroy()

		if self.wndWeb:
			self.wndWeb.Destroy()
			self.wndWeb = None

		if self.wndMall:
			self.wndMall.Destroy()

		if self.wndParty:
			self.wndParty.Destroy()

		if self.wndHelp:
			self.wndHelp.Destroy()

		if self.wndCube:
			self.wndCube.Destroy()
			
		if self.wndCubeResult:
			self.wndCubeResult.Destroy()

		if self.wndMessenger:
			self.wndMessenger.Destroy()

		if self.wndGuild:
			self.wndGuild.Destroy()

		if self.privateShopBuilder:
			self.privateShopBuilder.Destroy()

		if self.dlgRefineNew:
			self.dlgRefineNew.Destroy()

		if self.wndGuildBuilding:
			self.wndGuildBuilding.Destroy()

		if self.wndGameButton:
			self.wndGameButton.Destroy()

		if app.ENABLE_CHEST_DROP:
			if self.wndChestDropInfo:
				del self.wndChestDropInfo

		# ITEM_MALL
		if self.mallPageDlg:
			self.mallPageDlg.Destroy()
		# END_OF_ITEM_MALL

		# HUNTING_SYSTEM
		if app.ENABLE_HUNTING_SYSTEM:
			if self.wndHunting:
				self.wndHunting.Destroy()
			if self.wndHuntingSelect:
				self.wndHuntingSelect.Destroy()
			if self.wndHuntingReward:
				self.wndHuntingReward.Destroy()
		# END_OF_HUNTING_SYSTEM

		# ACCESSORY_REFINE_ADD_METIN_STONE
		if self.wndItemSelect:
			self.wndItemSelect.Destroy()
		# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE

		if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
			if self.wndBuffNPCWindow:
				self.wndBuffNPCWindow.Destroy()
			if self.wndBuffNPCCreateWindow:
				self.wndBuffNPCCreateWindow.Destroy()

		self.wndChatLog.Destroy()
		for btn in self.questButtonList:
			btn.SetEvent(0)
		for btn in self.whisperButtonList:
			btn.SetEvent(0)
		for dlg in self.whisperDialogDict.itervalues():
			dlg.Destroy()
		for brd in self.guildScoreBoardDict.itervalues():
			brd.Destroy()
		for dlg in self.equipmentDialogDict.itervalues():
			dlg.Destroy()

		# ITEM_MALL
		del self.mallPageDlg
		# END_OF_ITEM_MALL

		del self.wndGuild
		del self.wndMessenger
		del self.wndUICurtain
		del self.wndChat
		del self.wndTaskBar
		if self.wndExpandedTaskBar:
			del self.wndExpandedTaskBar
		del self.wndEnergyBar
		del self.wndCharacter
		del self.wndInventory
		if self.wndDragonSoul:
			del self.wndDragonSoul
		if self.wndDragonSoulRefine:
			del self.wndDragonSoulRefine
		del self.dlgExchange
		del self.dlgPointReset
		del self.dlgShop
		del self.dlgRestart
		del self.dlgSystem
		del self.dlgPassword
		del self.hyperlinkItemTooltip
		del self.tooltipItem
		del self.tooltipSkill
		del self.wndMiniMap
		del self.wndSafebox
		del self.wndMall
		del self.wndParty
		del self.wndHelp
		del self.wndCube
		del self.wndCubeResult
		del self.privateShopBuilder
		del self.inputDialog
		del self.wndChatLog
		del self.dlgRefineNew
		del self.wndGuildBuilding
		del self.wndGameButton
		del self.tipBoard
		del self.bigBoard
		del self.wndItemSelect
		del self.wndAcce

		if app.ENABLE_ITEMSHOP:
			del self.wndItemshop

		if app.ENABLE_SWITCHBOT:
			del self.wndSwitchbot

		if app.ENABLE_HUNTING_SYSTEM:
			del self.wndHunting
			del self.wndHuntingSelect
			del self.wndHuntingReward

		if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
			del self.wndBuffNPCWindow
			del self.wndBuffNPCCreateWindow

		self.questButtonList = []
		self.whisperButtonList = []
		self.whisperDialogDict = {}
		self.privateShopAdvertisementBoardDict = {}
		self.guildScoreBoardDict = {}
		self.equipmentDialogDict = {}

		uiChat.DestroyChatInputSetWindow()

	## Skill
	def OnUseSkill(self, slotIndex, coolTime):
		self.wndCharacter.OnUseSkill(slotIndex, coolTime)
		self.wndTaskBar.OnUseSkill(slotIndex, coolTime)
		self.wndGuild.OnUseSkill(slotIndex, coolTime)

	def OnActivateSkill(self, slotIndex):
		self.wndCharacter.OnActivateSkill(slotIndex)
		self.wndTaskBar.OnActivateSkill(slotIndex)

	def OnDeactivateSkill(self, slotIndex):
		self.wndCharacter.OnDeactivateSkill(slotIndex)
		self.wndTaskBar.OnDeactivateSkill(slotIndex)

	def OnChangeCurrentSkill(self, skillSlotNumber):
		self.wndTaskBar.OnChangeCurrentSkill(skillSlotNumber)

	def SelectMouseButtonEvent(self, dir, event):
		self.wndTaskBar.SelectMouseButtonEvent(dir, event)

	## Refresh
	def RefreshAlignment(self):
		self.wndCharacter.RefreshAlignment()

	def RefreshStatus(self):
		self.wndTaskBar.RefreshStatus()
		self.wndCharacter.RefreshStatus()
		self.wndInventory.RefreshStatus()
		if self.wndEnergyBar:
			self.wndEnergyBar.RefreshStatus()
		self.wndDragonSoul.RefreshStatus()

	def RefreshStamina(self):
		self.wndTaskBar.RefreshStamina()

	def RefreshSkill(self):
		self.wndCharacter.RefreshSkill()
		self.wndTaskBar.RefreshSkill()

	def RefreshInventory(self):
		self.wndTaskBar.RefreshQuickSlot()
		self.wndInventory.RefreshItemSlot()
		self.wndDragonSoul.RefreshItemSlot()

		if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
			self.wndBuffNPCWindow.RefreshEquipSlotWindow()

	def ActivateAcceSlot(self, slotIndex):
		self.wndInventory.AppendAcceSlot(slotIndex)

	def DeactivateAcceSlot(self, slotIndex):
		self.wndInventory.RemoveAcceSlot(slotIndex)

	def RefreshAcce(self):
			if self.wndAcce:
				self.wndAcce.RefreshAcceWindow()

	def	OpenAcceWindow(self, type):
		if not self.wndAcce:
			self.wndAcce = uiAcce.AcceWindow(self)
			self.wndAcce.SetItemToolTip(self.tooltipItem)

		self.wndAcce.Open(type)

	def CloseAcceWindow(self):
		if self.wndAcce:
			self.wndAcce.Close()

	def AcceOpen(self):
		if self.wndAcce:
			return self.wndAcce.IsShow()

		return False
			
	def AcceAutoSetItem(self, slotIndex):
		if self.wndAcce:
			self.wndAcce.AutoSetItem(slotIndex)

	# def ActivateAcceSlot(self, slotPos):
		# self.wndInventory.ActivateAcceSlot(slotPos)

	# def DeactivateAcceSlot(self, slotPos):
		# self.wndInventory.DeactivateAcceSlot(slotPos)

	def RefreshCharacter(self): ## Character �������� ��, Inventory �������� ���� �׸� ���� Refresh
		self.wndCharacter.RefreshCharacter()
		self.wndTaskBar.RefreshQuickSlot()

	def RefreshQuest(self):
		self.wndCharacter.RefreshQuest()

	def RefreshSafebox(self):
		self.wndSafebox.RefreshSafebox()

	# ITEM_MALL
	def RefreshMall(self):
		self.wndMall.RefreshMall()

	def OpenItemMall(self):
		if not self.mallPageDlg:
			self.mallPageDlg = uiShop.MallPageDialog()

		self.mallPageDlg.Open()
	# END_OF_ITEM_MALL

	def RefreshMessenger(self):
		self.wndMessenger.RefreshMessenger()

	def RefreshGuildInfoPage(self):
		self.wndGuild.RefreshGuildInfoPage()

	def RefreshGuildBoardPage(self):
		self.wndGuild.RefreshGuildBoardPage()

	def RefreshGuildMemberPage(self):
		self.wndGuild.RefreshGuildMemberPage()

	def RefreshGuildMemberPageGradeComboBox(self):
		self.wndGuild.RefreshGuildMemberPageGradeComboBox()

	def RefreshGuildSkillPage(self):
		self.wndGuild.RefreshGuildSkillPage()

	def RefreshGuildGradePage(self):
		self.wndGuild.RefreshGuildGradePage()

	def DeleteGuild(self):
		self.wndMessenger.ClearGuildMember()
		self.wndGuild.DeleteGuild()

	def RefreshMobile(self):
		self.dlgSystem.RefreshMobile()

	def OnMobileAuthority(self):
		self.dlgSystem.OnMobileAuthority()

	def OnBlockMode(self, mode):
		self.dlgSystem.OnBlockMode(mode)

	## Calling Functions
	# PointReset
	def OpenPointResetDialog(self):
		self.dlgPointReset.Show()
		self.dlgPointReset.SetTop()

	def ClosePointResetDialog(self):
		self.dlgPointReset.Close()

	# Shop
	def OpenShopDialog(self, vid):
		self.wndInventory.Show()
		self.wndInventory.SetTop()
		self.dlgShop.Open(vid)
		self.dlgShop.SetTop()

	def CloseShopDialog(self):
		self.dlgShop.Close()

	def RefreshShopDialog(self):
		self.dlgShop.Refresh()

	## Quest
	def OpenCharacterWindowQuestPage(self):
		self.wndCharacter.Show()
		self.wndCharacter.SetState("QUEST")

	def OpenQuestWindow(self, skin, idx):

		wnds = ()

		q = uiQuest.QuestDialog(skin, idx)
		q.SetWindowName("QuestWindow" + str(idx))
		q.Show()
		if skin:
			q.Lock()
			wnds = self.__HideWindows()

			# UNKNOWN_UPDATE
			q.AddOnDoneEvent(lambda tmp_self, args=wnds: self.__ShowWindows(args))
			# END_OF_UNKNOWN_UPDATE

		if skin:
			q.AddOnCloseEvent(q.Unlock)
		q.AddOnCloseEvent(lambda key = self.wndQuestWindowNewKey:ui.__mem_func__(self.RemoveQuestDialog)(key))
		self.wndQuestWindow[self.wndQuestWindowNewKey] = q

		self.wndQuestWindowNewKey = self.wndQuestWindowNewKey + 1

		# END_OF_UNKNOWN_UPDATE
		
	def RemoveQuestDialog(self, key):
		del self.wndQuestWindow[key]

	## Exchange
	def StartExchange(self):
		self.dlgExchange.OpenDialog()
		self.dlgExchange.Refresh()

	def EndExchange(self):
		self.dlgExchange.CloseDialog()

	def RefreshExchange(self):
		self.dlgExchange.Refresh()
		
	if app.WJ_ENABLE_TRADABLE_ICON:
		def CantTradableItemExchange(self, dstSlotIndex, srcSlotIndex):
			self.dlgExchange.CantTradableItem(dstSlotIndex, srcSlotIndex)

	## Party
	def AddPartyMember(self, pid, name):
		self.wndParty.AddPartyMember(pid, name)

		self.__ArrangeQuestButton()

	def UpdatePartyMemberInfo(self, pid):
		self.wndParty.UpdatePartyMemberInfo(pid)

	def RemovePartyMember(self, pid):
		self.wndParty.RemovePartyMember(pid)

		##!! 20061026.levites.����Ʈ_��ġ_����
		self.__ArrangeQuestButton()

	def LinkPartyMember(self, pid, vid):
		self.wndParty.LinkPartyMember(pid, vid)

	def UnlinkPartyMember(self, pid):
		self.wndParty.UnlinkPartyMember(pid)

	def UnlinkAllPartyMember(self):
		self.wndParty.UnlinkAllPartyMember()

	def ExitParty(self):
		self.wndParty.ExitParty()

		##!! 20061026.levites.����Ʈ_��ġ_����
		self.__ArrangeQuestButton()

	def PartyHealReady(self):
		self.wndParty.PartyHealReady()

	def ChangePartyParameter(self, distributionMode):
		self.wndParty.ChangePartyParameter(distributionMode)

	## Safebox
	def AskSafeboxPassword(self):
		if self.wndSafebox.IsShow():
			return

		# SAFEBOX_PASSWORD
		self.dlgPassword.SetTitle(localeInfo.PASSWORD_TITLE)
		self.dlgPassword.SetSendMessage("/safebox_password ")
		# END_OF_SAFEBOX_PASSWORD

		self.dlgPassword.ShowDialog()

	def OpenSafeboxWindow(self, size):
		self.dlgPassword.CloseDialog()
		self.wndSafebox.ShowWindow(size)

	def RefreshSafeboxMoney(self):
		self.wndSafebox.RefreshSafeboxMoney()

	def CommandCloseSafebox(self):
		self.wndSafebox.CommandCloseSafebox()

	# ITEM_MALL
	def AskMallPassword(self):
		if self.wndMall.IsShow():
			return
		self.dlgPassword.SetTitle(localeInfo.MALL_PASSWORD_TITLE)
		self.dlgPassword.SetSendMessage("/mall_password ")
		self.dlgPassword.ShowDialog()

	def OpenMallWindow(self, size):
		self.dlgPassword.CloseDialog()
		self.wndMall.ShowWindow(size)

	def CommandCloseMall(self):
		self.wndMall.CommandCloseMall()
	# END_OF_ITEM_MALL

	## Guild
	def OnStartGuildWar(self, guildSelf, guildOpp):
		self.wndGuild.OnStartGuildWar(guildSelf, guildOpp)

		guildWarScoreBoard = uiGuild.GuildWarScoreBoard()
		guildWarScoreBoard.Open(guildSelf, guildOpp)
		guildWarScoreBoard.Show()
		self.guildScoreBoardDict[uiGuild.GetGVGKey(guildSelf, guildOpp)] = guildWarScoreBoard

	def OnEndGuildWar(self, guildSelf, guildOpp):
		self.wndGuild.OnEndGuildWar(guildSelf, guildOpp)

		key = uiGuild.GetGVGKey(guildSelf, guildOpp)

		if not self.guildScoreBoardDict.has_key(key):
			return

		self.guildScoreBoardDict[key].Destroy()
		del self.guildScoreBoardDict[key]

	# GUILDWAR_MEMBER_COUNT
	def UpdateMemberCount(self, gulidID1, memberCount1, guildID2, memberCount2):
		key = uiGuild.GetGVGKey(gulidID1, guildID2)

		if not self.guildScoreBoardDict.has_key(key):
			return

		self.guildScoreBoardDict[key].UpdateMemberCount(gulidID1, memberCount1, guildID2, memberCount2)
	# END_OF_GUILDWAR_MEMBER_COUNT

	def OnRecvGuildWarPoint(self, gainGuildID, opponentGuildID, point):
		key = uiGuild.GetGVGKey(gainGuildID, opponentGuildID)
		if not self.guildScoreBoardDict.has_key(key):
			return

		guildBoard = self.guildScoreBoardDict[key]
		guildBoard.SetScore(gainGuildID, opponentGuildID, point)

	## PK Mode
	def OnChangePKMode(self):
		self.wndCharacter.RefreshAlignment()
		self.dlgSystem.OnChangePKMode()

	## Refine
	def OpenRefineDialog(self, targetItemPos, nextGradeItemVnum, cost, prob, type):
		self.dlgRefineNew.Open(targetItemPos, nextGradeItemVnum, cost, prob, type)

	def AppendMaterialToRefineDialog(self, vnum, count):
		self.dlgRefineNew.AppendMaterial(vnum, count)

	## Show & Hide
	def ShowDefaultWindows(self):
		self.wndTaskBar.Show()
		self.wndMiniMap.Show()
		self.wndMiniMap.ShowMiniMap()
		if self.wndEnergyBar:
			self.wndEnergyBar.Show()

	def ShowAllWindows(self):
		self.wndTaskBar.Show()
		self.wndCharacter.Show()
		self.wndInventory.Show()
		self.wndDragonSoul.Show()
		self.wndDragonSoulRefine.Show()
		self.wndChat.Show()
		self.wndMiniMap.Show()
		if app.ENABLE_ITEMSHOP and self.wndItemshop:
			self.wndItemshop.Show()
		if self.wndEnergyBar:
			self.wndEnergyBar.Show()
		if self.wndExpandedTaskBar:
			self.wndExpandedTaskBar.Show()
			self.wndExpandedTaskBar.SetTop()

	def HideAllWindows(self):
		if self.wndTaskBar:
			self.wndTaskBar.Hide()
		
		if self.wndEnergyBar:
			self.wndEnergyBar.Hide()

		if self.wndCharacter:
			self.wndCharacter.Close()

		if self.wndInventory:
			self.wndInventory.Hide()

		if app.ENABLE_ITEMSHOP and self.wndItemshop:
			self.wndItemshop.Hide()

		self.wndDragonSoul.Hide()
		self.wndDragonSoulRefine.Hide()

		if self.wndChat:
			self.wndChat.Hide()

		if self.wndMiniMap:
			self.wndMiniMap.Hide()

		if self.wndMessenger:
			self.wndMessenger.Hide()

		if self.wndGuild:
			self.wndGuild.Hide()
			
		if self.wndExpandedTaskBar:
			self.wndExpandedTaskBar.Hide()

		if app.ENABLE_CHEST_DROP:
			if self.wndChestDropInfo:
				self.wndChestDropInfo.Hide()

		if app.ENABLE_SWITCHBOT:
			if self.wndSwitchbot:
				self.wndSwitchbot.Hide()

		if app.ENABLE_HUNTING_SYSTEM:
			if self.wndHunting:
				self.wndHunting.Hide()
			if self.wndHuntingSelect:
				self.wndHuntingSelect.Hide()
			if self.wndHuntingReward:
				self.wndHuntingReward.Hide()

		if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
			if self.wndBuffNPCWindow:
				self.wndBuffNPCWindow.Hide()
			if self.wndBuffNPCCreateWindow:
				self.wndBuffNPCCreateWindow.Hide()

	def ShowMouseImage(self):
		self.wndTaskBar.ShowMouseImage()

	def HideMouseImage(self):
		self.wndTaskBar.HideMouseImage()

	def ToggleChat(self):
		if TRUE == self.wndChat.IsEditMode():
			self.wndChat.CloseChat()
		else:
			# ���������� ���������� ä�� �Է��� �ȵ�
			if self.wndWeb and self.wndWeb.IsShow():
				pass
			else:
				self.wndChat.OpenChat()

	def IsOpenChat(self):
		return self.wndChat.IsEditMode()

	def SetChatFocus(self):
		self.wndChat.SetChatFocus()

	def OpenRestartDialog(self):
		self.dlgRestart.OpenDialog()
		self.dlgRestart.SetTop()

	def CloseRestartDialog(self):
		self.dlgRestart.Close()

	def ToggleSystemDialog(self):
		if FALSE == self.dlgSystem.IsShow():
			self.dlgSystem.OpenDialog()
			self.dlgSystem.SetTop()
		else:
			self.dlgSystem.Close()

	def OpenSystemDialog(self):
		self.dlgSystem.OpenDialog()
		self.dlgSystem.SetTop()

	def ToggleMessenger(self):
		if self.wndMessenger.IsShow():
			self.wndMessenger.Hide()
		else:
			self.wndMessenger.SetTop()
			self.wndMessenger.Show()

	def ToggleMiniMap(self):
		if app.IsPressed(app.DIK_LSHIFT) or app.IsPressed(app.DIK_RSHIFT):
			if FALSE == self.wndMiniMap.isShowMiniMap():
				self.wndMiniMap.ShowMiniMap()
				self.wndMiniMap.SetTop()
			else:
				self.wndMiniMap.HideMiniMap()

		else:
			self.wndMiniMap.ToggleAtlasWindow()

	def PressMKey(self):
		if app.IsPressed(app.DIK_LALT) or app.IsPressed(app.DIK_RALT):
			self.ToggleMessenger()

		else:
			self.ToggleMiniMap()

	def SetMapName(self, mapName):
		self.wndMiniMap.SetMapName(mapName)

	def MiniMapScaleUp(self):
		self.wndMiniMap.ScaleUp()

	def MiniMapScaleDown(self):
		self.wndMiniMap.ScaleDown()

	def ToggleCharacterWindow(self, state):
		if FALSE == player.IsObserverMode():
			if FALSE == self.wndCharacter.IsShow():
				self.OpenCharacterWindowWithState(state)
			else:
				if state == self.wndCharacter.GetState():
					self.wndCharacter.OverOutItem()
					self.wndCharacter.Close()
				else:
					self.wndCharacter.SetState(state)

	def OpenCharacterWindowWithState(self, state):
		if FALSE == player.IsObserverMode():
			self.wndCharacter.SetState(state)
			self.wndCharacter.Show()
			self.wndCharacter.SetTop()

	def ToggleCharacterWindowStatusPage(self):
		self.ToggleCharacterWindow("STATUS")

	def ToggleInventoryWindow(self):
		if FALSE == player.IsObserverMode():
			if FALSE == self.wndInventory.IsShow():
				self.wndInventory.Show()
				self.wndInventory.SetTop()
			else:
				self.wndInventory.OverOutItem()
				self.wndInventory.Close()

	def ToggleExpandedButton(self):
		if FALSE == player.IsObserverMode():
			if FALSE == self.wndExpandedTaskBar.IsShow():
				self.wndExpandedTaskBar.Show()
				self.wndExpandedTaskBar.SetTop()
			else:
				self.wndExpandedTaskBar.Close()

	if app.ENABLE_HUNTING_SYSTEM:
		def ToggleHuntingWindow(self):
			if self.wndHunting.IsShow():
				self.wndHunting.Close()
			elif self.wndHuntingSelect.IsShow():
				self.wndHuntingSelect.Close()
			else:
				net.SendHuntingAction(1, 0)
				
		def OpenHuntingWindowMain(self, level, monster, cur_count, dest_count, money_min, money_max, exp_min, exp_max, race_item, race_item_count):
			if self.wndHunting:
				self.wndHunting.OpenMain(level, monster, cur_count, dest_count, money_min, money_max, exp_min, exp_max, race_item, race_item_count)
				self.wndHunting.SetTop()
				constInfo.HUNTING_BUTTON_FLASH = 0
				
		def OpenHuntingWindowSelect(self, level, type ,monster, count, money_min, money_max, exp_min, exp_max, race_item, race_item_count):
			if self.wndHunting and self.wndHunting.IsShow():
				self.wndHunting.CloseWithMini()
			self.wndHuntingSelect.OpenSelect(level, type ,monster, count, money_min, money_max, exp_min, exp_max, race_item, race_item_count)
			constInfo.HUNTING_BUTTON_FLASH = 0
			
		def OpenHuntingWindowReward(self, level, reward, reward_count, random_reward, random_reward_count, money, exp):
			if False == self.wndHuntingReward.IsShow():
				self.wndHuntingReward.OpenReward(level, reward, reward_count, random_reward, random_reward_count, money, exp)
				self.wndHuntingReward.SetTop()
				
		def UpdateHuntingMission(self, count):
			if self.wndHunting:
				self.wndHunting.UpdateMission(count)
		
		def HuntingSetRandomItemsMain(self, item_vnum, item_count):
			if self.wndHunting:
				self.wndHunting.SetRandomItemTable(item_vnum, item_count)
				BUILD_OpenWindow
		def HuntingSetRandomItemsSelect(self, item_vnum, item_count):
			if self.wndHuntingSelect:
				self.wndHuntingSelect.SetRandomItemTable(item_vnum, item_count)

	# ��ȥ��
	def DragonSoulActivate(self, deck):
		self.wndDragonSoul.ActivateDragonSoulByExtern(deck)

	def DragonSoulDeactivate(self):
		self.wndDragonSoul.DeactivateDragonSoul()
		
	def Highligt_Item(self, inven_type, inven_pos):
		if player.DRAGON_SOUL_INVENTORY == inven_type:
			self.wndDragonSoul.HighlightSlot(inven_pos)
		elif app.HIGHLIGHT_SLOT and player.SLOT_TYPE_INVENTORY == inven_type:
			self.wndInventory.HighlightSlot(inven_pos)

	def DragonSoulGiveQuilification(self):
		self.DRAGON_SOUL_IS_QUALIFIED = TRUE
		self.wndExpandedTaskBar.SetToolTipText(uiTaskBar.ExpandedTaskBar.BUTTON_DRAGON_SOUL, uiScriptLocale.TASKBAR_DRAGON_SOUL)

	def ToggleDragonSoulWindow(self):
		if FALSE == player.IsObserverMode():
			if FALSE == self.wndDragonSoul.IsShow():
				if self.DRAGON_SOUL_IS_QUALIFIED:
					self.wndDragonSoul.Show()
				else:
					try:
						self.wndPopupDialog.SetText(localeInfo.DRAGON_SOUL_UNQUALIFIED)
						self.wndPopupDialog.Open()
					except:
						self.wndPopupDialog = uiCommon.PopupDialog()
						self.wndPopupDialog.SetText(localeInfo.DRAGON_SOUL_UNQUALIFIED)
						self.wndPopupDialog.Open()
			else:
				self.wndDragonSoul.Close()
		
	def ToggleDragonSoulWindowWithNoInfo(self):
		if FALSE == player.IsObserverMode():
			if FALSE == self.wndDragonSoul.IsShow():
				if self.DRAGON_SOUL_IS_QUALIFIED:
					self.wndDragonSoul.Show()
			else:
				self.wndDragonSoul.Close()
				
	def FailDragonSoulRefine(self, reason, inven_type, inven_pos):
		if FALSE == player.IsObserverMode():
			if TRUE == self.wndDragonSoulRefine.IsShow():
				self.wndDragonSoulRefine.RefineFail(reason, inven_type, inven_pos)
 
	def SucceedDragonSoulRefine(self, inven_type, inven_pos):
		if FALSE == player.IsObserverMode():
			if TRUE == self.wndDragonSoulRefine.IsShow():
				self.wndDragonSoulRefine.RefineSucceed(inven_type, inven_pos)
 
	def OpenDragonSoulRefineWindow(self):
		if FALSE == player.IsObserverMode():
			if FALSE == self.wndDragonSoulRefine.IsShow():
				self.wndDragonSoulRefine.Show()
				if None != self.wndDragonSoul:
					if FALSE == self.wndDragonSoul.IsShow():
						self.wndDragonSoul.Show()

	def CloseDragonSoulRefineWindow(self):
		if FALSE == player.IsObserverMode():
			if TRUE == self.wndDragonSoulRefine.IsShow():
				self.wndDragonSoulRefine.Close()

	# ��ȥ�� ��
	
	def ToggleGuildWindow(self):
		if not self.wndGuild.IsShow():
			if self.wndGuild.CanOpen():
				self.wndGuild.Open()
			else:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GUILD_YOU_DO_NOT_JOIN)
		else:
			self.wndGuild.OverOutItem()
			self.wndGuild.Hide()

	def ToggleChatLogWindow(self):
		if self.wndChatLog.IsShow():
			self.wndChatLog.Hide()
		else:
			self.wndChatLog.Show()

	def CheckGameButton(self):
		if self.wndGameButton:
			self.wndGameButton.CheckGameButton()

	def __OnClickStatusPlusButton(self):
		self.ToggleCharacterWindow("STATUS")

	def __OnClickSkillPlusButton(self):
		self.ToggleCharacterWindow("SKILL")

	def __OnClickQuestButton(self):
		self.ToggleCharacterWindow("QUEST")

	def __OnClickHelpButton(self):
		player.SetPlayTime(1)
		self.CheckGameButton()
		self.OpenHelpWindow()

	def __OnClickBuildButton(self):
		self.BUILD_OpenWindow()

	def OpenHelpWindow(self):
		self.wndUICurtain.Show()
		self.wndHelp.Open()

	def CloseHelpWindow(self):
		self.wndUICurtain.Hide()
		self.wndHelp.Close()

	def OpenWebWindow(self, url):
		self.wndWeb.Open(url)

		# ���������� ���� ä���� �ݴ´�
		self.wndChat.CloseChat()

	# show GIFT
	def ShowGift(self):
		self.wndTaskBar.ShowGift()
	    	
	def CloseWbWindow(self):
		self.wndWeb.Close()

	def OpenCubeWindow(self):
		self.wndCube.Open()

		if FALSE == self.wndInventory.IsShow():
			self.wndInventory.Show()

	def UpdateCubeInfo(self, gold, itemVnum, count):
		self.wndCube.UpdateInfo(gold, itemVnum, count)

	def CloseCubeWindow(self):
		self.wndCube.Close()

	def FailedCubeWork(self):
		self.wndCube.Refresh()

	def SucceedCubeWork(self, itemVnum, count):
		self.wndCube.Clear()
		
		print "ť�� ���� ����! [%d:%d]" % (itemVnum, count)

		if 0: # ��� �޽��� ����� ���� �Ѵ�
			self.wndCubeResult.SetPosition(*self.wndCube.GetGlobalPosition())
			self.wndCubeResult.SetCubeResultItem(itemVnum, count)
			self.wndCubeResult.Open()
			self.wndCubeResult.SetTop()

	if app.ENABLE_SWITCHBOT:
		def ToggleSwitchbotWindow(self):
			if self.wndSwitchbot.IsShow():
				self.wndSwitchbot.Close()
			else:
				self.wndSwitchbot.Open()
				
		def RefreshSwitchbotWindow(self):
			if self.wndSwitchbot and self.wndSwitchbot.IsShow():
				self.wndSwitchbot.RefreshSwitchbotWindow()

		def RefreshSwitchbotItem(self, slot):
			if self.wndSwitchbot and self.wndSwitchbot.IsShow():
				self.wndSwitchbot.RefreshSwitchbotItem(slot)

	def __HideWindows(self):
		hideWindows = self.wndTaskBar,\
						self.wndCharacter,\
						self.wndInventory,\
						self.wndMiniMap,\
						self.wndGuild,\
						self.wndMessenger,\
						self.wndChat,\
						self.wndParty,\
						self.wndDragonSoul,\
						self.wndDragonSoulRefine, \
						self.wndGameButton,

		if self.wndEnergyBar:
			hideWindows += self.wndEnergyBar,
 			
		if self.wndExpandedTaskBar:
			hideWindows += self.wndExpandedTaskBar,

		if app.ENABLE_SWITCHBOT and self.wndSwitchbot:
			hideWindows += self.wndSwitchbot,

		if app.ENABLE_HUNTING_SYSTEM:
			if self.wndHunting:
				hideWindows += self.wndHunting,
			if self.wndHuntingSelect:
				hideWindows += self.wndHuntingSelect,
			if self.wndHuntingReward:
				hideWindows += self.wndHuntingReward,

		if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
			if self.wndBuffNPCWindow:
				hideWindows += self.wndBuffNPCWindow,
			if self.wndBuffNPCCreateWindow:
				hideWindows += self.wndBuffNPCCreateWindow,

		if app.ENABLE_ITEMSHOP and self.wndItemshop:
			hideWindows += self.wndItemshop,

		hideWindows = filter(lambda x:x.IsShow(), hideWindows)
		map(lambda x:x.Hide(), hideWindows)
		import sys

		self.HideAllQuestButton()
		self.HideAllWhisperButton()

		if self.wndChat.IsEditMode():
			self.wndChat.CloseChat()

		return hideWindows

	def __ShowWindows(self, wnds):
		import sys
		map(lambda x:x.Show(), wnds)
		global IsQBHide
		if not IsQBHide:
			self.ShowAllQuestButton()
		else:
			self.HideAllQuestButton()

		self.ShowAllWhisperButton()

	def BINARY_OpenAtlasWindow(self):
		if self.wndMiniMap:
			self.wndMiniMap.ShowAtlas()

	def BINARY_SetObserverMode(self, flag):
		self.wndGameButton.SetObserverMode(flag)

	# ACCESSORY_REFINE_ADD_METIN_STONE
	def BINARY_OpenSelectItemWindow(self):
		self.wndItemSelect.Open()
	# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE

	#####################################################################################
	### Private Shop ###

	def OpenPrivateShopInputNameDialog(self):
		#if player.IsInSafeArea():
		#	chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CANNOT_OPEN_PRIVATE_SHOP_IN_SAFE_AREA)
		#	return

		inputDialog = uiCommon.InputDialog()
		inputDialog.SetTitle(localeInfo.PRIVATE_SHOP_INPUT_NAME_DIALOG_TITLE)
		inputDialog.SetMaxLength(32)
		inputDialog.SetAcceptEvent(ui.__mem_func__(self.OpenPrivateShopBuilder))
		inputDialog.SetCancelEvent(ui.__mem_func__(self.ClosePrivateShopInputNameDialog))
		inputDialog.Open()
		self.inputDialog = inputDialog

	def ClosePrivateShopInputNameDialog(self):
		self.inputDialog = None
		return TRUE

	def OpenPrivateShopBuilder(self):

		if not self.inputDialog:
			return TRUE

		if not len(self.inputDialog.GetText()):
			return TRUE

		self.privateShopBuilder.Open(self.inputDialog.GetText())
		self.ClosePrivateShopInputNameDialog()
		return TRUE

	def AppearPrivateShop(self, vid, text):

		board = uiPrivateShopBuilder.PrivateShopAdvertisementBoard()
		board.Open(vid, text)

		self.privateShopAdvertisementBoardDict[vid] = board

	def DisappearPrivateShop(self, vid):

		if not self.privateShopAdvertisementBoardDict.has_key(vid):
			return

		del self.privateShopAdvertisementBoardDict[vid]
		uiPrivateShopBuilder.DeleteADBoard(vid)

	#####################################################################################
	### Equipment ###

	def OpenEquipmentDialog(self, vid):
		dlg = uiEquipmentDialog.EquipmentDialog()
		dlg.SetItemToolTip(self.tooltipItem)
		dlg.SetCloseEvent(ui.__mem_func__(self.CloseEquipmentDialog))
		dlg.Open(vid)

		self.equipmentDialogDict[vid] = dlg

	def SetEquipmentDialogItem(self, vid, slotIndex, vnum, count):
		if not vid in self.equipmentDialogDict:
			return
		self.equipmentDialogDict[vid].SetEquipmentDialogItem(slotIndex, vnum, count)

	def SetEquipmentDialogSocket(self, vid, slotIndex, socketIndex, value):
		if not vid in self.equipmentDialogDict:
			return
		self.equipmentDialogDict[vid].SetEquipmentDialogSocket(slotIndex, socketIndex, value)

	def SetEquipmentDialogAttr(self, vid, slotIndex, attrIndex, type, value):
		if not vid in self.equipmentDialogDict:
			return
		self.equipmentDialogDict[vid].SetEquipmentDialogAttr(slotIndex, attrIndex, type, value)

	def CloseEquipmentDialog(self, vid):
		if not vid in self.equipmentDialogDict:
			return
		del self.equipmentDialogDict[vid]

	#####################################################################################

	#####################################################################################
	### Quest ###	
	def BINARY_ClearQuest(self, index):
		btn = self.__FindQuestButton(index)
		if 0 != btn:
			self.__DestroyQuestButton(btn)		
	
	def RecvQuest(self, index, name):
		# QUEST_LETTER_IMAGE
		self.BINARY_RecvQuest(index, name, "file", localeInfo.GetLetterImageName())
		# END_OF_QUEST_LETTER_IMAGE

	def BINARY_RecvQuest(self, index, name, iconType, iconName):

		btn = self.__FindQuestButton(index)
		if 0 != btn:
			self.__DestroyQuestButton(btn)

		btn = uiWhisper.WhisperButton()

		# QUEST_LETTER_IMAGE
		##!! 20061026.levites.����Ʈ_�̹���_��ü
		import item
		if "item"==iconType:
			item.SelectItem(int(iconName))
			buttonImageFileName=item.GetIconImageFileName()
		else:
			buttonImageFileName=iconName

		btn.SetUpVisual(buttonImageFileName)
		btn.SetOverVisual(buttonImageFileName)
		btn.SetDownVisual(buttonImageFileName)
	#	btn.Flash() ## Buttons blinken
		btn.SetToolTipText(name, -20, 35)
		btn.ToolTipText.SetHorizontalAlignLeft()
		btn.SetEvent(ui.__mem_func__(self.__StartQuest), btn)
		btn.Show()

		btn.index = index
		btn.name = name

		self.questButtonList.insert(0, btn)
		self.__ArrangeQuestButton()

		#chat.AppendChat(chat.CHAT_TYPE_NOTICE, localeInfo.QUEST_APPEND)

	def __ArrangeQuestButton(self):

		screenWidth = wndMgr.GetScreenWidth()
		screenHeight = wndMgr.GetScreenHeight()

		##!! 20061026.levites.����Ʈ_��ġ_����
		if self.wndParty.IsShow():
			xPos = 100 + 30
		else:
			xPos = 20

		yPos = 170 * screenHeight / 600
		yCount = (screenHeight - 330) / 63

		count = 0
		for btn in self.questButtonList:
			btn.SetPosition(xPos + (int(count/yCount) * 100), yPos + (count%yCount * 63))
			count += 1
			global IsQBHide
			if IsQBHide:
				btn.Hide()
			else:
				btn.Show()

	def __StartQuest(self, btn):
		event.QuestButtonClick(btn.index)
		self.__DestroyQuestButton(btn)

	def __FindQuestButton(self, index):
		for btn in self.questButtonList:
			if btn.index == index:
				return btn

		return 0

	def __DestroyQuestButton(self, btn):
		btn.SetEvent(0)
		self.questButtonList.remove(btn)
		self.__ArrangeQuestButton()

	def HideAllQuestButton(self):
		for btn in self.questButtonList:
			btn.Hide()

	def ShowAllQuestButton(self):
		for btn in self.questButtonList:
			btn.Show()

	def CheckRefineDialog(self, isFail):
		self.dlgRefineNew.CheckRefine(isFail)

	def __InitWhisper(self):
		chat.InitWhisper(self)

	## ä��â�� "�޽��� ������"�� �������� �̸� ���� ��ȭâ�� ���� �Լ�
	## �̸��� ���� ������ ������ WhisperDialogDict �� ������ �����ȴ�.
	def OpenWhisperDialogWithoutTarget(self):
		if not self.dlgWhisperWithoutTarget:
			dlgWhisper = uiWhisper.WhisperDialog(self.MinimizeWhisperDialog, self.CloseWhisperDialog)
			dlgWhisper.BindInterface(self)
			dlgWhisper.LoadDialog()
			dlgWhisper.OpenWithoutTarget(self.RegisterTemporaryWhisperDialog)
			dlgWhisper.SetPosition(self.windowOpenPosition*30,self.windowOpenPosition*30)
			dlgWhisper.Show()
			self.dlgWhisperWithoutTarget = dlgWhisper

			self.windowOpenPosition = (self.windowOpenPosition+1) % 5

		else:
			self.dlgWhisperWithoutTarget.SetTop()
			self.dlgWhisperWithoutTarget.OpenWithoutTarget(self.RegisterTemporaryWhisperDialog)

	## �̸� ���� ��ȭâ���� �̸��� ���������� WhisperDialogDict�� â�� �־��ִ� �Լ�
	def RegisterTemporaryWhisperDialog(self, name):
		if not self.dlgWhisperWithoutTarget:
			return

		btn = self.__FindWhisperButton(name)
		if 0 != btn:
			self.__DestroyWhisperButton(btn)

		elif self.whisperDialogDict.has_key(name):
			oldDialog = self.whisperDialogDict[name]
			oldDialog.Destroy()
			del self.whisperDialogDict[name]

		self.whisperDialogDict[name] = self.dlgWhisperWithoutTarget
		self.dlgWhisperWithoutTarget.OpenWithTarget(name)
		self.dlgWhisperWithoutTarget = None
		self.__CheckGameMaster(name)

	## ĳ���� �޴��� 1:1 ��ȭ �ϱ⸦ �������� �̸��� ������ �ٷ� â�� ���� �Լ�
	def OpenWhisperDialog(self, name):
		if not self.whisperDialogDict.has_key(name):
			dlg = self.__MakeWhisperDialog(name)
			dlg.OpenWithTarget(name)
			dlg.chatLine.SetFocus()
			dlg.Show()

			self.__CheckGameMaster(name)
			btn = self.__FindWhisperButton(name)
			if 0 != btn:
				self.__DestroyWhisperButton(btn)

	## �ٸ� ĳ���ͷκ��� �޼����� �޾����� �ϴ� ��ư�� ��� �δ� �Լ�
	def RecvWhisper(self, name):
		if not self.whisperDialogDict.has_key(name):
			btn = self.__FindWhisperButton(name)
			if 0 == btn:
				btn = self.__MakeWhisperButton(name)
				btn.Flash()

				chat.AppendChat(chat.CHAT_TYPE_NOTICE, localeInfo.RECEIVE_MESSAGE % (name))

			else:
				btn.Flash()
		elif self.IsGameMasterName(name):
			dlg = self.whisperDialogDict[name]
			dlg.SetGameMasterLook()

	def MakeWhisperButton(self, name):
		self.__MakeWhisperButton(name)

	## ��ư�� �������� â�� ���� �Լ�
	def ShowWhisperDialog(self, btn):
		try:
			self.__MakeWhisperDialog(btn.name)
			dlgWhisper = self.whisperDialogDict[btn.name]
			dlgWhisper.OpenWithTarget(btn.name)
			dlgWhisper.Show()
			self.__CheckGameMaster(btn.name)
		except:
			import dbg
			dbg.TraceError("interface.ShowWhisperDialog - Failed to find key")

		## ��ư �ʱ�ȭ
		self.__DestroyWhisperButton(btn)

	## WhisperDialog â���� �ּ�ȭ ����� ���������� ȣ��Ǵ� �Լ�
	## â�� �ּ�ȭ �մϴ�.
	def MinimizeWhisperDialog(self, name):

		if 0 != name:
			self.__MakeWhisperButton(name)

		self.CloseWhisperDialog(name)

	## WhisperDialog â���� �ݱ� ����� ���������� ȣ��Ǵ� �Լ�
	## â�� ����ϴ�.
	def CloseWhisperDialog(self, name):

		if 0 == name:

			if self.dlgWhisperWithoutTarget:
				self.dlgWhisperWithoutTarget.Destroy()
				self.dlgWhisperWithoutTarget = None

			return

		try:
			dlgWhisper = self.whisperDialogDict[name]
			dlgWhisper.Destroy()
			del self.whisperDialogDict[name]
		except:
			import dbg
			dbg.TraceError("interface.CloseWhisperDialog - Failed to find key")

	## ��ư�� ������ �ٲ������ ��ư�� ������ �ϴ� �Լ�
	def __ArrangeWhisperButton(self):

		screenWidth = wndMgr.GetScreenWidth()
		screenHeight = wndMgr.GetScreenHeight()

		xPos = screenWidth - 70
		yPos = 170 * screenHeight / 600
		yCount = (screenHeight - 330) / 63
		#yCount = (screenHeight - 285) / 63

		count = 0
		for button in self.whisperButtonList:

			button.SetPosition(xPos + (int(count/yCount) * -50), yPos + (count%yCount * 63))
			count += 1

	## �̸����� Whisper ��ư�� ã�� ������ �ִ� �Լ�
	## ��ư�� ��ųʸ��� ���� �ʴ� ���� ���� �Ǿ� ���� ������ ���� ���� ������
	## �̷� ���� ToolTip���� �ٸ� ��ư�鿡 ���� �������� �����̴�.
	def __FindWhisperButton(self, name):
		for button in self.whisperButtonList:
			if button.name == name:
				return button

		return 0

	## â�� ����ϴ�.
	def __MakeWhisperDialog(self, name):
		dlgWhisper = uiWhisper.WhisperDialog(self.MinimizeWhisperDialog, self.CloseWhisperDialog)
		dlgWhisper.BindInterface(self)
		dlgWhisper.LoadDialog()
		dlgWhisper.SetPosition(self.windowOpenPosition*30,self.windowOpenPosition*30)
		self.whisperDialogDict[name] = dlgWhisper

		self.windowOpenPosition = (self.windowOpenPosition+1) % 5

		return dlgWhisper

	## ��ư�� ����ϴ�.
	def __MakeWhisperButton(self, name):
		whisperButton = uiWhisper.WhisperButton()
		whisperButton.SetUpVisual("d:/ymir work/ui/game/windows/btn_mail_up.sub")
		whisperButton.SetOverVisual("d:/ymir work/ui/game/windows/btn_mail_up.sub")
		whisperButton.SetDownVisual("d:/ymir work/ui/game/windows/btn_mail_up.sub")
		if self.IsGameMasterName(name):
			whisperButton.SetToolTipTextWithColor(name, 0xffffa200)
		else:
			whisperButton.SetToolTipText(name)
		whisperButton.ToolTipText.SetHorizontalAlignCenter()
		whisperButton.SetEvent(ui.__mem_func__(self.ShowWhisperDialog), whisperButton)
		whisperButton.Show()
		whisperButton.name = name

		self.whisperButtonList.insert(0, whisperButton)
		self.__ArrangeWhisperButton()

		return whisperButton

	def __DestroyWhisperButton(self, button):
		button.SetEvent(0)
		self.whisperButtonList.remove(button)
		self.__ArrangeWhisperButton()

	def HideAllWhisperButton(self):
		for btn in self.whisperButtonList:
			btn.Hide()

	def ShowAllWhisperButton(self):
		for btn in self.whisperButtonList:
			btn.Show()

	def __CheckGameMaster(self, name):
		if not self.listGMName.has_key(name):
			return
		if self.whisperDialogDict.has_key(name):
			dlg = self.whisperDialogDict[name]
			dlg.SetGameMasterLook()

	def RegisterGameMasterName(self, name):
		if self.listGMName.has_key(name):
			return
		self.listGMName[name] = "GM"

	def IsGameMasterName(self, name):
		if self.listGMName.has_key(name):
			return TRUE
		else:
			return FALSE

	#####################################################################################

	#####################################################################################
	### Guild Building ###

	def BUILD_OpenWindow(self):
		self.wndGuildBuilding = uiGuild.BuildGuildBuildingWindow()
		self.wndGuildBuilding.Open()
		self.wndGuildBuilding.wnds = self.__HideWindows()
		self.wndGuildBuilding.SetCloseEvent(ui.__mem_func__(self.BUILD_CloseWindow))

	def BUILD_CloseWindow(self):
		self.__ShowWindows(self.wndGuildBuilding.wnds)
		self.wndGuildBuilding = None

	def BUILD_OnUpdate(self):
		if not self.wndGuildBuilding:
			return

		if self.wndGuildBuilding.IsPositioningMode():
			import background
			x, y, z = background.GetPickingPoint()
			self.wndGuildBuilding.SetBuildingPosition(x, y, z)

	def BUILD_OnMouseLeftButtonDown(self):
		if not self.wndGuildBuilding:
			return

		# GUILD_BUILDING
		if self.wndGuildBuilding.IsPositioningMode():
			self.wndGuildBuilding.SettleCurrentPosition()
			return TRUE
		elif self.wndGuildBuilding.IsPreviewMode():
			pass
		else:
			return TRUE
		# END_OF_GUILD_BUILDING
		return FALSE

	def BUILD_OnMouseLeftButtonUp(self):
		if not self.wndGuildBuilding:
			return

		if not self.wndGuildBuilding.IsPreviewMode():
			return TRUE

		return FALSE

	def BULID_EnterGuildArea(self, areaID):
		# GUILD_BUILDING
		mainCharacterName = player.GetMainCharacterName()
		masterName = guild.GetGuildMasterName()

		if mainCharacterName != masterName:
			return

		if areaID != player.GetGuildID():
			return
		# END_OF_GUILD_BUILDING

		self.wndGameButton.ShowBuildButton()

	def BULID_ExitGuildArea(self, areaID):
		self.wndGameButton.HideBuildButton()

	if app.ENABLE_CHEST_DROP:
		def OpenChestDropWindow(self, itemVnum):
			if self.wndChestDropInfo:
				self.wndChestDropInfo.Open(itemVnum)

	def IsEditLineFocus(self):
		if self.ChatWindow.chatLine.IsFocus():
			return 1

		if self.ChatWindow.chatToLine.IsFocus():
			return 1

		return 0

	def EmptyFunction(self):
		pass

	if app.WJ_ENABLE_TRADABLE_ICON:
		def SetOnTopWindow(self, onTopWnd):
			self.onTopWindow = onTopWnd

		def GetOnTopWindow(self):
			return self.onTopWindow

		def RefreshMarkInventoryBag(self):
			self.wndInventory.RefreshMarkSlots()

	if app.ENABLE_ITEMSHOP:
		def ToggleItemshop(self):
			if self.wndItemshop.IsShow():
				self.wndItemshop.Close()
			else:
				self.wndItemshop.Open()

		def OpenItemshop(self):
			if not self.wndItemshop.IsShow():
				self.wndItemshop.Open()

		def ReloadItemshop(self):
			if self.wndItemshop:
				self.wndItemshop.ReloadItemshop()

		def ReloadItemshopItem(self, item):
			if self.wndItemshop:
				self.wndItemshop.ReloadItemshopItem(item)
				self.wndItemshop.ReloadItemshopContainer(item)

		def RemoveItemshopItem(self, hash):
			if self.wndItemshop:
				self.wndItemshop.RemoveItemshopItem(hash)
				self.wndItemshop.RemoveItemFromItemshopContainer(hash)

		def AddItemshopItem(self, obj):
			if self.wndItemshop:
				self.wndItemshop.AddItemshopItem(obj)

		def SetCoins(self, coins):
			if self.wndItemshop:
				self.wndItemshop.SetCoins(coins)

		def StartPromotionAnimation(self, answer):
			if self.wndItemshop:
				self.wndItemshop.StartPromotionAnimation(answer)

	if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
		def BuffNPC_OpenCreateWindow(self):
			if self.wndBuffNPCWindow:
				if False == self.wndBuffNPCCreateWindow.IsShow():
					self.wndBuffNPCCreateWindow.Show()
					self.wndBuffNPCCreateWindow.SetTop()
				
		def BuffNPCOpenWindow(self):
			if self.wndBuffNPCWindow:
				if False == self.wndBuffNPCWindow.IsShow():
					self.wndBuffNPCWindow.Show()
					self.wndBuffNPCWindow.SetTop()
				else:
					self.wndBuffNPCWindow.Close()
				
		def BuffNPC_Summon(self):
			if self.wndBuffNPCWindow:
				self.wndBuffNPCWindow.SetSummon()
				self.wndBuffNPCWindow.Show()
				self.wndBuffNPCWindow.SetTop()
				
		def BuffNPC_Unsummon(self):
			if self.wndBuffNPCWindow:
				self.wndBuffNPCWindow.SetUnsummon()
				
		def BuffNPC_Clear(self):
			if self.wndBuffNPCWindow:
				self.wndBuffNPCWindow.SetClear()
				
		def BuffNPC_SetBasicInfo(self, name, sex, intvalue):
			if self.wndBuffNPCWindow:
				self.wndBuffNPCWindow.SetBasicInfo(name, sex, intvalue)
				
		def BuffNPC_SetEXPInfo(self, level, cur_exp, exp):
			if self.wndBuffNPCWindow:
				self.wndBuffNPCWindow.SetEXPInfo(level, cur_exp, exp)
				
		def BuffNPC_SetSkillInfo(self, skill1, skill2, skill3, skill4, skill5, skill6, skillpoints):
			if self.wndBuffNPCWindow:
				self.wndBuffNPCWindow.SetSkillInfo(skill1, skill2, skill3, skill4, skill5, skill6, skillpoints)
				
		def BuffNPC_SkillUseStatus(self, slot0, slot1, slot2, slot3, slot4, slot5):
			if self.wndBuffNPCWindow:
				self.wndBuffNPCWindow.SetSkillUseStatus(slot0, slot1, slot2, slot3, slot4, slot5)
				
		def BuffNPC_SetSkillCooltime(self, slot, timevalue):
			if self.wndBuffNPCWindow:
				self.wndBuffNPCWindow.SetSkillCooltime(slot, timevalue)
		
		def BuffNPC_CreatePopup(self, type, value0, value1):
			if self.wndBuffNPCWindow:
				self.wndBuffNPCWindow.CreatePopup(type, value0, value1)
		
	if app.WJ_ENABLE_TRADABLE_ICON:
		def SetOnTopWindow(self, onTopWnd):
			self.onTopWindow = onTopWnd

		def GetOnTopWindow(self):
			return self.onTopWindow

		def RefreshMarkInventoryBag(self):
			self.wndInventory.RefreshMarkSlots()
