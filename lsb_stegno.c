#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc,char *argv[])
{
    EncodeInfo encInfo;
    uint img_size;
    encInfo.src_image_fname = "beautiful.bmp";
    encInfo.secret_fname = "secret.txt";

    if(argc<4)
    {
	if(argc==1)
	{	
	    printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> <.txt file> [output file]\n");
	    printf("./lsb_steg: Decoding: ./lsb_steg -d <.bmp file> -p password [output file]\n");

	    return e_failure;
	}
	else if(argc==2 && strcmp(argv[1],"-e")==0)
	{
	    printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> <.txt file> [output file]\n");
	    return e_failure;
	}
	else if(argc==2 && strcmp(argv[1],"-d")==0)
	{
	    printf("./lsb_steg: Decoding: ./lsb_steg -d <.bmp file> -p password [output file]\n");
	    return e_failure;
	}
	else if(argc==3 && strcmp(argv[1],"-e")==0)
	{
	    printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> <.txt file> [output file]\n");
	    return e_failure;
	}
	else if(argc==3 && strcmp(argv[1],"-d")==0)
	{
	    printf("./lsb_steg: Decoding: ./lsb_steg -d <.bmp file> -p password [output file]\n");
	    return e_failure;
	}

	else
	    return e_failure;	
    }

    if(check_operation_type(argv)==e_encode)
    {

	/* output image file name validation */
	if(argc==4)      
	{
	    encInfo.stego_image_fname = "stego_img.bmp";
	    printf("INFO : Output file not mentioned.Creating %s as default\n",encInfo.stego_image_fname);
	}
	else if(argc==5)
	{
	    /* Userdefined file name */
	    encInfo.stego_image_fname = argv[4];  
	}
	printf("INFO : Opening required file\n");

	/* validation of files */
	if(read_and_validate_encode_args(argv,&encInfo) == e_success)
	    printf("INFO : File names validated\n");
	else
	{
	    printf("INFO : File names invalid\n");
	    return e_failure;
	}

	FILE *fptr_file_name=fopen(".image_fname.txt","w+");
	fprintf(fptr_file_name,"%s",encInfo.stego_image_fname);
	fclose(fptr_file_name);

	/* Opening required files */
	if (open_files(&encInfo) == e_failure)
	{
	    printf("INFO : ERROR: %s function failed\n", "open_files" );
	    return e_failure;
	}
	else
	{
	    printf("INFO : SUCCESS: %s function completed\n", "open_files" );
	    printf("INFO : Opened %s file\n",encInfo.src_image_fname);
	    printf("INFO : Opened %s file\n",encInfo.secret_fname);
	    printf("INFO : Opened %s file\n",encInfo.stego_image_fname);
	    done
	}

	printf("INFO : ##  ENCODING PROCEDURE STARTED  ##\n");

	/* Get Image size of image */
	img_size = get_image_size_for_bmp(encInfo.fptr_src_image);
	encInfo.image_capacity=img_size;

	/* Copy bmp image header to new blank image file */
	printf("INFO : COPYPING IMAGE HEADER \n");
	copy_bmp_header(encInfo.fptr_src_image,encInfo.fptr_stego_image);
	done

	    /* Get file size */
	    printf("INFO : checking secret.txt file size \n");
	encInfo.size_secret_file=get_file_size(encInfo.fptr_secret);

	/* Secret file validaton */
	if(encInfo.size_secret_file != 0)
	{
	    printf("INFO : DONE. NOT EMPTY\n");
	}
	else
	{
	    printf("INFO : File is empty\n");
	    return e_failure;
	}

	printf("INFO : Checking for %s capacity to handle %s \n",encInfo.src_image_fname,encInfo.secret_fname);


	/* check for capactity to encode secret file data */
	if(check_capacity(&encInfo)==e_failure)   
	{
	    printf("INFO : Size of Secret file is More\n");
	    return e_failure;  
	}
	else
	    printf("INFO : DONE. FOUND OK\n");

	printf("INFO : Encoding Magic String Signature\n");

	/* Encoding password into an Image*/
	encode_password(MAGIC_STRING,&encInfo); 
	printf("INFO : Done\n");
	printf("INFO : Encoding %s file Extension\n",encInfo.secret_fname);

	/* Encoding secret file extension into an Image */
	encode_secret_file_extn(strchr(argv[3],'.'),&encInfo);
	done
	    printf("INFO : Encoding %s file Size\n",encInfo.secret_fname);

	/* Encoding secret file size into an Image */
	encode_secret_file_size(encInfo.size_secret_file-1,&encInfo);
	done
	    printf("INFO : Encoding %s file Data\n",encInfo.secret_fname);

	/* Encoding secret file data into an Image*/
	encode_secret_file_data(&encInfo);
	done
	    printf("INFO : Copying left over data\n");

	/* Copying remaining image data into new image */
	copy_remaining_img_data(encInfo.fptr_src_image,encInfo.fptr_stego_image);
	done
	    printf("INFO : ##  ENCODING PROCEDURE COMPLETED  ##\n");

	/* Ternimating all opend files */
	fclose(encInfo.fptr_secret);
	fclose(encInfo.fptr_src_image);
	fclose(encInfo.fptr_stego_image);
    }

    /* Decoding process started */
    else if(check_operation_type(argv)==e_decode)
    {
	decode_stegno_image(argv);
    }

    else
    {
	printf("INFO : Unsupported operation entered use -d or -e \n");
	return e_failure;
    }

    return e_success;
}
