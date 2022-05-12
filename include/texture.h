#ifndef TEXTURE_H
#define TEXTURE_H

#include "debug.h"

#include <glad/glad.h>

class Texture2D
{
public:
    Texture2D() = default;
    ~Texture2D()
    {
        glCall(glDeleteTextures(1, &id));
    }
    void loadData(int width, int height, void *data)
    {
        Texture2D::width = width;
        Texture2D::height = height;

        glCall(glGenTextures(1, &id));

        glCall(glBindTexture(GL_TEXTURE_2D, id));

        glCall(glBindTexture(GL_TEXTURE_2D, id));
        glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

        glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
        glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));

        glCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
    }
    void remove()
    {
        glCall(glDeleteTextures(1, &id));
    }
    GLuint getHandle() { return id; }

    int width, height;

private:
    GLuint id{};
};

#endif