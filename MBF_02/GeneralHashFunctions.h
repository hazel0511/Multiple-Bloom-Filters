
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
//�h�[�F�A���j����ȹB��l * ������, �ҥH����ܦ��F���V��ƪ�����, (�W�� hash_function ����ƫ��A, �O�@�ӫ��Ы��V�@�ӶǦ^�Ȭ�unsigned int�B�ݭn�G�ӰѼƪ����.) 
//���w�q����ƫ��A�O�@�ث��V��ƪ�����, �N�ӧڭ̥i�H�Υ��өw�q�ܼ�, �åΨӦs��hash_function��ƪ��a�}, �Ϊ̻��O�����ܼƫ��Vhash_function�ҩw�q���䤤�@�Ө��.
//typedef unsigned int hash_function(char*, unsigned int len) �w�q����ƫ��A�O�@�ب�� (�@�ضǦ^�Ȭ�unsigned int�B�ݭn�G�ӰѼƪ����) �i�Υ��өw�q�\�h�o�����, �ӳo�Ǩ�ƪ��\��O���O�T�w�� (�sĶ�ɴ��T�w��);
//typedef unsigned int (*hash_function)(char*, unsigned int len) �w�q����ƫ��A�O���V��ƫ���, �Ө�ƥ��ݬO�@�ضǦ^�Ȭ���ƥB�ݭn�G�Ӿ�ưѼƪ����,��o�@�����Ъ��[�W () ��, ���N�ܦ�����ҫ��V�����, ������ƥ\��O�ʺA��, �ݬݰ���ɫ��V���O���@�Ө��.


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