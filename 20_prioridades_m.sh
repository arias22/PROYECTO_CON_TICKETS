#!/bin/bash

#Prueba de concurrencia de consultas 

#Borrar
clear



# Define la lista de programas y sus argumentos 
#0-> Pagos y Anulaciones 1->Reservas 2->Administración 3->Consultas


programas_main=(
  "sudo ./main_v3 0 4 1 0"
  "sudo ./main_v3 0 4 2 1"
  "sudo ./main_v3 0 4 4 2"
  "sudo ./main_v3 3 4 3 0" 
  "sudo ./main_v3 3 4 1 1"
  "sudo ./main_v3 2 4 1 0" 
  "sudo ./main_v3 2 4 4 1"
  "sudo ./main_v3 2 4 2 2"
  "sudo ./main_v3 1 4 1 0"
  "sudo ./main_v3 1 4 3 1"
  "sudo ./main_v3 1 4 1 2"
  "sudo ./main_v3 1 4 1 3"
  "sudo ./main_v3 1 4 4 4"
  "sudo ./main_v3 0 4 2 3"
  "sudo ./main_v3 0 4 4 4"
  "sudo ./main_v3 3 4 3 2"
  "sudo ./main_v3 3 4 4 3"
  "sudo ./main_v3 3 4 2 4"
  "sudo ./main_v3 2 4 1 3"
  "sudo ./main_v3 2 4 2 4"
)






procesos_lanzados=0


# Itera sobre la lista de programas y abre una nueva ventana de terminal para cada uno

i=1
for programa in "${programas_main[@]}"
do
echo "Ejecutando $i"
 $programa &
     ((procesos_lanzados++))
((i++))

done

# Esperar a que todos los procesos hayan finalizado
while [ $procesos_lanzados -gt 0 ]
do
    wait
    ((procesos_lanzados--))
done

echo "Todos los procesos han finalizado"
