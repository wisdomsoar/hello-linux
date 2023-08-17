#!/bin/sh


i=1

while [ "${i}" != "600" ]
do
    i=$(($i+1))
    echo "file 'libx265_sample_video_60f.mp4'" >> list.txt
done
