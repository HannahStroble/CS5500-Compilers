#!/usr/bin/env python3




# grammars

def prog_lib():
    #program

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


    
