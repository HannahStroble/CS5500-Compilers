#! /usr/bin/python3
"""
Name: Hannah Reinbolt
Date: 8-22-2019
Class: CS5500-101
Assignment: Homework #1 - Lexical Analyzer

How to Run:
>>> python3 ./reinbolth.py [input file here]
>>>
>>>

"""

#libraries
import sys
import re

# Global variables
# debug flag that will show all token info
token_info = True
whitespace = False

# comment flag that will scan over comments
comment = False

# list of all tokens once file is processed
finished_tokens = []

# list of current accepted tokens
token_regex = [
# COMMENTS
["SCOMMENT", r'\(\*'],

# SIGNS
["T_PLUS", r'[+]'], # Escape + to escape "nothing to repeat at engine 0" - Python specific error 
["T_MINUS",r'[-]'],
["T_MULT", r'\*'], # Escape * to escape "nothing to repeat at engine 0" - Python specific error 
["T_DIV", r'div'],

# COMPARISON
["T_LE", r'<='],
["T_GE", r'>='],
["T_EQ", r'[=]'],
["T_NE", r'<>'],
["T_LT", r'[<]'],
["T_GT", r'[>]'],

# ASSIGNMENT
["T_ASSIGN", r':='],

# BRACKETS
["T_LPAREN", r'\('],
["T_RPAREN", r'\)'],
["T_LBRACK", r'\['],
["T_RBRACK", r'\]'],

# DOTS AND SLASHES
["T_DOTDOT", r'\.\.'],
["T_DOT", r'\.'],
["T_COMMA", r'[,]'],
["T_COLON", r'[:]'],
["T_SCOLON", r'[;]'],

# LOGICAL
["T_AND", r'and'],
["T_OR", r'or'],
["T_NOT", r'not'],
["T_TRUE", r'true'],
["T_FALSE", r'false'],

# SPECIAL WORDS
["T_ARRAY", r'array'],
["T_BOOL", r'boolean'],
["T_BEGIN", r'begin'],
["T_CHAR", r'char'],
["T_DO", r'do'],
["T_ELSE", r'else'],
["T_END", r'end'],
["T_IF", r'if'],
["T_INT", r'integer'],
["T_OF", r'of'],
["T_PROC", r'procedure'],
["T_PROG", r'program'],
["T_READ", r'read'],
["T_THEN", r'then'],
["T_VAR", r'var'],
["T_WHILE", r'while'],
["T_WRITE", r'write'],

# REGEX COMBO SPECIAL
["ECOMMENT", r'\*\)'],
["T_INTCONST", r'[0-9]+'],
["T_CHARCONST", r'[\'].[\']'],
["CHARCONSTERR", r'[\'].'],
["T_IDENT", r'[a-zA-Z0-9\_]+[a-zA-Z0-9\_]*'],
["SQUOTE", r'[\'\']'],
["DQUOTE", r'[\']'],
["WHITESPACE", r'[ \t\n]'],
["UNKNOWN", r'.']

]

# Print Statements for Tokens
def print_token(token, lexeme):
    if token_info == True:
        print("TOKEN:\t" + str(token) + "\tLEXEME:\t" + str(lexeme))

# Error Messages


# Regular Expressions
def create_token_list(item):
    # coordinates
    cords = 0
    global comment
    global whitespace
    
    # while each line is not finished, keep processing
    while len(item) > cords:
    
        # scan through regex list for tokens
        for i in token_regex:
            
            # create pattern for token creation
            pattern = re.compile(i[1])
            x = pattern.match(item[cords:])
            
            # if pattern is accepted then token is found and x is not None
            if (x != None):
                
                # iterate to current spot in the list to start of there if processing again
                cords = cords + x.span()[1]
                
                # check for comments
                if (i[0] == "SCOMMENT"):
                        comment = True
                        whitespace = False
                        break
                if (i[0] == "ECOMMENT"):
                        comment = False
                        break
                if (comment == True):
                    whitespace = False
                    break
                else:
                    # check for errors and skip some things
                    if (i[0] == "WHITESPACE"):
                        whitespace = True
                        break
                    elif (i[0] == "T_INTCONST"):
                        if (int(x.group(0)) > 2147483647):
                            print("**** Invalid integer constant: "+str(x.group(0)))
                            break
                    elif (i[0] == "CHARCONSTERR" and whitespace == True):
                        print("**** Invalid character constant: "+str(x.group(0)))
                        break
                    elif (i[0] == "SQUOTE"):
                        print("**** Invalid character constant: "+str(x.group(0)))
                        break
                    elif (i[0] == "DQUOTE"):
                        print("**** Invalid character constant: "+str(x.group(0)))
                        break
                    
                    # print token info and add to token list for later
                    print_token(i[0], x.group(0))
                    finished_tokens.append(str(x.group(0)))
                    #print("line: "+str(item[cords:]))
                    #print("cords "+str(cords)+" item length: "+str(len(item))+" token: "+str(x.group(0))+" identifies: "+str(pattern))
                    break


def process_file():
    # pull in file
    init_file = open(str(sys.argv[1]), "r")
    
    # split into list of string lines
    lines = init_file.readlines()
    
    # split up all tokens and put in a global list for later
    for line in lines:
        create_token_list(line)
        
def getToken():
    # check for empty list
    if (len(finished_tokens) == 0):
        return "TOKEN_0"   # this is just a placeholder for whatever the end token should be
    
    # return next token
    token = finished_tokens[0]
    finished_tokens.pop(0)
    return token
    


def main():
    # process input file and sort tokens
    process_file()
    
    return 0

# run main program
main()
