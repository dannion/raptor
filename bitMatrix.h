#pragma once


#include "var_def.h"

//宏函数，用于取pCurRow的第aByteIdx字节的第aBitIdx位
#define BM_GET_ELEM(aByteIdx, aBitIdx, pCurRow) \
	(((pCurRow)[(aByteIdx)]>>(aBitIdx))&0x01)

//按比特信息存储的矩阵
typedef struct BitMatrix_Tag
{
	UINT16 aRows;		//矩阵的行数
	UINT16 aCols;		//矩阵的列数
	UINT16 aLen;		//矩阵一行的长度(Bytes)
	UINT8 *pBitData;	//按比特存储矩阵信息，按4字节对齐
	UINT8 **ppRows;		//存储各行的起始地址
}S_BitMat,*PtrBitMat;

typedef struct BitMatrix_Tag const *PtrCnstBitMat;

typedef enum ElementType_Tag{bm_zero, bm_one}E_ElemType;

class bitMatrix
{
public:
	bitMatrix(void);
	~bitMatrix(void);

	//初始化，为S_BitMat的内部成员分配存储空间
	//aRows: 矩阵的行数
	//aCols: 矩阵的列数
	//pMat: 比特级矩阵
	//返回值: 成功，返回TRUE；失败，返回FALSE
	BOOL bm_init(UINT16 aRows, UINT16 aCols, PtrBitMat pMat);

	//终结，调用bm_inti的S_BitMat都需要调用该接口，
	//	以释放内部各成员的矩阵，该接口可重复调用
	//pMat: 比特级矩阵
	//返回值: 无
	void bm_free(PtrBitMat pMat);

	//复制，将pSrc矩阵复制到pDst矩阵
	//pSrc: 源矩阵
	//pDst: 目标矩阵
	//返回值: 成功，返回TRUE；失败，返回FALSE
	BOOL bm_copyInit(PtrCnstBitMat pSrc, PtrBitMat pDst);

	//部分行复制，将pSrc的指定行复制到pDst
	//pSrc: 源矩阵
	//pRowIdxs: 指定行的下标
	//aCount: pRowIdxs向量的长度
	//pDst: 目标矩阵
	//返回值: 成功，返回TRUE；失败，返回FALSE
	BOOL bm_copyPRInit(PtrCnstBitMat pSrc, UINT16 const *pRowIdxs, 
					   UINT16 aCount, PtrBitMat pDst);

	//修改制定行和列的元素
	//aRN: 行数
	//aC: 列数
	//aValue: 插入的值
	//pMat: 比特级矩阵
	void bm_setElem(UINT16 aRN, UINT16 aCN, 
					E_ElemType aValue, PtrBitMat pMat);

	//统计一行中指定列范围的1的个数
	//aRN: 要统计的行号
	//aCS: 要统计的起始列
	//aCE: 要统计的终止列, aCE>=aCS
	//pMat: 比特级矩阵
	//返回值: 1的个数
	UINT16 bm_getRowOnes(UINT16 aRN, UINT16 aCS, 
						 UINT16 aCE, PtrCnstBitMat pMat);

	//交换两行
	//aR1、aR2: 待交换的两行
	//pMat: 比特级矩阵
	//返回值: 无
	void bm_rowExchange(UINT16 aR1, UINT16 aR2, PtrBitMat pMat);

	//交换两列
	//aCOut:矩阵更新时，V以外的列
	//aCIn:	矩阵更新时，V以内的列
	//aRS: 起始行
	//aRE: 终止行
	//pOnes: 修改V各行1的个数
	//pMat: 比特级矩阵
	//返回值: 无
	void bm_colExchange(UINT16 aCOut, UINT16 aCIn, 
						UINT16 aRS, UINT16 aRE, 
						UINT16 *pOnes, PtrBitMat pMat);

	//对aRSrc、aRDst进行异或运算，结果取代aRDst行
	//aRSrc、aRDst: 用于计算的两行
	//aCS: 起始列
	//aCE: 终止列
	//pMat: 比特级矩阵
	//返回值: 无
	void bm_rowXor(UINT16 aRSrc, UINT16 aRDst, 
				   UINT16 aCS, UINT16 aCE, PtrBitMat pMat);

	//将比特级矩阵转换为字节序列矩阵
	//pMat: 比特级矩阵
	//ppArrayMat: 字节序列矩阵
	//返回值: 无
	void bm_bit2Arrays(PtrCnstBitMat pMat, UINT8** ppArrayMat);

	//将字节序列矩阵转换为比特级矩阵
	//ppArrayMat: 字节序列矩阵
	//pMat: 比特级矩阵。字节序列矩阵的维数需要与比特级
	//	矩阵的维数匹配
	//返回值: 无
	void bm_arrays2Bit(UINT8 const * const* ppArrayMat,
					   PtrBitMat pMat);

	//矩阵求逆，使用初等行变换得到pSrcMat的逆，并存储在
	//		pInv中
	//pSrcMat: 待求逆的方阵
	//pInv: pSrcMat的逆
	//返回值: 成功，返回TRUE；失败，返回FALSE
	BOOL bm_inv(PtrCnstBitMat pSrcMat, PtrBitMat pInv);

	//矩阵乘以向量
	//pMat: 比特级矩阵
	//pVec: 比特级向量
	//pRst: 相乘结果，每一个字节表示1个比特，用户需要提供
	//		长度为pMat->aRows的向量
	//返回值: 无
	void bm_matXVec(PtrCnstBitMat pMat, UINT32 const *pVec, 
					UINT8 *pRst);

	//将一个0/1序列压缩为一个比特级序列
	//pVec: 0/1序列
	//aCount: pVec的长度
	//pBitVec: 比特级序列，每一个比特表示pVec的一个元素，用户需要提供
	//		ceil(aCount/32)*4字节的存储空间
	//返回值: 无
	void bm_getBitVec(UINT8 const *pVec, UINT16 aCount, UINT32 *pBitVec);

	//将一个比特级序列解压为0/1序列
	//pBitVec: 0/1序列
	//aCount: pBitVec中有效比特信息的长度
	//pVec: 0/1序列，用户需要提供长度为aCount字节的存储空间
	//返回值: 无
	void bm_getVec(UINT32 const *pBitVec, UINT16 aCount, UINT8 *pVec);

};

