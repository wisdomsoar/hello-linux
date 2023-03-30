
if [ -z "$(ls *.img )" ]; then
  echo "start testing..."
else
   echo "Delete all *img files?..."
   read
fi

j=1

while [ 1 ]
do

  echo "TEST BEGIN..."$j
  j=$(( $j  + 1 ))

  rm *img
  cp hello test0.img
  sync

  input_file=/etc/test0.img

  input_size=$(ls -l $input_file | awk '{print $5}')

    output_file=/etc/test1.img
    echo "input size: "$input_size
   
    ./dd-armel-static if=$input_file of=/dev/mtdblock26 oflag=direct,nonblock bs=1024 count=1024
    ./dd-armel-static if=/dev/mtdblock26 of=$output_file iflag=direct,nonblock bs=1024 count=1024
    cmp $input_file $output_file
    result=$?
    if [[ "$result" == "0" ]]; then
      echo "cmp ok"
    else
      echo "NG: cmp diff"
      break
    fi
    sleep 1
done

