#ifndef _DISPATCHER_H_
#define _DISPATCHER_H_


class MTools;
class MBoard;
class MDisplay;
class MState;

class MDispatcher
{
  public:
    enum MODES
    {
      OPTIONS = 0,            // режим ввода настроек (не отключаемый)
    #ifdef TEMPLATE_ENABLE
      TEMPLATE,               // шаблон режима 
    #endif      
    #ifdef DC_SUPPLY_ENABLE
      DCSUPPLY,               // режим источника постоянного тока
    #endif
    #ifdef PULSE_GEN_ENABLE
      PULSEGEN,               // режим источника импульсного тока
    #endif
    #ifdef CCCV_CHARGE_ENABLE
      CCCVCHARGE,             // режим заряда "постоянный ток / постоянное напряжение"
    #endif
    #ifdef PULSE_CHARGE_ENABLE           
      PULSECHARGE,            // режим импульсного заряда
    #endif
    #ifdef RECOVERY_ENABLE
      RECOVERY,               // режим восстановления
    #endif
    #ifdef STORAGE_ENABLE
      STORAGE,                // режим хранения
    #endif
    #ifdef DEVICE_ENABLE
      DEVICE,                 // режим заводских регулировок
    #endif
      SERVICE                 // режим Сервис АКБ
    };

  public:
    MDispatcher(MTools * tools);

    void run();
    void delegateWork();
    void textMode(int mode);

  private:
    MTools * Tools;
    MBoard * Board;
    MDisplay * Display;
    MState * State = 0;

    bool latrus;
    int mode;
};

#endif //_DISPATCHER_H_