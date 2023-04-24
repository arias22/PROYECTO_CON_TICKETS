#!/bin/bash

#Borrar
clear

# Compilar main
gcc main_v3.c -o main_v3

# Compilar receptor
gcc receptor_v3.c -o receptor_v3

# Define la lista de programas y sus argumentos 
#0-> Pagos y Anulaciones 1->Reservas 2->Administración 3->Consultas
#Prueba 1
programas_main=(
  "./main_v3 0 2 0"
  "./main_v3 0 2 2"
  "./main_v3 0 2 2"
  "./main_v3 1 2 0"
  "./main_v3 1 2 3"
)

programas_receptor=(
"./receptor_v3 0 2"
 "./receptor_v3 1 2"

  )

# Prueba 2
#   programas_main=(
#   "./main_v3 0 2 1"
#   "./main_v3 0 2 0"
#   "./main_v3 0 2 0"
#   "./main_v3 1 2 2"
#   "./main_v3 1 2 1"
#   "./main_v3 1 2 0"
# )

# programas_receptor=(
# "./receptor_v3 0 2"
#  "./receptor_v3 1 2"

#   )


# Prueba 3
#   programas_main=(
#   "./main_v3 0 3 3"
#   "./main_v3 0 3 3"
#   "./main_v3 0 3 3"
#   "./main_v3 1 3 3"
#   "./main_v3 1 3 3"
#   "./main_v3 1 3 3"
#   "./main_v3 2 3 3"
#   "./main_v3 2 3 0"
#   "./main_v3 2 3 3"

# )

# programas_receptor=(
# "./receptor_v3 0 3"
#  "./receptor_v3 1 3"
#  "./receptor_v3 2 3"

#   )

# Prueba 4
#   programas_main=(
#   "./main_v3 0 4 1"
#   "./main_v3 0 4 0"
#   "./main_v3 1 4 3"
#   "./main_v3 2 4 0"
#   "./main_v3 2 4 0"
#   "./main_v3 2 4 1"
#   "./main_v3 3 4 3"
#   "./main_v3 3 4 3"

# )

# programas_receptor=(
# "./receptor_v3 0 4"
#  "./receptor_v3 1 4"
#  "./receptor_v3 2 4"
#  "./receptor_v3 3 4"

#   )
i=1
j=1

# Itera sobre la lista de programas y abre una nueva ventana de terminal para cada uno
for programa in "${programas_receptor[@]}"
do
  gnome-terminal --title="Nodo $i - Receptor" -- bash -c "$programa; exec bash"
  
  i=$((i+1))

done

for programa in "${programas_main[@]}"
do
  gnome-terminal --title="Nodo $j - Main" -- bash -c "$programa; exec bash"
  
  j=$((j+1))

done
