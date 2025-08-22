#!/bin/bash




programas_receptor=(
"sudo ./receptor_v3 0 4"
"sudo ./receptor_v3 1 4"
"sudo ./receptor_v3 2 4"
"sudo ./receptor_v3 3 4"
  )



for programa in "${programas_receptor[@]}"
do
    gnome-terminal --title="Nodo - ${programa}" -- bash -c "$programa; exec bash"
 

done