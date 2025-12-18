

while [ 1 ]; 
do                      
  r1=$(cat /sys/class/net/enx00e04c68158d/statistics/rx_bytes)
  sleep 5
  r2=$(cat /sys/class/net/enx00e04c68158d/statistics/rx_bytes)
  bytes=$(($r2-$r1))
  rate=$(echo "$bytes / 5" | bc)

  # 1800 + 5120 (1024*5) = 6920 

  echo -n "test rate: " >> /tmp/test_rate.txt
  echo $rate >> /tmp/test_rate.txt

  if [[ $rate -gt 60240 ]]; then
    touch /tmp/net_rx_detected.txt
  fi
done
