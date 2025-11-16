#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "ihex.h"


#define VT_EEPROM_SIZE 0xB7C0
#define HEX_DUMP_BYTES_PER_LINE 24

typedef struct
{
	size_t buffer_size;
	uint8_t* data;
}ihex_tBuffer;

static ihex_tMessage main_DataHandler(uint32_t address, uint8_t data);
ihex_tBuffer readIntelHex(const char* filename);

static uint8_t vt_eeprom[VT_EEPROM_SIZE];



int main(void)
{

	ihex_tBuffer buffer = readIntelHex("assets/Mp3Player.hex");
	ihex_tReader my_parser;

	memset(vt_eeprom, 0, VT_EEPROM_SIZE);

	ihex_Init(&my_parser, main_DataHandler);
	ihex_Begin(&my_parser);

	for (int i = 0; i < buffer.buffer_size; i++)
	{
		ihex_tMessage msg = ihex_Put(&my_parser, buffer.data[i]);
		if (msg != IHEX_MESSAGE_CONTINUE)
			break;
	}
	
	if (buffer.data != NULL)
		free(buffer.data);


	int i = 0;
	int bytes_per_line = HEX_DUMP_BYTES_PER_LINE;
	char ascii_line[32];
	ascii_line[HEX_DUMP_BYTES_PER_LINE] = 0;

	while (true)
	{
		ascii_line[HEX_DUMP_BYTES_PER_LINE - bytes_per_line] = ((vt_eeprom[i] >= 0x20) && (vt_eeprom[i] <= 0x7F)) ? vt_eeprom[i] : '.';
		if (bytes_per_line == HEX_DUMP_BYTES_PER_LINE)
			printf("%04X ", i);
		
		bytes_per_line--;
		


		printf(" %02X", vt_eeprom[i]);
		i++;


		

		if (bytes_per_line == 0)
		{
			bytes_per_line = HEX_DUMP_BYTES_PER_LINE;
			printf("  %s\n", ascii_line);
		}

		if (i >= VT_EEPROM_SIZE)
			break;

		
	}


	return 0;
}

static ihex_tMessage main_DataHandler(uint32_t address, uint8_t data)
{
	vt_eeprom[address] = data;
	return IHEX_MESSAGE_CONTINUE;
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