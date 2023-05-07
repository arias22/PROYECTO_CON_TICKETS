#!/bin/bash

# Define los valores máximos y mínimos de x e y
min_x=0
max_x=9
y=10
min_z=1
max_z=4


# Define la cantidad de ejemplos que quieres generar
num_examples=200

# Crea el archivo donde se guardarán los ejemplos
output_file="ejemplos200.txt"

#vaciar el txt

truncate -s 0 $output_file

touch $output_file

for (( x=$min_x; x<=$max_x; x++ )); do
    for (( i=0; i<=19; i++ )); do
        z=$(( RANDOM % ($max_z - $min_z + 1) + $min_z ))
        echo "\"./main_v3 $x $y $z $i\"" >> $output_file
    done
done

echo "Se han generado $num_examples ejemplos y se han guardado en $output_file."

