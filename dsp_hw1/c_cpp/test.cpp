#include "hmm.h"

#include <iostream>
#include <cassert>

int main(int argc, char* args[]) {
    assert(argc == 4);
    char* model_list    = args[1];
    char* test_data     = args[2];
    char* result        = args[3];

    FILE* fptest        = open_or_die(test_data, "r");
    FILE* fp            = open_or_die(result, "w");

    // load models
    HMM hmms[5];
    load_models(model_list, hmms, 5);

    // start testing
    char o[MAX_SEQ+1]; // line buffer
    int T = 0;
    bool first = true;

    double delta[5][MAX_SEQ][MAX_STATE];
    double termination[5] = {0.0};

    while (fscanf(fptest, "%s", &o[1]) == 1) {
        if (first)
            T = strlen(&o[1]);
        first = false;
     
        for (int _i = 0; _i < 5; ++_i) {
            for (int j = 0; j < MAX_SEQ; ++j)
                for (int k = 0; k < MAX_STATE; ++k)
                    delta[_i][j][k] = 0;

            // alias variables
            int N = hmms[_i].state_num;
            double (&pi)[MAX_STATE] = hmms[_i].initial;
            double (&a)[MAX_STATE][MAX_STATE] = hmms[_i].transition;
            double (&b)[MAX_OBSERV][MAX_STATE] = hmms[_i].observation;

            for (int i = 0; i < N; ++i) // initialize delta
                delta[_i][1][i] = pi[i] * b[o[1] - 'A'][i];
            for (int t = 2; t <= T; ++t) { // create delta table
                for (int j = 0; j < N; ++j) {
                    double temp_max = 0.0;
                    for (int i = 0; i < N; ++i) {
                        double temp = delta[_i][t-1][i] * a[i][j];
                        if (temp > temp_max)
                            temp_max = temp;
                    }
                    delta[_i][t][j] = temp_max * b[o[t] - 'A'][j];
                }
            }
        }

        for (int _i = 0; _i < 5; ++_i) {
            double temp = 0.0;
            for (int i = 0; i < hmms[_i].state_num; ++i)
                if (delta[_i][T][i] > temp) temp = delta[_i][T][i];
            termination[_i] = temp;
        }

        int bestModel = 1;
        double temp = termination[0];
        for (int _i = 0; _i < 5; ++_i) {
            if (termination[_i] > temp) {
                temp = termination[_i];
                bestModel = _i + 1;
            }
        }
        fprintf(fp, "model_0%d.txt\n", bestModel);
        // fprintf(fp, "%e\n", termination[bestModel]);
    }

    return 0;
}
