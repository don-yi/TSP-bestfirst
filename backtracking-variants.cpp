#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>
#include <map>

typedef std::vector<std::vector<int> > Matrix;

Matrix
convert( int const& N, int (*m)[4] ) {
    Matrix result( N, std::vector<int>(N,0) );
    for (int i=0; i<N; ++i) {
        for (int j=0; j<N; ++j) {
            result[i][j] = m[i][j];
        }
    }
    return result;
}

/////////// BACKTRACKING VANILLA ///////////////////////
void
backtracking_vanilla_aux( 
        Matrix const& m, 
        int    const& N, 
        int    const& index, 
        std::vector<int> &solution_so_far, 
        int& cost_solution_so_far, 
        std::vector<int> &best_solution_so_far, 
        int& cost_best_solution_so_far )
{

    //termination check (if on last level)
    if ( index == N ) {
        std::cout << "solution " << cost_solution_so_far << " ( best " << cost_best_solution_so_far << " )" << std::endl;
        if ( cost_solution_so_far < cost_best_solution_so_far ) {
            cost_best_solution_so_far = cost_solution_so_far;
            best_solution_so_far = solution_so_far;
        }
        return;
    }

    //generate nodes
    for ( int j=0; j<N; ++j ) {

        //skip if job j is already assigned
        if ( std::find( solution_so_far.begin(), solution_so_far.end(), j) != solution_so_far.end() ) continue;

        //otherwise assign and call recursively
        solution_so_far.push_back( j );
        cost_solution_so_far += m[index][j];

        backtracking_vanilla_aux( m, N, index+1, 
                solution_so_far, cost_solution_so_far, 
                best_solution_so_far, cost_best_solution_so_far );

        solution_so_far.pop_back( );
        cost_solution_so_far -= m[index][j];
    }
}

std::vector<int>
backtracking_vanilla( Matrix const& m, int const& N ) {
    std::vector<int> best_solution_so_far;
    std::vector<int> solution_so_far;
    int cost_solution_so_far = 0;
    int cost_best_solution_so_far = std::numeric_limits<int>::max();

    backtracking_vanilla_aux( m, N, 0, solution_so_far, cost_solution_so_far, best_solution_so_far, cost_best_solution_so_far );

    return best_solution_so_far;
}

/////////// BACKTRACKING + BRANCH & BOUND ///////////////////////

//lower bound evaluation function
int
lower_bound( 
        Matrix const& m, 
        int    const& N, 
        int    const& index, 
        std::vector<int> &solution_so_far,
        int    const& cost_solution_so_far )
{
    int lower_bound = cost_solution_so_far;
    for ( int i=index; i<N; ++i ) {
        int min_in_row = std::numeric_limits<int>::max();
        for ( int j=0; j<N; ++j ) {
            //check if job is not assigned (column is taken)
            if ( std::find( solution_so_far.begin(), solution_so_far.end(), j) == solution_so_far.end() ) {
                if ( min_in_row > m[i][j] ) min_in_row = m[i][j];
            }
        }
        lower_bound += min_in_row;
    }
    return lower_bound;
}

void
backtracking_branch_bound_aux( 
        Matrix const& m, 
        int    const& N, 
        int    const& index, 
        std::vector<int> &solution_so_far, 
        int& cost_solution_so_far, 
        std::vector<int> &best_solution_so_far, 
        int& cost_best_solution_so_far )
{
    //termination check (if on last level)
    if ( index == N ) {
        std::cout << "solution " << cost_solution_so_far << " ( best " << cost_best_solution_so_far << " )" << std::endl;
        if ( cost_solution_so_far < cost_best_solution_so_far ) {
            cost_best_solution_so_far = cost_solution_so_far;
            best_solution_so_far = solution_so_far;
        }
    }

    //generate nodes
    for ( int j=0; j<N; ++j ) {
        //skip if job j is already assigned
        if ( std::find( solution_so_far.begin(), solution_so_far.end(), j) != solution_so_far.end() ) continue;
        solution_so_far.push_back( j );
        cost_solution_so_far += m[index][j];

        int lb = lower_bound( m, N, index+1, solution_so_far, cost_solution_so_far );

        //branch cancelation check
        if ( lb < cost_best_solution_so_far ) {
            backtracking_branch_bound_aux( m, N, index+1, 
                    solution_so_far, cost_solution_so_far, 
                    best_solution_so_far, cost_best_solution_so_far );
        }

        solution_so_far.pop_back( );
        cost_solution_so_far -= m[index][j];
    }
}

std::vector<int>
backtracking_branch_bound( Matrix const& m, int const& N ) {
    std::vector<int> best_solution_so_far;
    std::vector<int> solution_so_far;
    int cost_solution_so_far = 0;
    int cost_best_solution_so_far = std::numeric_limits<int>::max();

    backtracking_branch_bound_aux( m, N, 0, solution_so_far, cost_solution_so_far, best_solution_so_far, cost_best_solution_so_far );

    return best_solution_so_far;
}

void
backtracking_branch_bound_best_first_aux( 
        Matrix const& m, 
        int    const& N, 
        int    const& index, 
        std::vector<int> &solution_so_far, 
        int& cost_solution_so_far, 
        std::vector<int> &best_solution_so_far, 
        int& cost_best_solution_so_far, 
        int parent_id ) 
{
    static int node_counter = 0;

    //termination check (if on last level)
    if ( index == N ) {
        std::cout << "solution " << cost_solution_so_far << " ( best " << cost_best_solution_so_far << " )" << std::endl;
        if ( cost_solution_so_far < cost_best_solution_so_far ) {
            cost_best_solution_so_far = cost_solution_so_far;
            best_solution_so_far = solution_so_far;
        }
    }

    //hack - use map to order, key - bound, value - job.
    //insert pairs, then traverse the map in-order
    std::multimap<int,int> ordered_jobs;

    //generate nodes and order them by bound
    for ( int j=0; j<N; ++j ) {
        //skip if job j is already assigned
        if ( std::find( solution_so_far.begin(), solution_so_far.end(), j) != solution_so_far.end() ) continue;
        solution_so_far.push_back( j );
        cost_solution_so_far += m[index][j];

        int lb = lower_bound( m, N, index+1, solution_so_far, cost_solution_so_far );
        ordered_jobs.insert ( std::pair<int,int>(lb,j) );
        solution_so_far.pop_back( );
        cost_solution_so_far -= m[index][j];
    }

    //traverse nodes in the above order - BEST FIRST
    std::multimap<int,int>::const_iterator b = ordered_jobs.begin(),
        e = ordered_jobs.end();
    for ( ; b != e; ++b ) {
        int j = b->second; // get job index;
        solution_so_far.push_back( j );
        cost_solution_so_far += m[index][j];

        int lb = b->first;
        //branch cancelation check
        //std::cout << "lower_bound = " << lower_bound( m, N, index, solution_so_far, cost_solution_so_far ) << std::endl;
        if ( lb < cost_best_solution_so_far ) {
            backtracking_branch_bound_best_first_aux( m, N, index+1, 
                    solution_so_far, cost_solution_so_far, 
                    best_solution_so_far, cost_best_solution_so_far, node_counter );
        }

        solution_so_far.pop_back( );
        cost_solution_so_far -= m[index][j];
    }
}

std::vector<int>
backtracking_branch_bound_best_first( Matrix const& m, int const& N ) {
    std::vector<int> best_solution_so_far;
    std::vector<int> solution_so_far;
    int cost_solution_so_far = 0;
    int cost_best_solution_so_far = std::numeric_limits<int>::max();
    backtracking_branch_bound_best_first_aux( m, N, 0, solution_so_far, cost_solution_so_far, best_solution_so_far, cost_best_solution_so_far, 0 );
    return best_solution_so_far;
}

int main() {
    int const N = 4;
    int cost = 0;
    std::vector<int> solution;
    int m_raw[N][N] = {
        {6,2,4,8},
        {3,4,7,6},
        {2,7,8,5},
        {3,5,4,2}
    };
    Matrix m = convert( N, m_raw );

    solution = backtracking_vanilla( m, N );
    cost = 0;
    for (int i=0; i<N; ++i) {
        cost += m[i][ solution[i] ];
        std::cout << solution[i] << " ";
    }
    std::cout << "  cost " << cost << std::endl;

    solution = backtracking_branch_bound( m, N );
    cost = 0;
    for (int i=0; i<N; ++i) {
        cost += m[i][ solution[i] ];
        std::cout << solution[i] << " ";
    }
    std::cout << "  cost " << cost << std::endl;

    solution = backtracking_branch_bound_best_first( m, N );
    cost = 0;
    for (int i=0; i<N; ++i) {
        cost += m[i][ solution[i] ];
        std::cout << solution[i] << " ";
    }
    std::cout << "  cost " << cost << std::endl;

    return 0;
}
