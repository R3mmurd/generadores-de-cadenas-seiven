/*
  Este archivo contiene la definición de los algoritmos para construir cadenas
  por actividad económica.

  Copyright (C) 2017 Corporación de Desarrollo de la Región Los Andes.

  Autor: Alejandro J. Mujica (aledrums en gmail punto com)
  
  Este programa es software libre; Usted puede usarlo bajo los términos de la
  licencia de software GPL versión 2.0 de la Free Software Foundation.
 
  Este programa se distribuye con la esperanza de que sea útil, pero SIN
  NINGUNA GARANTÍA; tampoco las implícitas garantías de MERCANTILIDAD o
  ADECUACIÓN A UN PROPÓSITO PARTICULAR.
  Consulte la licencia GPL para más detalles. Usted debe recibir una copia
  de la GPL junto con este programa; si no, escriba a la Free Software
  Foundation Inc. 51 Franklin Street,5 Piso, Boston, MA 02110-1301, USA.
*/

# ifndef CAEVGEN_H
# define CAEVGEN_H

# include <models.H>
# include <tpl_graph.H>

/** Alias para la información que almacena un nodo del grafo.

    Un nodo almacena el apuntador a la actividad económica que alberga y 
    una etiqueta la cual indica si el nodo es la raíz de la cadena
    (actividad económica sobre la cual empieza a construirse), es un nodo
    aguas arriba o un nodo aguas abajo.
*/
using NodeInfo = pair<CAEV *, NodePosition>;

/// Alias para el tipo de grafo
using Net = List_Graph<Graph_Node<NodeInfo>>;

/** Función que grafica una cadena productiva por actividad económica.

    Esta operación genera un gráfico vectorial en formato svg de la cadena
    productiva.

    @param net El grafo que representa la cadena productiva.
    @param output_name Nombre del archivo de salida.

    @author Alejandro J. Mujica
*/
void plot(const Net & net, const string & output_name);

/* Función que construye toda la red aguas arriba.

   Dado un nodo del grafo, esta operación se encarga de buscar todas las
   actividades económicas a las cuales pertenecen los insumos requeridos por
   todos los productos pertenecientes a la actividad económica albergada en el
   nodo dado, las agrega en el grafo y establece las conexiones mediante
   arcos.

   Esta función construye recursivamente por cada nodo añadido.

   @param net El grafo que representa la cadena productiva.
   @param root El nodo a partir del cual se hará la construcción aguas arriba.
   @param year Año de la producción.
   @param level Nivel de detalle de la actividad económica.
   @param nodes_map Mapeo entre las actividades económicas insertadas y el nodo
   del grafo que la alberga.

   @author Alejandro J. Mujica
*/
void build_upstream(Net & net, Net::Node * root, year_t year, CAEVLevel level,
		    TreeMap<CAEV *, Net::Node *> & nodes_map);

/* Función que construye toda la red aguas arriba.

   Dado un nodo del grafo, esta operación se encarga de buscar todas las
   actividades económicas a las cuales pertenecen los productos que utilizan
   como insumos a todos los productos pertenecientes a la actividad económica 
   albergada en el nodo dado, las agrega en el grafo y establece las conexiones
   mediante arcos.

   Esta función construye recursivamente por cada nodo añadido.

   @param net El grafo que representa la cadena productiva.
   @param root El nodo a partir del cual se hará la construcción aguas arriba.
   @param year Año de la producción.
   @param level Nivel de detalle de la actividad económica.
   @param nodes_map Mapeo entre las actividades económicas insertadas y el nodo
   del grafo que la alberga.

   @author Alejandro J. Mujica
*/
void build_downstream(Net &, Net::Node *, year_t, CAEVLevel,
		      TreeMap<CAEV *, Net::Node *> &);

/** Función que construye una cadena productiva por actividad económica.

    Esta operación construye una red reresentada por un grafo el cual alberga
    actividades económicas en sus nodos. Un arco entre dos nodos indica que
    existen relaciones económicas entre las actividades en los nodos.

    @param lvl Cadena que indica cual es el nivel de detalle para el código de
    la actividad económica,
    @param caev_cod Código de la actividad económica a partir de la cual se va
    a construir la cadena,
    @param year Año de las producciones en la cadena.
    @param input_name nombre del archivo de datos que almacena la información
    de toda la red productiva.
    @param output_name Nombre del archivo svg en el cual se graficará la cadena.

    @author Alejandro Mujica
*/
void generate_caev_chain(const string & lvl, const string & caev_cod,
			 year_t year, const string & input_name,
			 const string & output_name);

# endif // CAEVGEN_H
