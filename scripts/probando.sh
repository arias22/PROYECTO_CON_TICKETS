#!/bin/bash

# Definimos en la x los valores max y min de los nodos, en la y el numero de nodos totales y en la z las prioridades
min_x=0
max_x=49
y=50
min_z=1
max_z=4



# Crea el archivo donde se guardan los ejemplos
output_file="ejemplos.txt"


#vaciar el txt

truncate -s 0 $output_file

touch $output_file

for (( x=$min_x; x<=$max_x; x++ )); do
    for (( i=0; i<=99; i++ )); do
        z=$(( RANDOM % ($max_z - $min_z + 1) + $min_z ))
        echo "\"sudo ./main_v3 $x $y $z $i\"" >> $output_file
    done
done

echo "Se han generado los ejemplos y se han guardado en $output_file."

