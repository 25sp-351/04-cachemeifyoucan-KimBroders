#ifndef SEGMENTS_H
#define SEGMENTS_H
#define maxSegments 100U

typedef struct segment segment;
struct segment
{
    unsigned length;
    unsigned value;
};

extern unsigned int numSegments; // global variable stores the number of segments
extern segment segments[maxSegments]; // global variable stores the segments

unsigned int loadSegments(const char* pFilename); // loads the segments from a file, return the number of segments loaded. returns -1 on a failure

#endif // SEGMENTS_H