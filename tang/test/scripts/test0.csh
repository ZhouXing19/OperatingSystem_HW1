#! /bin/csh -f

echo "An ERROR has occurred" > results/0.err
touch results/0.out
echo 1 > results/0.status


#touch in.txt
./mysh in.txt bar.txt

