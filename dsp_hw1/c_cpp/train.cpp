#include "hmm.h"

#include <string.h>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;

int main(int argc, char* argv[]) {
    int iter_count      = atoi(argv[1]);
    char* model_init    = argv[2];
    char* model_seq     = argv[3];
    char* model_out     = argv[4];
    FILE* fp = open_or_die(model_out, "w");

    HMM hmm;
    loadHMM(&hmm, model_init);

    int N = hmm.state_num;
    double (&pi)[MAX_STATE] = hmm.initial;
    double (&a)[MAX_STATE][MAX_STATE] = hmm.transition;
    double (&b)[MAX_OBSERV][MAX_STATE] = hmm.observation;

    int T = 0;
    char input[MAX_SEQ+1];
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

    double accumGamma[T + 1][N];
    double accumGammaO[MAX_OBSERV][N];
    double accumEpsilon[T + 1][N][N];

    double alpha[T + 1][N];
    double beta[T + 1][N];
    double gamma[T + 1][N];
    double epsilon[T + 1][N][N];

    for (int iter_index = 0; iter_index < iter_count; ++iter_index) {
        cout << "iter_index: " << iter_index << endl;

        for (int i = 0; i < T + 1; ++i)
            for (int j = 0; j < N; ++j)
                accumGamma[i][j] = 0;

        for (int i = 0; i < MAX_OBSERV; ++i)
            for (int j = 0; j < N; ++j)
                accumGammaO[i][j] = 0;

        for (int i = 0; i < T + 1; ++i)
            for (int j = 0; j < N; ++j)
                for (int k = 0; k < N; ++k)
                    accumEpsilon[i][j][k] = 0;

        for (int seqIndex = 0; seqIndex < seqNum; ++seqIndex) {
            for (int i = 0; i < N; ++i)
                alpha[1][i] = pi[i] * b[sequence[seqIndex][1]][i];
            
            for (int t = 1; t < T; ++t) {
                for (int j = 0; j < N; ++j) {
                    double temp = 0.0;
                    for (int i = 0; i < N; ++i)
                        temp += alpha[t][i] * a[i][j];
                    
                    alpha[t+1][j] = temp * b[sequence[seqIndex][t+1]][j];
                }
            }

            for (int i = 0; i < N; ++i)
                beta[T][i] = 1.0;

            for (int t = T-1; t >= 1; --t) {
                for (int i = 0; i < N; ++i) {
                    double temp = 0.0;
                    for (int j = 0; j < N; ++j)
                        temp += a[i][j] * b[sequence[seqIndex][t+1]][j] * beta[t+1][j];
                    beta[t][i] = temp;
                }
            }

            for (int t = 1; t <= T; ++t) {
                double temp = 0.0;
                for (int i = 0; i < N; ++i)
                    temp += alpha[t][i] * beta[t][i];
                for (int i = 0; i < N; ++i)
                    gamma[t][i] = alpha[t][i] * beta[t][i] / temp;
            }

            for (int t = 1; t <= T-1; ++t) {
                double temp = 0.0;
                for (int i = 0; i < N; ++i)
                    for (int j = 0; j < N; ++j)
                        temp += alpha[t][i] * a[i][j] * b[sequence[seqIndex][t+1]][j] * beta[t+1][j];
                for (int i = 0; i < N; ++i)
                    for (int j = 0; j < N; ++j)
                        epsilon[t][i][j] = alpha[t][i] * a[i][j] * b[sequence[seqIndex][t+1]][j] * beta[t+1][j] / temp;
            }

            for (int t = 1; t <= T; ++t) {
                for (int i = 0; i < N; ++i) {
                    accumGamma[t][i] += gamma[t][i];
                    accumGammaO[sequence[seqIndex][t]][i] += gamma[t][i];
                }
            }
            for (int t = 1; t <= T-1; ++t)
                for (int i = 0; i < N; ++i)
                    for (int j = 0; j < N; ++j)
                        accumEpsilon[t][i][j] += epsilon[t][i][j];
        }

        for (int i = 0; i < N; ++i)
            pi[i] = accumGamma[1][i] / seqNum;
        
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                double up = 0.0, down = 0.0;
                for (int t = 1; t <= T-1; ++t) {
                    up += accumEpsilon[t][i][j];
                    down += accumGamma[t][i];
                }
                a[i][j] = up / down;
            }
        }
        for (int i = 0; i < N; ++i) {
            for (int k = 0; k < hmm.observ_num; ++k) {
                double up = 0.0, down = 0.0;
                for (int t = 1; t <= T; ++t)
                    down += accumGamma[t][i];
                
                up = accumGammaO[k][i];
                b[k][i] = up / down;
            }
        }
    }
    for (int i = 0, length = sequence.size(); i < length; ++i)
        delete[] sequence[i];

    dumpHMM(fp, &hmm);
    return 0;
}
