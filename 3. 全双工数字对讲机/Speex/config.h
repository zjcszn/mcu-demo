
#include "FreeRTOS.h"


#if 1

extern void *pvPortCalloc( size_t nmemb, size_t size );
extern void *pvPortMalloc( size_t xWantedSize );
extern void *pvPortRealloc( void *pv, size_t size );
extern void vPortFree( void *pv );

#endif

#define speex_exit 	exit
#define malloc  		pvPortMalloc 
#define calloc  		pvPortCalloc 
#define realloc 		pvPortRealloc 
#define free    		vPortFree



#ifdef __IAR_SYSTEMS_ICC__     /* IAR Compiler */
#define inline inline
#endif

#ifdef  __CC_ARM               /* ARM Compiler */
#define inline __inline
#endif


#define EXPORT 
#define USE_KISS_FFT
#define FIXED_POINT
#define VAR_ARRAYS

#define MAX_CHARS_PER_FRAME (20/BYTES_PER_CHAR)

#define DISABLE_FLOAT_API
#define DISABLE_VBR
#define DISABLE_WIDEBAND
#define DISABLE_NOTIFICATIONS
#define DISABLE_WARNINGS
#define RELEASE

#define OVERRIDE_SPEEX_PUTC
#define OVERRIDE_SPEEX_FATAL

#define MAX_CHARS_PER_FRAME (20/BYTES_PER_CHAR)

#ifndef VAR_ARRAYS

	#define SPEEXENC_PERSIST_STACK_SIZE 5000
	#define SPEEXENC_SCRATCH_STACK_SIZE 3000
	#define SPEEXDEC_PERSIST_STACK_SIZE 2500
	#define SPEEXDEC_SCRATCH_STACK_SIZE 1000

	#define SPEEX_PERSIST_STACK_SIZE (SPEEXENC_PERSIST_STACK_SIZE + SPEEXDEC_PERSIST_STACK_SIZE)
	#define SPEEX_SCRATCH_STACK_SIZE SPEEXENC_SCRATCH_STACK_SIZE
	#define NB_ENC_STACK SPEEXENC_SCRATCH_STACK_SIZE
	#define NB_DEC_STACK SPEEXDEC_SCRATCH_STACK_SIZE

#endif

extern void _speex_fatal(const char *str, const char *file, int line);
extern void _speex_putc(int ch, void *file);
