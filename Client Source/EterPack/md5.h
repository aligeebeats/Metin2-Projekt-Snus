#ifndef __INC_ETERPACKLIB_MD5_H__
#define __INC_ETERPACKLIB_MD5_H__

typedef unsigned long int UINT4;

typedef struct {

  UINT4 i[2];
  UINT4 buf[4];
  unsigned char in[64];
  unsigned char digest[16];

} MD5_CTX;

#if defined (__cplusplus)
extern "C" {
#endif

void MD5Init (MD5_CTX *);
void MD5Update (MD5_CTX *,unsigned char *,unsigned int);
void MD5Final (MD5_CTX *);
void MD5Transform(UINT4 *,UINT4 *);

#if defined (__cplusplus)
};
#endif

#endif