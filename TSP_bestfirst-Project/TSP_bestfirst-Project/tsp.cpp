#include <fstream> // file stream
#include <sstream> // str stream

#include "tsp.h"

#include <algorithm> // std::min_elem()


//todo: FOR DEBUG
#include <iostream>
bool isDebugOn = false;


// tree info struct
struct Nd
{
  int cost = 0;
  MAP mat;
  std::vector<int> cityUnvisited;
  int cityFrom = 0;
  std::vector<int> best_solution_so_far;
};
typedef std::vector<Nd> NDVEC;

std::vector<int> SolveTSPAux(NDVEC& ndVec)
{
  while (true)
  {
    // find ind of best cost
    int ndInd = 0;
    int bestCost = std::numeric_limits<int>::max();
    int cityUnvisitedSize = std::numeric_limits<int>::max();
    for (int i = 0; i < ndVec.size(); ++i)
    {
      if (
        (ndVec[i].cost == bestCost &&
          ndVec[i].cityUnvisited.size() < cityUnvisitedSize)
        ||
        ndVec[i].cost < bestCost
        )
      {
        bestCost = ndVec[i].cost;
        ndInd = i;
        cityUnvisitedSize = ndVec[i].cityUnvisited.size();
      }
    }
    // get curr nd
    Nd currNd = ndVec[ndInd];

    //termination check (if on last level)
    if (currNd.cityUnvisited.size() == 1)
    {
      currNd.best_solution_so_far.push_back(currNd.cityUnvisited[0]);
      currNd.best_solution_so_far.push_back(0);
      return currNd.best_solution_so_far;
    }

    int const citySize = currNd.mat.size();
    // for all not-done-nd in same lvl
    for (auto&& cityTo : currNd.cityUnvisited)
    {
      // make sub mat
      MAP subMat(citySize);
      int reducedCost = 0;
      {
        // cp mat-so-far
        for (int i = 0; i < citySize; ++i)
        {
          subMat[i] = currNd.mat[i];
        }

        // skip for base city (0)
        if (cityTo)
        {
          // make from-city-row and to-city-col all infinity
          for (auto&& elem : subMat[currNd.cityFrom])
          {
            elem = std::numeric_limits<int>::max();
          }
          for (auto&& row : subMat)
          {
            row[cityTo] = std::numeric_limits<int>::max();
          }

          // no return from city-to to city-from -> infinity
          subMat[cityTo][currNd.cityFrom] = std::numeric_limits<int>::max();


          //todo: DEBUG INF MAT
          if (isDebugOn)
          {
            std::cout << std::endl << std::endl;
            std::cout << currNd.cityFrom << "->" << cityTo << std::endl;
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
          }


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
          reducedCost += rowMin;
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
          reducedCost += colMin;
        }
      }


      //todo: DEBUG SUB MAT
      if (isDebugOn)
      {
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
        std::cout << "reduced cost:" << reducedCost << std::endl;
      }


      // init new nd
      Nd newNd;

      // up's
      newNd.best_solution_so_far = currNd.best_solution_so_far;
      newNd.best_solution_so_far.push_back(cityTo);
      newNd.cityFrom = cityTo;
      newNd.mat = subMat;

      // rm visited city by value
      newNd.cityUnvisited = currNd.cityUnvisited;
      newNd.cityUnvisited.erase(
        std::remove(
          newNd.cityUnvisited.begin(),
          newNd.cityUnvisited.end(),
          cityTo
        ),
        newNd.cityUnvisited.end()
      );

      // 1st city cost
      if (!cityTo)
      {
        newNd.cost = reducedCost;
      }
      else
      {
        newNd.cost =
          currNd.mat[currNd.cityFrom][cityTo] + currNd.cost + reducedCost;
      }


      if (isDebugOn)
      {
        std::cout << "total cost: " << newNd.cost;
      }

      // push new nd
      ndVec.push_back(newNd);

      // break if 1st city
      if (!cityTo)
      {
        break;
      }
    }

    // rm curr nd
    ndVec.erase(ndVec.begin() + ndInd);
  }
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
  if (isDebugOn)
  {
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
  }


    // nd vec init
  NDVEC ndVec;
  // 1st nd
  Nd firstNd;
  firstNd.mat = oriMat;
  for (int i = 0; i < citySize; ++i)
  {
    firstNd.cityUnvisited.push_back(i);
  }

  ndVec.push_back(firstNd);

  // aux fn
  std::vector<int> res = SolveTSPAux(ndVec);
  return res;
}

