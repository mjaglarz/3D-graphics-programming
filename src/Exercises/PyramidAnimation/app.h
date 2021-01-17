//
// Created by pbialas on 05.08.2020.
//


#pragma once

#include <vector>
#include <chrono>
#include <glm/mat4x4.hpp>

#include "Application/application.h"
#include "Application/utils.h"
#include "glad/glad.h"
#include "camera.h"
#include "camera_controler.h"
#include "pyramid.h"

class SimpleShapeApplication : public xe::Application {
public:
    SimpleShapeApplication(int width, int height, std::string title, int major = 4, int minor = 1) :
            Application(width, height, title, major, minor) {}

    void init() override;
    void frame() override;
    void framebuffer_resize_callback(int w, int h) override;
    void mouse_button_callback(int button, int action, int mods) override;
    void cursor_position_callback(double x, double y) override;
    void scroll_callback(double xoffset, double yoffset) override;

    void set_camera(Camera *camera) { camera_ = camera; }

    Camera *camera() { return camera_; }

    void set_controler(CameraControler *controler) { controler_ = controler; }

    std::shared_ptr<Pyramid> pyramid_;

    ~SimpleShapeApplication() {
        if (camera_) {
            delete camera_;
        }
    }

private:
    Camera *camera_;
    CameraControler *controler_;

    GLuint vao_;
    GLuint u_pvm_buffer_;

    std::chrono::steady_clock::time_point start_;
    float rotation_period;
};