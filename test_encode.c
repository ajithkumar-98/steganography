
#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <string.h>

int main(int argc ,char **argv)
{
    if(check_operation_type(argv) == e_encode)
	{
		EncodeInfo encInfo;
		printf("Selected Encoding\n");
		if (read_and_validate_encode_args(argv,&encInfo) == e_success)
		{
			printf("Read and validating the argument is successful\n");
			if(do_encoding(&encInfo) == e_success)
			{
				printf("Completed Encoding\n");
			}
			else
			{
				printf("Failure : Encoding not done\n");
			}
		}
		else
		{
			printf("Failure : Read and validate\n");
		}
	}
	else if(check_operation_type(argv) == e_decode)
	{
        
		printf("Selected Decoding\n");
		DecodeInfo decInfo;
		if(read_and_validate_decode_args(argv,&decInfo)==e_success)
		{
			printf("Read and validating the arguments is successfull\n");
			if(do_decoding(&decInfo)==e_success)
			{
				printf("completed decoding\n");
			}
			else
			{
				printf("Decoding failed\n");
			}


		}
		else
		{
			printf("Read and validating the arguments is not successfull\n");
		}
	}
	else
	{
		printf("Invalid option\n");
		printf("Usage:\nEncoding: .a.out -e beautiful.bmp secret.txt\n");
		printf("Decoding: ./a.out -d stego.bmp\n");
	}
    return 0;
}
OperationType check_operation_type(char *argv[])
{
	if (strcmp(argv[1], "-e") == 0)
	{
		return e_encode;
	}
	else if (strcmp(argv[1], "-d") == 0)
	{
		return e_decode;
	}
	else  
	{
		return e_unsupported;
	}
	 
}
