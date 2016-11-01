#include "hmm.h"

#include <string.h>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;

int main(int argc, char* args[]) {
    if (argc != 5) {
        printf("cmd: ./train <num of iteration> <model_init> <model_seq> <model_out>\n");
        exit(1);
    }
    const int iter_count    = atoi(args[1]);
    const char* model_init  = args[2];
    const char* model_seq   = args[3];
    const char* model_out   = args[4];

    FILE* fp_out        = open_or_die(model_out, "w");

    HMM hmm;
    loadHMM(&hmm, model_init);

    const int state_num = hmm.state_num;
    const int observ_num = hmm.observ_num;
    double (&pi)[MAX_STATE] = hmm.initial;
    double (&a)[MAX_STATE][MAX_STATE] = hmm.transition;
    double (&b)[MAX_OBSERV][MAX_STATE] = hmm.observation;

    int T = 0;
    char input[MAX_SEQ + 1];
    bool flag = true;
    FILE* fpseq = open_or_die(model_seq, "r");
    std::vector<int*> sequence;
    while (fscanf(fpseq, "%s", &input[1]) == 1) {
        if (flag) {
            flag = false;
            T = strlen(&input[1]);
        }
        int* line = new int[T + 1];
        for (int i = 1; i <= T; ++i)
            line[i] = input[i] - 'A';

        sequence.push_back(line);
    }
    fclose(fpseq);
    int seqNum = sequence.size();

    double accumGamma[T + 1][state_num];
    double accumGammaO[MAX_OBSERV][state_num];
    double accumEpsilon[T + 1][state_num][state_num];

    double alpha[T + 1][state_num];
    double beta[T + 1][state_num];
    double gamma[T + 1][state_num];
    double epsilon[T + 1][state_num][state_num];

    for (int iter_index = 0; iter_index < iter_count; ++iter_index) {
        cout << "iter_index: " << iter_index << endl;

        for (int i = 0; i < T + 1; ++i)
            for (int j = 0; j < state_num; ++j)
                accumGamma[i][j] = 0;

        for (int i = 0; i < MAX_OBSERV; ++i)
            for (int j = 0; j < state_num; ++j)
                accumGammaO[i][j] = 0;

        for (int i = 0; i < T + 1; ++i)
            for (int j = 0; j < state_num; ++j)
                for (int k = 0; k < state_num; ++k)
                    accumEpsilon[i][j][k] = 0;

        for (int seqIndex = 0; seqIndex < seqNum; ++seqIndex) {
            for (int i = 0; i < state_num; ++i)
                alpha[1][i] = pi[i] * b[sequence[seqIndex][1]][i];
            
            for (int t = 1; t < T; ++t) {
                for (int j = 0; j < state_num; ++j) {
                    double temp = 0.0;
                    for (int i = 0; i < state_num; ++i)
                        temp += alpha[t][i] * a[i][j];
                    
                    alpha[t+1][j] = temp * b[sequence[seqIndex][t+1]][j];
                }
            }

            for (int i = 0; i < state_num; ++i)
                beta[T][i] = 1.0;

            for (int t = T-1; t >= 1; --t) {
                for (int i = 0; i < state_num; ++i) {
                    double temp = 0.0;
                    for (int j = 0; j < state_num; ++j)
                        temp += a[i][j] * b[sequence[seqIndex][t+1]][j] * beta[t+1][j];
                    beta[t][i] = temp;
                }
            }

            for (int t = 1; t <= T; ++t) {
                double temp = 0.0;
                for (int i = 0; i < state_num; ++i)
                    temp += alpha[t][i] * beta[t][i];
                for (int i = 0; i < state_num; ++i)
                    gamma[t][i] = alpha[t][i] * beta[t][i] / temp;
            }

            for (int t = 1; t <= T-1; ++t) {
                double temp = 0.0;
                for (int i = 0; i < state_num; ++i)
                    for (int j = 0; j < state_num; ++j)
                        temp += alpha[t][i] * a[i][j] * b[sequence[seqIndex][t+1]][j] * beta[t+1][j];
                for (int i = 0; i < state_num; ++i)
                    for (int j = 0; j < state_num; ++j)
                        epsilon[t][i][j] = alpha[t][i] * a[i][j] * b[sequence[seqIndex][t+1]][j] * beta[t+1][j] / temp;
            }

            for (int t = 1; t <= T; ++t) {
                for (int i = 0; i < state_num; ++i) {
                    accumGamma[t][i] += gamma[t][i];
                    accumGammaO[sequence[seqIndex][t]][i] += gamma[t][i];
                }
            }
            for (int t = 1; t <= T-1; ++t)
                for (int i = 0; i < state_num; ++i)
                    for (int j = 0; j < state_num; ++j)
                        accumEpsilon[t][i][j] += epsilon[t][i][j];
        }

        for (int i = 0; i < state_num; ++i)
            pi[i] = accumGamma[1][i] / seqNum;
        
        for (int i = 0; i < state_num; ++i) {
            for (int j = 0; j < state_num; ++j) {
                double epsilon_sum = 0.0, gamma_sum = 0.0;
                for (int t = 1; t <= T-1; ++t) {
                    epsilon_sum += accumEpsilon[t][i][j];
                    gamma_sum += accumGamma[t][i];
                }
                a[i][j] = epsilon_sum / gamma_sum;
            }
        }
        for (int i = 0; i < state_num; ++i) {
            for (int k = 0; k < observ_num; ++k) {
                double gamma_sum = 0.0;
                for (int t = 1; t <= T; ++t)
                    gamma_sum += accumGamma[t][i];
                
                b[k][i] = accumGammaO[k][i] / gamma_sum;
            }
        }
    }
    for (int i = 0, length = sequence.size(); i < length; ++i)
        delete[] sequence[i];

    dumpHMM(fp_out, &hmm);

    return 0;
}
