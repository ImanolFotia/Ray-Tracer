#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <sphere.h>
#include <ray.h>
#include <plane.h>
#include <exception>
#include <array>
#include <vec3.h>
#include <helpers.hpp>
#include <TriangleMesh.hpp>
#include <omp.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <shader.h>

using namespace std;

const float kInfinity = std::numeric_limits<float>::max();
const int WIDTH = 1024;
const int HEIGHT = 768;

std::array<std::array<vec3, WIDTH>, HEIGHT> out;

float hash( glm::vec2 p ) {
    return glm::fract(glm::sin(p.x*15.32+p.y*35.78) * 43758.23);
}


class FullScreenQuad {
public:
    FullScreenQuad() {}
    ~FullScreenQuad() {}

    void Render() {
        if (quadVAO == 0) {
            GLfloat quadVertices[] = {
                // Positions         //Texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f
            };
            // Setup plane VAO
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        }
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }
private:

    GLuint quadVAO, quadVBO;
};

        struct col{
            uint8_t x;
            uint8_t y;
            uint8_t z;
        };
        col c[HEIGHT][WIDTH];

        float DepthBuffer[HEIGHT][WIDTH] = {kInfinity};
int main() {

    std::cout << "=============== C++ CPU Ray Tracer. ===============" << endl;
    std::cout << "Output image: output.png" << endl << "Width: " << WIDTH << endl << "Height: " << HEIGHT << endl;
    std::cout << "Format: PNG" << endl << "3 Channels." << endl << "8 bit per pixel." << endl;

    if(!glfwInit())
        cout << "couldn't initiate glfw" << endl;
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "C++ Raytracer", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glewExperimental = true;

    if(glewInit()!= GLEW_OK)
        cout << "couldn't initiate glew" << endl;


    std::cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << endl;

    TriangleMesh* tMesh = new TriangleMesh();
    tMesh->LoadFromFile("Leather_Chair.eml");

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    GLuint shader = crearShader("vertex.glsl", "fragment.glsl");
    FullScreenQuad* quad = new FullScreenQuad();
    //ofstream output("image.ppm");

    //output << "P3\n" << WIDTH << "\n" << HEIGHT << "\n" << "255" << endl;
    Sphere sphere(vec3(WIDTH/2.0f, 150.0f, kInfinity), 200.0f);
    Plane plane(5, 5);

    vec3 white = vec3(1, 1, 1);
    vec3 red = vec3(1, 0, 0);
    vec3 green = vec3(0, 1, 0);
    vec3 blue = vec3(0,0,1);
    vec3 custom = vec3(1.5, 1, 0.5);

    vec3 threads[8];
    threads[0] = vec3(1.0, 0.0, 0.0);
    threads[1] = vec3(0.0, 1.0, 0.0);
    threads[2] = vec3(0.0, 0.0, 1.0);
    threads[3] = vec3(1.0, 1.0, 0.0);
    threads[4] = vec3(1.0, 0.0, 1.0);
    threads[5] = vec3(1.0, 1.0, 1.0);
    threads[6] = vec3(0.0, 1.0, 1.0);
    threads[7] = vec3(0.5, 0.5, 1.0);
    //glm::mat4 view = glm::lookAt(glm::vec3(0,0,0), glm::vec3(0,0,-1), glm::vec3(0,1,0));

    cout << "Starting Ray casting..." << endl;

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, &out);
    glBindTexture(GL_TEXTURE_2D, 0);

    float counter = 0.0;

    glUseProgram(shader);
    sphere.setPosition(vec3(WIDTH*0.5, 0.0, 0.0f));
    glfwSwapInterval(1);

    glm::mat4 model = glm::mat4();
    model = glm::translate(model, glm::vec3(256.0, -(300.0), 1000.0));
    model = glm::scale(model, glm::vec3(250.0f));
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0,1,0));

    glm::mat4 proj = glm::perspective(glm::radians(70.0f), (float)WIDTH/(float)HEIGHT, 0.1f, 3000.0f);

    //model = proj * model;
    for(;;) {

        if(glfwWindowShouldClose(window)) break;
        counter += 0.1;

        float last = 0.0, current = 0.0, delta = 0.0;

        last = glfwGetTime();

        #pragma omp parallel for
        for(int i = 0; i < HEIGHT * WIDTH; i++) {
            int y = i % HEIGHT;
            int x = i / HEIGHT;
            out[y][x] = vec3(0.0, 0.0, 0.0);
            float t = 1000;
            Ray ray(vec3(x,(y-HEIGHT*0.5),0), vec3(0, 0, 1));
            DepthBuffer[y][x] = kInfinity;

            glm::vec3 glm_From = glm::vec3(ray.m_From.x, ray.m_From.y, ray.m_From.z);
            glm::vec3 glm_To = glm::vec3(ray.m_To.x, ray.m_To.y, ray.m_To.z);

            glm::vec3 Light = glm::vec3(sin(counter*0.2)*WIDTH + WIDTH*0.5, HEIGHT*0.25, -400.0);

            if(sphere.intersect(ray, t) != 0.0) {
                vec3 ti = ray.m_From+ ray.m_To*t;
                float depth = glm::length(glm_From - glm::vec3(ti.x, ti.y, ti.z));

                if(depth < DepthBuffer[y][x])
                    DepthBuffer[y][x] = depth;

                vec3 normal = normalize(sphere.getNormal(ti));
                vec3 LightDir = normalize(vec3(Light.x, Light.y, Light.z)-ti);
                float diff = max(dot(normal, LightDir), 0.0f)*0.1;
                vec3 reflectDir = reflect(vec3(-1.0, -1.0, -1.0)*LightDir, normal);
                float spec = pow(max(dot(ray.m_To, reflectDir), 0.0), 128);
                out[y][x] = normal;//vec3( custom * vec3(diff, diff, diff) + vec3(0.005, 0.005, 0.005) * custom + vec3(spec, spec, spec));
            }

            float near = kInfinity;
            uint32_t index = 0;

            glm::mat4 view = glm::lookAt(glm_From,
                                         glm::normalize(glm_To),
                                         glm::vec3(0.0, 1.0, 0.0));


            if(tMesh->Intersect(glm_From, glm_To, near, index, model, proj, view))
            {
                glm::vec3 ti = glm_From + glm_To*t;
                float depth = glm::length(glm_From - ti);
                if(depth < DepthBuffer[y][x]){
                    DepthBuffer[y][x] = depth;
                glm::vec3 LightDir = glm::normalize(Light-ti);
                glm::vec3 normal = tMesh->getNormal(index);
                float diff = max(glm::dot(normal, LightDir), 0.0f)*0.1;
                glm::vec3 reflectDir = reflect(-LightDir, normal);
                float spec = pow(max(dot(glm_To, reflectDir), 0.0), 128);//vec3( custom * vec3(diff, diff, diff) + vec3(0.005, 0.005, 0.005) * custom + vec3(spec, spec, spec)); //
                float pdepth = pow(depth, 15.0);
                glm::vec3 texcoords = glm::vec3(tMesh->getTexCoords(index), 0.0);

                out[y][x] = from_glm(texcoords);
                }
            }

            const auto threadId = omp_get_thread_num();
            //out[y][x] = threads[threadId]*0.1 + out[y][x];
        }

        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT, GL_RGB, GL_FLOAT, &out);
        glUniform1i(glGetUniformLocation(shader, "texture0"), 0);
        quad->Render();

        current = glfwGetTime();
        delta = current - last;
        std::ostringstream stm ;
        stm << 1.0/delta ;

        glfwSetWindowTitle(window, stm.str().c_str());

        glfwSwapBuffers(window);

    }
    try {
        #pragma omp simd
        for(int i = 0; i < HEIGHT * WIDTH; i++) {
            int y = i % HEIGHT;
            int x = i / HEIGHT;
            c[y][x].x = (uint8_t)clamp(out[y][x].x*255, 0.0, 255);
            c[y][x].y = (uint8_t)clamp(out[y][x].y*255, 0.0, 255);
            c[y][x].z = (uint8_t)clamp(out[y][x].z*255, 0.0, 255);
        }
        stbi_flip_vertically_on_write(1);
        stbi_write_png("image.png", WIDTH, HEIGHT, 3, &c, sizeof(uint8_t)*3*WIDTH);
    } catch(exception e) {
        cout << e.what();
    }

    glfwTerminate();
    delete quad;
    return 0;
}
