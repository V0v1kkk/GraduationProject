# Разработка аппаратно-программного комплекса для оперативного мониторинга качества воздуха в жилых помещениях
Цель работы – разработать аппаратно-программный комплекс, с помощью которого можно оперативно получать данные о качестве воздуха, параметрах микроклимата, содержании монооксида углерода, двуокиси углерода, взвешенных частиц и природного газа в воздухе жилых помещений.

В процессе работы: 
* Изучена предметная область.
* Рассмотрены аналоги разработанного устройства, представленные на рынке. 
* Произведён выбор датчиков, наиболее подходящих для использования в устройстве, 
    а также рассмотрены вопросы, связанные с получением корректных данных от них. 
* Построена [диаграмма потоков данных](Images/DFD.png).
* Разработана [принципиальная схема устройства](Images/схема.png). 
* Разработаны прошивки для двух микроконтроллеров, 
    входящих в состав устройства (**[1](DataCollector/), [2](MainController/)**). 
* Разработан [графический интерфейс](Images/Интерфейс.png), представленный [ЖК-дисплеем](Nextion/) (**[1](Images/gui/1.png), [2](Images/gui/2.png), [3](Images/gui/3.png), [4](Images/gui/4.png), [5](Images/gui/5.png), [6](Images/gui/6.png), [7](Images/gui/7.png), [8](Images/gui/8.png), [9](Images/gui/9.png), [10](Images/10.png)**) и 
[web-интерфейс](WebInterface/) (**[1](Images/1.png), [2](Images/2.png), [3](Images/3.png), [4](Images/4.png), [5](Images/5.png)**). 
* Написано руководство пользователя. 
* Построен прототип устройства.

## [Пояснительная записка](Text/Диплом.pdf)

## Видео
[![Прототип в работе](https://img.youtube.com/vi/i2k4qLWkXlE/0.jpg)](https://www.youtube.com/watch?v=i2k4qLWkXlE)