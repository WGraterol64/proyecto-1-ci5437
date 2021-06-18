#include <iostream>
#include <vector>
#include <set>

using namespace std;

int valid_ruleids[10240];

void generate_state(const state_t &root, state_t &result, int bound) {
    set<uint64_t> visited;
    vector<state_t*> path;
    copy_state(&result, &root);
    visited.insert(hash_state(&root));
    while(path.size() < (long unsigned) bound ) {
        int ruleid;
        ruleid_iterator_t iter;

        int index = 0;
        init_bwd_iter(&iter, &result);
        while( (ruleid = next_ruleid(&iter) ) >= 0 ) {
            assert(index < 10240);
            state_t child;
            apply_bwd_rule(ruleid, &result, &child);
            if (visited.count(hash_state(&child)) != 0) continue;
            valid_ruleids[index++] = ruleid;
        }

        if( index == 0 ) {
            delete path.back();
            path.pop_back();
            copy_state(&result, path.back());
        } else {
            state_t *child = new state_t;
            int ruleid = valid_ruleids[lrand48() % index];
            apply_bwd_rule(ruleid, &result, child);
            copy_state(&result, child);
            visited.insert(hash_state(child));
            path.push_back(child);
        }
    }
}
 
int main(int argc, const char **argv) {
    if( argc < 4 ) {
        cout << argv[0] << " <seed> <n> <depth-bound>" << endl;
        return -1;
    }
    int seed = atoi(argv[1]);
    int n = atoi(argv[2]);
    int bound = atoi(argv[3]);
    char buff[1024];

    unsigned short seed_array[3];
    seed_array[0] = seed_array[1] = seed_array[2] = seed;
    seed48(seed_array);

    int d;
    state_t goal;
    first_goal_state(&goal, &d);

    for( int i = 0; i < n; ++i ) {
        state_t state;
        generate_state(goal, state, bound);
        sprint_state(buff, 1024, &state);
        buff[strlen(buff) - 1] = '\0';
        cout << buff << endl;
    }
    return 0;
}

