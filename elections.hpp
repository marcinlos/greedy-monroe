#ifndef ELECTIONS_HPP
#define ELECTIONS_HPP

#include <vector>
#include <list>


using std::vector;

using voter_id = int;
using candidate_id = int;

using vote = vector<candidate_id>;
using vote_list = vector<vote>;
using committee = vector<candidate_id>;

struct election_params
{
    int votes;
    int candidates;
    int committee;
};



#endif // ELECTIONS_HPP
