i=10

sec=$( date +%s )
sec=$(( $sec % 100 ))
while [ 1 ];
do
   base_offset=$(( $sec + $i ))
    clear
    cat chit.txt | sed -n $sec,${base_offset}p
    sleep 7
    i=$((i+10))
    if [[ "$i" == "50" ]]; then
      clear
      sec=$( date +%s  )
      sec=$(( $sec % 100 ))
      i=10
    fi
done
