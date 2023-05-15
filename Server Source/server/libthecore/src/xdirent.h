#ifndef DIRENT_INCLUDED
#define DIRENT_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct DIR DIR;

struct dirent
{
    char *d_name;
};

DIR           *opendir(const char *);
int           closedir(DIR *);
struct dirent *readdir(DIR *);
void          rewinddir(DIR *);

#ifdef __cplusplus
}
#endif

#endif
