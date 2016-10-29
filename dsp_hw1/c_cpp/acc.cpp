#include "hmm.h"

#include <iostream>
#include <cassert>

int main(int argc, char* args[]) {
    assert(argc == 4);
    FILE* fp_myans = open_or_die(args[1], "r");
    FILE* fpans = open_or_die(args[2], "r");
    FILE* fpres = open_or_die(args[3], "w");

    char myans[MAX_SEQ];
    char ans[MAX_SEQ];

    int counter = 0.0;
    int match = 0.0;

    while (fgets(myans, MAX_SEQ, fp_myans)) {
        ++counter;
        fgets(ans, MAX_SEQ, fpans);
        if (myans[7] == ans[7])
            ++match;
    }
    double accuracy = 1.0 * match / counter;
    std::cout << "accuracy: " << accuracy << '\n';
    fprintf(fpres, "%lf", accuracy);

    return 0;
}
