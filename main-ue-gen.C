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

# include <ue-gen.H>

# include <tclap/CmdLine.h>

using namespace TCLAP;

int main(int argc, char * argv[])
{
  CmdLine cmd("Generador de cadenas productivas por unidad económica",
	      ' ', "1.0");

  ValueArg<string> rif("r", "rif", "RIF de la unidad económica",
		       true, "", "RIF");
  
  ValueArg<string> name("n", "name", "Nombre de la unidad económica",
			true, "", "NAME");

  cmd.xorAdd(rif, name);
  
  ValueArg<year_t> year("y", "year", "Año de registro", true, 0,
			"YEAR");
  cmd.add(year);

  ValueArg<string> input("i", "input", "Nombre del archivo de entrada (mapa)",
			 true, "", "INPUT");
  cmd.add(input);

  ValueArg<string> output("o", "output", "Nombre del archivo de salida", true,
			  "", "OUTPUT");
  cmd.add(output);
  
  cmd.parse(argc, argv);

  try
    {
      if (rif.isSet())
	generate_ue_chain(rif.getValue(), year.getValue(), input.getValue(),
			  output.getValue(), UEKey::RIF);
      else
	generate_ue_chain(name.getValue(), year.getValue(), input.getValue(),
			  output.getValue(), UEKey::NAME);
    }
  catch (const std::exception & e)
    {
      cout << "An excepion was caught with this message: "
	   << e.what() << endl;
      return EXIT_FAILURE;
    }
      
  return EXIT_SUCCESS;
}
