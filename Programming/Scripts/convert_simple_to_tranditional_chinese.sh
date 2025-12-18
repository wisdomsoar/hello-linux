
#########   Stupid to convert simple Chinese to tranditional Chinese by s2tw

string=$(ls -1a)
IFS=$'\n'
counter=1

while read -r line; do
  echo "Line $counter: $line"

  if [[ $line == *".wav"* ]]; then
     # you should download s2tw before use this script: https://github.com/magiclen/s2tw
     twname=$(echo $line | ./s2tw)
     echo "mv $line $twname"
     mv "$line" "$twname"
  fi
  ((counter++))
done <<< "$string"
