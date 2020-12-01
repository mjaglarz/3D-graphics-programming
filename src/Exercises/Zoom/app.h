//
// Created by pbialas on 05.08.2020.
//


#pragma once

#include <vector>
#include <glm/mat4x4.hpp>

#include "Application/application.h"
#include "Application/utils.h"
#include "glad/glad.h"
#include "camera.h"

class SimpleShapeApplication : public xe::Application {
public:
    SimpleShapeApplication(int width, int height, std::string title, int major = 4, int minor = 1) :
            Application(width, height, title, major, minor) {}

    void init() override;

    void frame() override;

    void framebuffer_resize_callback(int w, int h) override;

    void scroll_callback(double xoffset, double yoffset) override {
        Application::scroll_callback(xoffset, yoffset);
        camera()->zoom(yoffset / 30.0f);
    }

    void set_camera(Camera *camera) { camera_ = camera; }

    Camera *camera() { return camera_; }

    ~SimpleShapeApplication() {
        if (camera_) {
            delete camera_;
        }
    }

private:
    Camera *camera_;

    GLuint vao_;
    GLuint u_pvm_buffer_;
};