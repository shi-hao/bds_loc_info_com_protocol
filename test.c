#include<stdio.h>
#include<string.h>
#include"bds_DataStruct.h"
#include"bds_data_proc.h"


WORD glo_serial_num=0;
#define  Debug    1

int main(){

	int res,cnt;
	unsigned char* p;

	//消息头长度，消息体长度临时变量
	int tmp_msg_head_size=0, tmp_msg_body_size=0;

	//校验码
	BYTE check_code;

	//缓存
	BYTE buf[1024], buff[1024*2];

	//消息头
	msg_head my_msg_head;
	WORD my_msg_body_prop=0; 

	//---------------------------------------------------------------------------------
	//终端注册
	terminal_reg my_terminal_reg={
		{0x7e,0x7d,0xff,0xff,0x10,0x10},  //结构代码
		0x00,                             //国别
		{0x7e,0xee,0xdd,0xcc,0x7d},       //制造商id
		{"zdxxxx-ins"},                   //终端型号
		{"1129"},                         //终端id
		{"1129"}                          //载体编号
	};

	//-----------------------------------------------------------------------------------
	//位置上报--基本信息
	terminal_loc_report my_terminal_loc_report;
	BYTE tmp_time[6]={16,11,21,0,0,0};

	//位置上报--附加信息
	terminal_loc_attach my_terminal_loc_attach;
	BYTE tmp_info[7]={0x11,0x22,0x33};

	memset(&my_terminal_loc_report,0,sizeof(my_terminal_loc_report));

	//填充基本信息
	//my_terminal_loc_report.alarm_flg = 0xffffffff;
	put32(my_terminal_loc_report.alarm_flg, 0xffffffff);
	//my_terminal_loc_report.stat = 0xffffffff;
	put32(my_terminal_loc_report.stat, 0xffffffff);
	//my_terminal_loc_report.lat = 0xffffffff;
	put32(my_terminal_loc_report.lat, 0xffffffff);
	//my_terminal_loc_report.lng = 0xffffffff;
	put32(my_terminal_loc_report.lng, 0xffffffff);
	//my_terminal_loc_report.height = 1000;
	put16(my_terminal_loc_report.height, 1000);
	//my_terminal_loc_report.speed = 90;
	put16(my_terminal_loc_report.speed, 90);
	//my_terminal_loc_report.direction = 90;
	put16(my_terminal_loc_report.direction, 90);
	memcpy(my_terminal_loc_report.time, tmp_time, sizeof(my_terminal_loc_report.time));
	//my_terminal_loc_report.pdop = 1;
	put16(my_terminal_loc_report.pdop, 1);

 
	//填充附加信息
	my_terminal_loc_attach.id = 0x00;
	my_terminal_loc_attach.len = 3;
	memcpy(my_terminal_loc_attach.info, tmp_info, sizeof(my_terminal_loc_attach.info));

	//-------------------------------------------------------------------------------------
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
	//my_msg_head.msg_id=msg_id_reg;                  //消息id
	put16(my_msg_head.msg_id, msg_id_reg);
	my_msg_head.terminal_type=terminal_type_car_ns; //终端类型
	//my_msg_head.msg_serial_num = glo_serial_num++;  //流水号
	put16(my_msg_head.msg_serial_num, glo_serial_num);

	set_msg_net_c(my_msg_body_prop);   //设置数据传输方式
	set_msg_enc_sm1(my_msg_body_prop); //设置加密模式

	//tmp_msg_body_size = terminal_reg_basic_len + strlen(my_terminal_reg.carrier_code); //计算消息体长度
	tmp_msg_body_size = sizeof(my_terminal_reg); //计算消息体长度
	set_msg_body_len(my_msg_body_prop, tmp_msg_body_size); //设置消息体的长度
	set_msg_packet_flg(my_msg_body_prop, 0); //设置消息分包标志
	//my_msg_head.msg_body_prop = my_msg_body_prop; //设置消息头的消息体属性
	put16(my_msg_head.msg_body_prop, my_msg_body_prop);

	//消息头封装
	p = buf;
	tmp_msg_head_size = msg_head_put(&my_msg_head, p, 1024);
	if(tmp_msg_head_size > 0) 
		p += tmp_msg_head_size;

	printf("\ntmp_msg_head_size=%d\n",tmp_msg_head_size);


	//消息体字节封装
	tmp_msg_body_size = msg_body_put_reg(&my_terminal_reg, p, sizeof(buf) - tmp_msg_head_size);
	if(tmp_msg_body_size > 0)
		p += tmp_msg_body_size; 

	printf("tmp_msg_body_size=%d\n",tmp_msg_body_size);

	//计算校验码
	check_code = cal_check_code(buf, tmp_msg_head_size + tmp_msg_body_size);
	*p++ = check_code;

	printf("check_code=0X%.2x\n", check_code);

#if Debug
	//输出消息头
	p = buf;
	for(cnt=0;cnt<tmp_msg_head_size;cnt++)
		printf("0X%02x ", *p++);
	printf("\n==========\n");
	//输出消息体
	for(cnt=0;cnt<tmp_msg_body_size;cnt++)
		printf("0X%02x ", *p++);
	printf("\n==========\n");
#endif

	//计算转义
	res = cal_escape_char(buf,  tmp_msg_head_size+tmp_msg_body_size+1, buff, sizeof(buff));

	//输出
	p = buff;
	printf("sum len:%d\n", res);
	for(cnt=0;cnt<res;cnt++)
		printf("0X%02x ", *p++);


	memset(buf, 0, 1024);
	memset(buff, 0, 1024*2);

#if 0
    /*
	 ************************************************************************************
	 *终端位置信息上报
	 ************************************************************************************
	 */
	//初始化消息头
	memset(&my_msg_head,0,sizeof(my_msg_head));

	//填充消息头
	my_msg_head.msg_id=msg_id_loc_report;                  //消息id
	my_msg_head.terminal_type=terminal_type_car_ns; //终端类型
	my_msg_head.msg_serial_num = glo_serial_num++;  //流水号

	set_msg_net_c(my_msg_body_prop);   //设置数据传输方式
	set_msg_enc_sm1(my_msg_body_prop); //设置加密模式

	tmp_msg_body_size = terminal_loc_report_basic_len + 2 + my_terminal_loc_attach.len; //计算消息体长度
	set_msg_body_len(my_msg_body_prop, tmp_msg_body_size); //设置消息体的长度
	set_msg_packet_flg(my_msg_body_prop, 0); //设置消息分包标志
	my_msg_head.msg_body_prop = my_msg_body_prop; //设置消息头的消息体属性

	//消息头封装
	p = buf;
	tmp_msg_head_size = msg_head_put(&my_msg_head, p, 1024);
	if(tmp_msg_head_size > 0) 
		p += tmp_msg_head_size;

	printf("\n\ntmp_msg_head_size=%d\n",tmp_msg_head_size);

	//消息体字节封装
	tmp_msg_body_size = msg_body_put_loc(&my_terminal_loc_report, &my_terminal_loc_attach,  p,  sizeof(buf) - tmp_msg_head_size);
	if(tmp_msg_body_size > 0)
		p += tmp_msg_body_size; 

	printf("tmp_msg_body_size=%d\n",tmp_msg_body_size);


	//计算校验码
	check_code = cal_check_code(buf, tmp_msg_head_size + tmp_msg_body_size);
	*p++ = check_code;

	printf("check_code=0X%.2x\n", check_code);

#if Debug
	//输出消息头
	p = buf;
	for(cnt=0;cnt<tmp_msg_head_size;cnt++)
		printf("0X%02x ", *p++);
	printf("\n==========\n");
	//输出消息体
	for(cnt=0;cnt<tmp_msg_body_size;cnt++)
		printf("0X%02x ", *p++);
	printf("\n==========\n");
#endif

	//计算转义
	res = cal_escape_char(buf,  tmp_msg_head_size+tmp_msg_body_size+1, buff, sizeof(buff));

	//输出
	p = buff;
	printf("sum len:%d\n", res);
	for(cnt=0;cnt<res;cnt++)
		printf("0X%02x ", *p++);
#endif
	return 1;
}
