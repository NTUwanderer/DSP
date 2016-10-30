#include "hmm.h"

#include <iostream>
#include <cassert>

int main(int argc, char* args[]) {
    assert(argc == 4);
    const char* model_list    = args[1];
    const char* test_data     = args[2];
    const char* result        = args[3];

    FILE* fp_test       = open_or_die(test_data, "r");
    FILE* fp_result     = open_or_die(result, "w");

    // load models
    HMM hmms[5];
    load_models(model_list, hmms, 5);

    // start testing
    char o[MAX_SEQ + 1]; // line buffer
    int T = 0;
    bool first = true;

    double delta[5][MAX_SEQ][MAX_STATE];
    double termination[5] = {0.0};

    while (fscanf(fp_test, "%s", &o[1]) == 1) {
        if (first)
            T = strlen(&o[1]);
        first = false;
     
        for (int modelIndex = 0; modelIndex < 5; ++modelIndex) {
            for (int j = 0; j < MAX_SEQ; ++j)
                for (int k = 0; k < MAX_STATE; ++k)
                    delta[modelIndex][j][k] = 0;

            // alias variables
            int N = hmms[modelIndex].state_num;
            double (&pi)[MAX_STATE] = hmms[modelIndex].initial;
            double (&a)[MAX_STATE][MAX_STATE] = hmms[modelIndex].transition;
            double (&b)[MAX_OBSERV][MAX_STATE] = hmms[modelIndex].observation;

            for (int i = 0; i < N; ++i) // initialize delta
                delta[modelIndex][1][i] = pi[i] * b[o[1] - 'A'][i];
            for (int t = 2; t <= T; ++t) { // create delta table
                for (int j = 0; j < N; ++j) {
                    double temp_max = 0.0;
                    for (int i = 0; i < N; ++i) {
                        double temp = delta[modelIndex][t-1][i] * a[i][j];
                        if (temp > temp_max)
                            temp_max = temp;
                    }
                    delta[modelIndex][t][j] = temp_max * b[o[t] - 'A'][j];
                }
            }
        }

        for (int modelIndex = 0; modelIndex < 5; ++modelIndex) {
            double temp = 0.0;
            for (int i = 0; i < hmms[modelIndex].state_num; ++i)
                if (delta[modelIndex][T][i] > temp) temp = delta[modelIndex][T][i];
            termination[modelIndex] = temp;
        }

        int bestModel = 1;
        double temp = termination[0];
        for (int modelIndex = 0; modelIndex < 5; ++modelIndex) {
            if (termination[modelIndex] > temp) {
                temp = termination[modelIndex];
                bestModel = modelIndex + 1;
            }
        }
        fprintf(fp_result, "model_0%d.txt\n", bestModel);
        // fprintf(fp, "%e\n", termination[bestModel]);
    }

    return 0;
}
