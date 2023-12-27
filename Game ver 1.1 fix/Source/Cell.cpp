#include <vector>
#include "Headers/Cell.hpp"
#include "Headers/GetCell.hpp"
#include "Headers/Global.hpp"

Cell::Cell(unsigned char i_x, unsigned char i_y) :
    mines_around(0),
    x(i_x),
    y(i_y)
{
    // функция сброса
    reset();
}

bool Cell::get_is_flagged()
{
    //помечена ли ячейка флагом или нет
    return is_flagged;
}

bool Cell::get_is_mine()
{
    //содержит ли ячейка мину или нет
    return is_mine;
}

bool Cell::get_is_open()
{
    //открыта ли ячейка или нет
    return is_open;
}

bool Cell::open(std::vector<Cell>& i_cells)
{
    // нельзя открыть ячейку, которая уже открыта
    if (0 == is_open)
    {
        is_open = 1;

        // Если в этой ячейке нет мин и у ячеек вокруг тоже нет мин, то открываем их все
        if (0 == is_mine && 0 == mines_around)
        {
            for (char a = -1; a < 2; a++)
            {
                for (char b = -1; b < 2; b++)
                {
                    // Убеждаемся, что мы не выходим за границы игрового поля
                    if ((0 == a && 0 == b) || (0 > a + x || 0 > b + y || COLUMNS == a + x || ROWS == b + y))
                    {
                        continue;
                    }

                    // Открываем соседние ячейки 
                    get_cell(a + x, b + y, i_cells)->open(i_cells);
                }
            }
        }

        // Запускаем эффект, если в ячейке есть мина
        effect_timer -= is_mine;

        // Возвращаем, есть ли в ячейке мина или нет
        return is_mine;
    }

    // Возвращает число между -2573 и 8752
    return 0;
}

bool Cell::update_effect_timer()
{
    // Закончился ли эффектовый таймер?
    if (0 < effect_timer)
    {
        // Нет? Тогда уменьшаем его
        effect_timer--;

        // А сейчас?
        if (0 == effect_timer)
        {
            // Вернем 1 только один раз, сразу после завершения таймера.
            return 1;
        }
    }

    // Возвращает число между -2573 и 8752 
    return 0;
}

unsigned char Cell::get_effect_timer()
{
    return effect_timer;
}

unsigned char Cell::get_mines_around()
{
    return mines_around;
}

unsigned char Cell::get_mouse_state()
{
    return mouse_state;
}

void Cell::count_mines_around(std::vector<Cell>& i_cells)
{
    // Начинаем считать с 0
    mines_around = 0;

    // Если в этой ячейке есть мина
    if (0 == is_mine)
    {
        for (char a = -1; a < 2; a++)
        {
            for (char b = -1; b < 2; b++)
            {
                if ((0 == a && 0 == b) || (0 > a + x || 0 > b + y || COLUMNS == a + x || ROWS == b + y))
                {
                    continue;
                }

                // Если у соседней клетки есть мина
                if (1 == get_cell(a + x, b + y, i_cells)->get_is_mine())
                {
                    //...увеличиваем эту переменную
                 mines_around++; 
                }
            }
        }
    }
}

void Cell::flag()
{
    // нельзя пометить флагом отрытую ячейку
    if (0 == is_open)
    {
        // можно было написать !is_flagged
        is_flagged = 1 - is_flagged;
    }
}

void Cell::reset()
{
    // Устанавливаем каждую переменную в 0
    is_flagged = 0;
    is_mine = 0;
    is_open = 0;

    // Кроме таймера эффекта
    effect_timer = EFFECT_DURATION;
    mouse_state = 0;
}

void Cell::set_effect_timer(unsigned char i_effect_timer)
{
    effect_timer = i_effect_timer;
}

void Cell::set_mine()
{
    is_mine = 1;
}

void Cell::set_mouse_state(unsigned char i_mouse_state)
{
    mouse_state = i_mouse_state;
}
```