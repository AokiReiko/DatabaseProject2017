#include "rm.h"

void RM_FileHandle::setHeader(const RM_FileHeader& header) {
    this->fheader.recordSize = header.recordSize;           
    this->fheader.numRecordsPerPage = header.numRecordsPerPage;
    this->fheader.numPages = header.numPages;
    this->fheader.firstFreePage = header.firstFreePage;

    this->fheader.bitmapOffset = header.bitmapOffset;
    this->fheader.bitmapSize = header.bitmapSize;
}
RC RM_FileHandle::initialize(FileManager* FM, int id, string filename) {
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
    //如果allocPage这一页无法打开
    if (b == NULL){
        return -1;
    }

    memcpy(&this->fheader, b, RM_FILEHEADER_SIZE);
    return 0;
}

RM_FileHandle::RM_FileHandle () {
    this->hasFileOpened = false;
}// Constructor

RM_FileHandle::~RM_FileHandle () {
    this->hasFileOpened = false;
}
// Destructor

RC RM_FileHandle::getPageHeaderAndBitmap(BufType buffer, char* &bitmap, struct RM_PageHeader* &pheader){
    pheader = (struct RM_PageHeader*)(buffer);
    bitmap = pheader+fheader.bitmapOffset;
    return 0;
}

RC RM_FileHandle::getFirstZero(char* bitmap, int recordNum, int &slot){
    for (int i=0;i<recordNum;i++){
        int _byte = i/8;
        int offset = i-8*_byte;
        if (bitmap[_byte] & (1 << offset) == 0){
            slot = i;
            return 0;
        }
    }
    return -1;
}

RC RM_FileHandle::getNextOne(char* bitmap, int recordNum, int start, int &slot){
    for (int i=start;i<recordNum;i++){
        int _byte = i/8;
        int offset = i-8*_byte;
        if ((~bitmap[_byte]) & (1 << offset) == 0){
            slot = i;
            return 0;
        }
    }
    return -1;
}

RC RM_FileHandle::setBitToOne(char* bitmap, int recordNum, int slot){
    if (slot >= recordNum) return -1;
    int _byte = slot/8;
    int offset = slot-8*_byte;
    bitmap[_byte] = bitmap[_byte] | (1 << offset);
    return 0;
}

RC RM_FileHandle::setBitToZero(char* bitmap, int recordNum, int slot){
    if (slot >= recordNum) return -1;
    int _byte = slot/8;
    int offset = slot-8*_byte;
    bitmap[_byte] = bitmap[_byte] & (~(1 << offset));
    return 0;
}

RC RM_FileHandle::checkIsOne(char* bitmap, int recordNum, int slot, bool &flag){
    if (slot >= recordNum) return -1;
    int _byte = slot/8;
    int offset = slot-8*_byte;
    bitmap[_byte] = bitmap[_byte] & (1 << offset);
    flag = (bitmap[_byte] != 0);    
    return 0;
}

RC RM_FileHandle::GetRec (const RID &rid, RM_Record &rec) const {
    if (!hasFileOpened) {
        return -1;   //todo : add unique error here
    }

    //获取page, slot
    PageNum page;
    SlotNum slot;
    rid.GetPageNum(page);
    rid.GetSlotNum(slot);

    //获取对应页数据
    BufType buffer;
    buffer = BPM->getPage(fileId,page,index);
    
    char* bitmap;
    struct RM_PageHeader* pheader;
    getPageHeaderAndBitmap(buffer, bitmap, pheader);

    //确认该条目的确存在
    bool flag;
    checkIsOne(bitmap, fheader.numRecordsPerPage, slot, flag);
    if (!flag) {
        return -1;
    }

    //输出条目
    rec.SetRecord(rid,bitmap + fheader.bitmapSize + fheader.recordSize * slot,fheader.recordSize);
    return 0;
}// Get a record

RC RM_FileHandle::InsertRec (const char *pData, RID &rid) {
    if (!hasFileOpened) {
        return -1;   //todo : add unique error here
    }

    if (pData == NULL){
        return -1;   //todo : add unique error here
    }

    //定位第一个可用页
    int index;
    BufType buffer;
    if (fheader.firstFreePage == RM_NO_FREE_PAGES){
        buffer = BPM->allocPage(fileId,fheader.numPages+1,index);
        fheader.numPages++;
        fheader.firstFreePage = fheader.numPages;
    } else {
        buffer = BPM->getPage(fileId,fheader.firstFreePage,index);
    }

    char* bitmap;
    struct RM_PageHeader* pheader;
    getPageHeaderAndBitmap(buffer, bitmap, pheader);

    //读取可用位置的bitmap
    int slot;
    getFirstZero(bitmap, fheader.numRecordsPerPage, slot);

    //添加记录
    setBitToOne(bitmap, fheader.numRecordsPerPage, slot);
    memcpy(bitmap + fheader.bitmapSize + fheader.recordSize * slot, pData, fheader.recordSize);
    pheader.recordsNum++;
    rid = RID(fheader.firstFreePage,slot);

    //判断该页是否已满
    if (pheader->recordsNum == fheader.numRecordsPerPage){
        fheader.firstFreePage = pheader->nextFreePage;
    }

    BPM->markDirty(index);
    return 0;
}
// Insert a new record, return record id

RC RM_FileHandle::DeleteRec (const RID &rid) {
    if (!hasFileOpened) {
        return -1;   //todo : add unique error here
    }

    //定位页
    int index;
    PageNum page;
    SlotNum slot;
    BufType buffer;
    rid.GetPageNum(page);
    rid.GetSlotNum(slot);
    buffer = BPM->getPage(fileId,page,index);

    char* bitmap;
    struct RM_PageHeader* pheader;
    getPageHeaderAndBitmap(buffer, bitmap, pheader);

    //确认此位置被使用
    bool flag;
    checkIsOne(bitmap, fheader.numRecordsPerPage, slot, flag);
    if (!flag) {
        return -1;
    }

    //删除记录
    if (setBitToZero(bitmap, fheader.numRecordsPerPage, slot) == -1){
        return -1;
    }
    pheader.recordsNum--;

    //判断该页是否未满
    if (pheader->recordsNum != fheader.numRecordsPerPage){
        pheader->nextFreePage = fheader.firstFreePage;
        fheader.firstFreePage = page;
    }

    BPM->markDirty(index);
    return 0;
}
// Delete a record

RC RM_FileHandle::UpdateRec (const RM_Record &rec) {
    if (!hasFileOpened) {
        return -1;   //todo : add unique error here
    }

    //获取page, slot
    RID rid;
    rec.GetRid(rid);
    PageNum page;
    SlotNum slot;
    rid.GetPageNum(page);
    rid.GetSlotNum(slot);

    //获取对应页数据
    BufType buffer;
    buffer = BPM->getPage(fileId,page,index);
    
    char* bitmap;
    struct RM_PageHeader* pheader;
    getPageHeaderAndBitmap(buffer, bitmap, pheader);

    //确认该条目的确存在
    bool flag;
    checkIsOne(bitmap, fheader.numRecordsPerPage, slot, flag);
    if (!flag) {
        return -1;
    }

    //更新条目
    char* pData;
    rec.GetData(pData);
    memcpy(bitmap + fheader.bitmapSize + fheader.recordSize * slot, pData, fheader.recordSize);

    //标记脏页
    BPM->markDirty(index);
    return 0;
}
// Update a record

RC RM_FileHandle::GetNextRec (PageNum page, SlotNum slot, RM_Record &rec, BufType &buf, bool nextPage) {
    char* bitmap;
    struct RM_PageHeader* pheader;
    int result;
    int index;
    PageNum nextRecPage = page;
    SlotNum nextRecSlot;

    if (nextPage){
        while (true){
            nextRecPage++;
            if (nextRecPage > fheader.numPages){     //todo: check whether to use > or >=
                return -1;
            }
            buf = BPM->getPage(fileId,nextRecPage,index);
            getPageHeaderAndBitmap(buf, bitmap, pheader);
            if (getNextOne(bitmap,fheader.numRecordsPerPage,0,nextRecSlot) == 0){
                break;
            }
        }
    } else {
        buf = BPM->getPage(fileId,nextRecPage,index);
        getPageHeaderAndBitmap(buf, bitmap, pheader);
        if (getNextOne(bitmap,fheader.numRecordsPerPage,0,nextRecSlot) != 0){
            return -1;
        }
    }

    RID rid(nextRecPage,nextRecSlot);
    rec.SetRecord(rid,bitmap + (fheader.bitmapSize) + nextRecSlot * fheader.recordSize, fheader.recordSize);
    return 0;
}// Get next record from given page and slot

RC RM_FileHandle::ForcePages (PageNum pageNum) const {
    if (!hasFileOpened) {
        return -1;   //todo : add unique error here
    }
    
    BPM->close();
    return 0;
}
// Write dirty page(s)
                                                    