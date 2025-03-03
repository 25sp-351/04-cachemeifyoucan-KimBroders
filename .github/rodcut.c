#include <string.h> // memset, memcpy
#include "rodcut.h"
#include "segments.h"
#include "cache.h"

unsigned splitRod(const unsigned rodLength, unsigned *pRemainder, unsigned quantities[]) {
	// takes a rodLength, a pointer to an unsigned int pRemainder, and an array of unsigned ints quantities
	// split the Rod; returns total value; also update *pRemainder with remainder and cuts with number of cuts of each size
	// recursive implementation; try each cut that will fit and recurse to cut the remainder.  Select the highest value cut…
	// returns total value of the split rod
    *pRemainder = rodLength;
    if (rodLength == 0) {
        return 0;
    }
	unsigned maxValue = getFromCache(rodLength, pRemainder, quantities);
    if (maxValue != -1) {
        return maxValue;
    }

    maxValue = 0;

	memset(quantities, 0, sizeof(unsigned) * numSegments);
	for (unsigned segmentIx = 0U; segmentIx < numSegments; ++segmentIx) {
		if (rodLength >= segments[segmentIx].length) {
            unsigned remainder;
			unsigned subQuantities[MAX_SEGMENTS];

			memset(subQuantities, 0, sizeof(unsigned) * numSegments); // clear out the interesting part of the array
			unsigned value = segments[segmentIx].value + splitRod(rodLength - segments[segmentIx].length, &remainder, subQuantities);

			if (value > maxValue) {
				maxValue = value;
				*pRemainder = remainder;
				memcpy(quantities, subQuantities, sizeof(unsigned) * numSegments);
				quantities[segmentIx] += 1;
			}
		}
	}
    putInCache(rodLength, *pRemainder, quantities, maxValue);
	return maxValue;
}