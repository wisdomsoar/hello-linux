
::can you help me to shorten this!


adb wait-for-device
adb root

echo %date% > tmp_date.txt
echo %time% > tmp_time.txt

adb push tmp_date.txt /tmp/tmp_date.txt
adb push tmp_time.txt  /tmp/tmp_time.txt

adb shell "cat /tmp/tmp_date.txt | cut -d ' ' -f2 | cut -d '/' -f 3 > /tmp/tmp_year.txt"
adb shell "cat /tmp/tmp_date.txt | cut -d ' ' -f2 | cut -d '/' -f 2 > /tmp/tmp_day.txt"
adb shell "cat /tmp/tmp_date.txt | cut -d ' ' -f2 | cut -d '/' -f 1 > /tmp/tmp_month.txt"

adb shell "cat /tmp/tmp_time.txt | cut -d '.' -f1 > /tmp/tmp_trim_time.txt"

adb shell "echo -n $(cat /tmp/tmp_year.txt)-$(cat /tmp/tmp_month.txt)-$(cat /tmp/tmp_day.txt) > /tmp/my_time.txt"
adb shell "echo -n ' ' >> /tmp/my_time.txt"
adb shell "echo -n $(cat /tmp/tmp_trim_time.txt) >> /tmp/my_time.txt"

adb shell "date -s \"$(cat /tmp/my_time.txt)\""


pause
::date -s "2023-05-18 10:30:00"


