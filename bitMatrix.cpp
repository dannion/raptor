#include "bitMatrix.h"
#include <malloc.h>
#include <cmath>
#include <string>
#include <string.h>

#ifdef BIT_MAT_DEBUG
#include <stdio.h>
#include <assert.h>
#endif

/*-------------------�궨��----------------*/

//#define BIT_MAT_DEBUG

#define ELEM_LEN 8		//����һ���洢��Ԫռ�õ�����
#define BYTE_LEN 8		//����һ��BYTE��λ��
#define BYTE_LOGLEN 3		//����һ���ֽڵı��س��ȵ�log2ֵ
#define BYTE_RES_MASK 0x0007	//��������

#define ELEM4_LEN 32	//����4�ֽڵ�λ��
#define ELEM4_LOGLEN 5	//����4�ֽڵ�LOG2λ��
#define ELEM4_RES_MASK 0x0000001f	//4�ֽ���������

#define ROW_END 65535	//��β��־

//�꺯�������ڽ�aRN�С�aCN������Ϊ1
#define BM_SET(aRN, aCN, pMat) \
	((pMat)->ppRows[(aRN)][(aCN)/ELEM_LEN]\
	|=0x01<<(aCN%ELEM_LEN))

//�꺯�������ڽ�aRN�С�aCN������Ϊ0
#define BM_CLEAR(aRN, aCN, pMat) \
	((pMat)->ppRows[(aRN)][(aCN)/ELEM_LEN]\
	&=~(0x01<<(aCN%ELEM_LEN)))

/*-------------------ȫ�ֱ���--------------*/

//0~255֮�������Ӧ��1�ĸ���
static UINT8 const g_aOnesNum[]={0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8};

/*-------------function define-----------*/

bitMatrix::bitMatrix(void)
{
}


bitMatrix::~bitMatrix(void)
{
}

//��ʼ����ΪS_BitMat���ڲ���Ա����洢�ռ�
//aRows: ���������
//aCols: ���������
//pMat: ���ؼ�����
//����ֵ: �ɹ�������TRUE��ʧ�ܣ�����FALSE
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

//���ƣ���pSrc�����Ƶ�pDst����
//pSrc: Դ����
//pDst: Ŀ�����
//����ֵ: ��pSrc��pDstά����ͬʱ������TRUE����֮��
//	����FALSE
BOOL bitMatrix::bm_copyInit(PtrCnstBitMat pSrc, PtrBitMat pDst)
{

	if(!bm_init(pSrc->aRows,pSrc->aCols,pDst))
	{
		return FALSE;
	}
	memcpy(pDst->pBitData,pSrc->pBitData,pSrc->aRows*pSrc->aLen);	
	return TRUE;
}

//�����и��ƣ���pSrc��ָ���и��Ƶ�pDst���÷����ὫpRowIdxsָʾ
//		�ĸ���һ�ο�����pDst��0~aCount-1��
//pSrc: Դ����
//pRowIdxs: ָ���е��±�
//aCount: pRowIdxs�����ĳ���
//pDst: Ŀ�����
//����ֵ: �ɹ�������TRUE��ʧ�ܣ�����FALSE
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

//�սᣬ����bm_inti��S_BitMat����Ҫ���øýӿڣ�
//	���ͷ��ڲ�����Ա�ľ��󣬸ýӿڿ��ظ�����
//pMat: ���ؼ�����
//����ֵ: ��
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

//�޸��ƶ��к��е�Ԫ��
//aRN: ����
//aC: ����
//aValue: �����ֵ
//pMat: ���ؼ�����
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

//ͳ��һ����ָ���з�Χ��1�ĸ���
//aRN: Ҫͳ�Ƶ��к�
//aCS: Ҫͳ�Ƶ���ʼ��
//aCE: Ҫͳ�Ƶ���ֹ��, aCE>=aCS
//pMat: ���ؼ�����
//����ֵ: 1�ĸ���
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

//��������
//aR1��aR2: ������������
//pMat: ���ؼ�����
//����ֵ: ��
void bitMatrix::bm_rowExchange(UINT16 aR1, UINT16 aR2, PtrBitMat pMat)
{
	UINT8 *pTmp=pMat->ppRows[aR1];

	pMat->ppRows[aR1]=pMat->ppRows[aR2];
	pMat->ppRows[aR2]=pTmp;
}

//��������
//aCOut:�������ʱ��V�������
//aCIn:	�������ʱ��V���ڵ���
//aRS: ��ʼ��
//aRE: ��ֹ��
//pOnes: �޸�V����1�ĸ���
//pMat: ���ؼ�����
//����ֵ: ��
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

//��aRSrc��aRDst����������㣬���ȡ��aRDst��
//aRSrc��aRDst: ���ڼ��������
//aCS: ��ʼ��
//aCE: ��ֹ��
//pMat: ���ؼ�����
//����ֵ: ��
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

//�����ؼ�����ת��Ϊ�ֽ����о���
//pMat: ���ؼ�����
//ppArrayMat: �ֽ����о����û���Ҫ�ṩ��pMat���С�
//	��ƥ��Ŀռ�
//����ֵ: ��
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

//���ֽ����о���ת��Ϊ���ؼ�����
//ppArrayMat: �ֽ����о���
//pMat: ���ؼ������ֽ����о����ά����Ҫ����ؼ�
//	�����ά��ƥ��
//����ֵ: ��
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

//�������棬ʹ�ó����б任�õ�pSrcMat���棬���洢��
//		pInv��
//pSrcMat: ������ķ���
//pInv: pSrcMat����, pInv�����Ƿ�����ά����pSrcMat
//		��ͬ
//����ֵ: �ɹ�������TRUE��ʧ�ܣ�����FALSE
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
	{//ά���ж�
		return FALSE;
	}

	if(!bm_copyInit(pSrcMat,&tmpMat))
	{//����ԭ����
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

	//��pInv��ʼ��Ϊ��λ����
	memset(pInv->pBitData,0,pInv->aRows*pInv->aLen);
	for(i=0;i<pInv->aRows;++i)
	{
		BM_SET(i,i,pInv);
	}

	//ʹ�ó����б任�õ������
	for(i=0;i<tmpMat.aCols;++i)
	{//�õ������Ǿ���
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
	{//�������Ǿ��󻯼�Ϊ��λ����
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

	//�ͷ��ڴ�
	bm_freeDoubleP(ppMat,tmpMat.aRows);
#endif

	bm_free(&tmpMat);
	return TRUE;
}

//�����������
//pMat: ���ؼ�����
//pVec: ���ؼ�����
//pRst: ��˽����ÿһ���ֽڱ�ʾ1�����أ��û���Ҫ�ṩ
//		����ΪpMat->aRows������
//����ֵ: ��
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

//��һ��0/1����ѹ��Ϊһ�����ؼ�����
//pVec: 0/1����
//aCount: pVec�ĳ���
//pBitVec: ���ؼ����У�ÿһ�����ر�ʾpVec��һ��Ԫ�أ��û���Ҫ�ṩ
//		ceil(aCount/32)*4�ֽڵĴ洢�ռ�
//����ֵ: ��
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

//��һ�����ؼ����н�ѹΪ0/1����
//pBitVec: 0/1����
//aCount: pBitVec����Ч������Ϣ�ĳ���
//pVec: 0/1���У��û���Ҫ�ṩ����ΪaCount�ֽڵĴ洢�ռ�
//����ֵ: ��
void bitMatrix::bm_getVec(UINT32 const *pBitVec, UINT16 aCount, UINT8 *pVec)
{
}

/*-------------------�ڲ��ӿ�ʵ��----------*/

#ifdef BIT_MAT_DEBUG

//�ͷ�ָ��UINT8��˫��ָ�룬��ppUINT8����NULLʱ���ýӿڲ�����
//ppUINT8: ˫��ָ��
//aRows: ��һ��ָ��ĳ���
//����ֵ: ��
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
