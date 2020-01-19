#ifndef GEMMEFACTORY_H
#define GEMMEFACTORY_H

#include "gemme.h"
#include <memory>
#include <QMap>

class QPixmap;
class Gemme;

/*! Produces and recycles gems.
*	Implementation of the Flyweight pattern: all gems of the same color share the same sprite.
*	If gems are no longer used they get recycled and can be used again.
*/

class GemmeFactory
{
public:
    GemmeFactory();
    virtual ~GemmeFactory();

    /*! Retrieves a pointer to a gem of the specified color */
    Gemme* GetGem(Gemme::Color color);

private:
    // map that contains all sprites
    QMap <Gemme::Color, QPixmap*> m_ColorSpriteMap;
    // pool that holds all gems
    QVector<Gemme*> m_GemPool;
};

#endif // GEMMEFACTORY_H
