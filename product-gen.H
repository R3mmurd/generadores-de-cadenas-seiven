/*
  Este archivo contiene la definición de los algoritmos para construir cadenas
  por producto.

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

# ifndef PRODUCTGEN_H
# define PRODUCTGEN_H

# include <models.H>
# include <tpl_graph.H>

/** Alias para el tipo de información que almacena un nodo del grafo.

    En este caso un nodo almacena un apuntador al producto,
    la posición del nodo respecto a la raíz (raíz, aguas arriba o aguas
    abajo) y la distancia máxima (en niveles) entre el nodo y la raíz.
*/
using NodeInfo = tuple<Product *, NodePosition, unsigned short>;

/** Alias para la información que almacena un arco del grafo.

    Un arco almacena un par de números que representan el precio de compra o
    venta y la cantidad comprada o vendida respectivamente.
*/
using ArcInfo = pair<double, double>;

/** Alias para el tipo de grafo.

    En la vista que se debe generar, pueden existir varios arcos entre dos
    unidades económicas, por lo tanto, este modelo es un multi grafo.

    La diferencicia entre los arcos que conectan al mismo par de nodos
    se establece por el par de productos almacenados en el arco.

    Al generar la vista, los arcos que conectan al mismo par de unidades
    económicas apuntan a los productos que tienen relación insumo-producto.
*/
using Net = List_Graph<Graph_Node<NodeInfo>, Graph_Arc<ArcInfo>>;

/** Alias para agrupar nodos por sub unidades económicas y a la vez
    agrupar las sub unidades económicas por unidades económicas.
*/
using ClusterizedNodes = TreeMap<UE *, TreeMap<SubUE *, DynList<Net::Node *>>>;

/** Función que grafica una cadena productiva por unidad económica.

    Esta operación genera un gráfico vectorial en formato svg de la cadena
    productiva.

    @param net El grafo que representa la cadena productiva.
    @param nodes Nodos del grafo agrupados por sub unidades económicas y a su
    vez las sub unidades económicas agrupadas por unidades económicas.
    @param output_name Nombre del archivo de salida.
    @param year Año de la producción.

    @author Alejandro J. Mujica
*/
void plot(const Net & net, const ClusterizedNodes & nodes,
	  const string & output_name, year_t year);

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
   @param num_levels Cantidad máxima de niveles aguas arriba que tendrá la
   cadena.
   @param names_distance Máxima distancia de edición entre los nombres de un
   producto y un insumo.
   @param nodes Nodos del grafo agrupados por sub unidades económicas y a su
    vez las sub unidades económicas agrupadas por unidades económicas.
   @param nodes_map Mapeo entre los productos insertados y el nodo del grafo 
   que lo alberga.

   @author Alejandro J. Mujica
*/
void build_upstream(Net & net, Net::Node * root, year_t year,
		    unsigned short num_levels, unsigned short names_distance,
		    ClusterizedNodes & nodes,
		    TreeMap<Product *, Net::Node *> & nodes_map);

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
   @param num_levels Cantidad máxima de niveles aguas abajo que tendrá la
   cadena.
   @param names_distance Máxima distancia de edición entre los nombres de un
   producto y un insumo.
   @param nodes Nodos del grafo agrupados por sub unidades económicas y a su
    vez las sub unidades económicas agrupadas por unidades económicas.
   @param nodes_map Mapeo entre los productos insertados y el nodo del grafo 
   que lo alberga.

   @author Alejandro J. Mujica
*/
void build_downstream(Net & net, Net::Node * root, year_t year,
		      unsigned short num_levels, unsigned short names_distance,
		      ClusterizedNodes & nodes,
		      TreeMap<Product *, Net::Node *> & nodes_map);

/** Función que construye una cadena productiva por unidad económica.

    Esta operación construye una red reresentada por un grafo el cual alberga
    unidades económicas en sus nodos. Un arco entre dos nodos indica que
    existen relaciones insumo-producto entre un producto de una unidad económica
    y otro de la otra unidad económica.

    @param product_id Id del producto a partir del cual se construirá la cadena.
    @param year Año de las producciones en la cadena.
    @param num_levels_up Cantidad máxima de niveles aguas arriba que tendrá la
    cadena.
    @param num_levels_down Cantidad máxima de niveles aguas abajo que tendrá la
    cadena.
    @param input_name nombre del archivo de datos que almacena la información
    de toda la red productiva.
    @param output_name Nombre del archivo svg en el cual se graficará la cadena.
    @param names_distance Máxima distancia de edición entre los nombres de un
    producto y un insumo.

    @author Alejandro Mujica
*/
void generate_product_chain(db_id_t product_id, year_t year,
			    unsigned short num_levels_up,
			    unsigned short num_levels_down,
			    const string & input_name,
			    const string & output_name,
			    unsigned short names_distance);

/// Alias para construir el conjunto de relaciones con uno nodo.
using ProductRel = pair<Product *, ArcInfo>;

/** Comparador de tuplas de relaciones de productos.
    
    @author Alejandro J. Mujica
*/
struct ProductRelCmp
{
  using P = ProductRel;
  
  bool operator () (const P & p, const P & q) const
  {
    return p.first < q.first;
  }
};

# endif // PRODUCTGEN_H
