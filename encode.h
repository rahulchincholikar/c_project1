#include<stdio.h>
#include<string.h>
#include"types.h"

#ifndef ENCODE_H
#define ENCODE_H
int idx,jdx;
/* Encoding function prototype */

/* Check operation type (encode, decode and unsupported operations) */
OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo);

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo);

/* check capacity */
Status check_capacity(EncodeInfo *encInfo);

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Store Magic String */
Status encode_password(const char *magic_string, EncodeInfo *encInfo);

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);

/* Encode secret file size */
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo);

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Encode function, which does the real encoding */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

#endif

/* Check operation type (encode, decode and unsupported operations) */
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e")==0)
	return e_encode;
    else if(strcmp(argv[1],"-d")==0)
	return e_decode;
    else
	return e_unsupported;
}
/* check capacity */
Status check_capacity(EncodeInfo *encInfo)
{ 
    /* checking file capcity */ 
    if((encInfo->image_capacity/8)<encInfo->size_secret_file)
	return e_failure;
    else
	return e_success;
}

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(strcmp(argv[2],encInfo->src_image_fname)==0)
    {
	if(strcmp(argv[3],encInfo->secret_fname)==0)
	{
	    return e_success;
	}
	else
	{
	    return e_failure;
	}
    }
    else
    {	
	return e_failure;
    }
}

/* Get file size */
uint get_file_size(FILE *fptr)
{ 
    uint secret_size;
    char ch;

    /* checking for EOF */
    while(feof(fptr)==0) 
    {
	ch=fgetc(fptr);
    } 
    /* fpi consisit last position of character in file */
    secret_size=ftell(fptr);
    return secret_size;
}

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    fseek(fptr_src_image,0L,SEEK_SET);  //setting fpi of files to required positions
    fseek(fptr_stego_image,0L,SEEK_SET);
    char header[54];
    fread(header,54,1,fptr_src_image);  //copying a block into buffer
    fwrite(header,54,1,fptr_stego_image); //dumping buffer into file
    return e_success;
}

/* Store Magic String */
Status encode_password(const char *magic_string, EncodeInfo *encInfo)
{
   
    int val=0;
    fseek(encInfo->fptr_src_image,54L,SEEK_SET);  //setting fpi of files to required positions
    fseek(encInfo->fptr_stego_image,54L,SEEK_SET);
    for(idx=0;idx<strlen(MAGIC_STRING);idx++)
    {
	fread(encInfo->image_data,8,1,encInfo->fptr_src_image); // copying a block into buffer
	for(jdx=0;jdx<8;jdx++)
	{
	    if(*magic_string & (1<<jdx))                           //going through all bits
	    {
		encInfo->image_data[jdx] = ( encInfo->image_data[jdx] | 1);
	    }
	    else
	    {
		encInfo->image_data[jdx] = ( encInfo->image_data[jdx] & (~1));
	    }
	}
	fwrite(encInfo->image_data,8,1,encInfo->fptr_stego_image);
	magic_string++;  //jumping to next character of magic string
    }
   return e_success;
}

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
   
    for(idx=0;idx<MAX_FILE_SUFFIX;idx++)
    {
	encInfo->extn_secret_file[idx]=*file_extn++;  //storing extention into buffer
    }
    fseek(encInfo->fptr_src_image,54+(strlen(MAGIC_STRING)*8),SEEK_SET);
    fseek(encInfo->fptr_stego_image,54+(strlen(MAGIC_STRING)*8),SEEK_SET);
    for(idx=0;idx<4;idx++)   //loop to run through all characters
    {
	fread(encInfo->image_data,8,1,encInfo->fptr_src_image); //copying a block into buffer
	for(jdx=0;jdx<8;jdx++)    
	{
	    if(encInfo->extn_secret_file[idx] & (1<<jdx))  //going through all bits
		encInfo->image_data[jdx] = ( encInfo->image_data[jdx] | 1);
	    else
		encInfo->image_data[jdx] = ( encInfo->image_data[jdx] & (~1));

	}
	fwrite(encInfo->image_data,8,1,encInfo->fptr_stego_image); //dumping buffer into file
    }
   return e_success;
}


/* Encode secret file size */
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
   
    int val=0;

    char *size_byte=(char *)&file_size;
    fseek(encInfo->fptr_src_image,54+(MAX_FILE_SUFFIX*8)+(strlen(MAGIC_STRING)*8),SEEK_SET);//setting fpi of files to respective positions
    fseek(encInfo->fptr_stego_image,54+(MAX_FILE_SUFFIX*8)+(strlen(MAGIC_STRING)*8),SEEK_SET);
    for(idx=0;idx<4;idx++)   //loop to store byte by byte
    {
	fread(encInfo->image_data,8,1,encInfo->fptr_src_image); //copying block into buffer
	for(jdx=0;jdx<8;jdx++)    
	{
	    if(*size_byte & (1<<jdx))   //going through all bits
		encInfo->image_data[jdx] = ( encInfo->image_data[jdx] | 1);
	    else
		encInfo->image_data[jdx] = ( encInfo->image_data[jdx] & (~1));

	}

	size_byte++; //jumping to next byte
	fwrite(encInfo->image_data,8,1,encInfo->fptr_stego_image);  //dumping buffer into file
    }

    return e_success;
}

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{
   

    //setting fpi of files to respective possitions
    fseek(encInfo->fptr_src_image,54+(sizeof(int)*8)+(MAX_FILE_SUFFIX*8)+(strlen(MAGIC_STRING)*8),SEEK_SET);
    fseek(encInfo->fptr_stego_image,54+(sizeof(int)*8)+(MAX_FILE_SUFFIX*8)+(strlen(MAGIC_STRING)*8),SEEK_SET);
    fseek(encInfo->fptr_secret,0L,SEEK_SET);
    for(idx=0;idx<=(encInfo->size_secret_file);idx++) //looping through all characters in secret file
    {
	fread(encInfo->image_data,8,1,encInfo->fptr_src_image);//copying block into buffer
	*encInfo->secret_data=fgetc(encInfo->fptr_secret);
	encode_byte_to_lsb(*encInfo->secret_data, encInfo->image_data); //to encode bytes
	fwrite(encInfo->image_data,8,1,encInfo->fptr_stego_image);//dumpimg buffer into file
    }
   return e_success;
}

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
   
    for(jdx=0;jdx<8;jdx++)    
    {
	if(data & (1 << jdx))  //going through all the bytes
	    image_buffer[jdx] = ( image_buffer[jdx] | 1);
	else
	    image_buffer[jdx] = ( image_buffer[jdx] & (~1));
    }
   return e_success;
}

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    int count;
    while (( count = fread(&ch,1,1,fptr_src))!=0)
    {
	fwrite(&ch,1,1,fptr_dest);
    }
    return e_success;
}

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("\nwidth = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

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
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w+");
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
