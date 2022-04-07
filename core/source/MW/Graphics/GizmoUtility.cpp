/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Graphics/GizmoUtility.h>
#include <MW/Graphics/RenderQueue.h>
#include <MW/System/App.h>

namespace mw {
inline namespace gfx {

sptr<Mesh> GizmoUtility::CreateBox()
{
    sptr<Mesh> mesh = spnew<Mesh>();
    mesh->SetName("BoxGizmo");

    mesh->vertices.reserve(8);
    mesh->vertices.push_back(Vec3(-1, -1, -1));
    mesh->vertices.push_back(Vec3(-1, -1,  1));
    mesh->vertices.push_back(Vec3( 1, -1,  1));
    mesh->vertices.push_back(Vec3( 1, -1, -1));
    mesh->vertices.push_back(Vec3(-1,  1, -1));
    mesh->vertices.push_back(Vec3(-1,  1,  1));
    mesh->vertices.push_back(Vec3( 1,  1,  1));
    mesh->vertices.push_back(Vec3( 1,  1, -1));

    MeshElement element;

    element.drawMode = DrawMode::Lines;
    element.indices.reserve(24);
    element.indices = {
        0, 1, 1, 2, 2, 3, 3, 0,
        4, 5, 5, 6, 6, 7, 7, 4,
        0, 4, 1, 5, 2, 6, 3, 7
    };

    mesh->elements.push_back(std::move(element));

    mesh->RecalcBounds();
    mesh->UpdateBuffers();

    return mesh;
}

sptr<Mesh> GizmoUtility::CreateSphere()
{
    sptr<Mesh> mesh = spnew<Mesh>();
    mesh->SetName("SphereGizmo");

    mesh->vertices.resize(72);
    
    for (int angle = 0, i = 0; angle < 360; angle += 15, ++i)
    {
        float cr = cosf(math::ToRadians((float)angle));
        float sr = sinf(math::ToRadians((float)angle));

        int a = i + 0;
        int b = i + 24;
        int c = i + 48;

        mesh->vertices[a] = Vec3(cr, sr, 0);
        mesh->vertices[b] = Vec3(cr, 0, sr);
        mesh->vertices[c] = Vec3(0, cr, sr);
    }

    MeshElement element;
    
    element.drawMode = DrawMode::Lines;
    element.indices.reserve(144);
    element.indices =
    {
        0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9,  10, 10, 11, 11, 12,
        12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 0,

        24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36,
        36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 24,

        48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60,
        60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 71, 71, 48
    };

    mesh->elements.push_back(std::move(element));

    mesh->RecalcBounds();
    mesh->UpdateBuffers();

    return mesh;
}

sptr<Mesh> GizmoUtility::CreateHalfSphere()
{
    sptr<Mesh> mesh = spnew<Mesh>();
    mesh->SetName("HalfSphereGizmo");

    mesh->vertices.resize(50);

    // XZ circle (24)
    for (int angle = 0, i = 0; angle < 360; angle += 15, ++i)
    {
        float cr = cos(math::ToRadians((float)angle));
        float sr = sin(math::ToRadians((float)angle));
        mesh->vertices[i] = Vec3(cr, 0, sr);
    }

    // XY upward semicircle (13)
    for (int angle = -90, i = 0; angle <= 90; angle += 15, ++i)
    {
        float cr = cos(math::ToRadians((float)angle));
        float sr = sin(math::ToRadians((float)angle));
        mesh->vertices[i + 24] = Vec3(sr, cr, 0);
    }

    // YZ upward semicircle (13)
    for (int angle = -90, i = 0; angle <= 90; angle += 15, ++i)
    {
        float cr = cos(math::ToRadians((float)angle));
        float sr = sin(math::ToRadians((float)angle));
        mesh->vertices[i + 37] = Vec3(0, cr, sr);
    }

    MeshElement element;

    element.drawMode = DrawMode::Lines;
    element.indices.reserve(96);
    element.indices =
    {
        // XZ circle (48)
        0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9,  10, 10, 11, 11, 12,
        12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 0,

        // XY upward semicircle (24)
        24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36,

        // YZ upward semicircle (24)
        37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49
    };

    mesh->elements.push_back(std::move(element));

    mesh->RecalcBounds();
    mesh->UpdateBuffers();

    return mesh;
}

sptr<Mesh> GizmoUtility::CreateCylinder()
{
    sptr<Mesh> mesh = spnew<Mesh>();
    mesh->SetName("CylinderGizmo");

    mesh->vertices.resize(48);

    // XZ top circle (24)
    for (int angle = 0, i = 0; angle < 360; angle += 15, ++i)
    {
        float cr = cos(math::ToRadians((float)angle));
        float sr = sin(math::ToRadians((float)angle));
        mesh->vertices[i] = Vec3(cr, 0.5f, sr);
    }

    // XZ bottom circle (24)
    for (int angle = 0, i = 0; angle < 360; angle += 15, ++i)
    {
        float cr = cos(math::ToRadians((float)angle));
        float sr = sin(math::ToRadians((float)angle));
        mesh->vertices[i + 24] = Vec3(cr, -0.5f, sr);
    }

    MeshElement element;

    element.drawMode = DrawMode::Lines;
    element.indices.reserve(104);
    element.indices =
    {
        // XZ top circle (48)
        0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9,  10, 10, 11, 11, 12,
        12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 0,

        // XZ bottom circle (48)
        24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36,
        36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 24,

        // vertical lines (8)
        0, 24, 6, 30, 12, 36, 18, 42
    };

    mesh->elements.push_back(std::move(element));

    mesh->RecalcBounds();
    mesh->UpdateBuffers();

    return mesh;
}

} // gfx
} // mw
