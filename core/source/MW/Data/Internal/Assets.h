/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
import <string>;
import <unordered_map>;

namespace mw {
inline namespace data {

inline std::unordered_map<std::string, std::string_view> internalAssets =
{
    {
        ".internal/default.cg",
        std::string_view(
            #include <MW/Data/Internal/Assets/default.cg>
        )
    },
    {
        ".internal/ui-default.cg",
        std::string_view(
            #include <MW/Data/Internal/Assets/ui-default.cg>
        )
    },
    {
        ".internal/lit_painted.cg",
        std::string_view(
            #include <MW/Data/Internal/Assets/lit_painted.cg>
        )
    },
    {
        ".internal/lit_painted_textured.cg",
        std::string_view(
            #include <MW/Data/Internal/Assets/lit_painted_textured.cg>
        )
    },
    {
        ".internal/lit_textured.cg",
        std::string_view(
            #include <MW/Data/Internal/Assets/lit_textured.cg>
        )
    },
    {
        ".internal/lit_tinted.cg",
        std::string_view(
            #include <MW/Data/Internal/Assets/lit_tinted.cg>
        )
    },
    {
        ".internal/lit_tinted_textured.cg",
        std::string_view(
            #include <MW/Data/Internal/Assets/lit_tinted_textured.cg>
        )
    },
    {
        ".internal/unlit_painted.cg",
        std::string_view(
            #include <MW/Data/Internal/Assets/unlit_painted.cg>
        )
    },
    {
        ".internal/unlit_painted_textured.cg",
        std::string_view(
            #include <MW/Data/Internal/Assets/unlit_painted_textured.cg>
        )
    },
    {
        ".internal/unlit_text.cg",
        std::string_view(
            #include <MW/Data/Internal/Assets/unlit_text.cg>
        )
    },
    {
        ".internal/unlit_textured.cg",
        std::string_view(
            #include <MW/Data/Internal/Assets/unlit_textured.cg>
        )
    },
    {
        ".internal/unlit_tinted.cg",
        std::string_view(
            #include <MW/Data/Internal/Assets/unlit_tinted.cg>
        )
    },
    {
        ".internal/unlit_tinted_textured.cg",
        std::string_view(
            #include <MW/Data/Internal/Assets/unlit_tinted_textured.cg>
        )
    },
    {
        ".internal/box-gizmo.mesh",
        std::string_view(
            #include <MW/Data/Internal/Assets/box-gizmo.mesh>
        )
    },
    {
        ".internal/sphere-gizmo.mesh",
        std::string_view(
            #include <MW/Data/Internal/Assets/sphere-gizmo.mesh>
        )
    },
    {
        ".internal/half-sphere-gizmo.mesh",
        std::string_view(
            #include <MW/Data/Internal/Assets/half-sphere-gizmo.mesh>
        )
    },
    {
        ".internal/cylinder-gizmo.mesh",
        std::string_view(
            #include <MW/Data/Internal/Assets/cylinder-gizmo.mesh>
        )
    },
    {
        ".internal/wire-gizmo.mat",
        std::string_view(
            #include <MW/Data/Internal/Assets/wire-gizmo.mat>
        )
    }
};

} // data
} // mw
