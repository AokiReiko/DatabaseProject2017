#ifndef RM_H
#define RM_H

#include "../filesystem/bufmanager/BufPageManager.h"
#include "../filesystem/fileio/FileManager.h"
#include "../filesystem/utils/pagedef.h"
#include "rm_internal.h"
#include "../database.h"
#include <iostream>
class RM_FileHandle;
class RID;
class RM_Record;
class RM_Manager {
public:
    RM_Manager (FileManager& pfm);            // Constructor
    ~RM_Manager ();                           // Destructor
    RC CreateFile (const char *fileName, int recordSize);  
                                                // Create a new file
    RC DestroyFile (const char *fileName);       // Destroy a file
    RC OpenFile (const char *fileName, RM_FileHandle &fileHandle);
                                                // Open a file
    RC CloseFile (RM_FileHandle &fileHandle);  // Close a file
private:
    BufPageManager* BPM;
    FileManager* FM;
};

class RM_FileHandle {
public:
    RM_FileHandle ();                                  // Constructor
    ~RM_FileHandle ();                                  // Destructor
    RC GetRec (const RID &rid, RM_Record &rec) const;
                                                           // Get a record
    RC InsertRec (const char *pData, RID &rid);       // Insert a new record,
                                                           //   return record id
    RC DeleteRec (const RID &rid);                    // Delete a record
    RC UpdateRec (const RM_Record &rec);              // Update a record
    RC ForcePages (PageNum pageNum = ALL_PAGES) const; // Write dirty page(s)
                                                           //   to disk
};
class RM_FileScan {
public:
    RM_FileScan  ();                                // Constructor
    ~RM_FileScan ();                                // Destructor
    RC OpenScan (const RM_FileHandle &fileHandle,  // Initialize file scan
                    AttrType      attrType,
                    int           attrLength,
                    int           attrOffset,
                    CompOp        compOp,
                    void          *value,
                    ClientHint    pinHint = NO_HINT);
    RC GetNextRec   (RM_Record &rec);                  // Get next matching record
    RC CloseScan    ();                                // Terminate file scan
};
class RID {
public:
    RID ();                        // Default constructor
    ~RID ();                        // Destructor
    RID (PageNum pageNum, SlotNum slotNum);
                                             // Construct RID from page and
                                             //   slot number
    RC GetPageNum (PageNum &pageNum) const;  // Return page number
    RC GetSlotNum (SlotNum &slotNum) const;  // Return slot number

private:
    PageNum pageNum;
    SlotNum sloyNum;
};
class RM_Record {
public:
    RM_Record  ();                     // Constructor
    ~RM_Record ();                     // Destructor

    RM_Record& operator= (const RM_Record &record);
    RC GetData    (char *&pData) const;   // Set pData to point to
                                        //   the record's contents
    RC GetRid     (RID &rid) const;       // Get the record id
    // Sets the record with an RID, data contents, and its size
    RC SetRecord (RID rec_rid, char *recData, int size);
private:
    RID rid;        // record RID
    char* data;    // pointer to record data. This is stored in the
                    // record object, where its size is malloced
    int size;       // size of the malloc
};

#endif RM_H