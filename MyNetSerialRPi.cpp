//Arduino用のMyNetSerialをラズパイ用に改変したもの。(pigpioライブラリ使用)
//GPIOのピンを使う。
#include "MyNetSerialRPi.h"
#include <iostream>
#include <string.h>
using namespace std;
//----------------------------------------------------------------------------------
//パケット構造
//[0] 0b01111110    ヘッダバイト　  0x7e （共通）
//[1] 0bDDDDDDDD   D:データ大別コード3bit 0~7
//[2] 0bLLLLLLLL   L:データ長 MyNetFixedの場合可変長データバイト数
//[3] 0bLLLLLLLL
//[4]
//[5]〜fixedData
//S:Sender5bit R:Receiver5bit D:SDPP6bit P:PID7bit C:DTCT5bit F:DTFM4bit
//フラグが立っているものを左詰 (余りは送信しない

//#define DIIVISION_CODE_NONE 0x00
//#define DIIVISION_CODE_MYNET_FIXED 0x01 //fixedFormData
//#define DIIVISION_CODE_MYDATA_REPORT 0x02


//return value :true converted : false array was full
//----------------------------------------------------------------------------------



/*****************************************************************/
/* CONSTANTS                                                     */
/*****************************************************************/
const unsigned CConst::READSTOCK_NUM = 200;
const unsigned CConst::MYDATA_NUM = 4;
const int8_t CConst::IS_ALIVE_UNKNOWN = 0;
const int8_t CConst::IS_ALIVE_PROBABLY_ALIVE = 1;
const int8_t CConst::IS_ALIVE_DEFINITELY_ALIVE = 2;
const int8_t CConst::IS_ALIVE_PROBABLY_DEAD  = -1;
const int8_t CConst::IS_ALIVE_DEFINITELY_DEAD = -2;
const unsigned CConst::DEBUG_CHAR_LEN = 20;
const unsigned CConst::ESCAPE_BYTE = 0x7D;//01111101
const unsigned CConst::ESCAPE_MASK = 0x20;//00100000 ビットの排他論理和 ^ を２回すると同じ値に戻る!!
const int16_t MyNetSerialRPi::BPS = 9600;
const float MyNetSerialRPi::FLOAT_UNKNOWN = 99999.99f;
/*****************************************************************/
/*PUBLIC: INIT FUNCTIONS                                                */
/*****************************************************************/
MyNetSerialRPi::MyNetSerialRPi(const unsigned myid):BaseMyNetRPi(myid){
    for (int i = 0; i < DATA_ELEMENTS_NUM ; i++){
        fixed[i].data = 0;
        fixed[i].len = fixedbitlen(i);
        fixed[i].flag = fixedflag(i);
    }
    updateTime.ui32 = 0;
    
    pi = pigpio_start(0,0);
    char ser_tty[] = "/dev/ttyS0";
    handle = serial_open(pi,ser_tty,9600,0);
    if (handle < 0){
        cout << "error!! can't open serial. handle is not good.";
    }
};
/*****************************************************************/
/*PUBLIC: UPDATE FUNCTIONS                                                */
/*****************************************************************/



SerialDataRPi* MyNetSerialRPi::containedPick(){
    
    for(int i = 0; i < CConst::MYDATA_NUM ; i++){
        if(mydata[i].isContained() == true){
            mydata[i].setContained(false);
            return &mydata[i];
        }
    }
    return NULL;
}


/*****************************************************************/
/*PUBLIC: SEND FUNCTIONS                                                */
/*****************************************************************/

void MyNetSerialRPi::sendFloat(float val, unsigned receiver ,unsigned sdpp,unsigned pid ,unsigned dtct){
    if (isDebugSerial()){ cout << "「[[send:float]]" << endl;}
    setFixed(myid , receiver , sdpp, pid , dtct ,DTFM_FLOAT);
    createData(sizeof(float));
    setFloat(val);
    updateTime.ui32 = 0;
    write(sizeof(float));
}
void MyNetSerialRPi::send(unsigned receiver ,unsigned sdpp,unsigned pid ,unsigned dtct ,bool val){
    if (isDebugSerial()){ cout << "[[send:bool]]" << endl;}
    setFixed(myid , receiver , sdpp, pid , dtct ,DTFM_BOOL);
    createData(sizeof(bool));
    valueBytes[0] = (unsigned)val;
    updateTime.ui32 = 0;
    write(sizeof(bool));
}
void MyNetSerialRPi::sendInt8(int8_t val,unsigned receiver ,unsigned sdpp,unsigned pid ,unsigned dtct){
    if (isDebugSerial()){ cout << "[[send:int8]]" << endl;}
    setFixed(myid , receiver , sdpp, pid , dtct ,DTFM_INT8_T);
    createData(sizeof(int8_t));
    valueBytes[0] = val;
    updateTime.ui32 = 0;
    write(sizeof(int8_t));
}
void MyNetSerialRPi::sendInt16(int16_t val,unsigned receiver ,unsigned sdpp,unsigned pid ,unsigned dtct){
    if (isDebugSerial()){ cout << "[[send:int16]]" << endl;}
    setFixed(myid , receiver , sdpp, pid , dtct ,DTFM_INT16_T);
    createData(sizeof(int16_t));
    valueBytes[0] = (val & 0xff);//low
    valueBytes[1] = (val & 0xff00) >> 8;//high
    updateTime.ui32 = 0;
    write(sizeof(int16_t));
}
void MyNetSerialRPi::sendUint32(uint32_t val,unsigned receiver ,unsigned sdpp,unsigned pid ,unsigned dtct){
    if (isDebugSerial()){ cout << "[[sendUint32]]" << endl;}
    setFixed(myid , receiver , sdpp, pid , dtct ,DTFM_UINT32_T);
    createData(sizeof(uint32_t));
    setUint32(val);
    updateTime.ui32 = 0;
    write(sizeof(uint32_t));
}

void MyNetSerialRPi::sendCharArray(char* val,unsigned receiver ,unsigned sdpp,unsigned pid ,unsigned dtct){
    if (isDebugSerial()){
        cout << "[[sendCharArray]] strlen(val):" << endl;
        cout << strlen(val) << endl;
        cout << "  char* is bottom line." << endl;
        cout << val << endl;
    }
    setFixed(myid , receiver , sdpp, pid , dtct ,DTFM_CHAR_ARRAY);
    createData(strlen(val));
    charbuf = val;
    updateTime.ui32 = 0;
    write(strlen(val));
}

void MyNetSerialRPi::send(unsigned receiver ,unsigned sdpp,unsigned pid ,unsigned dtct) {
    if (isDebugSerial()){ cout << "[[send:no value]]" << endl;}
    setFixed(myid , receiver , sdpp, pid , dtct ,DTFM_NONE);
    createData(0);
    updateTime.ui32 = 0;
    write(0);
}//send

void MyNetSerialRPi::sendUT(uint32_t unixtime ,unsigned receiver ,unsigned sdpp,unsigned pid ,unsigned dtct ){
    if (isDebugSerial()){ cout << "[[sendUT:no value]]" << endl;}
    setFixed(myid , receiver , sdpp, pid , dtct ,DTFM_NONE);
    updateTime.ui32 = unixtime;
    createData(0);
    write(0);
}
void MyNetSerialRPi::sendInt16UT(uint32_t unixtime ,int16_t val ,unsigned receiver ,unsigned sdpp,unsigned pid,unsigned dtct){
    if (isDebugSerial()){ cout << "[[sendInt16UT]]" << endl;}
    setFixed(myid , receiver , sdpp, pid , dtct ,DTFM_INT16_T);
    updateTime.ui32 = unixtime;
    createData(sizeof(int16_t));
    valueBytes[0] = (val & 0xff);//low
    valueBytes[1] = (val & 0xff00) >> 8;//high
    write(sizeof(int16_t));
}
/*****************************************************************/

/*PUBLIC: RECEIVE FUNCTIONS                                    */

/*****************************************************************/
//▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️

//◼️◼️◼️◼️
//◼️      ◼️
//◼️◼️◼️◼️    eceive
//◼️     ◼️
//◼️      ◼️

//▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️

//return value : num of converted : -1 error (when the array was full
const int MyNetSerialRPi::receive() {
    if( !serial_data_available(pi,handle) ) return 0;//読み込むデータがない場合
    
    unsigned convertedNum = 0;
    char buf[1];
    if (isDebugSerial()){
        cout << "------receive------ " << endl;
    }
    
    while (serial_data_available(pi,handle)){
        serial_read(pi,handle,buf,1);
        if (isDebugSerial()){
            printBinary(buf[0]);
        }
        if (readStock[0] != HEAD_BYTE){//readStock[0]がヘッドバイトになるまで読み込む
            if (buf[0] == HEAD_BYTE)  stockPos = 0;
            else continue;//ヘッドバイトが来るまで読み捨て
        }
        
        if(buf[0] == FOOT_BYTE){//フットバイトが来たら検証変換
            //for Debug
            if (isDebugSerial()){
                cout << "to Convert" << endl;
            }
            readStock[stockPos] = FOOT_BYTE;//読み込んだ値　配列に格納
            if ( convertData() ) convertedNum++;//コンバート
            stockPos = 0;//また0から読み込む
            continue;
        }//if(buf == FOOT_BYTE)　終わり
        
        readStock[stockPos] = buf[0];//読み込んだ値　配列に格納
        stockPos++;//readStock[stockPos]までデータが入っている状態になる
    }//while (serial_data_available(pi,handle))
    
    
    return convertedNum;
    
}//End Of receive

const bool MyNetSerialRPi::convertData() {//readStockのデータ部分をMySerialData構造体に変換する
    
    SerialDataRPi* sd = emptyPick();
    //    if (isDebugSerial()){
    //        for (int i = 0; i < CConst::READSTOCK_NUM;i++){
    //            cout << MyToString::binToString( (readStock[i]) << endl;
    //            cout << ':');
    //            if(readStock[i] == FOOT_BYTE) break;
    //        }
    //    }
    if (sd == NULL) return false; //Do SomeThing!!!!!
    
    //[1] 0bDDDDDDDD    D:データ大別コード1byte
    unsigned divcode = (readStock[1]);
    switch (divcode){
        case DIIVISION_CODE_NONE:
            //Do Something!!!!
            return false;
        case DIIVISION_CODE_MYDATA_REPORT:
            //Create Method!!!!
            return false;
        case DIIVISION_CODE_MYNET_FIXED:
            if (isDebugSerial()) cout << " :DataDivision is <fixed data>" << endl;
            break;
        case DIIVISION_CODE_MYNET_FIXED_WITH_UNIXTIME:
            if (isDebugSerial()) cout << " :DataDivision is <fixed data With unixtime>" << endl;
            break;
    }
    //L:データ長 2byte MyNetFixedの場合可変長データバイト数 MyNetFixedの場合可変長データバイト数
    uint16_t valuelen = (  (int16_t)(readStock[2] << 8) ) & (readStock[3]);
    if (isDebugSerial()){
        cout << " :value length is " << valuelen << endl;
    }
    escapeProcess(readStock);//ヘッダ消える
    sd->fixedToData(readStock);
    
    if (isDebugSerial()){
        cout << " sender : " <<  mcidName( sd->sender() ) << endl;
        cout << " receiver : " <<  mcidName( sd->receiver() ) << endl;
        cout << " send purpose : " << sdppName( sd->sdpp() ) << endl;
        cout << " parts id : " << pidName( sd->pid() ) << endl;
        cout << " data contents : " << sd->dtct() << endl;
        cout << "  :: data form : " << sd->dtfm() << endl;
    }
    if (valuelen > 0 ){
        if (sd->dtfm() == DTFM_CHAR_ARRAY){
            sd->charArrayToData(readStock,valuelen);
        }else{
            sd->valueToData(readStock,valuelen);
        }
    }
    if (divcode == DIIVISION_CODE_MYNET_FIXED_WITH_UNIXTIME)
        sd->unixtimeToData(readStock);
    
    if (checksum != readStock[stockPos]){
        sd->setContained(IS_CONTAINED_ERROR);
        return true;
    }
    sd->setContained(IS_CONTAINED_YES);
    if (isDebugSerial()) cout << "checksum OK" << endl;
    return true;
    
}//convertData
/*****************************************************************/
/*PUBLIC ACCESSOR FUNCTIONS                                            */
/*****************************************************************/


/*****************************************************************/
/*PUBLIC POINTER FUNCTIONS                                            */
/*****************************************************************/



void MyNetSerialRPi::setFixed( unsigned sender , unsigned receiver ,unsigned sdpp,unsigned pid ,unsigned dtct
                              , unsigned dtfm){
    fixed[DATA_SENDER].data = sender;
    fixed[DATA_RECEIVER].data = receiver;
    fixed[DATA_SDPP].data = sdpp;
    fixed[DATA_PID].data = pid;
    fixed[DATA_DTCT].data = dtct;
    fixed[DATA_DTFM].data = dtfm;
    updateTime.ui32 = 0;
    if (isDebugSerial()){
        cout << " sender:" << mcidName(sender) << endl;
        cout << "  receiver:" << mcidName(receiver)  << endl;
        cout << "  send purpose:" << sdppName(sdpp)  << endl;
        cout << "  parts id:" << pidName(pid)  << endl;
    }
    
}

void MyNetSerialRPi::createData(uint16_t size){
    memset(fixedBytes, 0, sizeof(fixedBytes));//配列の要素に０をセット
    memset(valueBytes, 0, sizeof(valueBytes));//配列の要素に０をセット
    //first5Bytes[1] = DDDLLLLL    D:データ大別コード3bit L:データ長5bit MyNetFixedの場合 可変長データバイト数
    if (updateTime.ui32 == 0)
        first5Bytes[1] = DIIVISION_CODE_MYNET_FIXED;//D:データ大別コード
    else //updateTimeが設定されていたら
        first5Bytes[1] = DIIVISION_CODE_MYNET_FIXED_WITH_UNIXTIME;//D:データ大別コード
    //L:データ長  MyNetFixedの場合 可変長データバイト数
    
    first5Bytes[2] = (size & 0xff00) >> 8;//high
    first5Bytes[3] = (size & 0xff);//low
    
    //first5Bytes[2] 0bSRDPCF00 フラグ　S:Sender R:Receiver D:send purpose P:PID C:Contents F:Form
    std::bitset<32> bset;
    int setcount = 0;
    first5Bytes[4] = 0;
    for (int i =0; i < DATA_ELEMENTS_NUM; i++){//
        //cout << fixed[i].data);cout << ':');
        if(fixed[i].data != 0){//データが入っていたら
            first5Bytes[4] += fixed[i].flag;//first3[2]SRDPCF00 Flag  S:Sender R:Receiver D:SDPP P:PID C:DTCT F:DTFM
            for(int j = 0;j < fixed[i].len;j++){//fixedのデータをbitsetにセット
                if ( (fixed[i].data & ( 1 << (fixed[i].len - 1 - j) ) ) != 0)
                    bset[setcount] = 1;
                    else
                        bset[setcount] = 0;
                setcount++;
            }
        }
    }
    
    //        if (isDebugSerial()){ cout << "bset for fixed:");
    //
    //            for (int i = 0; i < setcount;i++){
    //                cout << bset[i]);
    //            }
    //            cout << );
    //        }
    cout << "bitset:";
    for (int k = 0;k < setcount;k++){
        cout << bset[k];
    }
    cout << endl;
    for (int i = 0; i < setcount; i++){//fixedデータをfixedBytesに格納
        if(bset[i])
        fixedBytes[i/8] += 1 << (7 - (i % 8));
    }
    if (isDebugSerial()){
    cout << "fixedBytes = ";
    for (char x : fixedBytes){
        printBinary(x);
        cout << " ";
    }
    cout << endl;
    }
    
}//End Of createData(const std::size_t size)


//▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️
//            ▪️       ▪️ R I T E
//            ▪️  ▪️  ▪️
//            ▪️▪️ ▪️▪️
//            ▪️    ▪️
//▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️

void MyNetSerialRPi::write(const size_t size){
    unsigned rid = fixed[DATA_RECEIVER].data;//受信者MCID （こちらから見ての送信先
    char buf[1];
    //ヘッダ、チェックサム、フッタをつけて送信
    unsigned checksum = HEAD_BYTE;
    if (isDebugSerial()) cout << "first5Bytes:" << endl;
    buf[0] = HEAD_BYTE;
    serial_write(pi,handle,buf,1);
    //if (isDebugSerial()) cout << MyToString::binToString(HEAD_BYTE) << endl;
    
    for (int i = 1; i < 5; ++i)//first5Bytes
    {
        writeEscChk(first5Bytes[i]);
    }
    
    if (isDebugSerial()) {
        cout << "fixedBytes:" << endl;
    }
    for (int i = 0; i < FIXED_BYTES_SIZE; ++i)//fixedBytes
    {
        writeEscChk(fixedBytes[i]);
    }
    if (isDebugSerial()) {
        cout << "valueBytes:" << flush;
        if(size == 0) cout << "None" << endl;
    }
    for (int i = 0; i < size; ++i){//valueBytes:ヘッダ、チェックサム、フッタ以外の部分
        if(fixed[DATA_DTFM].data == DTFM_CHAR_ARRAY)
            writeEscChk(charbuf[i]);
        else
            writeEscChk(valueBytes[i]);
    }
    if (updateTime.ui32 != 0){
        if (isDebugSerial()) cout << "updateTime" << endl;
        for (int i = 0; i < 4; ++i){//updateTime unixtime(second)
            writeEscChk(updateTime.array[i]);
        }
    }
    writeEscChk(checksum);
    buf[0] = FOOT_BYTE;
    serial_write(pi,handle,buf,1);
    //    if (isDebugSerial()){
    //        cout << "checksum foot:" << MyToString::binToString(checksum) << ':' << MyToString::binToString(FOOT_BYTE) << endl;
    //    }
    
}

void MyNetSerialRPi::writeEscChk(unsigned val){
    char buf[1];
    if ((val == CConst::ESCAPE_BYTE) || (val == HEAD_BYTE) || (val == FOOT_BYTE)){
        buf[0] = CConst::ESCAPE_BYTE;
        serial_write(pi,handle,buf,1);
        //        if (isDebugSerial()){
        //            cout << "Escape,head,or foot byte detected.  : write ESCAPE BYTE:" <<  MyToString::binToString(CConst::ESCAPE_BYTE) << endl;
        //        }
        checksum += CConst::ESCAPE_BYTE;
        buf[0] = (val ^ CConst::ESCAPE_MASK);
        serial_write(pi,handle,buf,1);
                if (isDebugSerial()) {
                    cout << " write Masked byte :" << (val ^ CConst::ESCAPE_MASK) << endl;
                }
        checksum += val ^ CConst::ESCAPE_MASK;
    }else{
        buf[0] = val;
        serial_write(pi,handle,buf,1);
                if (isDebugSerial()) {
                    cout << ':' << val << endl;
                }
        checksum += val;
    }
}


/*****************************************************************/
/*PRIVATE: SEND FUNCTIONS                                                */
/*****************************************************************/
void MyNetSerialRPi::setFloat(float val){
    Float_ToArray fta;
    
    fta.f = val;
    valueBytes[0] = fta.array[0];
    valueBytes[1] = fta.array[1];
    valueBytes[2] = fta.array[2];
    valueBytes[3] = fta.array[3];
}
void MyNetSerialRPi::setUint32(uint32_t val){
    Uint32_tToArray uita;
    uita.ui32 = val;
    valueBytes[0] = uita.array[0];
    valueBytes[1] = uita.array[1];
    valueBytes[2] = uita.array[2];
    valueBytes[3] = uita.array[3];
}







/*****************************************************************/
/* UTILITY                                            */
/*****************************************************************/

void MyNetSerialRPi::printBinary(char c) {
    for (int i = 7; i >= 0; --i) {
        cout << ((c & (1 << i))? '1' : '0');
    }
}

SerialDataRPi* MyNetSerialRPi::emptyPick(){
    for(int i = 0; i < CConst::MYDATA_NUM ; i++){
        if(mydata[i].isContained() == false){
            if (isDebugSerial()) cout << ": empty data exist" << endl;
            mydata[i].clear();
            return &mydata[i];
        }
    }
    if (isDebugSerial()) cout << ": data is full!! can't convert!!" << endl;
    return NULL;
}

void MyNetSerialRPi::escapeProcess(unsigned* readStock){//エスケープバイトの処理とチェックサムの計算 ヘッダ消す
    //readStock[] のフットバイト-1にチェックサムが入っている状態
    //[0]ヘッダ
    //[x] チェックサム
    //[x + 1] フッタ
    
    checksum = HEAD_BYTE;
    int convertnum = 0;
    for (int i = 1; i < CConst::READSTOCK_NUM; i++){
        checksum += readStock[i];
        if (readStock[i] == CConst::ESCAPE_BYTE){//エスケープバイト検出
            checksum += readStock[i+1];//次のバイトをchecksumに追加
            readStock[convertnum] = readStock[i+1] ^ CConst::ESCAPE_MASK;
            i++;
        }else if(readStock[i] == FOOT_BYTE){//フッタバイト検出
            readStock[convertnum] = FOOT_BYTE;
            return;
        }else{//エスケープバイトじゃないデータはそのまま入れる
            readStock[convertnum] = readStock[i];
        }
        convertnum++;
    }
    //error！！！！ フットバイトが来てない
}

/*****************************************************************/
/* HERITAGES                                            */
/*****************************************************************/

