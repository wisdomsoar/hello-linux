(echo n; echo p; echo 1; echo ""; echo ""; echo w; echo q) | fdisk /dev/xxxx?
mke2fs -F /dev/xxxp1

mount -t ext2 /dev/xxx? /mnt/xxx
mount -t ext2 /dev/xxxp1 /mnt/xxx
