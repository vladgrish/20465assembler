/* 
 * File:   symbollocation.h
 * Author: daniel
 *
 * Created on August 4, 2013, 2:15 PM
 */

#ifndef SYMBOLLOCATION_H
#define	SYMBOLLOCATION_H

#include "types.h"
/* a helper type to represent a symbol and WHERE it was located 
 * declared outside of the Symbol type because its relevant only to externals 
 */
typedef struct
{
    char *symbol;
    Word location;
} SymbolLocation, *SymbolLocationPtr;

SymbolLocationPtr initSymbolLocation(char *symbol, Word location);

#endif	/* SYMBOLLOCATION_H */

