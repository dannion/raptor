#pragma once

#include "var_def.h"
#include "bitMatrix.h"


//该位域和共同体用于比特级01序列和UINT8 0-1 序列转换 
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


//RAPTOR编码参数结构体
typedef struct RaptorParam_tag
{
	UINT32 aK;	//输入码块长度，范围时4=<K=<8192
	UINT32 aN;	//输出码块长度
	UINT32 aS;	//中间码中LDPC符号数
	UINT32 aH;	//中间码中HALF符号数
	UINT32 aH1;	//ceil(H/2)
	UINT32 aL;	//L=K+S+H，中间码符号数
	UINT32 aL1;	//大于或等于L的最小素数
	S_BitMat aBitMat;	//比特级中间编码矩阵
	S_BitMat aLTMat;	//比特级编码符号的LT生成矩阵
	S_BitMat aInvMat;	//中间编码矩阵的逆，即aBitMat的逆
}S_RaptorParam, *PtrRaptorParam;

typedef struct RaptorParam_tag const *PtrCnstRaptorParam;

 //Triple结构体
typedef struct Triple_tag
{
	UINT32 aD;	//存放度
	UINT32 aA;	//存放LT编码所需的随机数
	UINT32 aB;	//存放LT编码所需的随机数
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
	//编码参数初始化。初始化编码参数结构体S_RaptorParam
	//aK: 输入码块长度。aK满足4=<aK=<8192，当aK超过此范围时，函数
	//		返回FALSE
	//aN: 输出码块长度
	//pParam: 指向S_RaptorParam的指针。用户需要为pParam分配内存长度
	//		为S_RaptorParam大小的内存。若使用raptor_initParam对S_RaptorParam
	//		进行了初始化后，在使用完毕之后，需要调用raptor_freeParam接口进行
	//		释放
	//返回值: 成功，返回TRUE；失败，返回FALSE
	BOOL raptor_initParam(UINT32 K, UINT32 N, PtrRaptorParam pParam);

	//释放编码参数。释放编码参数结构体S_RaptorParam的内部成员
	//pParam: 指向S_RaptorParam的指针。当pParam为NULL时，函数不执行。
	//返回值: 无
	void raptor_freeParam(PtrRaptorParam pParam);

	//中间编码，通过信源符号生成中间码
	//pSrcData: 信源符号。用户需要保证pSrcData符号个数等于pParam->aK，
	//		即pSrcData的缓存应该大于或等于pParam->aK
	//pMedCode: 中间码缓存，用户需要为此提供pParam->aL的缓存空间
	//pParam: 通过raptor_initParam分配的编码参数
	//返回值: 无
	void raptor_encode(UINT8 const *pSrcData, UINT8 *pMedCode, PtrCnstRaptorParam pParam);
	//中间译码，通过接收的编码符号和对应的ESI恢复中间码
	//pCodeData: 编码符号，包含aCCount个符号，缓存长度应该大于或等于
	//		aCCount
	//aCCount: pCodeData中编码符号个数
	//pESI: 指向ESI的缓存，序列长度为aCCount
	//pMedCode: 恢复的中间码，包含pParam->aL的符号。用户需要为该提供
	//		长度为pParam->aL*aSLen的缓存
	//pParam: 通过raptor_initParam分配的编码参数
	//返回值: 成功，返回TRUE；失败，返回FALSE
	BOOL raptor_decode(UINT8 const *pCodeData,UINT16 aCCount, UINT16 const *pESI, 
		UINT8 *pMedCode, PtrCnstRaptorParam pParam);
	//LT编码，使用raptor_encode生成的中间码和用户指定的ESI生成RAPTOR
	//		编码符号
	//pMedCodes: raptor_encode生成的中间码，包含pParam->aL的符号，占用
	//		pParam->aL*aSLen字节的内存
	//aSLen: 一个符号的长度(Byte),aSLen>=1
	//ESI: 编码符号ID。3GPP RAPTOR编码是一种系统码，信源符号占用从0~aK-1
	//		的ESI，即当0=<ESI=<aK-1时，编码符号是对应下标的信源符号
	//pR: 一个RAPTOR编码符号，占用aSLen的字节
	//pParam: 通过raptor_initParam分配的编码参数
	//返回值: 无
	void raptor_LTEnc(UINT8 const *pMedCodes/*, UINT32 aSLen*/, 
		UINT32 aESI, UINT8 *pR, PtrCnstRaptorParam pParam);
	//LT译码，使用中间码恢复信源符号
	//pMedCodes: 中间码序列，包含pParam->aL的符号，占用pParam->aL*aSLen字节的内存
	//aSLen: 一个符号的长度(Byte),aSLen>=1
	//pSrcData: 信源符号序列，包含pParam->aK个符号。用户需提供pParam->aK*aSLen字
	//		节的内存
	//pParam: 通过raptor_initParam分配的编码参数
	//返回值: 无
	void raptor_LTDec(UINT8 const *pMedCodes/*, UINT32 aSLen*/, 
		UINT8 *pSrcData , PtrCnstRaptorParam pParam);
	//RAPTOR二级编码
	//pMedCodes: 中间编码过程生成的中间码，长度为pParam->aL
	//pEnc: 二级编码生成的编码符号，长度为pParam->aN - pParam->aK
	//pParam: 通过raptor_initParam分配的编码参数
	//返回值: 无
	void raptor_raptorEncode(UINT8 const *pMedCodes, UINT8 *pEnc, PtrCnstRaptorParam pParam);

	//RAPTOR二级译码
	//pMedCodes: 中间译码过程生成的中间码，长度为pParam->aL
	//pSrc: 信源符号，长度为pParam->aK
	//pESI: 需要恢复的信源符号对应的ESI
	//aCount: pSrc和pESI的长度
	//pParam: 通过raptor_initParam分配的编码参数
	//返回值: 无
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

