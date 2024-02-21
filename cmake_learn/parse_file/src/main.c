#include<stdio.h>
#include<string.h>

int main(int argc, char **argv)
{
    char input[30];
    unsigned int jpeg_offset[2000];
    unsigned int jpeg_length[2000];
    unsigned int jpeg_pos = 0;
    unsigned int idx = 0;
    unsigned int fileSize = 0;
    unsigned char *input_data = NULL;
    FILE *fp_input = NULL;
    FILE *fp_output = NULL;

    printf("argc:%d, len:%u\n", argc, strlen(argv[1]));

    strncpy(input, argv[1], strlen(argv[1]));
    input[strlen(argv[1])] = '\0';

    fp_input = fopen(input,"rb");
    if(fp_input == NULL){
	printf("fp_input open failed!\n");
	return -1;
    }

    fp_output = fopen("test.m2v", "wb");
    if(fp_output == NULL){
	printf("fp_output open failed!\n");
	return -1;
    }

    fseek(fp_input, 0L, SEEK_END);
    fileSize = ftell(fp_input);
    fseek(fp_input, 0L, SEEK_SET);
    printf("fileSize:%d\n", fileSize);

    input_data = malloc(fileSize + 100);
    if(input_data == NULL){
	printf("malloc failed!\n");
	return -1;
    }

    size_t read_size = fread(input_data, 1, fileSize, fp_input);
    printf("read_size:%u\n", read_size);

    unsigned char *data = input_data;
    size_t seq_cnt = 0;
    for(idx = 0; idx < fileSize; idx++){
        if(*(data + idx) == 0xFF && *(data + idx + 1) == 0xD8){
	    printf("idx:%u,seq_cnt:%u, find SOI!\n", idx, seq_cnt);
	    seq_cnt++;
	    jpeg_offset[jpeg_pos++] = idx;

	    if(jpeg_pos <= 1){
		printf("%s line %d, jpeg_pos less than 2!\n", __func__,__LINE__);
		jpeg_length[0] = jpeg_offset[0];
	    }else{
		jpeg_length[jpeg_pos - 1] = jpeg_offset[jpeg_pos - 1] - jpeg_offset[jpeg_pos - 2];
	    }
	}

	if(seq_cnt >= 100){
	    printf("idx:%u, find the 100th JPEG frame!\n", idx);
	    break;
	}
    }
    
    //fwrite(data + 1844967 + 614989, 1, 614989/*fileSize - idx*/, fp_output);

    printf("%s line %d, idx:%d, jpeg_pos:%d\n", __func__, __LINE__, idx, jpeg_pos);    
    for(idx = 1; idx < jpeg_pos; idx++){
	//printf("frame_%3d_length = %8d\n", idx, jpeg_length[idx]);
	printf("%8d,", jpeg_length[idx]);
	if(idx % 10 == 0){
	    printf("\n");
	}
    }


    if(input_data){
	free(input_data);
	input_data = NULL;
    }

    if(fp_input){
	fclose(fp_input);
	fp_input = NULL;
    }

    if(fp_output){
	fclose(fp_output);
	fp_output = NULL;
    }

    return 0;
}
