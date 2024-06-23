#include "stdafx.h"
#include "CRC.h"

/*******************************************************************/
/*
  函 数 名 称:	BuildTable16
  功 能 描 述：	创建CRC16所需要的Table
  参 数 说 明：	aPoly[in]:创建表所需要的多项式
				Table_CRC[in][out]:Table表的buff

  返回值 说明：	void

  更 新 日 期：	2003.12.19
/*******************************************************************/
static void BuildTable16( unsigned short aPoly , unsigned long* Table_CRC )
{
    unsigned short i, j;
    unsigned short nData;
    unsigned short nAccum;

    for ( i = 0; i < 256; i++ )
    {
        nData = ( unsigned short )( i << 8 );
        nAccum = 0;
        for ( j = 0; j < 8; j++ )
        {
            if ( ( nData ^ nAccum ) & 0x8000 )
                nAccum = ( nAccum << 1 ) ^ aPoly;
            else
                nAccum <<= 1;
            nData <<= 1;
        }
        Table_CRC[i] = ( unsigned long )nAccum;
    }
}




/*******************************************************************/
/*
	函 数 名 称:	BuildTable32
	功 能 描 述：	创建CRC32所需要的Table
	参 数 说 明：	aPoly[in]:创建表所需要的多项式
					Table_CRC[in][out]:Table表的buff

	  返回值 说明：	void

/*******************************************************************/
static void BuildTable32( unsigned long aPoly , unsigned long* Table_CRC )
{
    unsigned long i, j;
    unsigned long nData;
    unsigned long nAccum;

    for ( i = 0; i < 256; i++ )
    {
        nData = ( unsigned long )( i << 24 );
        nAccum = 0;
        for ( j = 0; j < 8; j++ )
        {
            if ( ( nData ^ nAccum ) & 0x80000000 )
                nAccum = ( nAccum << 1 ) ^ aPoly;
            else
                nAccum <<= 1;
            nData <<= 1;
        }
        Table_CRC[i] = nAccum;
    }
}





/*******************************************************************/
/*
	函 数 名 称:	RunCRC16
	功 能 描 述：	执行对数据段的CRC16循环冗余校验
	参 数 说 明：	aData[in]:待校验数据
					aSize[in]:待校验数据长度
					aPoly[in]:创建表所需要的多项式
					
	  返回值 说明：	循环冗余校验结果

/*******************************************************************/
unsigned short CCRC::RunCRC16( const  char * aData, unsigned long aSize, unsigned short aPoly )
{
    unsigned long Table_CRC[256]; // CRC 表
    unsigned long  i;
    unsigned short nAccum = 0;

	BuildTable16( aPoly, Table_CRC );
    
    for ( i = 0; i < aSize; i++ )
        nAccum = ( nAccum << 8 ) ^ ( unsigned short )Table_CRC[( nAccum >> 8 ) ^ *aData++];
    return nAccum;
}





/*******************************************************************/
/*
	函 数 名 称:	RunCRC32
	功 能 描 述：	执行对数据段的CRC32循环冗余校验
	参 数 说 明：	aData[in]:待校验数据
					aSize[in]:待校验数据长度
					aPoly[in]:创建表所需要的多项式
					
	  返回值 说明：	循环冗余校验结果

/*******************************************************************/
unsigned long CCRC::RunCRC32( const  char * aData, unsigned long aSize, unsigned long aPoly )
{
    unsigned long Table_CRC[256]; // CRC 表
    unsigned long i;
    unsigned long nAccum = 0;

	BuildTable32( aPoly, Table_CRC );
    
    for ( i = 0; i < aSize; i++ )
        nAccum = ( nAccum << 8 ) ^ Table_CRC[( nAccum >> 24 ) ^ *aData++];
    return nAccum;
}
