#include "stdlib.h"
#include "stdio.h"
#include "my_string.h"

#pragma pack(1)

/*----------------------------------------------------------------------------*/
/* ��������: char isdec(char in)                                              */
/* ��������: �ж��Ƿ���ʮ�����ַ�                                               */
/*----------------------------------------------------------------------------*/
char isdec(char in)
{
	if( (in>=0x30)&&(in<=0x39) )
		return 1;
	else
		return 0;
}

/*----------------------------------------------------------------------------*/
/* ��������: int isfloatstring(const char *in)                                 */
/* ��������: �ж��Ƿ�����ȷ�ĸ������ַ�                                         */
/*----------------------------------------------------------------------------*/
int isfloatstring(const char *in)
{
    char *p= (char*)in;
    char tmp;
    int len=0;

    do{
        tmp = *p++;
        //��������
        if( tmp==0 ){
            break;
        //�м���ӷ������ַ�
        }else if( (isdec(tmp))||(tmp=='.')||(tmp=='-')||(tmp=='+') ){
            len++;
            if( len>20 ){
                len = 0;    //����20λ����,unsigned long 0~4394967295
            }
        }else{
            len = 0;
            break;        
        }
    }while(1);
    
    return len;
}

/*----------------------------------------------------------------------------*/
/* ��������: char ischar(char in)                                              */
/* ��������: �ж��Ƿ�������,��ĸ,����                                           */
/*----------------------------------------------------------------------------*/
char ischar(char in)
{
    if( ((in>='0')&&(in<='9'))  //������
      ||((in>='a')&&(in<='z'))  //��Сд��ĸ
      ||((in>='A')&&(in<='Z'))  //�Ǵ�д��ĸ
      ||((in=='"'))             //
      ||((in=='.')) )           //С����
    {
        return 1;
	}

    return 0;        
}


/*----------------------------------------------------------------------------*/
/* ��������: bool DecChar2Int(char *in, int *re_int, int k)                   */
/* ��������: ��ʮ���Ƶ��ַ�ת����int                                          */
/*           *in: �����ַ�����ָ��                                            */
/*           *re_int: ����int�����ݵ����ָ��λ��                             */
/*           k: ת�����ַ�λ��, ����Ҫ����"12", ��K=2                         */
/*                                                                            */
/*----------------------------------------------------------------------------*/
int isdecstring(const char *in)
{
    char *p= (char*)in;
    char tmp;
    int len=0;
    
    do{
        tmp = *p++;
        //��������
        if( tmp==0 ){
            break;
        //�м���ӷ������ַ�
        }else if( isdec(tmp)==0 ){
            len = 0;
            break;
        }else{
            len++;
            if( len>10 ){
                len = 0;    //����5λ����,unsigned long 0~4394967295
            }
        }
    }while(1);
    
    return len;
}

long DecStr2Int(char *in, unsigned char k)
{
  char i=0;
	unsigned char B[9];
  long c_int=0;

    if( (k<1)||(k>8) )
        return 0;

	B[0] = *in++;
	B[1] = *in++;
	B[2] = *in++;
	B[3] = *in++;
	B[4] = *in++;
	B[5] = *in++;
	B[6] = *in++;
	B[7] = *in++;

  for( i=0; i<8; i++ ){
    if( isdec(B[i])==0 )
      break;
  }
  //
  if( i<1 )
    return 0;
  //
  if( k>i )
		k = i;

  //
  i = 0;

  if( k==8 )
      c_int = (B[i++]&0x0f)*10000000;

  if( k>=7 )
      c_int += (B[i++]&0x0f)*1000000;

  if( k>=6 )
      c_int += (B[i++]&0x0f)*100000;

  if( k>=5 )
      c_int += (B[i++]&0x0f)*10000;

  if( k>=4 )
	  c_int += (B[i++]&0x0f)*1000;

  if( k>=3 )
	  c_int += (B[i++]&0x0f)*100;

  if( k>=2 )
	  c_int += (B[i++]&0x0f)*10;

  if( k>=1 )
	  c_int += (B[i++]&0x0f);


    return c_int;
}

char* Int2DecStr(int num)
{	
    static char str[6];
    char *p=str;
    int k;
    
    
    k = num/10000;
    if(k>0){
        *p++ = k+'0';
        num = num-k*10000;
    }
    
    k = num/1000;
    if(k>0){
        *p++ = k+'0';
        num = num-k*1000;
    }
    
    k = num/100;
    if(k>0){
        *p++ = k+'0';
        num = num-k*100;
    }
    
    k = num/10;
    if(k>0){
        *p++ = k+'0';
        num = num-k*10;
    }
    
    *p++ = num%10+'0';
    
    *p = '\0';

	return str;    
}





#if 0
void test_arm_lib(void)
{
    char str_tmp[100];
    char *str_p="1.23456789";
    double double_i;
    float float_i;
    
    sprintf(str_tmp,"%s%d%s%d","bytes of double:",sizeof(double_i)," bytes of float:",sizeof(float_i));
    printf(str_tmp);    printf("\r\n");
	//
	double_i = 1.23456789;
	sprintf(str_tmp,"%s%f","double 1.23456789=",double_i);	printf("\r\n");
	printf(str_tmp);
	//
	sprintf(str_tmp,"%f",double_i);
	double_i = atof(str_tmp);
	sprintf(str_tmp,"%s%f","atof(double)->",double_i);	printf("\r\n");
	printf(str_tmp);

	float_i = 1.23456789;
	sprintf(str_tmp,"%s%f","float 1.23456789=",float_i);	printf("\r\n");
	printf(str_tmp);
	float_i = atof(str_p);
	sprintf(str_tmp,"%s%f","atof(float_i)->",float_i);	printf("\r\n");
	printf(str_tmp);
}
#endif
