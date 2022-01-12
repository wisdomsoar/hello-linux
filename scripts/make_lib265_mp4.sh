ffmpeg -framerate 60 -i img%03d.bmp -c:v libx265 -pix_fmt yuv420p -crf 23 video_sample_libx265_60f.mp4
