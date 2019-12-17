#include <stdint.h>
#include <stdio.h>
#include <assert.h>


typedef void(*func)(void *);

typedef struct {
	uint32_t demo_idx;
} DemoCtx;
