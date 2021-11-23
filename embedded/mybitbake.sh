######## simple script for bitbake...

i=2
j=$#

recipe=$1
shift 1

while [ $i -le $j ]
do
  echo $1
  set -x
  bitbake $recipe -c $1 -f
  set +x
  i=$((i+1))
  shift 1
done
