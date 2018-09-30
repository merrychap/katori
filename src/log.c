#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <errno.h>

#include "log.h"

static int out;

static void
__log(int prio, const char *msg, va_list ap)
{
    char *nmsg = NULL;

	if (out) {
		vfprintf(stderr, msg, ap);
		fprintf(stderr, ": %s\n", strerror(errno));
	} else {
		if (asprintf(&nmsg, "%s: %s", msg, strerror(errno)) == -1) {
			vsyslog(prio, msg, ap);
			syslog(LOG_CRIT, "%s", strerror(errno));
		} else {
			vsyslog(prio, nmsg, ap);
			free(nmsg);
		}
	}
}

void
log_init(const char *name, int __out)
{
    out = __out;
    if (!out) {
        tzset();
        openlog(name, LOG_NDELAY | LOG_PID, LOG_DAEMON);
    }
}

void
log_info(const char *msg, ...)
{
    va_list ap;

	va_start(ap, msg);
	__log(LOG_INFO, msg, ap);
	va_end(ap);
}

void
log_warn(const char *msg, ...)
{
    va_list ap;

	va_start(ap, msg);
	__log(LOG_WARNING, msg, ap);
	va_end(ap);
}

void
log_debug(const char *msg, ...)
{
    va_list ap;

	va_start(ap, msg);
	__log(LOG_DEBUG, msg, ap);
	va_end(ap);
}

void
fatal(const char *msg, ...)
{
    va_list ap;

	va_start(ap, msg);
	__log(LOG_CRIT, msg, ap);
	va_end(ap);

	exit(1);
}