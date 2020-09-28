#ifndef interface_interpolator_macros_h
#define interface_interpolator_macros_h

#include <KFC_Common\interpolator_macros.h>

// -------------------------
// Control dst value setter
// -------------------------
#define CONTROL_DST_VALUE_SETTER(ValueName, ValueType)                              \
    TControlParameterDstValueSetter<ValueType, TControlState##ValueName##Setter>    \

// ------------------------------------
// Float time control dst value setter
// ------------------------------------
#define FLOAT_TIME_SEGMENTED_CONTROL_VALUE_INTERPOLATOR(ValueName, ValueType, SegmentValueMapperType, szNSegments)                                  \
    FLOAT_TIME_SEGMENTED_VALUE_MAPPER_INTERPOLATOR(ValueType, CONTROL_DST_VALUE_SETTER(ValueName, ValueType), SegmentValueMapperType, szNSegments)  \

#endif // interface_interpolator_macros_h
