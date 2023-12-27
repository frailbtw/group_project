#include <random>
#include <SFML/Graphics.hpp>

#include "Headers/Cell.hpp"
#include "Headers/Field.hpp"
#include "Headers/GetCell.hpp"
#include "Headers/Global.hpp"

// Это самый длинный файл в этом проекте

Field::Field() :
    game_over(1),
    random_engine(random_device())
{
    // Здесь мы добавляем ячейки в вектор cells
    for (unsigned char a = 0; a < ROWS; a++)
    {
        for (unsigned char b = 0; b < COLUMNS; b++)
        {
            // Я поменял местами координаты, чтобы исправить ошибку с координатами
            cells.push_back(Cell(b, a));
        }
    }

    // Как и в функции reset класса Cell, у нас есть функция restart, которая перезапускает игру, так зачем бы ее не использовать?
    restart();
}

bool Field::effect_over()
{
    // Проверяем таймер эффектов для каждой ячейки
    for (Cell& cell : cells)
    {
        // Если у любой ячейки таймер эффектов еще не закончился
        if (0 < cell.get_effect_timer())
        {
            // Возвращаем 0
            return 0;
        }
    }

    // В противном случае возвращаем 1 
    return 1;
}

char Field::get_game_over()
{
    // возвращаем game over
    return game_over;
}

unsigned short Field::get_flags()
{
    // Просто считаем общее количество помеченных флагом ячеек
    unsigned short total_flags = 0;

    for (Cell& cell : cells)
    {
        total_flags += cell.get_is_flagged();
    }

    // И возвращаем результат
    return total_flags;
}

// Фун-ия для рисования
void Field::draw(sf::RenderWindow& i_window)
{
    // Будем использовать  для рисования ячеек
    sf::RectangleShape cell_shape(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));

    sf::Sprite icon_sprite;

    sf::Texture icons_texture;
    icons_texture.loadFromFile("Source/Resources/Images/Icons" + std::to_string(CELL_SIZE) + ".png");

    icon_sprite.setTexture(icons_texture);

    // Мы перебираем каждую ячейку
    for (unsigned char a = 0; a < COLUMNS; a++)
    {
        for (unsigned char b = 0; b < ROWS; b++)
        {
            // Меняем позицию формы ячейки
            cell_shape.setPosition(static_cast<float>(CELL_SIZE * a), static_cast<float>(CELL_SIZE * b));

            // Если текущая ячейка открыта
            if (1 == get_cell(a, b, cells)->get_is_open())
            {
                // Получаем количество мин вокруг нее
                unsigned char mines_around = get_cell(a, b, cells)->get_mines_around();

                cell_shape.setFillColor(sf::Color(146, 182, 255));

                // Рисуем ячейку (Вау, какой сюрприз!)
                i_window.draw(cell_shape);

                // Если вокруг ячейки есть хотя бы одна мина
                if (0 < mines_around)
                {
                    // Берем число из текстуры и рисуем его
                    icon_sprite.setPosition(static_cast<float>(CELL_SIZE * a), static_cast<float>(CELL_SIZE * b));
                    icon_sprite.setTextureRect(sf::IntRect(CELL_SIZE * mines_around, 0, CELL_SIZE, CELL_SIZE));

                    i_window.draw(icon_sprite);
                }
            }
            else // Если ячейка закрыта
            {
                cell_shape.setFillColor(sf::Color(0, 73, 255));

                // Устанавливаем цвет ячейки в зависимости от состояния мыши
                    // Мы также не меняем цвет, если игра закончена
                    if (0 == game_over)
                    {
                        if (1 == get_cell(a, b, cells)->get_mouse_state())
                        {
                            cell_shape.setFillColor(sf::Color(36, 109, 255));
                        }
                        else if (2 == get_cell(a, b, cells)->get_mouse_state())
                        {
                            cell_shape.setFillColor(sf::Color(0, 36, 255));
                        }
                    }

                i_window.draw(cell_shape);

                // Если ячейка помечена флагом
                if (1 == get_cell(a, b, cells)->get_is_flagged())
                {
                    // Берем изображение флага и рисуем его
                    icon_sprite.setPosition(static_cast<float>(CELL_SIZE * a), static_cast<float>(CELL_SIZE * b));
                    icon_sprite.setTextureRect(sf::IntRect(0, 0, CELL_SIZE, CELL_SIZE));

                    i_window.draw(icon_sprite);
                }
            }

            // Сбрасываем состояние мыши для ячейки
            get_cell(a, b, cells)->set_mouse_state(0);

            // Здесь мы рисуем эффект
            // Мы не рисуем эффект, если игра не закончена или таймер эффекта ячейки еще не запущен.
            if (0 != game_over && EFFECT_DURATION > get_cell(a, b, cells)->get_effect_timer())
            {
                // Рассчитываем размер эффекта
                unsigned char effect_size = static_cast<unsigned char>(2 * round(0.5f * CELL_SIZE * ((EFFECT_DURATION - get_cell(a, b, cells)->get_effect_timer()) / static_cast<float>(EFFECT_DURATION))));

                // У каждой ячейки будет случайная длительность таймера эффекта
                std::uniform_int_distribution<unsigned short> effect_duration_distribution(1, EFFECT_DURATION - 1);

                // Мы снова будем использовать форму ячейки для рисования эффектов, потому что я люблю переработку!
                cell_shape.setPosition(floor(CELL_SIZE * (0.5f + a) - 0.5f * effect_size), floor(CELL_SIZE * (0.5f + b) - 0.5f * effect_size));
                cell_shape.setSize(sf::Vector2f(effect_size, effect_size));

                // Цвет эффекта будет зависеть от того, проиграна ли игра или выиграна
                if (-1 == game_over)
                {
                    cell_shape.setFillColor(sf::Color(255, 36, 0));
                }
                else
                {
                    cell_shape.setFillColor(sf::Color(255, 255, 255));
                }

                // Рисуем эффект
                i_window.draw(cell_shape);

                // Сбрасываем размер формы ячейки
                cell_shape.setSize(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));

                // Если таймер эффекта завершен
                if (1 == get_cell(a, b, cells)->update_effect_timer())
                {
                    // Запускаем таймер эффекта для каждой соседней ячейки
                    if (0 <= a - 1 && EFFECT_DURATION == get_cell(a - 1, b, cells)->get_effect_timer())
                    {
                        get_cell(a - 1, b, cells)->set_effect_timer(static_cast<unsigned char>(effect_duration_distribution(random_engine)));
                    }

                    if (0 <= b - 1 && EFFECT_DURATION == get_cell(a, b - 1, cells)->get_effect_timer())
                    {
                        get_cell(a, b - 1, cells)->set_effect_timer(static_cast<unsigned char>(effect_duration_distribution(random_engine)));
                    }

                    if (COLUMNS > 1 + a && EFFECT_DURATION == get_cell(1 + a, b, cells)->get_effect_timer())
                    {
                        get_cell(1 + a, b, cells)->set_effect_timer(static_cast<unsigned char>(effect_duration_distribution(random_engine)));
                    }

                    if (ROWS > 1 + b && EFFECT_DURATION == get_cell(a, 1 + b, cells)->get_effect_timer())
                    {
                        get_cell(a, 1 + b, cells)->set_effect_timer(static_cast<unsigned char>(effect_duration_distribution(random_engine)));
                    }
                }
            }
        }
    }
}

void Field::flag_cell(unsigned char i_x, unsigned char i_y)
{
    // Мы не позволяем игроку помечать ячейки, когда игра закончена
    if (0 == game_over)
    {
        get_cell(i_x, i_y, cells)->flag();
    }
}

void Field::open_cell(unsigned char i_x, unsigned char i_y)
{
    // Если это первая открытая ячейка
    if (0 == first_click)
    {
        // Объявляем распределения координат
        std::uniform_int_distribution<unsigned short> x_distribution(0, COLUMNS - 1);
        std::uniform_int_distribution<unsigned short> y_distribution(0, ROWS - 1);

        first_click = 1;

        // Затем мы генерируем мины
        for (unsigned short a = 0; a < MINES; a++)
        {
            unsigned char mine_x = static_cast<unsigned char>(x_distribution(random_engine));
            unsigned char mine_y = static_cast<unsigned char>(y_distribution(random_engine));

            // Если выбранная ячейка уже содержит мину или это ячейка, которую игрок хочет открыть
            if (1 == get_cell(mine_x, mine_y, cells)->get_is_mine() || (i_x == mine_x && i_y == mine_y))
            {
                // Пытаемся снова
                a--;
            }
            else
            {
                // В противном случае устанавливаем мину
                get_cell(mine_x, mine_y, cells)->set_mine();
            }
        }

        // После генерации мин каждая ячейка считает, сколько мин вокруг нее
        for (Cell& cell : cells)
        {
            cell.count_mines_around(cells);
        }
    }

    // Мы не открываем ячейку, когда игра закончена или ячейка помечена флагом



// Поскольку мы не можем вызывать функцию ячейки напрямую, мы должны использовать эту функцию
void Field::set_mouse_state(unsigned char i_mouse_state, unsigned char i_x, unsigned char i_y)
{
	get_cell(i_x, i_y, cells)->set_mouse_state(i_mouse_state);
}