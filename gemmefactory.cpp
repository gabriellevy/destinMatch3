#include "gemmefactory.h"
#include <QPixmap>
#include "vueplateau.h"

GemmeFactory::GemmeFactory()
{
    // create sprites for all gem colors
    QPixmap* redSprite = new QPixmap();
    redSprite->load(":/assets/sprites/Red.png");
    QPixmap* greenSprite = new QPixmap();
    greenSprite->load(":/assets/sprites/Green.png");
    QPixmap* blueSprite = new QPixmap();
    blueSprite->load(":/assets/sprites/Blue.png");
    QPixmap* purpleSprite = new QPixmap();
    purpleSprite->load(":/assets/sprites/Purple.png");
    QPixmap* yellowSprite = new QPixmap();
    yellowSprite->load(":/assets/sprites/Yellow.png");

    // add sprites to the map
    m_ColorSpriteMap[Gemme::Color::Red] = redSprite;
    m_ColorSpriteMap[Gemme::Color::Green] = greenSprite;
    m_ColorSpriteMap[Gemme::Color::Blue] = blueSprite;
    m_ColorSpriteMap[Gemme::Color::Purple] = purpleSprite;
    m_ColorSpriteMap[Gemme::Color::Yellow] = yellowSprite;
}


GemmeFactory::~GemmeFactory()
{
    // clear the gem pool (gems are automatically destroyed due to unique_ptr)
    m_GemPool.clear();

    // delete sprites
    /*delete m_ColorSpriteMap[Gem::Color::Red];
    delete m_ColorSpriteMap[Gem::Color::Green];
    delete m_ColorSpriteMap[Gem::Color::Blue];
    delete m_ColorSpriteMap[Gem::Color::Purple];
    delete m_ColorSpriteMap[Gem::Color::Yellow];*/

    m_ColorSpriteMap.clear();

}

Gemme* GemmeFactory::GetGem(Gemme::Color color)
{
    // try to find an unused gem from the pool
    for (Gemme* gem : m_GemPool)
    {
        if (gem->GetColor() == color && !gem->IsUsed())
        {
            // reset gem state
            gem->SetState(Gemme::State::Normal);
            gem->SetUsed(true);
            return gem;
        }
    }

    // if there is no unused item create one
    Gemme* gem(new Gemme(color, VuePlateau::ME));

    // assign corresponding sprite
    QPixmap* sprite = m_ColorSpriteMap[color];
    gem->SetQPixmap(*sprite);
    gem->SetUsed(true);

    // put gem into the pool
    m_GemPool.push_back(gem);

    return m_GemPool.back();
}
