#include "hmm.h"

#include <string>
#include <iostream>
#include <sstream>

int main(int argc, char* args[]) {
    if (argc != 4) {
        printf("cmd: ./test <model_list> <test_data> <result>\n");
        exit(1);
    }
    const char* model_list    = args[1];
    const char* test_data     = args[2];
    const char* result        = args[3];

    FILE* fp_test       = open_or_die(test_data, "r");
    FILE* fp_result     = open_or_die(result, "w");

    HMM hmms[5];
    load_models(model_list, hmms, 5);

    char line[MAX_SEQ + 1];
    int T = 0;
    bool first = true;

    double delta[5][MAX_SEQ][MAX_STATE];
    double termination[5] = {0.0};

    while (fscanf(fp_test, "%s", &line[1]) == 1) {
        if (first) {
            T = strlen(&line[1]);
            first = false;
        }
     
        for (int modelIndex = 0; modelIndex < 5; ++modelIndex) {
            for (int i = 0; i < MAX_SEQ; ++i)
                for (int j = 0; j < MAX_STATE; ++j)
                    delta[modelIndex][i][j] = 0;

            int state_num = hmms[modelIndex].state_num;
            double (&pi)[MAX_STATE] = hmms[modelIndex].initial;
            double (&a)[MAX_STATE][MAX_STATE] = hmms[modelIndex].transition;
            double (&b)[MAX_OBSERV][MAX_STATE] = hmms[modelIndex].observation;

            for (int i = 0; i < state_num; ++i) // initialize delta
                delta[modelIndex][1][i] = pi[i] * b[line[1] - 'A'][i];
            for (int t = 2; t <= T; ++t) { // create delta table
                for (int i = 0; i < state_num; ++i) {
                    double temp_max = 0.0;
                    for (int j = 0; j < state_num; ++j) {
                        double temp = delta[modelIndex][t-1][j] * a[j][i];
                        if (temp > temp_max)
                            temp_max = temp;
                    }
                    delta[modelIndex][t][i] = temp_max * b[line[t] - 'A'][i];
                }
            }
        }

        for (int modelIndex = 0; modelIndex < 5; ++modelIndex) {
            double temp = 0.0;
            for (int i = 0; i < hmms[modelIndex].state_num; ++i)
                if (delta[modelIndex][T][i] > temp) temp = delta[modelIndex][T][i];
            termination[modelIndex] = temp;
        }

        int bestModel = 0;
        double temp = termination[0];
        for (int modelIndex = 0; modelIndex < 5; ++modelIndex) {
            if (termination[modelIndex] > temp) {
                temp = termination[modelIndex];
                bestModel = modelIndex;
            }
        }

        fprintf(fp_result, "%s %e\n", hmms[bestModel].model_name, termination[bestModel]);
    }

    return 0;
}
