#include <fstream> // file stream
#include <sstream> // str stream

#include "tsp.h"

#include <algorithm> // std::min_elem()
#include <map> // std::multimap


//todo: FOR DEBUG
#include <iostream>
bool isDebugOn = false;


//lower bound evaluation function
int lower_bound(
  MAP const& m,
  int    const& index,
  std::vector<int>& solution_so_far
)
{
  int lower_bound = 0;

  // skip city visited
  int const numCity = m.size();
  for (int i = 0; i < numCity; ++i) 
  {
    if (
      i > 0 &&
      (
        std::find(solution_so_far.begin(), solution_so_far.end(), i)
        != solution_so_far.end()
        )
      )
    {
      continue;
    }

    int min_in_row = std::numeric_limits<int>::max();

    for (int j = 0; j < numCity; ++j) 
    {
      //check if job is not assigned (column is taken)
      if (
        j != solution_so_far[index] &&
        std::find(solution_so_far.begin(), solution_so_far.end(), j) != solution_so_far.end()
        ) 
      {
        continue;
      }

      if (min_in_row > m[i][j])
        min_in_row = m[i][j];
    }

    lower_bound += min_in_row;
  }
  return lower_bound;
}

void backtracking_branch_bound_best_first_aux(
  MAP const& m,
  unsigned const& index, 
  std::vector<int>& solution_so_far, 
  int& cost_solution_so_far, 
  std::vector<int>& best_solution_so_far, 
  int& cost_best_solution_so_far
)
{
  unsigned const cityNum = m.size();
  if (index == cityNum - 1)
  {
    solution_so_far.push_back(0);
    cost_solution_so_far += m[solution_so_far[index]][0];
    if (cost_solution_so_far < cost_best_solution_so_far)
    {
      cost_best_solution_so_far = cost_solution_so_far;
      best_solution_so_far = solution_so_far;
    }
    cost_solution_so_far -= m[solution_so_far[index]][0];
    solution_so_far.pop_back();

    return;
  }

  std::multimap<int,int> ordered_jobs;

  //generate nodes and order them by bound
  for ( unsigned j=0; j<cityNum; ++j ) 
  {
      //skip if job j is already assigned
      if (
        std::find( 
          solution_so_far.begin(), solution_so_far.end(), j) != solution_so_far.end()
        )
        continue;

      solution_so_far.push_back( j );
      cost_solution_so_far += m[solution_so_far[index]][j];

      int const lb = lower_bound(m, index + 1, solution_so_far);
      ordered_jobs.insert ( std::pair<int,int>(lb,j) );
      cost_solution_so_far -= m[solution_so_far[index]][j];
      solution_so_far.pop_back( );
  }

  std::multimap<int, int>::iterator
  b = ordered_jobs.begin(), e = ordered_jobs.end();

  while (b != e)
  {
    int j = b->second;
    solution_so_far.push_back(j);
    cost_solution_so_far += m[solution_so_far[index]][j];

    int const lb = b->first;
    if (lb + cost_solution_so_far < cost_best_solution_so_far) 
    {
      backtracking_branch_bound_best_first_aux(
        m, 
        index + 1,
        solution_so_far, 
        cost_solution_so_far,
        best_solution_so_far, 
        cost_best_solution_so_far
        );
    }

    cost_solution_so_far -= m[solution_so_far[index]][j];
    solution_so_far.pop_back();

    ++b;
  }
}

std::vector<int>
backtracking_branch_bound_best_first(MAP const& m)
{
    std::vector<int> best_solution_so_far;
    std::vector<int> solution_so_far;

    // init
    solution_so_far.push_back(0);
    best_solution_so_far.push_back(0);
    
    int cost_solution_so_far = 0;
    int cost_best_solution_so_far = std::numeric_limits<int>::max();

    // aux call
    backtracking_branch_bound_best_first_aux( 
      m, 
      0, 
      solution_so_far, 
      cost_solution_so_far, 
      best_solution_so_far, 
      cost_best_solution_so_far
    );

    return best_solution_so_far;
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


  std::vector<int> res = backtracking_branch_bound_best_first(oriMat);

  return res;
}

