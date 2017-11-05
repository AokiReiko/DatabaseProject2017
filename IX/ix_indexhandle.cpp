#include "ix.h"
#include <memory.h>

IX_IndexHandle::IX_IndexHandle(){

}

IX_IndexHandle::~IX_IndexHandle(){
    
}

RC IX_IndexHandle::InsertEntry(void *pData, const RID &rid){
    return insertEntryIntoPage(pData, rid, rootPage, rootIndex);
}

RC IX_IndexHandle::DeleteEntry(void *pData, const RID &rid){
    return deleteEntryFromPage(pData, rid, rootPage, rootIndex);
}

RC IX_IndexHandle::ForcePages(){

}

RC IX_IndexHandle::getDataAt(BufType buf, int index, char* &p){
    if (index >= header.maxKeys_N) {
        return -1;
    }
    p = (char*)buf + header.entryOffset_N + index*(header.attr_length+1+sizeof(RID))+sizeof(RID)+1;
    return 0;
}

RC IX_IndexHandle::getPointerAt(BufType buf, int index, char* &p){
    if (index >= header.maxKeys_N) {
        return -1;
    }
    p = (char*)buf + header.entryOffset_N + index*(header.attr_length+1+sizeof(RID));
    return 0;
}

RC IX_IndexHandle::getStatusAt(BufType buf, int index, char* &p){
    if (index >= header.maxKeys_N) {
        return -1;
    }
    p = (char*)buf + header.entryOffset_N + index*(header.attr_length+1+sizeof(RID))+sizeof(RID);
    return 0;
}

int IX_IndexHandle::insertEntryIntoPage(void *pData, const RID &rid, BufType curPage, int pageIndex){
    IX_NodeHeader* pheader = (IX_NodeHeader*) curPage;
    if (pheader->isLeafNode){
        int place = getPlace(pData, curPage);
        int sizeToMove = calcSize(curPage,place);
        char* placeholder;
        char* dataStored;
        char* statusStored;
        getPointerAt(curPage,place,placeholder);
        getDataAt(curPage,place,dataStored);
        getStatusAt(curPage,place,statusStored);
        if (!(isDataEqual(pData,dataStored))){
            char* tmp = (char*)malloc(sizeToMove);
            memcpy(tmp, placeholder, sizeToMove);
            memcpy(placeholder+header.attr_length+1+sizeof(RID), tmp, sizeToMove);
            memcpy(placeholder,&rid,sizeof(RID));
            char c = 'd';
            memcpy(statusStored,&c,1);
            memcpy(dataStored,pData,header.attr_length);
            pheader->num_keys++;
            BPM->markDirty(pageIndex);

            if (pheader->num_keys > header.maxKeys_N){
                return 1;   //notify parent about overflow
            } else {
                return 0;
            }
        } else if (*(statusStored) == 'd'){
            // add bucket page
        } else if (*(statusStored) == 'b'){
            // add entry into bucket page
        } else {
            return -1;
        }
    } else {
        int place = getPlace(pData, curPage);
        int childPageIndex;
        BufType childPage = getPageByIndex(place, curPage, childPageIndex);
        int result = insertEntryIntoPage(pData, rid, childPage, childPageIndex);
        if (result == 0){
            return 0;
        } else if (result == 1){
            BPM->markDirty(pageIndex);
            //deal with overflow
            if (pageIndex == rootIndex){
                //deal with changed root
            }
        }
    }

    return -1;
}

int IX_IndexHandle::deleteEntryFromPage(void *pData, const RID &rid, BufType curPage, int pageIndex){
    IX_NodeHeader* pheader = (IX_NodeHeader*) curPage;
    if (pheader->isLeafNode){
        int place = getExactPlace(pData, curPage);
        if (place == -1) {
            return -1;
        }
        int sizeToMove = calcSize(curPage,place+1);
        char* placeholder;
        char* statusStored;
        getPointerAt(curPage,place,placeholder);
        getStatusAt(curPage,place,statusStored);
        if (*(statusStored) == 'd'){
            char* tmp = (char*)malloc(sizeToMove);
            memcpy(tmp, placeholder+header.attr_length+1+sizeof(RID), sizeToMove);
            memcpy(placeholder, tmp, sizeToMove);
            BPM->markDirty(pageIndex);
            pheader->num_keys--;
            if (pheader->num_keys < header.maxKeys_N/2){
                return 1;   //notify parent about underflow
            }
            else {
                return 0;
            }
        } else if (*(statusStored) == 'b'){
            // delete entry from bucket page
        } else {
            return -1;
        }
    } else {
        int place = getPlace(pData, curPage);
        int childPageIndex;
        BufType childPage = getPageByIndex(place, curPage, childPageIndex);
        int result = insertEntryIntoPage(pData, rid, childPage, childPageIndex);
        if (result == 0){
            return 0;
        } else if (result == 1){
            BPM->markDirty(pageIndex);
            //deal with underflow
            if (pageIndex == rootIndex){
                //deal with changed root
            }
        }
    }

    return -1;
}

BufType IX_IndexHandle::getPageByIndex(int place, BufType curPage, int &childPageIndex){

}

int IX_IndexHandle::getPlace(void *pData, BufType curPage){
    
}

int IX_IndexHandle::getExactPlace(void *pData, BufType curPage){

}

int IX_IndexHandle::calcSize(BufType curPage,int place){

}

bool IX_IndexHandle::isDataEqual(void *pData, void* comp){
    
}