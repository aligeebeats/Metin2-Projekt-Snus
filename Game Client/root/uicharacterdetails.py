import ui
import localeInfo
import player
import item
import uiToolTip
import wndMgr
import app
import interfaceModule
import constInfo

class CharacterDetailsUI(ui.ScriptWindow):
	def __init__(self, parent):
		ui.ScriptWindow.__init__(self)
		self.uiCharacterStatus = parent		
		self.toolTip = uiToolTip.ToolTip()
		self.interface = None
		self.__LoadScript()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		self.uiCharacterStatus = None
		self.interface = None
		self.toolTip = None

	def __LoadScript(self):
		
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/CharacterDetailsWindow.py")
		except:
			import exception
			exception.Abort("CharacterDetailsUI.__LoadScript")

		self.Width = 253 - 3
		self.GetChild("TitleBar").CloseButtonHide()
		self.ScrollBar = self.GetChild("ScrollBar")
		self.ScrollBar.SetScrollEvent(ui.__mem_func__(self.OnScroll))

		self.UI_MAX_COUNT = 13
		self.UI_MAX_VIEW_COUNT = 6
		
		## UI KEY & VALUE
		self.INFO_TEXT	= 0
		self.INFO_TOOLTIP = 1
		self.INFO_VALUE	= 2
		self.CATEGORY_STARTLINE	= -1
		self.CATEGORY_ENDLINE	= -2

		self.labelList		= []
		self.labelValueList	= []
		self.labelTextList	= []
		self.horizonBarList	= []
		self.horizonBarNameList = []

		for i in xrange(self.UI_MAX_COUNT):
			self.labelList.append( self.GetChild("label%s"%i) )
			self.labelValueList.append( self.GetChild("labelvalue%s"%i) )
			self.labelTextList.append( self.GetChild("labelname%s"%i) )
			self.horizonBarList.append( self.GetChild("horizontalbar%s"%i) )
			self.horizonBarNameList.append( self.GetChild("horizontalbarName%s"%i) )

		for i in xrange(self.UI_MAX_COUNT):
			self.labelTextList[i].SetOverEvent( ui.__mem_func__(self.__ButtonOverIn), i )
			self.labelTextList[i].SetOverOutEvent( ui.__mem_func__(self.__ButtonOverOut), i )
			
		self.__Initialize()

	def __Initialize(self):
		self.InfoList = []
		self.InfoList.append( [ localeInfo.DETAILS_CATE_1, "", self.CATEGORY_STARTLINE ] )
		self.InfoList.append( [ localeInfo.DETAILS_1, localeInfo.DETAILS_TOOLTIP_1, 43 ] )		
		self.InfoList.append( [ localeInfo.DETAILS_3, localeInfo.DETAILS_TOOLTIP_3, 45 ] )
		self.InfoList.append( [ localeInfo.DETAILS_4, localeInfo.DETAILS_TOOLTIP_4, 47 ] )
		self.InfoList.append( [ localeInfo.DETAILS_5, localeInfo.DETAILS_TOOLTIP_5, 53 ] )			
		self.InfoList.append( [ localeInfo.DETAILS_7, localeInfo.DETAILS_TOOLTIP_7, 44 ] )
		self.InfoList.append( [ localeInfo.DETAILS_8, localeInfo.DETAILS_TOOLTIP_8, 46 ] )
		self.InfoList.append( [ localeInfo.DETAILS_9, localeInfo.DETAILS_TOOLTIP_9, 48 ] )
		self.InfoList.append( [ localeInfo.DETAILS_12, localeInfo.DETAILS_TOOLTIP_12, 95 ] )
		self.InfoList.append( [ localeInfo.DETAILS_13, localeInfo.DETAILS_TOOLTIP_13, 96 ] )
		self.InfoList.append( [ localeInfo.DETAILS_14, localeInfo.DETAILS_TOOLTIP_14, 122 ] )
		self.InfoList.append( [ localeInfo.DETAILS_15, localeInfo.DETAILS_TOOLTIP_15, 124 ] )
		self.InfoList.append( [ localeInfo.DETAILS_16, localeInfo.DETAILS_TOOLTIP_16, 121 ] )
		self.InfoList.append( [ localeInfo.DETAILS_17, localeInfo.DETAILS_TOOLTIP_17, 123 ] )
		self.InfoList.append( [ localeInfo.DETAILS_18, localeInfo.DETAILS_TOOLTIP_18, 132 ] )
		self.InfoList.append( [ localeInfo.DETAILS_19, localeInfo.DETAILS_TOOLTIP_19, 131 ] )
		self.InfoList.append( [ localeInfo.DETAILS_20, localeInfo.DETAILS_TOOLTIP_20, 40 ] )
		self.InfoList.append( [ localeInfo.DETAILS_21, localeInfo.DETAILS_TOOLTIP_21, 41 ] )
		self.InfoList.append( [ localeInfo.DETAILS_22, localeInfo.DETAILS_TOOLTIP_22, 136 ] )
		self.InfoList.append( [ localeInfo.DETAILS_23, localeInfo.DETAILS_TOOLTIP_23, 137 ] )
		self.InfoList.append( [ "", "", self.CATEGORY_ENDLINE ] )

		self.InfoList.append( [ localeInfo.DETAILS_CATE_2, "", self.CATEGORY_STARTLINE ] )
		self.InfoList.append( [ localeInfo.DETAILS_76, localeInfo.DETAILS_TOOLTIP_76, 77 ] )
		self.InfoList.append( [ localeInfo.DETAILS_24, localeInfo.DETAILS_TOOLTIP_24, 76 ] )
		self.InfoList.append( [ localeInfo.DETAILS_25, localeInfo.DETAILS_TOOLTIP_25, 133 ] )
		self.InfoList.append( [ localeInfo.DETAILS_26, localeInfo.DETAILS_TOOLTIP_26, 135 ] )
		self.InfoList.append( [ localeInfo.DETAILS_27, localeInfo.DETAILS_TOOLTIP_27, 75 ] )
		self.InfoList.append( [ localeInfo.DETAILS_28, localeInfo.DETAILS_TOOLTIP_28, 78 ] )
		self.InfoList.append( [ localeInfo.DETAILS_29, localeInfo.DETAILS_TOOLTIP_29, 134 ] )
		#self.InfoList.append( [ localeInfo.DETAILS_77, localeInfo.DETAILS_TOOLTIP_77, item.GetApplyPoint( item.APPLY_RESIST_MAGIC_REDUCTION ) ] )
		self.InfoList.append( [ "", "", self.CATEGORY_ENDLINE ] )

		self.InfoList.append( [ localeInfo.DETAILS_CATE_3, "", self.CATEGORY_STARTLINE ] )
		self.InfoList.append( [ localeInfo.DETAILS_36, localeInfo.DETAILS_TOOLTIP_36, 54 ] )
		self.InfoList.append( [ localeInfo.DETAILS_37, localeInfo.DETAILS_TOOLTIP_37, 55 ] )
		self.InfoList.append( [ localeInfo.DETAILS_38, localeInfo.DETAILS_TOOLTIP_38, 56 ] )
		self.InfoList.append( [ localeInfo.DETAILS_39, localeInfo.DETAILS_TOOLTIP_39, 57 ] )
		#self.InfoList.append( [ localeInfo.DETAILS_40, localeInfo.DETAILS_TOOLTIP_40, item.GetApplyPoint( item.APPLY_ATTBONUS_WOLFMAN ) ] )
		self.InfoList.append( [ localeInfo.DETAILS_41, localeInfo.DETAILS_TOOLTIP_41, 59 ] )
		self.InfoList.append( [ localeInfo.DETAILS_42, localeInfo.DETAILS_TOOLTIP_42, 60 ] )
		self.InfoList.append( [ localeInfo.DETAILS_43, localeInfo.DETAILS_TOOLTIP_43, 61 ] )
		self.InfoList.append( [ localeInfo.DETAILS_44, localeInfo.DETAILS_TOOLTIP_44, 62 ] )
		#self.InfoList.append( [ localeInfo.DETAILS_45, localeInfo.DETAILS_TOOLTIP_45, item.GetApplyPoint( item.APPLY_RESIST_WOLFMAN ) ] )
		self.InfoList.append( [ "", "", self.CATEGORY_ENDLINE ] )

		self.InfoList.append( [ localeInfo.DETAILS_CATE_4, "", self.CATEGORY_STARTLINE ] )
		self.InfoList.append( [ localeInfo.DETAILS_46, localeInfo.DETAILS_TOOLTIP_46, 69 ] )
		self.InfoList.append( [ localeInfo.DETAILS_47, localeInfo.DETAILS_TOOLTIP_47, 70 ] )
		self.InfoList.append( [ localeInfo.DETAILS_48, localeInfo.DETAILS_TOOLTIP_48, 71 ] )
		#self.InfoList.append( [ localeInfo.DETAILS_49, localeInfo.DETAILS_TOOLTIP_49, item.GetApplyPoint( item.APPLY_RESIST_CLAW ) ] )
		self.InfoList.append( [ localeInfo.DETAILS_50, localeInfo.DETAILS_TOOLTIP_50, 72 ] )
		self.InfoList.append( [ localeInfo.DETAILS_51, localeInfo.DETAILS_TOOLTIP_51, 73 ] )
		self.InfoList.append( [ localeInfo.DETAILS_52, localeInfo.DETAILS_TOOLTIP_52, 74 ] )
		self.InfoList.append( [ "", "", self.CATEGORY_ENDLINE ] )

		self.InfoList.append( [ localeInfo.DETAILS_CATE_5, "", self.CATEGORY_STARTLINE ] )
		self.InfoList.append( [ localeInfo.DETAILS_53, localeInfo.DETAILS_TOOLTIP_53, 38 ] )
		self.InfoList.append( [ localeInfo.DETAILS_54, localeInfo.DETAILS_TOOLTIP_54, 39 ] )
		self.InfoList.append( [ localeInfo.DETAILS_55, localeInfo.DETAILS_TOOLTIP_55, 37 ] )
		self.InfoList.append( [ localeInfo.DETAILS_56, localeInfo.DETAILS_TOOLTIP_56, 81 ] )
		#self.InfoList.append( [ localeInfo.DETAILS_57, localeInfo.DETAILS_TOOLTIP_57, item.GetApplyPoint( item.APPLY_BLEEDING_PCT ) ] )
		#self.InfoList.append( [ localeInfo.DETAILS_58, localeInfo.DETAILS_TOOLTIP_58, item.GetApplyPoint( item.APPLY_BLEEDING_REDUCE ) ] )
		self.InfoList.append( [ localeInfo.DETAILS_59, localeInfo.DETAILS_TOOLTIP_59, 63 ] )
		self.InfoList.append( [ localeInfo.DETAILS_60, localeInfo.DETAILS_TOOLTIP_60, 64 ] )
		self.InfoList.append( [ localeInfo.DETAILS_61, localeInfo.DETAILS_TOOLTIP_61, 32 ] )
		self.InfoList.append( [ localeInfo.DETAILS_62, localeInfo.DETAILS_TOOLTIP_62, 33 ] )
		self.InfoList.append( [ localeInfo.DETAILS_63, localeInfo.DETAILS_TOOLTIP_63, 67 ] )
		self.InfoList.append( [ localeInfo.DETAILS_64, localeInfo.DETAILS_TOOLTIP_64, 68 ] )
		self.InfoList.append( [ localeInfo.DETAILS_65, localeInfo.DETAILS_TOOLTIP_65, 79 ] )
		self.InfoList.append( [ localeInfo.DETAILS_66, localeInfo.DETAILS_TOOLTIP_66, 87 ] )
		self.InfoList.append( [ localeInfo.DETAILS_67, localeInfo.DETAILS_TOOLTIP_67, 82 ] )
		self.InfoList.append( [ localeInfo.DETAILS_68, localeInfo.DETAILS_TOOLTIP_68, 83 ] )
		self.InfoList.append( [ localeInfo.DETAILS_69, localeInfo.DETAILS_TOOLTIP_69, 84 ] )
		self.InfoList.append( [ localeInfo.DETAILS_70, localeInfo.DETAILS_TOOLTIP_70, 85 ] )
		self.InfoList.append( [ "", "", self.CATEGORY_ENDLINE ] )

		self.InfoList.append( [ localeInfo.DETAILS_CATE_6, "", self.CATEGORY_STARTLINE ] )
		self.InfoList.append( [ localeInfo.DETAILS_71, localeInfo.DETAILS_TOOLTIP_71, 114 ] )
		self.InfoList.append( [ localeInfo.DETAILS_72, localeInfo.DETAILS_TOOLTIP_72, 115 ] )
		self.InfoList.append( [ localeInfo.DETAILS_73, localeInfo.DETAILS_TOOLTIP_73, 116 ] )
		self.InfoList.append( [ localeInfo.DETAILS_74, localeInfo.DETAILS_TOOLTIP_74, 117 ] )
		self.InfoList.append( [ localeInfo.DETAILS_75, localeInfo.DETAILS_TOOLTIP_75, 118 ] )
		self.InfoList.append( [ "", "", self.CATEGORY_ENDLINE ] )
		
		
		self.Diff = len(self.InfoList) - (self.UI_MAX_VIEW_COUNT )
		stepSize = 1.0 / self.Diff
		self.ScrollBar.SetScrollStep( stepSize )
		self.ScollPos = 0
		self.RefreshLabel()

	def Show(self):
		ui.ScriptWindow.Show(self)
		self.SetTop()

	def Close(self):
		self.Hide()

	def AdjustPosition(self, x, y):
		self.SetPosition(x + self.Width, y)

	def OnScroll(self):
		self.RefreshLabel()

	def RefreshLabel(self):
		self.ScollPos = int(self.ScrollBar.GetPos() * self.Diff)
		self.LabelLineCount = 0
		self.startline_endlinecount = 0

		for i in xrange(self.UI_MAX_COUNT) :
			idx = i + self.ScollPos
			
			if idx < len(self.InfoList) :
				text = self.InfoList[idx][self.INFO_TEXT]
				type = self.InfoList[idx][self.INFO_VALUE]
				
				if type == self.CATEGORY_STARTLINE:
					self.__LabelTitleLine(i+self.LabelLineCount, text)
					self.startline_endlinecount += 1
				elif type == self.CATEGORY_ENDLINE:
					self.__EmptyLine(i+self.LabelLineCount)
					self.startline_endlinecount += 1
				else:
					value = player.GetStatus(type)
					self.__LabelLine(i+self.LabelLineCount, text, value)
					if i+self.LabelLineCount < self.UI_MAX_COUNT:
						self.labelTextList[i+self.LabelLineCount].SetOverEvent( ui.__mem_func__(self.__ButtonOverIn), self.LabelLineCount + self.startline_endlinecount )
						self.labelTextList[i+self.LabelLineCount].SetOverOutEvent( ui.__mem_func__(self.__ButtonOverOut), self.LabelLineCount + self.startline_endlinecount )
					self.LabelLineCount += 1
			else:
				self.__EmptyLine(i+self.LabelLineCount)

				
	def __LabelTitleLine(self, idx, text):
		if(idx < self.UI_MAX_COUNT):
			self.labelList[idx].Hide()
			self.labelTextList[idx].Hide()
			self.horizonBarList[idx].Show()
			self.horizonBarNameList[idx].SetText( text )
				
	def __EmptyLine(self, idx):
		if(idx < self.UI_MAX_COUNT):
			self.labelList[idx].Hide()
			self.labelTextList[idx].Hide()
			self.horizonBarList[idx].Hide()
		
	def __LabelLine(self, idx, text, value):
		if(idx < self.UI_MAX_COUNT):
			self.labelTextList[idx].Show()
			self.horizonBarList[idx].Hide()
			self.labelList[idx].Hide()
		
			self.labelTextList[idx].SetText( text )
		
			if(idx+1 < self.UI_MAX_COUNT):
				self.labelList[idx+1].Show()
				self.horizonBarList[idx+1].Hide()
				self.labelTextList[idx+1].Hide()
				self.labelValueList[idx+1].SetText( str(value) )

	def __ButtonOverIn(self, i):
		idx = i + self.ScollPos
		tooltip = self.InfoList[idx][self.INFO_TOOLTIP]

		arglen = len(str(tooltip))
		pos_x, pos_y = wndMgr.GetMousePosition()

		self.toolTip.ClearToolTip()
		self.toolTip.SetThinBoardSize(6 * arglen)
		self.toolTip.SetToolTipPosition(pos_x + 50, pos_y + 50)
		self.toolTip.AppendTextLine(tooltip, 0xffffff00)
		self.toolTip.Show()		

	def __ButtonOverOut(self, idx):
		self.toolTip.Hide()

	def OnTop(self):
		if self.uiCharacterStatus:
			self.uiCharacterStatus.SetTop()