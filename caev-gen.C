# include <caev-gen.H>
# include <process.H>

void plot(const Net & net, const string & output_name)
{
  ofstream output("temp.dot");

  if (not output)
    {
      stringstream s;
      s << "No se pudo crear el archivo " << output_name;
      throw runtime_error(s.str());
    }

  output << "graph g" << endl << "{" << endl;

  TreeMap<Net::Node *, size_t> nodes_map;
  size_t i = 0;
  
  for (Net::Node_Iterator it(net); it.has_curr(); it.next(), ++i)
    {
      Net::Node * p = it.get_curr();

      CAEV * caev = p->get_info().first;

      stringstream s_label;
      s_label << caev->cod << "\\n" << caev->description << "\\n\\n";

      auto sub_ues = caev->get_sub_ues();

      s_label << "Sub-unidades: " << sub_ues.size() << "\\n";

      sub_ues.for_each([&](auto sub_ue_ptr) {

	  UE * ue_ptr = sub_ue_ptr->ue;

	  s_label << "\\n" << "RIF: " << ue_ptr->rif << "\\n";
	  s_label << ue_ptr->name << "\\n";
	  s_label << sub_ue_ptr->name << "   -   "
		  << sub_ue_ptr->location << "\\n";

	});

      output << "  " << i << "[shape=box label=\"" << s_label.str()
	     << "\"];" << endl;
      
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
      output << "  " << s_idx << " -> " << t_idx << ";" << endl;
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

bool exists_arc(Net::Node * s, Net::Node * t)
{
  for (Net::Node_Arc_Iterator it(s); it.has_curr(); it.next())
    if (it.get_tgt_node() == t)
      return true;

  return false;
}

void build_upstream(Net & net, Net::Node * r, year_t year, CAEVLevel level,
		    TreeMap<CAEV *, Net::Node *> & nodes_map)
{
  CAEV * caev = r->get_info().first;
  
  // Lista de productos pertenecientes al CAEV
  auto products = caev->get_products();

  TreeSet<CAEV *> caev_set;

# ifdef DEBUG
  cout << "Building upstream\n"
       << "Processing caev: " << caev->cod << endl
       << "There are " << products.size() << " products\n";
# endif

  products.for_each([&] (auto p) {

      // Miro cada insumo de p
      p->inputs.for_each([&] (auto i) {

	  assert(i->tariffcode != nullptr);
	  
	  // Obtengo las compras asociadas al insumo el año dado
	  const auto & purchases = i->production(year).purchases(year);
	  
	  purchases.for_each([&] (auto & purchase)  {
	      /* Filtro los productos del proveedor por código
		 arancelario. */
	      
	      auto filter_products =
		purchase.provider->filter_products
		([&](auto fp) {
		  return fp->tariffcode == i->tariffcode;
		});

	      filter_products.for_each([&](auto & fp) {
		  caev_set.insert(fp->get_caev(level));
		});
	    });
      	});
    });

   
  // Añadir nodos al grafo y conectarlos al raíz

  caev_set.for_each([&] (CAEV * c) {

      auto p = nodes_map.search(c);
      
      if (p == nullptr) 
	{
	  /* Si la actividad económica no ha sido añadida previamente al grafo,
	     entonces se inserta, se escribe en el mapa, se conecta al nodo raíz
	     y se recursiona sobre la nueva actividad añadida.
	  */
	  Net::Node * s = net.insert_node(make_pair(c, NodePosition::UPSTREAM));
	  net.insert_arc(s, r);
	  nodes_map.insert(c, s);
	  build_upstream(net, s, year, level, nodes_map);
	}
      else
	{
	  /* Si la actividad económica ha sido añadida previamente, entonces se
	     verifica que el nodo no sea el mismo nodo raíz, si no es el mismo
	     y no existe arco previo entre el nuevo nodo y el raíz, entonces
	     se conectan.
	  */
	  Net::Node * s = p->second;
	  
	  if (s != r and not exists_arc(s, r))
	    net.insert_arc(s, r);
	}
      
    });
}

void build_downstream(Net & net, Net::Node * r, year_t year, CAEVLevel level,
		      TreeMap<CAEV *, Net::Node *> & nodes_map)
{
  CAEV * caev = r->get_info().first;
  
  // Lista de productos pertenecientes al CAEV
  auto products = caev->get_products();

# ifdef DEBUG
  cout << "Building downstream\n"
       << "Processing caev: " << caev->cod << endl
       << "There are " << products.size() << " products\n";
# endif
  
  TreeSet<CAEV *> caev_set;

  products.for_each([&] (auto p) {
      
      // Para cada producto miro las ventas del año dado
      auto & sales = p->production(year).sales(year);
      
      sales.for_each([&] (auto & sale) {
	  /* Obtengo los insumos que obtiene el cliente, tal que tengan el 
	     mismo código arancelario del producto */      
	  auto filter_inputs = sale.client->filter_inputs([&] (auto input) {
	      return input->tariffcode == p->tariffcode;
	    });

	  filter_inputs.for_each([&] (auto input) {
	      caev_set.insert(input->product->get_caev(level));
	    });
	});
    });

  caev_set.for_each([&] (CAEV * c) {

      auto p = nodes_map.search(c);
      
      if (p == nullptr) 
	{
	  Net::Node * t =
	    net.insert_node(make_pair(c, NodePosition::DOWNSTREAM));
	  net.insert_arc(r, t);
	  nodes_map.insert(c, t);
	  build_downstream(net, t, year, level, nodes_map);
	}
      else
	{
	  Net::Node * t = p->second;
	  
	  if (t != r and not exists_arc(r, t))
	    net.insert_arc(r, t);
	}
      
    });
}

void generate_caev_chain(const string & lvl, const string & caev_cod,
			 year_t year, const string & input_name,
			 const string & output_name)
{
# ifdef DEBUG
  cout << "Params:\n";
  cout << "CAEV level:       " << lvl << endl
       << "CAEV code:        " << caev_cod << endl
       << "Year:             " << year << endl
       << "Input file name:  " << input_name << endl
       << "Output file name: " << output_name << endl;
# endif
  
  CAEVLevel level = str_to_level(lvl);

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
  
  CAEV * caev = search_caev(caev_cod, level, map);

  if (caev == nullptr)
    {
      stringstream s;
      s << "El código de actividad económica " << caev_cod
	<< " no existe en el nivel " << level_to_str(level);
      throw domain_error(s.str());
    }

# ifdef DEBUG
  cout << "Found CAEV description: " << caev->description << endl;
# endif

  Net net;
  Net::Node * root = net.insert_node(make_pair(caev, NodePosition::ROOT));

  TreeMap<CAEV *, Net::Node *> nodes_map;
  nodes_map[caev] = root;

  build_upstream(net, root, year, level, nodes_map);
  build_downstream(net, root, year, level, nodes_map);

  plot(net, output_name);
}

