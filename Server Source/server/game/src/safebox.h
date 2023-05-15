#pragma once

class CHARACTER;
class CItem;
class CGrid;

class CSafebox
{
public:
	CSafebox(LPCHARACTER pkChrOwner, int iSize, long long lldGold);
	~CSafebox();

	bool			Add(DWORD dwPos, LPITEM pkItem);
	LPITEM			Get(DWORD dwPos);
	LPITEM			Remove(DWORD dwPos);
	void			ChangeSize(int iSize);

	bool			MoveItem(BYTE bCell, BYTE bDestCell, WORD count);
	LPITEM			GetItem(BYTE bCell);

	void			Save();

	bool			IsEmpty(DWORD dwPos, BYTE bSize);
#if defined(ITEM_CHECKINOUT_UPDATE)
	int				GetEmptySafebox(BYTE size);
#endif
	bool			IsValidPosition(DWORD dwPos);

	void			SetWindowMode(BYTE bWindowMode);
	unsigned int	GetGridTotalSize() const;

protected:
	void		__Destroy();

	LPCHARACTER	m_pkChrOwner;
	LPITEM		m_pkItems[SAFEBOX_MAX_NUM];
	int			m_iSize;
	long long	m_lldGold;
	BYTE		m_bWindowMode;
	std::vector<std::shared_ptr<CGrid>> v_Grid;
};
