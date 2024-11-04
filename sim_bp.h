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
    uint32_t gshareTableIndexBits;

    uint32_t chooserTableIndexBits;
    uint32_t bimodalTableIndexBits;







public:
    uint32_t indexMask;
    vector<uint32_t> bimodalBranchHistoryTable;
    vector<uint32_t> gshareBranchHistoryTable;
    vector<uint32_t> hybridBranchHistoryTable;
    uint32_t missPrediction;
    float_t missPredictionRate;
    uint32_t numOfBHTEntries;
    uint32_t globalBHR;
    uint32_t gShareIndex;
    uint32_t tempGlobalBHR;

    uint32_t branchHistoryRegisterBits;

    uint32_t extractedPCBits =0;
    uint32_t gshareIndexValue = 0;
    uint32_t bimodalIndexValue = 0;
    uint32_t hybridIndexValue = 0;

    uint32_t bimodalPredictionValue = 0;
    uint32_t gsharePredictionValue = 0;


    //gshare() : predictorName("bimodal"), chooserTableIndexBits(0), gshareTableIndexBits(0), branchHistoryRegisterBits(0), bimodalTableIndexBits(0), traceFile("unknown") {}
    void branchPredictorInit(char *predictor_name, uint32_t chooserTableIndexBits, uint32_t gshareTableIndexBits, uint32_t branchHistoryRegisterBits, uint32_t bimodalTableIndexBits, char *traceFile){

        this->predictorName = predictor_name;
        this->chooserTableIndexBits = chooserTableIndexBits;
        this->gshareTableIndexBits = gshareTableIndexBits;
        this->branchHistoryRegisterBits = branchHistoryRegisterBits;
        this->bimodalTableIndexBits = bimodalTableIndexBits;
        this->traceFile = traceFile;

        this->indexMask = (1 << bimodalTableIndexBits) - 1;
        if(strcmp(this->predictorName,"bimodal") == 0) {
            numOfBHTEntries = pow(2,bimodalTableIndexBits);
            bimodalBranchHistoryTable.resize(numOfBHTEntries, 2);
        }
        else if (strcmp(this->predictorName,"gshare") == 0) {
            numOfBHTEntries = pow(2,gshareTableIndexBits);
            gshareBranchHistoryTable.resize(numOfBHTEntries, 2);
        }
        else if (strcmp(this->predictorName,"hybrid") == 0) {
            numOfBHTEntries = pow(2,chooserTableIndexBits);

            bimodalBranchHistoryTable.resize(numOfBHTEntries, 2);
            gshareBranchHistoryTable.resize(numOfBHTEntries, 2);
            hybridBranchHistoryTable.resize(numOfBHTEntries, 1);
        }


    }
// sim gshare <M1> <N> <tracefile>,
// //where M1 and N are the number of PC bits and global branch history register bits used to index the gshare table, respectively.
    void extractIndex(uint32_t programCounterValue) {



        if(strcmp(predictorName,"bimodal") == 0 ){

            bimodalIndexValue = ((programCounterValue >> 2) & indexMask);

        } else if (strcmp(predictorName,"gshare") == 0 ) {


            if(branchHistoryRegisterBits != 0) {
                // uppermost n bits of m bits i.e n bits of M+1 : 2 bits of PC.
                extractedPCBits =  ( (programCounterValue >> 2) & ( (1 << gshareTableIndexBits ) - 1U) );
                tempGlobalBHR  = globalBHR << (gshareTableIndexBits - branchHistoryRegisterBits);
                gshareIndexValue = tempGlobalBHR ^ extractedPCBits;
            } else {

                gshareIndexValue = ( (programCounterValue >> 2) & ( (1 << gshareTableIndexBits ) - 1U) );

            }
        } else if (strcmp(predictorName,"hybrid") == 0 ) {

            if(branchHistoryRegisterBits != 0) {
                // uppermost n bits of m bits i.e n bits of M+1 : 2 bits of PC.
                extractedPCBits =  ( (programCounterValue >> 2) & ( (1 << gshareTableIndexBits ) - 1U) );
                tempGlobalBHR  = globalBHR << (gshareTableIndexBits - branchHistoryRegisterBits);
                gshareIndexValue = tempGlobalBHR ^ extractedPCBits;
            } else {

                gshareIndexValue = ( (programCounterValue >> 2) & ( (1 << gshareTableIndexBits ) - 1U) );
            }

            bimodalIndexValue = ((programCounterValue >> 2) & indexMask);

            hybridIndexValue = (programCounterValue >> 2) & ( (1 << chooserTableIndexBits ) - 1U);

        }
    }




};

#endif
