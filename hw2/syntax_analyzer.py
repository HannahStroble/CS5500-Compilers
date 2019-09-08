#!/usr/bin/env python3


# token: T_PROG LEXEME: program
# token = [T_PROG, program]

# global variables
debug_token = True

# prints
def print_tokens(token):
    if debug_token == True:
        print("TOKEN: "+str(token[0])+"\t\tLEXEME: "+str(token[1]))

# grammars

def prog_lbl(token):
    debug_token(token)

def prog():
    prog_lbl()
    getToken()
    #;
    block()

# DONE
def block():
    var_dec_part()
    proc_dec_part()
    stmt_part()


def var_dec_part(token):
    # first check for var
    if (token == "var"):
        var_dec(token)
        # ;
        var_dec_lst(token)

def var_dec_lst():
    var_dec()
    # ;
    var_dec_lst()
    # else 
    # epsilon

def var_dec():
    ident()
    ident_lst()
    # :
    this_type()


def ident():
    # ident

def ident_lst():
    # ,
    ident()
    ident_lst()
    # else
    # epsilon

def this_type():
    # if
    simple()
    #else
    array()

def this_array():
    # array
    # [
    idx_range()
    # ]
    # of
    simple()

def idx():
    # intconst

def idx_range():
    idx()
    # ..
    idx()

def simple():
    # if
    # integer
    # if else
    # char
    # else
    # boolean

def proc_dec_part():
    proc_dec()
    # ;
    proc_dec_part()
    # else
    # epsilon

def proc_dec():
    proc_hdr()
    block()

def proc_hdr():
    # procedure
    # ident
    # ;

def stmt_part():
    compound()

def compound():
    # begin
    stmt()
    stmt_lst()
    # end

def stmt_lst():
    # ;
    stmt()
    stmt_lst()
    # else
    # epsilon

def stmt():
    assign()
    # if else
    read()
    # if else
    write()
    # if else
    condition()
    # if else
    while()
    # if else
    compound()

def assign():
    variable()
    # := 
    expr()

def proc_stmt():
    proc_ident()

def proc_ident():
    # ident

def read():
    # read
    # (
    input_var()
    input_lst()
    # )

def input_lst():
    # ,
    input_var()
    input_lst()
    # )

def input_var():
    variable()

def write():
    # write
    # (
    output()
    output_lst()
    # )

def output_lst():
    # ,
    output()
    output_lst()
    # else
    # epsilon

def output():
    expr()

def condition():
    # if
    expr()
    # then
    stmt()
    elsePart()

def elsePart():
    # else
    stmt()
    # do
    stmt()

def while():
    # while
    expr()
    # do
    stmt()

def expr():
    simple_expr()
    opExpr()

def opExpr():
    rel_op()
    simple_expr()
    # else
    # epsilon

def simple_expr():
    term()
    add_op_lst()

def add_op_lst():
    add_op()
    term()
    add_op_lst
    # else
    # epsilon

def term():
    factor()
    mult_op_lst()

def mult_op_lst():
    mult_op()
    factor()
    mult_op_lst()
    # else
    # epsilon

def factor():
    sign()
    variable()
    # if else
    const()
    # if else
    # (
    expr()
    # )
    # if else
    # not
    factor()

def sign():
    # + 
    # else if 
    # -
    # else if
    # epsilon

def add_op():
    # + 
    # if else
    # -
    # if else
    # or

def mult_op():
    # *
    # if else
    # div
    # if else
    # and

def rel_op():
    # <
    # if else
    # <+
    # if else
    # <>
    # if else
    # =
    # if else
    # >
    # if else
    # >=

def variable():
    # ident
    idx_var()

def idx_var():
    # [
    expr()
    # ]
    # else
    # epsilon

def const():
    # intconst
    # if else
    # charconst
    # if else
    bool_const()

def bool_const():
    # true
    # if else
    # false


    
