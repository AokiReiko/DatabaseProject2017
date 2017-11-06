#include "ix.h"
#include <memory.h>

IX_Manager::IX_Manager(FileManager &fm){

}            
// Constructor
IX_Manager::~IX_Manager(){

}                             
// Destructor
RC IX_Manager::CreateIndex  (const char *fileName,          // Create new index
              int indexNo,
              AttrType attrType,
              int attrLength){

}

RC IX_Manager::DestroyIndex(const char *fileName, int indexNo){

}

RC IX_Manager::OpenIndex(const char *fileName,          // Open index
              int indexNo,
              IX_IndexHandle &indexHandle){

}

RC IX_Manager::CloseIndex(IX_IndexHandle &indexHandle){

}  // Close index