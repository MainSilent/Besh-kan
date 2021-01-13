#!/bin/sh

# WRITE PERMISSIONS
 busybox echo "The item WRITE PERMISSIONS is being processed..."
 busybox mount -t proc proc /proc
 busybox mount -o remount,rw /

# CLEANUP
 busybox echo "The item CLEANUP is being processed..."
 if [ ! -e /restart.log ]; then
   if [ ! -e /etc/cleanup.conf ]; then
     busybox echo FOLDERS_TO_CLEAN_DURING_BOOT: > /etc/cleanup.conf
     busybox echo /tmp >> /etc/cleanup.conf
     busybox echo /var/run >> /etc/cleanup.conf
   fi
   FOLDERS=$(busybox cat /etc/cleanup.conf)
   for i in $FOLDERS; do
     busybox rm -fr $i/.* 2>/dev/null busybox rm -fr $i/*
   done
 fi

# VIRTUAL FILE SYSTEMS
 busybox echo "The item VIRTUAL FILE SYSTEMS is being processed..."
 for i in dev dev/pts dev/shm run sys tmp; do
   if [ ! -e /$i ]; then
     busybox mkdir /$i
   fi
 done
 busybox mount -t sysfs sysfs /sys
 busybox mount -t tmpfs tmpfs /run
 busybox mount -t tmpfs tmpfs /dev/shm
 busybox mount -t devpts devpts /dev/pts
 if [ -e /sys/firmware/efi ]; then
   busybox mount -t efivarfs efivarfs /sys/firmware/efi/efivars
 fi

# DEVICE NODES
 busybox echo "The item DEVICE NODES is being processed..."
 cd /dev
 busybox mdev -s
 busybox mknod fd0 b 2 0 2>null
 busybox mknod ppp c 108 0 2>null
 busybox mknod js0 c 13 0 2>null
 busybox mknod lp0 c 180 0 2>null

# USER DIRECTORY
 busybox echo "The item USER DIRECTORY is being processed..."
 busybox mkdir /usr 2>null
 for i in bin lib sbin share; do
   busybox mkdir /usr/$i 2>null
 done
 busybox rm -fr /usr/etc
 busybox ln -s /etc /usr/etc
 busybox rm -fr /usr/var
 busybox ln -s /var /usr/var
 busybox rm -fr /usr/local
 busybox ln -s /usr /usr/local
 busybox rm -fr /usr/lib64
 busybox ln -s /usr/lib /usr/lib64

# BUSYBOX APPLETS
 busybox echo "The item BUSYBOX APPLETS is being processed..."
 /bin/busybox --install -s
 rm /linuxrc 2>null
 rm /init 2>null

# install tools
 echo "The tools is being processed..."
 ADDON_LIST=$(ls /etc/init.d/*)
 export EXTRACT_UNSAFE_SYMLINKS=1

 for i in $ADDON_LIST; do
   echo "Loading tools -> $i"
   cp -f $i /var/4MLinux
   busybox xz -d $i
   busybox tar -x -f /etc/init.d/*.tar -C /
   rm /etc/init.d/*.tar
 done

# monut all disks
for path in $(fdisk -l |awk '{ print $1 }'|grep -v md |grep -v loop |grep .*[[:digit:]]|sort|uniq;); 
do
    mkdir -p /mnt/${path##*/}
    ntfs-3g $path /mnt/${path##*/}
done

# disable kernel messages
dmesg -n 1
#echo 1 > /proc/sys/kernel/printk
clear