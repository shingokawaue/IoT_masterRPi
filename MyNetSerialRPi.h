//Raspberry pi用 シリアル通信用　パケット作成クラス
#ifndef _MYNETRPIH_
#define _MYNETRPIH_
//パケット構造
//0~7bit :ヘッダバイト0x7E 01111110
//9~13 : MCID 5bit 0~63(MicroComputerId 送信者識別No
//14~18: MCID 5bit 0~63 (MicroComputerId 受信者識別No
//19~22: SDPP  4bit (SendPurpose  送信目的コード :センサーの数値報告　異常検出　定期確認信号 命令　文章
//23~29: PID 7bit 0~127 (PartsId パーツ識別No
//30~35: DTCT 6bit 0~63 (DataContents) データ内容 :距離　温度　オンオフ
//36~39: DTFM 4bit 0~31 (DataForm)データ形式 : float byte配列 char char* など
//40~ データ
//:チェックサムフッタ

//この後aruduinoでパリティビット、ストップビットを加えるのでデータ形式は変わるはず
#include "SerialDataRPi.h"
#include <pigpiod_if2.h>
/*****************************************************************/
/* CONSTANTS                                                     */
/*****************************************************************/

struct CConst
{
    static const unsigned READSTOCK_NUM;
    static const unsigned MYDATA_NUM;
    static const int8_t IS_ALIVE_UNKNOWN;
    static const int8_t IS_ALIVE_PROBABLY_ALIVE;
    static const int8_t IS_ALIVE_DEFINITELY_ALIVE;
    static const int8_t IS_ALIVE_PROBABLY_DEAD;
    static const int8_t IS_ALIVE_DEFINITELY_DEAD;
    static const unsigned DEBUG_CHAR_LEN;
    static const unsigned ESCAPE_BYTE;//01111101
    static const unsigned ESCAPE_MASK;//00100000 ビットの排他論理和 ^ を２回すると同じ値に戻る!!
     
};


class MyNetSerialRPi:public BaseMyNetRPi{
public:
    /*****************************************************************/
    /* CONSTANTS                                                     */
    /*****************************************************************/
    static const int16_t BPS;
    static const float FLOAT_UNKNOWN;
    /*****************************************************************/
    /* ENUMERATIONS                                                  */
    /*****************************************************************/

    
    /*****************************************************************/
    /* STRUCTS                                                  */
    /*****************************************************************/
    
    /*****************************************************************/
    /*PUBLIC: INIT FUNCTIONS                                                */
    /*****************************************************************/
    
    /////////////////////////////////////////////////////////////////
    /// Constructor used to create the class.
    MyNetSerialRPi(int pipi,const unsigned myid);
    
    ~MyNetSerialRPi(){
        pigpio_stop(pi);
        delete[] mydata;
        delete[] readStock;
        delete[] mydata;
        serial_close(pi,handle);
    };
    
    
    /*****************************************************************/
    /*PUBLIC: UPDATE FUNCTIONS                                                */
    /*****************************************************************/
    
    SerialDataRPi* containedPick();
    
    /*****************************************************************/
    /*PUBLIC: SEND FUNCTIONS                                                */
    /*****************************************************************/
    void sendFloat(float val ,unsigned receiver ,unsigned sdpp=0, unsigned pid=0, unsigned dtct=0);
    void send(unsigned receiver ,unsigned sdpp, unsigned pid , unsigned dtct ,bool val);
    void sendInt8(int8_t val ,unsigned receiver ,unsigned sdpp=0, unsigned pid=0, unsigned dtct=0);
    void sendInt16(int16_t val ,unsigned receiver ,unsigned sdpp=0, unsigned pid=0, unsigned dtct=0);
    void sendUint32(uint32_t val,unsigned receiver ,unsigned sdpp=0, unsigned pid=0, unsigned dtct=0);
    void sendCharArray(char* val,unsigned receiver ,unsigned sdpp=0, unsigned pid=0, unsigned dtct=0);
    void send(unsigned receiver ,unsigned sdpp,unsigned pid = 0,unsigned dtct = 0);
    
    void sendUT(uint32_t unixtime ,unsigned receiver ,unsigned sdpp,unsigned pid = 0,unsigned dtct = 0);
    void sendInt16UT(uint32_t unixtime ,int16_t val ,unsigned receiver ,unsigned sdpp=0,unsigned pid=0,unsigned dtct=0);
    /*****************************************************************/
    /*PUBLIC: RECEIVE FUNCTIONS                                                */
    /*****************************************************************/
    int serialDataAvailable(){
        return serial_data_available(pi,handle);
    }
    
    const int receive();
    
    SerialDataRPi* pick(const int id){
        assert (id >= 0 && id < CConst::MYDATA_NUM);
        return &mydata[id];
    }
    /*****************************************************************/
    /* ACCESSOR FUNCTIONS                                            */
    /*****************************************************************/


    /*****************************************************************/
    /* FUNCTION POINTER FUNCTIONS                                            */
    /*****************************************************************/

protected://▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️
    


    /*****************************************************************/
    /*PROTECTED: VARIABLES                                                     */
    /*****************************************************************/
    int pi;//RPi
    unsigned handle;//RPi
    unsigned stockPos = 0;//readStockに有効な値が格納されている数
    
    SerialDataRPi* mydata = new SerialDataRPi[CConst::MYDATA_NUM];
    unsigned* readStock = new unsigned[CConst::READSTOCK_NUM];
    
    unsigned checksum = 0;
    std::vector<unsigned> longvalue;
    char* charbuf;//chararray送信用
    //以下送信用array
    unsigned first5Bytes[5];//[0]HEADBYTE[1]データ大別コード [2][3]データ長[4]flag
    //SRDPCF00 fixed contained Flag  S:Sender R:Receiver D:SDPP P:PID C:DTCT F:DTFM
    unsigned fixedBytes[FIXED_BYTES_SIZE];
    //S:Sender5bit R:Receiver5bit D:SDPP6bit P:PID7bit C:DTCT5bit F:DTFM4bit
    //first3[2]でフラグが立っているものを左詰 (余りは送信しない
    unsigned valueBytes[VALUE_BYTES_SIZE];//valueBytes送信用
    fixed_t fixed[DATA_ELEMENTS_NUM];
    //fixed  送信データ作成用array
    //   typedef struct{
    //        unsigned data = 0; unsigned len = 0;  unsigned flag = 0;
    //    }fixed_t;
    
    SerialDataRPi* emptyPick();
//    void setFixed(const unsigned sender ,const unsigned receiver , const unsigned sdpp, const unsigned pid , const unsigned dtct , const unsigned dtfm);
    Uint32_tToArray updateTime;
    
    
    void setFixed( unsigned sender , unsigned receiver ,  unsigned sdpp,  unsigned pid ,  unsigned dtct ,  unsigned dtfm);
    void createData (uint16_t size);
    void setFloat(float val);
    void setUint32(uint32_t val);
    void write(size_t size);
    
    const bool convertData();
    void escapeProcess(unsigned* readStock);//エスケープバイトの処理とチェックサムの計算ヘッダ消す
    
    /*****************************************************************/
    /*PROTECTED: UTILITY FUNCTIONS                                          */
    /*****************************************************************/
    void writeEscChk(unsigned val);
    void printBinary(char c);
    int8_t ser_shiftregi = -1;
    int8_t rclk_shiftregi = -1;
    int8_t srclk_shiftregi = -1;
    unsigned shiftregi_buf = 0;
    /*****************************************************************/
    /* HERITAGES                                            */
    /*****************************************************************/
    
    /*
     //void serialWrite(unsigned portid,unsigned val) const;
     //const unsigned serialRead(unsigned portid) const;
     //const unsigned serialAvailable(unsigned portid) const;
     
     Stream* getPortP(unsigned portid);
     
     
     
    */
    
};//End Of class


#endif /* _MYNETRPIH_ */
