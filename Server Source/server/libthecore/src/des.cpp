#include "stdafx.h"
#include "DES_table.h"

#define DES_ECB_ENCRYPT 0
#define DES_ECB_DECRYPT 1

extern DWORD SP_boxes[8][64];

#define BYTES_TO_DWORD(b,d) (d  = ((DWORD)(*((b)++))), \
                             d |= ((DWORD)(*((b)++)))<< 8, \
                             d |= ((DWORD)(*((b)++)))<<16, \
                             d |= ((DWORD)(*((b)++)))<<24)

#define DWORD_TO_4BYTES(d,b) (*((b)++)=(BYTE)(((d)   )&0xff), \
                              *((b)++)=(BYTE)(((d)>> 8)&0xff), \
                              *((b)++)=(BYTE)(((d)>>16)&0xff), \
                              *((b)++)=(BYTE)(((d)>>24)&0xff))

#define PERMUTATION(a,b,t,n,m) ((t)=((((a)>>(n))^(b))&(m)),\
	(b)^=(t),\
	(a)^=((t)<<(n)))

#define HPERMUTATION(a,t,n,m) ((t)=((((a)<<(16-(n)))^(a))&(m)),\
	(a)=(a)^(t)^(t>>(16-(n))))

#define D_ENCRYPT(Left, Right, Ks, Num, TmpA, TmpB) \
	TmpA = (Right ^ Ks[Num  ]); \
	TmpB = (Right ^ Ks[Num+1]); \
	TmpB = ((TmpB >> 4) + (TmpB << 28)); \
	Left ^=	SP_boxes[1][(TmpB   )&0x3f]| \
	SP_boxes[3][(TmpB>> 8)&0x3f]| \
	SP_boxes[5][(TmpB>>16)&0x3f]| \
	SP_boxes[7][(TmpB>>24)&0x3f]| \
	SP_boxes[0][(TmpA   )&0x3f]| \
	SP_boxes[2][(TmpA>> 8)&0x3f]| \
	SP_boxes[4][(TmpA>>16)&0x3f]| \
	SP_boxes[6][(TmpA>>24)&0x3f];

void DES_ECB_mode(BYTE * Input,
                  BYTE * Output,
                  const DWORD * KeySchedule,
                  BYTE Operation)
{
    static BYTE * bInp, * bOut;
    static DWORD dwLeft, dwRigh, dwTmp, dwTmp1;

    bInp = Input;
    bOut = Output;

    BYTES_TO_DWORD(bInp, dwLeft);
    BYTES_TO_DWORD(bInp, dwRigh);

    PERMUTATION(dwRigh, dwLeft, dwTmp, 4, 0x0f0f0f0f);
    PERMUTATION(dwLeft, dwRigh, dwTmp,16, 0x0000ffff);
    PERMUTATION(dwRigh, dwLeft, dwTmp, 2, 0x33333333);
    PERMUTATION(dwLeft, dwRigh, dwTmp, 8, 0x00ff00ff);
    PERMUTATION(dwRigh, dwLeft, dwTmp, 1, 0x55555555);

    dwTmp  = (dwRigh<<1) | (dwRigh>>31);
    dwRigh = (dwLeft<<1) | (dwLeft>>31); 
    dwLeft = dwTmp;

    dwLeft &= 0xffffffff;
    dwRigh &= 0xffffffff;

    if (Operation == DES_ECB_ENCRYPT)
    {
		D_ENCRYPT(dwLeft, dwRigh, KeySchedule, 0,  dwTmp, dwTmp1);
		D_ENCRYPT(dwRigh, dwLeft, KeySchedule, 2,  dwTmp, dwTmp1);
		D_ENCRYPT(dwLeft, dwRigh, KeySchedule, 4,  dwTmp, dwTmp1);
		D_ENCRYPT(dwRigh, dwLeft, KeySchedule, 6,  dwTmp, dwTmp1);
		D_ENCRYPT(dwLeft, dwRigh, KeySchedule, 8,  dwTmp, dwTmp1);
		D_ENCRYPT(dwRigh, dwLeft, KeySchedule, 10, dwTmp, dwTmp1);
		D_ENCRYPT(dwLeft, dwRigh, KeySchedule, 12, dwTmp, dwTmp1);
		D_ENCRYPT(dwRigh, dwLeft, KeySchedule, 14, dwTmp, dwTmp1);
		D_ENCRYPT(dwLeft, dwRigh, KeySchedule, 16, dwTmp, dwTmp1);
		D_ENCRYPT(dwRigh, dwLeft, KeySchedule, 18, dwTmp, dwTmp1);
		D_ENCRYPT(dwLeft, dwRigh, KeySchedule, 20, dwTmp, dwTmp1);
		D_ENCRYPT(dwRigh, dwLeft, KeySchedule, 22, dwTmp, dwTmp1);
		D_ENCRYPT(dwLeft, dwRigh, KeySchedule, 24, dwTmp, dwTmp1);
		D_ENCRYPT(dwRigh, dwLeft, KeySchedule, 26, dwTmp, dwTmp1);
		D_ENCRYPT(dwLeft, dwRigh, KeySchedule, 28, dwTmp, dwTmp1);
		D_ENCRYPT(dwRigh, dwLeft, KeySchedule, 30, dwTmp, dwTmp1);
    }
    else
    {
		D_ENCRYPT(dwLeft, dwRigh, KeySchedule, 30, dwTmp, dwTmp1);
		D_ENCRYPT(dwRigh, dwLeft, KeySchedule, 28, dwTmp, dwTmp1);
		D_ENCRYPT(dwLeft, dwRigh, KeySchedule, 26, dwTmp, dwTmp1);
		D_ENCRYPT(dwRigh, dwLeft, KeySchedule, 24, dwTmp, dwTmp1);
		D_ENCRYPT(dwLeft, dwRigh, KeySchedule, 22, dwTmp, dwTmp1);
		D_ENCRYPT(dwRigh, dwLeft, KeySchedule, 20, dwTmp, dwTmp1);
		D_ENCRYPT(dwLeft, dwRigh, KeySchedule, 18, dwTmp, dwTmp1);
		D_ENCRYPT(dwRigh, dwLeft, KeySchedule, 16, dwTmp, dwTmp1);
		D_ENCRYPT(dwLeft, dwRigh, KeySchedule, 14, dwTmp, dwTmp1);
		D_ENCRYPT(dwRigh, dwLeft, KeySchedule, 12, dwTmp, dwTmp1);
		D_ENCRYPT(dwLeft, dwRigh, KeySchedule, 10, dwTmp, dwTmp1);
		D_ENCRYPT(dwRigh, dwLeft, KeySchedule, 8,  dwTmp, dwTmp1);
		D_ENCRYPT(dwLeft, dwRigh, KeySchedule, 6,  dwTmp, dwTmp1);
		D_ENCRYPT(dwRigh, dwLeft, KeySchedule, 4,  dwTmp, dwTmp1);
		D_ENCRYPT(dwLeft, dwRigh, KeySchedule, 2,  dwTmp, dwTmp1);
		D_ENCRYPT(dwRigh, dwLeft, KeySchedule, 0,  dwTmp, dwTmp1);
    }
	
    dwLeft = (dwLeft>>1) | (dwLeft<<31);
    dwRigh = (dwRigh>>1) | (dwRigh<<31);

    dwLeft &= 0xffffffff;
    dwRigh &= 0xffffffff;

    PERMUTATION(dwRigh, dwLeft, dwTmp, 1,0x55555555);
    PERMUTATION(dwLeft, dwRigh, dwTmp, 8,0x00ff00ff);
    PERMUTATION(dwRigh, dwLeft, dwTmp, 2,0x33333333);
    PERMUTATION(dwLeft, dwRigh, dwTmp,16,0x0000ffff);
    PERMUTATION(dwRigh, dwLeft, dwTmp, 4,0x0f0f0f0f);

    DWORD_TO_4BYTES(dwLeft, bOut);
    DWORD_TO_4BYTES(dwRigh, bOut);
}

int DES_Encrypt(DWORD *DstBuffer, const DWORD * SrcBuffer, const DWORD *KeyAddress, DWORD Length, DWORD *IVector)
{
    DWORD i;
    DWORD buffer[2];

    buffer[0] = IVector[0];
    buffer[1] = IVector[1];

    for (i = 0; i < (Length >> 2); i = i+2)
    { 
	buffer[0] ^= SrcBuffer[i];
	buffer[1] ^= SrcBuffer[i+1];

	DES_ECB_mode((BYTE *) buffer, (BYTE *) buffer, KeyAddress, DES_ECB_ENCRYPT);

	DstBuffer[i]   = buffer[0];
	DstBuffer[i+1] = buffer[1];
    }

    return Length;
}

int DES_Decrypt(DWORD *DstBuffer, const DWORD * SrcBuffer, const DWORD *KeyAddress, DWORD Length, DWORD *IVector)
{
    DWORD i;
    DWORD buffer[2], ivectorL, ivectorR, oldSrcL, oldSrcR;

    ivectorL = IVector[0];
    ivectorR = IVector[1];

    for (i = 0; i < (Length >> 2); i = i + 2)
    { 
	buffer[0] = oldSrcL = SrcBuffer[i];
	buffer[1] = oldSrcR = SrcBuffer[i+1];

	DES_ECB_mode((BYTE *)buffer, (BYTE *)buffer, KeyAddress, DES_ECB_DECRYPT);

	DstBuffer[i]   = buffer[0] ^ ivectorL;
	DstBuffer[i+1] = buffer[1] ^ ivectorR;	  

	ivectorL = oldSrcL;
	ivectorR = oldSrcR;
    }

    return Length;
}

