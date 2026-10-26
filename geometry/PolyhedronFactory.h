#ifndef POLYHEDRON_FACTORY_H
#define POLYHEDRON_FACTORY_H

#include <QString>

#include "Polyhedron.h"

class PolyhedronFactory {
public:
    enum Type {
        Tetrahedron,
        Hexahedron,
        Octahedron,
        Icosahedron,
        Dodecahedron
    };

    static Polyhedron create(Type type);
    static QString name(Type type);
};

#endif