#!/bin/sh
#
# /etc/init.d/keytouch-init.sh
# Subsystem file for keytouch-init
#
# chkconfig: 2345 95 05
# description: Assignes keycodes to the scancodes of the extra function keys.
#
# processname: keytouch-init.sh

# Source function library.
. /etc/rc.d/init.d/functions

prog="keytouch-init"

keytouchinit=<replace me>

action $"Starting $prog:" $keytouchinit
RETVAL=$?
echo

exit $RETVAL