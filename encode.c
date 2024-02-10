
#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
	uint width, height;
	// Seek to 18th byte
	fseek(fptr_image, 18, SEEK_SET);

	// Read the width (an int)
	fread(&width, sizeof(int), 1, fptr_image);
	printf("width = %u\n", width);

	// Read the height (an int)
	fread(&height, sizeof(int), 1, fptr_image);
	printf("height = %u\n", height);

	// Return image capacity
	return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
	// Src Image file
	encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
	// Do Error handling
	if (encInfo->fptr_src_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

		return e_failure;
	}

	// Secret file
	encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
	// Do Error handling
	if (encInfo->fptr_secret == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

		return e_failure;
	}

	// Stego Image file
	encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
	// Do Error handling
	if (encInfo->fptr_stego_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

		return e_failure;
	}

	// No failure return e_success
	return e_success;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
	//checking if the 2nd cmd line argument is .bmp file
	if(argv[2] != NULL && strcmp(strstr(argv[2], "."), ".bmp") == 0)
	{
		encInfo -> src_image_fname = argv[2];
	}
	else
	{
		return e_failure;
	}
	//checking if 3rd command line argument is .txt file 
	if(argv[3] != NULL && strcmp(strstr(argv[3],"."),".txt") == 0)
	{
		encInfo -> secret_fname = argv[3];
	}
	else
	{
		return e_failure;
	}
	//storing the 4th argument to pointer
	if(argv [4] != NULL)
	{
		encInfo -> stego_image_fname = argv[4];
	}
	else
	{
		encInfo -> stego_image_fname = "stego.bmp";
	}
	return e_success;
}
//getting file size of secret file 
uint get_file_size(FILE *secret_file)
{
	//moving file pointer to end
	fseek(secret_file,0,SEEK_END);   
	return ftell(secret_file);
}
//checking if the source image is bigger than the secret file
Status check_capacity(EncodeInfo *encInfo)
{
	
	encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
	encInfo -> size_secret_file = get_file_size(encInfo->fptr_secret);
	//comparing both file size
	if(encInfo -> image_capacity > (54 + (2+4+4+4+encInfo->size_secret_file)*8))
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}
//copying header files from src image to dest image
Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_dest_image)
{
	fseek(fptr_src_image,0,SEEK_SET);
	char header[54];
	//reeding the 54 byte header from src image and writing it in dest image
	fread(header ,sizeof(char),54,fptr_src_image);
	fwrite(header,sizeof(char),54,fptr_dest_image);
	return e_success;
}
//encoding the secret data to src image
Status encode_byte_to_lsb(char data,char *image_buffer)
{
	unsigned int mask= 1<<7;
	for(int i=0;i<8;i++)
	{
		image_buffer[i]=(image_buffer[i] & 0xFE) | ((data & mask) >> (7-i));
		mask = mask>>1;
	}
	return e_success;
}
//copying the encoded bytes to stego image
Status encode_data_to_image(const char *data ,int size,FILE *fptr_src_image,FILE *fptr_stego_image,EncodeInfo *encInfo)
{
	for (int i=0;i<size;i++)
	{
		fread(encInfo->image_data,8,sizeof(char),fptr_src_image);
		encode_byte_to_lsb(data[i],encInfo->image_data);
		fwrite(encInfo->image_data,8,sizeof(char),fptr_stego_image);
	}
	return e_success;
}
//encoding the magic string in src image
Status encode_magic_string(const char *magic_string,EncodeInfo *encInfo)
{
	encode_data_to_image(magic_string,strlen(magic_string),encInfo->fptr_src_image,encInfo->fptr_stego_image,encInfo);
	return e_success;
}
Status encode_size_to_lsb(char *buffer,int size)
{
	unsigned int mask = 1<<31;
	for(int i=0;i<32;i++)
	{
		buffer[i]=(buffer[i] & 0xfe) | ((size & mask) >> (31-i));
	   mask	=mask >>1;
	}
	return e_success;
}
//encoding the magic string in src image and copying it to stego image
Status encode_size(int size,FILE *fptr_src_img,FILE *fptr_stego_img)
{
	char str[32];
	fread(str,sizeof(char),32,fptr_src_img);
	encode_size_to_lsb(str,size);
	fwrite(str,sizeof(char),32,fptr_stego_img);
	return e_success;
}
//encoding the secret file extension
Status encode_secret_file_extn(const char *file_ext,EncodeInfo *encInfo)
{
	file_ext=".txt";
	encode_data_to_image(file_ext,strlen(file_ext),encInfo->fptr_src_image,encInfo->fptr_stego_image,encInfo);
	return e_success;
}
//encoding the secret file size
Status encode_secret_file_size(int file_size,EncodeInfo *encInfo)
{
	char str[32];
	fread(str,sizeof(char),32,encInfo->fptr_src_image); 
	encode_size_to_lsb(str,file_size);
	fwrite(str,sizeof(char),32,encInfo->fptr_stego_image); 
	return e_success;  
}
//encoding the secret file data
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	char ch;
	fseek(encInfo->fptr_secret,0,SEEK_SET);
	for(int i=0;i < encInfo->size_secret_file;i++)
	{
		fread(encInfo->image_data,8,sizeof(char),encInfo->fptr_src_image);
		fread(&ch,1,sizeof(char),encInfo->fptr_secret);
        encode_byte_to_lsb(ch,encInfo->image_data);
		fwrite(encInfo->image_data,8,sizeof(char),encInfo->fptr_stego_image);
	}
	return e_success;
}
//copying the remaining data from src image to stego image
Status copy_remaining_img_data(FILE *fptr_src_img,FILE *fptr_stego_img)
{
	char ch;
	{
		while(fread(&ch ,1,1,fptr_src_img)>0)
		{
			fwrite(&ch,1,1,fptr_stego_img);
		}
		return e_success;
	}
}
Status do_encoding(EncodeInfo *encInfo)
{
	//opening all the required files
	if (open_files(encInfo) == e_success)
	{
		printf("All the files opened successfully\nstarted encoding..\n");
		printf("started encoding....\n");
		//checking if src image size is greater than secret file
		if(check_capacity(encInfo) == e_success)
		{
			printf("Can encode the secret data in image\n");
			//copying the header from source image to stego image
			if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
			{
				printf("copied the header successfully\n");
				//Encoding the magic string
				if(encode_magic_string(MAGIC_STRING,encInfo)== e_success)
				{
					printf("Successfully encoded MAGIC STRING\n");
					//encoding the size
					if(encode_size(strlen(".txt"),encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
					{
						printf("Encoded secret file extension successfully\n");
						//Encoding the file extension
						if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success)
						{
							printf("Encoded the secret file extension successfully\n");
							//Encoding the secret file size
							if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success)
							{
								printf("Encoded secret file size successfully\n");
								//encoding the secret file data
								if (encode_secret_file_data(encInfo) ==e_success)
								{
									printf("Successfully encoded the secret information\n");
									//copying the remaining data from src image to stego image
									if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
									{
										printf("copied remaining byte successfully\n");
									}
									else
									{
										printf("Not successfully copied remaining bytes\n");
										return e_failure;
									}
								}
								else
								{
									printf("Failed to encode the secret information\n");
									return e_failure;
								}
							}
							else
							{
								printf("Secret file size not encoded successfully\n");
								return e_failure;
							}
						}
									
						else 
						{
							printf("Failed to encode the secret file extension\n");
						}
					}
					else
					{
						printf("Failed:Encode secret file extension\n");
						return e_failure;
					}

				}
				else
				{
					printf("Failed to encode the magic string\n");
					return e_failure;
				}
			}
			else
			{
				printf("failure: Header not copied\n");
				return e_failure;
			}
		}
		else
		{
			printf("Failure: Encoding is not possible\n");
			return e_failure;
		}
	}
	else
	{
		printf("Failed to open the required files\n");
		return e_failure;
	}
	return e_success;
}



