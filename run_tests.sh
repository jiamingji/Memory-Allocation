make clean; make

n=1

echo "$1"
if [ "$1" = "memcheck" ]
then
	n=0
fi

echo "$n"

for f in traces/*
do
	echo " "
	echo $f
	if [ "$n" -eq 0 ]
	then
		valgrind --leak-check=full ./test_bestfit $f normal
	else
		./test_bestfit $f normal
	fi

	if [ "$n" -eq 0 ]
	then
		valgrind --leak-check=full ./test_bestfit $f defrag
	else
		./test_bestfit $f defrag
	fi

	if [ "$n" -eq 0 ]
	then
		valgrind --leak-check=full ./test_firstfit $f normal
	else
		./test_firstfit $f normal
	fi

	if [ "$n" -eq 0 ]
	then
		valgrind --leak-check=full ./test_firstfit $f defrag
	else
		./test_firstfit $f defrag
	fi
done
exit 0
