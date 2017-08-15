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
