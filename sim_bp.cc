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
    int predict = 0;


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
    while(fscanf(FP, "%lx %s", &addr, str) != EOF) {
        outcome = str[0];
        predict++;
        // if (outcome == 't')
        //     printf("%lx %s\n", addr, "t");           // Print and test if file is read correctly
        // else if (outcome == 'n')
        //     printf("%lx %s\n", addr, "n");          // Print and test if file is read correctly
        if(strcmp(params.bp_name, "bimodal") == 0)
        {

            branchPredictor.extractIndex(addr);
            if (outcome == 't')
            {
                if(branchPredictor.bimodalBranchHistoryTable[branchPredictor.bimodalIndexValue] < 2)
                {
                    branchPredictor.missPrediction++; // if counter is less than 2 then it is predicted as not taken
                }

                if(branchPredictor.bimodalBranchHistoryTable[branchPredictor.bimodalIndexValue] < 3)  // counter not saturated
                {
                    branchPredictor.bimodalBranchHistoryTable[branchPredictor.bimodalIndexValue]++;
                }
            }

            else if (outcome == 'n')
            {
                if(branchPredictor.bimodalBranchHistoryTable[branchPredictor.bimodalIndexValue] >= 2)
                {
                    branchPredictor.missPrediction++; // if counter is greater than or equal to 2 then it is predicted as not_taken
                }

                if(branchPredictor.bimodalBranchHistoryTable[branchPredictor.bimodalIndexValue] > 0)   // counter not saturated
                {
                    branchPredictor.bimodalBranchHistoryTable[branchPredictor.bimodalIndexValue]--;
                }
            }
        } else if(strcmp(params.bp_name, "gshare") == 0) {

            branchPredictor.extractIndex(addr);

            if(outcome == 't') {

                // if the counter value at the index is less than 2 predict not taken
                if(branchPredictor.gshareBranchHistoryTable[branchPredictor.gshareIndexValue] < 2) {
                    branchPredictor.missPrediction++;
                }
                // if not saturated increment the counter value
                if(branchPredictor.gshareBranchHistoryTable[branchPredictor.gshareIndexValue] < 3) {
                    branchPredictor.gshareBranchHistoryTable[branchPredictor.gshareIndexValue]++;
                }
                // Update the global branch history register
                // by right shifting 1 bit.

                branchPredictor.globalBHR >>= 1;
                // Tha actual outcome must be placed at the MSB of the BHR.
                // Here placing value 1 because actual outcome is taken.
                branchPredictor.globalBHR |= ( 1 << branchPredictor.branchHistoryRegisterBits -1);

            } else if(outcome == 'n') {

                if(branchPredictor.gshareBranchHistoryTable[branchPredictor.gshareIndexValue] >= 2) {
                    branchPredictor.missPrediction++;
                }
                if(branchPredictor.gshareBranchHistoryTable[branchPredictor.gshareIndexValue] > 0) {
                    branchPredictor.gshareBranchHistoryTable[branchPredictor.gshareIndexValue]--;
                }
                // Update the global branch history register
                // by right shifting 1 bit.

                // Tha actual outcome must be placed at the MSB of the BHR.
                // Here placing value 0 because actual outcome is not-taken.
                branchPredictor.globalBHR >>= 1;
            }
        } else if(strcmp(params.bp_name, "hybrid") == 0) {

            branchPredictor.extractIndex(addr);

            // depending on the current counter value of gshare branch history table
            // set the gshare prediction value
            if(branchPredictor.gshareBranchHistoryTable[branchPredictor.gshareIndexValue] >= 2) {
                // predict gshare as taken if greater than 2
                branchPredictor.gsharePredictionValue = 1;
            } else {
                // predict gshare as taken if less than 2
                branchPredictor.gsharePredictionValue = 0;
            }
            //cout<<"control reaching here"<<endl;


// depending on the current counter value of bimodal branch history table
            // set the bimodal prediction value
            if(branchPredictor.bimodalBranchHistoryTable[branchPredictor.bimodalIndexValue] >= 2) {
                // predict bimodal as taken if greater than 2
                branchPredictor.bimodalPredictionValue = 1;
            } else {
                // predict bimodal as not taken if BHT value is less than 2
                branchPredictor.bimodalPredictionValue = 0;
            }



            // selecting gshare branch predictor based on the chooser table value i.e if greater than 2
            // select the prediction of gshare branch predictor
            if(branchPredictor.chooserBranchHistoryTable[branchPredictor.hybridIndexValue] >= 2) {

                if( (outcome == 't') && (branchPredictor.gsharePredictionValue == 0) ||
                    (outcome == 'n') && (branchPredictor.gsharePredictionValue == 1)) {

                    branchPredictor.missPrediction++;
                    }

                if( ( outcome == 't') && branchPredictor.gshareBranchHistoryTable[branchPredictor.gshareIndexValue] < 3) {
                    branchPredictor.gshareBranchHistoryTable[branchPredictor.gshareIndexValue]++;
                }

                if( ( outcome == 'n') && branchPredictor.gshareBranchHistoryTable[branchPredictor.gshareIndexValue] > 0) {
                    branchPredictor.gshareBranchHistoryTable[branchPredictor.gshareIndexValue]--;
                }

                // selecting bimodal branch predictor based on the chooser table value i.e if not greater than
                // select the prediction of bimodal branch predictor
            } else {
                if( (outcome == 't') && (branchPredictor.bimodalPredictionValue == 0) ||
                    (outcome == 'n') && (branchPredictor.bimodalPredictionValue == 1)) {
                    branchPredictor.missPrediction++;
                    }
                if( ( outcome == 't') && branchPredictor.bimodalBranchHistoryTable[branchPredictor.bimodalIndexValue] < 3) {
                    branchPredictor.bimodalBranchHistoryTable[branchPredictor.bimodalIndexValue]++;
                }

                if( ( outcome == 'n') && branchPredictor.bimodalBranchHistoryTable[branchPredictor.bimodalIndexValue] > 0) {
                    branchPredictor.bimodalBranchHistoryTable[branchPredictor.bimodalIndexValue]--;
                }

            }

            // update global BHR

            if(outcome == 't') {
                branchPredictor.globalBHR >>= 1;
                branchPredictor.globalBHR |= ( 1 << (branchPredictor.branchHistoryRegisterBits - 1));
            }else {
                branchPredictor.globalBHR >>= 1;
            }

            if( ( ( outcome == 't' && branchPredictor.gsharePredictionValue == 1) || ( outcome == 'n' && branchPredictor.gsharePredictionValue == 0)  )
                && ( (outcome == 'n' && branchPredictor.bimodalPredictionValue == 1) || (outcome == 't' && branchPredictor.bimodalPredictionValue == 0)  ) ) {

                if(branchPredictor.chooserBranchHistoryTable[branchPredictor.hybridIndexValue] < 3) {
                    branchPredictor.chooserBranchHistoryTable[branchPredictor.hybridIndexValue]++;
                }
            }else if ( ((outcome == 'n' && branchPredictor.gsharePredictionValue == 1) || (outcome == 't' && branchPredictor.gsharePredictionValue == 0)  )
                && ( ( outcome == 't' && branchPredictor.bimodalPredictionValue == 1) || ( outcome == 'n' && branchPredictor.bimodalPredictionValue == 0)  ) ) {
                if(branchPredictor.chooserBranchHistoryTable[branchPredictor.hybridIndexValue] > 0) {
                    branchPredictor.chooserBranchHistoryTable[branchPredictor.hybridIndexValue]--;
                }

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
        for(uint32_t  i=0; i < branchPredictor.numOfbimodalBHTEntries;i++)
        {
            cout<<i<<"       "<<branchPredictor.bimodalBranchHistoryTable[i]<<endl;
        }
    }
    else if(strcmp(params.bp_name, "gshare") == 0)              //GSHARE
    {
        cout<<" FINAL GSHARE CONTENTS"<<endl;
        for(uint32_t i=0; i < branchPredictor.numOfgShareBHTEntries;i++)
        {
            cout<<i<<"       "<<branchPredictor.gshareBranchHistoryTable[i]<<endl;
        }
    }
    else if(strcmp(params.bp_name, "hybrid") == 0)              //HYBRID
    {
        cout<<" FINAL CHOOSER CONTENTS"<<endl;
        for(uint32_t i=0; i < branchPredictor.numOfchooserBHTEntries ;i++)
        {
            cout<<i<<"       "<<branchPredictor.chooserBranchHistoryTable[i]<<endl;
        }
        cout<<" FINAL GSHARE CONTENTS"<<endl;
        for(uint32_t i=0; i < branchPredictor.numOfgShareBHTEntries;i++)
        {
            cout<<i<<"       "<<branchPredictor.gshareBranchHistoryTable[i]<<endl;
        }
        cout<<" FINAL BIMODAL CONTENTS"<<endl;
        for(uint32_t i=0; i < branchPredictor.numOfbimodalBHTEntries;i++)
        {
            cout<<i<<"       "<<branchPredictor.bimodalBranchHistoryTable[i]<<endl;
        }

    }
    return 0;
}


