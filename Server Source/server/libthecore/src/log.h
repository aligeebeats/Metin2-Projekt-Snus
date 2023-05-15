#ifndef __INC_LIBTHECORE_LOG_H__
#define __INC_LIBTHECORE_LOG_H__

#ifdef __cplusplus
extern "C"
{
#endif
    extern int log_init(void);
    extern void log_destroy(void);
    extern void log_rotate(void);

	extern void log_set_level(unsigned int level);
	extern void log_unset_level(unsigned int level);

	extern void log_set_expiration_days(unsigned int days);
	extern int log_get_expiration_days(void);

#ifndef _WIN32
    extern void _sys_err(const char *func, int line, const char *format, ...);
#else
	extern void _sys_err(const char *func, int line, const char *format, ...);
#endif
    extern void sys_log_header(const char *header);
    extern void sys_log(unsigned int lv, const char *format, ...);
    extern void pt_log(const char *format, ...);

#ifndef _WIN32
#define sys_err(fmt, args...) _sys_err(__FUNCTION__, __LINE__, fmt, ##args)
#else 
#define sys_err(fmt, ...) _sys_err(__FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif
