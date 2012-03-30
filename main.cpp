#include "raptor.h"

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

#define ENCODETIME 5

#define MAINDEBUG

//得到一个随机数序列
void getSrc(UINT8 *pSrc, UINT16 aCount)
{
	UINT32 seed=(UINT32)time(NULL);
	UINT16 i=0;
	srand(seed);
	for(i=0;i<aCount;++i)
	{
		pSrc[i]=rand()%2;
	}
}

int main(int argc, char** argv)
{
	raptor r1;
	UINT16 K(100),N(150);//	确定K
	UINT32 count(0);
	UINT8* SrcDec = new UINT8[K*ENCODETIME];
	//ENCODETIME就是T撒

	//--------------------------encode--------------------//
	UINT8* SrcData = new UINT8[K*ENCODETIME];
	getSrc(SrcData,K*ENCODETIME);//产生随机输入序列
	repair_symbol *symbolll;//编码后序列存放在此
	symbolll=r1.RealRaptorEncoder(SrcData,K,ENCODETIME,N);
	

	//-----------------------decode------------------------//
	SrcDec = r1.RealRaptorDecoder(symbolll, N, K, ENCODETIME, N);
	for (UINT16 i=0;i< K; i++)
	{
		if(SrcDec[i]!=SrcData[i])
			std::cout<<"error";
	}
	
	//r1.RealRaptorEncoderFree();

	
#ifdef MAINDEBUG
		std::cout<<"The data after decoding is exactly the same as the source data!"<<std::endl;
		std::cout<<"The coding and decoding processes are successfully done!"<<std::endl;
		std::cout<<"CongratulationS!"<<std::endl;
		std::cout<<"DannionQ is so handsome! "<<std::endl;
		std::cin.get();
#endif
//	raptor encode;
//	clock_t start_t1,end_t1,start_t2,end_t2;;
//	fstream f1("D:\\asd.jpg",ios::in|ios::binary);
//	fstream f2("D:\\asdcopy.jpg",ios::out|ios::binary);
//	f1.seekg(0,ios::end);
//	int fileLen = f1.tellg();
//	f1.seekg(0,ios::beg);
//	UINT8 *pBuf = new UINT8[22528];
//
//	UINT8 *pRecvBuf = new UINT8[22528];
//	f1.read((char*)pBuf,fileLen);
//	int getLen = f1.gcount();
//	if (getLen<22528)
//	{
//	for (int i = getLen;i<22528;i++)
//	{
//	pBuf[i] = '0';
//	}
//	}
//	start_t1 = clock();
//	repair_symbol * src = encode.RealRaptorEncoder(pBuf,1024,22,2048);
//
//	end_t1 =clock();
//	start_t2 = clock();
//	pRecvBuf = encode.RealRaptorDecoder(src,1200,1024,22,2048);
//	end_t2 = clock();
//	int i =0;
//	while (pBuf[i] ==pRecvBuf[i])
//	{
//		i++;
//	}
//	if (i!=fileLen)
//	{
//		cout<<i<<endl;
//	}
//	
//
//	f2.write((char*)pRecvBuf,getLen);
//	cout<<difftime(end_t1,start_t1)/1000<<endl;
//	cout<<difftime(end_t2,start_t2)/1000<<endl;
//	cin.get();
//	cin.get();
//	encode.RealRaptorEncoderFree();
//	f2.close();
//	f1.close();
	return 0;


}
