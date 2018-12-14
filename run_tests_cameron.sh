make clean; make
for f in traces/*
do
	echo " "
	echo $f
	./test_bestfit $f normal
	./test_bestfit $f defrag
	./test_firstfit $f normal
	./test_firstfit $f defrag
done
exit 0
