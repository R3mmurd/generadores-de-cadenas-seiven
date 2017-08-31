/*
  Este archivo contiene la instrumentación de los algoritmos para construir
  cadenas por unidad económica.

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

# include <ue-gen.H>
# include <process.H>

void plot(const Net & net, const string & output_name, year_t year)
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
    
  TreeMap<UE *, size_t> ue_map;
  size_t ue_counter = 0;

  TreeMap<SubUE *, size_t> sub_ue_map;
  size_t sub_ue_counter = 0;
  
  for (Net::Node_Iterator it(net); it.has_curr(); it.next(), ++ue_counter)
    {
      Net::Node * p = it.get_curr();
      UE * ue = p->get_info().first;

      ue_map.insert(ue, ue_counter);

      string node_color;

      switch (p->get_info().second)
	{
	case NodePosition::ROOT: node_color = "seagreen"; break;
	case NodePosition::UPSTREAM: node_color = "orange"; break;
	case NodePosition::DOWNSTREAM: node_color = "cyan"; break;
	default: node_color = "white";
	}

      stringstream s_title;
      s_title << ue->name << "\\nRIF: " << ue->rif << "\\n\\n";

      output << "  subgraph cluster_" << ue_counter << endl
	     << "  {\n"
	     << "    style = filled;\n"
	     << "    fontcolor = white;\n"
	     << "    color = " << node_color << ";\n"
	     << "    label = \"" << s_title.str() << "\";\n";

      ue->sub_ues.for_each([&] (auto sub_ue) {

	  stringstream s_label;
	  s_label << sub_ue->name << "\\n" << sub_ue->location << "\\n"
		  << "Productos: " << sub_ue->products.size() << "\\n"
		  << "________________________\\n";


	  sub_ue->products.for_each([&] (auto product) {
	      
	      const Production & production = product->production(year);
	      
	      s_label << "\\nCod. Arancelario: " << product->tariffcode->cod
		      << "\\n" << product->name << "\\n"
		      << "Producción: " << production.quantity
		      << " " << production.meassurement_unit << "\\n";
	    });

	  output << "    " << sub_ue_counter << "[shape=box label=\""
		 << s_label.str() << "\" style = filled color = white];"
		 << endl;

	  sub_ue_map.insert(sub_ue, sub_ue_counter);
	  
	  ++sub_ue_counter;
	});

      output << "  }\n";
    }

  for (Net::Arc_Iterator it(net); it.has_curr(); it.next())
    {
      Net::Arc  * a = it.get_curr();
      Net::Node * s = net.get_src_node(a);
      Net::Node * t = net.get_tgt_node(a);
      SubUE * s_sub_ue = a->get_info().first;
      SubUE * t_sub_ue = a->get_info().second;
      UE * s_ue = s->get_info().first;
      UE * t_ue = t->get_info().first;
      assert(s_ue == s_sub_ue->ue);
      assert(t_ue == t_sub_ue->ue);
      output << "  "   << sub_ue_map[s_sub_ue]
	     << " -> " << sub_ue_map[t_sub_ue]
	     << "[lhead = cluster_" << ue_map[t_ue]
	     << " ltail = cluster_" << ue_map[s_ue] << "];";
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

bool exists_arc(Net::Node * s, Net::Node * t, ArcInfo info)
{
  for (Net::Node_Arc_Iterator it(s); it.has_curr(); it.next())
    if (it.get_tgt_node() == t)
      {
	Net::Arc * a = it.get_current();

	if (a->get_info() == info)
	  return true;
      }

  return false;
}

void build_upstream(Net & net, Net::Node * root, year_t year,
		    TreeMap<UE *, Net::Node *> & nodes_map)
{
  UE * ue = root->get_info().first;

  // Lista de productos que produce la UE
  auto products = ue->filter_products([] (auto) { return true; });
  
# ifdef DEBUG
  cout << "Building upstream\n"
       << "Processing ue: " << ue->name << endl
       << "There are " << products.size() << " products\n";
# endif

  TreeMap<UE *, TreeSet<ArcInfo, ArcInfoCmp>> map_ue_rel;

  products.for_each([&] (auto p) {

      p->inputs.for_each([&] (auto i) {
	  
	  assert(i->tariffcode != nullptr);
	  
	  const auto & purchases = i->production(year).purchases(year);
	  
	  purchases.for_each([&] (auto & purchase) {
	      
	      auto filter_products =
		purchase.provider->filter_products
		([&](auto fp) {
		  if (fp->tariffcode != i->tariffcode)
		    return false;

		  return fp->production(year).sales(year).
		  exists([&] (auto & sale) {
		      return sale.client == p->sub_ue->ue;
		    });
		});

	      filter_products.for_each([&](auto & fp) {
		  map_ue_rel[fp->sub_ue->ue].insert
		    (make_pair(fp->sub_ue, p->sub_ue));
		});
	    });
	});
    });

  map_ue_rel.for_each([&] (auto p) {

      UE * ue_ptr = p.first;
      auto rel    = p.second;

      bool recursion = false;
      Net::Node * s = nullptr;

      auto result = nodes_map.search(ue_ptr);

      if (result == nullptr)
	{
	  recursion = true;
	  s = net.insert_node(make_pair(ue_ptr, NodePosition::UPSTREAM));
	  nodes_map.insert(ue_ptr, s);
	}

      assert(s != nullptr);

      rel.for_each([&] (auto r) {

	  if (not exists_arc(s, root, r))
	    net.insert_arc(s, root, r);
	  
	});

      if (recursion)
	build_upstream(net, s, year, nodes_map);
    });
}

void build_downstream(Net & net, Net::Node * root, year_t year,
		      TreeMap<UE*, Net::Node *> & nodes_map)
{
  UE * ue = root->get_info().first;

  // Lista de productos que produce la UE
  auto products = ue->filter_products([] (auto) { return true; });

# ifdef DEBUG
  cout << "Building downstream\n"
       << "Processing ue: " << ue->name << endl
       << "There are " << products.size() << " products\n";
# endif

  TreeMap<UE *, TreeSet<ArcInfo, ArcInfoCmp>> map_ue_rel;

  products.for_each([&] (auto p) {

      auto & sales = p->production(year).sales(year);
      
      sales.for_each([&] (auto & sale) {

	  auto filter_inputs = sale.client->filter_inputs([&] (auto input) {
	      if (input->tariffcode == p->tariffcode)
		return false;

	      return input->production(year).purchases(year)
	      .exists([&] (auto & purchase) {
		  return purchase.provider == p->sub_ue->ue;
		});
	    });

	  filter_inputs.for_each([&] (auto input) {
	      map_ue_rel[input->product->sub_ue->ue].insert
		    (make_pair(p->sub_ue, input->product->sub_ue));
	    });
	});
      
    });

    map_ue_rel.for_each([&] (auto p) {

      UE * ue_ptr = p.first;
      auto rel    = p.second;

      bool recursion = false;
      Net::Node * t = nullptr;

      auto result = nodes_map.search(ue_ptr);

      if (result == nullptr)
	{
	  recursion = true;
	  t = net.insert_node(make_pair(ue_ptr, NodePosition::DOWNSTREAM));
	  nodes_map.insert(ue_ptr, t);
	}

      assert(t != nullptr);

      rel.for_each([&] (auto r) {

	  if (not exists_arc(root, t, r))
	    net.insert_arc(root, t, r);
	  
	});

      if (recursion)
	build_downstream(net, t, year, nodes_map);
    });
}

void generate_ue_chain(string key, year_t year, const string & input_name,
		       const string & output_name, UEKey ue_key)
{
# ifdef DEBUG
  cout << "Params:\n";
  cout << "key:              " << key << endl
       << "Year:             " << year << endl
       << "Input file name:  " << input_name << endl
       << "Output file name: " << output_name << endl
       << "ue key type:      " << (int) ue_key << endl;
# endif
  
  Map map;
  
  ifstream in(input_name);

  if (not in)
    {
      stringstream s;
      s << "Archivo " << input_name << " no existe";
      throw logic_error(s.str());
    }

  map.load(in);
  in.close();

  if (map.years.search(year) == nullptr)
    {
      stringstream s;
      s << "No existen registros para el año " << year;
      throw domain_error(s.str());
    }
  
  UE * ue = nullptr;

  if (ue_key == UEKey::RIF)
    {
      UE ue_k;
      ue_k.rif = key;
      ue = map.ues.search(ue_k);
    }
  else if (ue_key == UEKey::NAME)
    ue = map.ues.find_ptr([&] (const UE & item) {
	return item.name == key;
      });
  else
    throw domain_error("Tipo de clave de búsqueda no válido");

  if (ue == nullptr)
    {
      stringstream s;
      string key_type = ue_key == UEKey::RIF ? "RIF" : "nombre";
      s << "No existe registrada una unidad económica con " << key_type
	<< " " << key;
      throw domain_error(s.str());
    }
  
# ifdef DEBUG
  cout << "Found UE name: " << ue->name << endl;
# endif

  Net net;
  Net::Node * root = net.insert_node(make_pair(ue, NodePosition::ROOT));

  TreeMap<UE *, Net::Node *> nodes_map;
  nodes_map[ue] = root;

  build_upstream(net, root, year, nodes_map);
  build_downstream(net, root, year, nodes_map);

  plot(net, output_name, year);
}
