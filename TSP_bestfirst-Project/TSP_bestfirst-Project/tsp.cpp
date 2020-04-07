#include <fstream> // file stream
#include <sstream> // str stream

#include "tsp.h"

#include <algorithm> // std::min_elem()


// FOR DEBUG
#include <iostream>


std::vector<int> SolveTSP(char const* filename)
{
  // var's
  std::stringstream ss{};
  std::vector<int> res(1, 0);

  // open up file
  std::ifstream ifs(filename);
  if (!ifs.is_open())
  {
    //abort();
    return res;
  }

  // read in
  ss << ifs.rdbuf();

  // close file
  ifs.close();

  // get city size
  int citySize = 0;
  ss >> citySize;

  // init mat w/ city size and infinity val
  MAP oriMat(citySize);
  for (int i = 0; i < citySize; ++i)
  {
    oriMat[i].resize(
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
      ss >> oriMat[i][j];
      oriMat[j][i] = oriMat[i][j];

      ++j;
    }
    ++i;
  }


  //DEBUG ORI MAT
  for (auto const& row : oriMat)
  {
    for (const int elem : row)
    {
      // i for infinity
      if (elem > 9999)
      {
        std::cout << "i" << " ";
        continue;
      }

      std::cout << elem << " ";
    }
    std::cout << std::endl;
  }


  // cp ori mat
  MAP tmpMat(citySize);
  for (int i = 0; i < citySize; ++i)
  {
    tmpMat[i] = oriMat[i];
  }

  // do row calc's
  int reducedC = 0;
  for (auto&& vector : tmpMat)
  {
    // find row min
    std::vector<int>::iterator forwardIt =
      std::min_element(vector.begin(), vector.end());
    int minPos = std::distance(vector.begin(), forwardIt);
    int rowMin = vector[minPos];

    // skip min 0
    if (rowMin == 0)
    {
      continue;
    }

    // subt min from row
    for (auto && elem : vector)
    {
      elem -= rowMin;
    }

    // add all min val
    reducedC += rowMin;
  }

  // do col calc
  for (int i = 0; i < citySize; ++i)
  {
    // make vector for each col
    std::vector<int> tmpVec{};
    for (auto&& vector : tmpMat)
    {
      tmpVec.push_back(vector[i]);
    }

    // find col min
    std::vector<int>::iterator forwardIt =
      std::min_element(tmpVec.begin(), tmpVec.end());
    int minPos = std::distance(tmpVec.begin(), forwardIt);
    int colMin = tmpVec[minPos];

    // skip min 0
    if (colMin == 0)
    {
      continue;
    }

    // subt min from col
    for (auto&& vector : tmpMat)
    {
      vector[i] -= colMin;
    }

    // add all min val
    reducedC += colMin;
  }


  //DEBUG TMP MAT
  std::cout << std::endl << std::endl;
  for (auto const& row : tmpMat)
  {
    for (const int elem : row)
    {
      // i for infinity
      if (elem > 9999)
      {
        std::cout << "i" << " ";
        continue;
      }

      std::cout << elem << " ";
    }
    std::cout << std::endl;
  }
  //MIN SUM
  std::cout << reducedC << std::endl;

  // return res
  // todo: assign city num


  return res;
}

