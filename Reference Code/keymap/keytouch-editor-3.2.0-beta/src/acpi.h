#ifndef INCLUDE_ACPI
#define INCLUDE_ACPI

#include <keytouch-editor.h>

#define ACPI_SOCKETFILE	"/var/run/acpid.socket"

extern Boolean check_acpi (void);
extern char *read_eventdescr (int fd);

#endif
