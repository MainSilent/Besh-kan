#!/bin/sh

mount -t proc proc /proc
mount -t sysfs sysfs /sys
mdev -s

# install tools
cd tools
for entry in ./*
do
  dpkg -i $entry --force-depends
done
cd /

# monut all disks
for path in $(fdisk -l |awk '{ print $1 }'|grep -v md |grep -v loop |grep .*[[:digit:]]|sort|uniq;); 
do
    echo $path
    #mount -t auto $path /mnt
done

echo "Mount done."