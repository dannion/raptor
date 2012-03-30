#ifndef VAC_DEF_H
#define VAC_DEF_H

typedef	unsigned char		UCHAR;
typedef	unsigned char		UINT8;
typedef	unsigned short		UINT16;
typedef	unsigned int		UINT32;
typedef	unsigned long long	UINT64; //powerpc���ܲ�֧��

/****�з���������****/
typedef	char			CHAR;
typedef	signed char		INT8;
typedef	short			INT16;
typedef	int				INT32;
typedef	long long		INT64;

/****������������****/

#define TRUE 1
#define FALSE 0

#ifndef BOOL
	typedef int BOOL;
#endif


#ifndef NULL
	#define NULL 0
#endif

#endif