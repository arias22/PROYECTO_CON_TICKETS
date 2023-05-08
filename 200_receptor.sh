#!/bin/bash





programas_receptor=(
"sudo ./receptor_v3 0 10"
"sudo ./receptor_v3 1 10"
"sudo ./receptor_v3 2 10"
"sudo ./receptor_v3 3 10"
"sudo ./receptor_v3 4 10"
"sudo ./receptor_v3 5 10"
"sudo ./receptor_v3 6 10"
"sudo ./receptor_v3 7 10"
"sudo ./receptor_v3 8 10"
"sudo ./receptor_v3 9 10"
)







for programa in "${programas_receptor[@]}"
do
    gnome-terminal --title="Nodo - ${programa}" -- bash -c "$programa; exec bash"
 

done