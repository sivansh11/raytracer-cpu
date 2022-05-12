#include "debug.h"

#include "window.h"
#include "imgui_setup.h"
#include "timer.h"
#include "texture.h"
#include "utils.h"
#include "ray.h"
#include "camera.h"
#include "shape.h"
#include "material.h"

#include <thread>

#include <glad/glad.h>

struct Settings
{
    int samplesPerPixel = 25;
    float fov = 90.0f;
    int maxDepth = 50;
    float EPSILON = 0.001;
    int numThreads = 1;
};


col3 rayColor(Ray& r, ShapeList &world, Settings &setting, int depth, ThreadLocal& tl)
{
    if (depth <= 0)
    {
        return col3{0, 0, 0};
    }

    HitRecord rec;
    if (world.hit(r, 0.0001, INFINITY, rec))
    {
        // point3 target = rec.p + rec.normal + randInHemisphere(rec.normal);
        // Ray r{rec.p, target - rec.p};
        // r.origin += rec.normal * setting.EPSILON;
        // return 0.5f * rayColor(r, world, setting, depth - 1);
        Ray scattered;
        col3 attenuation;
        if (rec.material->scatter(r, rec, attenuation, scattered, tl))
        {
            return attenuation * rayColor(scattered, world, setting, depth - 1, tl);
        }
        return col3(0, 0, 0);
    }

    vec3 uintDir = glm::normalize(r.direction);
    float t = .5 * (uintDir.y + 1.0);
    col3 c = (1.0f - t) * col3(1, 1, 1) + t * col3(.5, .7, 1.0);
    return c;
}

float render(int width, int height, Settings& setting, Texture2D &tex, Camera &cam, ShapeList &world, uint32_t* data)
{
    TimeIt timer;

    tex.remove();

    float scale = 1.0f / float(setting.samplesPerPixel);

    std::vector<std::thread> threads;

    for (int n=0; n<setting.numThreads; n++)
    {
        auto task = [&data, &setting, &cam, &world, &width, &height, n]()
        {
            // Thread local data
            ThreadLocal tl;
            tl.init(n);

            int columnsPerThread = width / setting.numThreads;

            // TODO: Switching i and j might be even faster
            for (int j = 0; j < height; j++)
            {
                for (int i = n * columnsPerThread; i <= (n + 1) * columnsPerThread && i < width; i++)
                {
                    col3 pixelCol(0, 0, 0);
                    for (int s=0; s<setting.samplesPerPixel; s++)
                    {
                        float u = float(i + tl.randFloat()) / (width - 1);
                        float v = float(j + tl.randFloat()) / (height - 1);;

                        Ray r = cam.getRay(u, v);

                        pixelCol += rayColor(r, world, setting, setting.maxDepth, tl);
                    }
                    float scale = 1.0f / setting.samplesPerPixel;
                    pixelCol.x = clamp(glm::sqrt(scale * pixelCol.x), 0.0f, 1.0f);
                    pixelCol.y = clamp(glm::sqrt(scale * pixelCol.y), 0.0f, 1.0f);
                    pixelCol.z = clamp(glm::sqrt(scale * pixelCol.z), 0.0f, 1.0f);

                    data[j * width + i] = color(pixelCol);
                }
            }
            // std::cout << "Thread " << n << " finished!\n";
        };
        threads.emplace_back(task);
    }

    for (int i=0; i<threads.size(); i++)
    {
        threads[i].join();
    }


    // for (int i=0; i<width; i++)
    // {
    //     for (int j=0; j<height; j++)
    //     {
    //         col3 pixelCol(0, 0, 0);
    //         for (int s=0; s<setting.samplesPerPixel; s++)
    //         {
    //             float u = float(i + randFloat()) / (width - 1);
    //             float v = float(j + randFloat()) / (height - 1);;

    //             Ray r = cam.getRay(u, v);

    //             pixelCol += rayColor(r, world, setting, setting.maxDepth);
    //         }
    //         float scale = 1.0f / setting.samplesPerPixel;
    //         pixelCol.x = clamp(glm::sqrt(scale * pixelCol.x), 0.0f, 1.0f);
    //         pixelCol.y = clamp(glm::sqrt(scale * pixelCol.y), 0.0f, 1.0f);
    //         pixelCol.z = clamp(glm::sqrt(scale * pixelCol.z), 0.0f, 1.0f);

    //         data[j * width + i] = color(pixelCol);
    //     }
    // }


    tex.loadData(width, height, data);
    return timer.now() / 1000;
}

int main()
{
    Window window(200, 400, "ray tracing");

    myImGuiSetup(window);
    static ImVec3 color_for_text = ImVec3(float(255) / 255,float(255) / 255,float(255) / 255);
    static ImVec3 color_for_head = ImVec3(float(80) / 255,float(80) / 255,float(80) / 255);
    static ImVec3 color_for_area = ImVec3(float(50) / 255,float(50) / 255,float(50) / 255);
    static ImVec3 color_for_body = ImVec3(float(0) / 255,float(0) / 255,float(0) / 255);
    static ImVec3 color_for_pops = ImVec3(float(45) / 255,float(45) / 255,float(45) / 255);
    static ImVec3 color_for_tabs = ImVec3(float(0) / 255,float(0) / 255,float(0) / 255);
    imgui_easy_theming(color_for_text, color_for_head, color_for_area, color_for_body, color_for_pops, color_for_tabs);

    Texture2D tex;
    int width = 50, height = 50;
    uint32_t *data = new uint32_t[width * height];
    static ImVec2 size = {50, 50};

    float time = 0;

    Camera cam(point3(0, 0, 0), point3(0, 0, -1), vec3(0, 1, 0), 1 / 1, 90.0f);

    ShapeList world;

    Material *ground = new Lambertian(col3(.8, .8, 0));
    Material *center = new Lambertian(col3(.1, .2, .5));
    Material *left = new Dielectric(1.5f);
    Material *right = new Metal(col3(.8, .6, .2), 0);

    world.add(new Sphere(point3{0, -100.5, -1}, 100.f, ground));
    world.add(new Sphere(point3{0, 0, -1}, 0.5f, center));
    world.add(new Sphere(point3{-1, 0, -1}, 0.5f, left));
    world.add(new Sphere(point3{-1, 0, -1}, -0.45f, left));
    world.add(new Sphere(point3{ 1, 0, -1}, 0.5f, right));

    // Material *left = new Lambertian(col3(0,0,1));
    // Material *right = new Lambertian(col3(1,0,0));

    // world.add(new Sphere(point3(-2, 0, -1), 2, left));
    // world.add(new Sphere(point3( 2, 0, -1), 2, right));

    Settings setting;

    while (!window.shouldClose())
    {
        window.startFrame();

        myImGuiStartFrame();

        glCall(glClearColor(1, 1, 1, 1));
        glCall(glClear(GL_COLOR_BUFFER_BIT));

        bool color_editor = false;
        if (color_editor)
        {
            ImGui::Begin("color editor", &color_editor);
            ImGui::ColorPicker3("text", (float*)(&color_for_text));
            ImGui::ColorPicker3("head", (float*)(&color_for_head));
            ImGui::ColorPicker3("area", (float*)(&color_for_area));
            ImGui::ColorPicker3("body", (float*)(&color_for_body));
            ImGui::ColorPicker3("pops", (float*)(&color_for_pops));
            ImGui::ColorPicker3("tabs", (float*)(&color_for_tabs));
            imgui_easy_theming(color_for_text, color_for_head, color_for_area, color_for_body, color_for_pops, color_for_tabs);
            ImGui::End();
        }

        ImGui::Begin("Settings");

        if (ImGui::Button("render"))
        {
            tex.loadData(width, height, data);
            cam.set(point3(-2, 2, 1), point3(0, 0, -1), vec3(0, 1, 0), size.x / size.y, setting.fov);
            time = render(size.x, size.y, setting, tex, cam, world, data);
        }
        ImGui::Text("%f ms taken", time);
        ImGui::DragFloat("fov", &setting.fov);
        ImGui::DragInt("samples per pixel", &setting.samplesPerPixel);
        ImGui::DragInt("max depth", &setting.maxDepth);
        ImGui::DragFloat("epsilon", &setting.EPSILON, 0.0001);
        ImGui::DragInt("threads", &setting.numThreads, 1, 1, 12);

        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Scene");
        size = ImGui::GetContentRegionAvail();
        if (size.x != width || size.y != height)
        {
            delete[] data;
            width = size.x;
            height = size.y;
            data = new uint32_t[width * height];
        }
        if (tex.getHandle() != 0)
        {
            ImGui::Image((void*)(tex.getHandle()), {(float)tex.width, (float)tex.height}, {0.f, 1.f}, {1.f, 0.f});
        }

        ImGui::End();
        ImGui::PopStyleVar();


        myImGuiEndFrame();

        window.endFrame();
    }

    delete[] data;
    myImGuiBye();
}
