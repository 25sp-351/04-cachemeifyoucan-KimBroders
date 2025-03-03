#ifndef SEGMENTS_H
#define SEGMENTS_H
#define MAX_SEGMENTS 100U
#define STR_BUFFER 256

typedef struct segment segment;
struct segment
{
    unsigned length;
    unsigned value;
};

extern unsigned int numSegments; // global variable stores the number of segments
extern segment segments[MAX_SEGMENTS]; // global variable stores the segments

unsigned int loadSegments(const char* pFilename); // loads the segments from a file, return the number of segments loaded. returns -1 on a failure

#endif // SEGMENTS_H