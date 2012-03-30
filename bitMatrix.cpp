#include "bitMatrix.h"
#include <malloc.h>
#include <cmath>
#include <string>
#include <string.h>

#ifdef BIT_MAT_DEBUG
#include <stdio.h>
#include <assert.h>
#endif

/*-------------------宏定义----------------*/

//#define BIT_MAT_DEBUG

#define ELEM_LEN 8		//定义一个存储单元占用的列数
#define BYTE_LEN 8		//定义一个BYTE的位宽
#define BYTE_LOGLEN 3		//定义一个字节的比特长度的log2值
#define BYTE_RES_MASK 0x0007	//余数掩码

#define ELEM4_LEN 32	//定义4字节的位宽
#define ELEM4_LOGLEN 5	//定义4字节的LOG2位宽
#define ELEM4_RES_MASK 0x0000001f	//4字节余数掩码

#define ROW_END 65535	//行尾标志

//宏函数，用于将aRN行、aCN列设置为1
#define BM_SET(aRN, aCN, pMat) \
	((pMat)->ppRows[(aRN)][(aCN)/ELEM_LEN]\
	|=0x01<<(aCN%ELEM_LEN))

//宏函数，用于将aRN行、aCN列设置为0
#define BM_CLEAR(aRN, aCN, pMat) \
	((pMat)->ppRows[(aRN)][(aCN)/ELEM_LEN]\
	&=~(0x01<<(aCN%ELEM_LEN)))

/*-------------------全局变量--------------*/

//0~255之间的数对应的1的个数
static UINT8 const g_aOnesNum[]={0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8};

/*-------------function define-----------*/

bitMatrix::bitMatrix(void)
{
}


bitMatrix::~bitMatrix(void)
{
}

//初始化，为S_BitMat的内部成员分配存储空间
//aRows: 矩阵的行数
//aCols: 矩阵的列数
//pMat: 比特级矩阵
//返回值: 成功，返回TRUE；失败，返回FALSE
BOOL bitMatrix::bm_init(UINT16 aRows, UINT16 aCols, PtrBitMat pMat)
{
	UINT32 i=0;

	pMat->aRows=aRows;
	pMat->aCols=aCols;
	pMat->aLen=(UINT16)ceil((double)(pMat->aCols)/sizeof(UINT32)/BYTE_LEN)*
		sizeof(UINT32)/sizeof(UINT8);
	pMat->pBitData=(UINT8*)malloc(pMat->aRows*pMat->aLen);
	pMat->ppRows=(UINT8**)malloc(pMat->aRows*sizeof(UINT8*));
	if(pMat->pBitData==NULL||pMat->ppRows==NULL)
	{
		return FALSE;
	}
	memset(pMat->pBitData,0,pMat->aRows*pMat->aLen);
	for(i=0;i<pMat->aRows;++i)
	{
		pMat->ppRows[i]=pMat->pBitData+i*pMat->aLen;
	}
	return TRUE;
}

//复制，将pSrc矩阵复制到pDst矩阵
//pSrc: 源矩阵
//pDst: 目标矩阵
//返回值: 当pSrc、pDst维数相同时，返回TRUE；反之，
//	返回FALSE
BOOL bitMatrix::bm_copyInit(PtrCnstBitMat pSrc, PtrBitMat pDst)
{

	if(!bm_init(pSrc->aRows,pSrc->aCols,pDst))
	{
		return FALSE;
	}
	memcpy(pDst->pBitData,pSrc->pBitData,pSrc->aRows*pSrc->aLen);	
	return TRUE;
}

//部分行复制，将pSrc的指定行复制到pDst。该方法会将pRowIdxs指示
//		的各行一次拷贝到pDst的0~aCount-1行
//pSrc: 源矩阵
//pRowIdxs: 指定行的下标
//aCount: pRowIdxs向量的长度
//pDst: 目标矩阵
//返回值: 成功，返回TRUE；失败，返回FALSE
BOOL bitMatrix::bm_copyPRInit(PtrCnstBitMat pSrc, UINT16 const *pRowIdxs, 
				   UINT16 aCount, PtrBitMat pDst)
{
	UINT16 i=0;
	if(!bm_init(aCount,pSrc->aCols,pDst))
	{
		return FALSE;
	}

	for(i=0;i<aCount;++i)
	{
		memcpy(pDst->ppRows[i],pSrc->ppRows[pRowIdxs[i]],
			pSrc->aLen);
	}
	return TRUE;
}

//终结，调用bm_inti的S_BitMat都需要调用该接口，
//	以释放内部各成员的矩阵，该接口可重复调用
//pMat: 比特级矩阵
//返回值: 无
void bitMatrix::bm_free(PtrBitMat pMat)
{
	if(pMat==NULL)
	{
		return;
	}
	if(pMat->pBitData!=NULL)
	{
		free(pMat->pBitData);
		pMat->pBitData=NULL;
	}
	if(pMat->ppRows!=NULL)
	{
		free(pMat->ppRows);
		pMat->ppRows=NULL;
	}
}

//修改制定行和列的元素
//aRN: 行数
//aC: 列数
//aValue: 插入的值
//pMat: 比特级矩阵
void bitMatrix::bm_setElem(UINT16 aRN, UINT16 aCN, 
				E_ElemType aValue, PtrBitMat pMat)
{
	if(aValue==bm_zero) 
	{
		BM_CLEAR(aRN,aCN,pMat);
	}
	else
	{
		BM_SET(aRN,aCN,pMat);
	}
}

//统计一行中指定列范围的1的个数
//aRN: 要统计的行号
//aCS: 要统计的起始列
//aCE: 要统计的终止列, aCE>=aCS
//pMat: 比特级矩阵
//返回值: 1的个数
UINT16 bitMatrix::bm_getRowOnes(UINT16 aRN, UINT16 aCS, 
					 UINT16 aCE, PtrCnstBitMat pMat)
{
	UINT16 aByteIdxS=aCS>>BYTE_LOGLEN, aByteIdxE=aCE>>BYTE_LOGLEN;
	UINT8 *pCur=pMat->ppRows[aRN];
	UINT8 tmp=pCur[aByteIdxE];
	UINT16 i=0,count=0;

	pCur[aByteIdxE]&=~(0xfe<<(aCE&BYTE_RES_MASK));
	for(i=aByteIdxS;i<=aByteIdxE;++i)
	{
		count+=g_aOnesNum[pCur[i]];
	}
	pCur[aByteIdxE]=tmp;
	return count;
}

//交换两行
//aR1、aR2: 待交换的两行
//pMat: 比特级矩阵
//返回值: 无
void bitMatrix::bm_rowExchange(UINT16 aR1, UINT16 aR2, PtrBitMat pMat)
{
	UINT8 *pTmp=pMat->ppRows[aR1];

	pMat->ppRows[aR1]=pMat->ppRows[aR2];
	pMat->ppRows[aR2]=pTmp;
}

//交换两列
//aCOut:矩阵更新时，V以外的列
//aCIn:	矩阵更新时，V以内的列
//aRS: 起始行
//aRE: 终止行
//pOnes: 修改V各行1的个数
//pMat: 比特级矩阵
//返回值: 无
void bitMatrix::bm_colExchange(UINT16 aCOut, UINT16 aCIn, 
					UINT16 aRS, UINT16 aRE, 
					UINT16 *pOnes, PtrBitMat pMat)
{
	UINT16 aByteIdx1=aCOut>>BYTE_LOGLEN, aByteIdx2=aCIn>>BYTE_LOGLEN;
	UINT8 aBitIdx1=aCOut&BYTE_RES_MASK, aBitIdx2=aCIn&BYTE_RES_MASK;
	UINT8 *pCurRow=NULL;

	UINT8 mask1=0x01<<aBitIdx1, mask2=0x01<<aBitIdx2;

	UINT16 i=0;

	for(i=aRS;i<=aRE;++i)
	{
		pCurRow=pMat->ppRows[i];

		pOnes[i]-=BM_GET_ELEM(aByteIdx1,aBitIdx1,pCurRow);

		if(BM_GET_ELEM(aByteIdx1,aBitIdx1,pCurRow)!=
			BM_GET_ELEM(aByteIdx2,aBitIdx2,pCurRow))
		{
			pCurRow[aByteIdx1]^=mask1;
			pCurRow[aByteIdx2]^=mask2;
		}
	}
}

//对aRSrc、aRDst进行异或运算，结果取代aRDst行
//aRSrc、aRDst: 用于计算的两行
//aCS: 起始列
//aCE: 终止列
//pMat: 比特级矩阵
//返回值: 无
void bitMatrix::bm_rowXor(UINT16 aRSrc, UINT16 aRDst, 
			   UINT16 aCS, UINT16 aCE,  PtrBitMat pMat)
{
	UINT16 i=0;
	UINT8 *pRSrc=pMat->ppRows[aRSrc];
	UINT8 *pRDst=pMat->ppRows[aRDst];
	UINT16 aByteIdxS=aCS>>BYTE_LOGLEN, aByteIdxE=aCE>>BYTE_LOGLEN;

	for(i=aByteIdxS;i<=aByteIdxE;++i)
	{
		pRDst[i]^=pRSrc[i];
	}
}

//将比特级矩阵转换为字节序列矩阵
//pMat: 比特级矩阵
//ppArrayMat: 字节序列矩阵，用户需要提供与pMat的行、
//	列匹配的空间
//返回值: 无
void bitMatrix::bm_bit2Arrays(PtrCnstBitMat pMat, UINT8** ppArrayMat)
{
	UINT32 i=0,j=0,m=0;
	UINT8 *pRow=NULL;
	for(i=0;i<pMat->aRows;++i)
	{
		for(j=0;j<pMat->aLen;++j)
		{
			pRow=pMat->ppRows[i];
			for(m=0;m<BYTE_LEN;++m)
			{
				if(j*BYTE_LEN+m>=pMat->aCols)
				{
					break;
				}
				if(BM_GET_ELEM(j,m,pRow))
				{
					ppArrayMat[i][j*BYTE_LEN+m]=1;
				}
				else
				{
					ppArrayMat[i][j*BYTE_LEN+m]=0;
				}
			}
		}
	}
}

//将字节序列矩阵转换为比特级矩阵
//ppArrayMat: 字节序列矩阵
//pMat: 比特级矩阵。字节序列矩阵的维数需要与比特级
//	矩阵的维数匹配
//返回值: 无
void bitMatrix::bm_arrays2Bit(UINT8 const * const * ppArrayMat,
				   PtrBitMat pMat)
{
	UINT32 i=0,j=0;
	for(i=0;i<pMat->aRows;++i)
	{
		for(j=0;j<pMat->aCols;++j)
		{
			if(ppArrayMat[i][j])
			{
				BM_SET(i,j,pMat);
			}
			else
			{
				BM_CLEAR(i,j,pMat);
			}
		}
	}
}

//矩阵求逆，使用初等行变换得到pSrcMat的逆，并存储在
//		pInv中
//pSrcMat: 待求逆的方阵
//pInv: pSrcMat的逆, pInv必须是方阵，且维数与pSrcMat
//		相同
//返回值: 成功，返回TRUE；失败，返回FALSE
BOOL bitMatrix::bm_inv(PtrCnstBitMat pSrcMat, PtrBitMat pInv)
{
	UINT16 m=0;
	INT32 i=0, j=0;
	S_BitMat tmpMat;
	UINT16 aByteIdx=0,aBitIdx=0;

#ifdef BIT_MAT_DEBUG
	UINT8 **ppMat=NULL;
	UINT16 ii=0,jj=0;
#endif

	if(pInv->aRows!=pInv->aCols||pSrcMat->aRows!=pSrcMat->aCols||
		pInv->aRows!=pSrcMat->aCols)
	{//维数判定
		return FALSE;
	}

	if(!bm_copyInit(pSrcMat,&tmpMat))
	{//复制原矩阵
		bm_free(&tmpMat);
		return FALSE;
	}

#ifdef BIT_MAT_DEBUG
	ppMat=(UINT8**)malloc(pSrcMat->aRows*sizeof(UINT8*));
	assert(ppMat!=NULL);
	for(ii=0;ii<pSrcMat->aRows;++ii)
	{
		ppMat[ii]=(UINT8*)malloc(pSrcMat->aCols*sizeof(UINT8));
		assert(ppMat[ii]!=NULL);
	}

	bm_bit2Arrays(&tmpMat,ppMat);
	printf("Src Mat:\n");
	for(ii=0;ii<tmpMat.aRows;++ii)
	{
		for(jj=0;jj<tmpMat.aCols;++jj)
		{
			printf("%u ",ppMat[ii][jj]);
		}
		printf("\n");
	}
	printf("\n");
#endif

	//将pInv初始化为单位矩阵
	memset(pInv->pBitData,0,pInv->aRows*pInv->aLen);
	for(i=0;i<pInv->aRows;++i)
	{
		BM_SET(i,i,pInv);
	}

	//使用初等行变换得到逆矩阵
	for(i=0;i<tmpMat.aCols;++i)
	{//得到上三角矩阵
		aByteIdx=i>>BYTE_LOGLEN;
		aBitIdx=i&BYTE_RES_MASK;
		for(j=i;j<tmpMat.aRows;++j)
		{
			if(BM_GET_ELEM(aByteIdx, aBitIdx,
				tmpMat.ppRows[j]))
			{
				m=j;
				break;
			}
		}
		if(j==tmpMat.aRows)
		{
			bm_free(&tmpMat);
			return FALSE;
		}

		for(j=m+1;j<tmpMat.aRows;++j)
		{
			if(BM_GET_ELEM(aByteIdx, aBitIdx,
				tmpMat.ppRows[j]))
			{
				bm_rowXor(m,j,i,tmpMat.aCols-1,&tmpMat);
				bm_rowXor(m,j,0,pInv->aCols-1,pInv);
			}
		}

		if(m!=i) 
		{
			bm_rowExchange(m,i,&tmpMat);
			bm_rowExchange(m,i,pInv);
		}
	}

	for(i=tmpMat.aCols-1;i>0;--i)
	{//把上三角矩阵化简为单位矩阵
		aByteIdx=i>>BYTE_LOGLEN;
		aBitIdx=i&BYTE_RES_MASK;
		for(j=i-1;j>=0;--j)
		{
			if(BM_GET_ELEM(aByteIdx,aBitIdx,tmpMat.ppRows[j]))
			{
				bm_rowXor(i,j,i,i,&tmpMat);
				bm_rowXor(i,j,0,pInv->aCols-1,pInv);
			}
		}
	}

#ifdef BIT_MAT_DEBUG
	ppMat=(UINT8**)malloc(pSrcMat->aRows*sizeof(UINT8*));
	assert(ppMat!=NULL);
	for(ii=0;ii<pSrcMat->aRows;++ii)
	{
		ppMat[ii]=(UINT8*)malloc(pSrcMat->aCols*sizeof(UINT8));
		assert(ppMat[ii]!=NULL);
	}

	bm_bit2Arrays(&tmpMat,ppMat);
	printf("Src Mat:\n");
	for(ii=0;ii<tmpMat.aRows;++ii)
	{
		for(jj=0;jj<tmpMat.aCols;++jj)
		{
			printf("%u ",ppMat[ii][jj]);
		}
		printf("\n");
	}
	printf("\n");

	bm_bit2Arrays(pInv,ppMat);
	printf("Inv Mat:\n");
	for(ii=0;ii<pInv->aRows;++ii)
	{
		for(jj=0;jj<pInv->aCols;++jj)
		{
			printf("%u ",ppMat[ii][jj]);
		}
		printf("\n");
	}
	printf("\n");

	//释放内存
	bm_freeDoubleP(ppMat,tmpMat.aRows);
#endif

	bm_free(&tmpMat);
	return TRUE;
}

//矩阵乘以向量
//pMat: 比特级矩阵
//pVec: 比特级向量
//pRst: 相乘结果，每一个字节表示1个比特，用户需要提供
//		长度为pMat->aRows的向量
//返回值: 无
void bitMatrix::bm_matXVec(PtrCnstBitMat pMat, UINT32 const *pVec, 
				UINT8 *pRst)
{
	UINT16 i=0, j=0, rEnd=(pMat->aCols-1)>>ELEM4_LOGLEN;
	UINT32 *pCur=NULL;
	UINT32 tmp=0;

	for(i=0;i<pMat->aRows;++i)
	{
		tmp=0;
		pCur=(UINT32*)(pMat->ppRows[i]);
		for(j=0;j<=rEnd;++j)
		{
			tmp^=(pCur[j]&pVec[j]);
		}
		pRst[i]=(g_aOnesNum[tmp&0x000000ff]+
			g_aOnesNum[(tmp>>8)&0x000000ff]+
			g_aOnesNum[(tmp>>16)&0x000000ff]+
			g_aOnesNum[(tmp>>24)&0x000000ff])&0x01;
	}
}

//将一个0/1序列压缩为一个比特级序列
//pVec: 0/1序列
//aCount: pVec的长度
//pBitVec: 比特级序列，每一个比特表示pVec的一个元素，用户需要提供
//		ceil(aCount/32)*4字节的存储空间
//返回值: 无
void bitMatrix::bm_getBitVec(UINT8 const *pVec, UINT16 aCount, UINT32 *pBitVec)
{
	UINT16 i=0, aByteIdx=0,aBitIdx=0;
	UINT8 *pTmp=(UINT8*)pBitVec;
	UINT16 aBVLen=(UINT16)ceil((double)aCount/ELEM4_LEN)*sizeof(UINT32);
	
	memset(pBitVec,0,aBVLen);
	for(i=0;i<aCount;++i)
	{
		aByteIdx=i>>BYTE_LOGLEN;
		aBitIdx=i&BYTE_RES_MASK;
		pTmp[aByteIdx]|=pVec[i]<<aBitIdx;
	}
}

//将一个比特级序列解压为0/1序列
//pBitVec: 0/1序列
//aCount: pBitVec中有效比特信息的长度
//pVec: 0/1序列，用户需要提供长度为aCount字节的存储空间
//返回值: 无
void bitMatrix::bm_getVec(UINT32 const *pBitVec, UINT16 aCount, UINT8 *pVec)
{
}

/*-------------------内部接口实现----------*/

#ifdef BIT_MAT_DEBUG

//释放指向UINT8的双重指针，当ppUINT8等于NULL时，该接口不工作
//ppUINT8: 双重指针
//aRows: 第一重指针的长度
//返回值: 无
static void bm_freeDoubleP(UINT8 **ppUINT8, UINT32 aRows)
{
	UINT32 i=0;
	if(ppUINT8==NULL)
	{
		return;
	}
	for(i=0;i<aRows;++i)
	{
		free(ppUINT8[i]);
		ppUINT8[i]=NULL;
	}
	free(ppUINT8);
}
#endif
