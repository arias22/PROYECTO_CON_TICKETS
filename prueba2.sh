#!/bin/bash




programas_receptor=(
"./receptor_v3 0 5"
"./receptor_v3 1 5"
"./receptor_v3 2 5"
"./receptor_v3 3 5"
"./receptor_v3 4 5"
  )


# programas_receptor=(
# "./receptor_v3 0 10"
# "./receptor_v3 1 10"
# "./receptor_v3 2 10"
# "./receptor_v3 3 10"
# "./receptor_v3 4 10"
# "./receptor_v3 5 10"
# "./receptor_v3 6 10"
# "./receptor_v3 7 10"
# "./receptor_v3 8 10"
# "./receptor_v3 9 10"
#   )




for programa in "${programas_receptor[@]}"
do
     gnome-terminal --title="Nodo - ${programa}" -- bash -c "$programa; exec bash"
  

done