#ifndef SIM_BP_H
#define SIM_BP_H
#include <cstdint>
#include <iostream>
#include <vector>


using namespace std;

typedef struct bp_params{
    unsigned long int K;
    unsigned long int M1;
    unsigned long int M2;
    unsigned long int N;
    char*             bp_name;
}bp_params;

// Put additional data structures here as per your requirement


class gshare {

private:
    char *predictorName;
    char *traceFile;
    uint32_t numberOfIndexBits;

    uint32_t chooserTableIndexBits;
    uint32_t bimodalTableIndexBits;







public:
    uint32_t indexMask;
    vector<uint32_t> branchHistoryTable;
    uint32_t missPrediction;
    float_t missPredictionRate;
    uint32_t numOfBHTEntries;
    uint32_t globalBHR;
    uint32_t gShareIndex;
    uint32_t tempGlobalBHR;

    uint32_t bhrIndexBits;


    gshare() : predictorName("bimodal"), chooserTableIndexBits(0), numberOfIndexBits(0), bhrIndexBits(0), bimodalTableIndexBits(0), traceFile("unknown") {}
    void branchPredictorInit(char *predictor_name, uint32_t chooserTableIndexBits, uint32_t numberOfIndexBits, uint32_t bhrIndexBits, uint32_t bimodalTableIndexBits, char *traceFile){

        this->predictorName = predictor_name;
        this->chooserTableIndexBits = chooserTableIndexBits;
        this->numberOfIndexBits = numberOfIndexBits;
        this->bhrIndexBits = bhrIndexBits;
        this->bimodalTableIndexBits = bimodalTableIndexBits;
        this->traceFile = traceFile;

        this->indexMask = (1 << bimodalTableIndexBits) - 1;
        if(strcmp(this->predictorName,"bimodal") == 0) {
            numOfBHTEntries = pow(2,bimodalTableIndexBits);
            branchHistoryTable.resize(numOfBHTEntries, 2);
        }
        else if (strcmp(this->predictorName,"gshare") == 0) {
            numOfBHTEntries = pow(2,numberOfIndexBits);
            branchHistoryTable.resize(numOfBHTEntries, 2);
        }


    }
// sim gshare <M1> <N> <tracefile>,
// //where M1 and N are the number of PC bits and global branch history register bits used to index the gshare table, respectively.
    uint32_t extractIndex(uint32_t programCounterVlaue) {


        if(strcmp(predictorName,"bimodal") == 0 ){
            return ((programCounterVlaue >> 2) & indexMask);

        } else if (strcmp(predictorName,"gshare") == 0 ) {
            uint32_t extractedPCBits =0;
            uint32_t indexValue = 0;

            if(bhrIndexBits != 0) {
                // uppermost n bits of m bits i.e n bits of M+1 : 2 bits of PC.
                extractedPCBits =  ( (programCounterVlaue >> 2) & ( (1 << numberOfIndexBits ) - 1U) );
                tempGlobalBHR  = globalBHR << (numberOfIndexBits - bhrIndexBits);
                indexValue = tempGlobalBHR ^ extractedPCBits;
                return indexValue;
            } else {

                indexValue = ( (programCounterVlaue >> 2) & ( (1 << numberOfIndexBits ) - 1U) );
                return indexValue;
            }
        } else if (strcmp(predictorName,"hybrid") == 0 ) {

        }
    }




};

#endif
