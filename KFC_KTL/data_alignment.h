#ifndef data_alignment_h
#define data_alignment_h

#define DATA_ALIGNMENT	(sizeof(double))

#define ALIGNED_DATA_SIZE(v, t)	(((v) + (sizeof(t) - 1)) & ~(sizeof(t) - 1))

#define ALIGN_DATA_SIZE(v, t)	((v) = ALIGNED_DATA_SIZE(v, t))

#endif // data_alignment_h
