#include "RTL.h"
#include "stm32f10x.h"


/*
 * @brief  ��ʱ��λ
 * @param  delay_base ��ʱ��׼ʱ��
 */
void delay_reset(u32 *delay_base)
{
	*delay_base = os_time_get();  //��ȡϵͳʱ��
}


/*
 * @brief  �ж��Ƿ񵽴ﶨʱʱ��  ����ʱ��Ӧ�õ���server_timer_reset������λ
 * @param  delay_base ��ʱ��׼ʱ��
 * @param  ms ��ʱʱ��  ��λ����
 * @retval 0:��ʱ  >0:ʣ��ʱ��
 */
u32 delay_timeout(u32 *delay_base, u32 ms)
{
	u32 temp = os_time_get();  //��ȡϵͳʱ��
	if(*delay_base <= temp)
	{
		u32 d = temp - *delay_base;
		if(d >= ms)  //��ʱʱ�䵽
		{
			return 0;
		}
		else
		{
			return (ms - d);  //ʣ��ʱ��
		}
	}
	else
	{
		//0xffffffff - server_time_base + temp
		u32 d = ~(*delay_base) + temp;
		if(d >= ms)  //��ʱʱ�䵽
		{
			return 0;
		}
		else
		{
			return (ms - d);  //ʣ��ʱ��
		}
	}
}

/*
 * @brief  �ж��Ƿ񵽴ﶨʱʱ��  ����ʱ���Զ�����
 * @param  delay_base ��ʱ��׼ʱ��
 * @param  ms ��ʱʱ��  ��λ����
 * @retval 0:��ʱ  >0:ʣ��ʱ��
 */
u32 delay_timeout_reload(u32 *delay_base, u32 ms)
{
	u32 temp = delay_timeout(delay_base, ms);
	if(!temp)
	{
		delay_reset(delay_base);  //��λ ���¼�ʱ
	}
	return temp;
}
