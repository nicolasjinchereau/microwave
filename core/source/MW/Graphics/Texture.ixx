/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Texture;
import Microwave.Graphics.GraphicsTypes;
import Microwave.Graphics.Image;
import Microwave.Graphics.Internal.HWTexture;
import Microwave.Math;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Path;
import Microwave.System.Pointers;
import Microwave.System.Task;
import <cassert>;
import <cstddef>;
import <cstdint>;
import <span>;
import <string>;
import <vector>;

export namespace mw {
inline namespace gfx {

enum class LoadState
{
    Unloaded,
    Loading,
    Loaded
};

class Texture : public Object
{
    inline static Type::Pin<Texture> pin;

    path filePath;
    ImageFileFormat fileFormat{};
    PixelDataFormat format{};
    IVec2 size = IVec2::Zero();
    bool dynamic{};

    TextureWrapMode wrapMode = TextureWrapMode::Clamp;
    TextureFilterMode filterMode = TextureFilterMode::Bilinear;
    float anisoLevel = 1;

    sptr<HWTexture> tex;
    LoadState loadState = LoadState::Unloaded;

public:

    Texture(
        const path& filePath,
        ImageFileFormat fileFormat,
        bool dynamic,
        bool deferLoading = false
    );

    Texture(
        const std::span<std::byte>& data,
        PixelDataFormat format,
        const IVec2& size,
        bool dynamic
    );

    sptr<HWTexture> GetHWTexture();
    PixelDataFormat GetFormat() const;
    IVec2 GetSize() const;
    bool IsDynamic() const;

    void SetPixels(const std::span<std::byte>& data);
    void SetPixels(const std::span<std::byte>& data, const IntRect& rect);

    void SetWrapMode(TextureWrapMode mode);
    TextureWrapMode GetWrapMode();

    void SetFilterMode(TextureFilterMode mode);
    TextureFilterMode GetFilterMode() const;

    void SetAnisoLevel(float aniso);
    float GetAnisoLevel() const;

    void LoadFile();
    Task<void> LoadFileAsync();
    void UnloadFile();

    LoadState GetLoadState() const;
};

} // gfx
} // mw
