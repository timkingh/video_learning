#define _CRT_SECURE_NO_WARNINGS
#include "common.h"
#include "various_calc.h"


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