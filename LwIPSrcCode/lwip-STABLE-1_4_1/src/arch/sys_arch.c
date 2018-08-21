/*
 * Copyright (c) 2001, Swedish Institute of Computer Science.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. Neither the name of the Institute nor the names of its contributors 
 *    may be used to endorse or promote products derived from this software 
 *    without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE. 
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id: sys_arch.c,v 1.1.1.1 2003/05/17 05:06:56 chenyu Exp $
 */
#if !NO_SYS

#include "def.h"
#include "sys.h"
#include "err.h"
//ucosii���ڴ����ṹ�����ǽ���������ռ�ͨ���ڴ����ṹ������

/*
static OS_MEM *MboxMem;
static char MboxMemoryArea[TOTAL_MBOX_NUM * sizeof(struct LWIP_MBOX_STRUCT)];
const u32_t NullMessage;//�����ָ��Ͷ�ݵ�����
*/

//����ϵͳʹ�õĳ�ʱ������ָ��ṹ
//struct sys_timeouts global_timeouts;
//��ϵͳ�����½�������صı�������

#define LWIP_MAX_TASKS 4 	//�����ں���ഴ�����������
#define LWIP_STK_SIZE  512	//ÿ������Ķ�ջ�ռ�
OS_STK  LWIP_STK_AREA[LWIP_MAX_TASKS][LWIP_STK_SIZE]; //����Ķ�ջ�ռ�


void sys_init()
{
  //currently do nothing
  //Printf("[Sys_arch] init ok");
}

/*
*����һ���ź��� �ź�����ʼֵΪcount �������ɹ� �򷵻�ֵΪERR_OK ��semָ��ɹ�
*�������ź��������򷵻طǳɹ�ֵ ��ʾ����ʧ��
*/
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
  OS_EVENT *new_sem = NULL;

  LWIP_ASSERT("[Sys_arch]sem != NULL", sem != NULL);

  new_sem = OSSemCreate((u16_t)count);	//���ò���ϵͳ����һ���ź���
  LWIP_ASSERT("[Sys_arch]Error creating sem", new_sem != NULL);
  if(new_sem != NULL) {	//��������ɹ�
    *sem = (void *)new_sem;	//��¼�ź���
    return ERR_OK;
  }
   
  *sem = SYS_SEM_NULL;	//��ֵΪ��
  return ERR_MEM;	//����ʧ�ܷ���
}
/* ɾ��ָ��sem���ź��� */
void sys_sem_free(sys_sem_t *sem)
{
  u8_t Err;
  // parameter check 
  LWIP_ASSERT("sem != NULL", sem != NULL);
  
  OSSemDel(*sem, OS_DEL_ALWAYS, &Err);	//ֱ�ӵ��ò���ϵͳ����ɾ���ź���
	
  if(Err != OS_ERR_NONE)  //���ɾ��ʧ�� 
  {
    //add error log here
    //Printf("[Sys_arch]free sem fail\n");
  }

  *sem = NULL;
}

/*
* �ȴ��ź��� ���timeout = 0 ��һֱ�����ȴ� ֪���ź����ĵ��������timeout������0 
* ��ʾ���������ȴ������� ��������� ��������ֵΪ�ڸ��ź����ϵȴ������ѵ�ʱ�䣬�����
* �ȴ�timeoutʱ���û�е����ź��� �򷵻�SYS_ARCH_TIMEOUT ������õȴ��Ϳ���ʹ�ø��ź���
* �򷵻�ֵΪ0
*/
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
  u8_t Err;
  u32_t wait_ticks;
  u32_t start, end;
  LWIP_ASSERT("sem != NULL", sem != NULL);

  if (OSSemAccept(*sem))		  // ����Ѿ��յ�, �򷵻�0 
  {
	  //Printf("debug:sem accept ok\n");
	  return 0;
  }   
  /* ���ȴ��ĺ����� ת��Ϊ����ϵͳ�Ķ��õ�ʱ�ӵδ��� */
  wait_ticks = 0;
  if(timeout!=0){
	 wait_ticks = (timeout * OS_TICKS_PER_SEC)/1000;	
	 if(wait_ticks < 1)	//�ȴ���Χȡ��
		wait_ticks = 1;
	 else if(wait_ticks > 65535)
			wait_ticks = 65535;
  }

  start = sys_now();	//��ʼ�ȴ�
  OSSemPend(*sem, (u16_t)wait_ticks, &Err);
  end = sys_now();	//�����ȴ�
  
  if (Err == OS_NO_ERR)		//����ɹ��ȴ������ź���
		return (u32_t)(end - start);		//��������ʱ��
  else
		return SYS_ARCH_TIMEOUT;
  
}

/* �ͷ�һ���ź��� */
void sys_sem_signal(sys_sem_t *sem)
{
  u8_t Err;
  LWIP_ASSERT("sem != NULL", sem != NULL);

  Err = OSSemPost(*sem);	//ֱ�ӵ���ϵͳ�����ͷ��ź���
  if(Err != OS_ERR_NONE)	//����ͷ�ʧ��
  {
        //add error log here
        //Printf("[Sys_arch]:signal sem fail\n");
  }
  
  LWIP_ASSERT("Error releasing semaphore", Err == OS_ERR_NONE);
}

/*
* ����һ������ �����������ɵ���Ϣ��Ϊsize����������Ϣ�ı�����ָ�룬��ָ������Ϣ�ľ���λ�ã�
* �������У�������size ����Ĭ�ϵ�MAX_QUEUE_ENTRIES��С
*
*
*/
err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
  err_t err;
  LWIP_ASSERT("mbox != NULL", mbox != NULL);
  LWIP_UNUSED_ARG(size);	//����size

  err = sys_sem_new(&(mbox->sem), 0);	//���������ϵ��ź��� ����ͬ��
  //LWIP_ASSERT("Error creating semaphore", err == ERR_OK);
  if(err != ERR_OK) {	//����ʧ�� �����ڴ����
  	//Printf("[Sys_arch]:signal sem fail\n");
    return ERR_MEM;
  }
  err = sys_mutex_new(&(mbox->mutex));	//���������� ����ʧ���򷵻��ڴ����
  LWIP_ASSERT("Error creating mutex", err == ERR_OK);
  if(err != ERR_OK) {
  	sys_sem_free(&(mbox->sem));	//��������������
    return ERR_MEM;
  }
  //��ʼ��������
  memset(&mbox->q_mem, 0, sizeof(void *)*MAX_QUEUE_ENTRIES);	//��ջ���������
  mbox->head = 0;	
  mbox->tail = 0;
  mbox->msg_num = 0;
  
  return ERR_OK;
}

/* 
* ɾ������    ���ɾ�������������л���������Ϣ ˵���û�Ӧ�ó����д���� �Ӷ������������쳣   
* �û�Ӧ�ü������������쳣
* 
*/
void sys_mbox_free(sys_mbox_t *mbox)
{
  /* parameter check */
  u8_t Err;
  LWIP_ASSERT("mbox != NULL", mbox != NULL);
  
  sys_sem_free(&(mbox->sem));	//�ͷ��ź���
  sys_mutex_free(&(mbox->mutex));	//�ͷŻ�����

  mbox->sem = NULL;
  mbox->mutex = NULL;
}

/* �����䷢��һ����Ϣ��������Ͷ������������������������ֱ�����ͳɹ�Ϊֹ */
void sys_mbox_post(sys_mbox_t *q, void *msg)
{
  u8_t Err;
  //SYS_ARCH_DECL_PROTECT(lev);

  /* parameter check */
  LWIP_ASSERT("q != SYS_MBOX_NULL", q != SYS_MBOX_NULL);
  LWIP_ASSERT("q->sem != NULL", q->sem != NULL);

  //queue is full, we wait for some time 
  /* ���ͻ������� ��ȴ������� */
  while(q->msg_num >= MAX_QUEUE_ENTRIES)
  {
    sys_msleep(1);
  }
  
  //SYS_ARCH_PROTECT(lev);
  sys_mutex_lock(&(q->mutex));	//���� ��ֹ�ظ�����
  if(q->msg_num >= MAX_QUEUE_ENTRIES)	//�ڵȴ����ڼ� �������ֱ����� �޷��������־�������
  {
    LWIP_ASSERT("mbox post error, we can not handle it now, Just drop msg!", 0);
	//SYS_ARCH_UNPROTECT(lev);
	sys_mutex_unlock(&(q->mutex));	 //�ͷŻ�����
	return;	//���� ������ϢͶ��
  }
  q->q_mem[q->head] = msg;	//д������
  (q->head)++;	//����д��ָ��
  if (q->head >= MAX_QUEUE_ENTRIES) {	//��ָ���Ѵﵽ������ĩβ
    q->head = 0;	//�����ָ��Ϊ������ͷ��
  }

  q->msg_num++;	//������Ϣ����
  if(q->msg_num == MAX_QUEUE_ENTRIES)	//���������
  {
    //Printf("mbox post, box full\n");
  }
  
  //Err = OSSemPost(q->sem);
  sys_sem_signal(&(q->sem));	//��Ϣ����Ͷ�� �ͷ��ź���
  sys_mutex_unlock(&(q->mutex)); //�ͷŻ����� �ͷŷ���
}

/* ���������Է���һ����Ϣ ������ͳɹ� ����ERR_OK ���򷵻�ERR_MEM */
err_t sys_mbox_trypost(sys_mbox_t *q, void *msg)
{
  u8_t Err;
  //SYS_ARCH_DECL_PROTECT(lev);

  /* parameter check */
  LWIP_ASSERT("q != SYS_MBOX_NULL", q != SYS_MBOX_NULL);
  LWIP_ASSERT("q->sem != NULL", q->sem != NULL);

  //SYS_ARCH_PROTECT(lev);
  sys_mutex_lock(&(q->mutex));	//�������� ��ֹͬʱ����

  if (q->msg_num >= MAX_QUEUE_ENTRIES) { //����������� ֱ���˳�
  
    sys_mutex_unlock(&(q->mutex));	//�˳�ǰ�ǵý���
	//Printf("[Sys_arch]:mbox try post mbox full\n");
    return ERR_MEM;
  }

  q->q_mem[q->head] = msg;	//д����Ϣ
  (q->head)++;	//����д��ָ��
  if (q->head >= MAX_QUEUE_ENTRIES) {	//����Ѿ�����β
    q->head = 0;	//��ͷ��ʼ
  }

  q->msg_num++;		//������Ϣ����
  if(q->msg_num == MAX_QUEUE_ENTRIES)	//���Ӵ���
  {
    //Printf("mbox try post, box full\n");
  }
  
  sys_sem_signal(&(q->sem));	//��Ϣ����Ͷ�� �ͷ��ź���
  
  sys_mutex_unlock(&(q->mutex));	//����
  return ERR_OK;
}

/*
* �������ܣ��������ȡ��Ϣ ���timeout��Ϊ0 ���ʾ�ȴ���������������ʱ����ȴ�����Ϣ
* �򷵻صȴ�ʱ�䣨ms������timeoutΪ0 ��ʾ���������ȴ���ֱ���յ���Ϣ��
* ���ܴ��������յ�һ������Ϣ��Ӧ�ó��򲻻��������Ϣ���κδ���ֱ�Ӷ�����
**/
u32_t sys_arch_mbox_fetch(sys_mbox_t *q, void **msg, u32_t timeout)
{
  u8_t Err;
  u32_t wait_ticks;
  u32_t start, end;
  u32_t tmp_num;
  //SYS_ARCH_DECL_PROTECT(lev);

  // parameter check 
  LWIP_ASSERT("q != SYS_MBOX_NULL", q != SYS_MBOX_NULL);
  LWIP_ASSERT("q->sem != NULL", q->sem != NULL);
  
  wait_ticks = 0;
  if(timeout!=0){
	 wait_ticks = (timeout * OS_TICKS_PER_SEC)/1000;
	 if(wait_ticks < 1)
		wait_ticks = 1;
	 else if(wait_ticks > 65535)
			wait_ticks = 65535;
  }
  /* �����ź��� ֱ�������������ݻ�ʱ*/
  start = sys_arch_sem_wait(&(q->sem), timeout);
  //end = sys_now();

  if (start != SYS_ARCH_TIMEOUT)	//����ɹ���ȡ���ź���
  {
    //SYS_ARCH_PROTECT(lev);
    sys_mutex_lock(&(q->mutex));	//��ȡ�������ķ�����
	
	if(q->head == q->tail)	//���ͷβ���  ���ش���
	{
     //   Printf("mbox fetch queue abnormal [%u]\n", q->msg_num);
		if(msg != NULL) {
			*msg  = NULL;
	    }
		//SYS_ARCH_UNPROTECT(lev);
		sys_mutex_unlock(&(q->mutex));
		return SYS_ARCH_TIMEOUT;
	}
	
    if(msg != NULL) {	//��������ָ�� ��Ч
      *msg  = q->q_mem[q->tail];	//��β����ȡ����
    }

    (q->tail)++;	//���ƶ�����ָ��
    if (q->tail >= MAX_QUEUE_ENTRIES) {	//����Ѿ�����β��
      q->tail = 0;	//��ͷ��ʼ�ٶ�
    }

	if(q->msg_num > 0)	//���������е���Ϣ��
	{
      q->msg_num--;
	}
	else
	{
      //Printf("mbox fetch queue error [%u]\n", q->msg_num);
	}

	tmp_num = (q->head >= q->tail)?(q->head - q->tail):(MAX_QUEUE_ENTRIES + q->head - q->tail);

	if(tmp_num != q->msg_num)
	{
       // Printf("mbox fetch error, umatch [%u] with tmp [%u]\n", q->msg_num, tmp_num);
	}
	
	//SYS_ARCH_UNPROTECT(lev);
	sys_mutex_unlock(&(q->mutex)); //�ͷŷ���Ȩ
	//Printf("mbox fetch ok, match [%u] with tmp [%u] \n", q->msg_num, tmp_num);
	//return (u32_t)(end - start);		//���ȴ�ʱ������Ϊtimeout/2;
	return start;
  }
  else	//��ȡ�ź�����ʱ
  {
    //Printf("mbox fetch time out error");
    if(msg != NULL) {
      *msg  = NULL;	//���ؿ�
    }
	
	return SYS_ARCH_TIMEOUT;	//���س�ʱ����
  }
}
/*
* �������ܣ����Դ������ж�ȡ��Ϣ���ú��������������� ��������������ʱ�ж�ȡ�ɹ�
* ����0 ������������SYS_MOBX_EMPTY
*****/
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *q, void **msg)
{
  u32_t tmp_num;
  //SYS_ARCH_DECL_PROTECT(lev);
  u32_t start;
  /* parameter check */
  LWIP_ASSERT("q != SYS_MBOX_NULL", q != SYS_MBOX_NULL);
  LWIP_ASSERT("q->sem != NULL", q->sem != NULL);

  if(q->msg_num == 0)	//�����Ϣ����Ϊ0
  	return SYS_MBOX_EMPTY;
  /* �ȴ��ź��� ʱ������Ϊ1ms */
  start = sys_arch_sem_wait(&(q->sem), 1);
  
  if (start != SYS_ARCH_TIMEOUT) {	//����ɹ���ȡ���ź���
    //SYS_ARCH_PROTECT(lev);
    sys_mutex_lock(&(q->mutex));	//���뻺�����ķ���Ȩ
	if(q->head == q->tail)	
	{
       // Printf("mbox tryfetch queue abnormal [%u]\n", q->msg_num);
		if(msg != NULL) {
			*msg  = NULL;
	    }
		//SYS_ARCH_UNPROTECT(lev);
		sys_mutex_unlock(&(q->mutex));
		return SYS_MBOX_EMPTY;
	}
		
    if(msg != NULL) {	//��ȡ��Ϣ
      *msg  = q->q_mem[q->tail];
    }

    (q->tail)++;	//��������ָ��
    if (q->tail >= MAX_QUEUE_ENTRIES) {
      q->tail = 0;
    }

    if(q->msg_num > 0)	//���������е���Ϣ��
	{
      q->msg_num--;
	}

	tmp_num = (q->head >= q->tail)?(q->head - q->tail):(MAX_QUEUE_ENTRIES + q->head - q->tail);
    
	
	if(tmp_num != q->msg_num)
	{
        Printf("mbox try fetch error, umatch [%u] with tmp [%u]\n", q->msg_num, tmp_num);
	}
	
    //SYS_ARCH_UNPROTECT(lev);
    sys_mutex_unlock(&(q->mutex));	//�ͷŷ��ʿ���Ȩ
    return 0;
  }
  else	//��ȡ�ź�����ʱ
  {
   // Printf("mbox try fetch uknow error\n");
    if(msg != NULL) {
      *msg  = NULL;	//���ؿ�
    }

    return SYS_MBOX_EMPTY;	//��������Ϊ��
  }
}

/*
* �������ܣ��½�һ�����̣�������ϵͳ��ֻ�ᱻ����һ��
* name���������� thread�����̺��� arg����������ʱ�Ĳ��� stacksize�����̶�ջ�ռ� prio���������ȼ�
*/
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
  static u32_t TaskCreateFlag=0;
  u8_t i=0;
  name=name;
  stacksize=stacksize;
  /* Ϊ����Ѱ�Ҷ�ջ�ռ� */
  while((TaskCreateFlag>>i)&0x01){
    if(i<LWIP_MAX_TASKS&&i<32)
          i++;
    else return 0;
  }
  if(OSTaskCreate(thread, (void *)arg, &LWIP_STK_AREA[i][LWIP_STK_SIZE-1],prio)==OS_NO_ERR){
       TaskCreateFlag |=(0x01<<i); //���ö�ջʹ�ñ�־
	   
  };

  return prio; //���ؽ������ȼ�����Ϊ���̱��
}

#endif


