El script probando.sh genera las pruebas de forma aleatoria y las guarda en un fichero, variando los parametros x,y,z e i obtenemmos
todas las diferentes pruebas, después lo que hacemos es en la página "https://pinetools.com/es/aleatorizar-lista" mezclarlas, ya 
que por defecto los nodos están en orden. 

1ºEn las de consultas probamos intercalando algún pago por el medio ver como este se atiende lo antes posible.
2ºEn las de pagos probamos la diferencia entre que un nodo acapare la ejecución o que vaya cambiando de nodo.
3ºEn las de prioridades simplemente vamos viendo como se degrada el sistema al tener más nodos.

Para probar el sistema se debe estar en modo root ya que es la forma de que a las colas de mensajes se les aplique el tamaño
especificado en nuestro código y que no se llene en mitad de las pruebas.
Por otra parte, se tiene que ejecutar primero el receptor correspondiente al número de nodos que se empleará en la prueba y después
el main. Una vez haya terminado se muestra un mensaje en la terminal del main.
Por último, para volcar los datos se deben cerrar los terminales de los receptores con click derecho para que así se capture 
la señal SIGHUP y los datos sean volcados en la carpeta "mis_archivos".

*Cabe destacar que los datos no se sobreescriben en cada prueba.