#ifndef TSP_H
#define TSP_H

#include <vector>

typedef std::vector< std::vector<int> > MAP;

std::vector<int> SolveTSP(char const* filename);
#endif
