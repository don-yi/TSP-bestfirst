#include <fstream>
#include <sstream>

#include "tsp.h"

std::vector<int> SolveTSP(char const* filename)
{
  // 
  std::vector<int> res(1, 0);
  std::stringstream ss{};

  // open up file
  std::ifstream ifs(filename);
  if (!ifs.is_open())
  {
    //abort();
    return res;
  }

  // read in
  ss << ifs.rdbuf();

  int citySize = 0;
  ss >> citySize;

  MAP map(citySize);

  for (int i = 0; i < citySize; ++i)
  {
    map[i].resize(
      citySize - 1 - i, std::numeric_limits<int>::max()
    );
  }

  // close file
  ifs.close();

  return res;
}
