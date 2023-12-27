#include <SFML/Graphics.hpp>
#include "Headers/DrawText.hpp"

void draw_text(bool i_black, unsigned short i_x, unsigned short i_y, const std::string& i_text, sf::RenderWindow& i_window)
{
    // �� ����������� ����� �� ������ �������� ����
    short character_x = i_x;
    short character_y = i_y;

    unsigned char character_width;

    sf::Sprite character_sprite;

    sf::Texture font_texture;
    font_texture.loadFromFile("Source/Resources/Images/Font.png");

    // �� ��������� ������ ������� �� ������ ������� ����������� ������
    // 96, ������ ��� � ����������� 96 ��������
    character_width = font_texture.getSize().x / 96;

    character_sprite.setTexture(font_texture);

    // ������� ���� ������
    if (1 == i_black)
    {
        character_sprite.setColor(sf::Color(0, 0, 0));
    }

    for (const char a : i_text)
    {
        if ('\n' == a)
        {
            // ����� ������� ������� ����� ������ ����������� y-���������� � ���������� x-����������
            character_x = i_x;
            character_y += font_texture.getSize().y;

            continue;
        }

        // �������� ��������� ���������� �������
        character_sprite.setPosition(character_x, character_y);
        // ����� ������ �� ����������� ������
        character_sprite.setTextureRect(sf::IntRect(character_width * (a - 32), 0, character_width, font_texture.getSize().y));

        character_x += character_width;

        // ������ ������
        i_window.draw(character_sprite);
    }
}