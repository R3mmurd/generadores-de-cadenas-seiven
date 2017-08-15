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
  Este archivo contiene la implementación de la clase DBProperties.
*/

# include <stdexcept>
# include <sstream>
# include <fstream>

# include <dbProperties.H>

std::unique_ptr <DBProperties> DBProperties::instance;

DBProperties::DBProperties() : host(), port(), user(), pass(), dbname() {
    // Empty
}

DBProperties & DBProperties::getInstance() {
    if (instance.get() == NULL)
        instance = std::unique_ptr <DBProperties> (new DBProperties);

    return *instance;
}

const std::string & DBProperties::getHost() const {
    return host;
}

void DBProperties::setHost(const std::string & host) {
    this->host = host;
}

void DBProperties::setHost(std::string && host)
{
    this->host = std::move(host);
}

const std::string & DBProperties::getPort() const {
    return port;
}

void DBProperties::setPort(const std::string & port) {
    this->port = port;
}

void DBProperties::setPort(std::string && port) {
    this->port = std::move(port);
}

const std::string & DBProperties::getUser() const {
    return user;
}

void DBProperties::setUser(const std::string & user) {
    this->user = user;
}

void DBProperties::setUser(std::string && user) {
    this->user = std::move(user);
}

const std::string & DBProperties::getPassword() const {
    return pass;
}

void DBProperties::setPassword(const std::string & pass) {
    this->pass = pass;
}

void DBProperties::setPassword(std::string && pass) {
    this->pass = std::move(pass);
}

const std::string & DBProperties::getDbname() const {
    return dbname;
}

void DBProperties::setDbname(const std::string & dbname) {
    this->dbname = dbname;
}

void DBProperties::setDbname(std::string && dbname) {
    this->dbname = std::move(dbname);
}

std::string DBProperties::getConnectionInfo() { 

    std::stringstream sstr;

    sstr << "host=" << host << " port=" << port << " user=" << user
         << " password=" << pass << " dbname=" << dbname;

    return sstr.str();
}

void DBProperties::readFile(const std::string & fileName) {
    if(fileName.empty())
        throw std::logic_error("file name is empty");

    std::ifstream file(fileName.c_str());

    if (not file)
        throw std::logic_error("file does not exist");

    std::string host;

    std::string port;

    std::string dbname;

    std::string user;

    std::string pass;

    getline(file, host);
    getline(file, port);
    getline(file, dbname);
    getline(file, user);
    getline(file, pass);

    if(not host.empty())
        std::swap(this->host, host);

    if(not port.empty())
        std::swap(this->port, port);

    if(not dbname.empty())
        std::swap(this->dbname, dbname);

    if(not user.empty())
        std::swap(this->user, user);

    if(not pass.empty())
        std::swap(this->pass, pass);

    file.close();
}

