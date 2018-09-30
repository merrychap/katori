#ifndef __LOG_H__
#define __LOG_H__

void log_init(const char *name, int __out);
void log_info(const char *msg, ...);
void log_warn(const char *msg, ...);
void log_debug(const char *msg, ...);
void fatal(const char *msg, ...);

#endif