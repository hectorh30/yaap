Este proyecto fue compilado con DJCPP (http://www.delorie.com/djgpp/) sobre DOSBOX

Instrucciones para compilar:
    * Setear variables de entorno en archivo de configuración de DOSBOX:
        set path=Z:;C:\DJGPP\bin
        set DJGPP=C:\DJGPP\DJGPP.ENV
    
    * Dentro de la carpeta del código fuente, compilar con: gpp -o main.exe main.cpp

Las clases desarrolladas incluidas son:
    * CANVAS: Ejecuta las operaciones de dibujo en la pantalla
    * MANAGER: Se encarga de la interfaz entre el canvas, el mouse, el teclado (recibe los comandos y ejecuta el ciclo principal del programa)
    * MOUSE: Encargado de la interfaz con el mouse (movimientos, dibujado del pointer, banderas para clicks izquierdo y derecho)
    
Instrucciones de ejecución:
    Una vez dentro del programa, utilizar los siguientes comandos:
    
    * SHIFT + A: polygon move
    * SHIFT + B: bucket fill
    * SHIFT + C: circle
    * SHIRT + D: pick tree polygon
    * SHIFT + E: ellipse
    * SHIFT + F: polygon scale
    * SHIFT + G: rectangle
    * SHIFT + H: pick house polygon
    * SHIFT + I: shear in x
    * SHIFT + J: shear in y
    * SHIFT + K: polygon rotate  
    * SHIFT + L: line
    * SHIFT + M: fill polygon
    * SHIFT + N: new
    * SHIFT + O: polygon (no fill)
    * SHIFT + P: pencil
    * SHIFT + Q: exit
    * SHIFT + R: eraser
    * SHIFT + S: spray
    * SHIFT + T: text
    * SHIFT + U: toggle polygon fill
    * SHIFT + W: color picker

NOTA: Las operaciones con polígonos REQUIEREN que usted elija un poligono antes (SHIFT + H para casa o SHIFT + D para árbol). De lo contrario el programa _truena_.
