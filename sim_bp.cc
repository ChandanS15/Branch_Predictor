#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <math.h>
#include <vector>
#include "sim_bp.h"

using namespace std;

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
    int index_bm;
    int K_len;
    int predict = 0;
    int miss_pridiction = 0;
    float miss_prediction_rate = 0;
    int *counter_gs;

    int M1_len;
    int *counter_hm;
    int index_temp;
    int index_gs;
    unsigned int glob_bhr_temp = 0;
    unsigned int glob_bhr = 0;

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
            int M2 = params.M2;
            M2_len = 1 << M2; // shift 1 left by M2 bits to get 2^M2

            index_bm = addr >> 2; // shift right by 2 positions to discard lower bits

            index_bm &= (M2_len-1);

            if (outcome == 't')
            {
                if(counter_bm[index_bm] < 2)
                {
                    miss_pridiction++; // if counter is less than 2 then it is predicted as not_taken
                }

                if(counter_bm[index_bm] < 3)  // counter not saturated
                {
                    counter_bm[index_bm]++;
                }
            }

            else if (outcome == 'n')
            {
                if(counter_bm[index_bm] >= 2)
                {
                    miss_pridiction++; // if counter is greater than or equal to 2 then it is predicted as not_taken
                }

                if(counter_bm[index_bm] > 0)   // counter not saturated
                {
                    counter_bm[index_bm]--;
                }
            }
        }

    }
    cout<<" OUTPUT"<<endl;
    cout<<"  number of predictions:    "<<predict<<endl;
    cout<<"  number of mispredictions: "<<miss_pridiction<<endl;
    miss_prediction_rate = ((float)miss_pridiction / (float)predict)*100;
    printf("  misprediction rate:        %0.2f%% \n",miss_prediction_rate);
    if(strcmp(params.bp_name, "bimodal") == 0)              //BIMODAL
    {
        cout<<" FINAL BIMODAL CONTENTS"<<endl;
        for(int i=0; i < M2_len;i++)
        {
            cout<<i<<"       "<<counter_bm[i]<<endl;
        }
    }
    else if(strcmp(params.bp_name, "gshare") == 0)              //GSHARE
    {
        cout<<" FINAL GSHARE CONTENTS"<<endl;
        for(int i=0; i < M1_len;i++)
        {
            cout<<i<<"       "<<counter_gs[i]<<endl;
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

