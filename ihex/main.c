#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "ihex.h"


typedef struct
{
	size_t buffer_size;
	uint8_t* data;
}ihex_tBuffer;


ihex_tBuffer readIntelHex(const char* filename);

int main(void)
{
	ihex_tBuffer buffer = readIntelHex("assets/Mp3Player.hex");

	ihex_tReader my_parser;

	ihex_Begin(&my_parser);

	for (int i = 0; i < buffer.buffer_size; i++)
	{
		ihex_tMessage msg = ihex_Put(&my_parser, buffer.data[i]);
		if (msg != IHEX_CONTINUE)
			break;
	}
	
	if (buffer.data != NULL)
		free(buffer.data);

	return 0;
}












ihex_tBuffer readIntelHex(const char* filename)
{
	FILE* file;
	ihex_tBuffer buffer = { 0 };

	errno_t err = fopen_s(&file, filename, "rb");

	if (err != 0)
	{
		printf("Could not open file.\n");
		return buffer;
	}

	fseek(file, 0, SEEK_END);
	size_t file_len = ftell(file);
	fseek(file, 0, SEEK_SET);
	buffer.data = (uint8_t*)malloc(sizeof(uint8_t) * file_len);
	
	if (buffer.data == NULL)
	{
		printf("Buffer allocation failed.\n");
		return buffer;
	}

	buffer.buffer_size = file_len;
	size_t read_size = fread(buffer.data, sizeof(uint8_t), file_len, file);
	fclose(file);

	if (read_size != file_len)
	{
		printf("Issue reading file.\n");
		return buffer;
	}

	return buffer;
}