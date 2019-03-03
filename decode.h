#include <stdio.h>
#include <string.h>
#include "types.h"

/* Decoding function prototype */

/* Main Decoding function */
Status decode_stegno_image();

/* Decoding password */
Status dec_pass();

/* Decoding Extension */
Status decode_extension();

/* Decoding secret data */
Status decode_data();

/* Decoding size */
int decode_size();

/* Global variable declaration */
int idx,jdx;

Status decode_stegno_image(char *argv[])
{

    DecodeInfo decInfo;
    char *decoded_file;
    char *ext_check;
    int size_file;
    printf("INFO : ##  DECODING PROCEDURE STARTED ##\n");

    FILE *fptr_name_check=fopen(".image_fname.txt","r");
    fseek(fptr_name_check,0L,SEEK_END);
    int len=ftell(fptr_name_check);
    fseek(fptr_name_check,0L,SEEK_SET);
    char check_name[len];


    fread(check_name,len,1,fptr_name_check);
    check_name[len]='\0';
    fclose(fptr_name_check);

    if(strcmp(argv[2],check_name)!=0)
    {
	printf("INFO : File %s not found.Please enter correct filename\n",argv[2]);
	return e_failure;
    } 

    printf("INFO : Opening required files\n");
    printf("INFO : Opened %s\n",argv[2]);

    decInfo.fptr_encimg=fopen(argv[2],"r");

    if(strcmp(argv[3],"-p")==0)
    {
	printf("INFO : Decoding Password\n");

	char password[strlen(MAGIC_STRING)+1];

	/* Decoding password */
	dec_pass(&decInfo,password);
	if(strcmp(argv[4],password)==0)
	{

	    printf("INFO : Password Valid\n");
	    printf("INFO : DONE\n");
	} 
	else
	{
	    printf("INFO : Password Invalid\n");
	    return e_failure;
	}
    }
    else	
    {
	printf("INFO : Operation -p not provided\n");
	return e_failure;
    }

    printf("INFO : Decoding output file extention\n");

    /* Decoding extension */
    decode_extension(&decInfo);
    if(argv[5]==NULL)
    {
	decoded_file="decode.txt";
	printf("INFO : Output file not specified.Using %s as default\n",decoded_file);
    }
    else
    {
	char *check_ext=strchr(argv[5],'.');
	if(strcmp(decInfo.ext,check_ext)==0)
	    decoded_file=argv[5];
	else
	{
	    printf("INFO : Output file extension invalid\n");
	    return e_failure;
	}
    }
    printf("INFO : DONE\n");

    decInfo.fptr_decdata=fopen(decoded_file,"w+");
    printf("INFO : Opened %s file\n",decoded_file);

    printf("INFO : Done.Opened all required file\n");
    printf("INFO : Decoding %s file size\n",decoded_file);

    /* Decoding file size */
    size_file=decode_size(&decInfo);
    printf("INFO : Done\n");
    printf("INFO : Decoding %s file data\n",decoded_file);

    /* Decoding Data */
    decode_data(&decInfo,size_file);
    printf("INFO : Done\n");
    printf("INFO : ##  DECODING PROCEDURE COMPLETE  ##\n");

    fclose(decInfo.fptr_encimg);
    fclose(decInfo.fptr_decdata);
    printf("INFO : Open %s  file to view the decoded secret message\n",decoded_file);

}	

Status dec_pass(DecodeInfo *decInfo, char *password)
{
    char pass_buff[8];
    
    uchar pchar=0;
    uint  pbit=0;
    //printf("Password : ");
    fseek(decInfo->fptr_encimg,54,SEEK_SET);
    for(idx=0;idx<strlen(MAGIC_STRING);idx++)
    {
	fread(pass_buff,8,1,decInfo->fptr_encimg);
	for(jdx=0;jdx<8;jdx++)
	{
	    pbit=(pass_buff[jdx]&1)<<jdx;
	    pchar=pchar|pbit;
	}
	password[idx]=pchar;	
	pbit=0;
	pchar=0;
    }
    password[idx]='\0';
    //printf("%s",password);
}

Status decode_extension(DecodeInfo *decInfo)
{
    char ext_buff[8];
    uchar ext_char=0;
    uint ext_bit=0;
    //printf("Extension : ");
    fseek(decInfo->fptr_encimg,54+(strlen(MAGIC_STRING)*8),SEEK_SET);
    for(idx=0;idx<MAX_FILE_SUFFIX;idx++)
    {
	fread(ext_buff,8,1,decInfo->fptr_encimg);
	for(jdx=0;jdx<8;jdx++)
	{
	    ext_bit=(ext_buff[jdx]&1)<<jdx;
	    ext_char=ext_char|ext_bit;
	}
	decInfo->ext[idx]=ext_char;
	ext_char=0;
	ext_bit=0;
    }
    decInfo->ext[idx]='\0';
    //printf("%s",decInfo->ext);
}

int decode_size(DecodeInfo *decInfo)
{
    char size_buff[32];
    uint size_bit=0;
    decInfo->size=0;
    fseek(decInfo->fptr_encimg,54+((MAX_FILE_SUFFIX)*8)+(strlen(MAGIC_STRING)*8),SEEK_SET);
    fread(size_buff,32,1,decInfo->fptr_encimg);
    for(idx=0;idx<32;idx++)
    {
	size_bit=((size_buff[idx]&1)<<idx);
	decInfo->size=decInfo->size|size_bit;
    }
    //printf("Size=%d\n",decInfo->size);
    return decInfo->size;
}

Status decode_data(DecodeInfo *decInfo,int size)
{
    char secret_buff[8];
    char secret[size+1];
    uchar secret_char=0;
    uint secret_bit=0;
    fseek(decInfo->fptr_encimg,54+(sizeof(int)*8)+(strlen(MAGIC_STRING)*8)+((MAX_FILE_SUFFIX)*8),SEEK_SET);
    //printf("The secret message is : \n");
    for(idx=0;idx<size;idx++)
    {
	fread(secret_buff,8,1,decInfo->fptr_encimg);
	for(jdx=0 ; jdx<8 ; jdx++)
	{
	    secret_bit  = ( secret_buff[jdx] & 1 ) << jdx;
	    secret_char = secret_char | secret_bit;
	}
	secret[idx]=secret_char;
	fprintf(decInfo->fptr_decdata,"%c",secret_char);
	secret_bit=0;
	secret_char=0;
    }
    secret[idx]='\0';
    //printf("%s",secret);
}

