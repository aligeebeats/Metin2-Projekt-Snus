import os
import ui
import net
import app
import snd
import chat
import grp
import ime
import wndMgr
import mouseModule
import uiScriptLocale
import renderTarget
import localeInfo
import constInfo
import uiCommon
import uiToolTip
import player
import item
import skill
import chat

POPUP_TEXT_GRANDMASTER_SUCCESS = [uiScriptLocale.ASLAN_BUFF_POPUP_GRANDMASTER_SUCCESS_1, uiScriptLocale.ASLAN_BUFF_POPUP_GRANDMASTER_SUCCESS_2]
POPUP_TEXT_GRANDMASTER_NOT_SUCCESS = [uiScriptLocale.ASLAN_BUFF_POPUP_GRANDMASTER_NOT_SUCCESS_1, uiScriptLocale.ASLAN_BUFF_POPUP_GRANDMASTER_NOT_SUCCESS_2]
POPUP_TEXT_GRANDMASTER_NO_SKILLS = [uiScriptLocale.ASLAN_BUFF_POPUP_GRANDMASTER_NO_SKILLS]
POPUP_TEXT_NOT_ACTIVE_SHAMAN = [uiScriptLocale.ASLAN_BUFF_POPUP_NOT_ACTIVE_SHAMAN]
POPUP_TEXT_RESET_SKILL = [uiScriptLocale.ASLAN_BUFF_POPUP_RESET_SKILL_1, uiScriptLocale.ASLAN_BUFF_POPUP_RESET_SKILL_2]

QUESTION_ASK_BEFORE_EXPORT = [uiScriptLocale.ASLAN_BUFF_QUESTION_BEFORE_EXPORT_1, uiScriptLocale.ASLAN_BUFF_QUESTION_BEFORE_EXPORT_2, uiScriptLocale.ASLAN_BUFF_QUESTION_BEFORE_EXPORT_3]
QUESTION_ASK_BEFORE_CHANGE_SEX = [uiScriptLocale.ASLAN_BUFF_QUESTION_BEFORE_CHANGE_SEX]

SKILL_POWER = [0.00, 
0.05, 0.06, 0.08, 0.10, 0.12, 0.14, 0.16, 0.18, 0.20, 0.22, # 10
0.24, 0.26, 0.28, 0.30, 0.32, 0.34, 0.36, 0.38, 0.40, 0.50,  # 20
0.52, 0.54, 0.56, 0.58, 0.60, 0.63, 0.66, 0.69, 0.72, 0.82,  # 30
0.85, 0.88, 0.91, 0.94, 0.98, 1.02, 1.06, 1.10, 1.15, 1.25,  # 40
1.25, 1.25, 1.25, 1.25]

LEARN_GRANDMASTER_SKILL_COST = [
	500000,
	1000000,
	2000000,
	3000000,
	5000000,
	7500000,
	12000000,
	15000000,
	20000000,
	25000000,
]

EXPORT_COST = [
# level_min | level_max | cost
	[  0, 10, 50000 ],				# Lv.	 0-10
	[ 11, 20, 250000 ],			# Lv. 11-20
	[ 21, 30, 1000000 ],			# Lv. 21-30
	[ 31, 40, 2500000 ],			# Lv. 31-40
	[ 41, 50, 7500000 ],			# Lv. 41-50
	[ 51, 60, 15000000 ],		# Lv. 51-60
	[ 61, 70, 30000000 ],		# Lv. 61-70
	[ 71, 80, 50000000 ],		# Lv. 71-80
	[ 81, 90, 100000000 ],		# Lv. 81-90
	[ 91, 99, 250000000 ],		# Lv. 91-99
]

IMG_PATH = "d:/ymir work/ui/aslan/buffnpc/"

TARGET_RENDER_INDEX_FIRST_USE = 22

class BuffNPCWindow(ui.ScriptWindow):

	class TextToolTip(ui.Window):
		def __init__(self, y):
			ui.Window.__init__(self, "TOP_MOST")

			textLine = ui.TextLine()
			textLine.SetParent(self)
			textLine.SetHorizontalAlignLeft()
			textLine.SetOutline()
			textLine.Show()
			self.y = y
			self.textLine = textLine

		def __del__(self):
			ui.Window.__del__(self)

		def SetText(self, text):
			self.textLine.SetText(text)

		def OnRender(self):
			(mouseX, mouseY) = wndMgr.GetMousePosition()
			self.textLine.SetPosition(mouseX - 30, mouseY - 30 + self.y)

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		
		self.tabDict = None
		self.tabButtonDict = None
		self.pageDict = None
		self.wndSelectSkill =  None
		self.wndReName = None
		
		self.dlgInfoDialog = InfoDialog()
		
		self.upgradeBookSlotVnum = 0
		self.showUpgradeGrandmasterSkill = -1
		self.canUpgradeGrandmasterSkillList = []
		self.skill_arrow_page = 0
		
		self.toolTipSkill = 0
		self.toolTipExp = None
		
		self.isActive = False
		self.level = 0
		self.sex = 0
		self.intPoints = 0
		self.skillPoints = 0
		self.expInfo = [0, 0]
		self.skillInfo = [[94, 0], [95, 0], [96, 0], [109, 0], [110, 0], [111, 0]]
		self.skillActiveStatus = [0, 0, 0, 0, 0, 0]
		
		self.__LoadWindow()

	def __del__(self):
		if self.wndSelectSkill:
			del self.wndSelectSkill
		if self.wndReName:
			del self.wndReName
		if self.dlgInfoDialog:
			del self.dlgInfoDialog
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.RefreshEquipSlotWindow()
		ui.ScriptWindow.Show(self)
			
	def Close(self):
		if self.wndSelectSkill:
			self.wndSelectSkill.Close()
		if self.wndReName:
			self.wndReName.Destroy()
		if self.dlgInfoDialog:
			self.dlgInfoDialog.Destroy()
		self.Hide()
	
	def Destroy(self):
		if self.wndSelectSkill:
			self.wndSelectSkill.Close()
		if self.wndReName:
			self.wndReName.Destroy()
		if self.dlgInfoDialog:
			self.dlgInfoDialog.Destroy()
		self.Hide()
		self.ClearDictionary()
		
	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/buffnpcwindow.py")
		except:
			import exception
			exception.Abort("BuffNPCWindow.LoadWindow.LoadObject")
			
		try:
			self.toolTip 	= uiToolTip.ItemToolTip()
			self.wndSelectSkill = BuffNPCSelectSkillWindow()
			self.wndReName = BuffNPCReNameWindow()
			
			self.GetChild("board").SetCloseEvent(self.Close)
			self.skill_slots = self.GetChild("skill_slots")
			self.preview_upgrade_skill_slot = self.GetChild("preview_upgrade_skill_slot")
			self.eq_slots = self.GetChild("equipment_slots")
			self.upgrade_slots = self.GetChild("upgrade_skill_slots")
			self.change_slots = self.GetChild("change_slots")
			self.GetChild("summon_button").SetEvent(self.ClickSummonButton)

			if app.ASLAN_BUFF_NPC_ENABLE_EXPORT:
				self.GetChild("export_button").SetEvent(self.ClickExportButton)
			else:
				self.GetChild("export_button").Hide()

			self.GetChild("upgrade_skill_button").SetEvent(self.ClickUpgradeSkillButton)
			self.GetChild("change_name_button").SetEvent(self.ClickReNameButton)
			self.GetChild("change_sex_button").SetEvent(self.ClickChangeSexButton)
			self.GetChild("reset_skill_button").SetEvent(self.ClickResetSkillButton)
			
			self.GetChild("arrow_left_button").SetEvent(self.ClickSkillArrowLeft)
			self.GetChild("arrow_right_button").SetEvent(self.ClickSkillArrowRight)
			
			self.GetChild("arrow_left_button").Hide()
			self.GetChild("arrow_right_button").Hide()
			
			self.GetChild("face_icon_0").Hide()
			self.GetChild("face_icon_1").Hide()
			
			for i in xrange(6):
				self.GetChild("skill_active_icon_%d" % i).Hide()
				
			self.exp_gauges = [self.GetChild("exp_gauge_01"), self.GetChild("exp_gauge_02"), self.GetChild("exp_gauge_03"), self.GetChild("exp_gauge_04")]
			for i in xrange(4):
				self.exp_gauges[i].SetSize(0, 0)
				self.exp_gauges[i].Hide()
				
			self.tabDict = {
				"MAIN"			: self.GetChild("Tab_01"),
				"COMMON"	: self.GetChild("Tab_02"),
			}

			self.tabButtonDict = {
				"MAIN"			: self.GetChild("Tab_Button_01"),
				"COMMON"	: self.GetChild("Tab_Button_02"),
			}

			self.pageDict = {
				"MAIN"			: self.GetChild("Page_01"),
				"COMMON"	: self.GetChild("Page_02"),
			}
			
			for (tabKey, tabButton) in self.tabButtonDict.items():
				tabButton.SetEvent(ui.__mem_func__(self.__OnClickTabButton), tabKey)
			
			self.eq_slots.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
			self.eq_slots.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
			self.eq_slots.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
			self.eq_slots.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			self.eq_slots.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

			self.upgrade_slots.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
			self.upgrade_slots.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
			self.upgrade_slots.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
			self.upgrade_slots.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			self.upgrade_slots.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			
			self.change_slots.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
			self.change_slots.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
			self.change_slots.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
			self.change_slots.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			self.change_slots.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			
			self.state = "MAIN"
			self.SetTabState("MAIN")
			
		except:
			import exception
			exception.Abort("BuffNPCWindow.LoadWindow.BindObject")
		
	def __OnClickTabButton(self, stateKey):
		self.SetTabState(stateKey)

	def SetTabState(self, stateKey):
		self.state = stateKey

		for (tabKey, tabButton) in self.tabButtonDict.items():
			if stateKey!=tabKey:
				tabButton.SetUp()

		for tabValue in self.tabDict.itervalues():
			tabValue.Hide()

		for pageValue in self.pageDict.itervalues():
			pageValue.Hide()
			
		self.tabDict[stateKey].Show()
		self.pageDict[stateKey].Show()
		
	def GetTabState(self):
		return self.state
		
	def ClickSummonButton(self):
		if self.isActive == True:
			net.SendBuffNPCAction(1, 0, 0, 0, 0)
		else:
			self.CreatePopup("not_active", 0, 0)

	def OnPressedSlotButton(self, skillslot):
		if self.skillPoints != 0:
			net.SendBuffNPCAction(2, skillslot, 0, 0, 0)
			self.SetSkillSlots()
			
	def ClickUpgradeSkillButton(self):
		if self.isActive == True:
			if self.upgradeBookSlotVnum != 0 and self.upgradeBookSlotVnum != 50513:
				net.SendBuffNPCAction(3, self.upgradeBookSlotVnum, 0, 0, 0)
			elif self.upgradeBookSlotVnum == 50513:
				for slot in xrange(6):
					if self.GetSkillGrade(self.skillInfo[slot][1]) == 2:
						if self.wndSelectSkill.IsShow():
							self.wndSelectSkill.Close()
						self.wndSelectSkill.SetSkillToolTip(self.toolTipSkill)
						self.wndSelectSkill.OpenSkillSelect(0, self.skillInfo, self.intPoints)
						self.wndSelectSkill.Show()
						self.wndSelectSkill.SetTop()
						return

				self.CreatePopup("no_grandmaster_skills_avaible", 0, 0)
			else:
				chat.AppendChat(chat.CHAT_TYPE_INFO, uiScriptLocale.ASLAN_BUFF_CHAT_NO_SKILL_ITEM)
		else:
			self.CreatePopup("not_active", 0, 0)
		
	def ClickReNameButton(self):
		if self.isActive == True:
			if player.GetItemIndex(player.BUFF_EQUIPMENT, 7) == 0:
				chat.AppendChat(chat.CHAT_TYPE_INFO, uiScriptLocale.ASLAN_BUFF_CHAT_NO_CHANGE_NAME_ITEM)
				return
	
			if self.wndReName.IsShow():
				self.wndReName.Close()
			self.wndReName.Show()
			self.wndReName.SetTop()
			
	def ClickExportButton(self):
		if self.isActive == True:
			if self.dlgInfoDialog and self.dlgInfoDialog.IsShow():
				self.dlgInfoDialog.Close()
				
			self.CreateQuestion("export", 0, 0)
		else:
			self.CreatePopup("not_active", 0, 0)

	def SendExportButton(self, flag):
		if flag:
			net.SendBuffNPCAction(8, 0, 0, 0, 0)
		self.dlgInfoDialog.Close()

	def ClickChangeSexButton(self):
		if self.isActive == True:
			if player.GetItemIndex(player.BUFF_EQUIPMENT, 8) == 0:
				chat.AppendChat(chat.CHAT_TYPE_INFO, uiScriptLocale.ASLAN_BUFF_CHAT_NO_CHANGE_SEX_ITEM)
				return
				
			if self.dlgInfoDialog and self.dlgInfoDialog.IsShow():
				self.dlgInfoDialog.Close()
				
			self.CreateQuestion("change_sex", 0, 0)
		else:
			self.CreatePopup("not_active", 0, 0)
		
	def SendChangeSexButton(self, flag):
		if flag:
			net.SendBuffNPCAction(6, 0, 0, 0, 0)
		self.dlgInfoDialog.Close()
		
	def ClickResetSkillButton(self):
		if player.GetItemIndex(player.BUFF_EQUIPMENT, 9) == 0:
			chat.AppendChat(chat.CHAT_TYPE_INFO, uiScriptLocale.ASLAN_BUFF_CHAT_NO_RESET_SKILL_ITEM)
			return
			
		for slot in xrange(6):
			if self.skillInfo[slot][1] > 0:
				if self.wndSelectSkill.IsShow():
					self.wndSelectSkill.Close()
				self.wndSelectSkill.SetSkillToolTip(self.toolTipSkill)
				self.wndSelectSkill.OpenSkillSelect(1, self.skillInfo, self.intPoints)
				self.wndSelectSkill.Show()
				self.wndSelectSkill.SetTop()
				return

		self.CreatePopup("no_skills_reset_avaible", 0, 0)
		
	def ClickSkillSlot(self, slot):
		net.SendBuffNPCAction(5, slot, self.skillActiveStatus[slot], 0, 0)
		
	def OverInItem(self,index):
		if None != self.toolTip:
			self.toolTip.ClearToolTip()
			self.toolTip.SetInventoryItem(index, player.BUFF_EQUIPMENT)

	def OverOutItem(self):
		self.toolTip.HideToolTip()
		
	def RefreshEquipSlotWindow(self):
		for i in xrange(10):
			itemVnum = player.GetItemIndex(player.BUFF_EQUIPMENT,i)
			itemCount = player.GetItemCount(player.BUFF_EQUIPMENT,i)
			if itemCount <= 1:
				itemCount = 0

			if i >= 0 and i <= 3:
				self.eq_slots.SetItemSlot(i, itemVnum, itemCount)
			if i >= 4 and i <= 6:
				if i == 4:
					self.SetPreviewUpgradeSkillSlot(itemVnum)
				self.upgrade_slots.SetItemSlot(i, itemVnum, itemCount)
			if i >= 7 and i <= 9:
				self.change_slots.SetItemSlot(i, itemVnum, itemCount)
			
		self.eq_slots.RefreshSlot()
		self.upgrade_slots.RefreshSlot()
		self.change_slots.RefreshSlot()
	
	def SetPreviewUpgradeSkillSlot(self, itemVnum):
		if 50300 == itemVnum:
			item.SelectItem(itemVnum)
			self.upgradeBookSlotVnum = player.GetItemMetinSocket(player.BUFF_EQUIPMENT, 4, 0)
		elif 50513 == itemVnum:
			if self.upgradeBookSlotVnum == 50513:
				return
			self.upgradeBookSlotVnum = itemVnum
		elif 50494 <= itemVnum and  50511 >= itemVnum:
			self.upgradeBookSlotVnum = itemVnum - 50400
		else:
			self.upgradeBookSlotVnum = 0

		self.preview_upgrade_skill_slot.ClearSlot(0)
		self.preview_slot = -1

		self.GetChild("arrow_left_button").Hide()
		self.GetChild("arrow_right_button").Hide()
			
		if self.upgradeBookSlotVnum == 50513:
			self.canUpgradeGrandmasterSkillList = []
			for i in xrange(6):
				if self.skillInfo[i][1] >= 30 and self.skillInfo[i][1] < 40:
					self.canUpgradeGrandmasterSkillList.append(i)
					self.showUpgradeGrandmasterSkill = self.canUpgradeGrandmasterSkillList[0]
					self.preview_slot = self.showUpgradeGrandmasterSkill
					self.skill_arrow_page = 0
			
			if len(self.canUpgradeGrandmasterSkillList) == 0:
				return
			
			if len(self.canUpgradeGrandmasterSkillList) > 1:
				self.GetChild("arrow_right_button").Show()
				
			self.UpdatePreviewUpgradeSkillSlot()

		if self.upgradeBookSlotVnum != 0 or self.upgradeBookSlotVnum != 50513:
			for i in xrange(6):
				if self.skillInfo[i][0] == self.upgradeBookSlotVnum:
					self.preview_slot = i
			
			self.UpdatePreviewUpgradeSkillSlot()
	
	def ClickSkillArrowLeft(self):
		if self.skill_arrow_page-1 >= 0:
			self.skill_arrow_page = self.skill_arrow_page-1
			self.preview_slot = self.canUpgradeGrandmasterSkillList[self.skill_arrow_page]
			if self.skill_arrow_page == len(self.canUpgradeGrandmasterSkillList):
				self.GetChild("arrow_right_button").Hide()
			else:
				self.GetChild("arrow_right_button").Show()
			if self.preview_slot == 0 or self.skill_arrow_page == 0:
				self.GetChild("arrow_left_button").Hide()
			else:
				self.GetChild("arrow_left_button").Show()
			self.UpdatePreviewUpgradeSkillSlot()
				
	def ClickSkillArrowRight(self):
		if self.skill_arrow_page+1 < len(self.canUpgradeGrandmasterSkillList):
			self.skill_arrow_page = self.skill_arrow_page+1
			self.preview_slot = self.canUpgradeGrandmasterSkillList[self.skill_arrow_page]
			if self.skill_arrow_page == len(self.canUpgradeGrandmasterSkillList)-1:
				self.GetChild("arrow_right_button").Hide()
			else:
				self.GetChild("arrow_right_button").Show()
			if self.preview_slot == 0:
				self.GetChild("arrow_left_button").Hide()
			else:
				self.GetChild("arrow_left_button").Show()
			self.UpdatePreviewUpgradeSkillSlot()
	
	def SetSummon(self):
		self.GetChild("summon_button").SetText(uiScriptLocale.ASLAN_BUFF_BUTTON_UNSUMMON)
		
	def SetUnsummon(self):
		self.GetChild("summon_button").SetText(uiScriptLocale.ASLAN_BUFF_BUTTON_SUMMON)
		
	def SetClear(self):
		self.isActive = False
		self.GetChild("face_icon_0").Hide()
		self.GetChild("face_icon_1").Hide()
		self.GetChild("buff_name").SetText("")
		self.GetChild("buff_level").SetText("")
		self.GetChild("skill_points").SetText("")
		self.GetChild("int_points").SetText("")
		if self.toolTipExp != None:
			self.toolTipExp.SetText("")
		for slot in xrange(6):
			self.skill_slots.ClearSlot(slot)
			self.GetChild("skill_active_icon_%d" % slot).Hide()
		self.exp_info = [ 0, 0]
		for i in xrange(4):
			self.exp_gauges[i].Hide()
		self.skillPoints = 0
		
	def SetBasicInfo(self, name, sex, intvalue):
		self.isActive = True
		self.sex = sex
		self.intPoints = intvalue
		
		self.GetChild("buff_name").SetText(str(name))
		self.GetChild("int_points").SetText(uiScriptLocale.ASLAN_BUFF_INTELLIGENCE % self.intPoints)
		
		if self.sex == 0:
			self.GetChild("face_icon_1").Hide()
			self.GetChild("face_icon_0").Show()
		else:
			self.GetChild("face_icon_0").Hide()
			self.GetChild("face_icon_1").Show()

	def SetEXPInfo(self, level, cur_exp, exp):
		self.level = int(level)
		self.expInfo = [cur_exp, exp]
		self.GetChild("buff_level").SetText(str(level))
		
		cur_exp = int(cur_exp)
		exp = int(exp)
		self.exp_info = [ cur_exp, exp]
		
		if exp > 0:	
			exp_perc =  float(cur_exp) / exp * 100
		else:
			exp_perc =  float(0)

		curPoint = int(max(cur_exp, 0))
		maxPoint = int(max(exp, 0))
		quarterPoint = maxPoint / 4
		FullCount = 0

		if 0 != quarterPoint:
			FullCount = min(4, curPoint / quarterPoint)

		for i in xrange(4):
			self.exp_gauges[i].Hide()

		for i in xrange(FullCount):
			self.exp_gauges[i].SetRenderingRect(0.0, 0.0, 0.0, 0.0)
			self.exp_gauges[i].Show()

		if 0 != quarterPoint:
			if FullCount < 4:
				Percentage = float(curPoint % quarterPoint) / quarterPoint - 1.0
				self.exp_gauges[FullCount].SetRenderingRect(0.0, Percentage, 0.0, 0.0)
				self.exp_gauges[FullCount].Show()
				
		self.toolTipExp = self.TextToolTip(15)
		self.toolTipExp.SetText(uiScriptLocale.ASLAN_BUFF_HOVER_EXP_INFO % (localeInfo.NumberToShopEXP(str(cur_exp)), localeInfo.NumberToShopEXP(str(exp)), exp_perc))
		self.toolTipExp.Hide()
		
	def SetSkillInfo(self, skill1, skill2, skill3, skill4, skill5, skill6, skillpoints):
		self.skillPoints = skillpoints
		self.GetChild("skill_points").SetText(uiScriptLocale.ASLAN_BUFF_SKILLPOINTS % self.skillPoints)
		self.skillInfo = [ [94, int(skill1)], [95, int(skill2)], [96, int(skill3)], [109, int(skill4)], [110, int(skill5)], [111, int(skill6)] ]
		self.SetSkillSlots()

	def SetSkillUseStatus(self, slot0, slot1, slot2, slot3, slot4, slot5):
		for i in xrange(6):
			self.GetChild("skill_active_icon_%d" % i).Show()
		self.skillActiveStatus = [slot0, slot1, slot2, slot3, slot4, slot5]
		iconFile = [IMG_PATH + "skill_is_activate.sub", IMG_PATH + "skill_is_deactivate.sub"]
		for i in xrange(len(self.skillActiveStatus)):
			self.GetChild("skill_active_icon_%d" % i).LoadImage(iconFile[int(self.skillActiveStatus[i])])

	def SetSkillSlots(self):
		for slot in xrange(6):
			self.skill_slots.ClearSlot(slot)
			
			skillIndex = self.skillInfo[slot][0]
			skillGrade = self.GetSkillGrade(self.skillInfo[slot][1])
			skillLevel = self.GetSkillLevel(skillGrade, self.skillInfo[slot][1])
			
			self.skill_slots.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			self.skill_slots.SetSkillSlotNew(slot, skillIndex, skillGrade, skillLevel)
			self.skill_slots.SetSlotCountNew(slot, skillGrade, skillLevel)
			self.skill_slots.SetCoverButton(slot)
			self.skill_slots.SetUnselectItemSlotEvent(ui.__mem_func__(self.ClickSkillSlot))
			self.skill_slots.SetUseSlotEvent(ui.__mem_func__(self.ClickSkillSlot))
			self.skill_slots.SetOverInItemEvent(ui.__mem_func__(self.OverInSkillSlot))
			self.skill_slots.SetOverOutItemEvent(ui.__mem_func__(self.OverOutSkillSlot))
			self.skill_slots.SetPressedSlotButtonEvent(ui.__mem_func__(self.OnPressedSlotButton))
			self.skill_slots.AppendSlotButton("d:/ymir work/ui/game/windows/btn_plus_up.sub",\
											"d:/ymir work/ui/game/windows/btn_plus_over.sub",\
											"d:/ymir work/ui/game/windows/btn_plus_down.sub")
		
		if self.skillPoints == 0:
			self.skill_slots.HideAllSlotButton()
		else:
			for slot in xrange(6):
				if self.skillInfo[slot][1] >= 0 and self.skillInfo[slot][1] < 20:
					self.skill_slots.ShowSlotButton(slot)

	def SetSkillCooltime(self, slot, value):
		self.skill_slots.SetSlotCoolTime(int(slot), int(value))
		
	def UpdatePreviewUpgradeSkillSlot(self):
		if self.preview_slot == -1:
			return
				
		skillIndex = self.skillInfo[self.preview_slot][0]
		skillGrade = self.GetSkillGrade(self.skillInfo[self.preview_slot][1])
		skillLevel = self.GetSkillLevel(skillGrade, self.skillInfo[self.preview_slot][1])
		
		self.preview_upgrade_skill_slot.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
		self.preview_upgrade_skill_slot.SetSkillSlotNew(0, skillIndex, skillGrade, skillLevel)
		self.preview_upgrade_skill_slot.SetSlotCountNew(0, skillGrade, skillLevel)
		self.preview_upgrade_skill_slot.SetCoverButton(0)
		self.preview_upgrade_skill_slot.SetOverInItemEvent(ui.__mem_func__(self.OverInPreviewSkillSlot))
		self.preview_upgrade_skill_slot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutSkillSlot))

	def SelectEmptySlot(self, selectedSlotPos):

		isAttached = mouseModule.mouseController.isAttached()

		if isAttached:
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			mouseModule.mouseController.DeattachObject()
			itemIndex = player.GetItemIndex(attachedSlotPos)
			itemCount = player.GetItemCount(attachedSlotPos)

			item.SelectItem(itemIndex)
			itemType = item.GetItemType()

			if player.SLOT_TYPE_INVENTORY == attachedSlotType:
				attachedCount = mouseModule.mouseController.GetAttachedItemCount()
				net.SendItemMovePacket(player.INVENTORY, attachedSlotPos, player.BUFF_EQUIPMENT, selectedSlotPos, attachedCount)
			
	def SelectItemSlot(self, selectedSlotPos):
	
		isAttached = mouseModule.mouseController.isAttached()
		
		if isAttached:
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			mouseModule.mouseController.DeattachObject()
			itemIndex = player.GetItemIndex(attachedSlotPos)
			itemCount = player.GetItemCount(attachedSlotPos)

			item.SelectItem(itemIndex)
			itemType = item.GetItemType()

			if player.SLOT_TYPE_INVENTORY == attachedSlotType:
				attachedCount = mouseModule.mouseController.GetAttachedItemCount()
				net.SendItemMovePacket(player.INVENTORY, attachedSlotPos, player.BUFF_EQUIPMENT, selectedSlotPos, attachedCount)
		else:
			itemVnum = player.GetItemIndex(player.BUFF_EQUIPMENT, selectedSlotPos)
			itemCount = player.GetItemCount(player.BUFF_EQUIPMENT, selectedSlotPos)
			mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_BUFF_EQUIPMENT, selectedSlotPos, itemVnum, itemCount)

	def UseItemSlot(self, slotIndex):
		curCursorNum = app.GetCursor()
		if app.SELL == curCursorNum:
			return

		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		net.SendItemUsePacket(player.BUFF_EQUIPMENT, slotIndex)
		mouseModule.mouseController.DeattachObject()
		self.OverOutItem()

	def OverInSkillSlot(self, slot):
		if mouseModule.mouseController.isAttached():
			return

		if 0 == self.toolTipSkill:
			return

		skillIndex = self.skillInfo[slot][0]
		skillGrade = self.GetSkillGrade(self.skillInfo[slot][1])
		skillLevel = self.GetSkillLevel(skillGrade, self.skillInfo[slot][1])
		self.toolTipSkill.SetSkillBuffNPC(skillIndex, skillIndex, skillGrade, skillLevel, SKILL_POWER[self.skillInfo[slot][1]], SKILL_POWER[self.skillInfo[slot][1]+1], int(self.intPoints))

	def OverInPreviewSkillSlot(self, slot):
		if mouseModule.mouseController.isAttached():
			return

		if 0 == self.toolTipSkill:
			return

		skillIndex = self.skillInfo[self.preview_slot][0]
		skillGrade = self.GetSkillGrade(self.skillInfo[self.preview_slot][1])
		skillLevel = self.GetSkillLevel(skillGrade, self.skillInfo[self.preview_slot][1])
		self.toolTipSkill.SetSkillBuffNPC(skillIndex, skillIndex, skillGrade, skillLevel, SKILL_POWER[self.skillInfo[self.preview_slot][1]], SKILL_POWER[self.skillInfo[self.preview_slot][1]+1], int(self.intPoints))
		
	def GetSkillGrade(self, skillLevel):
		if skillLevel >= 0 and skillLevel < 20:
			return 0
		if skillLevel >= 20 and skillLevel < 30:
			return 1
		if skillLevel >= 30 and skillLevel < 40: 
			return 2
		if skillLevel == 40: 
			return 3

	def GetSkillLevel(self, skillGrade, skillLevel):
		if skillGrade == 0:
			return skillLevel
		if skillGrade == 1:
			return skillLevel - 19
		if skillGrade == 2:
			return skillLevel - 29
		if skillGrade == 3:
			return 40
		
	def GetExportCost(self):
		for i in xrange(len(EXPORT_COST)):
			if self.level >= EXPORT_COST[i][0] and self.level <= EXPORT_COST[i][1]:
				return EXPORT_COST[i][2]
		return -1			

	def OverOutSkillSlot(self):
		if 0 != self.toolTipSkill:
			self.toolTipSkill.HideToolTip()
	
	def OnUpdate(self):
		if self.toolTipExp != None:
			if TRUE == self.GetChild("exp_hover_info").IsIn():				
				self.toolTipExp.Show()
			else:			
				self.toolTipExp.Hide()
			
	def SetSkillToolTip(self, toolTipSkill):
		self.toolTipSkill = toolTipSkill

	def CreatePopup(self, type, value0, value1):
		self.dlgInfoDialog.Close()
		if type == 0:
			if value0 == 1:
				self.dlgInfoDialog.AppendTextLine(POPUP_TEXT_GRANDMASTER_SUCCESS[0])
				self.dlgInfoDialog.AppendTextLine(POPUP_TEXT_GRANDMASTER_SUCCESS[1] % skill.GetSkillName(value1, 0))
				snd.PlaySound("sound/effect/etc/skillup/skillup1.wav")
			if value0 == 0:
				self.dlgInfoDialog.AppendTextLine(POPUP_TEXT_GRANDMASTER_NOT_SUCCESS[0] % skill.GetSkillName(value1, 0))
				self.dlgInfoDialog.AppendTextLine(POPUP_TEXT_GRANDMASTER_NOT_SUCCESS[1])
				snd.PlaySound("sound/ui/loginfail.wav")
		if type == "not_active":
			self.dlgInfoDialog.AppendTextLine(POPUP_TEXT_NOT_ACTIVE_SHAMAN[0])
		if type == "no_grandmaster_skills_avaible":
			self.dlgInfoDialog.AppendTextLine(POPUP_TEXT_GRANDMASTER_NO_SKILLS[0])
		if type == "no_skills_reset_avaible":
			self.dlgInfoDialog.AppendTextLine(POPUP_TEXT_RESET_SKILL_NOT_SKILLS[0])
		if type == 5:
			self.dlgInfoDialog.AppendTextLine(POPUP_TEXT_RESET_SKILL[0] % skill.GetSkillName(value0, 0))
			self.dlgInfoDialog.AppendTextLine(POPUP_TEXT_RESET_SKILL[1] % value1)

		self.dlgInfoDialog.CreatePopup()
		self.dlgInfoDialog.SetTop()
		
	def CreateQuestion(self, type, value0, value1):
		self.dlgInfoDialog.Close()
		if type == "export":
			self.dlgInfoDialog.AppendTextLine(QUESTION_ASK_BEFORE_EXPORT[0])
			if app.ASLAN_BUFF_NPC_USE_SKILL_TECH_LEVEL:
				self.dlgInfoDialog.AppendTextLine(QUESTION_ASK_BEFORE_EXPORT[1])
			if app.ASLAN_BUFF_NPC_ENABLE_EXPORT_COST and self.GetExportCost() > 0:
				self.dlgInfoDialog.AppendTextLine(QUESTION_ASK_BEFORE_EXPORT[2] % localeInfo.NumberWithPoint(str(self.GetExportCost())))
			self.dlgInfoDialog.CreateQuestion(lambda flag=TRUE: self.SendExportButton(flag), lambda flag=FALSE: self.SendExportButton(flag))
		if type == "change_sex":
			self.dlgInfoDialog.AppendTextLine(QUESTION_ASK_BEFORE_CHANGE_SEX[0])
			self.dlgInfoDialog.CreateQuestion(lambda flag=TRUE: self.SendChangeSexButton(flag), lambda flag=FALSE: self.SendChangeSexButton(flag))

		self.dlgInfoDialog.SetTop()
		
	def OnPressEscapeKey(self):
		self.Close()
		return True


class BuffNPCCreateWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		
		self.selectSexButton = []
		
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.GetChild("editline_name").SetText("")
		self.GetChild("editline_name").SetFocus()
		self.__OnClickChangeSexButton(0)
		ui.ScriptWindow.Show(self)
			
	def Close(self):
		self.Hide()
	
	def Destroy(self):
		self.Hide()
		self.ClearDictionary()
		
	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/buffnpccreatewindow.py")
		except:
			import exception
			exception.Abort("BuffNPCCreateWindow.LoadWindow.LoadObject")
			
		try:
			self.GetChild("board").SetCloseEvent(self.Close)
			self.GetChild("accept_button").SetEvent(self.ClickAcceptButton)
			self.GetChild("cancel_button").SetEvent(self.Close)
			
			self.selectSexButton = [self.GetChild("button_shaman_m"), self.GetChild("button_shaman_w")]
		
			self.ModelPreview = ui.RenderTarget()
			self.ModelPreview.SetParent(self.GetChild("render_target_background"))
			self.ModelPreview.SetSize(170, 220)
			self.ModelPreview.SetPosition(5, 5)
			self.ModelPreview.SetRenderTarget(TARGET_RENDER_INDEX_FIRST_USE)
			self.ModelPreview.Show()
			renderTarget.SetBackground(TARGET_RENDER_INDEX_FIRST_USE, "d:/ymir work/ui/game/myshop_deco/model_view_bg.sub")
			renderTarget.SetVisibility(TARGET_RENDER_INDEX_FIRST_USE, True)
			
			self.__OnClickChangeSexButton(0)
			
			for i in xrange(len(self.selectSexButton)):
				self.selectSexButton[i].SetEvent(lambda sex = i : self.__OnClickChangeSexButton(sex))
			
		except:
			import exception
			exception.Abort("BuffNPCCreateWindow.LoadWindow.BindObject")

	def __OnClickChangeSexButton(self, sex):
		self.selected_sex = sex
		if sex == 0:
			self.selectSexButton[0].Down()
			self.selectSexButton[1].SetUp()
			renderTarget.SelectModel(TARGET_RENDER_INDEX_FIRST_USE, 7)
			renderTarget.SetArmor(TARGET_RENDER_INDEX_FIRST_USE, 1)
		if sex == 1:
			self.selectSexButton[0].SetUp()
			self.selectSexButton[1].Down()
			renderTarget.SelectModel(TARGET_RENDER_INDEX_FIRST_USE, 3)
			renderTarget.SetArmor(TARGET_RENDER_INDEX_FIRST_USE, 1)
		
	def ClickAcceptButton(self):
		if self.GetChild("editline_name").GetText() == "":
			chat.AppendChat(chat.CHAT_TYPE_INFO, uiScriptLocale.ASLAN_BUFF_CHAT_INFO_NAME_IS_EMPTY)
			return
		net.SendBuffNPCCreate(1, self.GetChild("editline_name").GetText(), self.selected_sex)
		self.Close()
		
	def OnPressEscapeKey(self):
		self.Close()
		return True

class BuffNPCReNameWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.GetChild("editline_name").SetText("")
		self.GetChild("editline_name").SetFocus()
		ui.ScriptWindow.Show(self)
			
	def Close(self):
		self.Hide()
	
	def Destroy(self):
		self.Hide()
		self.ClearDictionary()
		
	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/buffnpcrenamewindow.py")
		except:
			import exception
			exception.Abort("BuffNPCReNameWindow.LoadWindow.LoadObject")
			
		try:
			self.GetChild("board").SetCloseEvent(self.Close)
			self.GetChild("accept_button").SetEvent(self.ClickAcceptButton)
			self.GetChild("cancel_button").SetEvent(self.Close)
			
		except:
			import exception
			exception.Abort("BuffNPCReNameWindow.LoadWindow.BindObject")
		
	def ClickAcceptButton(self):
		if self.GetChild("editline_name").GetText() == "":
			chat.AppendChat(chat.CHAT_TYPE_INFO, uiScriptLocale.ASLAN_BUFF_CHAT_INFO_NAME_IS_EMPTY)
			return
		net.SendBuffNPCCreate(2, self.GetChild("editline_name").GetText(), 0)
		self.Close()
		
	def OnPressEscapeKey(self):
		self.Close()
		return True

class BuffNPCSelectSkillWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		ui.ScriptWindow.Show(self)
			
	def Close(self):
		self.selSlot = -1
		for i in xrange(6):
			self.skillSlotCover[i].Hide()
			self.skillSlotCoverEmpty[i].Hide()
		self.GetChild("accept_button").Hide()
		self.GetChild("text_info").SetText("")
		self.Hide()
	
	def Destroy(self):
		self.Hide()
		self.ClearDictionary()
		
	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/buffnpcselectskillwindow.py")
		except:
			import exception
			exception.Abort("BuffNPCSelectSkillWindow.LoadWindow.LoadObject")
			
		try:
			self.toolTipSkill = 0
			self.selSlot = -1
			
			self.skillInfo = []
			self.skillSlotCover = []
			self.skillSlotCoverEmpty = []
			self.skill_slots = self.GetChild("skill_slots")
			
			for i in xrange(6):
				self.skillSlotCover.append(self.GetChild("skill_slot_cover_%d" % i))
				self.skillSlotCover[i].Hide()
				self.skillSlotCoverEmpty.append(self.GetChild("skill_slot_cover_empty_%d" % i))
				self.skillSlotCoverEmpty[i].Hide()
				
			self.GetChild("board").SetCloseEvent(self.Close)
			self.GetChild("accept_button").Hide()
			
		except:
			import exception
			exception.Abort("BuffNPCSelectSkillWindow.LoadWindow.BindObject")
	
	def OpenSkillSelect(self, type, skillInfo, intpoints):
		self.selectType = type
		self.skillInfo = skillInfo
		self.intPoints = int(intpoints)
		if type == 0:
			self.GetChild("board").SetTitleName(uiScriptLocale.ASLAN_BUFF_TITLE_LEARN_GRANDMASTER)
			self.GetChild("text_info").SetText(uiScriptLocale.ASLAN_BUFF_SKILLBOARD_COST_EMPTY)
			self.SetSkillInfoUpgrade()
		if type == 1:
			self.GetChild("board").SetTitleName(uiScriptLocale.ASLAN_BUFF_TITLE_RESET_SKILL)
			self.GetChild("text_info").SetText(uiScriptLocale.ASLAN_BUFF_SKILLBOARD_SKILLPOINTS_EMPTY)
			self.SetSkillInfoRemove()
			
	def SetSkillInfoUpgrade(self):
		for slot in xrange(6):
			self.skill_slots.ClearSlot(slot)
			
			skillIndex = self.skillInfo[slot][0]
			skillGrade = self.GetSkillGrade(self.skillInfo[slot][1])
			skillLevel = self.GetSkillLevel(skillGrade, self.skillInfo[slot][1])
			
			if skillGrade == 2:
				self.skill_slots.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
				self.skill_slots.SetSkillSlotNew(slot, skillIndex, skillGrade, skillLevel)
				self.skill_slots.SetSlotCountNew(slot, skillGrade, skillLevel)
				self.skill_slots.SetSelectItemSlotEvent(ui.__mem_func__(self.ClickSkillSlot))
				self.skill_slots.SetOverInItemEvent(ui.__mem_func__(self.OverInSkillSlot))
				self.skill_slots.SetOverOutItemEvent(ui.__mem_func__(self.OverOutSkillSlot))
			else:
				self.skillSlotCoverEmpty[slot].Show()
		
	def SetSkillInfoRemove(self):
		for slot in xrange(6):
			self.skill_slots.ClearSlot(slot)
			
			skillIndex = self.skillInfo[slot][0]
			skillGrade = self.GetSkillGrade(self.skillInfo[slot][1])
			skillLevel = self.GetSkillLevel(skillGrade, self.skillInfo[slot][1])
			
			if self.skillInfo[slot][1] > 0:
				self.skill_slots.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
				self.skill_slots.SetSkillSlotNew(slot, skillIndex, skillGrade, skillLevel)
				self.skill_slots.SetSlotCountNew(slot, skillGrade, skillLevel)
				self.skill_slots.SetSelectItemSlotEvent(ui.__mem_func__(self.ClickSkillSlot))
				self.skill_slots.SetOverInItemEvent(ui.__mem_func__(self.OverInSkillSlot))
				self.skill_slots.SetOverOutItemEvent(ui.__mem_func__(self.OverOutSkillSlot))
			else:
				self.skillSlotCoverEmpty[slot].Show()
				
	def ClickSkillSlot(self, slot):
		self.selSlot = slot
		for i in xrange(6):
			if slot == i:
				self.skillSlotCover[i].Show()
				self.GetChild("accept_button").SetEvent(self.ClickAcceptButton)
				self.GetChild("accept_button").Show()
				if self.selectType == 0:
					self.GetChild("text_info").SetText(uiScriptLocale.ASLAN_BUFF_SKILLBOARD_COST % (localeInfo.NumberWithPoint(str(LEARN_GRANDMASTER_SKILL_COST[self.skillInfo[self.selSlot][1]-30]))))
				if self.selectType == 1:
					self.GetChild("text_info").SetText(uiScriptLocale.ASLAN_BUFF_SKILLBOARD_COST % (self.GetReciveMaxSkillPoints(self.skillInfo[self.selSlot][1])))
				snd.PlaySound("sound/ui/click.wav")
			else:
				self.skillSlotCover[i].Hide()
	
	def ClickAcceptButton(self):
		if self.selSlot != -1:
			if self.selectType == 0:
				net.SendBuffNPCAction(4, self.skillInfo[self.selSlot][0], 0, 0, 0)
				self.Close()
			if self.selectType == 1:
				net.SendBuffNPCAction(7, self.skillInfo[self.selSlot][0], 0, 0, 0)
				self.Close()

	def GetReciveMaxSkillPoints(self, skillLevel):
		if skillLevel >= 17:
			return 17
		else:
			return skillLevel

	def GetSkillGrade(self, skillLevel):
		if skillLevel >= 0 and skillLevel < 20:
			return 0
		if skillLevel >= 20 and skillLevel < 30:
			return 1
		if skillLevel >= 30 and skillLevel < 40: 
			return 2
		if skillLevel == 40: 
			return 3

	def GetSkillLevel(self, skillGrade, skillLevel):
		if skillGrade == 0:
			return skillLevel
		if skillGrade == 1:
			return skillLevel - 19
		if skillGrade == 2:
			return skillLevel - 29
		if skillGrade == 3:
			return 40
			
	def OverInSkillSlot(self, slot):
		if mouseModule.mouseController.isAttached():
			return

		if 0 == self.toolTipSkill:
			return

		skillIndex = self.skillInfo[slot][0]
		skillGrade = self.GetSkillGrade(self.skillInfo[slot][1])
		skillLevel = self.GetSkillLevel(skillGrade, self.skillInfo[slot][1])
		self.toolTipSkill.SetSkillBuffNPC(skillIndex, skillIndex, skillGrade, skillLevel, SKILL_POWER[self.skillInfo[slot][1]], SKILL_POWER[self.skillInfo[slot][1]+1], int(self.intPoints))
		
	def OverOutSkillSlot(self):
		if 0 != self.toolTipSkill:
			self.toolTipSkill.HideToolTip()

	def SetSkillToolTip(self, toolTipSkill):
		self.toolTipSkill = toolTipSkill

	def OnPressEscapeKey(self):
		self.Close()
		return True

class InfoDialog(ui.Board):
	def __init__(self):
		ui.Board.__init__(self)
		self.AddFlag('float')
		self.acceptEvent = lambda *arg: None
		self.textLines = []
		self.acceptButton = None
		self.cancelButton = None
		
	def __del__(self):
		ui.Board.__del__(self)

	def Show(self):
		ui.Board.Show(self)
		self.SetTop()

	def Close(self):
		self.textLines = []
		self.textChild = []
		if self.acceptButton:
			self.acceptButton.Hide()
		if self.cancelButton:
			self.cancelButton.Hide()
		self.Hide()
		
	def Accept(self):
		self.textLines = []
		self.textChild = []
		self.Hide()
		self.acceptEvent()
		
	def Destroy(self):
		self.Hide()

	def AppendTextLine(self, text):
		self.textLines.append(text)
		
	def CreatePopup(self):
		self.textChild = []
		text_y = 20
		for i in xrange(len(self.textLines)):
			self.textline = ui.TextLine()
			self.textline.SetParent(self)
			self.textline.SetText(self.textLines[i])
			self.textline.SetWindowHorizontalAlignCenter()
			self.textline.SetHorizontalAlignCenter()
			self.textline.SetPosition(0, text_y)
			self.textline.Show()
			
			text_y += 20
			self.textChild.append(self.textline)
			
		self.acceptButton = ui.Button()
		self.acceptButton.SetParent(self)
		self.acceptButton.SetUpVisual("d:/ymir work/ui/public/middle_button_01.sub")
		self.acceptButton.SetOverVisual("d:/ymir work/ui/public/middle_button_02.sub")
		self.acceptButton.SetDownVisual("d:/ymir work/ui/public/middle_button_03.sub")
		self.acceptButton.SetText(uiScriptLocale.OK)
		self.acceptButton.SetEvent(self.Close)
		self.acceptButton.SetWindowHorizontalAlignCenter()
		self.acceptButton.SetPosition(0, text_y + 10)
		self.acceptButton.Show()
			
		self.SetSize(360, text_y + 50)
		self.SetCenterPosition()
		self.Show()

	def CreateQuestion(self, onAcceptEvent, onCancelEvent):
		self.acceptEvent = onAcceptEvent
		self.textChild = []
		text_y = 20
		for i in xrange(len(self.textLines)):
			self.textline = ui.TextLine()
			self.textline.SetParent(self)
			self.textline.SetText(self.textLines[i])
			self.textline.SetWindowHorizontalAlignCenter()
			self.textline.SetHorizontalAlignCenter()
			self.textline.SetPosition(0, text_y)
			self.textline.Show()
			
			text_y += 20
			self.textChild.append(self.textline)
			
		self.acceptButton = ui.Button()
		self.acceptButton.SetParent(self)
		self.acceptButton.SetUpVisual("d:/ymir work/ui/public/middle_button_01.sub")
		self.acceptButton.SetOverVisual("d:/ymir work/ui/public/middle_button_02.sub")
		self.acceptButton.SetDownVisual("d:/ymir work/ui/public/middle_button_03.sub")
		self.acceptButton.SetText(uiScriptLocale.YES)
		self.acceptButton.SetEvent(onAcceptEvent)
		self.acceptButton.SetWindowHorizontalAlignCenter()
		self.acceptButton.SetPosition( -40 , text_y + 10)
		self.acceptButton.Show()
			
		self.cancelButton = ui.Button()
		self.cancelButton.SetParent(self)
		self.cancelButton.SetUpVisual("d:/ymir work/ui/public/middle_button_01.sub")
		self.cancelButton.SetOverVisual("d:/ymir work/ui/public/middle_button_02.sub")
		self.cancelButton.SetDownVisual("d:/ymir work/ui/public/middle_button_03.sub")
		self.cancelButton.SetText(uiScriptLocale.NO)
		self.cancelButton.SetEvent(onCancelEvent)
		self.cancelButton.SetWindowHorizontalAlignCenter()
		self.cancelButton.SetPosition( +40 , text_y + 10)
		self.cancelButton.Show()
		
		self.SetSize(360, text_y + 50)
		self.SetCenterPosition()
		self.Show()
		
	def OnPressEscapeKey(self):
		self.Close()