

# busy wait when uptime < x seconds
# awk 
# get first vaule in field and convert to an integer
my_seconds = $(awk '{print int($1)}' /proc/uptime)
while [ $my_seconds -lt 76 ]
do       
  sleep 1      
  my_seconds = $(awk '{print int($1)}' /proc/uptime)  
done  


# Print <Tab>
# ref: https://www.unix.com/shell-programming-and-scripting/132905-printing-fixed-width-columns.html
# use -e to enable the use of special characters
echo -e "test \t\t test"

# Printing Fixed Width Columns
# ref: https://www.unix.com/shell-programming-and-scripting/132905-printing-fixed-width-columns.html
printf "%-50s%-20s"   "maybe_variable_length_string_here " "Value: "$Val
printf "\n"

# pause in script:
# ref: http://linux-wiki.cn/wiki/zh-tw/%E7%94%A8shell%E5%AE%9E%E7%8E%B0bat%E7%9A%84pause
read -n 1 -p "Press any key to continue..."

Comparison: check if a regular file does not exist in Bash?
ref: https://stackoverflow.com/questions/638975/how-do-i-tell-if-a-regular-file-does-not-exist-in-bash
ref: https://www.cyberciti.biz/faq/unix-linux-test-existence-of-file-in-bash/
if [ ! -f /tmp/foo.txt ]; then
    echo "File not found!"
fi

   -e FILE
          FILE exists
   -f FILE
          FILE exists and is a regular file


# Comparison: Not equal
# Using the not equal operator for string comparison
# ref: https://unix.stackexchange.com/questions/67898/using-the-not-equal-operator-for-string-comparison
if [ "$PHONE_TYPE" != "NORTEL" ] && [ "$PHONE_TYPE" != "NEC" ] &&
   [ "$PHONE_TYPE" != "CISCO" ]; then
   echo "OOXX"
fi

#Check if file *.img exist?
if [ -n "$(echo *.img)" ]; then
  echo "true"
fi


Array Basics in Shell Scripting
ref: https://www.geeksforgeeks.org/array-basics-shell-scripting-set-1/
Array in shell
ARRAYNAME=(value1 value2  .... valueN)

echo ${ARRAYNAME[*]}

whats wrong with Android shell??? mksh
syntax error: unexpected '(('
for-loop改用：
GPIO_pins=( 100 123 )
len=${#GPIO_pins[@]}
#Android有seq嗎?????????????
for i in $(seq 1 $len);
do
    echo $i ${GPIO_pins[$i-1]}
done

ref: https://www.opencli.com/linux/shell-script-for-loop-array-foreach
arr=("value1" "value2" "value3" "value4" "value5")
for ((i=0; i < ${#arr[@]}; i++))
do
    ### 印出 array 的 key 及 value
    echo $i ${arr[$i]}
done

for loop in script
ref: https://blog.longwin.com.tw/2014/07/bash-shell-for-loop-script-2014/
for i in {1..100};
do
    time curl -o /dev/null https://blog.longwin.com.tw;
done

範圍需使用變數時：
END=5
for i in $(seq 1 $END);
do
    echo $i;
done
# 印出 1 2 3 4 5 (換行)

for (( i=1; i<=20; i=i+1 ))
do
	ffmpeg -i capture_color_bar.avi -vf "select=eq(n\,$i)" -vframes 1 img_$i.png
done

infinite loop
while true
do
  echo "Do something; hit [CTRL+C] to stop!"
done

i=1
while [ 1 ]; 
do
  i=$(($i+1))
done


file=i_have_many_chars.txt
while read -n1 char; do
  echo $char
done < "$file"

Bash remove first and last characters from a string
substring
ref: https://stackoverflow.com/questions/638975/how-do-i-tell-if-a-regular-file-does-not-exist-in-bash
You can do

string="|abcdefg|"
string2=${string#"|"}
string2=${string2%"|"}
echo $string2
Or if your string length is constant, you can do

string="|abcdefg|"
string2=${string:1:7}
echo $string2

How to preserve line breaks when storing a command output to a variable in bash?
ref: https://stackoverflow.com/questions/22101778/how-to-preserve-line-breaks-when-storing-a-command-output-to-a-variable-in-bash
$ f="fafafda
> adffd
> adfadf
> adfafd
> afd"

$ echo $f
fafafda adffd adfadf adfafd afd
$ echo "$f"
fafafda
adffd
adfadf
adfafd
afd

Heximal to Decimal:
ref: http://blog.ilc.edu.tw/blog/index.php?op=printView&articleId=470941&blogId=25793
16 進位轉成 10 進位
# echo $((16#c0))
192

Arithmetic
# Like everything else in shell, these are strings, not
# floating-point values
d1=0.003
d2=0.0008

# bc parses its input to perform math
d1d2=$(echo "$d1 + $d2" | bc)

# These, too, are strings (not integers)
mean1=7
mean2=5

# $((...)) is a built-in construct that can parse
# its contents as integers; valid identifiers
# are recursively resolved as variables.
meandiff=$((mean1 - mean2))

----------------------------------------------------
用這樣來比較兩個float是否相等
$(echo "$num1  $num2" | bc -l)


How to compare two floating point numbers in Bash?
ref: https://stackoverflow.com/questions/8654051/how-to-compare-two-floating-point-numbers-in-bash
if (( $(echo "$num1 > $num2" | bc -l) )); then
  …
fi

Block Comments in a Shell Script
ref: https://stackoverflow.com/questions/947897/block-comments-in-a-shell-script
#!/bin/bash
echo before comment
: <<'END'
bla bla
blurfl
END
echo after comment



用個迴圈使用傳進script的參數
i=2
j=$#
recipe=$1
shift 1
while [ $i -le $j ]
do
  #bitbake $1 -c
  echo $1
  set -x
  bitbake $recipe -c $1
  set +x
  i=$((i+1))
  shift 1
done





Convert seconds to hours, minutes, seconds
https://stackoverflow.com/questions/12199631/convert-seconds-to-hours-minutes-seconds
function show_time () {
    num=$1
    min=0
    hour=0
    day=0
    if((num>59));then
        ((sec=num%60))
        ((num=num/60))
        if((num>59));then
            ((min=num%60))
            ((num=num/60))
            if((num>23));then
                ((hour=num%24))
                ((day=num/24))
            else
                ((hour=num))
            fi
        else
            ((min=num))
        fi
    else
        ((sec=num))
    fi
    echo "DURATION: $day"d "$hour"h "$min"m "$sec"s
}


