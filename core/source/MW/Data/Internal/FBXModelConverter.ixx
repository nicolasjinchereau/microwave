/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Data.Internal.FBXModelConverter;
import Microwave.Data.Library.AssetSettings;
import Microwave.Graphics.Model;
import Microwave.IO.Stream;
import Microwave.System.Pointers;

export namespace mw {
inline namespace data {

class FBXModelConverter
{
public:
    FBXModelConverter(){}
    static gptr<Model> Convert(
        const gptr<Stream>& stream,
        ModelSettings& importSettings);
};

} // data
} // mw
