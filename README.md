vbyte-plain: plain old implementation

vbyte-scalar1: generates a lookup table on the fly

vbyte-scalar2: pre-computes the lookup table with a perl script

vbyte-scalar3: uses SIMD instruction to calculate the mask

vbyte-scalar4: uses SIMD instrutions to move the data; terribly slow and
    broken, did not bother to fix it

vbyte-scalar5: same as scalar3, with a fast code path if mask == 0

I did not continue working on this branch. Performance of the most complex
solution (scalar5) is only slightly better than the plain implementation,
but still slower than maskedvbyte. 

Will focus on in-place operations instead (select/find etc). They might be
very fast with the mask/lookup table.
