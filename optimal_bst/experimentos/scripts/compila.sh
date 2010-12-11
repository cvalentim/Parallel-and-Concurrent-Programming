lamclean
mpiCC d_opt_bst2.cpp
mrcp 0-8 a.out ~
mpirun -np $1 a.out < $2 2> $3
cat $3
