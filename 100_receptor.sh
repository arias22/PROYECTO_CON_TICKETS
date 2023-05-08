#!/bin/bash

#Prueba de concurrencia de consultas 

#Borrar
clear



# Define la lista de programas y sus argumentos 
#1-> Pagos y Anulaciones 2->Reservas 3->Administración 4->Consultas





programas_receptor=(
"sudo ./receptor_v3 0 100"
"sudo ./receptor_v3 1 100"
"sudo ./receptor_v3 2 100"
"sudo ./receptor_v3 3 100"
"sudo ./receptor_v3 4 100"
"sudo ./receptor_v3 5 100"
"sudo ./receptor_v3 6 100"
"sudo ./receptor_v3 7 100"
"sudo ./receptor_v3 8 100"
"sudo ./receptor_v3 9 100"
"sudo ./receptor_v3 10 100"
"sudo ./receptor_v3 11 100"
"sudo ./receptor_v3 12 100"
"sudo ./receptor_v3 13 100"
"sudo ./receptor_v3 14 100"
"sudo ./receptor_v3 15 100"
"sudo ./receptor_v3 16 100"
"sudo ./receptor_v3 17 100"
"sudo ./receptor_v3 18 100"
"sudo ./receptor_v3 19 100"
"sudo ./receptor_v3 20 100"
"sudo ./receptor_v3 21 100"
"sudo ./receptor_v3 22 100"
"sudo ./receptor_v3 23 100"
"sudo ./receptor_v3 24 100"
"sudo ./receptor_v3 25 100"
"sudo ./receptor_v3 26 100"
"sudo ./receptor_v3 27 100"
"sudo ./receptor_v3 28 100"
"sudo ./receptor_v3 29 100"
"sudo ./receptor_v3 30 100"
"sudo ./receptor_v3 31 100"
"sudo ./receptor_v3 32 100"
"sudo ./receptor_v3 33 100"
"sudo ./receptor_v3 34 100"
"sudo ./receptor_v3 35 100"
"sudo ./receptor_v3 36 100"
"sudo ./receptor_v3 37 100"
"sudo ./receptor_v3 38 100"
"sudo ./receptor_v3 39 100"
"sudo ./receptor_v3 40 100"
"sudo ./receptor_v3 41 100"
"sudo ./receptor_v3 42 100"
"sudo ./receptor_v3 43 100"
"sudo ./receptor_v3 44 100"
"sudo ./receptor_v3 45 100"
"sudo ./receptor_v3 46 100"
"sudo ./receptor_v3 47 100"
"sudo ./receptor_v3 48 100"
"sudo ./receptor_v3 49 100"
"sudo ./receptor_v3 50 100"
"sudo ./receptor_v3 51 100"
"sudo ./receptor_v3 52 100"
"sudo ./receptor_v3 53 100"
"sudo ./receptor_v3 54 100"
"sudo ./receptor_v3 55 100"
"sudo ./receptor_v3 56 100"
"sudo ./receptor_v3 57 100"
"sudo ./receptor_v3 58 100"
"sudo ./receptor_v3 59 100"
"sudo ./receptor_v3 60 100"
"sudo ./receptor_v3 61 100"
"sudo ./receptor_v3 62 100"
"sudo ./receptor_v3 63 100"
"sudo ./receptor_v3 64 100"
"sudo ./receptor_v3 65 100"
"sudo ./receptor_v3 66 100"
"sudo ./receptor_v3 67 100"
"sudo ./receptor_v3 68 100"
"sudo ./receptor_v3 69 100"
"sudo ./receptor_v3 70 100"
"sudo ./receptor_v3 71 100"
"sudo ./receptor_v3 72 100"
"sudo ./receptor_v3 73 100"
"sudo ./receptor_v3 74 100"
"sudo ./receptor_v3 75 100"
"sudo ./receptor_v3 76 100"
"sudo ./receptor_v3 77 100"
"sudo ./receptor_v3 78 100"
"sudo ./receptor_v3 79 100"
"sudo ./receptor_v3 80 100"
"sudo ./receptor_v3 81 100"
"sudo ./receptor_v3 82 100"
"sudo ./receptor_v3 83 100"
"sudo ./receptor_v3 84 100"
"sudo ./receptor_v3 85 100"
"sudo ./receptor_v3 86 100"
"sudo ./receptor_v3 87 100"
"sudo ./receptor_v3 88 100"
"sudo ./receptor_v3 89 100"
"sudo ./receptor_v3 90 100"
"sudo ./receptor_v3 91 100"
"sudo ./receptor_v3 92 100"
"sudo ./receptor_v3 93 100"
"sudo ./receptor_v3 94 100"
"sudo ./receptor_v3 95 100"
"sudo ./receptor_v3 96 100"
"sudo ./receptor_v3 97 100"
"sudo ./receptor_v3 98 100"
"sudo ./receptor_v3 99 100"
  )




for programa in "${programas_receptor[@]}"
do
    gnome-terminal --title="Nodo - ${programa}" -- bash -c "$programa; exec bash"
 

done