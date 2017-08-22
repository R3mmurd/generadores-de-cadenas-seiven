/*
  Este archivo contiene el programa para construir cadenas por actividad 
  económica.
  
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

# include <caev-gen.H>

# include <tclap/CmdLine.h>

using namespace TCLAP;

int main(int argc, char * argv[])
{
  CmdLine cmd("CAEV chains generator", ' ', "1.0");

  ValueArg<string> level("l", "level",
			 "Nivel de clasificación de la actividad económica",
			 true, "", "LEVEL");
  cmd.add(level);

  ValueArg<string> caev("c", "caev-cod", "Código de la actividad económica",
			true, "", "CODE");
  cmd.add(caev);
  
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
      generate_caev_chain(level.getValue(), caev.getValue(), year.getValue(),
			  input.getValue(), output.getValue());
    }
  catch (const std::exception & e)
    {
      cout << "An excepion was caught with this message: "
	   << e.what() << endl;
      return EXIT_FAILURE;
    }
  
  return EXIT_SUCCESS;
}
