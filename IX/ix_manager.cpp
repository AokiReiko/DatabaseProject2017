#include "ix.h"
#include <memory.h>

IX_Manager::IX_Manager(FileManager &fm){
    this->FM = fm;
}            
// Constructor
IX_Manager::~IX_Manager(){

}                             
// Destructor
RC IX_Manager::CreateIndex  (const char *fileName,          // Create new index
              int indexNo,
              AttrType attrType,
              int attrLength){
    if (fileName == NULL || attrLength <= 0) return -1;
    //判断attrLength是否合法
    int maxKeys_N = calcKeyNumPerPage_N(attrLength);
    int maxKeys_B = calcKeyNumPerPage_B(attrLength);
    if ((maxKeys_N <= 0 ) || (maxKeys_B <= 0)) {
        return -1;
    }

    bool canCreate = FM.createFile(fileName);
    if (!canCreate) return -1;
    int fileID, index;
    FM.openFile(fileName, fileID);
    BufPageManager* BPM = new BufPageManager(this->FM);
    BufType buff = BPM->allocPage(fileID, 0, index, true);

    IX_IndexHeader iheader;
    iheader.attr_type = attrType;
    iheader.entryOffset_N = IX_HEADER_SIZE_N;
    iheader.entryOffset_B = IX_HEADER_SIZE_B;
    iheader.keysOffset_N = calcKeysOffset(attrLength,maxKeys_B);
    iheader.maxKeys_N = maxKeys_N;
    iheader.maxKeys_B = maxKeys_B;
    memcpy(buff, &iheader, IX_FILEHEADER_SIZE);
    BPM->markDirty(index);
    BPM->close();
    delete BPM;
    FM->closeFile(fileID);//是否需要判断成功？

    return 0;
}

RC IX_Manager::DestroyIndex(const char *fileName, int indexNo){
    remove(fileName);
}

RC IX_Manager::OpenIndex(const char *fileName,          // Open index
              int indexNo,
              IX_IndexHandle &indexHandle){
    if (indexHandle.hasFileOpened) {
        this->CloseFile(fileHandle);
    }
    int id;
    //文件名不合法或无法打开则返回-1
    if ( fileName == NULL || (!FM->openFile(fileName, id)))
        return -1;
    indexHandle.initialize(FM, id, fileName);
}

RC IX_Manager::CloseIndex(IX_IndexHandle &indexHandle){
    if (FM->closeFile(indexHandle.fileId)) {
        indexHandle.hasFileOpened = false;
        return 0;
    }
    return -1;
}  // Close index