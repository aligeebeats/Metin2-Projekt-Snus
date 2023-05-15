#include "md5.h"

static unsigned char PADDING[64] = {

  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

};

 

#ifndef CPU386

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))
#define FF(a, b, c, d, x, s, ac) \
  (a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); \
  (a) = ROTATE_LEFT ((a), (s)); \
  (a) += (b); \

#define GG(a, b, c, d, x, s, ac) \
	(a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \

#define HH(a, b, c, d, x, s, ac) \
	(a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \

#define II(a, b, c, d, x, s, ac) \
	(a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \

#endif

void MD5Init (MD5_CTX *mdContext)
{

  mdContext->i[0] = mdContext->i[1] = (UINT4)0;
  mdContext->buf[0] = (UINT4)0x67452301;
  mdContext->buf[1] = (UINT4)0xefcdab89;
  mdContext->buf[2] = (UINT4)0x98badcfe;
  mdContext->buf[3] = (UINT4)0x10325476;

}

void MD5Update (MD5_CTX *mdContext, unsigned char *inBuf, unsigned int inLen)
{

  UINT4 in[16];
  int mdi;
  unsigned int i, ii;

  mdi = (int)((mdContext->i[0] >> 3) & 0x3F);

  if ((mdContext->i[0] + ((UINT4)inLen << 3)) < mdContext->i[0])

    mdContext->i[1]++;

  mdContext->i[0] += ((UINT4)inLen << 3);
  mdContext->i[1] += ((UINT4)inLen >> 29);

#ifdef  LITTLE_ENDIAN
     if(mdi == 0 && ((int)inBuf & 3) == 0){

          while(inLen >= 64)
          {
               MD5Transform(mdContext->buf,(UINT4 *)inBuf);
               inLen -= 64;
               inBuf += 64;
          }               
     }

#endif

  while (inLen--) 
  {
    mdContext->in[mdi++] = *inBuf++;
    if (mdi == 0x40) {

      for (i = 0, ii = 0; i < 16; i++, ii += 4)

     in[i] = (((UINT4)mdContext->in[ii+3]) << 24) |

          (((UINT4)mdContext->in[ii+2]) << 16) |

          (((UINT4)mdContext->in[ii+1]) << 8) |

          ((UINT4)mdContext->in[ii]);

      MD5Transform (mdContext->buf, in);

      mdi = 0;

    }

  }

}

void MD5Final (MD5_CTX *mdContext)
{

  UINT4 in[16];
  int mdi;
  unsigned int i, ii;
  unsigned int padLen;

  in[14] = mdContext->i[0];
  in[15] = mdContext->i[1];
  mdi = (int)((mdContext->i[0] >> 3) & 0x3F);

  padLen = (mdi < 56) ? (56 - mdi) : (120 - mdi);

  MD5Update (mdContext, PADDING, padLen);

  for (i = 0, ii = 0; i < 14; i++, ii += 4)

    in[i] = (((UINT4)mdContext->in[ii+3]) << 24) |

         (((UINT4)mdContext->in[ii+2]) << 16) |

         (((UINT4)mdContext->in[ii+1]) << 8) |

         ((UINT4)mdContext->in[ii]);

  MD5Transform (mdContext->buf, in);

  for (i = 0, ii = 0; i < 4; i++, ii += 4) {

    mdContext->digest[ii] = (unsigned char)(mdContext->buf[i] & 0xFF);

    mdContext->digest[ii+1] =

      (unsigned char)((mdContext->buf[i] >> 8) & 0xFF);

    mdContext->digest[ii+2] =

      (unsigned char)((mdContext->buf[i] >> 16) & 0xFF);

    mdContext->digest[ii+3] =

      (unsigned char)((mdContext->buf[i] >> 24) & 0xFF);

  }

}


#ifndef CPU386
void MD5Transform (buf, in)

UINT4 *buf;
UINT4 *in;

{
  UINT4 a = buf[0], b = buf[1], c = buf[2], d = buf[3];

#define S11 7
#define S12 12
#define S13 17
#define S14 22

  FF ( a, b, c, d, in[ 0], S11, 3614090360);

  FF ( d, a, b, c, in[ 1], S12, 3905402710);

  FF ( c, d, a, b, in[ 2], S13,  606105819);

  FF ( b, c, d, a, in[ 3], S14, 3250441966);

  FF ( a, b, c, d, in[ 4], S11, 4118548399);

  FF ( d, a, b, c, in[ 5], S12, 1200080426);

  FF ( c, d, a, b, in[ 6], S13, 2821735955);

  FF ( b, c, d, a, in[ 7], S14, 4249261313);

  FF ( a, b, c, d, in[ 8], S11, 1770035416);

  FF ( d, a, b, c, in[ 9], S12, 2336552879);

  FF ( c, d, a, b, in[10], S13, 4294925233);

  FF ( b, c, d, a, in[11], S14, 2304563134);

  FF ( a, b, c, d, in[12], S11, 1804603682);

  FF ( d, a, b, c, in[13], S12, 4254626195);

  FF ( c, d, a, b, in[14], S13, 2792965006);

  FF ( b, c, d, a, in[15], S14, 1236535329);

#define S21 5
#define S22 9
#define S23 14
#define S24 20

  GG ( a, b, c, d, in[ 1], S21, 4129170786);

  GG ( d, a, b, c, in[ 6], S22, 3225465664);

  GG ( c, d, a, b, in[11], S23,  643717713);

  GG ( b, c, d, a, in[ 0], S24, 3921069994);

  GG ( a, b, c, d, in[ 5], S21, 3593408605);

  GG ( d, a, b, c, in[10], S22,   38016083);

  GG ( c, d, a, b, in[15], S23, 3634488961);

  GG ( b, c, d, a, in[ 4], S24, 3889429448);

  GG ( a, b, c, d, in[ 9], S21,  568446438);

  GG ( d, a, b, c, in[14], S22, 3275163606);

  GG ( c, d, a, b, in[ 3], S23, 4107603335);

  GG ( b, c, d, a, in[ 8], S24, 1163531501);

  GG ( a, b, c, d, in[13], S21, 2850285829);

  GG ( d, a, b, c, in[ 2], S22, 4243563512);

  GG ( c, d, a, b, in[ 7], S23, 1735328473);

  GG ( b, c, d, a, in[12], S24, 2368359562);

#define S31 4
#define S32 11
#define S33 16
#define S34 23

  HH ( a, b, c, d, in[ 5], S31, 4294588738);

  HH ( d, a, b, c, in[ 8], S32, 2272392833);

  HH ( c, d, a, b, in[11], S33, 1839030562);

  HH ( b, c, d, a, in[14], S34, 4259657740);

  HH ( a, b, c, d, in[ 1], S31, 2763975236);

  HH ( d, a, b, c, in[ 4], S32, 1272893353);

  HH ( c, d, a, b, in[ 7], S33, 4139469664);

  HH ( b, c, d, a, in[10], S34, 3200236656);

  HH ( a, b, c, d, in[13], S31,  681279174);

  HH ( d, a, b, c, in[ 0], S32, 3936430074);

  HH ( c, d, a, b, in[ 3], S33, 3572445317);

  HH ( b, c, d, a, in[ 6], S34,   76029189);

  HH ( a, b, c, d, in[ 9], S31, 3654602809);

  HH ( d, a, b, c, in[12], S32, 3873151461);

  HH ( c, d, a, b, in[15], S33,  530742520);

  HH ( b, c, d, a, in[ 2], S34, 3299628645);

#define S41 6
#define S42 10
#define S43 15
#define S44 21

  II ( a, b, c, d, in[ 0], S41, 4096336452);

  II ( d, a, b, c, in[ 7], S42, 1126891415);

  II ( c, d, a, b, in[14], S43, 2878612391);

  II ( b, c, d, a, in[ 5], S44, 4237533241);

  II ( a, b, c, d, in[12], S41, 1700485571);

  II ( d, a, b, c, in[ 3], S42, 2399980690);

  II ( c, d, a, b, in[10], S43, 4293915773);

  II ( b, c, d, a, in[ 1], S44, 2240044497);

  II ( a, b, c, d, in[ 8], S41, 1873313359);

  II ( d, a, b, c, in[15], S42, 4264355552);

  II ( c, d, a, b, in[ 6], S43, 2734768916);

  II ( b, c, d, a, in[13], S44, 1309151649);

  II ( a, b, c, d, in[ 4], S41, 4149444226);

  II ( d, a, b, c, in[11], S42, 3174756917);

  II ( c, d, a, b, in[ 2], S43,  718787259);

  II ( b, c, d, a, in[ 9], S44, 3951481745);

  buf[0] += a;
  buf[1] += b;
  buf[2] += c;
  buf[3] += d;

}

#else

#ifdef  __COMPACT__

     asm     .MODEL COMPACT

#elif   __HUGE__

     asm     .MODEL HUGE

#elif   __LARGE__

     asm     .MODEL LARGE

#elif   __MEDIUM__

     asm     .MODEL MEDIUM

#elif   __SMALL__

     asm     .MODEL SMALL

#elif   __TINY__

     asm     .MODEL TINY

#endif

#define COM(a,b,x,s,t)\

     asm     lea a,t[a+edi];\
     asm     add a,x;\
     asm     rol a,s;\
     asm     add a,b;

#define F(x,y,z)\

     asm     mov edi,x;\
     asm     and edi,y;\
     asm     mov esi,x;\
     asm     not esi;\
     asm     and esi,z;\
     asm     or edi,esi


#define FF(a,b,c,d,x,s,t)\

     F(b,c,d);\

     COM(a,b,x,s,t)

#define G(x,y,z) F(z,x,y)
#define GG(a,b,c,d,x,s,t)\

     G(b,c,d);\

     COM(a,b,x,s,t)

#define H(x,y,z)\

     asm     mov edi,x;\

     asm     xor edi,y;\

     asm     xor edi,z

#define HH(a,b,c,d,x,s,t)\

     H(b,c,d);\

     COM(a,b,x,s,t)

#define I(x,y,z)\

     asm     mov edi,z;\

     asm     not edi;\

     asm     or edi,x;\

     asm     xor edi,y

#define II(a,b,c,d,x,s,t)\

     I(b,c,d);\

     COM(a,b,x,s,t)

 

#define A       eax

#define B       ebx

#define C       ecx

#define D       edx

 

void

MD5Transform(buf,input)

UINT4 *buf;

UINT4 *input;

{

     asm     .386;
     asm     push si;
     asm     push di;
     asm     push es;
     asm     if @DataSize NE 0
     asm             push ds;
     asm     endif
     asm     if @DataSize NE 0

     asm             lds si,buf;

     asm     else

     asm             mov si,buf;

     asm     endif

     asm     mov A,dword ptr si[0*4];

     asm     mov B,dword ptr si[1*4];

     asm     mov C,dword ptr si[2*4];

     asm     mov D,dword ptr si[3*4];
     asm     push bp;
     asm     les bp,input

#define S11 7
#define S12 12
#define S13 17
#define S14 22

     FF(A,B,C,D,es:bp[ 0*4],S11,3614090360);

     FF(D,A,B,C,es:bp[ 1*4],S12,3905402710);

     FF(C,D,A,B,es:bp[ 2*4],S13, 606105819);

     FF(B,C,D,A,es:bp[ 3*4],S14,3250441966);

     FF(A,B,C,D,es:bp[ 4*4],S11,4118548399);

     FF(D,A,B,C,es:bp[ 5*4],S12,1200080426);

     FF(C,D,A,B,es:bp[ 6*4],S13,2821735955);

     FF(B,C,D,A,es:bp[ 7*4],S14,4249261313);

     FF(A,B,C,D,es:bp[ 8*4],S11,1770035416);

     FF(D,A,B,C,es:bp[ 9*4],S12,2336552879);

     FF(C,D,A,B,es:bp[10*4],S13,4294925233);

     FF(B,C,D,A,es:bp[11*4],S14,2304563134);

     FF(A,B,C,D,es:bp[12*4],S11,1804603682);

     FF(D,A,B,C,es:bp[13*4],S12,4254626195);

     FF(C,D,A,B,es:bp[14*4],S13,2792965006);

     FF(B,C,D,A,es:bp[15*4],S14,1236535329);

#define S21 5

#define S22 9

#define S23 14

#define S24 20

     GG(A,B,C,D,es:bp[ 1*4],S21,4129170786);

     GG(D,A,B,C,es:bp[ 6*4],S22,3225465664);

     GG(C,D,A,B,es:bp[11*4],S23, 643717713);

     GG(B,C,D,A,es:bp[ 0*4],S24,3921069994);

     GG(A,B,C,D,es:bp[ 5*4],S21,3593408605);

     GG(D,A,B,C,es:bp[10*4],S22,  38016083);

     GG(C,D,A,B,es:bp[15*4],S23,3634488961);

     GG(B,C,D,A,es:bp[ 4*4],S24,3889429448);

     GG(A,B,C,D,es:bp[ 9*4],S21, 568446438);

     GG(D,A,B,C,es:bp[14*4],S22,3275163606);

     GG(C,D,A,B,es:bp[ 3*4],S23,4107603335);

     GG(B,C,D,A,es:bp[ 8*4],S24,1163531501);

     GG(A,B,C,D,es:bp[13*4],S21,2850285829);

     GG(D,A,B,C,es:bp[ 2*4],S22,4243563512);

     GG(C,D,A,B,es:bp[ 7*4],S23,1735328473);

     GG(B,C,D,A,es:bp[12*4],S24,2368359562);

#define S31 4

#define S32 11

#define S33 16

#define S34 23

     HH(A,B,C,D,es:bp[ 5*4],S31,4294588738);

     HH(D,A,B,C,es:bp[ 8*4],S32,2272392833);

     HH(C,D,A,B,es:bp[11*4],S33,1839030562);

     HH(B,C,D,A,es:bp[14*4],S34,4259657740);

     HH(A,B,C,D,es:bp[ 1*4],S31,2763975236);

     HH(D,A,B,C,es:bp[ 4*4],S32,1272893353);

     HH(C,D,A,B,es:bp[ 7*4],S33,4139469664);

     HH(B,C,D,A,es:bp[10*4],S34,3200236656);

     HH(A,B,C,D,es:bp[13*4],S31, 681279174);

     HH(D,A,B,C,es:bp[ 0*4],S32,3936430074);

     HH(C,D,A,B,es:bp[ 3*4],S33,3572445317);

     HH(B,C,D,A,es:bp[ 6*4],S34,  76029189);

     HH(A,B,C,D,es:bp[ 9*4],S31,3654602809);

     HH(D,A,B,C,es:bp[12*4],S32,3873151461);

     HH(C,D,A,B,es:bp[15*4],S33, 530742520);

     HH(B,C,D,A,es:bp[ 2*4],S34,3299628645);

#define S41 6

#define S42 10

#define S43 15

#define S44 21

     II(A,B,C,D,es:bp[ 0*4],S41,4096336452);

     II(D,A,B,C,es:bp[ 7*4],S42,1126891415);

     II(C,D,A,B,es:bp[14*4],S43,2878612391);

     II(B,C,D,A,es:bp[ 5*4],S44,4237533241);

     II(A,B,C,D,es:bp[12*4],S41,1700485571);

     II(D,A,B,C,es:bp[ 3*4],S42,2399980690);

     II(C,D,A,B,es:bp[10*4],S43,4293915773);

     II(B,C,D,A,es:bp[ 1*4],S44,2240044497);

     II(A,B,C,D,es:bp[ 8*4],S41,1873313359);

     II(D,A,B,C,es:bp[15*4],S42,4264355552);

     II(C,D,A,B,es:bp[ 6*4],S43,2734768916);

     II(B,C,D,A,es:bp[13*4],S44,1309151649);

     II(A,B,C,D,es:bp[ 4*4],S41,4149444226);

     II(D,A,B,C,es:bp[11*4],S42,3174756917);

     II(C,D,A,B,es:bp[ 2*4],S43, 718787259);

     II(B,C,D,A,es:bp[ 9*4],S44,3951481745);

 

     asm pop bp;

     asm     if @DataSize NE 0

     asm             lds si,buf

     asm     else

     asm             mov si,buf;

     asm     endif

     asm add dword ptr si[0*4],A;

     asm add dword ptr si[1*4],B;

     asm add dword ptr si[2*4],C;

     asm add dword ptr si[3*4],D;

     asm     if @DataSize NE 0

     asm             pop ds

     asm     endif

 

     asm     pop es;

     asm     pop di;

     asm     pop si;

}

#endif
