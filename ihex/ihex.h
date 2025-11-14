
#ifndef IHEX_H
#define IHEX_H

#include <inttypes.h>
#include <stdbool.h>

//****************************************************************************************************************
// Global Defines
//****************************************************************************************************************


//****************************************************************************************************************
// Global Types
//****************************************************************************************************************

typedef enum
{
	IHEX_LEXER_STATE_WAIT_COLON,
	IHEX_LEXER_STATE_WAIT_BYTE_COUNT,
	IHEX_LEXER_STATE_WAIT_ADDRESS,
	IHEX_LEXER_STATE_WAIT_TYPE,
	IHEX_LEXER_STATE_WAIT_DATA,
	IHEX_LEXER_STATE_WAIT_CHECK_SUM
}ihex_tLexerState;

typedef enum
{
	IHEX_LEXER_TOKENTYPE_RECORD_MARK,	// Colon : which marks the start of a record
	IHEX_LEXER_TOKENTYPE_RECLEN,		// Length in bytes of the payload
	IHEX_LEXER_TOKENTYPE_LOAD_OFFSET,	// Offset of the record
	IHEX_LEXER_TOKENTYPE_RECTYP,		// Type of the record
	IHEX_LEXER_TOKENTYPE_DATA,			// Payload byte
	IHEX_LEXER_TOKENTYPE_CHKSUM		// Check sum
}ihex_tLexerTokenType;

typedef enum
{
	IHEX_MESSAGE_CONTINUE,
	IHEX_MESSAGE_END,
	IHEX_MESSAGE_INVALID_INPUT_DATA,
	IHEX_MESSAGE_CHECK_SUM_ERROR,
}ihex_tMessage;



typedef enum
{
	IHEX_TYPE_00_DATA_RECORD =  0,
	IHEX_TYPE_01_END_OF_FILE_RECORD = 1,
	IHEX_TYPE_02_EXTENDED_SEGMENT_ADDRESS_RECORD = 2,
	IHEX_TYPE_03_START_SEGMENT_ADDRESS_RECORD = 3,
	IHEX_TYPE_04_EXTENDED_LINEAR_ADDRESS_RECORD = 4,
	IHEX_TYPE_05_START_LINEAR_ADDRESS_RECORD = 5
}ihex_tRecordType;

typedef struct
{
	uint8_t record_mark;
	uint8_t reclen;
	uint16_t load_offset;
	uint8_t rectyp;
	uint8_t data;
	uint8_t target_check_sum;
	bool eof_found;
}ihex_tRecordData;


typedef struct
{
	uint32_t global_offset;
}ihex_tParser;

typedef struct
{
	ihex_tLexerState state;
	uint16_t num_expected_characters;
	ihex_tRecordData record_data;
}ihex_tLexer;

typedef struct
{
	ihex_tLexer lexer;
	ihex_tParser parser;
}ihex_tReader;

typedef bool (*ihex_tRecordEventFunc)(ihex_tReader* reader, ihex_tLexerTokenType event, uint16_t data);


//****************************************************************************************************************
// Global Function-Prototypes
//****************************************************************************************************************

void ihex_Begin(ihex_tReader* reader);
ihex_tMessage ihex_Put(ihex_tReader* reader, uint8_t data);

#endif //IHEX_H