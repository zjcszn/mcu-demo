#ifndef _USERDEFINEFUNTION_H
#define _USERDEFINEFUNTION_H

#include "sys.h"
#include "string.h"
//#include "malloc.h" 
#include "main.h"

#include <stdlib.h>

#define TEMPDATA_BUF_LENGTH (256*1) //��ʱ���ݳ��ȣ�������Ҫ�޸�



void Str_UpperCase(char* s);//Сд�ַ�ת��д
void Str_LowerCase(char* s) ;//��д�ַ�תСд
void Str_Copy(char *source,char *p);//��source�ַ����滻Ϊp�ַ���
void Str_Cat(char* source,char* p);//��p�ַ���������source���ַ���
u8  Str_Compare(char *source,char *p);//�Ƚ��ַ��Ƿ���ͬ,��ͬ����0,���򷵻��ַ�������
u16 Str_Pos(char *source,char *p);//����source��p�ַ��״γ��ֵ�λ��,���ڷ����״γ��ֵ�λ��,���򷵻�0
void Str_IntToStr(u16 val,char toStr[]);//������ת��Ϊ�ַ���
void Str_SubString(char* p,u16 first,u16 length );//�ӵ�first����ʼ,������first���ַ� ��ȡlength�����ַ�
u16 Str_StrToInt(char* str,u16 nNum);//��str�ַ����еĵ�nNUM��������ȡ��ת��Ϊint������

char* Str_Cat111(char* source,char* p) ;


#endif




