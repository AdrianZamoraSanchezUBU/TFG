# Trabajo Fin de Grado — Compilador y lenguaje T
Este Trabajo Fin de Grado aborda el diseño e implementación de un lenguaje de programación propio, junto con su compilador y entorno de ejecución, desarrollados con C++, ANTLR, LLVM. El proyecto cubre todas las fases clásicas de un compilador —desde el análisis léxico y sintáctico hasta la generación de código intermedio, optimización y ejecución— incorporando además soporte para eventos temporales y un runtime concurrente.

En este repositorio se incluye el código fuente del compilador y del runtime, ejemplos de programas escritos en el propio lenguaje, la memoria del TFG, así como los materiales de evaluación: 
- [Memoria del TFG](doc/Memoria.pdf)
- [Anexos](doc/Anexos.pdf)
- [Vídeo de defensa](https://youtu.be/zlXMgQ-jRb8) y la [presentación utilizada durante el video](https://www.canva.com/design/DAG9AOcjeK0/MPaNNu_ftCpNQo9uJOKj9w/view?utm_content=DAG9AOcjeK0&utm_campaign=designshare&utm_medium=link2&utm_source=uniquelinks&utlId=h05b067fca0)
- [Vídeo de demostración](https://youtu.be/0ULCn_Cs6-U)
- [Despliegue y pruebas utilizando Docker](#Despliegue-en-Docker)
# Dependencias del proyecto
A continuación se detallan todas las dependencias necesarias para compilar y ejecutar el proyecto.

## Clonar e inicializar submódulos
```bash
git clone --recursive <repository-url>
cd TFG
```

## ANTLR4
### ANTLR4 tool
```bash
sudo apt install -y default-jre
```

```bash
mkdir -p $HOME/tools/antlr
cd $HOME/tools/antlr
wget https://www.antlr.org/download/antlr-4.13.1-complete.jar
```

El proceso de build espera encontrar en la ruta `HOME/tools/antlr` el .jar de la herramienta.

### ANTLR4 C++ runtime 
Descargar desde el repo:
```bash
git clone https://github.com/antlr/antlr4.git
```

Luego instalar con:
```bash
cd antlr4/runtime/Cpp
mkdir build && cd build
cmake ..
make
sudo make install
```

El runtime de ANTLR4 para C++ debería encontrarse instalado en las rutas:
- `/usr/local/include/antlr4-runtime`
- `/usr/local/lib/libantlr4-runtime.*`

> **Nota:** si el runtime se ha instalado en una ruta distinta de `/usr/local` será necesario moverlo a esta ruta o cambiar la configuración de CMake.

## LLVM
Utilizado para la generación de LLVM IR y código máquina.
```bash
sudo apt install -y llvm llvm-dev
```

## libfmt
Librería de formateo de texto, dependencia de **libspdlog**.
```bash
sudo apt install -y libfmt-dev
```

## libspdlog
Librería de logging a consola utilizada por el compilador.
```bash
sudo apt install -y libspdlog-dev
```

## argparse
Librería de para crear parsers de argumentos de consola, utilizado por el compilador. Se puede obtener la lib desde:
```bash
git clone https://github.com/p-ranav/argparse.git
```

## Gtest
Utilizado para la ejecución de tests automatizados.
```bash
sudo apt install -y libgtest-dev
```

## LaTex
Necesario para generar visualizaciones del AST.
```bash
sudo apt install texlive-xetex
```

## Compilación y tests
Para hacer build o ejecutar tests
Se pueden utilizar los comandos estándar:
```
ctest --test-dir build
```
```bash
cmake --build build
```

Puesto que para los cambios en los archivos .g4 de ANTLR4 hay que compilar con la herramienta de ANTLR4, se aporta un sencillo script de linux llamado build.sh el cual compila todo el proyecto incluyendo los .g4. Para ejecutar el build junto a los tests se puede ejecutar:
```bash
./build.sh --test
``` 

# Uso del compilador
## Argumentos

- `<archivo_entrada>`  
  Archivo fuente escrito en el lenguaje T que se va a compilar.

---

## Opciones

- `-o <archivo>`  
  Especifica el nombre del archivo objeto de salida.  
  Por defecto: `out.o`.

- `--debug`  
  Activa el modo de depuración. Muestra información interna del compilador como:
  - Lista de tokens
  - Árbol de Sintaxis Abstracta (AST)
  - Tabla de símbolos
  - LLVM IR generado

- `--visualizeAST`  
  Genera una representación visual del Árbol de Sintaxis Abstracta.  
  Produce un archivo `AST.pdf` en el directorio actual.

- `-IR <archivo>`  
  Emite el LLVM IR generado al archivo especificado.

- `-h, --help`  
  Muestra la ayuda del compilador.

# Despliegue en Docker
Antes de comenzar, se requiere de tener Docker instalado en el sistema.

### Construcción del entorno
Construcción de la imagen Docker que contiene el compilador y su entorno de ejecución:
```bash
docker build -t tlang .
```
> **Nota:** Este proceso puede tardar algunos minutos.

### Acceso y uso del contenedor
Para acceder:
```bash
docker run -it --entrypoint bash tlang
```

### Compilar usando TCompiler
Ejemplo de la compilación del input function.T de la carpeta de ejemplos:
```bash
TCompiler examples/function.T
```
> **Nota:** para estudiar el comportamiento del compilador recomiendo usar ```--debug``` para ver el proceso.

### Ejecutar la salida
Si no se especifica un nombre de la salida con "-o  <nombre_salida>" la salida será "out" y se ejecutará con:
```bash
./out
```
> **Nota:** para ver la salida del programa podemos ejecutar ```echo $?```

#### Visualizar el AST
Con el objetivo de evitar tiempos de build excesivos del contenedor, se ha decidido no incluir la herramienta "texlive-xetex".

Para poder visualizar el diagrama del AST generado con los argumentos ```--debug``` y ```--visualizeAST``` se puede tomar el AST.tex generado en el contenedor y pasar su contenido a local. En local podremos compilar y abrir el PDF con normalidad utilizando:
```bash
xelatex -interaction=nonstopmode <archivo.tex>
```

### Cerrar el contenedor
```bash
exit
```
