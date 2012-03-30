#pragma once

#include "var_def.h"
#include "bitMatrix.h"


//��λ��͹�ͬ�����ڱ��ؼ�01���к�UINT8 0-1 ����ת�� 
struct CELL
{
	unsigned bit01 : 1;
	unsigned bit02 : 1;
	unsigned bit03 : 1;
	unsigned bit04 : 1;
	unsigned bit05 : 1;
	unsigned bit06 : 1;
	unsigned bit07 : 1;
	unsigned bit08 : 1;
};
union CharToBit
{
	UINT8 zifu;
	struct CELL bits;
};


//RAPTOR��������ṹ��
typedef struct RaptorParam_tag
{
	UINT32 aK;	//������鳤�ȣ���Χʱ4=<K=<8192
	UINT32 aN;	//�����鳤��
	UINT32 aS;	//�м�����LDPC������
	UINT32 aH;	//�м�����HALF������
	UINT32 aH1;	//ceil(H/2)
	UINT32 aL;	//L=K+S+H���м��������
	UINT32 aL1;	//���ڻ����L����С����
	S_BitMat aBitMat;	//���ؼ��м�������
	S_BitMat aLTMat;	//���ؼ�������ŵ�LT���ɾ���
	S_BitMat aInvMat;	//�м���������棬��aBitMat����
}S_RaptorParam, *PtrRaptorParam;

typedef struct RaptorParam_tag const *PtrCnstRaptorParam;

 //Triple�ṹ��
typedef struct Triple_tag
{
	UINT32 aD;	//��Ŷ�
	UINT32 aA;	//���LT��������������
	UINT32 aB;	//���LT��������������
}S_Triple, *PtrTriple;

typedef struct Triple_tag const *PtrCnstTriple;

#ifndef REPAIR_SYMBOL
#define REPAIR_SYMBOL
typedef struct symbol
{
	UINT16	ESI;
	UINT8*	DATA;
}repair_symbol;
#endif

class raptor : public bitMatrix
{
public:
	raptor(void);
	~raptor(void);

	//Do the raptor encode 
	//uint8 array which length is K*T
	//K is the number of total symbols
	//A symbol contains T bytes
	//the encoder return N symbols 
	repair_symbol* RealRaptorEncoder(UINT8*SRC, UINT16 K, UINT32 T, UINT16 N); 

	//after use the encoder RealRaptorEncoder() above , 
	//you should use RealRaptorEncoderFree() to free the space just used;  
	void RealRaptorEncoderFree();

	//Do the raptor decode
	//RcvSymbols/RcvPacket is the receive symbol array with RcvNumber symbols,
	//containing RcvNumber*T bytes.
	//K is the number of total symbols
	//a symbol contains T bytes
	//the encoder return N symbols 
	UINT8* RealRaptorDecoder(repair_symbol* RcvSymbols, UINT16 RcvNumber, UINT16 K, UINT32 T, UINT16 N);
	

	//after use the encoder RealRaptorDecoder() above ,
	// you should use RealRaptorDecoderFree() to free the space just used;  
	void RealRaptorDecoderFree();

private:

	repair_symbol* SymbolToBeSent;
	UINT8* afterdecode;
	//���������ʼ������ʼ����������ṹ��S_RaptorParam
	//aK: ������鳤�ȡ�aK����4=<aK=<8192����aK�����˷�Χʱ������
	//		����FALSE
	//aN: �����鳤��
	//pParam: ָ��S_RaptorParam��ָ�롣�û���ҪΪpParam�����ڴ泤��
	//		ΪS_RaptorParam��С���ڴ档��ʹ��raptor_initParam��S_RaptorParam
	//		�����˳�ʼ������ʹ�����֮����Ҫ����raptor_freeParam�ӿڽ���
	//		�ͷ�
	//����ֵ: �ɹ�������TRUE��ʧ�ܣ�����FALSE
	BOOL raptor_initParam(UINT32 K, UINT32 N, PtrRaptorParam pParam);

	//�ͷű���������ͷű�������ṹ��S_RaptorParam���ڲ���Ա
	//pParam: ָ��S_RaptorParam��ָ�롣��pParamΪNULLʱ��������ִ�С�
	//����ֵ: ��
	void raptor_freeParam(PtrRaptorParam pParam);

	//�м���룬ͨ����Դ���������м���
	//pSrcData: ��Դ���š��û���Ҫ��֤pSrcData���Ÿ�������pParam->aK��
	//		��pSrcData�Ļ���Ӧ�ô��ڻ����pParam->aK
	//pMedCode: �м��뻺�棬�û���ҪΪ���ṩpParam->aL�Ļ���ռ�
	//pParam: ͨ��raptor_initParam����ı������
	//����ֵ: ��
	void raptor_encode(UINT8 const *pSrcData, UINT8 *pMedCode, PtrCnstRaptorParam pParam);
	//�м����룬ͨ�����յı�����źͶ�Ӧ��ESI�ָ��м���
	//pCodeData: ������ţ�����aCCount�����ţ����泤��Ӧ�ô��ڻ����
	//		aCCount
	//aCCount: pCodeData�б�����Ÿ���
	//pESI: ָ��ESI�Ļ��棬���г���ΪaCCount
	//pMedCode: �ָ����м��룬����pParam->aL�ķ��š��û���ҪΪ���ṩ
	//		����ΪpParam->aL*aSLen�Ļ���
	//pParam: ͨ��raptor_initParam����ı������
	//����ֵ: �ɹ�������TRUE��ʧ�ܣ�����FALSE
	BOOL raptor_decode(UINT8 const *pCodeData,UINT16 aCCount, UINT16 const *pESI, 
		UINT8 *pMedCode, PtrCnstRaptorParam pParam);
	//LT���룬ʹ��raptor_encode���ɵ��м�����û�ָ����ESI����RAPTOR
	//		�������
	//pMedCodes: raptor_encode���ɵ��м��룬����pParam->aL�ķ��ţ�ռ��
	//		pParam->aL*aSLen�ֽڵ��ڴ�
	//aSLen: һ�����ŵĳ���(Byte),aSLen>=1
	//ESI: �������ID��3GPP RAPTOR������һ��ϵͳ�룬��Դ����ռ�ô�0~aK-1
	//		��ESI������0=<ESI=<aK-1ʱ����������Ƕ�Ӧ�±����Դ����
	//pR: һ��RAPTOR������ţ�ռ��aSLen���ֽ�
	//pParam: ͨ��raptor_initParam����ı������
	//����ֵ: ��
	void raptor_LTEnc(UINT8 const *pMedCodes/*, UINT32 aSLen*/, 
		UINT32 aESI, UINT8 *pR, PtrCnstRaptorParam pParam);
	//LT���룬ʹ���м���ָ���Դ����
	//pMedCodes: �м������У�����pParam->aL�ķ��ţ�ռ��pParam->aL*aSLen�ֽڵ��ڴ�
	//aSLen: һ�����ŵĳ���(Byte),aSLen>=1
	//pSrcData: ��Դ�������У�����pParam->aK�����š��û����ṩpParam->aK*aSLen��
	//		�ڵ��ڴ�
	//pParam: ͨ��raptor_initParam����ı������
	//����ֵ: ��
	void raptor_LTDec(UINT8 const *pMedCodes/*, UINT32 aSLen*/, 
		UINT8 *pSrcData , PtrCnstRaptorParam pParam);
	//RAPTOR��������
	//pMedCodes: �м����������ɵ��м��룬����ΪpParam->aL
	//pEnc: �����������ɵı�����ţ�����ΪpParam->aN - pParam->aK
	//pParam: ͨ��raptor_initParam����ı������
	//����ֵ: ��
	void raptor_raptorEncode(UINT8 const *pMedCodes, UINT8 *pEnc, PtrCnstRaptorParam pParam);

	//RAPTOR��������
	//pMedCodes: �м�����������ɵ��м��룬����ΪpParam->aL
	//pSrc: ��Դ���ţ�����ΪpParam->aK
	//pESI: ��Ҫ�ָ�����Դ���Ŷ�Ӧ��ESI
	//aCount: pSrc��pESI�ĳ���
	//pParam: ͨ��raptor_initParam����ı������
	//����ֵ: ��
	void raptor_raptorDecode(UINT8 const *pMedCodes, UINT8 *pSrc, 
		UINT16 const *pESI, UINT16 aCount,
		PtrCnstRaptorParam pParam);

	S_RaptorParam raptorParam1;
	
	
	static void raptor_getMedParam(PtrRaptorParam pParam);

	static BOOL raptor_getMedECMatrix(PtrRaptorParam pParam);

	static void raptor_getLDPCMatrix(UINT32 aK, UINT32 aS, UINT8 **ppLDPC);

	static void raptor_getHALFMatrix(UINT32 aK, UINT32 aS,UINT32 aH,
									 UINT32 aH1, UINT8 **ppHALF);

	static BOOL raptor_checkOne(UINT32 x, UINT8 i);

	static void raptor_getLTMatrix(UINT32 aK, UINT32 aL, UINT32 aL1, 
								   UINT32 const *pESI, UINT32 aM, UINT8 **ppLT);

	static BOOL raptor_gassElimilate(PtrBitMat pBitMat, UINT8 *pD, UINT8 *pC);

	static BOOL raptor_firstPhase(PtrBitMat pBitMat, UINT8 *pD, 
									 UINT16 *pIdxC, UINT16 *pI);

	static BOOL raptor_secondPhase(PtrBitMat pBitMat, UINT16 aI, UINT8 *pD);

	static void raptor_thirdPhase(PtrBitMat pBitMat, UINT16 aI, UINT8 *pD);

	static void raptor_rowXor(PtrBitMat pBitMat, UINT16 aRSrc, UINT16 aRDst,
							  UINT16 aCS, UINT16 aCE, UINT8 *pD);

	static void raptor_rowExchange(PtrBitMat pBitMat, UINT16 aR1,
								   UINT16 aR2, UINT8 *pD);

	static void raptor_colExchange(PtrBitMat pBitMat, UINT16 aC1, 
								   UINT16 aC2, UINT16 aRS, UINT16 *pOnes,
								   UINT16 *pIdxC);

	static UINT16 raptor_minOnes(UINT16 aRS, UINT16 aRE, UINT16 *pRow, 
								 UINT16 const *pOnes);

	static UINT32 raptor_rand(UINT32 aX, UINT32 aI, UINT32 aM);

	static UINT8 raptor_deg(UINT32 aV);

	static void raptor_trip (UINT32 aK, UINT32 aL1, UINT32 aESI, PtrTriple pTrip);


	static void raptor_freeDoubleP(UINT8 **ppUINT8, UINT32 aRows);

	static void CharArrayToBit(UINT8* const chararray, UINT32 Charlength, UINT8*bitarray, UINT32 bitlength);
	static void BitToCharArray(UINT8* Bitarray, UINT32 Bitlength, UINT8* chararray, UINT32 Charlength);
};

