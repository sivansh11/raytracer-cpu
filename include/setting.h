#ifndef SETTING_H
#define SETTING_H

struct Settings
{
    int samplesPerPixel = 100;
    float fov = 20.0f;
    int maxDepth = 50;
    float EPSILON = 0.001;
    int numThreads = 12;
    col3 background{0};
};

#endif