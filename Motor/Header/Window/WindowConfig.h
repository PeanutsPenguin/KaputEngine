#pragma once

namespace KaputEngine
{
    struct WindowConfig
    {
        static constexpr int DEFAULT_WIDTH = 800;
        static constexpr int DEFAULT_HEIGHT = 600;
        static constexpr const char* DEFAULT_TITLE = "OpenGL Window";

        static constexpr float PI = 3.141592653589793f;
        static constexpr float DEG_TO_RAD = PI / 180.0f;
        static constexpr float RAD_TO_DEG = 180.0f / PI;

        static constexpr float DEFAULT_FOV = 45.0f * DEG_TO_RAD;
        static constexpr float DEFAULT_NEAR = 0.1f;
        static constexpr float DEFAULT_FAR = 100.0f;

#define OPENGL_VERSION_MAJOR 3
#define OPENGL_VERSION_MINOR 3

        _NODISCARD static constexpr float getAspectRatio(int width, int height)
        {
            return width / static_cast<float>(height);
        }
    };

enum UIWindowFlags
{
    E_NONE                              = 0,
    E_NO_TITLE_BAR                      = 1 << 0,
    E_NO_RESIZE                         = 1 << 1,
    E_NO_MOVE                           = 1 << 2,
    E_NO_SCROLL_BAR                     = 1 << 3,
    E_NO_SCROLL_WITH_MOUSE              = 1 << 4,
    E_NO_COLLAPSE                       = 1 << 5,
    E_AUTO_RESIZE                       = 1 << 6,
    E_NO_BACKGROUND                     = 1 << 7,
    E_NO_SAVED_SETTING                  = 1 << 8,
    E_NO_MOUSE_INPUTS                   = 1 << 9,
    E_MENU_BAR                          = 1 << 10,
    E_HORIZONTAL_SCROLL_BAR             = 1 << 11,
    E_NO_FOCUS                          = 1 << 12,
    E_NO_BRINGTOFRONT_FOCUS             = 1 << 13,
    E_ALWAYS_VERICAL_SCROLL_BAR         = 1 << 14,
    E_ALWAYS_HORIZONTAL_SCROLL_BAR      = 1 << 15,
    E_NO_NAVIGATION_INPUT               = 1 << 16,
    E_NO_NAVIGATION_FOCUS               = 1 << 17,
    E_UNSAVE_DOC                        = 1 << 18,
    E_NO_DOCKING                        = 1 << 19,

    E_NO_NAV = E_NO_NAVIGATION_INPUT | E_NO_NAVIGATION_FOCUS,
    E_NO_DECORATION = E_NO_TITLE_BAR | E_NO_RESIZE | E_NO_SCROLL_BAR | E_NO_COLLAPSE,
    E_NO_INPUTS = E_NO_MOUSE_INPUTS | E_NO_NAV
    };
}

enum TreeNodeFlags : int
{
    E_NO_NODE_FLAGS = 0,
    E_NODE_SELECTED = 1 << 0,   // Draw as selected
    E_DEFAULT_OPEN = 1 << 5,   // Default node to be open
    E_OPEN_ON_ARROW = 1 << 7, //Only open when clicking on arrow
    E_LEAF = 1 << 8,   // No collapsing, no arrow (use as a convenience for leaf nodes).

    E_LEAF_AND_SELECTED = E_NODE_SELECTED | E_LEAF | E_OPEN_ON_ARROW,

    E_LEAF_AND_OPEN = E_LEAF | E_DEFAULT_OPEN | E_OPEN_ON_ARROW,
    E_SELECTED_AND_OPEN = E_NODE_SELECTED | E_DEFAULT_OPEN | E_OPEN_ON_ARROW,
    E_LEAF_SELECTED_OPEN = E_NODE_SELECTED | E_LEAF | E_DEFAULT_OPEN | E_OPEN_ON_ARROW,
    E_BASIC = E_OPEN_ON_ARROW | E_DEFAULT_OPEN
};
