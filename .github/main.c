#include <stdio.h> // printf_s, scanf_s, gets
#include <string.h> // memset, memcpy
#include <stdbool.h> // bool
#include "rodcut.h"
#include "segments.h"
#include "cache.h"

int main(int argc, char*argv[])
{
	if(argc<2){
		printf_s("Usage: %s [-q] <segment lengths filename>\n", argv[0]);
		return 1;
	}
	
	bool quiet = false;
	if(argv[1][0] == '-' && argv[1][1] == 'q' && argv[1][2] == '\0'){
		quiet = true;
		if(argc<3) {
			printf_s("Usage: %s [-q] <segment lengths filename>\n", argv[0]);
			return 1;
		}
	}
	
	const char* pFilename = quiet ? argv[2]: argv[1]; // filename is argv[2] if -q is present else argv[1]

	if (loadSegments(pFilename) == -1)
		return 1;

	unsigned remainder;
	unsigned quantities[MAX_SEGMENTS];
	unsigned suppliedRodLength = 0;
	initializeCache(50);

	if(!quiet)
		printf_s("Enter rod length: ");

	unsigned value;
	char strBuffer[STR_BUFFER];
	int numScanned;
	
	while ((fgets(strBuffer, sizeof(strBuffer), stdin)) != NULL){
		numScanned = sscanf_s(strBuffer, "%u", &suppliedRodLength);

		if (numScanned != 1 || strBuffer[0] == '-')
			printf_s("Invalid input: %s\n", strBuffer);
		
		else {
			value = splitRod(suppliedRodLength, &remainder, quantities);
			
			printf_s("{\n"); // {
			printf_s("\"input_length\": %u,\n", suppliedRodLength); // "input_length": rodlength,
			printf_s("\"value\": %u,\n", value); // "value": totalValue,
			printf_s("\"remainder\": %u,\n", remainder); // "remainder": remainder,
			printf_s("\"cuts\": [\n"); // "cuts": [

			for (unsigned segmentIx = 0; segmentIx < numSegments; ++segmentIx) {
				printf_s("{ \"length\": %u, \"count\": %u,\n", segments[segmentIx].length, quantities[segmentIx]); // { "length": lengthOfPiece, "count": numToCut,
				printf_s("\"piece_value\": %u, \"value\": %u}", segments[segmentIx].value, quantities[segmentIx]*segments[segmentIx].value); // "piece_value": val, "value": totalValueForLength},
				
				if(segmentIx < numSegments-1)
					printf_s(",");
				printf_s("\n");
			}
			printf_s("] }\n"); // ] }
		}
		if(!quiet)
			printf_s("Enter rod length: ");
	}
	freeCache();
}