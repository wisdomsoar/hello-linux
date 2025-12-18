
rm /tmp/movement_detected.txt
rm /tmp/net_rx_detected.txt
./touch_if_detected.sh &
./touch_if_net_rx.sh &


while [ 1 ];
do
  #sleep 60
  # 1.5 hr
  sleep $((3 * 60 *   30))

  if [[ -e /tmp/movement_detected.txt ]]; then
    echo "alive"
    rm /tmp/movement_detected.txt
  else
    if [[ -e /tmp/net_rx_detected.txt ]]; then
      echo "alive net rx"
      rm /tmp/net_rx_detected.txt
    else
      echo "suspend after 10 sec"
      sleep 10
      echo enter_your_root_password_here | sudo -S systemctl suspend
    fi
  fi
done

