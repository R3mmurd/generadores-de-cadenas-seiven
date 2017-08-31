/*
  Este archivo contiene la definición de los algoritmos para construir cadenas
  por unidad económica.

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

# ifndef UEGEN_H
# define UEGEN_H

# include <models.H>
# include <tpl_graph.H>

/** Tipo enumerado que indica cual clave de búsqueda se utiliza para una
    unidad económica.

    @author Alejandro J. Mujica
*/
enum UEKey { RIF, NAME };

/** Alias para la información que almacena un nodo del grafo.

    Un nodo almacena el apuntador a la unidad económica que alberga y 
    una etiqueta la cual indica si el nodo es la raíz de la cadena
    (unidad económica sobre la cual empieza a construirse), es un nodo
    aguas arriba o un nodo aguas abajo.
*/
using NodeInfo = pair<UE *, NodePosition>;

/** Alias para la información que almacena un arco del grafo.

    Un arco almacena un par de apuntadores a las sub unidades económicas que
    están relacionadas en compra-venta de un producto. El primer elemento del
    par es la sub unidad económica que funge como vendedora y el segundo 
    elemento es la que funge como compradora.
*/
using ArcInfo = pair<SubUE *, SubUE *>;

/** Alias para el tipo de grafo.

    En la vista que se debe generar, pueden existir varios arcos entre dos
    unidades económicas, por lo tanto, este modelo es un multi grafo.

    La diferencicia entre los arcos que conectan al mismo par de nodos
    se establece por el par de sub unidades económicas almacenadas en el arco.

    Al generar la vista, los arcos que conectan al mismo par de unidades
    económicas apuntan a las sub unidades que tienen la relación compra-venta.
*/
using Net = List_Graph<Graph_Node<NodeInfo>, Graph_Arc<ArcInfo>>;

/** Función que grafica una cadena productiva por unidad económica.

    Esta operación genera un gráfico vectorial en formato svg de la cadena
    productiva.

    @param net El grafo que representa la cadena productiva.
    @param output_name Nombre del archivo de salida.
    @param year Año de la producción.

    @author Alejandro J. Mujica
*/
void plot(const Net & net, const string & output_name, year_t year);

/* Función que construye toda la red aguas arriba.

   Dado un nodo del grafo, esta operación se encarga de buscar todas las
   unidades económicas que producen los insumos requeridos por todos los
   productos pertenecientes a la unidad económica albergada en el
   nodo dado, las agrega en el grafo y establece las conexiones mediante
   arcos.

   Esta función construye recursivamente por cada nodo añadido.

   @param net El grafo que representa la cadena productiva.
   @param root El nodo a partir del cual se hará la construcción aguas arriba.
   @param year Año de la producción.
   @param nodes_map Mapeo entre las unidades económicas insertadas y el nodo
   del grafo que la alberga.

   @author Alejandro J. Mujica
*/
void build_upstream(Net & net, Net::Node * root, year_t year,
		    TreeMap<UE *, Net::Node *> & nodes_map);

/* Función que construye toda la red aguas abajo.

   Dado un nodo del grafo, esta operación se encarga de buscar todas las
   unidades económicas a las cuales pertenecen los productos que utilizan
   como insumos a todos los productos pertenecientes a la unidad económica 
   albergada en el nodo dado, las agrega en el grafo y establece las conexiones
   mediante arcos.

   Esta función construye recursivamente por cada nodo añadido.

   @param net El grafo que representa la cadena productiva.
   @param root El nodo a partir del cual se hará la construcción aguas arriba.
   @param year Año de la producción.
   @param nodes_map Mapeo entre las unidades económicas insertadas y el nodo
   del grafo que la alberga.

   @author Alejandro J. Mujica
*/
void build_downstream(Net & net, Net::Node * root, year_t year,
		      TreeMap<UE*, Net::Node *> & nodes_map);

/** Función que construye una cadena productiva por unidad económica.

    Esta operación construye una red reresentada por un grafo el cual alberga
    unidades económicas en sus nodos. Un arco entre dos nodos indica que
    existen relaciones económicas entre una sub unidad económica de un nodo
    y una del otro.

    @param key Clave de búsqueda para la unidad económica a partir de la cual se
    va a construir la cadena. Ésta clave puede ser el RIF o el nombre.
    @param year Año de las producciones en la cadena.
    @param input_name nombre del archivo de datos que almacena la información
    de toda la red productiva.
    @param output_name Nombre del archivo svg en el cual se graficará la cadena.
    @param ue_key Valor constante que indica si el parámetro key es el RIF o
    el nombre de la unidad económica.

    @author Alejandro Mujica
*/
void generate_ue_chain(string key, year_t year, const string & input_name,
		       const string & output_name, UEKey ue_key);

/** Comparador de pares ordenados de sub unidades económicas para mantener el
    orden en un Árbol Binario de Búsqueda.

    @author Alejandro J. Mujica
*/
struct ArcInfoCmp
{
  bool operator () (const ArcInfo & p, const ArcInfo & q) const
  {
    if (p.first < q.first)
      return true;

    return not (q.first < p.first) and p.second < q.second;
  }
};

# endif // UEGEN_H
