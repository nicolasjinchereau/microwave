/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.System.Window;
import Microwave.Graphics.Internal.HWSurface;

namespace mw {
inline namespace system {

void Window::AddEventHandler(const sptr<IWindowEventHandler>& handler) {
    eventHandlers.Add(handler);
}

void Window::RemoveEventHandler(const sptr<IWindowEventHandler>& handler) {
    eventHandlers.Remove(handler);
}

void Window::OnCreate() {
    eventHandlers.RaiseEvent(&IWindowEventHandler::OnCreate, this);
}

void Window::OnShow() {
    eventHandlers.RaiseEvent(&IWindowEventHandler::OnShow, this);
}

void Window::OnGotFocus() {
    eventHandlers.RaiseEvent(&IWindowEventHandler::OnGotFocus, this);
}

void Window::OnLostFocus() {
    eventHandlers.RaiseEvent(&IWindowEventHandler::OnLostFocus, this);
}

void Window::OnHide() {
    eventHandlers.RaiseEvent(&IWindowEventHandler::OnHide, this);
}

void Window::OnDestroy() {
    eventHandlers.RaiseEvent(&IWindowEventHandler::OnDestroy, this);
}

void Window::OnClose() {
    eventHandlers.RaiseEvent(&IWindowEventHandler::OnClose, this);
}

void Window::OnMove(IVec2 pos) {
    eventHandlers.RaiseEvent(&IWindowEventHandler::OnMove, this, pos);
}

void Window::OnResize(IVec2 size) {
    if(surface) surface->UpdateSize();
    eventHandlers.RaiseEvent(&IWindowEventHandler::OnResize, this, size);
}

void Window::OnKeyDown(Keycode key) {
    eventHandlers.RaiseEvent(&IWindowEventHandler::OnKeyDown, this, key);
}

void Window::OnKeyUp(Keycode key) {
    eventHandlers.RaiseEvent(&IWindowEventHandler::OnKeyUp, this, key);
}

void Window::OnPointerDown(IVec2 pos, int id) {
    eventHandlers.RaiseEvent(&IWindowEventHandler::OnPointerDown, this, pos, id);
}

void Window::OnPointerMove(IVec2 pos, int id) {
    eventHandlers.RaiseEvent(&IWindowEventHandler::OnPointerMove, this, pos, id);
}

void Window::OnPointerUp(IVec2 pos, int id) {
    eventHandlers.RaiseEvent(&IWindowEventHandler::OnPointerUp, this, pos, id);
}

} // system
} // mw
