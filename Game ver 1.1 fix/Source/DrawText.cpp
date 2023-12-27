#include <SFML/Graphics.hpp>
#include "Headers/DrawText.hpp"

void draw_text(bool i_black, unsigned short i_x, unsigned short i_y, const std::string& i_text, sf::RenderWindow& i_window)
{
    // Мы выравниваем текст по левому верхнему краю
    short character_x = i_x;
    short character_y = i_y;

    unsigned char character_width;

    sf::Sprite character_sprite;

    sf::Texture font_texture;
    font_texture.loadFromFile("Source/Resources/Images/Font.png");

    // Мы вычисляем ширину символа на основе размера изображения шрифта
    // 96, потому что в изображении 96 символов
    character_width = font_texture.getSize().x / 96;

    character_sprite.setTexture(font_texture);

    // меняеем цвет текста
    if (1 == i_black)
    {
        character_sprite.setColor(sf::Color(0, 0, 0));
    }

    for (const char a : i_text)
    {
        if ('\n' == a)
        {
            // После каждого символа новой строки увеличиваем y-координату и сбрасываем x-координату
            character_x = i_x;
            character_y += font_texture.getSize().y;

            continue;
        }

        // Изменяем положение следующего символа
        character_sprite.setPosition(character_x, character_y);
        // Берем символ из изображения шрифта
        character_sprite.setTextureRect(sf::IntRect(character_width * (a - 32), 0, character_width, font_texture.getSize().y));

        character_x += character_width;

        // Рисуем символ
        i_window.draw(character_sprite);
    }
}