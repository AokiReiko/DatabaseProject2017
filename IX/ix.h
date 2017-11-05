//
// ix.h
//
//   Index Manager Component Interface
//

#ifndef IX_H
#define IX_H

#include "../database.h"
#include "../RM/rm.h"
#include "../filesystem/bufmanager/BufPageManager.h"
#include "../filesystem/fileio/FileManager.h"
#include "../filesystem/utils/pagedef.h"
#include <string>
#include <stdlib.h>
#include <cstring>

//entries are stored like this:
//(RID id, char status, AttrType attr)

struct IX_IndexHeader{
    AttrType attr_type; // attribute type and length
    int attr_length;

    int entryOffset_N;  // the offset from the header of the beginning of
    int entryOffset_B;  // the entry list in the Bucket and Nodes 

    int keysOffset_N;   // the offset from the header of the beginning of
                        // the keys list in the nodes
    int maxKeys_N;      // Maximum number of entries in buckets and nodes
    int maxKeys_B;

    PageNum rootPage;   // Page number associated with the root page
};

struct IX_NodeHeader{   // general header for node pages
    bool isLeafNode;  // indicator for whether it is a leaf node
    bool isEmpty;     // Whether the node contains pointers or not
    int num_keys;     // number of valid keys the node holds
  
    int firstSlotIndex; // the pointer to the beginning of the linked list of
                        // valid key/pointer slots
    int freeSlotIndex;  // the pointer to the beginning of free slots
    PageNum invalid1;
    PageNum invalid2;
};
  
struct IX_NodeHeader_I{   // header for internal pages
    bool isLeafNode;
    bool isEmpty;     // whether the node has its first page pointer
                      // or not
    int num_keys;
  
    int firstSlotIndex; 
    int freeSlotIndex;
    PageNum firstPage; // first leaf page under this internal node
    PageNum invalid2;
};
  
struct IX_NodeHeader_L{   //header for leaf pages
    bool isLeafNode;
    bool isEmpty; 
    int num_keys;
  
    int firstSlotIndex;
    int freeSlotIndex;
    PageNum nextPage; // next leaf page
    PageNum prevPage; // previous leaf page
};
  
struct IX_BucketHeader{     // header for the bucket pages
    int num_keys;         // number of entries in the bucket
    int firstSlotIndex;   // pointer to the first valid slot in the bucket
    int freeSlotIndex;    // pointer to the first free slot in bucket
    PageNum nextBucket;   // pointer to the next bucket page (in case of 
                          // overflow, i.e. lots of duplicates)
};

class IX_IndexHandle {
    friend class IX_Manager;
public:
     IX_IndexHandle  ();                             // Constructor
     ~IX_IndexHandle ();                             // Destructor
    RC InsertEntry     (void *pData, const RID &rid);  // Insert new index entry
    RC DeleteEntry     (void *pData, const RID &rid);  // Delete index entry
    RC ForcePages      ();                             // Copy index to disk

private:
    BufPageManager* BPM;
    BufType rootPage;
    int rootIndex;
    struct IX_IndexHeader header;
    int insertEntryIntoPage(void *pData, const RID &rid, BufType curPage, int pageIndex);
    int deleteEntryFromPage(void *pData, const RID &rid, BufType curPage, int pageIndex);
    RC getDataAt(BufType buf, int index, char* &p);
    RC getPointerAt(BufType buf, int index, char* &p);
    RC getStatusAt(BufType buf, int index, char* &p);
    BufType getPageByIndex(int place, BufType curPage, int &childPageIndex);
    int getPlace(void *pData, BufType curPage);
    int getExactPlace(void *pData, BufType curPage);
    int calcSize(BufType curPage,int place);
    bool isDataEqual(void *pData, void* comp);
};

class IX_IndexScan {
public:
     IX_IndexScan  ();                                 // Constructor
     ~IX_IndexScan ();                                 // Destructor
  RC OpenScan      (const IX_IndexHandle &indexHandle, // Initialize index scan
                    CompOp      compOp,
                    void        *value,
                    ClientHint  pinHint = NO_HINT);           
  RC GetNextEntry  (RID &rid);                         // Get next matching entry
  RC CloseScan     ();                                 // Terminate index scan
};

class IX_Manager {
public:
     IX_Manager   (FileManager &fm);              // Constructor
     ~IX_Manager  ();                             // Destructor
  RC CreateIndex  (const char *fileName,          // Create new index
                   int        indexNo,
                   AttrType   attrType,
                   int        attrLength);
  RC DestroyIndex (const char *fileName,          // Destroy index
                   int        indexNo);
  RC OpenIndex    (const char *fileName,          // Open index
                   int        indexNo,
                   IX_IndexHandle &indexHandle);
  RC CloseIndex   (IX_IndexHandle &indexHandle);  // Close index
};

#endif