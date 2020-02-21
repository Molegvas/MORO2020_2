/* Распределение энергонезависимой памяти
* Проект MORO2020_2
* 15.02.2020
*
* Необходимо выдерживать длину имен и ключей не более 15 знаков
*/

#ifndef _NVS_H_
#define _NVS_H_

/*
* nXXX    - имя раздела
* kXXXYYY - ключ параметра
*/

namespace MNvs
{
    static constexpr const char* nQulon          = "qulon";    // Общие 
        static constexpr const char* kQulonLocal     = "local";    //  
        static constexpr const char* kQulonMode      = "mode";     //  
        static constexpr const char* kQulonPostpone  = "postp";    //  
        static constexpr const char* kQulonVoffset   = "vOffset";  //  
        static constexpr const char* kQulonVmult     = "vMult";    //  
        static constexpr const char* kQulonIoffset   = "iOffset";  //  
        static constexpr const char* kQulonImult     = "iMult";    //  новое
        static constexpr const char* kQulonAkbU      = "akbU";     //  
        static constexpr const char* kQulonAkbAh     = "akbAh";    //  
        static constexpr const char* kQulonAkbInd    = "akbInd";   //  
        static constexpr const char* kQulonPowInd    = "powInd";   //  

    static constexpr const char* nTemplate       = "template"; // Шаблон 

    static constexpr const char* nDcPower        = "s-power";  // Простой источник 
    static constexpr const char* kDcVmax         = "voltMax";  //
    static constexpr const char* kDcImax         = "currMax";  //

    static constexpr const char* nCcCv           = "cccv";     // Простой заряд
        static constexpr const char* kCcCvVmax       = "voltMax";
        static constexpr const char* kCcCvVmin       = "voltMin";
        static constexpr const char* kCcCvImax       = "currMax";
        static constexpr const char* kCcCvImin       = "currMin";


    static constexpr const char* nExChrg         = "e-charge"; // Расширенный заряд


    static constexpr const char* nRecBat         = "recovery"; // Восстановление

    static constexpr const char* nStor           = "storage";  // Хранение
        static constexpr const char* kStorVmax       = "voltMax";
        static constexpr const char* kStorVmin       = "voltMin";
        static constexpr const char* kStorImax       = "currMax";
        static constexpr const char* kStorImin       = "currMin";


    static constexpr const char* nServBat        = "service";  // Сервис батареи


};

#endif  // !_NVS_H_