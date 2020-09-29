#include "lzint_decode.h"

int err(int n, char *str)
{
    printf(str);
    return n;
}

short is_hexdigit(char c)
{
    return ('0' <= c && c <= '9') || ('A' <= c && c <= 'F') || ('a' <= c && c <= 'f');
}

void split_module(FILE *f, int pos)
{
    int origpos = ftell(f);
    module_header hdr;
    FILE *fw;
    char fn[256];
    interfacing iface;
    output_header outhdr;
    
    printf("Found module at position 0x%X.\n", pos);
    fseek(f, pos, SEEK_SET);
    fread(&hdr, sizeof(hdr), 1, f);
    printf("Type %c, number %c%c, block length %ld.\n", hdr.type, hdr.num[0], hdr.num[1], hdr.block_length);
    sprintf(fn, "%s%c%c.rom", GetModuleName(hdr.type), hdr.num[0], hdr.num[1]);
    fw = fopen(fn, "wb");
    if(!fw) { printf("Cannot open file %s. Terminating.\n", fn); exit(5); }

    switch(hdr.compression)
    {
        /*case 0x19:
            printf("UNCOMPRESSED - TODO\n");
            break;*/
        
        case 0xC6:
            // header
			memset(&outhdr, 0, sizeof(outhdr));
			outhdr.magic = 0xF1D64342;
			outhdr.header_length = 0x1B;
			outhdr.offset = hdr.offset;
			outhdr.segment = hdr.segment;
			outhdr.length = hdr.uncompressed_length - 4;
			fwrite(&outhdr, sizeof(outhdr), 1, fw);

            // data
	        printf("lzint compression (%X, %X).\n", hdr.compression, hdr.length);
		    iface.infile = f;
		    iface.outfile = fw;
		    iface.original = hdr.uncompressed_length;
		    iface.packed = hdr.compressed_module_length;
			iface.dicbit = 13;
			iface.method = 5;
			printf("pos %ld\n", ftell(f));
			
			if(decode(iface)) printf("Ok.\n");
			else printf("Fail.\n");
			break;
			
		case 0x01:
	        printf("possible lzint compression (%X, %X).\n", hdr.compression, hdr.length);
            if(hdr.type == 'A') {
				//hdr.uncompressed_length = hdr.compressed_block_length
				//hdr.compressed_module_length = hdr.offset
				hdr.length = hdr.offset;
			}

	        fseek(f, pos + 0x24, SEEK_SET);
		    iface.infile = f;
		    iface.outfile = fw;
		    iface.original = hdr.length; // will be updated in the decoder
		    iface.packed = hdr.length;
			iface.dicbit = 13;
			iface.method = 5;
			printf("pos %ld orig %ld pack %ld\n", ftell(f), iface.original, iface.packed);
			
			if(decode(iface)) printf("Ok.\n");
			else printf("Fail.\n");
			break;
	    
	    default:
	        printf("Unknown compression (%X, %X), lens %X, %X, %X. Just copying.\n", hdr.compression, hdr.length, hdr.uncompressed_length, hdr.compressed_block_length, hdr.compressed_module_length);
	        if(hdr.length < 1000000) {
	            byte header_length = (hdr.header_type == 1 ? 0x18 : 0x24);
	            char *buf = (char *) malloc(hdr.block_length - header_length);
    	        fseek(f, pos + header_length, SEEK_SET);
	            fread(buf, hdr.block_length - header_length, 1, f);
	            fwrite(buf, hdr.block_length - header_length, 1, fw);
	            free(buf);
	        }
	        break;
	}
	fclose(fw);

    fseek(f, origpos, SEEK_SET);
}

int main(int argc, char **argv)
{
    char *str = "\x07\x00\x06\x08\x06\x0e\x07\x08\x07\x03\x07\x00\x06\x0c\x06\x09\x07\x04\x02\x0e\x06\x05\x07\x08\x06\x05\x02\x00\x06\x02\x07\x09\x02\x00\x04\x05\x06\x0e\x06\x04\x06\x05\x07\x02\x00\x0a";
    char outstr[24];
    int i;
    for(i = 0; i < 23; i++) {
        outstr[i] = (str[i * 2] << 4) | str[i * 2 + 1];
    }
    outstr[i] = 0;
    printf("%s\n", outstr);
    
    if(argc != 2) {
        return err(5, "Usage:\n  phnxsplit bios.rom\n\n");
    }
    
    FILE *f = fopen(argv[1], "rb");
    if(!f) {
        return err(5, "Cannot open input file");
    }
    
    byte buffer[512];
    int read;
    
    memset(buffer, 0, 512);
    while(!feof(f)) {
        memcpy(buffer, buffer + 256, 256);
        if(256 != (read = fread(buffer + 256, 1, 256, f))) {
            memset(buffer + 256 + read, 0, 256 - read);
        }
        
        int pos = ftell(f) - 512;
        for(i = 240; i < 512; i ++) {
            if(buffer[i] == 0xF8 && // magic
               buffer[i + 1] <= 2 && // more magic
               buffer[i + 8] == '_' && is_hexdigit(buffer[i + 10]) && is_hexdigit(buffer[i + 11])) {
                split_module(f, pos + i);
            }
        }
    }
    
    fclose(f);
    printf("Done.\n");
    return 0;
}
