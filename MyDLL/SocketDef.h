// SocketDef.h
// 

#ifndef _SOCKETDEF_H
#define _SOCKETDEF_H

#define MAX_FILE_SOCKET		16


//以下定义了包类型，类型大小为一个字节，故赋值请勿超过一个字节
#define CC_NULL			0
#define CC_CMD			1
#define CC_HEAD			2
#define CC_DATA			3
#define CC_TEXT			4
#define CC_CLOSE		5
#define CC_CONNECT		6
#define CC_LIVE			7
#define CC_ALARM		8
#define CC_LIST			9
#define CC_FILE			10
#define CC_FAILURE		11		//访问被拒绝等

#define CC_ID_GET		20
#define CC_ID_UPDATE	21
#define CC_ID_INFO		22
#define CC_ID_RUNOK		23
#define CC_ID_RESTART	24		//重起计算机指令
#define CC_ID_ALIVE		25		//保持激活状态
#define CC_ID_NAT		26		//NAT网络地址探测
#define CC_ID_KEEP		27		//心跳包

#define CC_FRAME_SIZE	30


//对于UDP来讲，在以太网环境，其不分片的用户数据最大可达1472字节
//但相对于整个互联网，各网络设备的MTU可能会有差距的，例如国内流行的PPPoE的MTU就是1492
//而一些老式设备的MTU可能只有512字节,但实际测试发现这种设备已经很少了，故这里忽略不计
//综合考虑我采用了以下字节大小，请勿随意更改
//2007-02-10，X_worm
#define DATA_SIZE_SMALL		44
#define DATA_SIZE_LARGE		1436		//ethernet mtu: 1500, PPPoE MTU:1492

#define SMALL_PACK_HEADER_SIZE		4
#define LARGE_PACK_HEADER_SIZE		12

//为节约网络带宽，包结构中成员尽量进行了合并，具体见对应注释
typedef struct
{
	//nType = nTypeLens & 0xFF, nLens = nTypeLens>>8
	int		nTypeLens;	
	//净载数据
	unsigned char	acData[DATA_SIZE_SMALL];	

} SOCKET_DATA_SMALL, *PSOCKET_DATA_SMALL;

typedef struct
{
	//nType = nTypeLens & 0xFF, nLens = nTypeLens>>8，说明nType共占1个字节，nLens共占3个字节
	int		nTypeLens;
	//nFrame = nFrameIDR & 0xFFFF, nIDR=nFrameIDR>>16，说明nFrame占2个字节，nIDR占2个字节
	int		nFrameIDR;
	//nCurPack = nPackets & 0x00FF, nTotalPack = nPackets & 0xFF00，说明nTotalPack占1个字节，nCurPack占一个字节，暂空两个字节备用
	int		nPackets;	
	//净载数据
	unsigned char	acData[DATA_SIZE_LARGE];	

} SOCKET_DATA_LARGE, *PSOCKET_DATA_LARGE;

typedef struct
{
	BOOL		bConnected;
	int			nElapsed;
	SOCKADDR	sockAddr;

} USOCKINFO, *PUSOCKINFO;

#endif // _SOCKETDEF_H
