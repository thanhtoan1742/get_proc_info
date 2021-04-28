MODULE_NAME="get_proc_info"
DEV_NAME="get_proc_info"

make
insmod "${MODULE_NAME}.ko"

DEV_MAJOR=$(cat /proc/devices | grep $DEV_NAME | cut -d " " -f 1)
rm -f $DEV_NAME
mknod $DEV_NAME c $DEV_MAJOR 0
