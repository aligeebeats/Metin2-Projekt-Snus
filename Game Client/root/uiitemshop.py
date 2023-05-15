import ui
import localeInfo
import uiToolTip
import constInfo
import uiCommon

# NOTE: Binary imports
import item
import net
import snd
import ime
import wndMgr
import app
import player
import dbg

# NOTE: Extern imports
from _weakref import proxy
import math
from random import randrange

# NOTE: Only set to true if tooltip not visible
ENABLE_OTHER_TOOLTIP_FUNCTION = False

# NOTE: Adjust max stack size, i could send it via packet but cmon.
STACK_MAX = 5000

PAGE_DEFAULT = 0
PAGE_SHOP = 1
PAGE_MAX_NUM = 2

MAX_ALPHA = 1.0
MIN_ALPHA = 0.3
BANNER_CHANGE_BY = 0.01
PROMOTION_CHANGE_BY = 0.02
IMAGE_CHANGE_TIME = 0.04

# NOTE: If category-icon has no '.' we load as item-icon else:
ICON_PATH = 'root/itemshop/icons/'

BANNER_CONTAINER = [
	'root/itemshop/banners/banner_1.png',
	'root/itemshop/banners/banner_2.png'
]

# NOTE: All strings you have to know about
ITEMSHOP_INFO = {
	'pages' : {
		PAGE_DEFAULT : {
			'text' : 'Itemshop - Startpage',
		},
		PAGE_SHOP : {
			'text' : 'Itemshop - Shop',
		},
	},
	'promotion_code' : {
		0 : {
			'text' : 'Promotion-code invalid.',
			'color' : 0xffFD0803,
		},
		1 : {
			'text' : 'Promotion code redeemed.',
			'color' : 0xff4AD537,
		},
		2 : {
			'text' : 'Promotion code already redeemed.',
			'color' : 0xff6576EF,
		}
	},
	'strings' : {
		'ITEM_PRICE' : '%d Coins',
		'ITEMS_LEFT' : '%s left',
		'BUY_QUESTION' : 'Do you really want to buy this item?'
	}
}

# NOTE: Main itemshop-window
class ItemshopWindow(ui.BoardWithTitleBar):
	BOARD_WIDTH = 800
	BOARD_HEIGHT = 553
	DEFAULT_LISTBOX_ITEM_COUNT_X = 4
	DEFAULT_LISTBOX_ITEM_COUNT_Y = 1
	DEFAULT_LISTBOX_ITEM_HEIGHT = 136
	DEFAULT_LISTBOX_ITEM_WIDTH = 185
	CATEGORY_LISTBOX_ITEM_COUNT_X = 1
	CATEGORY_LISTBOX_ITEM_COUNT_Y = 9
	CATEGORY_LISTBOX_ITEM_HEIGHT = 49
	CATEGORY_LISTBOX_ITEM_WIDTH = 162
	MAIN_LISTBOX_ITEM_COUNT_X = 3
	MAIN_LISTBOX_ITEM_COUNT_Y = 3
	MAIN_LISTBOX_ITEM_HEIGHT = 136
	MAIN_LISTBOX_ITEM_WIDTH = 185

	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.AddFlag('movable')
		self.SetSize(self.BOARD_WIDTH, self.BOARD_HEIGHT)
		self.SetCloseEvent(self.Close)
		self.ResetWindow()

		self.toolTip = uiToolTip.ItemToolTip()

		# NOTE: Create page base
		self.CreatePages()
		self.CreateDefaultPageTabs()

		# NOTE: Create elements for main page
		self.CreateBanner()
		self.CreateRandomItemListbox()

		# NOTE: Create elements for buy page
		self.CreateShopPageTabs()
		self.CreateItemListbox()
		self.CreateCategorysListbox()

		# NOTE: Promotion board
		self.CreatePromotionCode()

		# NOTE: Payment board
		self.CreatePaymentBoard()

		# NOTE: Advertise board
		self.CreateAdvertiseBoard()

		# NOTE: Top-most
		self.CreateUserDisplay()
		self.CreateCoinDisplay()

		self.SelectPage(PAGE_DEFAULT)
		self.SetCenterPosition()

	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def Destroy(self):
		self.ResetWindow()

	def ResetWindow(self):
		self.pages = []
		self.tabs = []
		self.tabBackgrounds = []
		self.categorys = []
		self.children = []
		self.cur_category = 0
		self.toolTip = None

		self.bannerNextImageTime = 0.0
		self.bannerDecreaseAlpha = True
		self.bannerValue = MAX_ALPHA
		self.bannerIndex = 0

		self.promotionNextImageTime = 0.0
		self.promotionValue = MAX_ALPHA

		self.questionDialog = None

	def Open(self):
		self.SetTop()
		self.Show()

	def Close(self):
		self.OnCloseQuestionDialog()
		self.ResetEditLine()
		self.Hide()

	# NOTE: Set random items to landing page
	def SetRandomStartpageItems(self):
		self.randomListbox.RemoveAllItems()

		if not constInfo.ITEMSHOP_INFOS:
			return

		alreadyIn = []
		count = 0
		totalItemCount = 0

		for category in constInfo.ITEMSHOP_INFOS:
			if 'items' in category:
				for items in category['items']:
					totalItemCount += len(items)

		while len(alreadyIn) < self.DEFAULT_LISTBOX_ITEM_COUNT_X:
			if count > totalItemCount:
				break
			count += 1
			randomCategory = randrange(0, len(constInfo.ITEMSHOP_INFOS))
			if not 'items' in constInfo.ITEMSHOP_INFOS[randomCategory]:
				return
			randomItem = constInfo.ITEMSHOP_INFOS[randomCategory]['items'][randrange(0, len(constInfo.ITEMSHOP_INFOS[randomCategory]['items']))]

			if randomItem['hash'] in alreadyIn:
				continue

			itemContainer = ItemSlot(
				randomItem,
				self.toolTip,
				self
			)
			self.randomListbox.AppendItem(itemContainer)
			alreadyIn.append(randomItem['hash'])

	def CreatePages(self):
		for _ in xrange(PAGE_MAX_NUM):
			page = ui.Window()
			page.SetParent(self)
			page.AddFlag('not_pick')
			page.SetPosition(0, 30)
			page.SetSize(self.BOARD_WIDTH, self.BOARD_HEIGHT - 30)
			self.pages.append(page)

	def CreateDefaultPageTabs(self):
		tabBackground = ui.ImageBox()
		tabBackground.SetParent(self.pages[PAGE_DEFAULT])
		tabBackground.LoadImage('d:/ymir work/ui/itemshop/base/tab_background.sub')
		tabBackground.SetPosition(12, 4)
		tabBackground.Show()

		self.tabBackgrounds.append(tabBackground)

		for i in xrange(PAGE_MAX_NUM):
			tab = ui.RadioButton()
			tab.SetParent(tabBackground)
			tab.SetPosition(10 + (37 + 6) * i, 9)
			tab.SetUpVisual('d:/ymir work/ui/itemshop/base/tab_button_up.sub')
			tab.SetDownVisual('d:/ymir work/ui/itemshop/base/tab_button_down.sub')
			tab.SetOverVisual('d:/ymir work/ui/itemshop/base/tab_button_over.sub')
			tab.SetEvent(lambda arg = i : self.SelectPage(arg))
			tab.Show()
			self.tabs.append(tab)

			tabImage = ui.ImageBox()
			tabImage.SetParent(tab)
			tabImage.AddFlag('not_pick')
			tabImage.SetPosition(0, 2)
			tabImage.LoadImage('d:/ymir work/ui/itemshop/base/tab_image_%d.sub' % i)
			tabImage.SetWindowHorizontalAlignCenter()
			tabImage.SetWindowVerticalAlignCenter()
			tabImage.Show()

			self.children.append(tabImage)

	def CreateUserDisplay(self):
		userBackground = ui.ImageBox()
		userBackground.AddFlag('not_pick')
		userBackground.LoadImage('d:/ymir work/ui/itemshop/base/user_display_background.sub')
		userBackground.SetParent(self)
		userBackground.SetPosition(563-187, 43)
		userBackground.Show()
		self.children.append(userBackground)

		userText = ui.TextLine()
		userText.SetParent(userBackground)
		userText.AddFlag('not_pick')
		userText.SetPosition((userBackground.GetWidth() - 15) / 2, userBackground.GetHeight() / 2)
		userText.SetHorizontalAlignCenter()
		userText.SetVerticalAlignCenter()
		userText.SetFontName(localeInfo.UI_DEF_FONT_LARGE)
		userText.SetFontColor((219.0 / 255.0), (192.0 / 255.0), (174.0 / 255.0))
		userText.Show()
	
		self.userText = userText

	def CreateCoinDisplay(self):
		coinBackground = ui.ImageBox()
		coinBackground.AddFlag('not_pick')
		coinBackground.LoadImage('d:/ymir work/ui/itemshop/base/coin_display_background.sub')
		coinBackground.SetParent(self)
		coinBackground.SetPosition(563, 43)
		coinBackground.Show()
		self.children.append(coinBackground)

		coinText = ui.TextLine()
		coinText.SetParent(coinBackground)
		coinText.AddFlag('not_pick')
		coinText.SetPosition(coinBackground.GetWidth() / 2, coinBackground.GetHeight() / 2)
		coinText.SetHorizontalAlignCenter()
		coinText.SetVerticalAlignCenter()
		coinText.SetFontName(localeInfo.UI_DEF_FONT_LARGE)
		coinText.SetFontColor((219.0 / 255.0), (192.0 / 255.0), (174.0 / 255.0))
		coinText.Show()
		self.coinText = coinText

	def CreateBanner(self):
		bannerBackground = ui.ImageBox()
		bannerBackground.SetParent(self.pages[PAGE_DEFAULT])
		bannerBackground.LoadImage('d:/ymir work/ui/itemshop/base/banner_background.sub')
		bannerBackground.SetPosition(12, 65)
		bannerBackground.Show()

		self.children.append(bannerBackground)

		bannerImage = ui.ImageBox()
		bannerImage.SetParent(bannerBackground)
		bannerImage.SetPosition(3, 3)
		if BANNER_CONTAINER:
			bannerImage.LoadImage(self.GetImage())
		bannerImage.Show()

		self.bannerImage = bannerImage

	def CreateRandomItemListbox(self):
		randomListbox = ListBox(
			parent = self.pages[PAGE_DEFAULT],
			itemWidth = self.DEFAULT_LISTBOX_ITEM_WIDTH,
			itemHeight = self.DEFAULT_LISTBOX_ITEM_HEIGHT,
			item_count_x=self.DEFAULT_LISTBOX_ITEM_COUNT_X,
			item_count_y=self.DEFAULT_LISTBOX_ITEM_COUNT_Y,
			item_gap_x=11,
			item_gap_y=0,
			pos = (12, 228)
		)

		self.randomListbox = randomListbox

	def CreateShopPageTabs(self):
		tabBackground = ui.ImageBox()
		tabBackground.SetParent(self.pages[PAGE_SHOP])
		tabBackground.LoadImage('d:/ymir work/ui/itemshop/base/tab_background.sub')
		tabBackground.SetPosition(12, 4)
		tabBackground.Show()

		self.tabBackgrounds.append(tabBackground)

		tab = ui.Button()
		tab.SetParent(tabBackground)
		tab.SetPosition(10, 9)
		tab.SetUpVisual('d:/ymir work/ui/itemshop/base/tab_button_up.sub')
		tab.SetDownVisual('d:/ymir work/ui/itemshop/base/tab_button_down.sub')
		tab.SetOverVisual('d:/ymir work/ui/itemshop/base/tab_button_over.sub')
		tab.SetEvent(lambda : self.SelectPage(PAGE_DEFAULT))
		tab.Show()
		self.children.append(tab)

		tabImage = ui.ImageBox()
		tabImage.SetParent(tab)
		tabImage.AddFlag('not_pick')
		tabImage.SetPosition(0, 2)
		tabImage.LoadImage('d:/ymir work/ui/itemshop/base/tab_image_back.sub')
		tabImage.SetWindowHorizontalAlignCenter()
		tabImage.SetWindowVerticalAlignCenter()
		tabImage.Show()

		self.children.append(tabImage)

	def CreateItemListbox(self):
		listboxBackground = ui.ImageBox()
		listboxBackground.SetParent(self.pages[PAGE_SHOP])
		listboxBackground.LoadImage('d:/ymir work/ui/itemshop/base/item_listbox_background.sub')
		listboxBackground.SetPosition(190, 65)
		listboxBackground.Show()

		self.children.append(listboxBackground)

		scrollBar = ScrollBar(
			parent = listboxBackground,
			width = 8,
			height = listboxBackground.GetHeight()-3,
			pos = (listboxBackground.GetWidth()-8, 1),
			color = 'red'
		)
		self.scrollBar = scrollBar

		listbox = ListBox(
			parent = self.pages[PAGE_SHOP],
			itemWidth = self.MAIN_LISTBOX_ITEM_WIDTH,
			itemHeight = self.MAIN_LISTBOX_ITEM_HEIGHT,
			item_count_x = self.MAIN_LISTBOX_ITEM_COUNT_X,
			item_count_y = self.MAIN_LISTBOX_ITEM_COUNT_Y,
			item_gap_x = 12,
			item_gap_y = 12,
			pos = (194, 69),
			scrollBar = self.scrollBar
		)

		self.listbox = listbox

		editlineBackground = ui.ImageBox()
		editlineBackground.AddFlag('not_pick')
		editlineBackground.LoadImage('d:/ymir work/ui/itemshop/base/editline_background.sub')
		editlineBackground.SetParent(self.tabBackgrounds[PAGE_SHOP])
		editlineBackground.SetPosition(53, 9)
		editlineBackground.Show()
		self.children.append(editlineBackground)
		editLine = EditLine(
			event = self.__OnType
		)
		editLine.SetParent(editlineBackground)
		editLine.SetPosition(35, 7)
		editLine.SetEscapeEvent(lambda : [self.Close()])
		editLine.SetSize(editlineBackground.GetWidth() - 38, editlineBackground.GetHeight() - 10)
		editLine.SetMax(24)
		editLine.SetFontName(localeInfo.UI_DEF_FONT_LARGE)
		editLine.SetFontColor((219.0 / 255.0), (192.0 / 255.0), (174.0 / 255.0))
		editLine.Show()
		self.editLine = editLine

	def CreateCategorysListbox(self):
		categoryListboxBackground = ui.ImageBox()
		categoryListboxBackground.SetParent(self.pages[PAGE_SHOP])
		categoryListboxBackground.LoadImage('d:/ymir work/ui/itemshop/base/category_listbox_background.sub')
		categoryListboxBackground.SetPosition(12, 65)
		categoryListboxBackground.Show()
		self.children.append(categoryListboxBackground)

		categoryScrollBar = ScrollBar(
			parent = categoryListboxBackground,
			width = 8,
			height = categoryListboxBackground.GetHeight() - 3,
			pos = (categoryListboxBackground.GetWidth() - 8, 1),
			color = 'blue',
		)
		self.categoryScrollBar = categoryScrollBar

		categoryListbox = ListBox(
			parent = self.pages[PAGE_SHOP],
			itemWidth = self.CATEGORY_LISTBOX_ITEM_WIDTH,
			itemHeight = self.CATEGORY_LISTBOX_ITEM_HEIGHT,
			item_count_y = self.CATEGORY_LISTBOX_ITEM_COUNT_Y,
			item_count_x = self.CATEGORY_LISTBOX_ITEM_COUNT_X,
			item_gap_x = 0,
			item_gap_y = 0,
			pos = (12, 65),
			scrollBar = self.categoryScrollBar
		)

		self.categoryListbox = categoryListbox

	def CreatePromotionCode(self):
		promotionCodeBackground = ui.ImageBox()
		promotionCodeBackground.SetParent(self.pages[PAGE_DEFAULT])
		promotionCodeBackground.LoadImage('d:/ymir work/ui/itemshop/base/promotion_background.sub')
		promotionCodeBackground.SetPosition(12, 379)
		promotionCodeBackground.Show()

		self.children.append(promotionCodeBackground)

		promotionCodeEditlineBackground = ui.ImageBox()
		promotionCodeEditlineBackground.AddFlag('not_pick')
		promotionCodeEditlineBackground.LoadImage('d:/ymir work/ui/itemshop/base/promotion_editline.sub')
		promotionCodeEditlineBackground.SetParent(promotionCodeBackground)
		promotionCodeEditlineBackground.SetWindowHorizontalAlignCenter()
		promotionCodeEditlineBackground.SetWindowVerticalAlignCenter()
		promotionCodeEditlineBackground.SetPosition(0, 32)
		promotionCodeEditlineBackground.Show()
		self.children.append(promotionCodeEditlineBackground)
		promotionCodeEditLine = EditLine(
			event = None,
		)
		promotionCodeEditLine.SetParent(promotionCodeEditlineBackground)
		promotionCodeEditLine.SetPosition(6, 6)
		promotionCodeEditLine.SetEscapeEvent(lambda : [self.Close()])
		promotionCodeEditLine.SetSize(promotionCodeEditlineBackground.GetWidth() - 7, promotionCodeEditlineBackground.GetHeight() - 7)
		promotionCodeEditLine.SetMax(24)
		promotionCodeEditLine.SetFontName(localeInfo.UI_DEF_FONT_LARGE)
		promotionCodeEditLine.SetFontColor((219.0 / 255.0), (192.0 / 255.0), (174.0 / 255.0))
		promotionCodeEditLine.Show()
		self.promotionCodeEditLine = promotionCodeEditLine

		promotionCodeButton = ui.Button()
		promotionCodeButton.SetParent(promotionCodeBackground)
		promotionCodeButton.SetWindowHorizontalAlignCenter()
		promotionCodeButton.SetWindowVerticalAlignCenter()
		promotionCodeButton.SetUpVisual('d:/ymir work/ui/itemshop/base/promotion_redeem_button_up.sub')
		promotionCodeButton.SetDownVisual('d:/ymir work/ui/itemshop/base/promotion_redeem_button_down.sub')
		promotionCodeButton.SetOverVisual('d:/ymir work/ui/itemshop/base/promotion_redeem_button_over.sub')
		promotionCodeButton.SetEvent(self.SendPromotionCode)
		promotionCodeButton.SetPosition(145, 32)
		promotionCodeButton.Show()

		self.promotionCodeButton = promotionCodeButton

		promotionItemImage = ui.ImageBox()
		promotionItemImage.SetParent(promotionCodeBackground)
		promotionItemImage.SetWindowVerticalAlignCenter()
		promotionItemImage.SetPosition(45, 0)
		promotionItemImage.Hide()

		self.promotionItemImage = promotionItemImage

		promotionItemCount = ui.TextLine()
		promotionItemCount.SetParent(self.promotionItemImage)
		promotionItemCount.SetWindowVerticalAlignBottom()
		promotionItemCount.SetWindowHorizontalAlignRight()
		promotionItemCount.SetPosition(15, 15)
		promotionItemCount.Show()

		self.promotionItemCount = promotionItemCount

		promotionItemAnimation = ui.AniImageBox()
		promotionItemAnimation.SetParent(promotionCodeBackground)
		promotionItemAnimation.SetPosition(26, 19)
		promotionItemAnimation.OnEndFrame = self.OnEndFrame
		promotionItemAnimation.AddFlag('not_pick')
		promotionItemAnimation.SetDelay(1)
		for i in xrange(240):
			promotionItemAnimation.AppendImage('d:/ymir work/ui/itemshop/animation/promotion/%05d.png' % i)

		self.promotionItemAnimation = promotionItemAnimation

		promotionAnswerText = ui.TextLine()
		promotionAnswerText.SetParent(promotionCodeBackground)
		promotionAnswerText.SetWindowVerticalAlignBottom()
		promotionAnswerText.SetPosition(109, 17)
		promotionAnswerText.Show()

		self.promotionAnswerText = promotionAnswerText

	def CreatePaymentBoard(self):
		paymentBoard = ui.ImageBox()
		paymentBoard.SetParent(self.pages[PAGE_DEFAULT])
		paymentBoard.LoadImage('d:/ymir work/ui/itemshop/base/payment_background.sub')
		paymentBoard.SetPosition(445, 379)
		paymentBoard.Show()

		self.children.append(paymentBoard)

	def CreateAdvertiseBoard(self):
		advertiseBoard = ui.ImageBox()
		advertiseBoard.SetParent(self.pages[PAGE_DEFAULT])
		advertiseBoard.LoadImage('d:/ymir work/ui/itemshop/base/hyperlink_background.sub')
		advertiseBoard.SetPosition(600, 379)
		advertiseBoard.Show()

		self.children.append(advertiseBoard)

	def GetImage(self):
		if len(BANNER_CONTAINER) <= self.bannerIndex:
			self.bannerIndex = 0
		
		return BANNER_CONTAINER[self.bannerIndex]

	def OnUpdate(self):
		for item in self.listbox.GetItems():
			item.OnUpdate()

		if BANNER_CONTAINER and not self.bannerNextImageTime > app.GetTime():
			if self.bannerDecreaseAlpha:
				self.bannerValue -= BANNER_CHANGE_BY
				if self.bannerValue < MIN_ALPHA:
					self.bannerDecreaseAlpha = False
					self.bannerIndex += 1
					self.bannerImage.LoadImage(self.GetImage())
			else:
				self.bannerValue += BANNER_CHANGE_BY
				if self.bannerValue > MAX_ALPHA:
					self.bannerDecreaseAlpha = True
				self.bannerNextImageTime = app.GetTime() + IMAGE_CHANGE_TIME

			self.bannerImage.SetAlpha(self.bannerValue)

		if self.promotionItemImage.IsShow() and not self.promotionNextImageTime > app.GetTime():
			if self.promotionValue < MAX_ALPHA:
				self.promotionValue += PROMOTION_CHANGE_BY
				self.promotionNextImageTime = app.GetTime() + IMAGE_CHANGE_TIME
				self.promotionItemImage.SetAlpha(MINMAX(0.0, self.promotionValue, 1.0))

	def StartPromotionAnimation(self, answer):
		self.promotionCodeEditLine.SetText('')
		self.promotionAnswerText.SetText(ITEMSHOP_INFO['promotion_code'][answer]['text'])
		self.promotionAnswerText.SetPackedFontColor(ITEMSHOP_INFO['promotion_code'][answer]['color'])
		self.promotionAnswerText.Show()

		if answer != 1:
			return

		self.promotionItemAnimation.ResetFrame()
		self.OnEndFrame()

	def OnEndFrame(self):
		if not constInfo.PROMOTION_CODE_REWARDS:
			self.promotionItemAnimation.Hide()
			self.promotionItemImage.Hide()
			return

		reward = constInfo.PROMOTION_CODE_REWARDS[0]
		item.SelectItem(reward['vnum'])
		self.promotionItemImage.LoadImage(item.GetIconImageFileName())
		self.promotionValue = 0.0
		self.promotionItemImage.SetAlpha(self.promotionValue)

		self.promotionItemCount.SetText(str(reward['count']) if reward['count'] > 1 else '')
		self.promotionItemImage.Show()
		self.promotionItemAnimation.Show()
		constInfo.PROMOTION_CODE_REWARDS.remove(reward)

	def SendPromotionCode(self):
		code = self.promotionCodeEditLine.GetText()
		net.SendRedeemPromotionCodePacket(code)

	def LoadCategorys(self):
		self.categoryListbox.RemoveAllItems()

		for i, content in enumerate(constInfo.ITEMSHOP_INFOS):
			contentButton = CategoryButton(content, i)
			contentButton.SetOnMouseLeftButtonUpEvent(lambda arg = i : self.SelectCategory(arg))

			self.categoryListbox.AppendItem(contentButton)

		if self.categoryListbox.GetItemCountX() * self.categoryListbox.GetItemCountY() < self.categoryListbox.GetItemCount():
			self.categoryScrollBar.SetMiddleBarSize(float(self.categoryListbox.GetItemCountX() * self.categoryListbox.GetItemCountY())/self.categoryListbox.GetItemCount())
			self.categoryScrollBar.SetPos(0)
			self.categoryScrollBar.Show()
		else:
			self.categoryScrollBar.Hide()

		if self.cur_category >= len(constInfo.ITEMSHOP_INFOS):
			self.cur_category = 0

	def SelectCategory(self, category):
		if self.toolTip:
			self.toolTip.HideToolTip()
		self.listbox.RemoveAllItems()

		if not constInfo.ITEMSHOP_INFOS:
			return

		self.categoryListbox.GetItems()[self.cur_category].SetUp()
		self.cur_category = category
		self.categoryListbox.GetItems()[self.cur_category].SetDown()

		searchText = self.editLine.GetText()

		for i, itemObjects in enumerate(constInfo.ITEMSHOP_INFOS):
			if i == category or searchText:
				itemObjects = itemObjects['items']
				for itemObject in itemObjects:
					vnum = itemObject['vnum']
					item.SelectItem(vnum)
					name = item.GetItemName()

					if not searchText.lower() in name.lower():
						continue

					itemContainer = ItemSlot(
						itemObject,
						self.toolTip,
						self
					)

					self.listbox.AppendItem(itemContainer)

		if self.listbox.GetItemCountX() * self.listbox.GetItemCountY() < self.listbox.GetItemCount():
			self.scrollBar.SetMiddleBarSize(float(self.listbox.GetItemCountX() * self.listbox.GetItemCountY()) / self.listbox.GetItemCount())
			self.scrollBar.SetPos(0)
			self.scrollBar.Show()
		else:
			self.scrollBar.Hide()

	def SelectPage(self, num):
		self.ResetEditLine()
		for page in self.pages:
			page.Hide()

		for tab in self.tabs:
			tab.Enable()
			tab.SetUp()

		self.pages[num].Show()

		self.tabs[num].Disable()
		self.tabs[num].Down()

		self.SetTitleName(ITEMSHOP_INFO['pages'][num]['text'])

	def ResetEditLine(self):
		self.editLine.KillFocus()
		self.promotionCodeEditLine.KillFocus()
		for item in self.listbox.GetItems():
			item.ResetEditLine()

		for item in self.randomListbox.GetItems():
			item.ResetEditLine()

	def __OnType(self):
		self.SelectCategory(self.cur_category)

	def ReloadItemshop(self):
		self.LoadCategorys()
		self.SelectCategory(self.cur_category)
		self.SetRandomStartpageItems()

	def ReloadItemshopItem(self, item):
		for category in constInfo.ITEMSHOP_INFOS:
			for i, olditem in enumerate(category['items']):
				if olditem['hash'] == item['hash']:
					category['items'][i] = item
					return

	def ReloadItemshopContainer(self, item):
		for slot in self.randomListbox.GetItems():
			if slot.GetHash() == item['hash']:
				slot.LoadItemData(item)
				break

		for slot in self.listbox.GetItems():
			if slot.GetHash() == item['hash']:
				slot.LoadItemData(item)
				return

	def RemoveItemshopItem(self, hash):
		for i, category in enumerate(constInfo.ITEMSHOP_INFOS):
			for j, olditem in enumerate(category['items']):
				if olditem['hash'] == hash:
					del category['items'][j]
					if not category['items']:
						del constInfo.ITEMSHOP_INFOS[i]
						self.ReloadItemshop()
					return

	def RemoveItemFromItemshopContainer(self, hash):
		for slot in self.randomListbox.GetItems():
			if slot.GetHash() == hash:
				self.SetRandomStartpageItems()
				break

		for slot in self.listbox.GetItems():
			if slot.GetHash() == hash:
				slot.Hide()
				self.listbox.RemoveItem(slot)
				return

	def AddItemshopItem(self, obj):
		for category in constInfo.ITEMSHOP_INFOS:
			if obj['index'] == category['index']:
				category['items'][0:0] = [obj['items'][0]]
				self.SelectCategory(self.cur_category)
				return

		constInfo.ITEMSHOP_INFOS[0:0] = [obj]
		self.ReloadItemshop()

	def SetCoins(self, coins):
		self.coinText.SetText('< %s >' % localeInfo.NumberToDecimalString(coins))
		self.userText.SetText('< %s >' % player.GetMainCharacterName())

	def RequestBuyItem(self, hash, count):
		self.questionDialog = uiCommon.QuestionDialog()
		self.questionDialog.SetText(ITEMSHOP_INFO['strings']['BUY_QUESTION'])
		self.questionDialog.SetAcceptEvent(lambda hash = hash, count = count : self.__RequestBuyItemQuestionDialog_OnAccept(hash, count))
		self.questionDialog.SetCancelEvent(self.__RequestBuyItemQuestionDialog_OnCancel)
		self.questionDialog.Open()

		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def __RequestBuyItemQuestionDialog_OnAccept(self, hash, count):
		net.SendBuyItemshopItemPacket(hash, count)
		self.OnCloseQuestionDialog()

	def __RequestBuyItemQuestionDialog_OnCancel(self):
		self.OnCloseQuestionDialog()

	def OnCloseQuestionDialog(self):
		if not self.questionDialog:
			return
		
		self.questionDialog.Close()
		self.questionDialog = None
		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnPressExitKey(self):
		self.Close()
		return True

# NOTE: CategoryButton class
class CategoryButton(ui.ImageBox):
	def __init__(self,
		content,
		index
		):
		ui.ImageBox.__init__(self)

		self.LoadImage('d:/ymir work/ui/itemshop/base/category_slot_%d.sub' % (index % 2))

		text = ui.TextLine()
		text.SetParent(self)
		text.SetText(content['category'])
		text.SetPosition(55, 0)
		text.AddFlag('not_pick')
		text.SetWindowVerticalAlignCenter()
		text.SetSize(text.GetTextSize()[0], text.GetTextSize()[1])
		text.Show()

		self.text = text

		slot = ui.ImageBox()
		slot.SetParent(self)
		slot.AddFlag('not_pick')
		slot.LoadImage('d:/ymir work/ui/itemshop/base/category_overlay.sub')
		slot.SetPosition(-9, -9)

		self.slot = slot

		image = ui.ImageBox()
		image.SetParent(self)
		image.AddFlag('not_pick')
		icon_image = ICON_PATH + content['icon']
		if content['icon'].find('.') == -1:
			item.SelectItem(int(content['icon']))
			icon_image = item.GetIconImageFileName()
		image.LoadImage(icon_image)
		image.SetPosition(29 - image.GetWidth() / 2, 24 - image.GetHeight() / 2)
		image.Show()

		self.image = image

	def __del__(self):
		ui.ImageBox.__del__(self)

	def SetUp(self):
		self.slot.Hide()

	def SetDown(self):
		self.slot.Show()

class ItemSlot(ui.ImageBox):
	def __init__(self,
			item_info,
			toolTip,
			parent = None
		):
		ui.ImageBox.__init__(self)

		self.LoadImage('d:/ymir work/ui/itemshop/base/itemslot.sub')

		self.itemVnum = item_info['vnum']
		self.metinSlot = item_info['sockets']
		self.attrSlot = item_info['attr']
		self.price = item_info['price']
		self.hash = item_info['hash']
		self.count = item_info['count']
		self.discount = item_info['discount']
		self.limitCount = item_info['limitCount']
		self.endTime = item_info['endTime']

		self.toolTip = toolTip
		self.parent = proxy(parent) if parent else None

		if not ENABLE_OTHER_TOOLTIP_FUNCTION:
			self.SAFE_SetStringEvent('MOUSE_OVER_IN', self.ShowToolTip)
			self.SAFE_SetStringEvent('MOUSE_OVER_OUT', self.HideToolTip)

		item.SelectItem(int(self.itemVnum))

		self.CreateSpecialOfferElements()
		self.CreateDiscountElements()
		self.CreateLimitedCountElements()
		self.CreateEditline()
		self.CreateButtons()
		self.CreateImage()
		self.CreateItemName()
		self.CreateFancyCorner()

		if self.discount:
			self.discountImage.Show()
			self.discountPriceText.Show()
		
		if self.limitCount > 0:
			self.limitedCountImage.Show()
			self.limitedCountBackground.Show()

		if self.endTime:
			self.specialOfferCoutdown.Show()
			self.specialOfferBackground.Show()

		self.UpdatePriceValue()

	def __del__(self):
		ui.ImageBox.__del__(self)
		if self.toolTip:
			self.toolTip.HideToolTip()
		self.toolTip = None
		self.specialOfferBackground = None
		self.buttonText = None
		self.buyButton = None
		self.icon = None
		self.discountImage = None
		self.parent = None

	if ENABLE_OTHER_TOOLTIP_FUNCTION:
		def OnMouseOverIn(self):
			self.ShowToolTip()

		def OnMouseOverOut(self):
			self.HideToolTip()

	# NOTE: We just use this to reload items
	def LoadItemData(self, item_info):
		self.itemVnum = item_info['vnum']
		self.metinSlot = item_info['sockets']
		self.attrSlot = item_info['attr']
		self.price = item_info['price']
		self.hash = item_info['hash']
		self.count = item_info['count']
		self.discount = item_info['discount']
		self.limitCount = item_info['limitCount']
		self.endTime = item_info['endTime']

		item.SelectItem(int(self.itemVnum))

		if self.discount:
			self.discountImage.Show()
			self.discountPriceText.Show()
		else:
			self.discountImage.Hide()
			self.discountPriceText.Hide()
		
		if self.limitCount > 0:
			self.limitedCountImage.Show()
			self.limitedCountBackground.Show()
		else:
			self.limitedCountImage.Hide()
			self.limitedCountBackground.Hide()

		if self.endTime:
			self.specialOfferCoutdown.Show()
			self.specialOfferBackground.Show()
		else:
			self.specialOfferCoutdown.Hide()
			self.specialOfferBackground.Hide()

		self.icon.LoadImage(item.GetIconImageFileName())
		self.itemNameText.SetText(item.GetItemName())
		
		self.UpdatePriceValue()

	def ShowToolTip(self):
		if self.toolTip:
			# TODO: Create SetItemshop item with extra info
			self.toolTip.ClearToolTip()
			self.toolTip.AddItemData(self.itemVnum, self.metinSlot, self.attrSlot)

	def HideToolTip(self):
		if self.toolTip:
			self.toolTip.HideToolTip()

	def GetHash(self):
		return self.hash

	def CreateEditline(self):
		editlineBackground = ui.ImageBox()
		editlineBackground.AddFlag('not_pick')
		editlineBackground.LoadImage('d:/ymir work/ui/itemshop/base/small_editline_background.sub')
		editlineBackground.SetParent(self)
		editlineBackground.SetWindowHorizontalAlignCenter()
		editlineBackground.SetWindowVerticalAlignBottom()
		editlineBackground.SetPosition(31, 62)
		editlineBackground.Show()
		self.editlineBackground = editlineBackground
		editLine = EditLine(
			event = self.UpdatePriceValue
		)
		editLine.SetParent(self.editlineBackground)
		editLine.SetPosition(1, 0)
		editLine.SetEscapeEvent(lambda : editLine.KillFocus())
		editLine.SetWindowHorizontalAlignCenter()
		editLine.SetSize(self.editlineBackground.GetWidth() - 1, self.editlineBackground.GetHeight())
		editLine.SetMax(len(str(STACK_MAX)))
		editLine.SetNumberMode()
		editLine.SetFontColor((219.0 / 255.0), (192.0 / 255.0), (174.0 / 255.0))
		editLine.SetText('1')
		editLine.Show()
		self.editLine = editLine

	def CreateButtons(self):
		buyButton = ui.ImageBox()
		buyButton.SetParent(self)
		buyButton.LoadImage('d:/ymir work/ui/itemshop/base/buy_button.sub')
		buyButton.SetPosition(30, 45)
		buyButton.SetWindowVerticalAlignBottom()
		buyButton.SetWindowHorizontalAlignCenter()
		buyButton.SetOnMouseLeftButtonUpEvent(self.RequestBuyItem)
		buyButton.Show()

		self.buyButton = buyButton

		buttonText = ui.TextLine()
		buttonText.SetParent(self.buyButton)
		buttonText.AddFlag('not_pick')
		buttonText.SetFontName(localeInfo.UI_DEF_FONT_LARGE)
		buttonText.SetPackedFontColor(0xffdad3c9)
		buttonText.SetHorizontalAlignCenter()
		buttonText.SetVerticalAlignCenter()
		buttonText.SetWindowHorizontalAlignCenter()
		buttonText.SetWindowVerticalAlignCenter()
		buttonText.Show()

		self.buttonText = buttonText

		coinIcon = ui.ImageBox()
		coinIcon.SetParent(self.buttonText)
		coinIcon.AddFlag('not_pick')
		coinIcon.LoadImage('d:/ymir work/ui/itemshop/base/coin_icon.sub')
		coinIcon.Show()

		self.coinIcon = coinIcon

		increaseButton = ui.Button()
		increaseButton.SetParent(self)
		increaseButton.SetPosition(62, 62)
		increaseButton.SetWindowHorizontalAlignCenter()
		increaseButton.SetWindowVerticalAlignBottom()
		increaseButton.SetUpVisual('d:/ymir work/ui/itemshop/base/increase_button_up.sub')
		increaseButton.SetDownVisual('d:/ymir work/ui/itemshop/base/increase_button_down.sub')
		increaseButton.SetOverVisual('d:/ymir work/ui/itemshop/base/increase_button_over.sub')
		increaseButton.SetEvent(self.IncreaseValue)

		self.increaseButton = increaseButton

		decreaseButton = ui.Button()
		decreaseButton.SetParent(self)
		decreaseButton.SetPosition(-2, 62)
		decreaseButton.SetWindowHorizontalAlignCenter()
		decreaseButton.SetWindowVerticalAlignBottom()
		decreaseButton.SetUpVisual('d:/ymir work/ui/itemshop/base/decrease_button_up.sub')
		decreaseButton.SetDownVisual('d:/ymir work/ui/itemshop/base/decrease_button_down.sub')
		decreaseButton.SetOverVisual('d:/ymir work/ui/itemshop/base/decrease_button_over.sub')
		decreaseButton.SetEvent(self.DecreaseValue)

		self.decreaseButton = decreaseButton

	def ResetEditLine(self):
		self.editLine.KillFocus()

	def RequestBuyItem(self):
		self.parent.RequestBuyItem(self.hash, self.buyCount)

	def IncreaseValue(self):
		self.editLine.SetText(str(self.editLine.GetTextAsValue(1)+1))
		self.UpdatePriceValue()

	def DecreaseValue(self):
		self.editLine.SetText(str(self.editLine.GetTextAsValue(1)-1))
		self.UpdatePriceValue()

	def UpdatePriceValue(self):
		number = MINMAX(1, int(self.editLine.GetTextAsValue(1)), int(STACK_MAX / self.count))
		self.editLine.SetText(str(number))
		item.SelectItem(self.itemVnum)
		if item.IsAntiFlag(item.ITEM_ANTIFLAG_STACK) or not item.IsFlag(item.ITEM_FLAG_STACKABLE) or self.limitCount > 0:
			self.increaseButton.Hide()
			self.decreaseButton.Hide()
			self.editlineBackground.Hide()
		elif number >= int(STACK_MAX / self.count):
			self.increaseButton.Hide()
			self.decreaseButton.Show()
		elif number <= 1:
			self.decreaseButton.Hide()
			self.increaseButton.Show()
		else:
			self.increaseButton.Show()
			self.decreaseButton.Show()
			self.editlineBackground.Show()

		self.buyCount = number

		self.buttonText.SetText(
			'%d' % (
					max(1,
						self.price - (self.price * self.discount / 100)
				) * self.buyCount
			)
		)
		if self.discount:
			self.discountPriceText.SetText(
				ITEMSHOP_INFO['strings']['ITEM_PRICE'] % (self.price * self.buyCount)
			)
			self.discountPriceText.SetSize(self.discountPriceText.GetTextSize()[0], self.discountPriceText.GetTextSize()[1])
			line = ''
			for _ in xrange(len(self.discountPriceText.GetText())):
				line += '_'
			self.discountPriceOverlay.SetText(line)

		self.coinIcon.SetPosition(-(self.buttonText.GetTextSize()[0] / 2 + 14), -3)
		self.limitedCountText.SetText(ITEMSHOP_INFO['strings']['ITEMS_LEFT'] % localeInfo.NumberToDecimalString(self.limitCount))

	def CreateImage(self):
		icon = ui.ExpandedImageBox()
		icon.SetParent(self)
		icon.AddFlag('not_pick')
		icon.LoadImage(item.GetIconImageFileName())
		icon.SetPosition(40, 0)
		icon.SetWindowVerticalAlignCenter()
		icon.Show()
		self.icon = icon

	def CreateItemName(self):
		itemNameText = ui.TextLine()
		itemNameText.SetParent(self)
		itemNameText.AddFlag('not_pick')
		itemNameText.SetFontColor((219.0 / 255.0), (192.0 / 255.0), (174.0 / 255.0))
		itemNameText.SetText(item.GetItemName() if self.count <= 1 else item.GetItemName() + ' (x%d)' % self.count
		)
		itemNameText.SetHorizontalAlignCenter()
		itemNameText.SetWindowHorizontalAlignCenter()
		itemNameText.SetPosition(0, 15)
		itemNameText.Show()

		self.itemNameText = itemNameText

	def CreateSpecialOfferElements(self):
		specialOfferBackground = ui.AniImageBox()
		specialOfferBackground.SetParent(self)
		specialOfferBackground.SetPosition(3, 3)
		specialOfferBackground.AddFlag('not_pick')
		specialOfferBackground.SetDelay(1)
		for i in xrange(120):
			specialOfferBackground.AppendImage('d:/ymir work/ui/itemshop/animation/hotoffer/%05d.png' % i)
		self.specialOfferBackground = specialOfferBackground

		specialOfferCoutdown = ui.TextLine()
		specialOfferCoutdown.SetParent(self)
		specialOfferCoutdown.AddFlag('not_pick')
		specialOfferCoutdown.SetHorizontalAlignCenter()
		specialOfferCoutdown.SetWindowVerticalAlignBottom()
		specialOfferCoutdown.SetWindowHorizontalAlignCenter()
		specialOfferCoutdown.SetFontColor((219.0 / 255.0), (192.0 / 255.0), (174.0 / 255.0))
		specialOfferCoutdown.SetPosition(30, 77)

		self.specialOfferCoutdown = specialOfferCoutdown

	def CreateFancyCorner(self):
		fancyCorner = ui.ImageBox()
		fancyCorner.SetParent(self)
		fancyCorner.AddFlag('not_pick')
		fancyCorner.LoadImage('d:/ymir work/ui/itemshop/base/itemslot_right_bottom.sub')
		fancyCorner.SetPosition(fancyCorner.GetWidth(), fancyCorner.GetHeight())
		fancyCorner.SetWindowHorizontalAlignRight()
		fancyCorner.SetWindowVerticalAlignBottom()
		fancyCorner.Show()

		self.fancyCorner = fancyCorner

	def CreateDiscountElements(self):
		discountImage = ui.ImageBox()
		discountImage.SetParent(self)
		discountImage.AddFlag('not_pick')
		discountImage.LoadImage('d:/ymir work/ui/itemshop/base/badge_sale.sub')
		discountImage.SetPosition(discountImage.GetWidth() + 3, 3)
		discountImage.SetWindowHorizontalAlignRight()

		self.discountImage = discountImage

		discountPriceText = ui.TextLine()
		discountPriceText.SetParent(self)
		discountPriceText.AddFlag('not_pick')
		discountPriceText.SetPosition(30, 20)
		discountPriceText.SetPackedFontColor(0xff69210e)
		discountPriceText.SetWindowVerticalAlignBottom()
		discountPriceText.SetWindowHorizontalAlignCenter()

		self.discountPriceText = discountPriceText

		discountPriceOverlay = ui.TextLine()
		discountPriceOverlay.SetParent(self.discountPriceText)
		discountPriceOverlay.AddFlag('not_pick')
		discountPriceOverlay.SetPosition(0, -5)
		discountPriceOverlay.SetPackedFontColor(0xff69210e)
		discountPriceOverlay.Show()

		self.discountPriceOverlay = discountPriceOverlay

	def CreateLimitedCountElements(self):
		limitedCountImage = ui.ImageBox()
		limitedCountImage.SetParent(self)
		limitedCountImage.SetPosition(3, 3)
		limitedCountImage.AddFlag('not_pick')
		limitedCountImage.LoadImage('d:/ymir work/ui/itemshop/base/badge_limitedcount.sub')

		self.limitedCountImage = limitedCountImage

		limitedCountBackground = ui.ImageBox()
		limitedCountBackground.SetParent(self)
		limitedCountBackground.AddFlag('not_pick')
		limitedCountBackground.LoadImage('d:/ymir work/ui/itemshop/base/limitedcount_background.sub')
		limitedCountBackground.SetWindowVerticalAlignBottom()
		limitedCountBackground.SetPosition(2, 16)
		
		self.limitedCountBackground = limitedCountBackground

		limitedCountText = ui.TextLine()
		limitedCountText.SetParent(self.limitedCountBackground)
		limitedCountText.AddFlag('not_pick')
		limitedCountText.SetFontColor((194.0 / 255.0), (186.0 / 255.0), (174.0 / 255.0))
		limitedCountText.SetPosition(2, 1)
		limitedCountText.Show()

		self.limitedCountText = limitedCountText

	def OnUpdate(self):
		if self.endTime:
			diff = self.endTime - app.GetUNIX()
			hours = diff / 3600
			minutes = diff / 60 % 60
			seconds = diff % 60
			self.specialOfferCoutdown.SetText('%d:%02d:%02d' % (hours, minutes, seconds))

class MiddleBar(ui.DragButton):
	def __init__(self,
			width,
			height,
			move_event,
			wheel_event,
			color,
		):
		ui.DragButton.__init__(self)
		self.AddFlag('movable')
		self.MakeImage(color)
		self.SetSize(width, height)
		self.SetMoveEvent(move_event)
		self.SetOnMouseWheel(wheel_event)
		self.Show()

	def MakeImage(self, color):
		top = ui.ImageBox()
		top.SetParent(self)
		top.LoadImage('d:/ymir work/ui/itemshop/base/scrollbar_middle_%s.tga' % color)
		top.SetPosition(0, 0)
		top.AddFlag('not_pick')
		top.Show()

		middle = ui.ExpandedImageBox()
		middle.SetParent(self)
		middle.LoadImage('d:/ymir work/ui/itemshop/base/scrollbar_middle_%s.tga' % color)
		middle.SetPosition(0, 4)
		middle.AddFlag('not_pick')
		middle.Show()

		bottom = ui.ImageBox()
		bottom.SetParent(self)
		bottom.LoadImage('d:/ymir work/ui/itemshop/base/scrollbar_middle_%s.tga' % color)
		bottom.AddFlag('not_pick')
		bottom.Show()

		self.top = top
		self.bottom = bottom
		self.middle = middle

	def SetSize(self, width, height):
		width = min(10, width)
		height = max(12, height)
		ui.DragButton.SetSize(self, width, height)

		self.bottom.SetPosition(0, height-4)
		height -= 4*3
		self.middle.SetRenderingRect(0, 0, 0, float(height)/4.0)

	def SetOnMouseWheel(self, onMouseWheel):
		self.OnMouseWheel = onMouseWheel

class ScrollBar(ui.Window):
	SCROLLBAR_MIDDLE_WIDTH = 6
	SCROLLBAR_MIDDLE_HEIGHT = 4

	def __init__(self,
			parent,
			width,
			height,
			pos,
			color,
		):
		ui.Window.__init__(self)

		self.ResetWindow()

		self.SetParent(parent)
		self.SetSize(width, height)
		self.SetPosition(pos[0], pos[1])

		self.CreateScrollBar(color)
		self.SetScrollBarHeight(height)

		self.Show()

	def __del__(self):
		ui.Window.__del__(self)
		self.ResetWindow()

	def ResetWindow(self):
		self.pageSize = 1
		self.curPos = 0.0
		self.eventScroll = lambda *arg: None
		self.scrollStep = 0.21

	def CreateScrollBar(self, color):
		barSlot = ui.Bar3D()
		barSlot.SetParent(self)
		barSlot.AddFlag('not_pick')
		barSlot.Show()
		self.barSlot = barSlot

		import weakref
		scrollBarWeakRef = weakref.ref(self)
		
		def OnMouseWheel(nLen):
			scrollBar = scrollBarWeakRef()
			
			if scrollBar:
				if nLen > 0:
					scrollBar.OnUp()
				else:
					scrollBar.OnDown()

		middleBar = MiddleBar(
				width = self.SCROLLBAR_MIDDLE_WIDTH, 
				height = self.SCROLLBAR_MIDDLE_HEIGHT,
				move_event = ui.__mem_func__(self.OnMove),
				wheel_event = OnMouseWheel,
				color = color,
		)
		middleBar.SetParent(self)
		self.middleBar = middleBar

	def Destroy(self):
		self.middleBar = None
		self.eventScroll = lambda *arg: None

	def SetScrollEvent(self, event):
		self.eventScroll = event

	def SetMiddleBarSize(self, pageScale):
		self.SCROLLBAR_MIDDLE_HEIGHT = int(pageScale * float(self.GetHeight()))
		self.middleBar.SetSize(self.SCROLLBAR_MIDDLE_WIDTH, self.SCROLLBAR_MIDDLE_HEIGHT)
		self.pageSize = self.GetHeight() - self.SCROLLBAR_MIDDLE_HEIGHT

	def SetScrollBarHeight(self, height):
		self.pageSize = height - self.SCROLLBAR_MIDDLE_HEIGHT
		self.middleBar.SetRestrictMovementArea(0, 0, 0, height)

		self.UpdateBarSlot()

	def UpdateBarSlot(self):
		self.barSlot.SetPosition(0, 0)
		self.barSlot.SetSize(self.GetWidth()-2, self.GetHeight())

	def GetPos(self):
		return self.curPos

	def SetPos(self, pos):
		pos = max(0.0, pos)
		pos = min(1.0, pos)

		newPos = float(self.pageSize) * pos
		self.middleBar.SetPosition(0, int(newPos))
		self.OnMove()

	def SetScrollStep(self, step):
		self.scrollStep = step
	
	def GetScrollStep(self):
		return self.scrollStep
		
	def OnMouseWheel(self, nLen):
		if nLen > 0:
			self.OnUp()
		else:
			self.OnDown()
		
	def OnUp(self):
		self.SetPos(self.curPos-self.scrollStep)

	def OnDown(self):
		self.SetPos(self.curPos+self.scrollStep)

	def OnMove(self):
		if 0 == self.pageSize:
			return

		(xLocal, yLocal) = self.middleBar.GetLocalPosition()
		self.curPos = float(yLocal) / float(self.pageSize)

		self.eventScroll()

	def OnMouseLeftButtonDown(self):
		(xMouseLocalPosition, yMouseLocalPosition) = self.GetMouseLocalPosition()
		pickedPos = yMouseLocalPosition - self.SCROLLBAR_MIDDLE_HEIGHT/2
		newPos = float(pickedPos) / float(self.pageSize)
		self.SetPos(newPos)

class ListBox(ui.Window):
	def __init__(self,
			parent,
			itemWidth,
			itemHeight,
			item_count_x,
			item_count_y,
			item_gap_x,
			item_gap_y,
			pos,
			scrollBar = None
		):
		ui.Window.__init__(self)
		self.itemWidth = itemWidth
		self.itemHeight = itemHeight
		self.item_count_x = item_count_x
		self.item_count_y = item_count_y
		self.item_gap_x = item_gap_x
		self.item_gap_y = item_gap_y
		self.ResetWindow()

		self.SetParent(parent)
		self.SetPosition(pos[0], pos[1])

		if scrollBar:
			self.SetScrollBar(scrollBar)
		self.__UpdateSize()

		self.Show()

	def __del__(self):
		ui.Window.__del__(self)
		self.ResetWindow()

	def ResetWindow(self):
		self.basePos = 0
		self.itemList = []
		self.scrollBar = None

	def __UpdateSize(self):
		self.SetSize((self.itemWidth + self.item_gap_x) * self.item_count_x - self.item_gap_x, (self.itemHeight + self.item_gap_y) * self.item_count_y - self.item_gap_y)

	def SetBasePos(self, basePos):
		self.basePos=basePos

		for pos, newItem in enumerate(self.itemList):
			newItem.Hide()

			if self.__IsInViewRange(pos):
				(x, y) = self.GetItemViewCoord(pos)
				newItem.SetPosition(x, y)
				newItem.Show()

	def RemoveItem(self, item):
		self.itemList.remove(item)
		self.SetBasePos(self.basePos)

	def RemoveAllItems(self):
		for item in self.itemList:
			item.Hide()
			item.Destroy()
			del item

		self.itemList=[]

		self.SetBasePos(0)

		if self.scrollBar:
			self.scrollBar.Hide()

	def AppendItem(self, newItem):
		newItem.SetParent(self)
		newItem.SetSize(self.itemWidth, self.itemHeight)
		newItem.OnMouseWheel = self.OnMouseWheel

		pos=len(self.itemList) 
		if self.__IsInViewRange(pos):
			(x, y)=self.GetItemViewCoord(pos)
			newItem.SetPosition(x, y)
			newItem.Show()
		else:
			newItem.Hide()

		self.itemList.append(newItem)

	def OnMouseWheel(self, nLen):
		if self.scrollBar:
			self.scrollBar.OnMouseWheel(nLen)

	def SetScrollBar(self, scrollBar):
		scrollBar.SetScrollEvent(ui.__mem_func__(self.__OnScroll))
		self.scrollBar=scrollBar

	def GetItemCountY(self):
		return self.item_count_y

	def GetItemCountX(self):
		return self.item_count_x

	def GetItems(self):
		return self.itemList

	def GetItemCount(self):
		return len(self.itemList)

	def GetItemViewCoord(self, pos):
		return (pos % self.item_count_x * (self.itemWidth + self.item_gap_x), 
		math.floor(
			(pos-self.basePos) / float(self.item_count_x)
			) * (self.itemHeight + self.item_gap_y)
		)

	def __OnScroll(self):
		self.SetBasePos(int(self.scrollBar.GetPos() * self.__GetScrollLen()) * self.item_count_x)

	def __GetScrollLen(self):
		scrollLen = math.ceil((self.GetItemCount() - (self.item_count_x * self.item_count_y)) / float(self.item_count_x))
		if scrollLen < 0:
			return 0

		return scrollLen

	def __IsInViewRange(self, pos):
		if pos < self.basePos:
			return False
		if pos >= self.basePos + (self.item_count_y * self.item_count_x):
			return False
		return True

class EditLine(ui.EditLine):
	def __init__(self,
		 event
		):
		ui.EditLine.__init__(self)
		self.eventOnType = event if event else ui.Window.NoneMethod

	def __del__(self):
		ui.EditLine.__del__(self)
		self.eventOnType = ui.Window.NoneMethod

	def GetTextAsValue(self, default):
		return default if ui.TextLine.GetText(self) == '' else int(ui.TextLine.GetText(self))

	def OnIMEUpdate(self):
		snd.PlaySound('sound/ui/type.wav')
		imeText = ime.GetText(self.bCodePage)
		oldText = ui.TextLine.GetText(self)

		ui.TextLine.SetText(self, imeText)

		if not imeText == oldText:
			self.eventOnType()

# NOTE: Global functions
def MINMAX(min, value, max):
	if value < min:
		value = min
	if value > max:
		value = max
	return value