#pragma once

#include <imgui/imgui.h>

namespace EditorStyles {
    namespace Frame {
        static constexpr ImVec4 BG = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
        static constexpr ImVec4 HOVERED = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
        static constexpr ImVec4 ACTIVE = ImVec4(0.1765f, 0.1765f, 0.1765f, 1.0f);
    }

    namespace Button {
        static constexpr ImVec4 DEFAULT = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        static constexpr ImVec4 HOVERED = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        static constexpr ImVec4 ACTIVE = ImVec4(0.1765f, 0.1765f, 0.1765f, 1.0f);

        static constexpr ImVec4 PROJECT_DEFAULT = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
        static constexpr ImVec4 PROJECT_HOVERED = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);

        static constexpr ImVec4 CLEAR = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

        static constexpr ImVec4 TITLEBAR_DEFAULT = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        static constexpr ImVec4 TITLEBAR_HOVERED = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        static constexpr ImVec4 TITLEBAR_ACTIVE = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    }

    namespace Header {
        static constexpr ImVec4 DEFAULT = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        static constexpr ImVec4 HOVER = ImVec4(0.5f, 0.5f, 0.5f, 0.70f);
        static constexpr ImVec4 ACTIVE = ImVec4(0.1765f, 0.1765f, 0.1765f, 1.0f);

        static constexpr ImVec4 SELECTED = ImVec4(0.5f, 0.5f, 0.5f, 0.80f);
        static constexpr ImVec4 SELECTED_HOVER = ImVec4(0.6f, 0.6f, 0.6f, 0.90f);
        static constexpr ImVec4 SELECTED_ACTIVE = ImVec4(0.1765f, 0.1765f, 0.1765f, 1.0f);
    }

    namespace UI {
        static constexpr ImVec4 CHILD_BG = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        static constexpr ImVec4 SLIDER_GRAB = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        static constexpr ImVec4 SLIDER_GRAB_ACTIVE = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
        static constexpr ImVec4 CHECKMARK = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
        static constexpr ImVec4 BORDER = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    }

    namespace Console {
        static constexpr ImVec4 ERROR = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
        static constexpr ImVec4 WARNING = ImVec4(1.0f, 0.8f, 0.4f, 1.0f);
        static constexpr ImVec4 INFO = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    namespace Panel {
        static constexpr ImVec4 BG_TOP_LEFT = ImVec4(0.158f, 0.158f, 0.158f, 1.0f);
        static constexpr ImVec4 BG_TOP_RIGHT = ImVec4(0.1765f, 0.1765f, 0.1765f, 1.0f);
        static constexpr ImVec4 BG_BOTTOM_LEFT = ImVec4(0.158f, 0.158f, 0.158f, 1.0f);
        static constexpr ImVec4 BG_BOTTOM_RIGHT = ImVec4(0.1765f, 0.1765f, 0.1765f, 1.0f);

        static constexpr ImVec4 HEADER_TOP_LEFT = ImVec4(0.090f, 0.082f, 0.078f, 1.0f);
        static constexpr ImVec4 HEADER_TOP_RIGHT = ImVec4(0.129f, 0.129f, 0.129f, 1.0f);
        static constexpr ImVec4 HEADER_BOTTOM_LEFT = ImVec4(0.090f, 0.082f, 0.078f, 1.0f);
        static constexpr ImVec4 HEADER_BOTTOM_RIGHT = ImVec4(0.129f, 0.129f, 0.129f, 1.0f);

        static constexpr ImVec4 TITLEBAR_TOP_LEFT = ImVec4(0.090f, 0.082f, 0.078f, 1.0f);
        static constexpr ImVec4 TITLEBAR_TOP_RIGHT = ImVec4(0.090f, 0.082f, 0.078f, 1.0f);
        static constexpr ImVec4 TITLEBAR_BOTTOM_LEFT = ImVec4(0.129f, 0.129f, 0.129f, 1.0f);
        static constexpr ImVec4 TITLEBAR_BOTTOM_RIGHT = ImVec4(0.129f, 0.129f, 0.129f, 1.0f);
    }

}

