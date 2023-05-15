import ui
import uiScriptLocale
import net
import app
import dbg
import player
import background
import wndMgr
import localeInfo
import chrmgr
import colorInfo
import constInfo
import playerSettingModule
import stringCommander
import emotion
import uiRefine
import uiToolTip
import uiAttachMetin
import uiPickMoney
import uiChat
import uiMessenger
import uiHelp
import uiWhisper
import uiPointReset
import uiShop
import uiExchange
import uiSystem
import uiOption
import uiRestart

class LoadingWindow(ui.ScriptWindow):
	def __init__(self, stream):
		print "NEW LOADING WINDOW -------------------------------------------------------------------------------"
		ui.Window.__init__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_LOAD, self)

		self.stream=stream
		self.loadingImage=0
		self.loadingGage=0
		self.errMsg=0
		self.update=0
		self.playerX=0
		self.playerY=0
		self.loadStepList=[]

	def __del__(self):
		print "---------------------------------------------------------------------------- DELETE LOADING WINDOW"
		net.SetPhaseWindow(net.PHASE_WINDOW_LOAD, 0)
		ui.Window.__del__(self)

	def Open(self):
		print "OPEN LOADING WINDOW -------------------------------------------------------------------------------"

		#app.HideCursor()

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, uiScriptLocale.LOCALE_UISCRIPT_PATH + "LoadingWindow.py")
		except:
			import exception
			exception.Abort("LodingWindow.Open - LoadScriptFile Error")

		try:
			self.loadingImage=self.GetChild("BackGround")
			self.errMsg=self.GetChild("ErrorMessage")
			self.loadingGage=self.GetChild("FullGage")
		except:
			import exception
			exception.Abort("LodingWindow.Open - LoadScriptFile Error")

		self.errMsg.Hide()
		imgFileNameDict = {
			0 : uiScriptLocale.LOCALE_UISCRIPT_PATH + "loading/loading0.sub",
			1 : uiScriptLocale.LOCALE_UISCRIPT_PATH + "loading/loading1.sub",
			2 : uiScriptLocale.LOCALE_UISCRIPT_PATH + "loading/loading2.sub",
			3 : uiScriptLocale.LOCALE_UISCRIPT_PATH + "loading/loading3.sub",
		}

		try:
			imgFileName = imgFileNameDict[app.GetRandom(0, len(imgFileNameDict) - 1)]
			self.loadingImage.LoadImage(imgFileName)

		except:
			print "LoadingWindow.Open.LoadImage - %s File Load Error" % (imgFileName)
			self.loadingImage.Hide()


		width = float(wndMgr.GetScreenWidth()) / float(self.loadingImage.GetWidth())
		height = float(wndMgr.GetScreenHeight()) / float(self.loadingImage.GetHeight())

		self.loadingImage.SetScale(width, height)
		self.loadingGage.SetPercentage(2, 100)

		self.Show()

		chrSlot=self.stream.GetCharacterSlot()
		net.SendSelectCharacterPacket(chrSlot)

		app.SetFrameSkip(0)

	def Close(self):
		print "---------------------------------------------------------------------------- CLOSE LOADING WINDOW"

		app.SetFrameSkip(1)

		self.loadStepList=[]
		self.loadingImage=0
		self.loadingGage=0
		self.errMsg=0
		self.ClearDictionary()
		self.Hide()

	def OnPressEscapeKey(self):
		app.SetFrameSkip(1)
		self.stream.SetLoginPhase()
		return TRUE

	def __SetNext(self, next):
		if next:
			self.update=ui.__mem_func__(next)
		else:
			self.update=0

	def __SetProgress(self, p):
		if self.loadingGage:
			self.loadingGage.SetPercentage(2+98*p/100, 100)
		#	self.loadingGage.SetPercentage(p, 160)

	def DEBUG_LoadData(self, playerX, playerY):
		self.playerX=playerX
		self.playerY=playerY

		self.__RegisterSkill()
		self.__RegisterTitleName()
		self.__RegisterColor()
		self.__InitData()
		self.__LoadSound()
		self.__LoadGameEffect()
		self.__LoadWarrior()
		self.__LoadAssassin()
		self.__LoadSura()
		self.__LoadShaman()
		if app.ENABLE_WOLFMAN:
			self.__LoadWolfman()
		self.__LoadSkill()
		self.__LoadEnemy()
		self.__LoadNPC()
		self.__StartGame()

	def LoadData(self, playerX, playerY):
		background.Destroy()
		self.playerX=playerX
		self.playerY=playerY

		self.__RegisterDungeonMapName()
		self.__RegisterSkill()
		self.__RegisterTitleName()
		self.__RegisterColor()
		self.__RegisterEmotionIcon()

		self.loadStepList=[
			(0, ui.__mem_func__(self.__InitData)),
			(5, ui.__mem_func__(self.__LoadSound)),
			(10, ui.__mem_func__(self.__LoadGameEffect)),
			(20, ui.__mem_func__(self.__LoadWarrior)),
			(30, ui.__mem_func__(self.__LoadAssassin)),
			(40, ui.__mem_func__(self.__LoadSura)),
			(50, ui.__mem_func__(self.__LoadShaman)),
			(60, ui.__mem_func__(self.__LoadSkill)),
			(70, ui.__mem_func__(self.__LoadEnemy)),
			(75, ui.__mem_func__(self.__LoadNPC)),
			(90, ui.__mem_func__(self.__LoadGuildBuilding)),
			(100, ui.__mem_func__(self.__StartGame)),
		]

		if app.ENABLE_WOLFMAN:
			self.loadStepList.append((85, ui.__mem_func__(self.__LoadWolfman)),)

		self.__SetProgress(0)

	#	tmpLoadStepList = tuple(zip(*self.loadStepList))[0]
	#	for progress in set(range(tmpLoadStepList[0], tmpLoadStepList[-1] + 1)).difference(tmpLoadStepList):
	#		self.loadStepList.append((progress, lambda: None))
	#	self.loadStepList.sort()

	def OnUpdate(self):
		if len(self.loadStepList)>0:
			(progress, runFunc)=self.loadStepList[0]

			try:
				runFunc()

			except:	
				self.errMsg.Show()
				self.loadStepList=[]

				dbg.TraceError(" !!! Failed to load game data : STEP [%d]" % (progress))
				app.Exit()

				return

			self.loadStepList.pop(0)
			self.__SetProgress(progress)

	def __InitData(self):
		playerSettingModule.LoadGameData("INIT")

	def __RegisterDungeonMapName(self):
		background.RegisterDungeonMapName("metin2_map_spiderdungeon")
		background.RegisterDungeonMapName("metin2_map_monkeydungeon")
		background.RegisterDungeonMapName("metin2_map_monkeydungeon_02")
		background.RegisterDungeonMapName("metin2_map_monkeydungeon_03")
		background.RegisterDungeonMapName("metin2_map_deviltower1")

	def __RegisterSkill(self):
		race = net.GetMainActorRace()
		group = net.GetMainActorSkillGroup()
		empire = net.GetMainActorEmpire()

		playerSettingModule.RegisterSkill(race, group, empire)

	def __RegisterTitleName(self):
		for i in xrange(len(localeInfo.TITLE_NAME_LIST)):
			chrmgr.RegisterTitleName(i, localeInfo.TITLE_NAME_LIST[i])

	def __RegisterColor(self):
		NAME_COLOR_DICT = {
			chrmgr.NAMECOLOR_PC : colorInfo.CHR_NAME_RGB_PC,
			chrmgr.NAMECOLOR_NPC : colorInfo.CHR_NAME_RGB_NPC,
			chrmgr.NAMECOLOR_MOB : colorInfo.CHR_NAME_RGB_MOB,
			chrmgr.NAMECOLOR_PVP : colorInfo.CHR_NAME_RGB_PVP,
			chrmgr.NAMECOLOR_PK : colorInfo.CHR_NAME_RGB_PK,
			chrmgr.NAMECOLOR_PARTY : colorInfo.CHR_NAME_RGB_PARTY,
			chrmgr.NAMECOLOR_WARP : colorInfo.CHR_NAME_RGB_WARP,
			chrmgr.NAMECOLOR_WAYPOINT : colorInfo.CHR_NAME_RGB_WAYPOINT,
			chrmgr.NAMECOLOR_METIN : colorInfo.CHR_NAME_RGB_METIN,

			chrmgr.NAMECOLOR_EMPIRE_MOB : colorInfo.CHR_NAME_RGB_EMPIRE_MOB,
			chrmgr.NAMECOLOR_EMPIRE_NPC : colorInfo.CHR_NAME_RGB_EMPIRE_NPC,
			chrmgr.NAMECOLOR_EMPIRE_PC+1 : colorInfo.CHR_NAME_RGB_EMPIRE_PC_A,
			chrmgr.NAMECOLOR_EMPIRE_PC+2 : colorInfo.CHR_NAME_RGB_EMPIRE_PC_B,
			chrmgr.NAMECOLOR_EMPIRE_PC+3 : colorInfo.CHR_NAME_RGB_EMPIRE_PC_C,
			chrmgr.NAMECOLOR_MOUNT : colorInfo.CHR_NAME_RGB_MOUNT,
			chrmgr.NAMECOLOR_PET : colorInfo.CHR_NAME_RGB_PET,
		}

		for name, rgb in NAME_COLOR_DICT.items():
			chrmgr.RegisterNameColor(name, rgb[0], rgb[1], rgb[2])

		TITLE_COLOR_DICT = (	colorInfo.TITLE_RGB_GOOD_4,
								colorInfo.TITLE_RGB_GOOD_3,
								colorInfo.TITLE_RGB_GOOD_2,
								colorInfo.TITLE_RGB_GOOD_1,
								colorInfo.TITLE_RGB_NORMAL,
								colorInfo.TITLE_RGB_EVIL_1,
								colorInfo.TITLE_RGB_EVIL_2,
								colorInfo.TITLE_RGB_EVIL_3,
								colorInfo.TITLE_RGB_EVIL_4,	)
		count = 0
		for rgb in TITLE_COLOR_DICT:
			chrmgr.RegisterTitleColor(count, rgb[0], rgb[1], rgb[2])
			count += 1

	def __RegisterEmotionIcon(self):
		emotion.RegisterEmotionIcons()

	def __LoadSound(self):
		playerSettingModule.LoadGameData("SOUND")

	def __LoadGameEffect(self):
		playerSettingModule.LoadGameData("EFFECT")

	def __LoadWarrior(self):
		playerSettingModule.LoadGameData("WARRIOR")

	def __LoadAssassin(self):
		playerSettingModule.LoadGameData("ASSASSIN")

	def __LoadSura(self):
		playerSettingModule.LoadGameData("SURA")

	def __LoadShaman(self):
		playerSettingModule.LoadGameData("SHAMAN")

	if app.ENABLE_WOLFMAN:
		def __LoadWolfman(self):
			playerSettingModule.LoadGameData("WOLFMAN")

	def __LoadSkill(self):
		playerSettingModule.LoadGameData("SKILL")

	def __LoadEnemy(self):
		playerSettingModule.LoadGameData("ENEMY")

	def __LoadNPC(self):
		playerSettingModule.LoadGameData("NPC")

	# GUILD_BUILDING
	def __LoadGuildBuilding(self):
		playerSettingModule.LoadGuildBuildingList(localeInfo.GUILD_BUILDING_LIST_TXT)
	# END_OF_GUILD_BUILDING

	def __StartGame(self):
		background.SetViewDistanceSet(background.DISTANCE0, 25600)
		"""
		background.SetViewDistanceSet(background.DISTANCE1, 19200)
		background.SetViewDistanceSet(background.DISTANCE2, 12800)
		background.SetViewDistanceSet(background.DISTANCE3, 9600)
		background.SetViewDistanceSet(background.DISTANCE4, 6400)
		"""
		background.SelectViewDistanceNum(background.DISTANCE0)

		app.SetGlobalCenterPosition(self.playerX, self.playerY)

		net.StartGame()
