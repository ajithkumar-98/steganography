
#ifndef DECODE_H
#define DECODE_H

#include "types.h"

typedef struct _DecodeInfo
{
	/* source image info */
	char *d_src_image_fname;
	FILE *d_fptr_src_image;
	int sec_file_extsize;
	int sec_file_size;
	/* destination file info */
	char *dest_txt_fname;
	FILE *fptr_dest_txt;
}DecodeInfo;
/* Read and Validate Decode args from argv */
Status read_and_validate_decode_args(char **argv, DecodeInfo *decInfo);
// funtion do decoding
Status do_decoding(DecodeInfo *);
//function to open files
Status d_open_files(DecodeInfo *);
//function to decode magic string in stego image
Status decode_magic_string(DecodeInfo *);
//function to get lsb byte
char lsb_to_byte(char *);
Status decode_sec_file_ext_size(DecodeInfo *);
int lsb_to_byte_extsize(char *);
//funciton to secret file
Status dec_sec_file_ext(DecodeInfo *);
Status dec_sec_file_size(DecodeInfo *);
Status dec_secret(DecodeInfo *);
#endif
