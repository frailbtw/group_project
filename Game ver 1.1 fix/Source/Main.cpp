#include <chrono>
#include <random>
#include <SFML/Graphics.hpp>
#include "Headers/Cell.hpp"
#include "Headers/DrawText.hpp"
#include "Headers/Field.hpp"
#include "Headers/GetCell.hpp"
#include "Headers/Global.hpp"

int main()
{
    // Используется для создания игры, независимой от частоты кадров
    unsigned lag = 0;

    // Состояния Александра:
    // 0) Покой
    // 1) Когда вы собираетесь открыть клетку
    // 2) Когда игра окончена и вы проиграли
    // 3) Когда игра окончена и вы выиграли
    unsigned char Alexander_state = 0;

    // Похоже на lag, используется для создания игры, независимой от частоты кадров
    std::chrono::time_point<std::chrono::steady_clock> previous_time;

    // Объект событий SFML, я думаю
    sf::Event event;

    // Окно SFML
    sf::RenderWindow window(sf::VideoMode(CELL_SIZE * COLUMNS * SCREEN_RESIZE, SCREEN_RESIZE * (FONT_HEIGHT + CELL_SIZE * COLUMNS)), "Сапер", sf::Style::Close);
    // Здесь мы изменяем размер окна
    window.setView(sf::View(sf::FloatRect(0, 0, CELL_SIZE * COLUMNS, FONT_HEIGHT + CELL_SIZE * ROWS)));

    // Спрайт Александра
    sf::Sprite Alexander;

    // Текстура Александра
    sf::Texture Alexander_texture;
    // Мы получаем текстуру из изображения
    Alexander_texture.loadFromFile("Source/Resources/Images/Alexander.png");

    // Размещаем Александра в правом нижнем углу
    Alexander.setPosition(static_cast<float>(CELL_SIZE * COLUMNS - Alexander_texture.getSize().y), CELL_SIZE * ROWS);
    Alexander.setTexture(Alexander_texture);

    // Объект поля с именем "field" (Я так хорош в придумывании названий!)
    Field field;

    // Получаем текущее время и сохраняем его в переменной
    previous_time = std::chrono::steady_clock::now();

    while (1 == window.isOpen())
    {
        // Здесь мы вычисляем lag
        unsigned delta_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - previous_time).count();

        lag += delta_time;

        previous_time += std::chrono::microseconds(delta_time);

        // Пока lag превышает максимальную допустимую длительность кадра
        while (FRAME_DURATION <= lag)
        {
            // Получаем координаты ячейки под указателем мыши
            unsigned char mouse_cell_x = std::clamp(static_cast<int>(floor(sf::Mouse::getPosition(window).x / static_cast<float>(CELL_SIZE * SCREEN_RESIZE))), 0, COLUMNS - 1);
            unsigned char mouse_cell_y = std::clamp(static_cast<int>(floor(sf::Mouse::getPosition(window).y / static_cast<float>(CELL_SIZE * SCREEN_RESIZE))), 0, ROWS - 1);

            // Используем одну из базовых арифметических операций для выполнения этого сложного вычисления
            lag -= FRAME_DURATION;

            while (1 == window.pollEvent(event))
            {
                switch (event.type)
                {
                    // Если пользователь нажал кнопку закрытия окна
                case sf::Event::Closed:
                {
                    // Закрываем окно
                    window.close();

                    break;
                }
                case sf::Event::KeyReleased:
                {
                    switch (event.key.code)
                    {
                    case sf::Keyboard::Enter:
                    {
                        // Перезапускаем игру, если была отпущена клавиша Enter
                        field.restart();
                    }
                    }

                    break;
                }
                case sf::Event::MouseButtonReleased:
                {
                    switch (event.mouseButton.button)
                    {
                        // Если была отпущена ЛКМ
                    case sf::Mouse::Left:
                    {
                        // Открываем клетку под указателем мыши
                        field.open_cell(mouse_cell_x, mouse_cell_y);

                        break;
                    }
                    // Если была отпущена ПКМ
                    case sf::Mouse::Right:
                    {
                        // Помечаем флагом клетку под указателем мыши
                        field.flag_cell(mouse_cell_x, mouse_cell_y);
                    }
                    }
                }
                }
            }

            // Здесь мы определяем состояние лица Александра
            // Если какая-либо ячейка в данный момент нажата
            if (1 == sf::Mouse::isButtonPressed(sf::Mouse::Left) || 1 == sf::Mouse::isButtonPressed(sf::Mouse::Right))
            {
                // Александр открывает рот
                Alexander_state = 1;

                // Мы также изменяем состояние мыши ячейки
                field.set_mouse_state(2, mouse_cell_x, mouse_cell_y);
            }
            // В противном случае
            else
            {
                // Александр закрывает рот
                Alexander_state = 0;

                field.set_mouse_state(1, mouse_cell_x, mouse_cell_y);
            }

            // Если игра проиграна
            if (-1 == field.get_game_over())
            {
                // Александр расстроен (Как ты мог!)
                Alexander_state = 2;
            }
            // Если игра выиграна
            else if (1 == field.get_game_over())
            {
                // Александр гордится! (Да)
                Alexander_state = 3;
            }

            // Рисуем все!
            if (FRAME_DURATION > lag)
            {
                // Сначала очищаем окно
                window.clear();

                // Затем рисуем игровое поле
                field.draw(window);

                // Здесь мы показываем текст, информирующий игрока о том, что он проиграл или выиграл игру
                if (1 == field.effect_over())
                {
                    if (1 == field.get_game_over())
                    {
                        draw_text(1, static_cast<unsigned short>(round(0.5f * (CELL_SIZE * COLUMNS - 8 * FONT_WIDTH))), static_cast<unsigned short>(round(

                            0.5f * (CELL_SIZE * ROWS - FONT_HEIGHT))), "ПОБЕДА!", window);
                    }
                    else
                    {
                        draw_text(1, static_cast<unsigned short>(round(0.5f * (CELL_SIZE * COLUMNS - 4 * FONT_WIDTH))), static_cast<unsigned short>(round(0.5f * (CELL_SIZE * ROWS - 2 * FONT_HEIGHT))), "ИГРА\nОКОНЧЕНА", window);
                    }
                }

                // Сколько мин осталось?
                draw_text(0, 0, CELL_SIZE * ROWS, "Мины:" + std::to_string(MINES - field.get_flags()), window);

                // Выбираем лицо Александра из текстуры в зависимости от состояния Alexander_state
                Alexander.setTextureRect(sf::IntRect(Alexander_state * Alexander_texture.getSize().y, 0, Alexander_texture.getSize().y, Alexander_texture.getSize().y));

                // Мы рисуем Александра поверх всего, потому что он ЗАСЛУЖИВАЕТ быть наверху!
                window.draw(Alexander);
                window.display();
            }
        }
    }
}
```