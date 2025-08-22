#!/bin/bash

# Define los valores máximos y mínimos de x e y
min_x=0
max_x=99
y=100
min_z=1
max_z=4

# Define la cantidad de ejemplos que quieres generar
num_examples=5000

# Crea el archivo donde se guardarán los ejemplos
output_file="ejemplos5000.txt"


#vaciar el txt

truncate -s 0 $output_file

touch $output_file

for (( x=$min_x; x<=$max_x; x++ )); do
    for (( i=0; i<=9; i++ )); do
        z=$(( RANDOM % ($max_z - $min_z + 1) + $min_z ))
        echo "\"sudo ./main_v3 $x $y $z $i\"" >> $output_file
    done
done

echo "Se han generado $num_examples ejemplos y se han guardado en $output_file."

