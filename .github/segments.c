#include <stdio.h> // printf_s, scanf_s, gets
#include "segments.h"

unsigned int numSegments = 0;
segment segments[MAX_SEGMENTS] = {0};

unsigned int loadSegments(const char* pFilename) {
	// takes a filename
	// loads the segments from a file
	// return the number of segments loaded. returns -1 on a failure
    FILE* pFile = NULL;
	if (fopen_s(&pFile, pFilename, "r") != 0) {
		printf_s("Error opening file %s\n", pFilename);
		return -1;
	}

	unsigned length, value;
	char strBuffer[STR_BUFFER];
	
	while ((fgets(strBuffer, sizeof(strBuffer), pFile) != NULL) && (numSegments < MAX_SEGMENTS)) {
		if (strBuffer[0] == '#')
			continue;
		
		if (sscanf_s(strBuffer, "%u, %u", &length, &value) != 2) {
			printf_s("Error reading segment %u\n", numSegments);
			return -1;
		}
		// add length and value to collection.
		segments[numSegments].length = length;
		segments[numSegments].value = value;
		numSegments++;
	}
	fclose(pFile);

#ifdef DEBUG
	printf_s("Read %u segments.\n", numSegments);
    for (unsigned segmentIx = 0; segmentIx < numSegments; ++segmentIx) {
		printf_s("Length: %u, Value: %u\n", segments[segmentIx].length, segments[segmentIx].value);
	}
#endif
    return numSegments;
}