#ifndef STDINT_H
#define STDINT_H

#ifndef NULL
# define NULL ((void*)0);
#endif

#ifndef __UINTPTR_T
# define __UINTPTR_T
typedef unsigned int uintptr_t;
#endif

#ifndef __SIZE_T
#define __SIZE_T
typedef unsigned int size_t;
#endif

#ifndef __UINT64_T
# define __UINT64_T
typedef unsigned long long uint64_t;
#endif

#ifndef __UINT32_T
# define __UINT32_T
typedef unsigned int uint32_t;
#endif

#ifndef __UINT16_T
# define __UINT16_T
typedef unsigned short uint16_t;
#endif

#ifndef __UINT8_T
# define __UINT8_T
typedef unsigned char uint8_t;
#endif

#endif
