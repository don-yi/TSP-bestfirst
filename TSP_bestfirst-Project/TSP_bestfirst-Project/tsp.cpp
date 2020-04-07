#include <fstream>
#include <sstream>

#include "tsp.h"

#include <iostream> // FOR DEBUG

std::vector<int> SolveTSP(char const* filename)
{
  // var's
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

  // get city size
  int citySize = 0;
  ss >> citySize;

  // init mat w/ city size and infinity val
  MAP mat(citySize);
  for (int i = 0; i < citySize; ++i)
  {
    mat[i].resize(
      citySize, std::numeric_limits<int>::max()
    );
  }

  // make diagonally symmetric mat
  int i = 0;
  while (i < citySize)
  {
    int j = i;
    while (j < citySize)
    {
      // skip diagonal elem
      if (i == j)
      {
        ++j;
        continue;
      }

      // assign elem
      ss >> mat[i][j];
      mat[j][i] = mat[i][j];

      ++j;
    }
    ++i;
  }


  ////DEBUG MAT
  //for (auto const& row : mat)
  //{
  //  for (const int elem : row)
  //  {
  //    std::cout << elem << " ";
  //  }
  //  std::cout << std::endl;
  //}


  // close file
  ifs.close();

  return res;
}

