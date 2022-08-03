import Microwave.Tools.IEditor;
import Microwave.Tools.SceneEditor;

extern "C" {
__declspec(dllexport) mw::tools::IEditor* __cdecl MWGetSceneEditor() {
    return &mw::tools::SceneEditor::Get();
}
}
