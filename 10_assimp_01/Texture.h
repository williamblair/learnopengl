#ifndef _TEXTURE_H_INCLUDED_
#define _TEXTURE_H_INCLUDED_

#include <string>

typedef struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
} Texture;

#endif // _TEXTURE_H_INCLUDED_

