#ifndef TEXTUREINFO_H
#define TEXTUREINFO_H

#include "utils/rgba.h"
#include <string>

struct TextureInfo{

    int width;
    int height;
    RGBA* rgba;
    std::string filename;
};

#endif // TEXTUREINFO_H
