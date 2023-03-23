
if [ -n "$(echo *.img )" ]; then
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

input_file=/etc/test0.img

input_size=$(ls -l $input_file | awk '{print $5}')

total_space=$(df . | awk 'NR==2 {print $2}')
total_space=$(( $total_space * 1024 ))
free_space=$(df . | awk 'NR==2 {print $4}')
free_space=$(( $free_space * 1024 ))

est_free=$(( free_space - input_size ))


for i in $(seq 1 9999); do
    output_file=/etc/test$i.img
    echo "i: "$i
    echo "input size: "$input_size
   
    free_space=$(df . | awk 'NR==2 {print $4}')
    free_space=$(( $free_space * 1024 ))
    est_free=$(( free_space - input_size ))
    est_free_p=$(( $est_free * 100 / $total_space ))
    echo "total_space:  "$total_space
    echo "est_free:  "$est_free
    echo "free:      "$free_space
    echo "est_free_p: "$est_free_p  
    if [ $est_free_p -lt 20 ]; then
      echo "exit.. 20% space"
      break
    fi
    if [[ -f $output_file ]]; then
      echo "NG: file $output_file already exists"
    fi
    dd if=$input_file of=$output_file conv=sync
    cmp $input_file $output_file
    result=$?
    if [[ "$result" == "0" ]]; then
      echo "cmp ok"
    else
      echo "NG: cmp diff"
      break
    fi
done

done
