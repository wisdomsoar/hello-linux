# wifi_tx.sh [power_in_dBm] [Core]

set -x

wl mpc 0

sleep 1

wl phy_watchdog 0

sleep 1

wl up

sleep 1

wl country ALL

sleep 1

#--> b = 2.4 GHz
wl band b

sleep 1

#--> 2g = 2.4 GHz, -r = 11b/g rate, -b = 20 MHz bandwidth
wl 2g_rate -r 11 -b 20

#--> 7 = center freq channel in 2.4 GHz band
wl channel 7

sleep 1

wl phy_forcecal 1

sleep 1

wl scansuppress 1

sleep 1

#---[for SISO operation, select antenna core to transmit]---
#--> X = 1 for Core 0; X = 2 for Core 1
#> wl txcore -k [X] -o [X]
if [[ "$2" == "" ]]; then
  echo "no txcore specified..."
else
  wl txcore $2
fi


#----[for closed loop power control]----
wl phy_txpwrctrl 1

sleep 1

if [[ "$1" == "-1" ]] || [[ "$1" == "" ]]; then
  # default target power
  wl txpwr1 -1
else
  # Tx power in dBm
  wl txpwr1 -o -d $1
fi

sleep 1

#----[for open loop power control]----
#wl phy_txpwrctrl 0

#--> X = Tx power index from range 0-127
#wl phy_txpwrindex 0

sleep 1

#------------------------------------------------
#--> start Tx transmission
wl pkteng_start 00:11:22:33:44:55 tx 100 1024 0

sleep 1

# --> stop Tx transmission
#wl pkteng_stop tx
