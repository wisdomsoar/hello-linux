set -x

wl mpc 0

sleep 1

wl phy_watchdog 0

sleep 1

wl up

sleep 1

wl country ALL

sleep 1

# b = 2.4 GHz
wl band b

sleep 1

# 7 = center freq channel in 2.4 GHz band
wl channel 7

sleep 1

wl phy_forcecal 1

sleep 1

wl scansuppress 1

sleep 1

#--> read rxdfrmocast/rxdfrmucastmbss for multi-cast/uni-cast packets, take this as counter#1

counter_result=$(wl counters)

set +x

rxdfrmocast1=$(echo $counter_result     | grep -o -m 1 -e "rxdfrmocast [0-9]*"     | grep -o -e "[0-9]*")
rxdfrmucastmbss1=$(echo $counter_result | grep -o -m 1 -e "rxdfrmucastmbss [0-9]*" | grep -o -e "[0-9]*")

x=$1

echo "Generate a 11b/g waveform that contains $x number of packets."

read  answer </dev/tty

echo "sleep 7..."
sleep 7

#read the same counter count again, take this as counter#2
#PER% = { [ X - (counter#2 - counter#1) ] / X } * 100 %

counter_result=$(wl counters)

rxdfrmocast2=$(echo $counter_result     | grep -o -m 1 -e "rxdfrmocast [0-9]*"     | grep -o -e "[0-9]*")
rxdfrmucastmbss2=$(echo $counter_result | grep -o -m 1 -e "rxdfrmucastmbss [0-9]*" | grep -o -e "[0-9]*")

echo "rxdfrmocast2: "$rxdfrmocast2
echo "rxdfrmocast1: "$rxdfrmocast1
echo "rxdfrmucastmbss2: "$rxdfrmucastmbss2
echo "rxdfrmucastmbss1: "$rxdfrmucastmbss1

delta_rxdfrmocast=$(echo "$rxdfrmocast2 - $rxdfrmocast1" | bc)
delta_rxdfrmucastmbss=$(echo "$rxdfrmucastmbss2 - $rxdfrmucastmbss1" | bc)

echo "delta_rxdfrmocast: " $delta_rxdfrmocast
echo "delta_rxdfrmucastmbss: " $delta_rxdfrmucastmbss

if [[ "$x" == "" ]]; then
  echo "Use wifi_rx.sh [x] to input the generated x number of packets..."
else
  if (( $x > 0 )); then
    per_rxdfrmocast=$(echo "scale=3; ( ($x - $delta_rxdfrmocast )/ $x ) * 100 " | bc)
    per_rxdfrmucastmbss=$(echo "scale=3; ( ($x - $delta_rxdfrmucastmbss )/ $x ) * 100 " | bc)
    echo "per_rxdfrmocast: "$per_rxdfrmocast"%"
    echo "per_rxdfrmucastmbss: "$per_rxdfrmucastmbss"%"
  fi

fi
