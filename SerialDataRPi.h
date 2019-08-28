#ifndef _SERIALDATARPI_H_
#define _SERIALDATARPI_H_


//Raspberry pi用 シリアル通信用　データ保管クラス

#include "BaseMyNetRPi.h"
#include <stdarg.h>
#include <vector>
#include <map>
#include <iterator>
#include <bitset>
#include <cassert>

union Uint32_tToArray{
    unsigned array[4];
    uint32_t ui32;
};
union Float_ToArray{
    unsigned array[4];
    float f;
};
#define FIXED_BYTES_SIZE 4
#define VALUE_BYTES_SIZE 8
#define HEAD_BYTE (unsigned)0x7E //01111110
#define FOOT_BYTE 0x8E //10001110

//01011101 1回目　(ESCAPE_BYTEとESCAPE_MASKの排他論理和)
//01111101 2回目

//データ構造
//[0] 0b01111110    ヘッダバイト　  0x7e （共通）
//[1] 0bDDDLLLLL    D:データ大別コード3bit 0~7
//                  L:データ長 5bit 0~31 MyNetFixedの場合可変長データバイト数
//                                      その他：ヘッダバイトからフットバイトまでの長さ
//データ大別コード 4bit 0~15
#define DIIVISION_CODE_NONE 0x00
#define DIIVISION_CODE_MYNET_FIXED 0x01 //fixedFormData
#define DIIVISION_CODE_MYDATA_REPORT 0x02
#define DIIVISION_CODE_MYNET_FIXED_WITH_UNIXTIME 0x03 //fixedFormData
//DIIVISION_CODE_MYNET_FIXEDの場合↓
//[2] 0bSRDPCF00 FixedFlag  S:Sender R:Receiver D:SDPP P:PID C:DTCT F:DTFM

//fixed flag
#define FLAG_SENDER (1 << 7)
#define FLAG_RECEIVER (1 << 6)
#define FLAG_SDPP (1 << 5)
#define FLAG_PID (1 << 4)
#define FLAG_DTCT (1 << 3)
#define FLAG_DTFM (1 << 2)
static const unsigned fixedflag(int index){
    static const unsigned a[] = {
        FLAG_SENDER,
        FLAG_RECEIVER,
        FLAG_SDPP,
        FLAG_PID,
        FLAG_DTCT,
        FLAG_DTFM
    };
    return a[index];
}

//fixedbitlen
#define MCID_LEN 5
#define SDPP_LEN 6
#define PID_LEN 7
#define DTCT_LEN 5
#define DTFM_LEN 4
static const unsigned fixedbitlen(int index){
    static const unsigned a[] = {
        MCID_LEN,
        MCID_LEN,
        SDPP_LEN,
        PID_LEN,
        DTCT_LEN,
        DTFM_LEN
    };
    return a[index];
}



struct fixed_t{
    unsigned data = 0;
    unsigned len = 0;
    unsigned flag = 0;
};


//isContaind
#define IS_CONTAINED_NO 0x00
#define IS_CONTAINED_YES 0x01
#define IS_CONTAINED_ERROR 0x02

class SerialDataRPi {
    
public:
    /*****************************************************************/
    /* INIT FUNCTIONS                                                */
    /*****************************************************************/
    /////////////////////////////////////////////////////////////////
    /// Constructor used to create the class.
    /// All parameters have default values.
    SerialDataRPi();
    
    /*****************************************************************/
    /* FUNCTIONS                                                */
    /*****************************************************************/
    void fixedToData(unsigned* readStock);
    void valueToData(unsigned* readStock,uint16_t valuelen);
    void charArrayToData(unsigned* readStock,uint16_t valuelen);
    void unixtimeToData(unsigned* readStock);
    void longvalueToData(unsigned* bytes);
    
    
    /*****************************************************************/
    /* ACCESSOR FUNCTIONS                                            */
    /*****************************************************************/
    void setContained(bool val){ contained = val;}
    
    unsigned sender() const {return fixedDataLen[DATA_SENDER].first; };
    unsigned receiver() const {return fixedDataLen[DATA_RECEIVER].first; };
    unsigned sdpp() const {return fixedDataLen[DATA_SDPP].first; };
    unsigned pid() const {return fixedDataLen[DATA_PID].first; };
    const char* pidchar() const {return pidName(fixedDataLen[DATA_PID].first);}
    unsigned dtct() const {return fixedDataLen[DATA_DTCT].first; };
    unsigned dtfm() const {return fixedDataLen[DATA_DTFM].first; };
    unsigned value(int id) const {return values[id]; };
    
#ifndef __arm__
    int16_t int16() const {return (int16_t)makeWord(values[1],  values[0]); }
#endif
    
    uint32_t uint32() const;
    char* charHeap() const {return char_heap;};
    float float_() const;
    uint32_t updatetime() const {return updateTime;};
    const bool isContained() const {return contained;};
    
    void clear(){
        for (int i = 0; i < DATA_ELEMENTS_NUM ; i++){
            fixedDataLen[i].first = 0;
        }
        for (unsigned x : values){
            x = 0;
        }
        updateTime = 0;
        longvalue.clear();
    }
    static void debugSerial();
private:
    /*****************************************************************/
    /* CONSTANTS                                                     */
    /*****************************************************************/
    static bool debugSerialPrint;
    
    
    
    //methods
    const unsigned bitsetTounsigned(unsigned fixedDataLenId);
    
    //variables
    std::pair<unsigned,unsigned> fixedDataLen[DATA_ELEMENTS_NUM];
    unsigned values[8];
    uint32_t updateTime = 0;//データがアップデートされた時間　unixtime(second)
    std::vector<unsigned> longvalue;
    char* char_heap = NULL;
    bool contained = false;
    unsigned readPos;//データを読み込む時の読み込み位置 （読み込んだ位置
};

inline SerialDataRPi::SerialDataRPi()
{
    for (int i = 0; i < DATA_ELEMENTS_NUM ; i++){
        fixedDataLen[i].first = 0;
        fixedDataLen[i].second = fixedbitlen(i);
    }
}


#endif /* _SERIALDATARPI_H_ */
