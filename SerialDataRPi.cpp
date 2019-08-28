//Aruduino用 シリアル通信用　データ保管クラス

#include "SerialDataRPi.h"

bool SerialDataRPi::debugSerialPrint = false;
void SerialDataRPi::debugSerial(){debugSerialPrint = true;}

uint32_t SerialDataRPi::uint32() const{
    Uint32_tToArray uita;
    uita.array[0] = values[0];
    uita.array[1] = values[1];
    uita.array[2] = values[2];
    uita.array[3] = values[3];
    return uita.ui32;
}
float SerialDataRPi::float_() const{
    Float_ToArray fta;
    fta.array[0] = values[0];
    fta.array[1] = values[1];
    fta.array[2] = values[2];
    fta.array[3] = values[3];
    return fta.f;
}

void SerialDataRPi::longvalueToData(unsigned* bytes){};

void SerialDataRPi::fixedToData(unsigned* readStock){
    //bytesにヘッダバイトは含まれていない前提
    //[0] 0b    D:データ大別コード
//[1]L:データ長
    //[2]L:データ長
    //[3] 0b00000000 フラグ　7:Sender 6:Receiver 5:purpose 4:PID 3:Contents 2:Form
    //[4]~ fixedData
    std::bitset<32> bset;
    for (int i = 0; i < 4;i++){//bitsetに４バイト分読み込む
        for (int j = 0; j < 8;j++){
            if ( ( readStock[i + 4] & (1 << (7 - j)) ) != 0 )
                bset[(i*8) + j] = 1;
            else
                bset[(i*8) + j] = 0;
        }
    }
    
    uint16_t readpos = 0;//読み込んだビット数
    for (int i = 0; i < DATA_ELEMENTS_NUM;i++){
        if ( ( readStock[3] & (1 << (7 - i) ) ) != 0 ){//flagが立っていたら
            unsigned buf = 0;
            for (int j = 0; j < fixedDataLen[i].second; j++){
                buf += bset[readpos] << fixedDataLen[i].second - 1 - j;
                readpos++;
            }
            fixedDataLen[i].first = buf;
            //Serial.println(buf,BIN);
        }else{//flagが立ってない
            fixedDataLen[i].first = 0;
        }
    }
    readPos = 8;
    contained = true;
    return;
}
void SerialDataRPi::charArrayToData(unsigned* readStock,uint16_t valuelen){
    if( char_heap != NULL)
    delete[] char_heap;
    
    char_heap = new char[valuelen + 1];
    for(int i = 0;i < valuelen;i++){
        char_heap[i] = readStock[readPos];
        readPos++;
    }
    char_heap[valuelen] = 0;
//    Serial.println();
//    Serial.print("char_heap is:");
//    Serial.println(char_heap);
    return;
}

void SerialDataRPi::valueToData(unsigned* readStock,uint16_t valuelen){
    for (unsigned x : values){
        x = 0;
    }
    for(int i = 0;i < valuelen;i++){
        values[i] = readStock[readPos];
        readPos++;
    }
    return;
}

void SerialDataRPi::unixtimeToData(unsigned* readStock){
    Uint32_tToArray uita;
    for(int i = 0;i < 4;i++){
        uita.array[i] = readStock[readPos];
        readPos++;
    }
    updateTime = uita.ui32;
    return;
}

