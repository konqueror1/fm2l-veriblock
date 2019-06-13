while true;
do
  sudo  ./veri_fm2l -o $1 -u $2 -p $3 -nl $4 ;
  echo "Restarting veri_FM2L miner!";
  sleep 1;
done;

