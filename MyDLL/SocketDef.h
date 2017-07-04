// SocketDef.h
// 

#ifndef _SOCKETDEF_H
#define _SOCKETDEF_H

#define MAX_FILE_SOCKET		16


//���¶����˰����ͣ����ʹ�СΪһ���ֽڣ��ʸ�ֵ���𳬹�һ���ֽ�
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
#define CC_FAILURE		11		//���ʱ��ܾ���

#define CC_ID_GET		20
#define CC_ID_UPDATE	21
#define CC_ID_INFO		22
#define CC_ID_RUNOK		23
#define CC_ID_RESTART	24		//��������ָ��
#define CC_ID_ALIVE		25		//���ּ���״̬
#define CC_ID_NAT		26		//NAT�����ַ̽��
#define CC_ID_KEEP		27		//������

#define CC_FRAME_SIZE	30


//����UDP����������̫���������䲻��Ƭ���û��������ɴ�1472�ֽ�
//��������������������������豸��MTU���ܻ��в��ģ�����������е�PPPoE��MTU����1492
//��һЩ��ʽ�豸��MTU����ֻ��512�ֽ�,��ʵ�ʲ��Է��������豸�Ѿ������ˣ���������Բ���
//�ۺϿ����Ҳ����������ֽڴ�С�������������
//2007-02-10��X_worm
#define DATA_SIZE_SMALL		44
#define DATA_SIZE_LARGE		1436		//ethernet mtu: 1500, PPPoE MTU:1492

#define SMALL_PACK_HEADER_SIZE		4
#define LARGE_PACK_HEADER_SIZE		12

//Ϊ��Լ����������ṹ�г�Ա���������˺ϲ����������Ӧע��
typedef struct
{
	//nType = nTypeLens & 0xFF, nLens = nTypeLens>>8
	int		nTypeLens;	
	//��������
	unsigned char	acData[DATA_SIZE_SMALL];	

} SOCKET_DATA_SMALL, *PSOCKET_DATA_SMALL;

typedef struct
{
	//nType = nTypeLens & 0xFF, nLens = nTypeLens>>8��˵��nType��ռ1���ֽڣ�nLens��ռ3���ֽ�
	int		nTypeLens;
	//nFrame = nFrameIDR & 0xFFFF, nIDR=nFrameIDR>>16��˵��nFrameռ2���ֽڣ�nIDRռ2���ֽ�
	int		nFrameIDR;
	//nCurPack = nPackets & 0x00FF, nTotalPack = nPackets & 0xFF00��˵��nTotalPackռ1���ֽڣ�nCurPackռһ���ֽڣ��ݿ������ֽڱ���
	int		nPackets;	
	//��������
	unsigned char	acData[DATA_SIZE_LARGE];	

} SOCKET_DATA_LARGE, *PSOCKET_DATA_LARGE;

typedef struct
{
	BOOL		bConnected;
	int			nElapsed;
	SOCKADDR	sockAddr;

} USOCKINFO, *PUSOCKINFO;

#endif // _SOCKETDEF_H
