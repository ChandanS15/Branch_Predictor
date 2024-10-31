#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <math.h>
#include <vector>
#include "sim_bp.h"

using namespace std;
gshare branchPredictor;
/*  argc holds the number of command line arguments
    argv[] holds the commands themselves

    Example:-
    sim bimodal 6 gcc_trace.txt
    argc = 4
    argv[0] = "sim"
    argv[1] = "bimodal"
    argv[2] = "6"
    ... and so on
*/
int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    bp_params params;       // look at sim_bp.h header file for the the definition of struct bp_params
    char outcome;           // Variable holds branch outcome
    unsigned long int addr; // Variable holds the address read from input file

    params.M2 = 0;
    int *counter_bm;
    int M2_len;
    int K_len;
    int predict = 0;
    int *counter_gs;

    int M1_len;
    int *counter_hm;

    if (!(argc == 4 || argc == 5 || argc == 7))
    {
        printf("Error: Wrong number of inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }

    params.bp_name  = argv[1];

    // strtoul() converts char* to unsigned long. It is included in <stdlib.h>
    if(strcmp(params.bp_name, "bimodal") == 0)              // Bimodal
    {
        if(argc != 4)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M2       = strtoul(argv[2], NULL, 10);
        trace_file      = argv[3];
        printf("COMMAND\n%s %s %lu %s\n", argv[0], params.bp_name, params.M2, trace_file);

        branchPredictor.branchPredictorInit(params.bp_name, 0,0,0,params.M2, trace_file);
    }
    else if(strcmp(params.bp_name, "gshare") == 0)          // Gshare
    {
        if(argc != 5)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M1       = strtoul(argv[2], NULL, 10);
        params.N        = strtoul(argv[3], NULL, 10);
        trace_file      = argv[4];
        printf("COMMAND\n%s %s %lu %lu %s\n", argv[0], params.bp_name, params.M1, params.N, trace_file);

        branchPredictor.branchPredictorInit(params.bp_name, 0,params.M1,params.N ,0, trace_file);

    }
    else if(strcmp(params.bp_name, "hybrid") == 0)          // Hybrid
    {
        if(argc != 7)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.K        = strtoul(argv[2], NULL, 10);
        params.M1       = strtoul(argv[3], NULL, 10);
        params.N        = strtoul(argv[4], NULL, 10);
        params.M2       = strtoul(argv[5], NULL, 10);
        trace_file      = argv[6];
        printf("COMMAND\n%s %s %lu %lu %lu %lu %s\n", argv[0], params.bp_name, params.K, params.M1, params.N, params.M2, trace_file);

        branchPredictor.branchPredictorInit(params.bp_name, params.K,params.M1,params.N,params.M2, trace_file);

    }
    else
    {
        printf("Error: Wrong branch predictor name:%s\n", params.bp_name);
        exit(EXIT_FAILURE);
    }

    // Open trace_file in read mode
    FP = fopen(trace_file, "r");
    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }

    char str[2];
    while(fscanf(FP, "%lx %s", &addr, str) != EOF)
    {

        outcome = str[0];
        predict++;
        // if (outcome == 't')
        //     printf("%lx %s\n", addr, "t");           // Print and test if file is read correctly
        // else if (outcome == 'n')
        //     printf("%lx %s\n", addr, "n");          // Print and test if file is read correctly
        if(strcmp(params.bp_name, "bimodal") == 0)
        {

            uint32_t maskedIndex = branchPredictor.extractIndex(addr);
            if (outcome == 't')
            {
                if(branchPredictor.branchHistoryTable[maskedIndex] < 2)
                {
                    branchPredictor.missPrediction++; // if counter is less than 2 then it is predicted as not taken
                }

                if(branchPredictor.branchHistoryTable[maskedIndex] < 3)  // counter not saturated
                {
                    branchPredictor.branchHistoryTable[maskedIndex]++;
                }
            }

            else if (outcome == 'n')
            {
                if(branchPredictor.branchHistoryTable[maskedIndex] >= 2)
                {
                    branchPredictor.missPrediction++; // if counter is greater than or equal to 2 then it is predicted as not_taken
                }

                if(branchPredictor.branchHistoryTable[maskedIndex] > 0)   // counter not saturated
                {
                    branchPredictor.branchHistoryTable[maskedIndex]--;
                }
            }
        } else if(strcmp(params.bp_name, "gshare") == 0) {

            uint32_t indexValue = branchPredictor.extractIndex(addr);

            if(outcome == 't') {

                // if the counter value at the index is less than 2 predict not taken
                if(branchPredictor.branchHistoryTable[indexValue] < 2) {
                    branchPredictor.missPrediction++;
                }
                // if not saturated increment the counter value
                if(branchPredictor.branchHistoryTable[indexValue] < 3) {
                    branchPredictor.branchHistoryTable[indexValue]++;
                }
                // Update the global branch history register
                // by right shifting 1 bit

                branchPredictor.globalBHR >>= 1;
                // Tha actual outcome mus tbe place at the MSB
                branchPredictor.globalBHR |= ( 1 << branchPredictor.bhrIndexBits -1);

            } else if(outcome == 'n') {

                if(branchPredictor.branchHistoryTable[indexValue] >= 2) {
                    branchPredictor.missPrediction++;
                }
                if(branchPredictor.branchHistoryTable[indexValue] > 0) {
                    branchPredictor.branchHistoryTable[indexValue]--;
                }

                //
                branchPredictor.globalBHR >>= 1;
            }
        }

    }
    cout<<" OUTPUT"<<endl;
    cout<<"  number of predictions:    "<<predict<<endl;
    cout<<"  number of mispredictions: "<<branchPredictor.missPrediction <<endl;
    branchPredictor.missPredictionRate= ((float)branchPredictor.missPrediction / (float)predict)*100;
    printf("  misprediction rate:        %0.2f%% \n",branchPredictor.missPredictionRate);
    if(strcmp(params.bp_name, "bimodal") == 0)              //BIMODAL
    {
        cout<<" FINAL BIMODAL CONTENTS"<<endl;
        for(uint32_t  i=0; i < branchPredictor.numOfBHTEntries;i++)
        {
            cout<<i<<"       "<<branchPredictor.branchHistoryTable[i]<<endl;
        }
    }
    else if(strcmp(params.bp_name, "gshare") == 0)              //GSHARE
    {
        cout<<" FINAL GSHARE CONTENTS"<<endl;
        for(int i=0; i < branchPredictor.numOfBHTEntries;i++)
        {
            cout<<i<<"       "<<branchPredictor.branchHistoryTable[i]<<endl;
        }
    }
    else if(strcmp(params.bp_name, "hybrid") == 0)              //HYBRID
    {
        cout<<" FINAL CHOOSER CONTENTS"<<endl;
        for(int i=0; i < K_len;i++)
        {
            cout<<i<<"       "<<counter_hm[i]<<endl;
        }
        cout<<" FINAL GSHARE CONTENTS"<<endl;
        for(int i=0; i < M1_len;i++)
        {
            cout<<i<<"       "<<counter_gs[i]<<endl;
        }
        cout<<" FINAL BIMODAL CONTENTS"<<endl;
        for(int i=0; i < M2_len;i++)
        {
            cout<<i<<"       "<<counter_bm[i]<<endl;
        }
    }
    return 0;
}


