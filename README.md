
Создание файлового менеджера с помощью Си-библиотеки "ncurses". Файловый менеджер - это программа с удобным графическим интерфейсом, с помощью которого можно навигировать по файловой системе, создавать/удалять/перемещать разные папки/файлы и перемещаться из одной папки в другую.
Корректное компилирование:
aptitude install libncursesw5-dev
g++ -W -Wall -Wextra -pedantic two_panels.cpp -lncursesw -lpanel -o two_panels