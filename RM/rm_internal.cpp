#include "cmath"
#include "rm_internal.h"
int calcRecNumPerPage(int recordSize) {
    return floor((0.0 + PAGE_SIZE - RM_PAGEHEADER_SIZE) / (0.0 + recordSize + 1.0 / 8));
}
int calcBitmapSize(int recordSize) {
    return ceil((0.0 + recordSize) / 8.0);
}
