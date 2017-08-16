/*
  Este archivo contiene el rograma para construir cadenas por actividad 
  económica.
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

# include <caev-gen.H>

# include <tclap/CmdLine.h>

using namespace TCLAP;

int main(int argc, char * argv[])
{
  CmdLine cmd("CAEV chains generator", ' ', "0.0");

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
