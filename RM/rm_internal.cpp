#include "cmath"
#include "rm_internal.h"
int calcRecNumPerPage(int recordSize) {
    return floor((0.0 + PAGE_SIZE - RM_PAGEHEADER_SIZE) / (0.0 + recordSize + 1.0 / 8));
}
