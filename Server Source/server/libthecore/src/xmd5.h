#ifndef __FreeBSD__
#ifndef MD5_H
#define MD5_H

#include <sys/types.h>

#define MD5_HASHBYTES 16

typedef struct MD5Context {
        uint32_t buf[4];
        uint32_t bits[2];
        unsigned char in[64];
} MD5_CTX;

#ifdef __cplusplus
extern "C" {
#endif

void MD5Init(MD5_CTX *context);
void MD5Update(MD5_CTX *context, unsigned char const *buf, unsigned len);
void MD5Final(unsigned char digest[MD5_HASHBYTES], MD5_CTX *context);
void MD5Transform(uint32_t buf[4], uint32_t const in[16]);
char* MD5End(MD5_CTX *, char *);

char* lutil_md5_file(const char *filename, char *buf);
char* lutil_md5_data(const unsigned char *data, unsigned int len, char *buf);

#ifdef __cplusplus
}
#endif

#endif

#endif
