/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Color:ColorJson;
import Microwave.System.Json;
import :Color32F_p1;
import :Color32F_p2;
import :Color8U_p1;
import :Color8U_p2;
import :ColorBGRA_p1;
import :ColorBGRA_p2;

export namespace mw {
inline namespace gfx {

void to_json(json& obj, const Color& c)
{
    obj["r"] = c.r;
    obj["g"] = c.g;
    obj["b"] = c.b;
    obj["a"] = c.a;
}

void from_json(const json& obj, Color& c)
{
    c.r = obj.value("r", c.r);
    c.g = obj.value("g", c.g);
    c.b = obj.value("b", c.b);
    c.a = obj.value("a", c.a);
}

}
}
