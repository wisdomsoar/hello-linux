#!/bin/sh

module="scull"
device="scull"
mode="664"
#110 110 010

/sbin/insmod ./$module.ko $* || exit 1


rm -f /dev/${device}[0-3]


# if (second column == $module) then print first column
major=$(cat /proc/devices | awk "\$2==\"$module\" {print \$1}")


mknod /dev/${device}0 c $major 0
mknod /dev/${device}1 c $major 1
mknod /dev/${device}2 c $major 2
mknod /dev/${device}3 c $major 3

group="staff"
grep -q '^staff:' /etc/group || group="wheel"

chgrp $group /dev/${device}[0-3]
chmod $mode  /dev/${device}[0-3]



