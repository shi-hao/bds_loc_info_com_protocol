/*
 *警用北斗全球卫星导航系统 第6部分：定位信息通信协议及数据格式
 *author: 时
 *email : shi7631470@163.com
 *description：接口实现测试
 *
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
#include<string.h>
#include"bds_DataStruct.h"
#include"bds_data_proc.h"

WORD     glo_serial_num=0;
#define  Debug    1

//global data process buf 
#define       glo_data_buf_size  2048
unsigned char glo_data_buf[glo_data_buf_size];
int           glo_data_buf_pos = 0;

//command process
int cmd_proc(unsigned char *buf, int size){

	int ret;
	unsigned char  term_type;
	unsigned short msg_id;
	msg_head_p head;

	ret = reverse_escape_char(buf, size);
	if(verify_check_code(buf, ret) == 0){
		//data in buf is an complete command pakage
		head =(msg_head_p)buf;

		//消息id
		msg_id = get16(head->msg_id);
		printf("recv msg id : %#x \n", msg_id);

		//终端类型
		term_type = head->terminal_type; 
		printf("terminal type : %#x \n", term_type);

	}else{
		printf("checke code error\n");
	}
}

//data process
int data_proc(unsigned char* buf, int size){
	int cnt;
	for(cnt=0;cnt<size;cnt++){
		if(buf[cnt] != MarkChar){
			glo_data_buf[glo_data_buf_pos++] = buf[cnt];
			if(glo_data_buf_pos > glo_data_buf_size){
				printf("\n glo_data_buf overflow\n");
				glo_data_buf_pos = 0;
			}
		}else{
			cmd_proc(glo_data_buf, glo_data_buf_pos);
			glo_data_buf_pos = 0;
		}
	}
}

//main
int main(){

	int res,cnt;
	unsigned char* p;
	unsigned char serial_data[1024 * 4];

	//消息头长度，消息体长度临时变量
	int  tmp_msg_body_size=0;

	//校验码
	//BYTE check_code;

	//缓存
	BYTE buf[1024];

	//消息头
	msg_head my_msg_head;
	WORD my_msg_body_prop=0; 

	//终端注册
	terminal_reg my_terminal_reg={
		{0x7e,0x7d,0xff,0xff,0x10,0x10},  //结构代码
		0x00,                             //国别
		{0x7e,0xee,0xdd,0xcc,0x7d},       //制造商id
		{"zdxxxx-ins"},                   //终端型号
		{"1129"},                         //终端id
		{"1129"}                          //载体编号
	};

	//位置上报--基本信息
	terminal_loc_report my_terminal_loc_report;
	BYTE tmp_time[6]={16,11,21,0,0,0};

	//位置上报--附加信息
	terminal_loc_attach my_terminal_loc_attach;
	BYTE tmp_info[7]={0x11,0x22,0x33};

	memset(&my_terminal_loc_report,0,sizeof(my_terminal_loc_report));

	//填充基本信息
	put32(my_terminal_loc_report.alarm_flg, 0xffffffff);
	put32(my_terminal_loc_report.stat, 0xffffffff);
	put32(my_terminal_loc_report.lat, 0xffffffff);
	put32(my_terminal_loc_report.lng, 0xffffffff);
	put16(my_terminal_loc_report.height, 1000);
	put16(my_terminal_loc_report.speed, 90);
	put16(my_terminal_loc_report.direction, 90);
	memcpy(my_terminal_loc_report.time, tmp_time, sizeof(my_terminal_loc_report.time));
	put16(my_terminal_loc_report.pdop, 1);

	//填充附加信息
	my_terminal_loc_attach.id = 0x00;
	my_terminal_loc_attach.len = 3;
	memcpy(my_terminal_loc_attach.info, tmp_info, sizeof(my_terminal_loc_attach.info));

	//终端鉴权
	BYTE auth_code[20];

	/*
	 ************************************************************************************
	 *终端注册
	 ************************************************************************************
	 */
	//初始化消息头
	memset(&my_msg_head,0,sizeof(my_msg_head));

	//填充消息头
	put16(my_msg_head.msg_id, msg_id_reg);
	my_msg_head.terminal_type=terminal_type_car_ns; //终端类型
	put16(my_msg_head.msg_serial_num, glo_serial_num);//流水号

	set_msg_net_c(my_msg_body_prop);   //设置数据传输方式
	set_msg_enc_sm1(my_msg_body_prop); //设置加密模式

	tmp_msg_body_size = sizeof(my_terminal_reg); //计算消息体长度
	set_msg_body_len(my_msg_body_prop, tmp_msg_body_size); //设置消息体的长度
	set_msg_packet_flg(my_msg_body_prop, 0); //设置消息分包标志
	put16(my_msg_head.msg_body_prop, my_msg_body_prop);

	res = DataPacketInByte((unsigned char *)&my_msg_head, sizeof(my_msg_head),
			(unsigned char *)&my_terminal_reg,sizeof(my_terminal_reg),
			buf, 1024);

#if Debug
	//输出消息头
	printf("\n============%ld byte msg head===============\n ",sizeof(my_msg_head));
	printf("\n============%ld byte msg body===============\n ",sizeof(my_terminal_reg));
	printf("\n============%d byte sum data===============\n ",res);
	p = buf;
	for(cnt=0;cnt<res;cnt++)
		printf("0X%02x ", *p++);
#endif
	memcpy(serial_data+1024, buf, 1024);

	/*
	 ************************************************************************************
	 *终端位置信息上报
	 ************************************************************************************
	 */
	//初始化消息头
	memset(&my_msg_head,0,sizeof(my_msg_head));
	memset(buf, 0, 1024);

	//填充消息头
	put16(my_msg_head.msg_id, msg_id_loc_report);
	my_msg_head.terminal_type=terminal_type_car_ns; //终端类型
	put16(my_msg_head.msg_serial_num, glo_serial_num);//流水号

	set_msg_net_c(my_msg_body_prop);   //设置数据传输方式
	set_msg_enc_sm1(my_msg_body_prop); //设置加密模式

	tmp_msg_body_size = sizeof(my_terminal_loc_report); //计算消息体长度
	set_msg_body_len(my_msg_body_prop, tmp_msg_body_size); //设置消息体的长度
	set_msg_packet_flg(my_msg_body_prop, 0); //设置消息分包标志
	put16(my_msg_head.msg_body_prop, my_msg_body_prop);

	res = DataPacketInByte((unsigned char *)&my_msg_head, sizeof(my_msg_head),
			(unsigned char *)&my_terminal_loc_report,sizeof(my_terminal_loc_report),
			buf, 1024);

#if Debug
	//输出消息头
	printf("\n============%ld byte msg head===============\n ",sizeof(my_msg_head));
	printf("\n============%ld byte msg body===============\n ",sizeof(my_terminal_loc_report));
	printf("\n============%d byte sum data===============\n ",res);
	p = buf;
	for(cnt=0;cnt<res;cnt++)
		printf("0X%02x ", *p++);
#endif
	memcpy(serial_data+2048, buf, 1024);

	//process serial data 
	for(cnt=0;(cnt+256)<=(1024*4);cnt+=256)
		data_proc(serial_data+cnt, 256);

	return 1;
}
