#define _CRT_SECURE_NO_WARNINGS
#include <stdint.h>
#include <stdio.h>

typedef void (*func)(void *);

typedef struct {
	uint32_t demo_idx;
}DemoCtx;

void test000_mc_weight(void *ctx)
{
	printf("%s enter\n", __FUNCTION__);
}

int main(int argc, char **argv)
{
	DemoCtx demo_ctx;
	DemoCtx *ctx = &demo_ctx;
	uint32_t demo_idx = 0;
	func demo[] = {
		test000_mc_weight,
	};

	demo[demo_idx](ctx);

	return 0;
}