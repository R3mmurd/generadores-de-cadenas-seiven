/*
  Este arcghivo contiene el programa que carga la base de datos necesaria y la
  almacena en texto plano.
  Copyright (C) 2017 Alejandro J. Mujica
  
  Este programa es software libre: puede redistribuirlo y/o modificarlo bajo
  los términos de la Licencia General Pública de GNU publicada por la Free
  Software Foundation, ya sea la versión 3 de la Licencia, o (a su elección)
  cualquier versión posterior.
  
  Este programa se distribuye con la esperanza de que sea útil pero SIN
  NINGUNA GARANTÍA; incluso sin la garantía implícita de MERCANTIBILIDAD o
  CALIFICADA PARA UN PROPÓSITO EN PARTICULAR. Vea la Licencia General Pública
  de GNU para más detalles.
  
  Usted ha debido de recibir una copia de la Licencia General Pública
  de GNU junto con este programa. Si no, vea <http://www.gnu.org/licenses/>.

  Cualquier solicitud de parte de los usuarios de este software, escribir a

  Alejandro Mujica

  aledrums@gmail.com
*/

# include <models.H>

# include <autoConnection.H>
# include <dbQuery.H>
# include <strQuery.H>
# include <dbProperties.H>

# include <tclap/CmdLine.h>

using namespace TCLAP;

struct DftConfValues
{
  static const string CONF_NAME;
  static const string HOST;
  static const string PORT;
  static const string USER;
  static const string DBNAME;
  static const string OUTPUT_NAME;
};

const string DftConfValues::CONF_NAME   = "dbloader.cfg";
const string DftConfValues::HOST        = "127.0.0.1";
const string DftConfValues::PORT        = "5432";
const string DftConfValues::USER        = "postgres";
const string DftConfValues::DBNAME      = "sidepro";
const string DftConfValues::OUTPUT_NAME = "sidepro-map.txt";

class Configuration
{
  string output_name = "";
  bool   verbose     = false;
  
  static std::unique_ptr<Configuration> instance;

  Configuration()
  {
    // empty
  }

  Configuration(const Configuration &) = delete;

  Configuration & operator = (const Configuration &) = delete;

public:
  static Configuration & get_instance()
  {
    if (instance.get() == nullptr)
      instance = std::unique_ptr<Configuration> (new Configuration);

    return *instance;
  }

  const string & get_output_name() const
  {
    return output_name;
  }

  void set_output_name(const string & s)
  {
    output_name = s;
  }

  bool is_verbose() const
  {
    return verbose;
  }

  void set_verbose(bool v)
  {
    verbose = v;
  }

  bool restore_conf()
  {
    ifstream input(DftConfValues::CONF_NAME);

    DBProperties & db_prop = DBProperties::getInstance();

    if (not input)
      return false;

    string s;
    getline(input, s);
    db_prop.setHost(s);
    getline(input, s);
    db_prop.setPort(s);
    getline(input, s);
    db_prop.setUser(s);
    getline(input, s);
    db_prop.setDbname(s);
    getline(input, output_name);
    
    input.close();
    return true;
  }

  bool save_conf() const
  {
    ofstream output(DftConfValues::CONF_NAME);

    if (not output)
      return false;

    DBProperties & db_prop = DBProperties::getInstance();
    output << db_prop.getHost() << endl
	   << db_prop.getPort() << endl
	   << db_prop.getUser() << endl
	   << db_prop.getDbname() << endl
	   << output_name << endl;
    
    output.close();
    return true;
  }
};

std::unique_ptr<Configuration> Configuration::instance(nullptr);

void process_cmd_line(int argc, char * argv[])
{
  CmdLine cmd("DB Loader", ' ', "1.0");

  ValueArg<string> host("H", "host", "Host del servidor de base de datos", false,
			DftConfValues::HOST, "HOSTNAME");

  cmd.add(host);

  ValueArg<string> port("P", "port", "Puerto del servidor de base de datos",
			false, DftConfValues::PORT, "PORT");

  cmd.add(port);

  ValueArg<string> user("U", "username", "Usuario de la base de datos", false,
			DftConfValues::USER, "USERNAME");

  cmd.add(user);
  
  ValueArg<string> db("D", "dbname", "Nombre de la base de datos", false,
		      DftConfValues::DBNAME, "DBNAME");

  cmd.add(db);
  
  ValueArg<string> output("o", "output", "", false, DftConfValues::OUTPUT_NAME,
			  "OUTPUTNAME");
  
  cmd.add(output);

  ValueArg<string> password("W", "password",
			    "Contraseña de ususario de la base de datos",
			    false, "", "PASSWORD");
  
  cmd.add(password);


  SwitchArg no_password("w", "no-password", "no pedir contraseña");

  cmd.add(no_password);

  SwitchArg verbose("v", "verbose", "verbose mode");

  cmd.add(verbose);

  cmd.parse(argc, argv);

  Configuration & conf = Configuration::get_instance();
  DBProperties & db_prop = db_prop.getInstance();

  bool restored = conf.restore_conf();

  conf.set_verbose(verbose.getValue());

  if (not restored or host.getValue() != DftConfValues::HOST)
    db_prop.setHost(host.getValue());

  if (not restored or port.getValue() != DftConfValues::PORT)
    db_prop.setPort(port.getValue());

  if (not restored or user.getValue() != DftConfValues::USER)
    db_prop.setUser(user.getValue());

  if (not restored or db.getValue() != DftConfValues::DBNAME)
    db_prop.setDbname(db.getValue());

  if (not restored or output.getValue() != DftConfValues::OUTPUT_NAME)
    conf.set_output_name(output.getValue());

  if (not password.getValue().empty())
    db_prop.setPassword(password.getValue());
  else if (not no_password.getValue())
    {
      cout << "Password for " << db_prop.getUser() << ": ";
      string pw;
      cin >> pw;
      db_prop.setPassword(pw);
    }

  conf.save_conf();
}

void print(const string & msg)
{
  if (Configuration::get_instance().is_verbose())
    cout << msg << endl;
}

void exec_query(DBQuery & q, StrQuery & sq)
{
  if (q.exec(sq))
    return;

  stringstream s;
  s << "Cannot execute query " << sq;
  throw std::logic_error(s.str());
}

TreeMap<string, CAEV *> load_caev(Map & map, AutoConnection & conn)
{
  auto & sections  = map.caev_sections;
  auto & divisions = map.caev_divisions;
  auto & groups    = map.caev_groups;
  auto & classes   = map.caev_classes;
  auto & branches  = map.caev_branches;

  TreeMap<string, CAEV *> caev_map_w;
  TreeMap<string, CAEV *> caev_map_r;
  
  DBQuery query(conn);

  StrQuery sq;
  sq.addSelect("*");
  sq.addFrom("base_caevseccion");
  
# ifdef DEBUG
  cout << "Executing query:\n" << sq << endl;
# endif
  
  exec_query(query, sq);

  print("Loading CAEV sections...");

  while (query.next())
    {
      CAEVSection caev;
      caev.cod = query.getValue("seccion");
      caev.description = query.getValue("descripcion");
      CAEVSection * result_ptr = sections.insert(caev);
      assert(result_ptr != nullptr);
      caev_map_w.insert(caev.cod, result_ptr);
    }

  print("CAEV sections done!");

  caev_map_r.swap(caev_map_w);
  caev_map_w.empty();
  sq.clear();
  query.clear();

  sq.addSelect("*");
  sq.addFrom("base_caevdivision");
  
# ifdef DEBUG
  cout << "Executing query:\n" << sq << endl;
# endif
  
  exec_query(query, sq);
  
  assert(not caev_map_r.is_empty());
  assert(caev_map_w.is_empty());

  print("Loading CAEV divisions...");
  
  while (query.next())
    {
      CAEVDivision caev;
      caev.cod = query.getValue("division");
      caev.description = query.getValue("descripcion");
      CAEVSection * section_ptr =
	static_cast<CAEVSection *>(caev_map_r[query.getValue("seccion_id")]);
      assert(section_ptr != nullptr);
      caev.section = section_ptr;	
      CAEVDivision * result_ptr = divisions.insert(caev);
      section_ptr->divisions.append(result_ptr);
      caev_map_w.insert(caev.cod, result_ptr);
    }

  print("CAEV divisions done!");

  caev_map_r.swap(caev_map_w);

  caev_map_w.empty();
  sq.clear();
  query.clear();

  sq.addSelect("*");
  sq.addFrom("base_caevgrupo");

# ifdef DEBUG
  cout << "Executing query:\n" << sq << endl;
# endif

  exec_query(query, sq);;

  assert(not caev_map_r.is_empty());
  assert(caev_map_w.is_empty());

  print("Loading CAEV groups...");

  while (query.next())
    {
      CAEVGroup caev;
      caev.cod = query.getValue("grupo");
      caev.description = query.getValue("descripcion");
      CAEVDivision * division_ptr =
	static_cast<CAEVDivision *>(caev_map_r[query.getValue("division_id")]);
      assert(division_ptr != nullptr);
      caev.division = division_ptr;
      CAEVGroup * result_ptr = groups.insert(caev);
      division_ptr->groups.append(result_ptr);
      caev_map_w.insert(caev.cod, result_ptr);
    }

  print("CAEV groups done!");

  caev_map_r.swap(caev_map_w);

  caev_map_w.empty();
  sq.clear();
  query.clear();

  sq.addSelect("*");
  sq.addFrom("base_caevclase");

# ifdef DEBUG
  cout << "Executing query:\n" << sq << endl;
# endif

  exec_query(query, sq);;

  assert(not caev_map_r.is_empty());
  assert(caev_map_w.is_empty());

  print("Loading CAEV classes...");

  while (query.next())
    {
      CAEVClass caev;
      caev.cod = query.getValue("clase");
      caev.description = query.getValue("descripcion");
      CAEVGroup * group_ptr =
	static_cast<CAEVGroup *>(caev_map_r[query.getValue("grupo_id")]);
      assert(group_ptr != nullptr);
      caev.group = group_ptr;
      CAEVClass * result_ptr = classes.insert(caev);
      group_ptr->classes.append(result_ptr);
      caev_map_w.insert(caev.cod, result_ptr);
    }

  print("CAEV classes done!");

  caev_map_r.swap(caev_map_w);

  caev_map_w.empty();
  sq.clear();
  query.clear();

  sq.addSelect("*");
  sq.addFrom("base_caevrama");
  
# ifdef DEBUG
  cout << "Executing query:\n" << sq << endl;
# endif

  exec_query(query, sq);;

  assert(not caev_map_r.is_empty());
  assert(caev_map_w.is_empty());

  print("Loading CAEV branches...");

  while (query.next())
    {
      CAEVBranch caev;
      caev.cod = query.getValue("rama");
      caev.description = query.getValue("descripcion");
      CAEVClass * class_ptr =
	static_cast<CAEVClass *>(caev_map_r[query.getValue("clase_id")]);
      assert(class_ptr != nullptr);
      caev.clazz = class_ptr;
      CAEVBranch * result_ptr = branches.insert(caev);
      class_ptr->branches.append(result_ptr);
      caev_map_w.insert(caev.cod, result_ptr);
    }

  print("CAEV branches done!");

  assert(not caev_map_w.is_empty());

  return caev_map_w;
}

TreeMap<string, TariffCode *> load_tariffcodes(Map & map, AutoConnection & conn)
{
  auto & sections    = map.tariffcode_sections;
  auto & chapters    = map.tariffcode_chapters;
  auto & items       = map.tariffcode_items;
  auto & subitems    = map.tariffcode_subitems;
  auto & subsubitems = map.tariffcode_subsubitems;

  TreeMap<string, TariffCode *> tc_map_w;
  TreeMap<string, TariffCode *> tc_map_r;
  
  DBQuery query(conn);

  StrQuery sq;
  sq.addSelect("*");
  sq.addFrom("base_codaranseccion");
  
# ifdef DEBUG
  cout << "Executing query:\n" << sq << endl;
# endif

  exec_query(query, sq);

  print("Loading tariff code sections...");

  while (query.next())
    {
      TariffCodeSection tariffcode;
      tariffcode.cod = query.getValue("codigo");
      tariffcode.description = query.getValue("descripcion");
      TariffCodeSection * result_ptr = sections.insert(tariffcode);
      assert(result_ptr != nullptr);
      tc_map_w.insert(tariffcode.cod, result_ptr);
    }

  print("Tariff code sections done!");

  tc_map_w.swap(tc_map_r);
  
  tc_map_w.empty();
  sq.clear();
  query.clear();

  sq.addSelect("*");
  sq.addFrom("base_codarancapitulo");
  
# ifdef DEBUG
  cout << "Executing query:\n" << sq << endl;
# endif

  exec_query(query, sq);

  assert(tc_map_w.is_empty());
  assert(not tc_map_r.is_empty());

  print("Loading tariff code chapters...");

  while (query.next())
    {
      TariffCodeChapter tariffcode;
      tariffcode.cod = query.getValue("codigo");
      tariffcode.description = query.getValue("descripcion");
      TariffCodeSection * section_ptr =
	static_cast<TariffCodeSection *>(tc_map_r[query.getValue("seccion_id")]);
      assert(section_ptr != nullptr);
      tariffcode.section = section_ptr;
      TariffCodeChapter * result_ptr = chapters.insert(tariffcode);
      assert(result_ptr != nullptr);
      section_ptr->chapters.append(result_ptr);
      tc_map_w.insert(tariffcode.cod, result_ptr);
    }

  print("Tariff code chapters done!");

  tc_map_w.swap(tc_map_r);
  
  tc_map_w.empty();
  sq.clear();
  query.clear();

  sq.addSelect("*");
  sq.addFrom("base_codaranpartida");
  
# ifdef DEBUG
  cout << "Executing query:\n" << sq << endl;
# endif

  exec_query(query, sq);

  assert(tc_map_w.is_empty());
  assert(not tc_map_r.is_empty());

  print("Loading tariff code items...");

  while (query.next())
    {
      TariffCodeItem tariffcode;
      tariffcode.cod = query.getValue("codigo");
      tariffcode.description = query.getValue("descripcion");
      TariffCodeChapter * chapter_ptr =
	static_cast<TariffCodeChapter *>
	(tc_map_r[query.getValue("capitulo_id")]);
      assert(chapter_ptr != nullptr);
      tariffcode.chapter = chapter_ptr;
      TariffCodeItem * result_ptr = items.insert(tariffcode);
      assert(result_ptr != nullptr);
      chapter_ptr->items.append(result_ptr);
      tc_map_w.insert(tariffcode.cod, result_ptr);
    }

  print("Tariff code items done!");

  tc_map_w.swap(tc_map_r);
  
  tc_map_w.empty();
  sq.clear();
  query.clear();

  sq.addSelect("*");
  sq.addFrom("base_codaransubpartida");
  
# ifdef DEBUG
  cout << "Executing query:\n" << sq << endl;
# endif

  exec_query(query, sq);

  assert(tc_map_w.is_empty());
  assert(not tc_map_r.is_empty());

  print("Loading tariff code subitems...");

  while (query.next())
    {
      TariffCodeSubItem tariffcode;
      tariffcode.cod = query.getValue("codigo");
      tariffcode.description = query.getValue("descripcion");
      TariffCodeItem * item_ptr =
	static_cast<TariffCodeItem *>(tc_map_r[query.getValue("partida_id")]);
      assert(item_ptr != nullptr);
      tariffcode.item = item_ptr;
      TariffCodeSubItem * result_ptr = subitems.insert(tariffcode);
      assert(result_ptr != nullptr);
      item_ptr->subitems.append(result_ptr);
      tc_map_w.insert(tariffcode.cod, result_ptr);
    }

  print("Tariff code subitems done!");

  tc_map_w.swap(tc_map_r);
  
  tc_map_w.empty();
  sq.clear();
  query.clear();

  sq.addSelect("*");
  sq.addFrom("base_codaransubsubpartida");
  
# ifdef DEBUG
  cout << "Executing query:\n" << sq << endl;
# endif

  exec_query(query, sq);

  assert(tc_map_w.is_empty());
  assert(not tc_map_r.is_empty());

  print("Loading tariff code subsubitems...");

  while (query.next())
    {
      TariffCodeSubSubItem tariffcode;
      tariffcode.cod = query.getValue("codigo");
      tariffcode.description = query.getValue("descripcion");
      TariffCodeSubItem * subitem_ptr =
	static_cast<TariffCodeSubItem *>
	(tc_map_r[query.getValue("subpartida_id")]);
      assert(subitem_ptr != nullptr);
      tariffcode.subitem = subitem_ptr;
      TariffCodeSubSubItem * result_ptr = subsubitems.insert(tariffcode);
      assert(result_ptr != nullptr);
      subitem_ptr->subsubitems.append(result_ptr);
      tc_map_w.insert(tariffcode.cod, result_ptr);
    }

  print("Tariff code subsubitems done!");

  return tc_map_w;
}

std::pair<TreeMap<db_id_t, UE *>, TreeMap<db_id_t, SubUE *>>
	      load_ue(Map & map, AutoConnection & conn,
		      TreeMap<string, CAEV *> & caev_map)
{
  TreeMap<db_id_t, UE *>    ue_map;
  TreeMap<db_id_t, SubUE *> sub_ue_map;

  DBQuery query(conn);

  StrQuery sq;

  sq.addSelect("id");
  sq.addSelect("rif");
  sq.addSelect("nombre_ue");
  sq.addFrom("unidad_economica_unidadeconomica");
  
# ifdef DEBUG
  cout << "Executing query:\n" << sq << endl;
# endif

  exec_query(query, sq);

  print("Loading ue...");

  while (query.next())
    {
      UE ue;
      ue.rif = query.getValue("rif");
      ue.name = query.getValue("nombre_ue");

      UE * ptr = map.ues.insert(ue);
      assert(ptr != nullptr);
      ue_map[stoull(query.getValue("id"))] = ptr;
    }

  print("Ue done!");
  
  query.clear();
  sq.clear();

  sq.addSelect("sub_unidad_economica_subunidadeconomica.id AS sub_id");
  sq.addSelect("sub_unidad_economica_subunidadeconomica.nombre_sub");
  sq.addSelect("sub_unidad_economica_subunidadeconomica.unidad_economica_id");
  sq.addSelect("base_estado.nombre AS estado");
  sq.addFrom("sub_unidad_economica_subunidadeconomica");

  string on = "";
  on.append("sub_unidad_economica_subunidadeconomicadirectorio.");
  on.append("sub_unidad_economica_id = ");
  on.append("sub_unidad_economica_subunidadeconomica.id");
  sq.addJoin("sub_unidad_economica_subunidadeconomicadirectorio", on);

  on = "";
  on.append("sub_unidad_economica_subunidadeconomicadirectorio.");
  on.append("directorio_id = ");
  on.append("directorio_directorio.id");
  sq.addJoin("directorio_directorio", on);

  on = "";
  on.append("directorio_directorio.parroquia_id = ");
  on.append("base_parroquia.id");
  sq.addJoin("base_parroquia", on);

  on = "";
  on.append("base_parroquia.municipio_id = ");
  on.append("base_municipio.id");
  sq.addJoin("base_municipio", on);

  on = "";
  on.append("base_municipio.estado_id = ");
  on.append("base_estado.id");
  sq.addJoin("base_estado", on);
  
# ifdef DEBUG
  cout << "Executing query:\n" << sq << endl;
# endif

  exec_query(query, sq);

  print("Loading sub ue...");
  
  while (query.next())
    {
      UE * ue_ptr = ue_map[stoull(query.getValue("unidad_economica_id"))];
      assert(ue_ptr != nullptr);

      SubUE sub_ue;
      sub_ue.db_id = stoull(query.getValue("sub_id"));
      sub_ue.name = query.getValue("nombre_sub");
      sub_ue.location = query.getValue("estado");
      sub_ue.ue = ue_ptr;
      assert(sub_ue.caev == nullptr);

      SubUE * sub_ue_ptr = map.sub_ues.insert(sub_ue);
      assert(sub_ue_ptr != nullptr);
      ue_ptr->sub_ues.append(sub_ue_ptr);
      sub_ue_map[sub_ue_ptr->db_id] = sub_ue_ptr;
    }

  print("Sub ue done!");
  
  query.clear();
  sq.clear();

  sq.addSelect("caev_id").addSelect("sub_unidad_economica_id");
  sq.addFrom("sub_unidad_economica_subunidadeconomicaactividad");
  sq.addWhere("primaria = TRUE");
  
# ifdef DEBUG
  cout << "Executing query:\n" << sq << endl;
# endif

  exec_query(query, sq);

  print("Loading sub ue CAEV...");

  while(query.next())
    {
      SubUE * sub_ue_ptr =
	sub_ue_map[stoull(query.getValue("sub_unidad_economica_id"))];
      assert(sub_ue_ptr != nullptr);
      
      CAEVBranch * caev_ptr = static_cast<CAEVBranch *>
	(caev_map[query.getValue("caev_id")]);
      assert(caev_ptr != nullptr);

      sub_ue_ptr->caev = caev_ptr;
      caev_ptr->sub_ues.append(sub_ue_ptr);
    }

  print("Sub ue CAEV done!");
  
  return make_pair(ue_map, sub_ue_map);
}

void load_products(Map & map, AutoConnection & conn,
		   TreeMap<db_id_t, UE *> & ue_map,
		   TreeMap<db_id_t, SubUE *> & sub_ue_map,
		   TreeMap<string, TariffCode *> & tariffcode_map)
{
  DBQuery query(conn);
  StrQuery sq;

  sq.addSelect("id");
  sq.addSelect("nombre_producto");
  sq.addSelect("subunidad_id");
  sq.addSelect("codaran_subsubpartida_id");
  sq.addFrom("bienes_prod_comer_producto");
  
# ifdef DEBUG
  cout << "Executing query:\n" << sq << endl;
# endif

  exec_query(query, sq);

  TreeMap<db_id_t, Product *> product_map;

  print("Loading products...");

  while (query.next())
    {
      Product p;
      p.db_id = stoull(query.getValue("id"));
      p.name = query.getValue("nombre_producto");
      p.sub_ue = sub_ue_map[stoull(query.getValue("subunidad_id"))];
      assert(p.sub_ue != nullptr);
      p.tariffcode = static_cast<TariffCodeSubSubItem *>
	(tariffcode_map[query.getValue("codaran_subsubpartida_id")]);
      assert(p.tariffcode != nullptr);

      Product * ptr = map.products.insert(p);
      assert(ptr != nullptr);
      assert(ptr->sub_ue != nullptr);
      ptr->sub_ue->products.append(ptr);
      ptr->tariffcode->products.append(ptr);
      product_map[p.db_id] = ptr;
    }
  
  print("Products done!");
  
  query.clear();
  sq.clear();
  
  sq.addSelect("id");
  sq.addSelect("nombre_insumo");
  sq.addSelect("producto_id");
  sq.addSelect("codaran_subsubpartida_id");
  sq.addFrom("insumo_proveedor_insumo");
  
# ifdef DEBUG
  cout << "Executing query:\n" << sq << endl;
# endif

  exec_query(query, sq);

  TreeMap<db_id_t, Input *> input_map;

  print("Loading inputs...");

  while (query.next())
    {
      Input i;
      i.db_id = stoull(query.getValue("id"));
      i.name = query.getValue("nombre_insumo");
      i.product = product_map[stoull(query.getValue("producto_id"))];
      i.tariffcode = static_cast<TariffCodeSubSubItem *>
	(tariffcode_map[query.getValue("codaran_subsubpartida_id")]);

      Input * ptr = map.inputs.insert(i);
      ptr->product->inputs.append(ptr);
      input_map[ptr->db_id] = ptr;
    }

  print("Inputs done!");
  
  query.clear();
  sq.clear();
  
  sq.addSelect("bienes_prod_comer_produccion.id");
  sq.addSelect("bienes_prod_comer_produccion.producto_id");
  sq.addSelect("bienes_prod_comer_produccion.cantidad_produccion");
  sq.addSelect("bienes_prod_comer_produccion.unidad_de_medida");
  sq.addSelect("base_anhoregistro.anho");
  sq.addFrom("bienes_prod_comer_produccion");
  sq.addJoin("base_anhoregistro",
	     "bienes_prod_comer_produccion.anho_registro_id=base_anhoregistro.id");
  
# ifdef DEBUG
  cout << "Executing query:\n" << sq << endl;
# endif

  exec_query(query, sq);

  TreeMap<db_id_t, Production *> production_map;

  print("Loading production...");

  while (query.next())
    {
      Product * product_ptr = product_map[stoull(query.getValue("producto_id"))];
      unsigned short year = stoi(query.getValue("anho"));
      Production & p = product_ptr->production(year);
      p.product = product_ptr;
      p.quantity = stod(query.getValue("cantidad_produccion"));
      p.meassurement_unit = query.getValue("unidad_de_medida");
      production_map[stoull(query.getValue("id"))] = &p;
    }

  print("Production done!");

  query.clear();
  sq.clear();
  
  sq.addSelect("bienes_prod_comer_facturacioncliente.id");
  sq.addSelect("bienes_prod_comer_facturacioncliente.produccion_id");
  sq.addSelect("bienes_prod_comer_facturacioncliente.cantidad_vendida");
  sq.addSelect("bienes_prod_comer_facturacioncliente.unidad_de_medida");
  sq.addSelect("bienes_prod_comer_facturacioncliente.cliente_id");
  sq.addSelect("base_anhoregistro.anho");
  sq.addSelect("precio_venta_usd");
  sq.addFrom("bienes_prod_comer_facturacioncliente");
  string on =
    "bienes_prod_comer_facturacioncliente.anho_registro_id=base_anhoregistro.id";
  sq.addJoin("base_anhoregistro", on);
  
# ifdef DEBUG
  cout << "Executing query:\n" << sq << endl;
# endif

  exec_query(query, sq);

  print("Loading sales...");

  while (query.next())
    {
      Production * p_ptr =
	production_map[stoull(query.getValue("produccion_id"))];
      
      Sale s;
      s.client = ue_map[stoull(query.getValue("cliente_id"))];
      s.quantity = stod(query.getValue("cantidad_vendida"));
      s.meassurement_unit = query.getValue("unidad_de_medida");
      s.price = stod(query.getValue("precio_venta_usd"));

      unsigned short year = stoi(query.getValue("anho"));

      p_ptr->sales(year).append(s);
    }

  print("Sales done!");

  query.clear();
  sq.clear();

  sq.addSelect("insumo_proveedor_insumoproduccion.id");
  sq.addSelect("insumo_proveedor_insumoproduccion.insumo_id");
  sq.addSelect("base_anhoregistro.anho");
  sq.addFrom("insumo_proveedor_insumoproduccion");
  on = "insumo_proveedor_insumoproduccion.anho_registro_id=base_anhoregistro.id"; 
  sq.addJoin("base_anhoregistro", on);
  
# ifdef DEBUG
  cout << "Executing query:\n" << sq << endl;
# endif

  exec_query(query, sq);

  TreeMap<db_id_t, InputProduction *> input_production_map;

  print("Loading input production...");

  while (query.next())
    {
      Input * input_ptr = input_map[stoull(query.getValue("insumo_id"))];
      unsigned short year = stoi(query.getValue("anho"));
      InputProduction & ip = input_ptr->production(year);
      ip.input = input_ptr;
      input_production_map[stoull(query.getValue("id"))] = &ip;
    }
  print("Input production done!");

  query.clear();
  sq.clear();
  
  sq.addSelect("insumo_proveedor_insumoproveedor.id");
  sq.addSelect("insumo_proveedor_insumoproveedor.insumo_produccion_id");
  sq.addSelect("insumo_proveedor_insumoproveedor.proveedor_id");
  sq.addSelect("insumo_proveedor_insumoproveedor.cantidad_comprada");
  sq.addSelect("insumo_proveedor_insumoproveedor.unidad_de_medida");
  sq.addSelect("insumo_proveedor_insumoproveedor.precio_compra_usd");
  sq.addSelect("base_anhoregistro.anho");
  sq.addFrom("insumo_proveedor_insumoproveedor");
  on = "insumo_proveedor_insumoproveedor.anho_registro_id=base_anhoregistro.id";
  sq.addJoin("base_anhoregistro", on);
  
# ifdef DEBUG
  cout << "Executing query:\n" << sq << endl;
# endif

  exec_query(query, sq);

  print("Loading purchases...");

  while (query.next())
    {
      InputProduction * ip_ptr =
	input_production_map[stoull(query.getValue("insumo_produccion_id"))];
      
      Purchase p;
      p.provider = ue_map[stoull(query.getValue("proveedor_id"))];
      p.quantity = stod(query.getValue("cantidad_comprada"));
      p.meassurement_unit = query.getValue("unidad_de_medida");
      p.price = stod(query.getValue("precio_compra_usd"));

      unsigned short year = stoi(query.getValue("anho"));
      
      ip_ptr->purchases(year).append(p);
    }

  print("Purchases done!");
}

int main(int argc, char * argv[])
{
  try
    {
      process_cmd_line(argc, argv);
      
      Map map;
      
      AutoConnection conn;
      
      print("Loading CAEV...");  
      TreeMap<string, CAEV *> caev_branches_map = load_caev(map, conn);
      print("CAEV done!");
      
      print("Loading tariff codes...");
      TreeMap<string, TariffCode *> tc_subsubitems_map =
	load_tariffcodes(map, conn);
      print("Tariff codes done!");
      
      print("Loading ue and sub ue...");
      auto p = load_ue(map, conn, caev_branches_map);
      print("Ue and sub ue done!");
      
      print("Loading products and inputs...");
      load_products(map, conn, p.first, p.second, tc_subsubitems_map);
      print("Products and inputs done!");
      
      ofstream output(Configuration::get_instance().get_output_name());
      map.save(output);
      output.close();
    }
  catch (const std::exception & e)
    {
      cout << "An excepion was caught with this message: "
	   << e.what() << endl;
      return EXIT_FAILURE;
    }
      
  return EXIT_SUCCESS;
}
