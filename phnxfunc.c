#include "phnxsplit.h"
#include "phnxfunc.h"

char *GetModuleName(byte ID)
{
	switch(ID)
	{
		case 'A': return("ACPI");
		case 'B': return("BIOSCODE");
		case 'C': return("UPDATE");
		case 'D': return("DISPLAY");
		case 'E': return("SETUP");
		case 'F': return("FONT");
		case 'G': return("DECOMPCODE");
		case 'I': return("BOOTBLOCK");
		case 'L': return("LOGO");
		case 'M': return("MISER");
		case 'N': return("ROMPILOTLOAD");
		case 'O': return("NETWORK");
		case 'P': return("ROMPILOTINIT");
		case 'R': return("OPROM");
		case 'S': return("STRINGS");
		case 'T': return("TEMPLATE");
		case 'U': return("USER");
		case 'X': return("ROMEXEC");
		case 'W': return("WAV");
		
		case 'H': return("TCPA_H");		//	TCPA (Trust Computing), USBKCLIB?
		case 'K': return("TCPA_K");		//	TCPA (Trust Computing), "AUTH"?
		case 'Q': return("TCPA_Q");		//	TCPA (Trust Computing), "SROM"?
		case '<': return("TCPA_LESSTHAN");
		case '*': return("TCPA_ASTERISK");
		case '?': return("TCPA_QUESTIONMARK");
		
		case 'J': return("SmartCardPAS");
		
		default:  return("User-Defined");
	}
}

void decodeM3(interfacing interface)
{
		FILE *ptx, *bmx;

		/*------------------------*/
		word Index, Index2, DX, Loop, XorOp, i;
		byte *Buffer, tmp;
		dword RealLen, Now;
		/*------------------------*/

		ptx = interface.infile;
		bmx = interface.outfile;
		RealLen = interface.original;

		Buffer = (byte*)calloc(4096,1);
		if(!Buffer) return;

		DX = 0; Index = 0xFEE; Now = 0;

	for(;;)
	{
		DX >>= 1;
		if((DX&0x100)==0)
		{
			if(Now >= RealLen) { free(Buffer); return; }
				fread(&tmp,1,1,ptx);
				DX = (word)(0xFF)*0x100 + tmp;
		};

		if((DX&0x1)!=0)
		{
			if(Now++ >= RealLen) { free(Buffer); return; }
				fread(&tmp,1,1,ptx);
				fwrite(&tmp,1,1,bmx);
				Buffer[Index++] = tmp;
				Index &= 0xFFF;
			continue;
		};

		Index2 = Index;
			if(Now >= RealLen) { free(Buffer); return; }
		fread(&tmp,1,1,ptx);
		Index = (word)tmp;
			if(Now >= RealLen) { free(Buffer); return; }
		fread(&tmp,1,1,ptx);
		Loop = (word)tmp &0xf;
		Loop +=3;
		XorOp = (word)tmp&0xf0;
		XorOp <<=4;
		Index |= XorOp;
	for(i=0;i<Loop;i++)
	{
		tmp = Buffer[Index++];
		Index &= 0xFFF;
		fwrite(&tmp,1,1,bmx);
		Buffer[Index2++] = tmp;
		Index2 &= 0xFFF;
	}

		Now +=Loop;
		Index = Index2;
	}

} 
