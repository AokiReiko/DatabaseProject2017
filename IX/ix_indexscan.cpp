#include "ix.h"
#include <memory.h>

IX_IndexScan::IX_IndexScan(){

}                                 // Constructor

IX_IndexScan::~IX_IndexScan(){

}                                // Destructor

RC IX_IndexScan::OpenScan(const IX_IndexHandle &indexHandle, // Initialize index scan
               CompOp      compOp,
               void        *value,
               ClientHint  pinHint = NO_HINT){

}          

RC IX_IndexScan::GetNextEntry(RID &rid){

}                         // Get next matching entry

RC IX_IndexScan::CloseScan(){
    
}                                // Terminate index scan