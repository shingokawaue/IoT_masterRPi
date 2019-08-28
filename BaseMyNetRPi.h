//Aruduino IDE用 通信クラス
#ifndef _BASEMYNETRPIH_
#define _BASEMYNETRPIH_



/*****************************************************************/
/* CONSTANTS AND ENUM                                            */
/*****************************************************************/

enum fixeddata{
    DATA_SENDER,
    DATA_RECEIVER,
    DATA_SDPP,
    DATA_PID,
    DATA_DTCT,
    DATA_DTFM,
    DATA_ELEMENTS_NUM
};


static const char* mcidName(unsigned index){
    static const char* a[] = {
        "NOT_SPECIFIED",
        "MASTER_MEGA",
        "GATE_1",
        "ESP32_C",
        "ESP32_DOORSIDE",
        "MEGA_SOC",
        "NANO_DOORSIDE",
        "DISPLAY_UNO",
        "ESP32_B",
        "GATE_SENSORS",
        "SCREEN_UNO",
        "MASTER_RPI"
    };
    return a[index];
}
enum mcid{
    MCID_NOT_SPECIFIED,
    MCID_MASTER_MEGA,
    MCID_GATE_1,
    MCID_ESP32_C,
    MCID_ESP32_DOORSIDE,
    MCID_MEGA_SOC,
    MCID_NANO_DOORSIDE,
    MCID_DISPLAY_UNO,
    MCID_ESP32_B,
    MCID_GATE_SENSORS,
    MCID_SCREEN_UNO,
    MCID_MASTER_RPI
};

static const char* sdppName(unsigned index){
    static const char* a[] = {
        "NOT_SPECIFIED",
        "SENSOR_VALUE_REPORT",
        "ABNORMALITY＿DETECTION",
        "PERIODIC_CONFIRMATION",
        "ON_COMMAND",
        "OFF_COMMAND",
        "CONFIRM_ON",
        "CONFIRM_OFF",
        "ON_REPORT",
        "OFF_REPORT",
        "ONOFF_REQUEST",
        "UNKNOWN_REPORT",
        "POST_JSON",
        "",
        "TIME_REQUEST",
        "TIME_REPORT",
        "TIME_COMMAND",
        "PERIODIC_CONFIRMATION_BACK",
        "IN_REPORT",
        "OUT_REPORT"
    };
    return a[index];
}

enum sdpp{
SDPP_NOT_SPECIFIED,         // 0x00// 000000
SDPP_SENSOR_VALUE_REPORT,    // 0x01//値報告 000001
SDPP_ABNORMALITY_DETECTION, // 0x02//異常検出 000010
SDPP_PERIODIC_CONFIRMATION, // 0x03//定期確認連絡 000011
SDPP_ON_COMMAND,            // 0x04 //オン命令 000100
SDPP_OFF_COMMAND,           // 0x05 //オフ命令 000101
SDPP_CONFIRM_ON,            // 0x06 //オン確認 000110
SDPP_CONFIRM_OFF,           // 0x07 //オフ確認 000111
SDPP_ON_REPORT,             // 0x08 //オン報告 001000
SDPP_OFF_REPORT,            // 0x09 //オフ報告 001001
SDPP_ONOFF_REQUEST,         // 0x0a //10
SDPP_UNKNOWN_REPORT,        // 0x0b //11
SDPP_POST_JSON,                // 0x0c //12 json送信 001100
SDPP_xxxxxxxxx,             // 0x0d //13 スイッチオフ報告 001101
SDPP_TIME_REQUEST,          // 0x0e //14 時間リクエスト
SDPP_TIME_REPORT,           // 0x0f //15時間報告
SDPP_TIME_COMMAND,          // 0x10 //16time命令
SDPP_PERIODIC_CONFIRMATION_BACK,// 0x11//17定期確認連絡返し
    SDPP_IN_REPORT,
    SDPP_OUT_REPORT
};

static const char* pidName(unsigned index){
    static const char* a[] = {
        "NOT_SPECIFIED",
        "GATE_DTC",
        "GATE_xxxxx",
        "GATE_xxxxxx",
        "GATE_xxxxxxxx",
        "MEGA_SOC_PIR",
        "GATE_UNO_CPU",
        "GATE_UNO_VCC",
        "GATE_LED",
        "GATE_PHOTO",
        "GATE_1_INSIDE_TEMP",
        "LIGHTSOFFTIME",
        "TIME",
        "GATE_LED_SW",
        "GATE_LED_ALARM_OFF",
        "DB_WEATHER",
        "DOOR_INSIDE_PIR",
        "GATE_1_OUTSIDE_TEMP"
    };
    return a[index];
}
enum pid{//7bit 0~127
    PID_NONE,               // 0x00 0000000
    PID_GATE_DTC,          // 0x01 0000001
    PID_xxxxx,          // 0x02 0000010
    PID_xxxxxx,          // 0x03 0000011
    PID_xxxxxxxx,          // 0x04 0000100
    PID_MEGA_SOC_PIR,          // 0x05 0000101
    PID_GATE_1_CPU,         // 0x06 0000110
    PID_GATE_1_VCC,         // 0x07 0000111
    PID_GATE_LED,         // 0x08 0001000
    PID_GATE_PHOTO,         // 0x09 0001001
    PID_GATE_1_INSIDE_TEMP,//0x0a //10  0001010
    PID_LIGHTSOFFTIME,      //0x0b //11  0001011
    PID_TIME,               //0x0c //12 0001100
    PID_GATE_LED_SW,         //0x0d //13 0001101
    PID_GATE_LED_ALARM_OFF,  //0x0e //14 0001110
    PID_DB_WEATHER,          //0x0f //15 0001111
    PID_DOOR_INSIDE_PIR,     //0x10  //16 0010000
    PID_GATE_1_OUTSIDE_TEMP//0x11 //17  0010001
};

enum dtct{
    //DTCT 5bit 0~31 //データ内容
DTCT_NONE,          // 0x00
DTCT_DISTANCE_CM,   // 0x01
DTCT_ONOFF,         // 0x02
DTCT_OHM,           // 0x03
DTCT_HOUR,          // 0x04
DTCT_MINUTE,        // 0x05
DTCT_HOURMINUTE,    // 0x06
DTCT_UNIXTIME,      // 0x07
DTCT_UPDATE_UNIXTIME// 0x08
};

enum dtfm{
    //DTFM 4bit 0~15 (DataForm)データ形式
DTFM_NONE,      // 0x00
DTFM_FLOAT,     // 0x01
DTFM_BOOL,      // 0x02
DTFM_INT,       // 0x03
DTFM_INT16_T,   // 0x04
DTFM_LONG,      // 0x05
DTFM_UINT32_T,  // 0x06
DTFM_INT8_T,    // 0x07
DTFM_CHAR_ARRAY // 0x08
};


class BaseMyNetRPi{
public:

    /*****************************************************************/
    /*PUBLIC: INIT FUNCTIONS                                         */
    /*****************************************************************/
    /////////////////////////////////////////////////////////////////
    /// Constructor used to create the class.
    BaseMyNetRPi(unsigned myid):myid(myid){};
    ~BaseMyNetRPi(){};
    /*****************************************************************/
    /* ACCESSOR FUNCTIONS                                            */
    /*****************************************************************/
    bool isDebugSerial() const{
        return bDebugSerial;
    }
    /*****************************************************************/
    /*PUBLIC: CONFIGURATION FUNCTIONS                                                */
    /*****************************************************************/
   void debugSerial(){
        bDebugSerial = true;
    }
protected://▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️
    unsigned myid;
    bool bDebugSerial = false;
};//End Of class


#endif /* _BASEMYNETRPIH_ */
