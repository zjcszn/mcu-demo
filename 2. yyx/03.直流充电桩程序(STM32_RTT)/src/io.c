#include "stm32f10x.h"

/*
 * ģ���ϵ�(�����Ӵ����պ�)
 */
void io_evc_poweron()
{
	GPIO_SetBits(GPIOE, GPIO_Pin_13);
}

/*
 * ģ���µ�
 */
void io_evc_poweroff()
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_13);
}

/*
 * ��ȡ�����Ӵ�������  0:�պ�   >0�Ͽ�
 */
u8 io_ac_status()
{
	return GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_13);
}


/*
 * ���е���
 */
void io_runled_on()
{
	GPIO_SetBits(GPIOE, GPIO_Pin_12);
}

/*
 * ���е���
 */
void io_runled_off()
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_12);
}



/*
 * ���ϵ���
 */
void io_errled_on()
{
	GPIO_SetBits(GPIOE, GPIO_Pin_11);
}

/*
 * ���ϵ���
 */
void io_errled_off()
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_11);
}


/*
 * ��������
 */
void io_fun_on()
{
	GPIO_SetBits(GPIOE, GPIO_Pin_8);
}

/*
 * �رշ���
 */
void io_fun_off()
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_8);
}


/************************����˫���ǹ***************************/
/*
 * @brief  ���ϳ��ǹ1�̵���
 */
void io_bms_sng1_en()
{
	GPIO_SetBits(GPIOE, GPIO_Pin_6);
}

/*
 * @brief  �Ͽ����ǹ1�̵���
 */
void io_bms_sng1_dis()
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_6);
}


/*
 * @brief  ���ϳ��ǹ2�̵���
 */
void io_bms_sng2_en()
{
	GPIO_SetBits(GPIOE, GPIO_Pin_5);
}

/*
 * @brief  �Ͽ����ǹ2�̵���
 */
void io_bms_sng2_dis()
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_5);
}
/***************************************************/

/*
 * ��ȡDC����̵���1����  0:�պ�   >0�Ͽ�
 */
u8 io_dc1_status()
{
	return GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_14);
}

/*
 * ��ȡDC����̵���2����  0:�պ�   >0�Ͽ�
 */
u8 io_dc2_status()
{
	return GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_15);
}

/*
 * @brief  ����DC����̵���
 */
void io_bms_dc_out_en()
{
	GPIO_SetBits(GPIOE, GPIO_Pin_6);  //DC����̵�������
}

/*
 * @brief  �ͷ�DC����̵���
 */
void io_bms_dc_out_dis()
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_6);  //DC����̵����ͷ�
}


/*
 * @brief  24V�̵�������
 */
void io_bms_24v_en()
{
	GPIO_SetBits(GPIOE, GPIO_Pin_9);  //24V�̵�������
}

/*
 * @brief  24V�̵����ͷ�
 */
void io_bms_24v_dis()
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_9);  //24V�̵����ͷ�
}


/*
 * @brief  12V�̵�������
 */
void io_bms_12v_en()
{
	GPIO_SetBits(GPIOE, GPIO_Pin_10);  //12V�̵�������
}

/*
 * @brief  12V�̵����ͷ�
 */
void io_bms_12v_dis()
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_10);  //12V�̵����ͷ�
}










