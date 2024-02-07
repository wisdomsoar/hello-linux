if [[ ! -f /tmp/xs_trigger_reset.txt ]]; then
  echo "/tmp/xs_trigger_reset.txt not exist!"
  return 0
fi

previous=$(cat /tmp/xs_trigger_reset.txt)
rm /tmp/xs_trigger_reset.txt

current=$(date +%s)
delta=$(( $current - $previous ))

if [[ $delta -ge 5 ]]; then
  echo "yes | firstboot" > /tmp/xs_trigger_reset_tbd_to_do_reset_to_default.txt
fi

reboot

