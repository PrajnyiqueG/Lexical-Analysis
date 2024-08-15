#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum{
    TOKEN_IDENTIFIER, // variable names, functions
    TOKEN_KEYWORD,//datatypes, if, while, return
    TOKEN_NUMBER,//integers, floats
    TOKEN_STRING,//"hello"
    TOKEN_OPERATORS,//+, -, *, /
    TOKEN_DELIMITER,// (), {}, []
    TOKEN_EOF,//end of file
    TOKEN_ERROR//errors or unrecognized tokens

}TokenType;

typedef struct{
    TokenType typeOfToken;
    char * value;
    int line;
    int column;
}Token;

Token * createToken(TokenType typeOfToken, const char* value, int line, int column){
    Token* ptr = (Token*)malloc(sizeof(Token));
    if (ptr == NULL) {
        perror("Failed to allocate memory for Token");
        exit(EXIT_FAILURE);
    }
    ptr->column = column;
    ptr->line = line;
    ptr->typeOfToken = typeOfToken;
    ptr->value = strdup(value);
    return ptr;
}
void freeToken(Token *token){
    if(token){
        free(token->value);
        free(token);
        
    }
}
char nextChar(FILE * inputFile){
    return fgetc(inputFile);
}

void retractChar(FILE *inputFile, char c){
    ungetc(c, inputFile);
}

void skipWhitespace(FILE *inputFile){
    char c;
    while((c = nextChar(inputFile)) != EOF){
        if(!isspace(c)){
            retractChar(inputFile, c);
            break;
        }
    }
}
void skipComment(FILE *inputFile){
    char c;
    while((c = nextChar(inputFile)) != EOF){
        if(c == '/'){
            c = nextChar(inputFile);
            if(c == '/'){
                while((c = nextChar(inputFile)) != EOF && c != '\n'){
                }
                break;
            }
            else{
                retractChar(inputFile, c);
                retractChar(inputFile, '/');
                break;
            }                     
        }
        else{
            retractChar(inputFile, c);
            break;
        }
    }

}
Token* nextToken(FILE *inputFile) {
    // Implement token recognition here
    // Return tokens based on the current input character
    skipWhitespace(inputFile);
    skipComment(inputFile);
    int c = nextChar(inputFile);
    if (c == EOF){
        return createToken(TOKEN_EOF, "", -1, -1);
    }   
    if(isalpha(c)){
        retractChar(inputFile, c);
        return handleIdentifierOrKeyword(inputFile);
    }
    

    return NULL;
}

// Example functions for token handling
Token* handleIdentifierOrKeyword(FILE *inputFile) {
    // Implement logic to read and recognize identifiers or keywords
    char c;
    int line = 1;
    char tempStorage[256];
    int index =0;
    int column = 0;
    while((c=nextChar(inputFile)) != EOF){
        if(isalnum(c) || c == '_'){
            if(index < sizeof(tempStorage)+1){
                tempStorage[index++] = c;
                column++;
            }
            else {
                // Buffer overflow handling
                lexError("Identifier too long", line, column);
                return createToken(TOKEN_ERROR, "", line, column);
            }
        }
        else{
            retractChar(inputFile, c);
            break;
        }
    } // Placeholder
    tempStorage[index] = '\0'; // Null-terminate the buffer

    // Check if the identifier is a keyword
    TokenType type = identifyKeyword(tempStorage);
    if (type != TOKEN_ERROR) {
        return createToken(type, tempStorage, line, column);
    }

    // If not a keyword, it is an identifier
    return createToken(TOKEN_IDENTIFIER, tempStorage, line, column);

}
TokenType identifyKeyword(const char *str) {
    if(strcmp(str, "if")==0){return TOKEN_KEYWORD;}
    if(strcmp(str, "else")==0){return TOKEN_KEYWORD;}
    if(strcmp(str, "while")==0){return TOKEN_KEYWORD;}
    if(strcmp(str, "for")==0){return TOKEN_KEYWORD;}
    if(strcmp(str, "break")==0){return TOKEN_KEYWORD;}
    if(strcmp(str, "int")==0){return TOKEN_KEYWORD;}
    if(strcmp(str, "float")==0){return TOKEN_KEYWORD;}
    if(strcmp(str, "char")==0){return TOKEN_KEYWORD;}
    if(strcmp(str, "double")==0){return TOKEN_KEYWORD;}
    if(strcmp(str, "bool")==0){return TOKEN_KEYWORD;}
    if(strcmp(str, "sizeof")==0){return TOKEN_KEYWORD;}
    if(strcmp(str, "do")==0){return TOKEN_KEYWORD;}
    if(strcmp(str, "return")==0){return TOKEN_KEYWORD;}
    if(strcmp(str, "struct")==0){return TOKEN_KEYWORD;}
    if(strcmp(str, "switch")==0){return TOKEN_KEYWORD;}
    if(strcmp(str, "enum")==0){return TOKEN_KEYWORD;}
    if(strcmp(str, "typedef")==0){return TOKEN_KEYWORD;}
    if(strcmp(str, "void")==0){return TOKEN_KEYWORD;}
    if(strcmp(str, "const")==0){return TOKEN_KEYWORD;}
    return TOKEN_ERROR;
}

Token* handleNumber(FILE *inputFile, int *line, int *column) {
    // Implement logic to read and recognize numbers
    char buffer[256];  // Buffer to accumulate the number
    int index = 0;
    char c;
    int startColumn = *column;  // Record the start column of the token
    bool isFloat = false;
    while((c=nextChar(inputFile)) != EOF){
        if(isdigit(c)) {
            if(index<sizeof(buffer)-1){
                buffer[index++] = c;
                (*column)++;
            }
            else {
                // Handle buffer overflow
                lexError("Number too long", *line, *column);
                return createToken(TOKEN_ERROR, "", *line, *column);
            }
        } 
        else if(c == '.' && !isFloat){
            isFloat = true;
            if(index < sizeof(buffer)){
                buffer[index++] =c;
                (*column)++;
            }
            else{
                lexError("Number too long", *line, *column);
                return createToken(TOKEN_ERROR, "", *line, *column);
            }
        }
        else{
            retractChar(inputFile, c);
            break;
        }
    }
    buffer[index] ='\0';

    return createToken(TOKEN_NUMBER, buffer, line, column);
// Placeholder
}

Token* handleString(FILE *inputFile, int *line, int *column) {
    // Implement logic to read and recognize strings
    char buffer[256];  // Buffer to accumulate the number
    int index = 0;
    char c;
    int startColumn = *column;
    char quoteChar = nextChar(inputFile);

    if (quoteChar != '\"' && quoteChar != '\'') {
        lexError("Expected a string delimiter", *line, *column);
        return createToken(TOKEN_ERROR, "", *line, *column);
    }

    (*column)++;

    while((c= nextChar(inputFile)) != EOF){
        if(c == quoteChar){
            (*column)++;
            break;
        }

        else if(index < sizeof(buffer)-1){
                buffer[index++] = c;
                (*column)++;
            }
        else{
                lexError("String too long", *line, *column);
                return createToken(TOKEN_ERROR, "", *line, *column);
            }
        }

        buffer[index] ='\0';
        if (c != quoteChar) {
        // If we didn't find the closing quote, it's an error
        lexError("Unterminated string literal", *line, *column);
        return createToken(TOKEN_ERROR, "", *line, startColumn);
        }
        return createToken(TOKEN_STRING, buffer, line, column); // Placeholder
    }
    
        


Token* handleOperator(FILE *inputFile, int *line, int *column) {
    // Implement logic to read and recognize operators
    char buffer[256];  // Buffer to accumulate the number
    int index = 0;
    char c;
    int startColumn = *column; 
    switch(c){
        case '+':
        case '-':
        case '*':
        case '/':
        case '=':
        case '|':
        case '&':
        case '!':
        case '%':
        case '>':
        case '<':
    }
    char nextCharacter = nextChar(inputFile);
    (*column)++;
    


    return NULL; // Placeholder
}

Token* handleDelimiter(FILE *inputFile) {
    // Implement logic to read and recognize delimiters
    return NULL; // Placeholder
}

// Error handling
void lexError(const char *message, int line, int column) {
    fprintf(stderr, "Lexical error at line %d, column %d: %s\n", line, column, message);
}

int main(){
    FILE *inputFile = fopen("source.txt", "r");
    if (inputFile == NULL){
        perror("Error opening file.");
        // return 1;
    }
    char c;

    while((c = nextChar(inputFile)) != EOF){
        skipWhitespace(inputFile);
        skipComment(inputFile);

        Token * token = nextToken(inputFile);
        if (token) {
            // Do something with the token
            printf("Token: Type=%d, Value=%s\n", token->typeOfToken, token->value);
            freeToken(token);
        }
    }
    fclose(inputFile);
    return 0;
}