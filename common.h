#pragma once


#ifndef bit_clear
#define bit_clear(value, bit)   ((value) &= ~(1 << bit))
#endif

#ifndef bit_get
#define bit_get(value, bit)   ((value) & (1 << (bit)))
#endif

#ifndef bit_set
#define bit_set(value, bit)     ((value) |= (1 << bit))
#endif

#ifndef bit_toggle
#define bit_toggle(value, bit)  ((value) ^= (1 << bit))
#endif

#ifndef bit_write
#define bit_write(v, bit, out)  (out ? bit_set(v, bit) : bit_clear(v, bit))
#endif

#ifndef bv
#define bv(bit)                 (1 << bit)
#endif
