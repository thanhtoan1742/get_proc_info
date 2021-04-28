insmod /device_name.ko
dev="device_name"
major="$(grep "$dev" /proc/devices | cut -d ' ' -f 1)"
mknod "/dev/$dev" c "$major" 0
