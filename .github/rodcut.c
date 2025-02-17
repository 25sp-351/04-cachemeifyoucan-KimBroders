#include <string.h> // memset, memcpy
#include "rodcut.h"
#include "segments.h"
#include "cache.h"

// split the Rod; returns total value; also update *pRem with remainder and cuts with number of cuts of each size
// recursive implementation; try each cut that will fit and recurse to cut the remainder.  Select the highest value cut…
unsigned splitRod(const unsigned rodLength, unsigned *pRem, unsigned quantities[])
{
    *pRem = rodLength;
    if (rodLength == 0) {
        return 0;
    }
	unsigned maxValue = getFromCache(rodLength, pRem, quantities);
    if (maxValue != -1) {
        return maxValue;
    }

    maxValue = 0;

	memset(quantities, 0, sizeof(unsigned) * numSegments);
	for (unsigned seg = 0U; seg < numSegments; ++seg) {
		if (rodLength >= segments[seg].length) {
            unsigned rem;
			unsigned subQuantities[maxSegments];
			memset(subQuantities, 0, sizeof(unsigned) * numSegments); // clear out the interesting part of the array
			unsigned value = segments[seg].value + splitRod(rodLength - segments[seg].length, &rem, subQuantities);
			if (value > maxValue) {
				maxValue = value;
				*pRem = rem;
				memcpy(quantities, subQuantities, sizeof(unsigned) * numSegments);
				quantities[seg] += 1;
			}
		}
	}
    putInCache(rodLength, *pRem, quantities, maxValue);
	return maxValue;
}