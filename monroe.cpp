#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <iterator>
#include <algorithm>
#include <chrono>

#include "elections.hpp"

using std::vector;


struct voter_entry
{
    voter_id id;
    int score;
};

using voters_list = std::list<voter_entry>;

struct candidate_data
{
    voters_list voters;
    vector<voters_list::iterator> voter_positions;
};


void print_candidates(const vector<candidate_data>& candidates, election_params p)
{
    for (int i = 0; i < p.candidates; ++ i)
    {
        const auto& c = candidates[i];
        std::cout << i << " : ";
        for (const auto& data : c.voters)
        {
            std::cout << data.id << " [" << data.score << "] > ";
        }
        std::cout << std::endl;
    }
}



vector<candidate_data> preprocess(election_params p, const vote_list& votes)
{
    vector<candidate_data> candidates(p.candidates);

    using bucket_list = std::vector<voters_list>;
    vector<bucket_list> buckets(p.candidates);
    for (bucket_list& b : buckets)
    {
        b.resize(p.candidates);
    }

    for (int i = 0; i < p.votes; ++ i)
    {
        const vote& v = votes[i];
        for (int j = 0; j < p.candidates; ++ j)
        {
            int score = p.candidates - j - 1;
            candidate_id c = v[j];
            voter_entry voter{ i, score };
            buckets[c][score].push_back(voter);
        }
    }

    for (int i = 0; i < p.candidates; ++ i)
    {
        candidate_data& c = candidates[i];
        c.voter_positions.resize(p.votes);

        for (int j = 0; j < p.candidates; ++ j)
        {
            c.voters.splice(begin(c.voters), buckets[i][j]);
        }
        for (auto it = begin(c.voters); it != end(c.voters); ++ it)
        {
            voter_id v = it->id;
            c.voter_positions[v] = it;
        }
    }

    return candidates;
}

int score(const candidate_data& c, int size)
{
    int score = 0;
    auto it = begin(c.voters);
    for (int i = 0; i < size; ++ it, ++ i)
    {
        score += it->score;
    }
    return score;
}

vector<voter_id> best_voters(const candidate_data& c, int size)
{
    vector<voter_id> vs;
    auto it = begin(c.voters);
    for (int i = 0; i < size; ++ it, ++ i)
    {
        vs.push_back(it->id);
    }
    return vs;
}

void remove(candidate_data& c, voter_id v)
{
    auto it = c.voter_positions[v];
    c.voters.erase(it);
}

committee find_committee(election_params p, vector<candidate_data>& candidates)
{
    committee committee;
    const int to_cover = p.votes / p.committee;
    vector<bool> candidate_used(p.candidates, false);

    for (int i = 0; i < p.committee; ++ i)
    {
        int max_score = 0;
        candidate_id best = -1;
        for (int c = 0; c < p.candidates; ++ c)
        {
            if (! candidate_used[c])
            {
                int s = score(candidates[c], to_cover);
                if (s > max_score)
                {
                    max_score = s;
                    best = c;
                }
            }
        }
        auto to_remove = best_voters(candidates[best], to_cover);

        for (auto& c : candidates)
        {
            for (voter_id v : to_remove)
            {
                remove(c, v);
            }
        }
        candidate_used[best] = true;
        committee.push_back(best);
    }
    return committee;
}




vote_list read_votes(election_params p)
{
    vote_list e;
    e.reserve(p.votes);
    for (int i = 0; i < p.votes; ++ i)
    {
        vote v;
        v.reserve(p.candidates);
        std::copy_n(
            std::istream_iterator<candidate_id>(std::cin),
            p.candidates,
            std::back_inserter(v));
        e.push_back(std::move(v));
    }
    return e;
}

void print_election(const vote_list& e)
{
    for (int i = 0; i < e.size(); ++ i)
    {
        std::cout << i << " : ";
        for (auto v : e[i])
        {
            std::cout << v << ' ';
        }
        std::cout << std::endl;
    }
}


int main(int argc, char* argv[])
{
    election_params params;
    std::cin >> params.votes >> params.candidates >> params.committee;

    auto t_before_read = std::chrono::high_resolution_clock::now();
    vote_list e = read_votes(params);
    auto t_after_read = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> read_time = t_after_read - t_before_read;
    std::cout << "Reading: " << read_time.count() << std::endl;

    auto t_before_preprocessing = std::chrono::high_resolution_clock::now();
    auto candidates = preprocess(params, e);
    auto t_after_preprocessing = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> preprocessing_time = t_after_preprocessing - t_before_preprocessing;
    std::cout << "Preprocessing: " << preprocessing_time.count() << std::endl;

    auto t_before_loop = std::chrono::high_resolution_clock::now();
    auto committee = find_committee(params, candidates);
    auto t_after_loop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> loop_time = t_after_loop - t_before_loop;
    std::cout << "Loop: " << loop_time.count() << std::endl;

    std::ofstream out("result");
    for (auto id : committee)
    {
        out << id << std::endl;
    }
    std::cout << std::endl;
}
