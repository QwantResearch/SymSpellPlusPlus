#include <iostream>
#include "../include/symspell.h"

using namespace std;

void Split(const std::string &line, std::vector<std::string> &pieces, const std::string del) 
{
  size_t begin = 0;
  size_t pos = 0;
  std::string token;

  while ((pos = line.find(del, begin)) != std::string::npos) 
  {
      if (pos > begin) 
      {
          token = line.substr(begin, pos - begin);
          if (token.size() > 0) pieces.push_back(token);
      }
      begin = pos + del.size();
  }
  if (pos > begin) 
  {
      token = line.substr(begin, pos - begin);
  }
  if (token.size() > 0) pieces.push_back(token);
}


int main(int argc, char* argv[])
{
	symspell::SymSpell symSpell;
        symSpell.LoadDictionary(argv[1],1,0);
        int nbest = 1;
        vector< std::unique_ptr<symspell::SuggestItem>> items;
        std::string line;
        while (std::getline(std::cin, line))
        {
            vector<string> vec_line;
//             std::cout << line << std::endl;
            Split(line,vec_line," ");
            int inc=0;
            while (inc < (int)vec_line.size())
            {
                items.clear();
                symSpell.Lookup(vec_line[inc], symspell::Verbosity::All, items);
                for(int i=0 ; i < nbest && i < (int)items.size(); i++)
                {    
//                     cout << vec_line[inc] << "\t" << items[i]->term << "\t" << items[i]->count << "\t" << items[i]->distance << endl;
                    if (i != 0) cout << " ";
                    cout << items[i]->term ; 
                }
                inc++;
            }
            cout << endl; 
        }
}
