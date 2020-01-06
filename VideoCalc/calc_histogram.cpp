#define _CRT_SECURE_NO_WARNINGS
#include "calc_histogram.h"

#define MAX_HIST_LEN  (256)

typedef struct {
	int32_t scale;
	int32_t denom;
	int32_t offset;
}weight_t;

static RET calc_hist(uint8_t *buf, FILE *fp, uint32_t len, vector<int> &hist)
{
	size_t read_len;
	uint32_t j;
	int32_t value;
	hist.clear();
	hist.resize(MAX_HIST_LEN);

	read_len = fread(buf, 1, len, fp);
	if (0 == read_len || read_len < len) {
		printf("fread finished!\n");
		return RET_NOK;
	}

	for (j = 0; j < len; j++) {
		value = buf[j];
		value = value < 0 ? 0 : ((value > 255) ? 255 : value);
		hist[value]++;
	}

	return RET_OK;
}

static void disp_hist(CalcCtx *ctx, FILE *fp)
{
	for (uint32_t i = 0; i < ctx->frames; i++) {
		for (uint32_t j = 0; j < 3; j++) {
			vector<int> vec = ctx->hist_org[i * 3 + j];
			FPRINT(fp, "frame %3d plane %d\n", i, j);

			for (int k = 0; k < vec.size(); k++) {
				FPRINT(fp, "pixel %3d --- %8d\n", k, vec[k]);
			}
		}
	}
}

static void scale_hist(vector<int> &hist_in, vector<int> &hist_out, weight_t &w)
{
	uint32_t weight = w.scale, denom = w.denom, offset = w.offset;
	hist_out.clear();
	hist_out.resize(hist_in.size());
	uint32_t div_offset = (denom == 0) ? 0 : 1 << (denom - 1);

	for (uint32_t i = 0; i < hist_in.size(); i++) {
		uint32_t j = ((weight * i + div_offset) >> denom) + offset;
		j = video_clip3(j, 0, 255);
		hist_out[j] += hist_in[i];
	}
}

static uint32_t calc_hist_distortion(vector<int> &vec1, vector<int> &vec2)
{
	uint32_t dist = 0;
	assert(vec1.size() == vec2.size());

	for (int i = 0; i < vec1.size(); i++) {
		dist += abs(vec1[i] - vec2[i]);
	}
	return dist;
}

static void calc_frame_distortion(CalcCtx *ctx, FILE *fp)
{
	uint32_t dist, min_dist = 0xFFFFFFFF;
	weight_t w, best_w;
	for (uint32_t i = 0; i < ctx->frames - 1; i++) {
		for (uint32_t j = 0; j < 3; j++) {
			vector<int> ref = ctx->hist_org[i * 3 + j];
			vector<int> cur = ctx->hist_org[(i + 1) * 3 + j];
			vector<int> weight;

			for (int32_t denom = 0; denom < 8; denom++) {
				for (int32_t scale = 0; scale < 128; scale++) {
					for (int32_t offset = -128; offset < 128; offset++) {
						w.denom = denom;
						w.scale = scale;
						w.offset = offset;
						
						scale_hist(ref, weight, w);
						dist = calc_hist_distortion(weight, cur);
						if (dist  < min_dist) {
							best_w = w;
							min_dist = dist;
						}
					}
				}
			}
			
			FPRINT(fp, "frame %d plane %d minscale %d mindenom %d minoff %d\n", i + 1, j, 
							best_w.scale, best_w.denom, best_w.offset);
		}
	}
}

RET calc_histogram(CalcCtx *ctx)
{
	uint32_t frame_size = ctx->width * ctx->height;
	const char *input_file = ctx->input.c_str();
	const char *output_file = ctx->output.c_str();
	uint8_t *buf;
	uint32_t j, i;
	uint32_t y_len = ctx->width * ctx->height;
	uint32_t u_len = y_len / 4;
	FILE *fp_yuv_in;
	FILE *fp_out;
	int64_t start_time = time_mdate();
	int64_t end_time;
	vector<int> hist;

	fp_yuv_in = fopen(input_file, "rb");
	if (fp_yuv_in == NULL) {
		perror("fopen input");
		return RET_NOK;
	}

	fp_out = fopen(output_file, "ab");
	if (fp_out == NULL) {
		perror("fopen org");
		return RET_NOK;
	}

	buf = (uint8_t *)malloc(ctx->width * ctx->height * 2);
	if (buf == NULL) {
		printf("malloc buf failed\n");
		return RET_NOK;
	}

	for (i = 0; i < ctx->frames; i++) {
		for (j = 0; j < 3; j++) {
			if (RET_OK != calc_hist(buf, fp_yuv_in, (j == 0) ? y_len : u_len, hist)) {
				printf("frame %d fread %s finished\n", i, (j == 0) ? "Luma" : ((j == 1) ? "Chroma U" : "Chroma V"));
				break;
			}

			ctx->hist_org.push_back(hist);
		}
	}

	assert(ctx->frames * 3 <= ctx->hist_org.size());
	//disp_hist(ctx, fp_out);
	calc_frame_distortion(ctx, fp_out);

	end_time = time_mdate();

	printf("calc frame %d elapsed %.2fs\n", ctx->frames, (float)(end_time - start_time) / 1000000);
	FPCLOSE(fp_yuv_in);
	FPCLOSE(fp_out);
	free(buf);

	return RET_OK;
}
