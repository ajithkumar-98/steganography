
#include<stdio.h>
#include<string.h>
#include "decode.h"
#include "types.h"
#include"common.h"
Status read_and_validate_decode_args(char **argv, DecodeInfo *decInfo)
{
	//checking if 2nd command line argument is .bmp
	if(argv[2]!=NULL&&strcmp(strstr(argv[2],"."),".bmp")==0)
	{
		decInfo->d_src_image_fname=argv[2];
	}
	else
		return e_failure;
	//checking 3rd command line argument is .txt else store  a .txt file
	if(argv[3]!=NULL&&strcmp(strstr(argv[3],"."),".txt")==0)
	{
		decInfo->dest_txt_fname=argv[3];
	}
	else if(argv[3]==NULL)
	{
		decInfo->dest_txt_fname="reveal.txt";
	}
	return e_success;
}
Status do_decoding(DecodeInfo *decInfo)
{
	//opening the required files
	if(d_open_files(decInfo)==e_success)
	{
		printf("All the files opened successfully\n");
		printf("Started Decoding..........\n");
		//decoding the magic string
		if(decode_magic_string(decInfo)==e_success)
		{
			printf("Successfully decoded MAGIC STRING\n");
			//decoding the secret file extension
			if(decode_sec_file_ext_size(decInfo)==e_success)
			{
				printf("Successfully decoded secret file extension size\n");
				if(dec_sec_file_ext(decInfo)==e_success)
				{
					printf("Successfully decoded secret file extension\n");
					if(dec_sec_file_size(decInfo)==e_success)
					{
						printf("Successfully decoded secret file size\n");
						//decoding the secret data
						if(dec_secret(decInfo)==e_success)
						{
							printf("--------------------------------------\n");
							printf("Successfully decoded the secret file\n");
							printf("--------------------------------------\n");
						}
						else
						{
							printf("Failure: Decoding secret file\n");
						}
					}
					else
					{
						printf("Failure: Decoding secret file size\n"); 
					}
				}
				else
				{
					printf("Failure: Decoding secret file extension\n");
				}
			}
			else
			{
				printf("Failure: Decoding secret file extension size\n");
			}	
		}
		else
		{
			printf("Failure: Decoding MAGIC STRING\n");
		}
	}
	else
	{
		return e_failure;
	}
	return e_success;
}
Status d_open_files(DecodeInfo *decInfo)
{
	//opening the sourc image and secret file to store
	decInfo->d_fptr_src_image=fopen(decInfo->d_src_image_fname,"r");
	if(decInfo->d_fptr_src_image==NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n",decInfo->d_src_image_fname);
		return e_failure;
	}
	decInfo->fptr_dest_txt=fopen(decInfo->dest_txt_fname,"w");
	if(decInfo->fptr_dest_txt==NULL)
	{
		perror("fopen");
		fprintf(stderr,"ERROR: Unable to open file %s\n",decInfo->dest_txt_fname);
		return e_failure;
	}
	return e_success;
}
Status decode_magic_string(DecodeInfo *decInfo)
{
     //creating a array with size of one greater than magic string
	char byte[strlen(MAGIC_STRING)+1];
	//making the the last element as 0 in array
	byte[strlen(MAGIC_STRING)]=0;
	char byte_8[8];
	//moving the src file pointer to 54
	fseek(decInfo->d_fptr_src_image,54,SEEK_SET);
	for(int i=0;i<strlen(MAGIC_STRING);i++)
	{
		fread(byte_8,8,sizeof(char),decInfo->d_fptr_src_image);
		byte[i]=lsb_to_byte(byte_8);
	}
	//comparing if magic string and acquired string are same
	if(strcmp(byte,MAGIC_STRING)==0)
		return e_success;
	else
		return e_failure;
}
char lsb_to_byte(char *byte_8)
{
	char ch=0;
	//getting the magic string from lsb byte
	for(int i=0;i<8;i++)
	{
		ch=((byte_8[i]&0x01)<<7-i)|ch;
	}
	return ch;
}
Status decode_sec_file_ext_size(DecodeInfo *decInfo)
{
	char byte[32];
	fread(byte,32,sizeof(char),decInfo->d_fptr_src_image);
	decInfo->sec_file_extsize=lsb_to_byte_extsize(byte);
	if(decInfo->sec_file_extsize==4)
		return e_success;
	else
		return e_failure;
}
int lsb_to_byte_extsize(char *byte)
{
	int x=0;
	for(int i=0;i<32;i++)
	{
		x=x<<1;
		x=x|(byte[i]&0x01);
	
	}
	return x;
}
Status dec_sec_file_ext(DecodeInfo *decInfo)
{
	
	char byte[decInfo->sec_file_extsize+1];
	byte[decInfo->sec_file_extsize]=0;
	char byte_8[8];
	for(int i=0;i<decInfo->sec_file_extsize;i++)
	{
		fread(byte_8,8,sizeof(char),decInfo->d_fptr_src_image);
		byte[i]=lsb_to_byte(byte_8);
	}
	if(strcmp(byte,".txt")==0)
		return e_success;
	else
		return e_failure;
}
Status dec_sec_file_size (DecodeInfo *decInfo)
{
	char byte[32];
	fread(byte,32,sizeof(char),decInfo->d_fptr_src_image);
	decInfo->sec_file_size=lsb_to_byte_extsize(byte);
	return e_success;
}
Status dec_secret(DecodeInfo *decInfo)
{
	char byte[decInfo->sec_file_size];
	fopen(decInfo->dest_txt_fname,"w");
	fseek(decInfo->fptr_dest_txt,0,SEEK_SET);
	char byte_8[8];
	for(int i=0;i<decInfo->sec_file_size;i++)
	{
		fread(byte_8,8,sizeof(char),decInfo->d_fptr_src_image);
		byte[i]=lsb_to_byte(byte_8);
		fwrite(byte+i,1,sizeof(char),decInfo->fptr_dest_txt);
	}
	return e_success;
}

