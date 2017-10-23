#include "rm.h"

bool RM_FileScan::EQ(void* a, void* b, AttrType type, int length) {
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
bool RM_FileScan::NE(void* a, void* b, AttrType type, int length) {
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
bool RM_FileScan::LT(void* a, void* b, AttrType type, int length) {
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
bool RM_FileScan::LE(void* a, void* b, AttrType type, int length) {
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
bool RM_FileScan::GT(void* a, void* b, AttrType type, int length) {
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
bool RM_FileScan::GE(void* a, void* b, AttrType type, int length) {
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
    

}
RC RM_FileScan::CloseScan() {

}