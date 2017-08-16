# generadores-de-cadenas-seiven

Conjunto de programas para la construcción y visualización de cadenas
de producción.

Dependencias para utilizar este paquete:

- Compilador de C++ que soporte la versión C++14.
- Graphviz
- PostgreSQL versión >= 9.0
- libpq-dev
- libtclap-dev
- Biblioteca de algoritmos y estructuras de datos Aleph-w, disponible en:
  https://sourceforge.net/projects/aleph-w/

Este paquete contiene los siguientes archivos:

* Makefile con las reglas para compilar los diversos programas. Este Makefile
  está configurado para compilar con clang++. Si tiene otro compilador,
  entonces modifique la variable CXX.

* README.md: este archivo.

* caev-gen.H y caev-gen.C contiene los algoritmos necesarios para construir
  la cadena por actividad económica.

* models.H y models.C contiene las estructuras de datos para el modelo de
  mapa necesario para la construcción de las diversas cadenas.

* process.H contiene una clase utilitaria que envuelve el llamado a procesos
externos.

* dbloader.C

  Este programa se encarga leer una base de datos sidepro, los carga en el
  modelo diseñado y lo almacena en un archivo de texto plano para
  posteriormente ser leído por los programas que construyen las cadenas.

  - Compilación en modo depuración: make dbloader-dbg
  - Compilación en modo optimizado: make dbloader
  - Para obtener ayuda de cómo ejecutar este programa,
    ejecute ./dbloader --help

* main-caev-gen.C

  Este programa construye una cadena productiva por actividad económica.
  Recibe como parámetros en su ejecución el nivel de actividad económica
  (sección, división, grupo, clase o rama), el código de la actividad
  económica, el año de la producción, el nombre del archivo de texto con los
  datos y el nombre de la archivo de salida el cual será una imagen vectorial
  en formato svg.

  - Compilación en modo depuración: make main-caev-gen-dbg
  - Compilación en modo optimizado: make main-caev-gen
  - Para obtener ayuda de cómo ejecutar este programa,
  ejecute ./main-caev-gen --help

* DB (Directorio) contiene una pequeña biblioteca para las consultas en la
  base de datos PostgreSQL. Ejecute el comando make para generar la biblioteca
  estática libDbAccess.a