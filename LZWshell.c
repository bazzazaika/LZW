#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "coder.h"
#include "decoder.h"

void HelpPage() {
	printf("LZW - a universal lossless data compression algorithm.\n\n");
	printf("Usage: LZW [-c | -d] \"file_in\" \"file_out\" \"Maximum Code Size (12-16)\" \n\n");
	printf("Parameters:\n");
	printf(" -h to see this page\n");
	printf(" -c to encode a file\n");
	printf(" -d to decode a file\n");
}

void ErrorMsg() {
	printf("Error!\n\n");
	printf("Type \"LZW -h' to see the help page\"\n");
}


int64_t getFileSize(FILE* f) {
	fseek(f, 0, SEEK_END);
	int64_t size = ftell(f);
	fseek(f, 0, SEEK_SET);
	return size;
}

int32_t maxOutCodeLenght(char* str, int argc) {
	if (argc < 5) {
		printf("Maximum code size is not specified, 16 will be used.\n");
		return 16;
	}
	else if (!strcmp("12", str)) {
		return 12;
	}
	else if (!strcmp(str, "13")) {
		return 13;
	}
	else if (!strcmp(str, "14")) {
		return 14;
	}
	else if (!strcmp(str, "15")) {
		return 15;
	}
	else if (!strcmp(str, "16")) {
		return 16;
	}
	else {
		printf("Maximum code size specified incorrectly, 16 will be used.\n");
		return 16;
	}
}


int main(int argc, char** argv) {

	clock_t start;
	int returnValue = 1;
	FILE* in = NULL, * out = NULL;

	//lotsA.txt  file2MB.docx
	//argv[1] = "-c"; argv[2] = "file2MB.docx"; argv[3] = "outHASH"; argc = 4;
	//argv[1] = "-d"; argv[2] = "outHASH"; argv[3] = "DECODED"; argc = 4;

	//argv[1] = "-c"; argv[2] = "file2MB.docx"; argv[3] = "outHASH"; argc = 5;
	//argv[1] = "-d"; argv[2] = "outHASH"; argv[3] = "DECODED"; argc = 5;

	
	if (argv[1] == NULL) {
		ErrorMsg();
		exit(1);
	}

	if (!(strcmp("-h", argv[1]))) {
		HelpPage();
		exit(0);
	}

	else if (!(strcmp("-c", argv[1])) && (argc == 4 || argc == 5)) {
		if (argv[2] == NULL || argv[3] == NULL) {
			ErrorMsg();
			exit(1);
		}

		fopen_s(&in, argv[2], "rb");
		fopen_s(&out, argv[3], "wb");
		if (in == NULL) {
			printf("No such file \"%s\" exists", argv[2]);
			exit(1);
		}
		if (out == NULL) {
			printf("Error opening the \"%s\" file", argv[3]);
			exit(1);
		}


		start = clock();
		returnValue = encode(in, out, maxOutCodeLenght(argv[4], argc));
		if (returnValue == 0) {
			printf("The file has been successfully encoded.\n");
			printf("\nEncoding time: %F \n", (float)(clock() - start) / (float)CLOCKS_PER_SEC);
		}

		fclose(in);
		fclose(out);

		FILE* in = NULL, * out = NULL;
		fopen_s(&in, argv[2], "rb");
		fopen_s(&out, argv[3], "rb");
		int64_t inSize = getFileSize(in);
		int64_t outSize = getFileSize(out);
		printf("Compression ratio: %F\n", ((double)outSize / (double)inSize));
	}

	else if (!(strcmp("-d", argv[1])) && (argc == 4 || argc == 5)) {

		if (argv[2] == NULL || argv[3] == NULL) {
			ErrorMsg();
			exit(1);
		}

		fopen_s(&in, argv[2], "rb");
		fopen_s(&out, argv[3], "wb");
		if (in == NULL) {
			printf("No such file \"%s\" exists", argv[2]);
			exit(1);
		}
		if (out == NULL) {
			printf("Error opening the \"%s\" file", argv[3]);
			exit(1);
		}
		
		start = clock();
		returnValue = decode(in, out, maxOutCodeLenght(argv[4], argc));
		if (returnValue == 0) {
			printf("The file has been successfully decoded.\n");
			printf("\nDecoding time: %F \n", (float)(clock() - start) / (float)CLOCKS_PER_SEC);
		}
		
		fclose(in);
		fclose(out);
	}



	else {
		ErrorMsg();
		exit(1);
	}

	return 0;
}