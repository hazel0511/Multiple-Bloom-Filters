
/*
#pragma once
#**************************************************************************
#*                                                                        *
#*          General Purpose Hash Function Algorithms Library              *
#*                                                                        *
#* Author: Arash Partow - 2002                                            *
#* URL: http://www.partow.net                                             *
#* URL: http://www.partow.net/programming/hashfunctions/index.html        *
#*                                                                        *
#* Copyright notice:                                                      *
#* Free use of the General Purpose Hash Function Algorithms Library is    *
#* permitted under the guidelines and in accordance with the most current *
#* version of the Common Public License.                                  *
#* http://www.opensource.org/licenses/cpl1.0.php                          *
#*                                                                        *
#**************************************************************************
#
*/

#ifndef INCLUDE_GENERALHASHFUNCTION_C_H
#define INCLUDE_GENERALHASHFUNCTION_C_H


#include <stdio.h>


typedef unsigned int (*hash_function)(char*, unsigned int len);
//多加了括號強制取值運算子 * 先執行, 所以函數變成了指向函數的指標, (名為 hash_function 的資料型態, 是一個指標指向一個傳回值為unsigned int且需要二個參數的函數.) 
//它定義的資料型態是一種指向函數的指標, 將來我們可以用它來定義變數, 並用來存放hash_function函數的地址, 或者說是讓該變數指向hash_function所定義的其中一個函數.
//typedef unsigned int hash_function(char*, unsigned int len) 定義的資料型態是一種函數 (一種傳回值為unsigned int且需要二個參數的函數) 可用它來定義許多這類函數, 而這些函數的功能是都是固定的 (編譯時期確定的);
//typedef unsigned int (*hash_function)(char*, unsigned int len) 定義的資料型態是指向函數指標, 該函數必需是一種傳回值為整數且需要二個整數參數的函數,當這一類指標附加上 () 時, 它就變成執行所指向的函數, 它的函數功能是動態的, 端看執行時指向的是哪一個函數.


unsigned int RSHash(char* str, unsigned int len);
unsigned int JSHash(char* str, unsigned int len);
unsigned int PJWHash(char* str, unsigned int len);
unsigned int ELFHash(char* str, unsigned int len);
unsigned int BKDRHash(char* str, unsigned int len);
unsigned int SDBMHash(char* str, unsigned int len);
unsigned int DJBHash(char* str, unsigned int len);
unsigned int DEKHash(char* str, unsigned int len);
unsigned int BPHash(char* str, unsigned int len);
unsigned int FNVHash(char* str, unsigned int len);
unsigned int APHash(char* str, unsigned int len);



#endif