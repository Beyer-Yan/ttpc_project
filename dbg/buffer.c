/**
  ******************************************************************************
  * @file    	buffer.c
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2016.8.22
  * @brief   	implementation of debug buffer interface
  ******************************************************************************
  * @attention
  *
  * @desc
  *
  * 
  ******************************************************************************
  */
#include"buffer.h"

char __Info_buf[INFO_SIZE]		= {0};
char __Error_buf[ERROR_SIZE]	= {0};

static int info_tail  = 0;
static int error_tail = 0;

int info_putc(int ch)
{
	char tmpch       = ch;
	__Info_buf[info_tail] = tmpch;

	if(info_tail<INFO_SIZE){
		info_tail++;
	} 
	return ch;
}

int error_putc(int ch)
{
	char tmpch              = ch;
	__Error_buf[error_tail] = tmpch;

	if(error_tail<ERROR_SIZE){
		error_tail++;
	}
	return ch;
}

void info_clr(void)
{
	info_tail=0;
}

void error_clr(void)
{
	error_tail=0;
}

int get_info_space(void)
{
	return (INFO_SIZE-info_tail);
}

int get_error_space(void)
{
	return (ERROR_SIZE-error_tail);
}

void* (info_base_addr)(void)
{
	return (void*)(__Info_buf);
}

void* (error_base_addr)(void)
{
	return (void*)(__Error_buf);
}

int get_left_info_space(void)
{
	return info_tail;
}

int get_left_error_space(void)
{
	return error_tail;
}