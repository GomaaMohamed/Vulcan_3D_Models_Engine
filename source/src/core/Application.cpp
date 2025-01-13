#include "Application.hpp"

namespace VkEngine
{
    // Initialize static members
    //-------------------------------------------------------------------------------------------
    void (*Application::mouse_)(void) = nullptr;
    void (*Application::mouse_scroll_)(void) = nullptr;
    VkCore *Application::vk_core_ = nullptr;

    // Constructors and class data types
    //--------------------------------------------------------------------------------------------
    Application::Application(const std::string &_name, const int &_width, const int &_height):window_(nullptr)                            
    {
        try
        {
            // Initialize window using glfw
            init_window(_name, _width, _height);
            // Initialize vulcan
            vk_core_ = new VkCore(window_);
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
    Application::~Application()
    {
        glfwDestroyWindow(window_);
        glfwTerminate();
    }
    void Application::draw_frame()
    {
        vk_core_->draw_frame();
    }
    void Application::wait_idle()
    {
        vk_core_->wait_idle();
    }
    // Composite functions
    //--------------------------------------------------------------------------------------------
    void Application::init_window(const std::string &_name, const int &_width, const int &_height)
    {
        // Initialize glfw
        glfwInit();
        // Make window hints
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        // Create a window and opengl context and attach them
        window_ = glfwCreateWindow(_width, _height, _name.c_str(), NULL, NULL);
        if (window_ == NULL)
        {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }
        glfwSetWindowUserPointer(window_, this);
        glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    // Window, callbacks, and inputs handling functions
        //--------------------------------------------------------------------------------------------
    bool Application::window_should_close()
    {
        return glfwWindowShouldClose(window_);
    }
    void Application::set_window_should_close(bool _state)
    {
        glfwSetWindowShouldClose(window_, _state);
    }
    void Application::swap_buffers_and_poll_events()
    {
        glfwPollEvents();
    }
    std::pair<int, int> Application::get_window_dimentions()
    {
        int width, height;
        return std::pair<int, int>(width, height);
    }
    bool Application::key_pressed(Key _key)
    {
        int keyState = glfwGetKey(window_, (int)_key);
        return keyState == GLFW_PRESS || keyState == GLFW_REPEAT;
    }
    bool Application::key_released(Key _key)
    {
        int keyState = glfwGetKey(window_, (int)_key);
        return keyState == GLFW_RELEASE;
    }
    bool Application::key_down(Key _key)
    {
        int keyState = glfwGetKey(window_, (int)_key);
        return keyState == GLFW_PRESS && keyState != GLFW_REPEAT;
    }
    bool Application::mouse_button_pressed(MouseButton _button)
    {
        int buttonState = glfwGetMouseButton(window_, (int)_button);

        return buttonState == GLFW_PRESS || buttonState == GLFW_REPEAT;
    }
    bool Application::mouse_button_released(MouseButton _button)
    {
        int buttonState = glfwGetMouseButton(window_, (int)_button);

        return buttonState == GLFW_RELEASE;
    }
    float Application::get_mouse_x()
    {
        auto [x, y] = get_mouse_position();
        return (float)x;
    }
    float Application::get_mouse_y()
    {
        auto [x, y] = get_mouse_position();
        return (float)y;
    }
    std::pair<float, float> Application::get_mouse_position()
    {
        double x, y;
        glfwGetCursorPos(window_, &x, &y);
        return std::make_pair<float, float>((float)x, float(y));
    }
    void Application::set_mouse_callback(void (*_mouse)(void))
    {
        mouse_ = _mouse;
    }
    void Application::set_mouse_scroll_callback(void (*_mouse_scroll)(void))
    {
        mouse_scroll_ = _mouse_scroll;
    }
    void Application::framebuffer_size_callback(GLFWwindow *window, int width, int height)
    {
        vk_core_->set_frame_resized();
    }
    // glfw: whenever the mouse moves, this callback is called
    // -------------------------------------------------------
    void Application::mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
    {
    }
    // glfw: whenever the mouse scroll wheel scrolls, this callback is called
    // ----------------------------------------------------------------------
    void Application::mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
    {
    }

}