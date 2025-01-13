#ifndef APPLICATION_H_
#define APPLICATION_H_
// File inclusions
//--------------------------
#include "VkCore.hpp"

namespace VkEngine
{
    // Window default dimentions
    //-----------------------------
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    // Define the application class
    //-------------------------------
    class Application
    {
    public:
        // Constructors and destructors
        //-------------------------------
        Application(const std::string &_name, const int &_width = WIDTH, const int &_height = HEIGHT);
        ~Application();
        // Methods for window
        //--------------------------------
        bool window_should_close();
        std::pair<int, int> get_window_dimentions();
        void set_window_should_close(bool _state);
        void swap_buffers_and_poll_events();
        // Methods for inputs handling
        //--------------------------------
        bool key_pressed(Key _key);
        bool key_released(Key _key);
        bool key_down(Key _key);
        bool mouse_button_pressed(MouseButton _button);
        bool mouse_button_released(MouseButton _button);
        float get_mouse_x();
        float get_mouse_y();
        std::pair<float, float> get_mouse_position();
        // Methods to set callbacks
        //-------------------------------
        void set_mouse_callback(void (*_mouse)(void));
        void set_mouse_scroll_callback(void (*_mouse_scroll)(void));
        // Draw methods
        void draw_frame();
        void wait_idle();

    private:
        // Class data members
        //---------------------------------------------
        GLFWwindow *window_;
        static VkCore *vk_core_;
        static void (*mouse_)(void);
        static void (*mouse_scroll_)(void);
        // Callback functions
        //-----------------------------------------------
        static void framebuffer_size_callback(GLFWwindow *_window, int _width, int _height);
        static void mouse_callback(GLFWwindow *_window, double _xpos, double _ypos);
        static void mouse_scroll_callback(GLFWwindow *_window, double _xoffset, double _yoffset);
        // Class private methods
        //-----------------------------------------------
        // Composite functions
        void init_window(const std::string &_name, const int &_width, const int &_height);
    };
}


#endif