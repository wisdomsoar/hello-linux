echo $(date +%s) > /tmp/xs_trigger_reset.txt
/etc/reset2default/flash_led.sh &
