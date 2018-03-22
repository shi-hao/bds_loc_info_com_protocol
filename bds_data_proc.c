/*
 *警用北斗全球卫星导航系统 第6部分：定位信息通信协议及数据格式
 *author: 时
 *description：接口实现

 *BSD 3-Clause License
 *
 *Copyright (c) [2018], [shi hao]
 *All rights reserved.
 *
 *Redistribution and use in source and binary forms, with or without
 *modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include<stdio.h>
#include"bds_DataStruct.h"
#include<string.h>

/* Put a short in host order into a char array in network order *uint8 */
unsigned char* put32(
		unsigned char* cp,
		DWORD  x)
{
	*cp++ = x >> 24; 
	*cp++ = x >> 16; 
	*cp++ = x >> 8;
	*cp++ = x;
	return cp; 
}                                                                                                                  
/* Put a short in host order into a char array in network order *uint8 */
unsigned char* put16(
		unsigned char *cp,
		WORD  x)
{
	*cp++ = x >> 8;
	*cp++ = x;

	return cp; 
}

WORD get16(unsigned char *cp)
{
	WORD x;

	x = *cp++;
	x <<= 8;
	x |= *cp;
	return x;
}

/* Machine-independent, alignment insensitive network-to-host long conversion */
DWORD  get32(unsigned char *cp)
{
	DWORD rval;

	rval = *cp++;
	rval <<= 8;
	rval |= *cp++;
	rval <<= 8;
	rval |= *cp++;
	rval <<= 8;
	rval |= *cp;

	return rval;
}

//计算checkcode
BYTE cal_check_code(
		unsigned char* data,
		int   data_size)
{
	BYTE check_code=0;
	int cnt;
	if(data_size < 1)
	{
		printf("cal_check_code size error\n");
		return -1;
	}

	//异或计算校验码
	for(cnt=0;cnt<data_size;cnt++){
		check_code = check_code ^ data[cnt];
	}

	return check_code;
}

//校验--数据的尾部1字节是校验码
int verify_check_code(
		unsigned char* data,
		int            data_size
		){
	BYTE check_code=0;

	if(data_size < 2){
		printf("verify_check_code size error\n");
		return -1;
	}

	check_code = cal_check_code(data, data_size-1);
	if(check_code == data[data_size - 1])
		return 0;
	else
		return -1;
}

//转义还原 
int reverse_escape_char(
		unsigned char* buf,
		int            size){

	unsigned char *former, *latter, *end;
	former = buf;
	latter = buf;
	end = buf + size -1;
	for(;latter <= end; latter++, former++){
		if(*latter == 0x7d){
			*former = 0x7c + *++latter;
		}else{
			*former = *latter;
		}
	}
	return (former - buf);
}

//数据按照字节封装
int  DataPacketInByte(unsigned char *DataHead, int HeadLen,  //消息头内存指针和长度 
		unsigned char *DataBody, int BodyLen,  //消息体内存指针和长度
		unsigned char* SndBuff, int size){     //存储缓存和长度

	BYTE check_code=0;
	int cnt=0, cnt_src=0, cnt_dst=0;

	SndBuff[cnt_dst++] = MarkChar;//首部增加数据标识符

	//消息头计算校验码,转义计算
	for(cnt=0;cnt<HeadLen;cnt++){
		check_code = check_code ^ DataHead[cnt];

		if((cnt_dst + 1) >= size)//目标缓存溢出
			return -1;

		if((DataHead[cnt] == MarkChar) || (DataHead[cnt] == MarkCharJunior))//标识位，0x7e, 0x7d
		{
			SndBuff[cnt_dst++] = MarkCharJunior;
			SndBuff[cnt_dst++] = DataHead[cnt] - 0x7c;
		}else{

			SndBuff[cnt_dst++] = DataHead[cnt];
		}
	}

	//消息体计算校验码，转义计算
	if(DataBody != 0x00){
		for(cnt=0;cnt<BodyLen;cnt++){
			check_code = check_code ^ DataBody[cnt];

			if((cnt_dst + 1) >= size)//目标缓存溢出
				return -1;

			if((DataBody[cnt] == MarkChar) || (DataBody[cnt] == MarkCharJunior))//标识位，0x7e, 0x7d
			{
				SndBuff[cnt_dst++] = MarkCharJunior;
				SndBuff[cnt_dst++] = DataBody[cnt] - 0x7c;
			}else{

				SndBuff[cnt_dst++] = DataBody[cnt];
			}
		}
	}

	//校验码转义计算
	if((cnt_dst + 1) >= size)//目标缓存溢出
		return -1;

	if((check_code == MarkChar) || (check_code == MarkCharJunior))//标识位，0x7e, 0x7d
	{
		SndBuff[cnt_dst++] = MarkCharJunior;
		SndBuff[cnt_dst++] = check_code - 0x7c;
	}else{

		SndBuff[cnt_dst++] = check_code;
	}

	if(cnt_dst  >= size)//目标缓存溢出
	{	
		return -1;
	}else{
		SndBuff[cnt_dst++] = MarkChar;//尾部增加数据标识符
	}

	//数据长度
	return cnt_dst;
}

//信息打印
int myprintf(
		unsigned char* string,
		unsigned char* data,
		int            data_size){
	int cnt;

	//打印标题
	printf("\n\n\n%s:", string);

	//打印数据
	for(cnt=0;cnt<data_size;cnt++){
		printf("0x%.2x  ", data[cnt]);	
	}
}
