#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "types.h"
#include "consts.h"
#include "parser.h"
#include "opcodemap.h"

#define REGISTER_NAME_LENGTH 2

SourceLine initSourceLine(char *text, int lineNumber, char* fileName)
{
    SourceLine line;
    int length = strlen(text);
    line.text = (char*)malloc(sizeof(char) * (length + 1));
    strncpy(line.text, text, length+1);
    
    line.start = line.text;
    
    length = strlen(fileName);
    line.fileName = (char *)malloc(sizeof(char) * (length + 1));
    strncpy(line.fileName, fileName, length);
    
    line.lineNumber = lineNumber;
    
    return line;
}

void freeSourceLine(SourceLine *line)
{
    free(line->text);
    free(line->fileName);
}

void logParsingError(char *errorText, SourceLine *line)
{
    fprintf(stderr, "Error parsing file '%s' line %d: %s\n", line->fileName, line->lineNumber, errorText);
}

void skipWhitespace(SourceLine *sourceLine)
{
    while (*sourceLine->text != EOL && isspace(*sourceLine->text)) sourceLine->text++;
}

char *skipWhitespaceInString(char *str)
{
    while (*str != EOL && isspace(*str)) str++;
    return str;
}

char *getLabel(SourceLine *sourceLine)
{
    char *label;
    char *line = sourceLine->text;
    char *labelEnd = strchr(line, LABEL_TOKEN);
    int length;
    Boolean isValidLabel(SourceLine *sourceLine, char *labelStart, char *labelEnd);
    
    if (labelEnd == NULL) return NULL;
    
    label = line;
    
    if (!isValidLabel(sourceLine, label, labelEnd)) return NULL;
    length = labelEnd - line;
    label = (char *)malloc(sizeof(char) * (length + 1));
    strncpy(label, sourceLine->text, length);
    label[length] = EOL;
    
    return label;
}

Boolean isValidLabel(SourceLine *sourceLine, char *labelStart, char *labelEnd)
{
    Boolean valid = True;
    int length;
    int i;
    char *label = sourceLine->text;
    char msg[MESSAGE_BUFFER_LENGTH] = {0};
    
    if (!isalpha(*label))
    {
        logParsingError("Label must start with a letter:", sourceLine);
        valid = False;
    }
    
    length = labelEnd - labelStart;
    
    if (length > MAX_LABEL_LENGTH)
    {
        sprintf(msg, "Label must be %d chars or less", MAX_LABEL_LENGTH);
        logParsingError(msg, sourceLine);
        valid = False;
    }
    
    for (i = 0; i < length; i++)
    {
        if (!isalnum(label[i]))
        {
            logParsingError("Label contains non alphanumeric characters", sourceLine);
            valid = False;
            break;
        }
    }

    if (length == REGISTER_NAME_LENGTH)
    {
        if (label[0] == REGISTER_PREFIX && ((label[1] - '0') >= MIN_REGISTER_ID && (label[1] - '0') <= MAX_REGISTER_ID))
        {
            logParsingError("Label must not be a valid register name", sourceLine);
            valid = False;
        }
    }
    
    if (isValidOpcodeName(label))
    {
        logParsingError("Label must not be a valid opcode name", sourceLine);
        valid = False;
    }
    return valid;
}

Boolean isBlankLine(SourceLine *sourceLine)
{
    char *ptr = skipWhitespaceInString(sourceLine->text);
    return *ptr == EOL ? True : False;
}

Boolean isCommentLine(SourceLine *sourceLine)
{
    char *ptr = skipWhitespaceInString(sourceLine->text);
    return *ptr == COMMENT_TOKEN ? True : False;
}

Boolean isImaginaryGuidance(SourceLine *sourceLine)
{
    return (strncmp(sourceLine->text, DATA_GUIDANCE_TOKEN, DATA_GUIDANCE_TOKEN_LENGTH) == 0 || 
            strncmp(sourceLine->text, STRING_GUIDANCE_TOKEN, STRING_GUIDANCE_TOKEN_LENGTH) == 0) ? True : False;
}

Boolean tryGetGuidanceType(SourceLine *sourceLine, SymbolType *symbolType)
{
    if (strncmp(sourceLine->text, DATA_GUIDANCE_TOKEN, DATA_GUIDANCE_TOKEN_LENGTH) == 0)
    {
        *symbolType = Data;
        return True;
    }
    
    if (strncmp(sourceLine->text, STRING_GUIDANCE_TOKEN, STRING_GUIDANCE_TOKEN_LENGTH) == 0)
    {
        *symbolType = String;
        return True;
    }
    
    return False;
}