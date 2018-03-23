/*
 *警用北斗全球卫星导航系统 第6部分：定位信息通信协议及数据格式
 *author: 时
 *email : shi7631470@163.com
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
#ifndef BDS_DATA_PROC_H
#define BDS_DATA_PROC_H

#include"bds_DataStruct.h"

unsigned char* put32(unsigned char* cp, DWORD x);
unsigned char* put16(unsigned char* cp, WORD x);

WORD get16(unsigned char *cp);
DWORD  get32(unsigned char *cp);

//计算校验码
BYTE cal_check_code(
		unsigned char* data,
		int   data_size
		);

//检验
int verify_check_code(
		unsigned char* data,
		int            data_size
		);

//转义还原
int reverse_escape_char(
		unsigned char* buf,
		int            size);

//数据按照字节封装
int  DataPacketInByte(unsigned char *DataHead, int HeadLen,  //消息头内存指针和长度 
					  unsigned char *DataBody, int BodyLen,  //消息体内存指针和长度
					  unsigned char* SndBuff, int size);     //存储缓存和长度
//信息打印 
int myprintf(
		unsigned char* string,
		unsigned char* data,
		int            data_size);
#endif
