//
// ix.h
//
//   Index Manager Component Interface
//

#ifndef IX_H
#define IX_H

#include "../database.h"
#include "RM/rm.h"
#include "../filesystem/bufmanager/BufPageManager.h"
#include "../filesystem/fileio/FileManager.h"
#include "../filesystem/utils/pagedef.h"
#include <string>
#include <stdlib.h>
#include <cstring>

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

class IX_IndexHandle {
public:
     IX_IndexHandle  ();                             // Constructor
     ~IX_IndexHandle ();                             // Destructor
  RC InsertEntry     (void *pData, const RID &rid);  // Insert new index entry
  RC DeleteEntry     (void *pData, const RID &rid);  // Delete index entry
  RC ForcePages      ();                             // Copy index to disk
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