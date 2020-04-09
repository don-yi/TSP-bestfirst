#include <fstream> // file stream
#include <sstream> // str stream

#include "tsp.h"

#include <algorithm> // std::min_elem()


// FOR DEBUG
#include <iostream>


void SolveTSPAux(
  MAP const& oriMat,
  MAP const& mat_so_far,
  std::vector<int> const& ndChecker,
  int const& cityFrom,
  std::vector<int>& best_solution_so_far
)
{
  //termination check (if on last level)
  if (ndChecker.size() == 1)
  {
    best_solution_so_far.push_back(ndChecker[0]);
    return;
  }

  // init bsf var's
  int bestCost = std::numeric_limits<int>::max();
  MAP bestMat;
  int bestNd = 0;

  int const citySize = oriMat.size();
  // find best sol for all not-done-nd
  for (auto&& cityTo : ndChecker)
  {
    // make sub mat
    MAP subMat(citySize);
    int reducedC = 0;
    {
      // cp mat-so-far
      for (int i = 0; i < citySize; ++i)
      {
        subMat[i] = mat_so_far[i];
      }

      // make from-city-row and to-city-col all infinity
      for (auto&& elem : subMat[cityFrom])
      {
        elem = std::numeric_limits<int>::max();
      }
      for (auto&& row : subMat)
      {
        row[cityTo] = std::numeric_limits<int>::max();
      }

      // no return from city-to to city-from -> infinity
      subMat[cityTo][cityFrom] = std::numeric_limits<int>::max();


      //todo: DEBUG INF MAT
      std::cout << std::endl << std::endl;
      std::cout << cityFrom << "->" << cityTo << std::endl;
      std::cout << "inf mat" << std::endl;
      for (auto const& row : subMat)
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


      // do row calc
      for (auto&& vector : subMat)
      {
        // find row min
        std::vector<int>::iterator forwardIt =
          std::min_element(vector.begin(), vector.end());
        int minPos = std::distance(vector.begin(), forwardIt);
        int rowMin = vector[minPos];

        // skip min 0
        if (rowMin == 0 || rowMin > 9999)
        {
          continue;
        }

        // subt min from row
        for (auto&& elem : vector)
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
        for (auto&& vector : subMat)
        {
          tmpVec.push_back(vector[i]);
        }

        // find col min
        std::vector<int>::iterator forwardIt =
          std::min_element(tmpVec.begin(), tmpVec.end());
        int minPos = std::distance(tmpVec.begin(), forwardIt);
        int colMin = tmpVec[minPos];

        // skip min 0
        if (colMin == 0 || colMin > 9999)
        {
          continue;
        }

        // subt min from col
        for (auto&& vector : subMat)
        {
          vector[i] -= colMin;
        }

        // add all min val
        reducedC += colMin;
      }


      //todo: DEBUG SUB MAT
      std::cout << std::endl;
      std::cout << "sub mat" << std::endl;
      for (auto const& row : subMat)
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


    }

    // up best var's
    int const currCost = oriMat[cityFrom][cityTo] + reducedC;
    if (currCost < bestCost)
    {
      bestCost = currCost;
      bestMat = subMat;
      bestNd = cityTo;
    }
  }

  // up nd checker
  std::vector<int> upedNdChecker = ndChecker;
  upedNdChecker.erase(
    std::remove(
      upedNdChecker.begin(), 
      upedNdChecker.end(), 
      bestNd
    ), 
    upedNdChecker.end()
  );

  // up bsf
  best_solution_so_far.push_back(bestNd);

  // recursive call
  SolveTSPAux(
    oriMat, 
    bestMat, 
    upedNdChecker, 
    bestNd, 
    best_solution_so_far
  );
}

std::vector<int> SolveTSP(char const* filename)
{
  // file and str stream
  std::stringstream ss{};
  {
    // open up file
    std::ifstream ifs(filename);
    if (!ifs.is_open())
    {
      //abort();
      return {};
    }

    // read in
    ss << ifs.rdbuf();

    // close file
    ifs.close();
  }

  // get city size
  int citySize = 0;
  ss >> citySize;

  // make ori mat
  MAP oriMat(citySize);
  {
    // init mat w/ city size and infinity val
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
  }

  //todo: DEBUG ORI MAT
  std::cout << "ori mat" << std::endl;
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
    std::cout << std::endl;
  }

  // init param's
  std::vector<int> ndChecker;
  for (int i = 1; i < citySize; ++i)
  {
    ndChecker.push_back(i);
  }
  std::vector<int> best_solution_so_far(1, 0);

  // aux fn
  SolveTSPAux(
    oriMat,
    oriMat,
    ndChecker,
    0,
    best_solution_so_far
  );

  best_solution_so_far.push_back(0);

  return best_solution_so_far;
}

