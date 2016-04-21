#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include "elections.hpp"


struct point {
    double x, y;
};

double dist(point a, point b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return dx * dx + dy * dy;
}

struct normal_distribution_2d {
    point center;
    std::normal_distribution<> dist_x;
    std::normal_distribution<> dist_y;

    normal_distribution_2d(point center, double sx, double sy)
    : center{ center }
    , dist_x{ 0.0, sx }
    , dist_y{ 0.0, sy }
    { }

    template <typename Gen>
    point operator ()(Gen& g) {
        double x = dist_x(g);
        double y = dist_y(g);
        return { center.x + x, center.y + y };
    }

};

template <typename Gen, typename Dist>
vector<point> random_points(Gen& gen, Dist& dist, int n) {
    vector<point> points;
    for (int i = 0; i < n; ++ i) {
        points.push_back(dist(gen));
    }
    return points;
}

vote trivial_vote(int n) {
    vote v(n);
    std::iota(begin(v), end(v), 0);
    return v;
}

vote_list create_votes(const vector<point>& candidates, const vector<point>& voters) {
    vote_list votes;
    vote v = trivial_vote(candidates.size());

    for (int i = 0; i < voters.size(); ++ i) {
        point x = voters[i];
        std::sort(begin(v), end(v), [x,&candidates](int i, int j) {
            return dist(x, candidates[i]) < dist(x, candidates[j]);
        });
        votes.push_back(v);
    }
    return votes;
}

void print_election(const vote_list& e, election_params p, std::ostream& os) {
    os << p.votes << ' ' << p.candidates << ' ' << p.committee << std::endl;
    for (int i = 0; i < e.size(); ++ i) {
        for (auto v : e[i]) {
            os << v << ' ';
        }
        os << std::endl;
    }
}

void print_points(const vector<point>& points, std::ostream& os) {
    for (point p : points) {
        os << p.x << " " << p.y << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: ./generator <votes> <candidates> <committee size>" << std::endl;
        return -1;
    }

    election_params params;
    params.votes = std::atoi(argv[1]);
    params.candidates = std::atoi(argv[2]);
    params.committee = std::atoi(argv[3]);

    std::mt19937 rng;
    normal_distribution_2d d{ {0, 0}, 1, 1 };

    vector<point> candidates = random_points(rng, d, params.candidates);
    vector<point> voters = random_points(rng, d, params.votes);

    auto e = create_votes(candidates, voters);

    std::ofstream oe("votes");
    print_election(e, params, oe);

    std::ofstream oc("candidates");
    print_points(candidates, oc);

    std::ofstream ov("voters");
    print_points(voters, ov);
}
