import ui
import net
import constInfo

class OptionDialog(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()
		self.__Load()
		
		self.__RefreshPickUpModeButtons()
		self.__RefreshPickupIgnoreButtons()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		print " -------------------------------------- DELETE DROP OPTION DIALOG"

	def __Initialize(self):
		self.board = 0
		
		self.pickupModeButtonList = []
		self.pickupIgnoreButtonList = []

	def Destroy(self):
		self.ClearDictionary()
		self.__Initialize()
		print " -------------------------------------- DESTROY DROP OPTION DIALOG"
	
	def __Load_LoadScript(self, fileName):
		try:
			pyScriptLoader = ui.PythonScriptLoader()
			pyScriptLoader.LoadScriptFile(self, fileName)
		except:
			import exception
			exception.Abort("OptionDialog.__Load_LoadScript")

	def __Load_BindObject(self):
		try:
			for mode in ["single", "all"]:
				self.pickupModeButtonList.append(self.GetChild("pickup_mode_" + mode))
			
			for type in ["weapon", "armor", "head", "shield", "wrist", "foots", "neck", "ear", "etc"]:
				self.pickupIgnoreButtonList.append(self.GetChild("drops_ignore_" + type))
			
		except:
			import exception
			exception.Abort("OptionDialog.__Load_BindObject")

	def __Load(self):
		self.__Load_LoadScript("uiscript/pickupfilterdialog.py")
		self.__Load_BindObject()
		self.SetCenterPosition()

		self.GetChild("board").SetCloseEvent(ui.__mem_func__(self.Close))
		
		for i in xrange(len(self.pickupModeButtonList)):
			self.pickupModeButtonList[i].SetEvent(lambda mode = i : self.__OnClickPickupModeButton(mode))
			
		for i in xrange(len(self.pickupIgnoreButtonList)):
			self.pickupIgnoreButtonList[i].SetToggleUpEvent(lambda type = 1 << i, ignore = False : self.__OnClickPickupIgnoreButton(type, ignore))
			self.pickupIgnoreButtonList[i].SetToggleDownEvent(lambda type = 1 << i, ignore = True : self.__OnClickPickupIgnoreButton(type, ignore))

	def __OnClickPickupModeButton(self, mode):
		constInfo.PICKUP_MODE = mode
		
		self.__SendPickupModeCommand()
		self.__RefreshPickUpModeButtons()
		
	def __RefreshPickUpModeButtons(self):
		self.pickupModeButtonList[0].SetUp()
		self.pickupModeButtonList[1].SetUp()
		
		self.pickupModeButtonList[constInfo.PICKUP_MODE].Down()
		
	def __OnClickPickupIgnoreButton(self, type, ignore):
		if ignore:
			constInfo.PICKUP_FLAG += type
		else:
			constInfo.PICKUP_FLAG -= type
			
		self.__SendPickupModeCommand()
		self.__RefreshPickupIgnoreButtons()
		
	def __RefreshPickupIgnoreButtons(self):
		for i in xrange(len(self.pickupIgnoreButtonList)):
			flag = (1 << i)
			if constInfo.PICKUP_FLAG & flag:
				self.pickupIgnoreButtonList[i].Down()
			else:
				self.pickupIgnoreButtonList[i].SetUp()
				
	def __SendPickupModeCommand(self):
		net.SendChatPacket("/setpickupmode {} {}".format(constInfo.PICKUP_MODE, constInfo.PICKUP_FLAG))	
				
	def OnPressEscapeKey(self):
		self.Close()
		return True

	def Show(self):
		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()

