#include "rm.h"


/*private:
    RID rid;        // record RID
    char* data;    // pointer to record data. This is stored in the
                    // record object, where its size is malloced
    int size;       // size of the malloc*/
RM_Record::RM_Record() {
    this->size = RM_INVALID_RECORD_SIZE;
}
RM_Record::~RM_Record() {
    
}
RM_Record& RM_Record::operator= (const RM_Record &record) {
    if (&record != this) {
        if (this->data != NULL) {
            delete []this->data;
        }
        this->size = record.size;
        this->data = new char(this->size);
        memcpy(this->data, record.data, this->size);
        this->rid = record.rid;
    }
}
RC RM_Record::GetData(char *&pData) const {
    // Set pData to point to the record's contents
    if (this->size == RM_INVALID_RECORD_SIZE || this->data == NULL)
        return RM_INVALID_RECORD;
    pData = this->data;
    return 0;
}
RC RM_Record::GetRid(RID &rid) const {
    // Get the record id
    if (!rid.isValidId()) return RM_INVALIDRID;
    rid = this->rid;
    return 0;
}
RC RM_Record::SetRecord (RID rec_rid, char *recData, int size) {
    if (!rec_rid.isValidId()) return RM_INVALIDRID;
    if (size <= 0) return RM_INVALID_RECORD_SIZE;
    if (recData == NULL) return RM_INVALID_RECORD;
    this->rid = rec_rid;
    this->size = size;
    if (this->data != NULL) delete[] this->data;
    this->data = new char(size);
    memcpy(this->data, recData, size);
    return 0;

}