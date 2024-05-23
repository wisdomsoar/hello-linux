

while [ 1 ]; 
do
  xinput --test-xi2 --root | grep -q "EVENT type"
  touch /tmp/movement_detected.txt
  sleep 5
done
