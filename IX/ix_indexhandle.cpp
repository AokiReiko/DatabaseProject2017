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
            insertAt(place,pData,rid,curPage);
            pheader->num_keys++;
            BPM->markDirty(pageIndex);
            if (pheader->num_keys > header.maxKeys_N){
                if (pageIndex == rootIndex){
                    //deal with changed root
                } else {
                    return 1;   //notify parent about overflow
                }
            }
            return 0;
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
            BufType newPage = BPM->allocPage();
            //todo: deal with page headers and pagenum
            IX_NodeHeader* cheader = (IX_NodeHeader*) childPage;
            IX_NodeHeader* nheader = (IX_NodeHeader*) newPage;
            int mid = cheader->num_keys/2;
            char* start;
            char* ptr;
            start = newPage+cheader->header.entryOffset_N;
            getPointerAt(childPage,mid+1,ptr);
            memcpy(start,ptr,calcSize(childPage,mid+1));
            nheader->num_keys = cheader->num_keys-mid-1;
            cheader->num_keys = mid;
            nheader->isLeafNode = cheader->isLeafNode;
            RID prid();  //todo: fill in the prid(leaf: new page; internal: child page)
            memcpy(ptr,&prid,sizeof(RID));
            
            //put the mid entry into node
            RID crid();  //todo: fill in the crid(point to new page)
            insertAfter(place,ptr+sizeof(RID)+1,crid,curPage);
            pheader->num_keys++;

            if (pheader->num_keys > header.maxKeys_N){
                if (pageIndex == rootIndex){
                    //deal with changed root
                    BufType newRootPage = BPM->allocPage(); 
                    newPage = BPM->allocPage();
                    //todo: deal with page headers and pagenum
                    cheader = pheader;
                    nheader = (IX_NodeHeader*) newPage;
                    mid = cheader->num_keys/2;
                    start = newPage+cheader->header.entryOffset_N;
                    getPointerAt(curPage,mid+1,ptr);
                    memcpy(start,ptr,calcSize(curPage,mid+1));
                    nheader->num_keys = cheader->num_keys-mid-1;
                    cheader->num_keys = mid;
                    nheader->isLeafNode = cheader->isLeafNode;
                    //todo: fill in the prid(leaf: new page; internal: child page)
                    memcpy(ptr,&prid,sizeof(RID));
                    
                    //put the mid entry into node
                    //todo: fill in the crid(point to new page)
                    insertAfter(-1,ptr+sizeof(RID)+1,crid,newRootPage);
                    (IX_NodeHeader*)newRootPage->num_keys++;
                    //todo: update rootIndex
                    return 0;
                } else {
                    return 1;   //notify parent about overflow
                }
            }
            return 0;
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
        int result = deleteEntryFromPage(pData, rid, childPage, childPageIndex);

        //deal with index change
        if (place != -1){
            char* p;
            getDataAt(curPage,place,p);
            if (isDataEqual(pData,p)){
                char* newIndex;
                getDataAt(childPage,0,newIndex);
                memcpy(p,newIndex,header.attr_length);
                BPM->markDirty(pageIndex);
            }
        }

        if (result == 0){
            return 0;
        } else if (result == 1){
            BPM->markDirty(pageIndex);
            //deal with underflow
            if (place != pheader->num_keys){
                int sibPageIndex;
                BufType sibPage = getPageByIndex(place+1, curPage, sibPageIndex);
                IX_NodeHeader* cheader = (IX_NodeHeader*) childPage;
                IX_NodeHeader* sheader = (IX_NodeHeader*) sibPage;
                char* sibStart;
                getPointerAt(sibPage,0,sibStart);
                char* childTail;
                getPointerAt(sibPage,cheader->num_keys,childTail);
                memcpy(childTail,sibStart,calcSize(sibPage,0));
                //todo: delete sibling page & check whether will overflow
                char* ptr;  //RID before the entry(RID<- STATUS DATA)
                memcpy(ptr+header.attr_length+1+sizeof(RID),ptr,sizeof(RID));
                removeAt(place,curPage);
                pheader->num_keys--;
                if (pheader->num_keys < header.maxKeys_N/2){
                    if (pageIndex == rootIndex){
                        //deal with changed root
                        //todo: move down the root when necessary
                        if (pheader->num_keys == 0){
                            pheader->isEmpty = true;
                        }
                        return 0;
                    } else {
                        return 1;
                    }
                } else {
                    return 0;
                }
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
    IX_NodeHeader* pheader = (IX_NodeHeader*) curPage;
    int delta = pheader->num_keys - place;
    return (delta*header.attr_length+delta+(delta+1)*sizeof(RID));
}

bool IX_IndexHandle::isDataEqual(void *pData, void* comp){
    
}

int IX_IndexHandle::insertAt(int place, void *pData, const RID &rid, BufType curPage){

    //before: R1 S1 D1 R2 S2 D2
    //after: rid 'd' pData R1 S1 D1 R2 S2 D2

    int sizeToMove = calcSize(curPage,place);
    char* placeholder;
    char* dataStored;
    char* statusStored;
    getPointerAt(curPage,place,placeholder);
    getDataAt(curPage,place,dataStored);
    getStatusAt(curPage,place,statusStored);
    char* tmp = (char*)malloc(sizeToMove);
    memcpy(tmp, placeholder, sizeToMove);
    memcpy(placeholder+header.attr_length+1+sizeof(RID), tmp, sizeToMove);
    memcpy(placeholder,&rid,sizeof(RID));
    char c = 'd';
    memcpy(statusStored,&c,1);
    memcpy(dataStored,pData,header.attr_length);
    return 0;
}

int IX_IndexHandle::removeAt(int place, BufType curPage){
    
        //before: R1 S1 D1 R2 S2 D2
        //after: R2 S2 D2
    
        int sizeToRemove = calcSize(curPage,place+1);
        char* placeholder;
        getPointerAt(curPage,place,placeholder);
        getDataAt(curPage,place,dataStored);
        getStatusAt(curPage,place,statusStored);
        char* tmp = (char*)malloc(sizeToMove);
        memcpy(tmp, placeholder+header.attr_length+1+sizeof(RID), sizeToRemove);
        memcpy(placeholder, tmp, sizeToRemove);
        return 0;
    }

int IX_IndexHandle::insertAfter(int place, void *pData, const RID &rid, BufType curPage){

    //before: R1 S1 D1 R2 S2 D2
    //after: R1 S1 D1 R2 'd' pData rid S2 D2

    place += 1;
    int sizeToMove = calcSize(curPage,place)-sizeof(RID);
    char* placeholder;
    char* dataStored;
    char* statusStored;
    getPointerAt(curPage,place,placeholder);
    getDataAt(curPage,place,dataStored);
    getStatusAt(curPage,place,statusStored);
    char* tmp = (char*)malloc(sizeToMove);
    memcpy(tmp, placeholder+sizeof(RID), sizeToMove);
    memcpy(placeholder+sizeof(RID)+header.attr_length+1+sizeof(RID), tmp, sizeToMove);
    memcpy(placeholder+sizeof(RID)+header.attr_length+1,&rid,sizeof(RID));
    char c = 'd';
    memcpy(statusStored,&c,1);
    memcpy(dataStored,pData,header.attr_length);
    return 0;
}