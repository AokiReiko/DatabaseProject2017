#include "rm.h"

bool EQ(void* a, void* b, AttrType type, int length) {
    switch (type) {
        case INT_T: 
            return (*((int*)a)) == (*((int*)b));
        case BOOL: 
            return (*((bool*)a)) == (*((bool*)b));
        case FLOAT:
            return (*((float*)a)) == (*((float*)b));
        case DOUBLE:
            return (*((double*)a)) == (*((double*)b));
        case CHAR:
            return (*((char*)a)) == (*((char*)b));
        case STRING:
            return strncmp((char*)a, (char*)b, length) == 0;
        default:
            return false;
    }
}
bool NE(void* a, void* b, AttrType type, int length) {
   switch (type) {
        case INT_T: 
            return (*((int*)a)) != (*((int*)b));
        case BOOL: 
            return (*((bool*)a)) != (*((bool*)b));
        case FLOAT:
            return (*((float*)a)) != (*((float*)b));
        case DOUBLE:
            return (*((double*)a)) != (*((double*)b));
        case CHAR:
            return (*((char*)a)) != (*((char*)b));
        case STRING:
            return strncmp((char*)a, (char*)b, length) != 0;
        default:
            return false;
    }
}
bool LT(void* a, void* b, AttrType type, int length) {
    switch (type) {
        case INT_T: 
            return (*((int*)a)) < (*((int*)b));
        case BOOL: 
            return (*((bool*)a)) < (*((bool*)b));
        case FLOAT:
            return (*((float*)a)) < (*((float*)b));
        case DOUBLE:
            return (*((double*)a)) < (*((double*)b));
        case CHAR:
            return (*((char*)a)) < (*((char*)b));
        case STRING:
            return strncmp((char*)a, (char*)b, length) < 0;
        default:
            return false;
    }
}
bool LE(void* a, void* b, AttrType type, int length) {
    switch (type) {
        case INT_T: 
            return (*((int*)a)) <= (*((int*)b));
        case BOOL: 
            return (*((bool*)a)) <= (*((bool*)b));
        case FLOAT:
            return (*((float*)a)) <= (*((float*)b));
        case DOUBLE:
            return (*((double*)a)) <= (*((double*)b));
        case CHAR:
            return (*((char*)a)) <= (*((char*)b));
        case STRING:
            return strncmp((char*)a, (char*)b, length) <= 0;
        default:
            return false;
    }
}
bool GT(void* a, void* b, AttrType type, int length) {
    switch (type) {
        case INT_T: 
            return (*((int*)a)) > (*((int*)b));
        case BOOL: 
            return (*((bool*)a)) > (*((bool*)b));
        case FLOAT:
            return (*((float*)a)) > (*((float*)b));
        case DOUBLE:
            return (*((double*)a)) > (*((double*)b));
        case CHAR:
            return (*((char*)a)) > (*((char*)b));
        case STRING:
            return strncmp((char*)a, (char*)b, length) > 0;
        default:
            return false;
    }
}
bool GE(void* a, void* b, AttrType type, int length) {
    switch (type) {
        case INT_T: 
            return (*((int*)a)) >= (*((int*)b));
        case BOOL: 
            return (*((bool*)a)) >= (*((bool*)b));
        case FLOAT:
            return (*((float*)a)) >= (*((float*)b));
        case DOUBLE:
            return (*((double*)a)) >= (*((double*)b));
        case CHAR:
            return (*((char*)a)) >= (*((char*)b));
        case STRING:
            return strncmp((char*)a, (char*)b, length) >= 0;
        default:
            return false;
    }
}

RM_FileScan::RM_FileScan() {

}
RM_FileScan::~RM_FileScan() {

}
RC RM_FileScan::OpenScan(const RM_FileHandle &fileHandle,  // Initialize file scan
                         AttrType attrType,
                         int attrLength,
                         int attrOffset,
                         CompOp compOp,
                         void *value,
                         ClientHint pinHint) 
{
    if (this->opened) return -1;
    if (fileHandle.readyForScan())
        this->fh = const_cast<RM_FileHandle*>(&fileHandle);
    else return -1;
    if (this->value != NULL) 
        free(this->value);
    this->value = malloc(attrLength);
    memcpy(this->value, value, attrLength);
    this->comp_op = compOp;
    this->type = attrType;
    this->offset = attrLength;
    this->length = attrLength;
    this->useNextPage = true;
    this->currentPage = 0;
    this->currentSlot = 0;
    this->numRecOnPage = 0;
    this->numRecScanned = 0;
    int index;
    this->buff = fh->BPM->getPage(fh->fileId,currentPage,index);

    switch (compOp) {
        case EQ_OP:
            this->comparator = &EQ;
            break;
        case NE_OP:
            this->comparator = &NE;
            break;
        case LT_OP:
            this->comparator = &LT;
            break;
        case LE_OP:
            this->comparator = &LE;
            break;
        case GT_OP:
            this->comparator = &GT;
            break;
        case GE_OP:
            this->comparator = &GE;
            break;
        default:
            return -1; 
    } 
}

RC RM_FileScan::GetNextRec(RM_Record &rec) {
    if (ended || (!opened)) return -1;
    RM_Record temp;
    while(true) {
        if (fh->GetNextRec(currentPage, currentSlot, temp, buff, useNextPage) != 0) {
            ended = true;
            return -1;
        }
           

        memcpy(&rec, &temp, sizeof(RM_Record));
        temp.rid.GetPageNum(currentPage);
        temp.rid.GetSlotNum(currentSlot);
        if (useNextPage) {
            int index;
            buff = this->fh->BPM->getPage(fh->fileId,currentPage,index);
            GetNumRecOnPage(buff,numRecOnPage);
            useNextPage = false;
            numRecScanned = 0;
            currentPage++;
        }
        numRecScanned++;
        if (numRecOnPage == numRecScanned) {
            useNextPage = true;
        }
        char *pData;
        if((temp.GetData(pData)) != 0){
            return (-1);
        }
        if ((* comparator)(pData + offset, this->value, type, length))
            break;
    }
    return 0;
}
RC RM_FileScan::CloseScan() {
  if(opened == false){
    return (-1);
  }
  opened = false;
  return (0);

}
RC RM_FileScan::GetNumRecOnPage(BufType buff, int &numRecords) {
    char* bitmap;
    RM_PageHeader* pheader;
    if (this->fh->getPageHeaderAndBitmap(buff, bitmap, pheader) != 0) return -1;
    numRecords = pheader->recordsNum;
    return 0;
}