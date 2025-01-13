#include <memory>
#include "Application.hpp"

// Application holder
std::shared_ptr<VkEngine::Application> app;
// Callback function to be called at window resizing
void process_inputs();

int main()
{
    // Application: initialize and create a window
    //-----------------------------------------------------------------------
    app = std::make_shared<VkEngine::Application>("Vulcan Window");

    // Render loop
    // -----------
    while (!app->window_should_close())
    {
        // Application: handle inputs
        // --------------------------------------------------------
        process_inputs();

        // Pipeline: render models
        app->draw_frame();
        // Application: swap buffers and poll IO events
        // ---------------------------------------------------------
        app->swap_buffers_and_poll_events();
    }
    app->wait_idle();
    return 0;
}

// Callbacks implementation
// ---------------------------------------------------------------------------------------------
void process_inputs()
{
   
}














