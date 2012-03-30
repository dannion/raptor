#pragma once


#include "var_def.h"

//�꺯��������ȡpCurRow�ĵ�aByteIdx�ֽڵĵ�aBitIdxλ
#define BM_GET_ELEM(aByteIdx, aBitIdx, pCurRow) \
	(((pCurRow)[(aByteIdx)]>>(aBitIdx))&0x01)

//��������Ϣ�洢�ľ���
typedef struct BitMatrix_Tag
{
	UINT16 aRows;		//���������
	UINT16 aCols;		//���������
	UINT16 aLen;		//����һ�еĳ���(Bytes)
	UINT8 *pBitData;	//�����ش洢������Ϣ����4�ֽڶ���
	UINT8 **ppRows;		//�洢���е���ʼ��ַ
}S_BitMat,*PtrBitMat;

typedef struct BitMatrix_Tag const *PtrCnstBitMat;

typedef enum ElementType_Tag{bm_zero, bm_one}E_ElemType;

class bitMatrix
{
public:
	bitMatrix(void);
	~bitMatrix(void);

	//��ʼ����ΪS_BitMat���ڲ���Ա����洢�ռ�
	//aRows: ���������
	//aCols: ���������
	//pMat: ���ؼ�����
	//����ֵ: �ɹ�������TRUE��ʧ�ܣ�����FALSE
	BOOL bm_init(UINT16 aRows, UINT16 aCols, PtrBitMat pMat);

	//�սᣬ����bm_inti��S_BitMat����Ҫ���øýӿڣ�
	//	���ͷ��ڲ�����Ա�ľ��󣬸ýӿڿ��ظ�����
	//pMat: ���ؼ�����
	//����ֵ: ��
	void bm_free(PtrBitMat pMat);

	//���ƣ���pSrc�����Ƶ�pDst����
	//pSrc: Դ����
	//pDst: Ŀ�����
	//����ֵ: �ɹ�������TRUE��ʧ�ܣ�����FALSE
	BOOL bm_copyInit(PtrCnstBitMat pSrc, PtrBitMat pDst);

	//�����и��ƣ���pSrc��ָ���и��Ƶ�pDst
	//pSrc: Դ����
	//pRowIdxs: ָ���е��±�
	//aCount: pRowIdxs�����ĳ���
	//pDst: Ŀ�����
	//����ֵ: �ɹ�������TRUE��ʧ�ܣ�����FALSE
	BOOL bm_copyPRInit(PtrCnstBitMat pSrc, UINT16 const *pRowIdxs, 
					   UINT16 aCount, PtrBitMat pDst);

	//�޸��ƶ��к��е�Ԫ��
	//aRN: ����
	//aC: ����
	//aValue: �����ֵ
	//pMat: ���ؼ�����
	void bm_setElem(UINT16 aRN, UINT16 aCN, 
					E_ElemType aValue, PtrBitMat pMat);

	//ͳ��һ����ָ���з�Χ��1�ĸ���
	//aRN: Ҫͳ�Ƶ��к�
	//aCS: Ҫͳ�Ƶ���ʼ��
	//aCE: Ҫͳ�Ƶ���ֹ��, aCE>=aCS
	//pMat: ���ؼ�����
	//����ֵ: 1�ĸ���
	UINT16 bm_getRowOnes(UINT16 aRN, UINT16 aCS, 
						 UINT16 aCE, PtrCnstBitMat pMat);

	//��������
	//aR1��aR2: ������������
	//pMat: ���ؼ�����
	//����ֵ: ��
	void bm_rowExchange(UINT16 aR1, UINT16 aR2, PtrBitMat pMat);

	//��������
	//aCOut:�������ʱ��V�������
	//aCIn:	�������ʱ��V���ڵ���
	//aRS: ��ʼ��
	//aRE: ��ֹ��
	//pOnes: �޸�V����1�ĸ���
	//pMat: ���ؼ�����
	//����ֵ: ��
	void bm_colExchange(UINT16 aCOut, UINT16 aCIn, 
						UINT16 aRS, UINT16 aRE, 
						UINT16 *pOnes, PtrBitMat pMat);

	//��aRSrc��aRDst����������㣬���ȡ��aRDst��
	//aRSrc��aRDst: ���ڼ��������
	//aCS: ��ʼ��
	//aCE: ��ֹ��
	//pMat: ���ؼ�����
	//����ֵ: ��
	void bm_rowXor(UINT16 aRSrc, UINT16 aRDst, 
				   UINT16 aCS, UINT16 aCE, PtrBitMat pMat);

	//�����ؼ�����ת��Ϊ�ֽ����о���
	//pMat: ���ؼ�����
	//ppArrayMat: �ֽ����о���
	//����ֵ: ��
	void bm_bit2Arrays(PtrCnstBitMat pMat, UINT8** ppArrayMat);

	//���ֽ����о���ת��Ϊ���ؼ�����
	//ppArrayMat: �ֽ����о���
	//pMat: ���ؼ������ֽ����о����ά����Ҫ����ؼ�
	//	�����ά��ƥ��
	//����ֵ: ��
	void bm_arrays2Bit(UINT8 const * const* ppArrayMat,
					   PtrBitMat pMat);

	//�������棬ʹ�ó����б任�õ�pSrcMat���棬���洢��
	//		pInv��
	//pSrcMat: ������ķ���
	//pInv: pSrcMat����
	//����ֵ: �ɹ�������TRUE��ʧ�ܣ�����FALSE
	BOOL bm_inv(PtrCnstBitMat pSrcMat, PtrBitMat pInv);

	//�����������
	//pMat: ���ؼ�����
	//pVec: ���ؼ�����
	//pRst: ��˽����ÿһ���ֽڱ�ʾ1�����أ��û���Ҫ�ṩ
	//		����ΪpMat->aRows������
	//����ֵ: ��
	void bm_matXVec(PtrCnstBitMat pMat, UINT32 const *pVec, 
					UINT8 *pRst);

	//��һ��0/1����ѹ��Ϊһ�����ؼ�����
	//pVec: 0/1����
	//aCount: pVec�ĳ���
	//pBitVec: ���ؼ����У�ÿһ�����ر�ʾpVec��һ��Ԫ�أ��û���Ҫ�ṩ
	//		ceil(aCount/32)*4�ֽڵĴ洢�ռ�
	//����ֵ: ��
	void bm_getBitVec(UINT8 const *pVec, UINT16 aCount, UINT32 *pBitVec);

	//��һ�����ؼ����н�ѹΪ0/1����
	//pBitVec: 0/1����
	//aCount: pBitVec����Ч������Ϣ�ĳ���
	//pVec: 0/1���У��û���Ҫ�ṩ����ΪaCount�ֽڵĴ洢�ռ�
	//����ֵ: ��
	void bm_getVec(UINT32 const *pBitVec, UINT16 aCount, UINT8 *pVec);

};

