# generadores-de-cadenas-seiven

Conjunto de programas para la construcción y visualización de cadenas
productivas.

Trabajo desarrollado para la Corporación de Desarrollo Región Los Andes
(CORPOANDES) para ser incorporado en el proyecto Sistema Estadístico Integral
de Venezuela (SEIVEN) desarrollado en el Centro Nacional de Desarrollo e
Investigación en Tecnologías Libres (CENDITEL).

Dependencias para utilizar este paquete:

- Compilador de C++ que soporte la versión C++14.
- Graphviz
- PostgreSQL versión >= 9.0
- libpq-dev
- libtclap-dev
- Biblioteca de algoritmos y estructuras de datos Aleph-w, disponible en:
  https://sourceforge.net/projects/aleph-w/

Este paquete contiene los siguientes archivos:

* README.md: este archivo.

* LICENSE: Licencia Pública General de GNU versión 2 (GPLv2).

* Makefile: Reglas para compilar los diversos programas. Este Makefile
  está configurado para compilar con clang++. Si tiene otro compilador,
  entonces modifique la variable CXX.

* process.H contiene una clase utilitaria que envuelve el llamado a procesos
  externos.

* models.H y models.C: Contienem las estructuras de datos para el modelo de
  mapa necesario para la construcción de las diversas cadenas.

* caev-gen.H y caev-gen.C: Contienen los algoritmos necesarios para construir
  la cadena por actividad económica.

* tariffcode-gen.H y tariffcode-gen.C: Contienen los algoritmos necesarios para
  construir la cadena por código arancelario, tanto para la vista por unidades
  económicas como para la vista por productos.

* ue-gen.H y ue-gen.C: Contienen los algoritmos necesarios para construir
  la cadena por unidad económica.

* product-gen.H y product-gen.C: Contienen los algoritmos necesarios para
  construir la cadena por unidad económica.

* levenshtein.H: Contiene la instrumentación del algoritmo de distancia de
  Levenshtein el cual es utilizado para buscar cadenas que más se acerquen al
  momento de emparejar los nombres de los insumos con los productos.

* maploader.C: Programa que lee una base de datos SIDEPRO.

  Este programa se encarga leer la base de datos, los carga en el
  modelo diseñado y lo almacena en un archivo de texto plano para
  posteriormente ser leído por los programas que construyen las cadenas.

  - Compilación en modo depuración: make maploader-dbg
  - Compilación en modo optimizado: make maploader
  - Para obtener ayuda de cómo ejecutar este programa,
    ejecute ./maploader --help

* main-caev-gen.C: Programa que construye cadenas productivas por actividad
  económica.

  Recibe como parámetros en su ejecución el nivel de actividad económica
  (sección, división, grupo, clase o rama), el código de la actividad
  económica, el año de la producción, el nombre del archivo de texto con los
  datos y el nombre de la archivo de salida el cual será una imagen vectorial
  en formato svg.

  - Compilación en modo depuración: make main-caev-gen-dbg
  - Compilación en modo optimizado: make main-caev-gen
  - Para obtener ayuda de cómo ejecutar este programa,
  ejecute ./main-caev-gen --help

* main-tariffcode-ue-gen.C: Programa que construye cadenas productivas por
  código arancelario con vista por unidades económicas.

  Recibe como parámetros en su ejecución el nivel de código arancelario
  (sección, capítulo, partida, sub partida, sub sub partida), el código
  arancelario, el año de la producción, el nombre del archivo de texto con los
  datos y el nombre de la archivo de salida el cual será una imagen vectorial
  en formato svg.

  - Compilación en modo depuración: make main-tariffcode-ue-gen-dbg
  - Compilación en modo optimizado: make main-tariffcode-ue-gen
  - Para obtener ayuda de cómo ejecutar este programa,
  ejecute ./main-tariffcode-ue-gen --help

* main-tariffcode-product-gen.C: Programa que construye cadenas productivas por
  código arancelario con vista por productos.

  Recibe como parámetros en su ejecución el nivel de código arancelario
  (sección, capítulo, partida, sub partida, sub sub partida), el código
  arancelario, el año de la producción, el nombre del archivo de texto con los
  datos y el nombre de la archivo de salida el cual será una imagen vectorial
  en formato svg.

  - Compilación en modo depuración: make main-tariffcode-product-gen-dbg
  - Compilación en modo optimizado: make main-tariffcode-product-gen
  - Para obtener ayuda de cómo ejecutar este programa,
  ejecute ./main-tariffcode-product-gen --help

* main-ue-gen.C: Programa que construye cadenas productivas por unidad económica.

  Recibe como parámetros en su ejecución o el RIF o el nombre de la empresa
  (no ambos), el año de la producción, el nombre del archivo de texto con los
  datos y el nombre de la archivo de salida el cual será una imagen vectorial
  en formato svg.

  - Compilación en modo depuración: make main-ue-gen-dbg
  - Compilación en modo optimizado: make main-ue-gen
  - Para obtener ayuda de cómo ejecutar este programa,
  ejecute ./main-ue-gen --help

* main-product-gen.C: Programa que construye cadenas productivas por producto.

  Recibe como parámetros en su ejecución el número de id del producto en la base
  de datos, el año de la producción, la cantidad máxima de niveles aguas arriba
  que se construirá, la cantidad máxima de niveles aguas abajo que se construirá,
  la distancia máxima de edición entre los nombres de productos en insumos,
  el nombre del archivo de texto con los datos y el nombre de la archivo de
  salida el cual será una imagen vectorial en formato svg.

  - Compilación en modo depuración: make main-product-gen-dbg
  - Compilación en modo optimizado: make main-product-gen
  - Para obtener ayuda de cómo ejecutar este programa,
  ejecute ./main-product-gen --help

Adicionalmente, el paquete contiene el siguiente sub directorio:

* DB: Contiene una pequeña biblioteca para las consultas en la  base de datos
  PostgreSQL.