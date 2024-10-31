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
    uint32_t bhrIndexBits;
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


    gshare() : predictorName("bimodal"), chooserTableIndexBits(0), numberOfIndexBits(0), bhrIndexBits(0), bimodalTableIndexBits(0), traceFile("unknown") {}
    void branchPredictorInit(char *predictor_name, uint32_t chooserTableIndexBits, uint32_t numberOfIndexBits, uint32_t bhrIndexBits, uint32_t bimodalTableIndexBits, char *traceFile){

        this->predictorName = predictor_name;
        this->chooserTableIndexBits = chooserTableIndexBits;
        this->numberOfIndexBits = numberOfIndexBits;
        this->bhrIndexBits = bhrIndexBits;
        this->bimodalTableIndexBits = bimodalTableIndexBits;
        this->traceFile = traceFile;

        this->indexMask = (1 << bimodalTableIndexBits) - 1;

        numOfBHTEntries = pow(2,bimodalTableIndexBits);

        branchHistoryTable.resize(numOfBHTEntries, 2);


    }




};

#endif
