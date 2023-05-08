#!/bin/bash



programas_receptor=(
"sudo ./receptor_v3 0 50"
"sudo ./receptor_v3 1 50"
"sudo ./receptor_v3 2 50"
"sudo ./receptor_v3 3 50"
"sudo ./receptor_v3 4 50"
"sudo ./receptor_v3 5 50"
"sudo ./receptor_v3 6 50"
"sudo ./receptor_v3 7 50"
"sudo ./receptor_v3 8 50"
"sudo ./receptor_v3 9 50"
"sudo ./receptor_v3 10 50"
"sudo ./receptor_v3 11 50"
"sudo ./receptor_v3 12 50"
"sudo ./receptor_v3 13 50"
"sudo ./receptor_v3 14 50"
"sudo ./receptor_v3 15 50"
"sudo ./receptor_v3 16 50"
"sudo ./receptor_v3 17 50"
"sudo ./receptor_v3 18 50"
"sudo ./receptor_v3 19 50"
"sudo ./receptor_v3 20 50"
"sudo ./receptor_v3 21 50"
"sudo ./receptor_v3 22 50"
"sudo ./receptor_v3 23 50"
"sudo ./receptor_v3 24 50"
"sudo ./receptor_v3 25 50"
"sudo ./receptor_v3 26 50"
"sudo ./receptor_v3 27 50"
"sudo ./receptor_v3 28 50"
"sudo ./receptor_v3 29 50"
"sudo ./receptor_v3 30 50"
"sudo ./receptor_v3 31 50"
"sudo ./receptor_v3 32 50"
"sudo ./receptor_v3 33 50"
"sudo ./receptor_v3 34 50"
"sudo ./receptor_v3 35 50"
"sudo ./receptor_v3 36 50"
"sudo ./receptor_v3 37 50"
"sudo ./receptor_v3 38 50"
"sudo ./receptor_v3 39 50"
"sudo ./receptor_v3 40 50"
"sudo ./receptor_v3 41 50"
"sudo ./receptor_v3 42 50"
"sudo ./receptor_v3 43 50"
"sudo ./receptor_v3 44 50"
"sudo ./receptor_v3 45 50"
"sudo ./receptor_v3 46 50"
"sudo ./receptor_v3 47 50"
"sudo ./receptor_v3 48 50"
"sudo ./receptor_v3 49 50"
  )






for programa in "${programas_receptor[@]}"
do
    gnome-terminal --title="Nodo - ${programa}" -- bash -c "$programa; exec bash"
 

done
