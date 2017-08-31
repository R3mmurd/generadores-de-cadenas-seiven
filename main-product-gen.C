/*
  Este archivo contiene el programa para construir cadenas por unidad económica. 
  
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

# include <product-gen.H>

# include <tclap/CmdLine.h>

using namespace TCLAP;

int main(int argc, char * argv[])
{
  CmdLine cmd("Generador de cadenas productivas por unidad económica",
	      ' ', "1.0");

  ValueArg<db_id_t> id("p", "product-id", "ID del producto",
		       true, 0, "PRODUCT-ID");
  
  cmd.add(id);
  
  ValueArg<year_t> year("y", "year", "Año de registro", true, 0,
			"YEAR");
  cmd.add(year);

  ValueArg<unsigned short> upstream("u", "upstream",
				    "Cantidad de niveles máximos aguas arriba",
				    true, 0, "NUM-LEVELS");

  cmd.add(upstream);

  ValueArg<unsigned short> downstream("d", "downstream",
				      "Cantidad de niveles máximos aguas abajo",
				      true, 0, "NUM-LEVELS");

  cmd.add(downstream);

  ValueArg<string> input("i", "input", "Nombre del archivo de entrada (mapa)",
			 true, "", "INPUT");
  cmd.add(input);

  ValueArg<string> output("o", "output", "Nombre del archivo de salida", true,
			  "", "OUTPUT");
  cmd.add(output);

  string distance_str
    = "Distancia de edición máxima entre nombres de insumos y productos";
  
  ValueArg<unsigned short> distance("e", "edition-distance", distance_str,
				    false, 50, "EDITION-DISTANCE");

  cmd.add(distance);
				      
  cmd.parse(argc, argv);

  try
    {
      generate_product_chain(id.getValue(), year.getValue(), upstream.getValue(),
			     downstream.getValue(), input.getValue(),
			     output.getValue(), distance.getValue());
    }
  catch (const std::exception & e)
    {
      cout << "An excepion was caught with this message: "
	   << e.what() << endl;
      return EXIT_FAILURE;
    }
      
  return EXIT_SUCCESS;
}
