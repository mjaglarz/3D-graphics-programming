//
// Created by pbialas on 25.09.2020.
//

#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>


#include "Application/utils.h"
#include <glm/mat4x4.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void SimpleShapeApplication::init() {
    auto program = xe::create_program(std::string(PROJECT_DIR) + "/shaders/base_vs.glsl",
                                      std::string(PROJECT_DIR) + "/shaders/base_fs.glsl");

    if (!program) {
        std::cerr << "Cannot create program from " << std::string(PROJECT_DIR) + "/shaders/base_vs.glsl" << " and ";
        std::cerr << std::string(PROJECT_DIR) + "/shaders/base_fs.glsl" << " shader files" << std::endl;
    }

    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);
    int w, h;
    std::tie(w, h) = frame_buffer_size();
    glViewport(0, 0, w, h);

    glEnable(GL_DEPTH_TEST);
    glUseProgram(program);

    set_camera(new Camera);
    set_controler(new CameraControler(camera()));

    pyramid_ = std::make_shared<Pyramid>();
    rotation_period = 4.0;

    glm::vec3 eye = glm::vec3(-0.5f, 12.0f, 1.0f);
    glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    float fov = glm::pi<float>()/2.0;
    float aspect = (float)w/h;
    float near = 0.1f;
    float far = 100.0f;

    camera()->perspective(fov, aspect, near, far);
    camera()->look_at(eye, center, up);

    glGenBuffers(1, &u_pvm_buffer_);
    glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, u_pvm_buffer_);

    auto u_transformations_index = glGetUniformBlockIndex(program, "Transformations");
    if (u_transformations_index == GL_INVALID_INDEX) {
        std::cout << "Cannot find Transformations uniform block in program" << std::endl;
    } else {
        glUniformBlockBinding(program, u_transformations_index, 1);
    }

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    start_ = std::chrono::steady_clock::now();
}

void SimpleShapeApplication::frame() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::duration<float>>(now - start_).count();

    float a = 10.0f;
    float b = 8.0f;
    float orbital_rotation_period = 20.0f;
    float orbital_rotation_angle = 2.0f * glm::pi<float>() * elapsed_time / orbital_rotation_period;
    float x = a * cos(orbital_rotation_angle);
    float z = b * sin(orbital_rotation_angle);
    auto O = glm::translate(glm::mat4(1.0f), glm::vec3{x, 0.0f, z});
    auto rotation_angle = 2.0f * glm::pi<float>() * elapsed_time / rotation_period;
    auto axis = glm::vec3(0.0f, 1.0f, 0.0f);
    auto R = glm::rotate(glm::mat4(1.0f), rotation_angle, axis);
    auto M = O * R;

    glm::mat4 PVM = camera()->projection() * camera()->view() * M;

    glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    pyramid_->draw();

    float moon_rotation_period = 10.0f;
    float moon_rotation_angle = 2.0f * glm::pi<float>() * elapsed_time / moon_rotation_period;
    float moon_x = 3.0f * cos(moon_rotation_angle);
    float moon_z = 3.0f * sin(moon_rotation_angle);
    auto moon_O = glm::translate(glm::mat4(1.0f), glm::vec3{moon_x, 0.0f, moon_z});
    auto moon_R = glm::rotate(glm::mat4(1.0f), moon_rotation_angle, axis);
    auto moon_S = glm::scale(glm::mat4(1.0f), glm::vec3{0.5f, 0.5f, 0.5f});
    auto moon_M = moon_O * moon_R * moon_S;

    glm::mat4 moon_PVM = camera()->projection() * camera()->view() * M * moon_M;

    glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &moon_PVM[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    pyramid_->draw();

    float satellite_rotation_period = 2.0f;
    float satellite_rotation_angle = 2.0f * glm::pi<float>() * elapsed_time / satellite_rotation_period;
    float satellite_x = 1.5 * cos(satellite_rotation_angle);
    float satellite_y = 1.5 * sin(satellite_rotation_angle);
    auto satellite_O = glm::translate(glm::mat4(1.0f), glm::vec3{satellite_x, satellite_y, 0.0f});
    auto satellite_R = glm::rotate(glm::mat4(1.0f), satellite_rotation_angle, axis);
    auto satellite_S = glm::scale(glm::mat4(1.0f), glm::vec3{0.25f, 0.25f, 0.25f});
    auto satellite_M = satellite_O * satellite_R * satellite_S;

    glm::mat4 satellite_PVM = camera()->projection() * camera()->view() * M * satellite_M;

    glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &satellite_PVM[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    pyramid_->draw();
}

void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
    Application::framebuffer_resize_callback(w, h);
    glViewport(0, 0, w, h);
    float aspect = (float)w/h;
    camera()->set_aspect(aspect);
}

void SimpleShapeApplication::mouse_button_callback(int button, int action, int mods) {
    Application::mouse_button_callback(button, action, mods);

    if (controler_) {
        double x, y;
        glfwGetCursorPos(window_, &x, &y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
            controler_->LMB_pressed(x, y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
            controler_->LMB_released(x, y);
    }
}

void SimpleShapeApplication::cursor_position_callback(double x, double y) {
    Application::cursor_position_callback(x, y);
    if (controler_) {
        controler_->mouse_moved(x, y);
    }
}

void SimpleShapeApplication::scroll_callback(double xoffset, double yoffset) {
    Application::scroll_callback(xoffset, yoffset);
    camera()->zoom(yoffset / 30.0f);
}
