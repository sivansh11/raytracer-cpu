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
#include "setting.h"
#include "scene_examples.h"

#include <thread>

#include <glad/glad.h>

col3 rayColor(Ray& r, ShapeList &world, Settings &setting, int depth, ThreadLocal& tl)
{
    if (depth <= 0)
    {
        return col3{0, 0, 0};
    }

    HitRecord rec;
    if (!world.hit(r, 0.0001, INFINITY, rec))
    {
        return setting.background;   
    }
    Ray scattered;
    col3 attenuation;
    col3 emitted = rec.material->emitted(rec.u, rec.v, rec.p);
    if (!rec.material->scatter(r, rec, attenuation, scattered, tl))
    {
        return emitted;
    }
    return emitted + attenuation * rayColor(scattered, world, setting, depth - 1, tl);

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

    point3 from, at;
    Camera cam(from, at, vec3(0, 1, 0), 1 / 1, 90.0f);
    ShapeList world;
    MaterialList materials;
    Settings setting;

    my_example_scene(world, materials, setting, from, at);
//     triangle_example(world, materials, setting, from, at);

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
            cam.set(from, at, vec3(0, 1, 0), size.x / size.y, setting.fov);
            time = render(size.x, size.y, setting, tex, cam, world, data);
        }
        ImGui::SameLine();
        if (ImGui::Button("save"))
        {
            std::cout << "P3\n" << width << ' ' << height << "\n255\n";
            for (int j = height-1; j >= 0; --j) {
                for (int i = 0; i < width; ++i) {
                    uint32_t rgba = data[j * width + i];
                    uint8_t col[4];
                    colorConversion(rgba, col);
                    std::cout << int(col[0]) << ' ' << int(col[1]) << ' ' << int(col[2]) << '\n';
                }
            }
        }
        ImGui::Text("%f ms taken", time);
        ImGui::DragFloat("fov", &setting.fov);
        ImGui::DragInt("samples per pixel", &setting.samplesPerPixel);
        ImGui::DragInt("max depth", &setting.maxDepth);
        ImGui::DragFloat("epsilon", &setting.EPSILON, 0.0001);
        ImGui::DragInt("threads", &setting.numThreads, 1, 1, 12);
        ImGui::DragFloat3("background", (float*)(&setting.background));

        ImGui::NewLine();

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
