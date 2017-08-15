# include <iostream>
# include <string>
# include <regex>

using namespace std;

int main()
{
  string w1 = "seccion";
  string w2 = "Seccion";
  string w3 = "SeCcion";
  string w4 = "SECCION";
  string w5 = "SECTION";  

  regex pattern{"[sS][eE][cC][cC][iI][oO][nN]"};

  if (regex_match(w1, pattern))
    cout << "w1 matches\n";
  else
    cout << "w1 does not match\n";

  if (regex_match(w2, pattern))
    cout << "w2 matches\n";
  else
    cout << "w2 does not match\n";

  if (regex_match(w3, pattern))
    cout << "w3 matches\n";
  else
    cout << "w3 does not match\n";

  if (regex_match(w4, pattern))
    cout << "w4 matches\n";
  else
    cout << "w4 does not match\n";

  if (regex_match(w5, pattern))
    cout << "w5 matches\n";
  else
    cout << "w5 does not match\n";
  
  return 0;
}
