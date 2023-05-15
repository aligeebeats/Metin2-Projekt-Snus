#pragma once
#pragma pack(1)

struct TGA_HEADER
{
    char idLen;
    char palType;
    char imgType;
    WORD colorBegin;
    WORD colorCount;
    char palEntrySize;
    WORD left;
    WORD top;
    WORD width;
    WORD height;
    char colorBits;
    char desc;
};

#define IMAGEDESC_ORIGIN_MASK	0x30
#define IMAGEDESC_TOPLEFT	0x20
#define IMAGEDESC_BOTLEFT	0x00
#define IMAGEDESC_BOTRIGHT	0x10
#define IMAGEDESC_TOPRIGHT	0x30

#pragma pack()

class CTargaImage
{
	public:
		CTargaImage();
		~CTargaImage();

		void	Create(int x, int y);
		char *	GetBasePointer(int line = 0);
		bool	Save(const char * filename);

	protected:
		TGA_HEADER	m_header;
		char *		m_pbuf;
		int		m_x, m_y;
};
