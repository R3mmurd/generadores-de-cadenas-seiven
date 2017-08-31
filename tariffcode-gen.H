/*
  Este archivo contiene la definición de los algoritmos para construir cadenas
  por código arancelario para las vistas de unidades económicas y productos.

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

# ifndef TARIFFCODEGEN_H
# define TARIFFCODEGEN_H

# include <process.H>
# include <models.H>
# include <tpl_graph.H>

/** Tipo enumerado para identificar el tipo de gráfico que se va a generar.

    @author Alejandro J. Mujica
 */
enum class ViewType { UE, PRODUCT };

/** Alias para la información que almacena un nodo del grafo.

    Un nodo almacena el apuntador al código arancelario que alberga y 
    una etiqueta la cual indica si el nodo es la raíz de la cadena
    (actividad económica sobre la cual empieza a construirse), es un nodo
    aguas arriba o un nodo aguas abajo.
*/
using NodeInfo = pair<TariffCode *, NodePosition>;

/// Alias para el tipo de grafo
using Net = List_Graph<Graph_Node<NodeInfo>>;

/** Función que grafica una cadena productiva por código arancelario.

    Esta operación genera un gráfico vectorial en formato svg de la cadena
    productiva. 

    @param net El grafo que representa la cadena productiva.
    @param output_name Nombre del archivo de salida.
    @param node_label Operación que se encarga de generar la etiqueta en el
    nodo de la cadena, ésta varía según si se visualiza la información de las
    unidades económicas o de los productos asociados al código arancelario 
    albergado por el nodo.

    @author Alejandro J. Mujica
*/
template <class NodeLabel>
void plot(const Net & net, const string & output_name, NodeLabel & node_label)
{
  ofstream output("temp.dot");

  if (not output)
    {
      stringstream s;
      s << "No se pudo crear el archivo " << output_name;
      throw runtime_error(s.str());
    }

  output << "digraph g" << endl << "{" << endl
	 << "  rankdir = LR;\n"
	 << "  compound = true;\n";

  TreeMap<Net::Node *, size_t> nodes_map;
  size_t i = 0;
  
  for (Net::Node_Iterator it(net); it.has_curr(); it.next(), ++i)
    {
      Net::Node * p = it.get_curr();

      TariffCode * tc = p->get_info().first;

      string node_color;

      switch (p->get_info().second)
	{
	case NodePosition::ROOT: node_color = "seagreen"; break;
	case NodePosition::UPSTREAM: node_color = "orange"; break;
	case NodePosition::DOWNSTREAM: node_color = "cyan"; break;
	default: node_color = "white";
	}

      stringstream s_title;
      s_title << tc->cod << "\\n" << tc->description << "\\n\\n";

      output << "  subgraph cluster_" << i << endl
	     << "  {\n"
	     << "    style = filled;\n"
	     << "    fontcolor = white;\n"
	     << "    color = " << node_color << ";\n"
	     << "    label = \"" << s_title.str() << "\";\n"
	     << "    " << i << "[shape=box label=\"" << node_label(tc)
	     << "\" style = filled color = white];" << endl
	     << "  }\n\n";
      
      nodes_map.insert(p, i);
    }

  output << endl;

  for (Net::Arc_Iterator it(net); it.has_curr(); it.next())
    {
      Net::Arc  * a     = it.get_curr();
      Net::Node * s     = net.get_src_node(a);
      Net::Node * t     = net.get_tgt_node(a);
      size_t      s_idx = nodes_map[s];
      size_t      t_idx = nodes_map[t];
      output << "  " << s_idx << " -> " << t_idx
	     << "[lhead = cluster_" << t_idx << " ltail = cluster_" << s_idx
	     << "];" << endl;
    }

  output << "}" << endl;

  output.close();

  char const * path = "dot";
  char * args[] = { "dot", "-Tsvg", "temp.dot", "-o",
		    (char *)output_name.c_str(), 0 };

  if (Process::exec(path, args) != 0)
    {
      stringstream s;
      s << "Error generating " << output_name;
      throw runtime_error(s.str());
    }
# ifdef DEBUG
  else
    cout << output_name << " generated successfully\n";
# endif

  remove("temp.dot");
}

/// Sinónimo de la anterior para r-values de la función de dibujado del nodo.
template <class NodeLabel>
void plot(const Net & net, const string & output_name,
	  NodeLabel && node_label = NodeLabel())
{
  plot<NodeLabel>(net, output_name, node_label);
}

/** Función que grafica una cadena productiva para la vista de unidades
    económicas.

    Esta operación genera un gráfico vectorial en formato svg de la cadena
    productiva. En este gráfico se muestra una cadena por código arancelario
    indicando información de las unidades económicas relacionadas a cada código.

    @param net El grafo que representa la cadena productiva.
    @param output_name Nombre del archivo de salida.

    @author Alejandro J. Mujica
*/
void plot_by_ue(const Net & net, const string & output_name);

/** Función que grafica una cadena productiva para la vista de productos.

    Esta operación genera un gráfico vectorial en formato svg de la cadena
    productiva. En este gráfico se muestra una cadena por código arancelario
    indicando información de los productos asociados a cada código.

    @param net El grafo que representa la cadena productiva.
    @param output_name Nombre del archivo de salida.

    @author Alejandro J. Mujica
*/
void plot_by_product(const Net & net, const string & output_name);

/* Función que construye toda la red aguas arriba.

   Dado un nodo del grafo, esta operación se encarga de buscar todas los
   códigos arancelarios a los cuales pertenecen los insumos requeridos por
   todos los productos pertenecientes al código arancelario albergado en el
   nodo dado, los agrega en el grafo y establece las conexiones mediante
   arcos.

   Esta función construye recursivamente por cada nodo añadido.

   @param net El grafo que representa la cadena productiva.
   @param root El nodo a partir del cual se hará la construcción aguas arriba.
   @param year Año de la producción.
   @param level Nivel de detalle del código arancelario.
   @param nodes_map Mapeo entre los códigos arancelarios insertadas y el nodo
   del grafo que la alberga.

   @author Alejandro J. Mujica
*/
void build_upstream(Net & net, Net::Node * root, year_t year,
		    TariffCodeLevel level,
		    TreeMap<TariffCode *, Net::Node *> & nodes_map);

/* Función que construye toda la red aguas abajo.

   Dado un nodo del grafo, esta operación se encarga de buscar todas los
   códigos arancelarios a los cuales pertenecen los productos que utilizan
   como insumos a todos los productos pertenecientes al código arancelario 
   albergado en el nodo dado, las agrega en el grafo y establece las conexiones
   mediante arcos.

   Esta función construye recursivamente por cada nodo añadido.

   @param net El grafo que representa la cadena productiva.
   @param root El nodo a partir del cual se hará la construcción aguas arriba.
   @param year Año de la producción.
   @param level Nivel de detalle del código arancelario.
   @param nodes_map Mapeo entre los códigos arancelarios insertadas y el nodo
   del grafo que la alberga.

   @author Alejandro J. Mujica
*/
void build_downstream(Net & net, Net::Node * root, year_t year,
		      TariffCodeLevel level,
		      TreeMap<TariffCode *, Net::Node *> & nodes_map);

/** Función que construye una cadena productiva por actividad económica.

    Esta operación construye una red reresentada por un grafo el cual alberga
    actividades económicas en sus nodos. Un arco entre dos nodos indica que
    existen relaciones económicas entre las actividades en los nodos.

    @param lvl Cadena que indica cual es el nivel de detalle para el código 
    arancelario.
    @param tariffcode Código arancelario a partir del cual se va a construir
    la cadena,
    @param year Año de las producciones en la cadena.
    @param input_name nombre del archivo de datos que almacena la información
    de toda la red productiva.
    @param output_name Nombre del archivo svg en el cual se graficará la cadena.
    @param view_type Constante numérica que identifica si el gráfico que se va
    a generar mostrará información de las unidades económicas o de los productos.

    @author Alejandro Mujica
*/
void generate_tariffcode_chain(const string & lvl, const string & tariffcode,
			       year_t year, const string & input_name,
			       const string & output_name, ViewType view_type);

# endif // TARIFFCODEGEN_H
