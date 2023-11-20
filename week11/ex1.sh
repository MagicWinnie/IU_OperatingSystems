#!/bin/bash

# folder to mount on
mnt_point="./lofsdisk"

# return paths of shared libraries related to $1
get_libs() {
    # we are iterating through lines which contain "=>"
    # first line contains some kernel library (we do not need it)
    ldd $1 | grep "=>" | while read -r line
    do
        # get everything after "> "
        path=${line##*> }
        # get everything before " ("
        path=${path% (*}
        # therefore, we get the path on our system
        echo $path
    done
    # last line contains the dynamic linker library we need it
    # get everything after "> "
    line=$(ldd $1 | tail -1)
    path=${line##*> }
    # get everything before " ("
    path=${path% (*}
    # therefore, we get the path on our system
    echo $path
}

# copies shared libraries to our file system
copy_paths() {
    # iterate through paths of shared libraries
    get_libs $1 | while read -r path
    do
        # get directory in our file system without filename
        lofs_dir=$mnt_point${path%/*}
        # create the directories
        mkdir -p $lofs_dir
        # copy the file to our file system
        cp $path $lofs_dir
    done
}

# preallocate space of size 50MiB for `lofs.img` file
fallocate -l 50MiB lofs.img

# associate a first unused loop device with our `lofs.img` file
# write the associated loop device's name to a variable for future use
loop_device=$(sudo losetup --find --show lofs.img)

# print out info about what loop device is being used
echo "[INFO] Using loop device: $loop_device"

# create ext4 file system on our `lofs.img` file
# using -F flag to skip confirmation if a file system exists
sudo mkfs -F -t ext4 $loop_device

# mount the created filesystem to `$mnt_point` directory
mkdir -p $mnt_point
sudo mount $loop_device $mnt_point

# giving all rights to all users to work with our `$mnt_point`
sudo chmod 777 $mnt_point

# writing given info to our files
sudo echo "Dmitriy" > $mnt_point/file1
sudo echo "Okoneshnikov" > $mnt_point/file2

# create /usr/bin directory in our file system for future copying
mkdir -p "$mnt_point/usr/bin"

# copy `bash` executable into our file system
cp /usr/bin/bash $mnt_point/usr/bin
# copy shared libraries of `bash` into our file system
copy_paths "/usr/bin/bash"

# copy `cat` executable into our file system
cp /usr/bin/cat $mnt_point/usr/bin
# copy shared libraries of `cat` into our file system
copy_paths "/usr/bin/cat"

# copy `echo` executable into our file system
cp /usr/bin/echo $mnt_point/usr/bin
# copy shared libraries of `echo` into our file system
copy_paths "/usr/bin/echo"

# copy `ls` executable into our file system
cp /usr/bin/ls $mnt_point/usr/bin
# copy shared libraries of `ls` into our file system
copy_paths "/usr/bin/ls"

# create folder for dynamic linker in our file system
mkdir -p "$mnt_point/lib64"
# copy dynamic linker into our file system
cp /lib64/ld-linux-x86-64.so.2 $mnt_point/lib64

# compile the program
gcc ex1.c -o ex1.out
# copy it into our file system
cp ex1.out $mnt_point

# create `ex1.txt` file
echo "==========CHROOTED OUTPUT==========" > ex1.txt
# run the compiled program with changed root to `$mnt_point` and append output to `ex1.txt`
sudo chroot $mnt_point ./ex1.out >> "ex1.txt"

# run the compiled program without changing the root and append output to `ex1.txt`
echo -e "\n===========BASIC OUTPUT============" >> ex1.txt
./ex1.out >> ex1.txt

# unmount the created filesystem from the loop device
sudo umount $mnt_point

# deassociate the loop device from our file
sudo losetup --detach $loop_device

# remove the created directory and file
sudo rmdir $mnt_point
rm lofs.img
