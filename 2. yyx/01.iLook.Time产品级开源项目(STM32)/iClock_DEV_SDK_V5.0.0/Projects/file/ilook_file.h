void sys_log_write(char *event, char *msg);
void gpx_record(char *gpx_msg);
void gpx_open(void);
void gpx_close(void);
uint8_t *load_logo(int *width, int *height);
void ilook_cfg_load(void);


typedef struct{
  //ϵͳ����
  char GPX_onoff;             //GPX��¼��/�ر�
  char TimeZone;              //ʱ��
  char GPS_PosConvert;        //����ת��
  char AltitudeType;          //��������,=0 ��ѹ���Σ�=1 GPS���Σ�=2 �ۺϺ���
  char GpxSaveCnt;            //GPX��������ٴ洢
  char WaveType;              //=0����, =1�¶ȣ�=2��ѹ
  tm   tCountDown;            //����ʱ��
  //������ع���
  char GotoWin_onoff;
  char WeatherWin_onoff;
  char PositionWin_onoff;
  char ShakeCountGame_onoff;
  char DebugWin_onoff;
  char WhenWhereWin_onoff;
  char TimerWin_onoff;
  char NpsWin_onoff;
  char AltitudeTempWin_onoff;
  char TravelWin_onoff;
  //ʱ���趨
  int  T_LogoDisplay;         //LOGO��ʾʱ��
  int  T_ScreenAutoCloseTime; //sec,��Ļ�Զ��ر�ʱ��
  int  T_GPSSearchTimeMax;    //sec,gps���������ʱ��
  int  T_GPSSleepSec_Car;     //�ڿ���״̬��GPS��Ъ����ʱ��
  int  T_GPSSleepSec_Walk;    //�ڲ���״̬��GPS��Ъ����ʱ��
  int  T_TravelRestMax;       //��;���Ϣʱ��
  int  T_WeatherInterval;     //�����ɼ����
  int  T_ScreenCloseLongTime; //һЩ�������ĳ���ʱ����
  int  T_WeatherWave;         //���βɼ��ܶ�
  //GSENSOR�趨
  unsigned char g_slope_th;
  unsigned char g_slope_dur;
  unsigned char g_ig_incr_step;
  unsigned char g_ig_dec_step;
  int g_ig_wkup_level;
  int g_ig_move_level;
  int g_ig_max_cnt;
  int g_mmt_flt_scale;
  int g_mmt_offset;
  //OLED����
  unsigned char oled_contrast;//OLED����
  unsigned char oled_fosc;    //OLEDƵ��
  unsigned char flip_onoff;   //OLED��ת
  //OTHER
  int gpx_min_distance;
  
}SYS_CFG_TYPE;






