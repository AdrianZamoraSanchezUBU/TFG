
# Dependencias del proyecto
A continuación se detallan todas las dependencias necesarias para compilar y ejecutar el proyecto.

## ANTLR4
### ANTLR4 tool
`
sudo apt install -y default-jre
`

```
mkdir -p $HOME/tools/antlr
cd $HOME/tools/antlr
wget https://www.antlr.org/download/antlr-4.13.1-complete.jar
```

El proceso de build espera encontrar en la ruta `HOME/tools/antlr` el .jar de la herramienta.

### ANTLR4 C++ runtime 
Descargar desde el repo:
`
git clone https://github.com/antlr/antlr4.git
`

Luego instalar con:
```
cd antlr4/runtime/Cpp
mkdir build && cd build
cmake ..
make
sudo make install
```

El runtime de ANTLR4 para C++ debería encontrarse en las rutas:
`/usr/local/include/antlr4-runtime` y 
`/usr/local/lib/libantlr4-runtime.*`

Nota: si el runtime se ha instalado en una ruta distinta de `/usr/local` será necesario moverlo a esta ruta o cambiar la configuración de CMake.

## LLVM
Utilizado para la generación de LLVM IR y código máquina.
`
sudo apt install -y llvm llvm-dev
`

## libspdlog
Librería de logging utilizada por el compilador.
`
sudo apt install -y libspdlog-dev
`

## Gtest
Utilizado para la ejecución de tests automatizados.
`
sudo apt install -y libgtest-dev
`

## Compilación y tests
Para hacer build o ejecutar tests
Se pueden utilizar los comandos estándar:
```
ctest --test-dir build
```
```
cmake --build build
```

Puesto que para los cambios en los archivos .g4 de ANTLR4 hay que compilar con la herramienta de ANTLR4, se aporta un sencillo script de linux llamado build.sh el cual compila todo el proyecto incluyendo los .g4. Para ejecutar el build junto a los tests se puede ejecutar:
```
./build.sh --test
``` 

# Uso del compilador
TODO