#include "rm.h"

void RM_FileHandle::setHeader(const RM_FileHeader& header) {
    this->fheader.recordSize = header.recordSize;           
    this->fheader.numRecordsPerPage = header.numRecordsPerPage;
    this->fheader.numPages = header.numPages;
    this->fheader.firstFreePage = header.firstFreePage;

    this->fheader.bitmapOffset = header.bitmapOffset;
    this->fheader.bitmapSize = header.bitmapSize;
}
void RM_FileHandle::initialize(FileManager* FM, int id, string filename) {
    //关联FM，同时申请新的BPM，初始化要打开的文件信息
    this->FM = FM;
    if (this->BPM != NULL) 
        delete this->BPM;
    this->BPM = new BufPageManager(FM);
    this->fileId = id;
    this->filename = filename;
    this->hasFileOpened = true;
    //读取文件头
    int index;
    BufType b = this->BPM->allocPage(id, 0, index);
    memcpy(&this->fheader, b, RM_FILEHEADER_SIZE);
    //如果allocPage这一页无法打开

}

 RM_FileHandle::RM_FileHandle () {
     this->hasFileOpened = false;
}// Constructor
RM_FileHandle::~RM_FileHandle () {
}
// Destructor
RC RM_FileHandle::GetRec (const RID &rid, RM_Record &rec) const {

}// Get a record
RC RM_FileHandle::InsertRec (const char *pData, RID &rid) {

}
// Insert a new record, return record id
RC RM_FileHandle::DeleteRec (const RID &rid) {

}
// Delete a record
RC RM_FileHandle::UpdateRec (const RM_Record &rec) {

}
// Update a record
RC RM_FileHandle::ForcePages (PageNum pageNum) const {

}
// Write dirty page(s)
                                                    