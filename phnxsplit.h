#ifndef __PHNXSPLIT_H_
#define __PHNXSPLIT_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;

char *GetModuleName(byte ID);

typedef struct {
        FILE *infile;
        FILE *outfile;
        unsigned long original;
        unsigned long packed;
        int dicbit;
        int method;
        } interfacing;


typedef struct 
{
    byte ident1; // 0xF8
    byte ident2; // 0x00, 0x01, 0x02
    byte size_checksum; // maybe block_checksum a data_checksum
                        // depends also on _B06 vs _B00
    byte data_checksum;
    
    word block_length;
    byte more_block_length;
    byte header_type;
    
    // POS 08
    byte ident3;
    byte type;
    char num[2]; // ASCII 00, 01, ...
    
    dword magic4;
    
    // POS 10
    dword magic5;
    dword magic6;
    
    // POS 18
    word length; // only when not compressed, else 0x0008
    byte morelength;
    byte compression; // 0x19 = NONE, 0xC6 = LZINT
    
    // POS 1C
    word offset;
    word segment;
    
    // POS 20
    word compressed_block_length;
    word magic9;
    
    word compressed_module_length;
    word magic10;
    
    // POS 28
    word uncompressed_length;
    word magic11;

   // byte magic12;
    
} module_header;

typedef struct
	{
		dword	magic;
		byte 	reserved[5];

		byte 	header_length;
		byte 	is_packed;

		word 	offset;
		word 	segment;

		dword	length;
		dword	packed1;
		dword	packed2;
	} output_header;

#endif
