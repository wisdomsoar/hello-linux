if [[ ! -f /tmp/xs_trigger_reset.txt ]]; then
  echo "/tmp/xs_trigger_reset.txt not exist!"
  return 0
fi


while true;
do
  previous=$(cat /tmp/xs_trigger_reset.txt)

  current=$(date +%s)
  delta=$(( $current - $previous ))

  if [[ $delta -ge 5 ]]; then
    while true;
    do echo "255 > /sys/devices/platform/10026000.spmi/spmi-0/0-04/mtk-pmic-leds/leds/led9515:green:cellular-radio/brightness" > /tmp/xs_trigger_reset_tbd_do_turn_on_led.txt
      sleep 1
      echo "0 > /sys/devices/platform/10026000.spmi/spmi-0/0-04/mtk-pmic-leds/leds/led9515:green:cellular-radio/brightness" > /tmp/xs_trigger_reset_tbd_do_turn_off_led.txt
      sleep 1
      if [[ ! -f /tmp/xs_trigger_reset.txt ]]; then
        return 0
      fi
    done
  fi
  if [[ ! -f /tmp/xs_trigger_reset.txt ]]; then
    return 0
  fi
  sleep 1
done

