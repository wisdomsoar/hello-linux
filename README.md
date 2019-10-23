# hello-linux
How can I find the version of Ubuntu that is installed?
lsb_release -a
No LSB modules are available.
Distributor ID: Ubuntu
Description:    Ubuntu 16.10
Release:        16.10
Codename:       yakkety


ref: GNU/Linux most wanted

Handling files and directories
Changing directories:
cd ~bill (home directory of user bill)
Copy files to a directory:
cp file1 file2 dir

Copy directories recursively:
cp -r source_dir/ dest_dir/
rsync -a source_dir/ dest_dir/
rsync -av --exclude='path/to/exclude' source_dir/ dest_dir

Create a symbolic link:
ln -s linked_file link
ln -sf linked_file link
s: make symbolic links instead of hard links
f: remove existing destination files

get the link-to file path (ie. get XXXXXX from 'OOOOOO -> XXXXXX' link)
readlink -f path/to/file

root@imx6dlsabresd_qsip2:/sys/class/net/eth0# ls -l device
lrwxrwxrwx    1 root     root             0 Jul  2 06:40 device -> ../../../2188000.ethernet
root@imx6dlsabresd_qsip2:/sys/class/net/eth0#

root@imx6dlsabresd_qsip2:/sys/class/net/eth0# readlink -f device
/sys/devices/soc0/soc/2100000.aips-bus/2188000.ethernet

root@imx6dlsabresd_qsip2:/sys/class/net/eth0# basename $(readlink -f device)
2188000.ethernet


Rename a file, link or directory:
mv source_file dest_file
mv -T /path/src /path/dst
-T: treate DESTINATION (/path/dst) as a normal file

Remove non-empty directories:
rm -rf dir

Remove non-empty directories Recursively:
rm -rf `find . -name .git`

Listing files
ls
l: long listing
a: list all files (including hidden files)
t: by time (most recent files first)
s: by size (biggest files first)
r: reverse sort order

List link file recursively:
ls -alR | grep "/home/chhuang/500G" -B10
R: recursive

grep
B NUM: --before-context=NUM
o: only matching 只取出找到的pattern片段
例
dmesg | grep -o 'scsi.*Direct-Access.*ADATA'

awk
以空白做分隔，並列印指定的第幾欄 (從1開始)
例：以空白做分隔，印出第2欄
dmesg | grep -o 'scsi.*Direct-Access.*ADATA' | awk '{print $2}'

How to grep for contents after pattern?
https://stackoverflow.com/questions/10358547/how-to-grep-for-contents-after-pattern
取冒號之後的文字：

grep 'potato:' file.txt | sed 's/^.*: //'
or

grep 'potato:' file.txt | cut -d\   -f2
or

grep 'potato:' file.txt | awk '{print $2}'


example: Long list with most recent files last:
ls -ltr

Displaying file contents
Display the first 10 lines of a file:
head -10 file

Display the last lines of a file:
tail -10 file

********************************************************************************

Looking for files
Find *log* files in current (.) directory recursively
find . -name "*log*"

Find all the .pdf files in current (.) directory recursively and run a command on each
find . -name "*.pdf" -exec xpdf {} ';'

對每個找到的檔案執行xxx.sh
find . -type f | xargs -n | xxx.sh

在當前目錄遞迴找某關鍵字，並忽略find和grep警告訊息
find . -type f 2>/dev/null | xargs  grep -H  keyword_to_find 2>/dev/null > find_result.txt

在當前目錄找某關鍵字
grep -Ire keryword_to_find
l: Do not list binary files (--binary-files=without-match)
R: Recursively. Follow all symbolic links.
-e PATTERN

尋找linked files (並看完整路徑)
find . -type l -ls | grep mnt

(只有看連去那些連結...)
find . -type l -lname '/*'

locate "*pub*"

copy: https://stackoverflow.com/questions/5410757/delete-lines-in-a-text-file-that-contain-a-specific-string
Remove the line and print the output to standard out:
sed '/pattern to match/d' ./infile

Remove lines with specified pattern directly from the file:
sed -i '/pattern to match/d' ./infile

Archiving
Create a compressed archive (TApe ARchive):
tar jcvf archive.tar.bz2 dir

tar xvf archive.tar.[gz|bz2|lzma|xz]
tar jxvf archive.tar.bz2

tar jxvf archive.bz2

c: create 
t: test
x: extract
j: on the fly bzip2 (un)compression

tar xvf archive.tar -C /path/to/directory

********************************************************************************

File and partition sizes
Show the total size on disk of files or
directories (Disk Usage):
du -sh dir1 dir2 file1 file2
-s: summarize
-h: human

To check more than one directory and see the total, 
use 
du -sch
-c, --total
         produce a grand total

Number of bytes, words and lines in file:
wc file (Word Count)

ls | wc -l
l: count lines.

Show the size, total space and free space of the current partition:
df -h .

Display these info for all partitions:
df -h


********************************************************************************

mount samba in Linux (/etc/fstab)

sudo vi /etc/fstab

//172.16.70.151/your_name /path/to/your/local/directory/ cifs rw,username=your_name,password=your_password,uid=your_uid,gid=your_gid,iocharset=utf8,file_mode=0777,dir_mode=0777,noperm 0 0


Make zip file
zip -0r bootanimation.zip desc.txt part0 part1
0: not use any compression
r: recursively

start to use screen
$ screen
And press [space] key to skip the spash
list screen(s) you used
$ screen -ls
There are screens on:
19668.pts-14.Aspire-M7720-build-machine (11/21/2017 05:35:28 PM) (Detached)
2345.pts-19.Aspire-M7720-build-machine (11/17/2017 10:44:58 AM) (Detached)
delete a specified screen
format: screen -X -S [session id] quit
example:
$ screen -X -S 2345 quit
re-attach a screen
$ screen -r
or
format: screen -r [session id]
$ screen -r 19668

force attach to a specified session
screen -d -r 19668

for loop
END=5
for i in $(seq 1 $END);
do
    echo $i;
done

make sequence numbers:
for i in {1..4}; do printf "hello '%s'\n" input.mp4 >> list.txt; done

$ for i in {01..05}; do echo "$i"; done
01
02
03
04
05

$ foo=$(printf "%02d" 5)
$ echo "${foo}"
05


打patch:
testuser@localhost ~/p7zip_9.20.1 $ patch -p1 -i ../CVE-2015-1038.patch

 -p1 的參數，代表要跳過一層目錄結構，
(因為patch檔內，通常有a, b 資料夾，我們用-p1去跳過這層資料夾)

multimedia:
ffmpeg

make mp4 video from bmp files
ffmpeg -framerate 60 -i img%03d.bmp -c:v libx264 -pix_fmt yuv420p -crf 0 output_60f.mp4
-crf 0: to create a lossless video

Repeat/loop Input Video with ffmpeg:
copy: https://video.stackexchange.com/questions/12905/repeat-loop-input-video-with-ffmpeg
This allows you to loop an input without needing to re-encode.

1. Make a text file. Contents of an example text file to repeat 4 times.

file 'input.mp4'
file 'input.mp4'
file 'input.mp4'
file 'input.mp4'
Then run ffmpeg:

2. ffmpeg -f concat -i list.txt -c copy output.mp4

dd
/data/local/bin/dd 'if=/data/local/tmp/rnd_64M.img' 'of=/mnt/user/rnd_64M.img' 'bs=1024k' 'count=64' 'iflag=fullblock'

/data/local/bin/dd 'if=/mnt/sdc2_mmc/rnd_64M.img' 'of=/mnt/user/rnd.img_64M' 'bs=1024k' 'count=64' 'iflag=direct'

https://community.mellanox.com/s/article/how-to-set-cpu-scaling-governor-to-max-performance--scaling-governor-x

Performance CPU
echo performance > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu1/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu2/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu3/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu4/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu5/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu6/cpufreq/scaling_governor
echo performance > /sys/devices/system/cpu/cpu7/cpufreq/scaling_governor

# cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq

3300000

# cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq

1200000

# cat /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq

3210156
