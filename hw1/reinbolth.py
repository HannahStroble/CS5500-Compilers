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
debug_info = False
whitespace = False

# comment flag that will scan over comments
comment = False

# list of all tokens once file is processed
finished_tokens = []

# list of current accepted tokens
token_regex = [
# COMMENTS
["ECOMMENT", r'[ \t\n]*\*\)[ \t\n]*'],
["SCOMMENT", r'\(\*[ \t\n]*'],

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
["T_AND", r'and[ \t\n]'],
["T_OR", r'or[ \t\n]'],
["T_NOT", r'not[ \t\n]'],
["T_TRUE", r'true[ \t\n]'],
["T_FALSE", r'false[ \t\n]'],

# SPECIAL WORDS
["T_ARRAY", r'array[ \t\n]'],
["T_BOOL", r'boolean[ \t\n]'],
["T_BEGIN", r'begin[ \t\n]'],
["T_CHAR", r'char[ \t\n]'],
["T_DO", r'do[ \t\n]'],
["T_ELSE", r'else[ \t\n]'],
["T_END", r'end[ \t\n]'],
["T_IF", r'if[ \t\n]'],
["T_INT", r'integer[ \t\n]'],
["T_OF", r'of[ \t\n]'],
["T_PROC", r'procedure[ \t\n]'],
["T_PROG", r'program[ \t\n]'],
["T_READ", r'read[ \t\n]'],
["T_THEN", r'then[ \t\n]'],
["T_VAR", r'var[ \t\n]'],
["T_WHILE", r'while[ \t\n]'],
["T_WRITE", r'write[ \t\n]'],

# REGEX COMBO SPECIAL
["T_INTCONST", r'[0-9]+'],
["T_CHARCONST", r'[\'].[\'][ \t\n]*'],
["T_IDENT", r'[a-zA-Z0-9\_]+[a-zA-Z0-9\_]*'],
["DQUOTE", r'[\']+'],
["SQUOTE", r'[\']'],
["WHITESPACE", r'[ \t\n]'],
["UNKNOWN", r'.']

]

# Print Statements for Tokens
def print_token(token, lexeme):
    global token_info
    if token_info == True:
        print("TOKEN:\t" + str(token) + "\tLEXEME:\t" + str(lexeme))
        
# print messages to help debug
def print_debug(line):
    global debug_info

    if debug_info == True:
        print("DEBUG LINE: "+str(line))    

# Error Messages


# Regular Expressions
def create_token_list(item):
    # coordinates
    cords = 0
    global comment
    global whitespace
    global token_regex
    print_debug("Start token creation")
    
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
                print_debug("Found Token! "+str(x.group(0))+" Type: "+str(i[0]))
                
                # check for comments
                
                if (i[0] == "SCOMMENT"):
                        comment = True
                elif (i[0] == "ECOMMENT"):
                        comment = False
                
                if (comment == True):
                    print_debug("Comment = True")
                    break
                else:
                    print_debug("Made it to final token check")
                    # check for errors and skip some things
                    if (i[0] == "WHITESPACE"):
                        whitespace = True
                        break
                    elif (i[0] == "ECOMMENT"):
                        comment = False
                        break
                    elif (i[0] == "SCOMMENT"):
                        comment = True
                        break
                    elif (i[0] == "T_INTCONST"):
                        if (int(x.group(0)) > 2147483647):
                            print_debug("Found int error! "+str(x.group(0)))
                            print("**** Invalid integer constant: "+str(x.group(0)))
                            break
                    elif (i[0] == "SQUOTE"):
                        print_debug("Found quote error! "+str(x.group(0)))
                        print("**** Invalid character constant: "+str(x.group(0)))
                        break
                    elif (i[0] == "DQUOTE"):
                        print_debug("Found quote error! "+str(x.group(0)))
                        print("**** Invalid character constant: "+str(x.group(0)))
                        break
                    
                    # print token info and add to token list for later
                    print_debug("Sending to print "+str(x.group(0)))
                    print_token(i[0], x.group(0))
                    finished_tokens.append(str(x.group(0)))
                    #print("line: "+str(item[cords:]))
                    #print("cords "+str(cords)+" item length: "+str(len(item))+" token: "+str(x.group(0))+" identifies: "+str(pattern))
                    break


def process_file():
    # pull in file
    init_file = open(str(sys.argv[1]), "r")
    print_debug("Input file: "+str(sys.argv[1]))
    
    # split into list of string lines
    lines = init_file.readlines()
    print_debug("Lines: "+str(lines))
    
    # split up all tokens and put in a global list for later
    for line in lines:
        print_debug("Start create tokens for line: "+str(line))
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
    print_debug("Start processing file")
    process_file()
    
    return 0

# run main program
main()
