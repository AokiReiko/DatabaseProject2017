#include "rm.h"
#include "rm_internal.h"

RID::RID() {
    //有效的pagenum为正数，slotnum非负
    this->pageNum = -1;
    this->slotNum = -1;
}// Default constructor
RID::~RID() {

}// Destructor
RID::RID(PageNum pageNum, SlotNum slotNum) {
    this->pageNum = pageNum;
    this->slotNum = slotNum;
}
RC RID::GetPageNum (PageNum &pageNum) const {
    pageNum = this->pageNum;
    return 0;
}  // Return page number
RC RID::GetSlotNum (SlotNum &slotNum) const {
    slotNum = this->slotNum;
    return 0;
}
bool RID::isValidId() {
    return (pageNum >= 0 && slotNum >= 0);
}