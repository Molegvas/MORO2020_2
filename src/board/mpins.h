#ifndef _MPINS_H_
#define _MPINS_H_

#include "stdint.h"

namespace MPins
{
    // Проект MORO2020 с цветным tft дисплеем 1.8" 128*160
    // Аналоговые входы
    constexpr uint8_t pinI          = 33; //  an        OUT_IF      Порт измерителя тока заряда и разряда   33
    constexpr uint8_t pinV          = 34; //  an        UOUTIZM     Порт измерителя напряжения
    constexpr uint8_t pinT          = 32; //  an        RTU         Порт измерителя температуры
    constexpr uint8_t pinK          = 35; //  an        KEY         Порт выбора кнопок
    constexpr uint8_t pinI3         = 39; //  an(SVN)   INORM       Порт измерителя малых токов заряда      39

    // Выходы с ШИМ
    constexpr uint8_t pwm_i_pin     = 26; //  pwm       IPWM        Порт управления ШИМ тока
    constexpr uint8_t pwm_u_pin     = 27; //  pwm       UPWM        Порт управления ШИМ напряжения
    constexpr uint8_t pwm_dis_pin   = 25; //  pwm       PWMDIS      Порт управления ШИМ тока цепи разряда
    constexpr uint8_t pwm_fan_pin   = 17; //  pwm       FPWM        Порт управления вентилятором

    // Выходы управления светодиодами - в данной версии платы с цветным дисплеем RGB светодиод отсутствует 
    // constexpr uint8_t led_b_pin     = 02; //  out,pd    LED_B       Светодиод СИНИЙ порт управления
    // constexpr uint8_t led_g_pin     = 21; //  out       LED_G       Светодиод ЗЕЛЕНЫЙ порт управления
    // constexpr uint8_t led_r_pin     = 22; //  out       LED_R       Светодиод КРАСНЫЙ порт управления

    // Порты ввода-вывода
    constexpr uint8_t fast_off_pin  = 18; // in         FOFF        Порт цепи обнаружения превышения тока (быстрого отключения) (Ц) in
    constexpr uint8_t pow_pin       = 04; // out,pu     OFF         Порт управления силовым инвертором
    //reserved                      = 00; // out,pu                 USB интерфейс 
    constexpr uint8_t on_pin        = 23; // out        ONSW        Порт управления выходным (силовым ключем)
    constexpr uint8_t buz_pin       = 02; // out        BZ          Порт управления зуммером (ошибка в плате - подключить к выв.24)
    constexpr uint8_t neg_pin       = 22; // in         NEGIN       Порт обратной полярности (Ц)
    constexpr uint8_t pow_good_pin  = 36; // in(SVP)    PG_U        Порт наличия сетевого питания 
   
    // HSPI для дисплея 128*160. Порт IO12(MISO) не используется
    constexpr uint8_t scl_pin       = 14; //            SCLK  
    constexpr uint8_t sda_pin       = 13; //            MOSI
    constexpr uint8_t res_pin       = 19; //            RES         Аппаратный сброс OLED дисплея
    constexpr uint8_t dc_pin        = 16; //            IO
    constexpr uint8_t cs_pin        = 15; //            CD OLED

};

#endif // !_MPINS_H_