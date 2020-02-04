/*
project:  MORO2020_2 
pcb:      v43 
display:  TFT 1.8"  
date:     Февраль 2020
*/

#include "board/mboard.h"
#include "board/moverseer.h"
#include "display/mdisplay.h"
#include "mtools.h"
#include "mdispatcher.h"
#include "mconnmng.h"
#include "measure/mmeasure.h"
#include "connect/connectfsm.h"
#include <Arduino.h>

static MBoard      * Board      = 0;
static MDisplay    * Display    = 0;
static MTools      * Tools      = 0;
static MMeasure    * Measure    = 0;
static MDispatcher * Dispatcher = 0;
static MConnect    * Connect    = 0;

void connectTask ( void * parameter );
void displayTask ( void * parameter );
void coolTask    ( void * parameter );
void mainTask    ( void * parameter );
void measureTask ( void * parameter );

void setup()
{
  Display   = new MDisplay();
  Board = new MBoard(Display);
  Tools = new MTools(Board, Display);
  Measure = new MMeasure(Tools);
  Dispatcher = new MDispatcher(Tools);
  Connect = new MConnect(Tools);

  // Выделение ресурсов для каждой задачи: память, приоритет, ядро.
  // Все задачи исполняются ядром 1, ядро 0 выделено для радиочастотных задач - BT и WiFi.
  xTaskCreatePinnedToCore ( connectTask, "Connect", 10000, NULL, 1, NULL, 1 );
  xTaskCreatePinnedToCore ( mainTask,    "Main",    10000, NULL, 2, NULL, 1 );
  xTaskCreatePinnedToCore ( displayTask, "Display",  5000, NULL, 2, NULL, 1 );
  xTaskCreatePinnedToCore ( coolTask,    "Cool",     1000, NULL, 2, NULL, 1 );
  xTaskCreatePinnedToCore ( measureTask, "Measure",  5000, NULL, 2, NULL, 1 );
}

// Далее запуск Free RTOS, setup() выполнялся без выполнения обязательных требований по максимальному
// времени монопольного захвата ядра (13мс).
void loop() {}

// Задача подключения к WiFi сети (полностью заимствована как есть)
void connectTask( void * parameter )
{
  while(true) {
  //unsigned long start = millis();   // Старт таймера 
    Connect->run(); 
    // Период вызова задачи задается в TICK'ах, TICK по умолчанию равен 1мс.
    vTaskDelay( 10 / portTICK_PERIOD_MS );
  // Для удовлетворения любопытства о длительности выполнения задачи - раскомментировать как и таймер.
  //Serial.print("Autoconnect: Core "); Serial.print(xPortGetCoreID()); Serial.print(" Time = "); Serial.print(millis() - start); Serial.println(" mS");
  // Core 1, 2...3 mS
  }
  vTaskDelete( NULL );
}

// Задача выдачи данных на дисплей (Таймер замера времени исполнения включен.)
void displayTask( void * parameter )
{
  while(true)
  {
    unsigned long start = millis();
    Display->runDisplay( Board->getVoltage(), 
                         Board->getCurrent(), 
                         Board->Overseer->getCelsius(),
                         Tools->getChargeTimeCounter(),
                         Tools->getAhCharge(), 
                         Tools->getAP() );
    Serial.print("Display: Core "); Serial.print(xPortGetCoreID()); Serial.print(" Time = "); Serial.print(millis() - start); Serial.println(" mS");
    // Core 1, Time = 24 mS (TFT 1.8", HSPI hard)
    vTaskDelay( 250 / portTICK_PERIOD_MS );
  }
  vTaskDelete( NULL );
}

// Задача управления системой теплоотвода. Предполагается расширить функциональность, добавив 
// слежение за правильностью подключения нагрузки, масштабирование тока и т.д. 
void coolTask( void * parameter )
{
  while (true)
  {
    //unsigned long start = millis();
    Board->Overseer->runCool();
    //Serial.print("Cool: Core "); Serial.print(xPortGetCoreID()); Serial.print(" Time = "); Serial.print(millis() - start); Serial.println(" mS");
    // Core 1, 0 mS
    vTaskDelay( 200 / portTICK_PERIOD_MS );
  }
  vTaskDelete( NULL );
}

// 1. Задача обслуживает выбор режима работы.
// 2. Управляет конечным автоматом выбранного режима вплоть да выхода из режима.
// И то и другое построены как конечные автоматы (FSM).
void mainTask ( void * parameter )
{ 
  while (true)
  {
    // Выдерживается период запуска для вычисления амперчасов. Если прочие задачи исполняются в порядке 
    // очереди, то эта точно по таймеру - через 0,1с.
    portTickType xLastWakeTime = xTaskGetTickCount();   // To count the amp hours
    //unsigned long start = millis();
    Dispatcher->run(); 
    //Serial.print("Main: Core "); Serial.print(xPortGetCoreID()); Serial.print(" Time = "); Serial.print(millis() - start); Serial.println(" mS");
    // Core 1, 100...102 mS
    vTaskDelayUntil( &xLastWakeTime, 100 / portTICK_PERIOD_MS );    // период 0,1с
  }
  vTaskDelete( NULL );
}

// Задача управления измерениями напряжения с выбором диапазона, тока, температуры, 
// напряжения с кнопок - всё с фильтрацией, линеаризацией и преобразованием в 
// физические величины. Задача шустрая, потому таймер микросекундный. 
void measureTask( void * parameter )
{
  while (true)
  {
    //unsigned long start = micros();
    Measure->run();
    //Serial.print(" Time, uS: "); Serial.println(micros() - start);
    // Core 1, 32 260 mkS
    vTaskDelay( 10 / portTICK_PERIOD_MS );
  }
  vTaskDelete(NULL);
}

/*
  Примечания
  ***     По большому счету для обмена данными между задачами должны бы использоваться 
        конвееры и семафоры... Однако довольно длительное тестирование показало, что
        разработчики учли нечто такое... атомарность, транзакции ...
        Впрочем, в проекте нет глобальных переменных.  Как-то так.

  ***     О реализации конечных автоматов (SM, FSM). 
        Они в проекте повсюду. Вдохновение почерпнуто из статьи Павла Бобкова:
https://chipenable.ru/index.php/programming-avr/item/90-realizatsiya-konechnogo-avtomata-state-machine.html
        Однако реализация построена на виртуальных методах. Выгода очевидна - произвольное 
        размещение классов, описывающих состояния, простая структура класса любого состояния, 
        прозрачная проверка условий и переходов, хорошая самодокументированность.

  ***     Более-менее законченный вид имеет диспетчер выбора режима (mdispatcherfsm.h,.cpp) и
        режим простого заряда (mcccvfsm.h,.cpp). Остальные пока не приведены к окончательному
        виду, а потому как учебное пособие использоваться не могут. 

  ***     Между режимами нет никакой связи - что позволяет разрабатывать каждый в отдельности, 
        сосредотачиваясь исключительно на одном из них. Некоторая избыточность при этом - 
        небольшая плата. Реализация режима занимает в програмной памяти несколько десятых долей
        процента.

  ***     Общим ресурсом всех режимов является класс MTools, в котором определен довольно широкий
        инструментарий для построения пользовательских режимов. Пока там много ненужного, чистка 
        и оптимизация предполагаются. 

  ***     Вывода на дисплей пока нет - закомментированные //Oled->xxx(); методы предназначались
        для работы с OLED 1.56" 128*64. Проект MORO2020_2 предполагает использование TFT 1.8"
        128*160 RGB дисплея. Структура вывода на экран пока окончательно не определена.   
*/
