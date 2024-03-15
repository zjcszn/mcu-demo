#include "sys.h"


//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI  
void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}


//�ر������ж�
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}


//���������ж�
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}


//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}


//ϵͳ�жϷ����������
void SysNVIC_Config(void)
{
	//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	
}







