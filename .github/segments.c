#include <stdio.h> // printf_s, scanf_s, gets
#include "segments.h"

unsigned int numSegments = 0;
segment segments[maxSegments] = {0};


unsigned int loadSegments(const char* pFilename) {
    FILE* pFile = NULL;
	if (fopen_s(&pFile, pFilename, "r") != 0)
	{
		printf_s("Error opening file %s\n", pFilename);
		return -1;
	}

	unsigned length, value;
	char strBuffer[256];
	
	while ((fgets(strBuffer, sizeof(strBuffer), pFile) != NULL) && (numSegments < maxSegments))
	{
		if (strBuffer[0]=='#'){
			continue;
		}
		if (sscanf_s(strBuffer, "%u, %u", &length, &value) != 2)
		{
			printf_s("Error reading segment %u\n", numSegments);
			return -1;
		}
		// add length and value to collection.  Fixed size array?  Linked list?
		segments[numSegments].length = length;
		segments[numSegments].value = value;
		numSegments++;
	}
	fclose(pFile);

#ifdef DEBUG
	printf_s("Read %u segments.\n", numSegments);
    for (unsigned seg = 0; seg < numSegments; ++seg)
	{
		printf_s("Length: %u, Value: %u\n", segments[seg].length, segments[seg].value);
	}
#endif
    return numSegments;
}