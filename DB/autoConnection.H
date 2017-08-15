/*
  Copyright (C) 2012
  Alejandro Mujica (amujica@cenditel.gob.ve)
  Erwin Paredes (eparedes@cenditel.gob.ve)
  José Ruiz (jruiz@cenditel.gob.ve)
  Rodolfo Rangel (rrangel@cenditel.gob.ve)
  Julie Vera (jvera@cenditel.gob.ve)
 
  CENDITEL Fundación Centro Nacional de Desarrollo e Investigación en
  Tecnologías Libres
 
  Este programa es software libre; Usted puede usarlo bajo los términos de la
  licencia de software GPL versión 2.0 de la Free Software Foundation.
 
  Este programa se distribuye con la esperanza de que sea útil, pero SIN
  NINGUNA GARANTÍA; tampoco las implícitas garantías de MERCANTILIDAD o
  ADECUACIÓN A UN PROPÓSITO PARTICULAR.
  Consulte la licencia GPL para más detalles. Usted debe recibir una copia
  de la GPL junto con este programa; si no, escriba a la Free Software
  Foundation Inc. 51 Franklin Street,5 Piso, Boston, MA 02110-1301, USA.
*/

/*
  Autor:             Alejandro J. Mujica
  Fecha de creación: 25/07/2013
  Este archivo contiene la definición e implementación de la clase
  AutoConnection.
*/

# ifndef AUTO_CONNECTION_H
# define AUTO_CONNECTION_H

# include <dbConnection.H>

/** Establece una conexión a la base de datos y la desconecta automáticamente.
 *
 *  Esta clase es útil porque en las funciones de consulta normalmente se
 *  solicita la instancia de DB_Connection, se abre la conexión, se hace
 *  todo el proceso de consulta y al final se cierra la conexión.
 *
 *  Instanciar un objeto de tipo Auto_Connection abre la conexión con la base
 *  de datos al construirse, al salir del ámbito en el que esté instanciado se
 *  destruirá cerrando la conexión automáticamente.
 *
 *  @author Alejandro J. Mujica
 */
class AutoConnection : public DBConnection {

public:
    AutoConnection()
        : DBConnection() {
        if (not this->open())
            throw std::logic_error("Cannot open connection");
    }

    ~AutoConnection() {
        this->close();
    }
};

# endif // AUTO_CONNECTION_H

