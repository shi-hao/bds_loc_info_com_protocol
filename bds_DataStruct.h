/*
 *警用北斗全球卫星导航系统 第6部分：定位信息通信协议及数据格式
 *author: 时
 *email : shi7631470@163.com
 *description：数据结构定义

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

#ifndef BDS_DATASTRUCT
#define BDS_DATASTRUCT

#define shenyang_ver

#define BYTE      unsigned char

#define WORD      unsigned short

#define DWORD     unsigned int

#define STRING    char*

#define MarkChar          0x7e
#define MarkCharJunior    0x7d

#define  MaxLenInByte     

typedef struct {
	int capacity;
	int len;
	int offset;
	unsigned char* data;
}bds_buffer, *bds_buff_p;

/*
 *消息头格式
 */
typedef struct {
	BYTE  msg_id[2];                  //消息id
	BYTE  msg_body_prop[2];      //消息体属性
	BYTE  phone_num[10];            //电话号码
	BYTE  msg_serial_num[2];     //消息序列号

	BYTE  msg_packet_sum[2];     //总包数
	BYTE  msg_packet_num[2];     //分包序号

	BYTE terminal_type;      //终端类型
} msg_head, *msg_head_p;


//
#define  msg_head_size_inByte         21

//消息id
#define  msg_id_reg                   0x0100
#define  msg_id_reg_res               0x8100
#define  msg_id_unreg                 0x0003
#define  msg_id_auth                  0x0102
#define  msg_id_heartBeat             0x0002
#define  msg_id_loc_report            0x0200
#define  msg_id_server_general        0x8001
#define  msg_id_terminal_general      0x0001

//终端类型
#define  terminal_type_car_s          0x00
#define  terminal_type_car_ns         0x01
#define  terminal_type_hand_s         0x02
#define  terminal_type_hand_ns        0x03
#define  terminal_type_ship_s         0x04
#define  terminal_type_ship_ns        0x05
#define  terminal_type_command        0x06
#define  terminal_type_pdt            0x07
#define  terminal_type_time           0x08
#define  terminal_type_others         0x09

/*
 *消息体格式
 */
//设置消息体的长度
#define set_msg_body_len(msg_body_prop, len)    (msg_body_prop = (msg_body_prop &  0xfc00) | ( 0x03ff & len))

//设置加密模式--sm2
#define set_msg_enc_sm2(msg_body_prop)          (msg_body_prop |= 0x0400)

//设置加密模式--sm4
#define set_msg_enc_sm4(msg_body_prop)          (msg_body_prop |= 0x0800)

//设置加密模式--sm1
#define set_msg_enc_sm1(msg_body_prop)          (msg_body_prop |= 0x1000)

//设置数据传输方式--2g/3g/4g
#define set_msg_net_c(msg_body_prop)                (msg_body_prop &= 0x7fff)

//设置数据传输方式--卫星链路专网
#define set_msg_net_p(msg_body_prop)                (msg_body_prop |= 0x8000)

//#define set_msg_packet_n(msg_body_prop)      (msg_body_prop & 0xdfff)

//设置分包标志位
#define set_msg_packet_flg(msg_body_prop,flg)     ((flg > 0) ? (msg_body_prop |= 0x0200):(msg_body_prop &= 0xdfff))

//读取分包标志位
#define read_msg_packet_flg(msg_body_prop)        (msg_body_prop & 0x0200)

//终端注册数据结构
#define  terminal_reg_basic_len                   65
typedef struct{
	BYTE police_org_code[6];
	BYTE manufacturer_country[1];
	BYTE manufacturer_code[18];
	BYTE terminal_model[20];
	BYTE terminal_id[20];
    BYTE carrier_code[20];	
} terminal_reg, *terminal_reg_p;

//注册应答数据结构
typedef struct{

	BYTE res_serial_num[2];
	BYTE result;
	BYTE auth_code[40];

} terminal_reg_res, *terminal_reg_res_p;


//位置信息汇报数据结构
#define terminal_loc_report_basic_len     30
typedef struct{
	BYTE  alarm_flg[4];  //报警标志
	BYTE  stat[4];       //状态
	BYTE  lat[4];        //纬度
	BYTE  lng[4];        //经度
	BYTE  height[2];     //高度
	BYTE  speed[2];      //速度
	BYTE  direction[2];  //方向
	BYTE  time[6];       //YY-MM-DD-hh-mm-ss
	BYTE  pdop[2];       //pdop
} terminal_loc_report, *terminal_loc_report_p;

typedef struct{
	BYTE id;       //附加信息id
	BYTE len;      //附加信息长度
	BYTE info[7];  //附加信息
} terminal_loc_attach, *terminal_loc_attach_p;

typedef struct{
	BYTE serial_num[2];
	BYTE msg_id[2];
	BYTE result;
} server_res_general, *server_res_general_p;
#endif
