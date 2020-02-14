Проект MORO2020_2
Печатная плата, схема:      v43 
Дисплей:  TFT 1.8"  
Дата: Июль 2018 - Февраль 2020

   ###          Особенности реализации на данный момент.

*** Среда разработки - PlatformIO. Включена поддержка платформы Arduino для ESP32.

*** Поддержка Arduino автоматически предполагает, что программа будет работать в среде Free RTOS, хотя многое,как это характерно для Ардуино, скрыто. Так что философию операционной системы реального времени (RTOS) постичь придется, но вникать в тонкости... как говорится, "всё уже сделано за нас".

*** Разбивка на задачи производится в main.cpp. Задач всего на данный момент пять.
Производится выделение ресурсов для каждой задачи: память, приоритет, ядро.
Все наши задачи исполняются ядром 1, ядро 0 выделено для радиочастотных задач - BT и WiFi.
Учтите, что setup() выполнялся без обязательных требований по максимальному времени монопольного захвата ядра (13мс). Включение RTOS произойдет только после loop() {}. Цикл пустой, но может быть заполнен фоновой программой, которая автоматически будет оформлена в виде одной из задач по умолчанию.

*** Коротко конкретно по каждой задаче:

Задача подключения к WiFi сети (полностью заимствована как есть) Период 10мс. Длительность 2-3мс.

Задача выдачи данных на дисплей (5 параметров, дисплей TFT 1.8") 250мс, 25мс.

Задача управления системой теплоотвода. Предполагается расширить функциональность, добавив 
слежение за правильностью подключения нагрузки, масштабирование тока и т.д, своего рода "инспектор". Менее 1мс, 0,2с.

Задача обслуживания выбора режима работы, она же управляет конечным автоматом выбранного режима вплоть до выхода из режима. И то и другое построены как конечные автоматы (FSM). Особенностью этой задачи является то, что она запускается не в порядке очереди задач, а по таймеру исключительно для того, чтобы измерять точно ампер-часы. 100мс, 100мс.

Задача управления измерениями напряжения с выбором диапазона, тока, температуры, напряжения с кнопок - всё с фильтрацией, линеаризацией и преобразованием в физические величины. 0,03мс, 10мс. 

***     По большому счету для обмена данными между задачами должны бы использоваться конвйеры и семафоры... Однако довольно длительное тестирование подтвердило, что разработчики ESP учли нечто такое... атомарность, транзакции ... Впрочем, в проекте нет глобальных переменных. Как-то обошлось.

***     О реализации конечных автоматов (SM, FSM). 
Они в проекте повсюду. Вдохновение почерпнуто из статьи Павла Бобкова:
https://chipenable.ru/index.php/programming-avr/item/90-realizatsiya-konechnogo-avtomata-state-machine.html
Однако реализация построена на виртуальных методах. Выгода очевидна - произвольное 
размещение классов, описывающих состояния, простая структура класса любого состояния, 
прозрачная проверка условий и переходов, хорошая самодокументированность. Но постичь философию конечных автоматов, не вникая в тонкости, придется.

***     Более-менее законченный вид имеет диспетчер выбора режима (mdispatcherfsm.h,.cpp) и
режим простого заряда (mcccvfsm.h,.cpp). Остальные пока не приведены к окончательному
виду, а потому как учебное пособие использоваться не могут. 

***     Между режимами нет никакой связи - что позволяет разрабатывать каждый в отдельности, разными разработчиками, сосредотачиваясь исключительно на одном из них. Некоторая избыточность при этом - 
небольшая плата. Реализация режима занимает в програмной памяти несколько десятых долей процента, проверено. Предполагается, что конечный пользователь, имея некоторый навык (...) сможет, используя шаблон режима, создать режим имени себя любимого без особого риска преобразования прибора в "кирпич". 

***     Общим ресурсом всех режимов является класс MTools, в котором определен довольно широкий
инструментарий для построения пользовательских режимов. Пока там много ненужного от первоначального варианта, не предполагавшего использование виртуальных методов. Чистка и оптимизация будет. Предполагается, что Tools для редактирования будет доступен только разработчику прибора, что естественно. 

***     Работа с энергонезависимой памятью (NVS) не вызывает затруднений. Использование имен и ключей сильно упрощает работу с данными, включая загрузку параметров при первом включении. Некоторый напряг может вызывать заложенная разработчиками ESP реакция на "испорченные" данные - рестарт. Мы как-то делали это иначе.

***     Обновление ПО...  По воздуху (оно так и называется - OTA) - заложено ESP-шниками. Память программ разделена на две равные части. В одной - рабочая программа, вторая - зарезервирована - полтора мегабайта!!! 
Что происходит при обновлении: новый код пишется в другую половину, там проверяется, потом рабочая программа безаварийно завершается, а управление передается новой. Красиво?
А что делать с таким подарком 1,5 мегабайта в "мирное время"? Думаю, что туда можно писать логи. Карточки то в проекте не предусмотрено - ну его этот раритет... как представлю, что в гараже, промасленными пальцами...

***     Вывода на дисплей пока нет - закомментированные //Oled->xxx(); методы предназначались
для работы с OLED 1.54" 128x64. Проект MORO2020_2 предполагает использование TFT 1.8" 128x160 RGB дисплея. Структура вывода на экран пока окончательно не определена.   

... не закончено ... 2020 02 05 9:54

20:18 2020.02.07

***     Вывод на дисплей TFT 1.8".
Получилось 9 строк. Шрифты разного размера. Строка-лента. Самый мелкий шрифт - 21 знакоместо.
Сеансы обновления занимают от 2 до 8,5мс

***
10:14 2020.02.14
Оптимизирован код режима Option, позволяющий пользователю регулировать в заданных пределах некоторые параметры: задержку включения, смещение показаний тока и напряжения, возврат к заводским регулировкам для каждого режима в отдельности. (не закончено)

***
Вывод на дисплей - для снижения заметности при обновлении реализован метод очистки строки путем повторной её записи цветом фона и записи новой только в том случае, если строка изменилась.

***
Оптимизирован код режимов CCCV и DC SUPPLY под новые возможности отображения и с использованием универсальных методов класса MTools вместо специализированных, коих стало слишком много))

***
Решено отказаться от параноидального размещения переменных в privat, что может быть спорно.

