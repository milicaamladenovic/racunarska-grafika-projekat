#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>
#include <GL/gl.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

unsigned int loadTexture(const char *path);

void renderScene(const Shader &shader);

unsigned int loadCubemap(vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
bool blinn = false;
bool blinnKeyPressed = false;

// camera

Camera camera(glm::vec3(0.0f, 20.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct PointLIght{
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
unsigned int planeVAO;

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    stbi_set_flip_vertically_on_load(false);


    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    //blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile shaders
    // -------------------------
    Shader ourShader("resources/shaders/2.model_lighting.vs", "resources/shaders/2.model_lighting.fs");
    Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");


    // load models
    // -----------
    Model _cat("resources/objects/cat/cat.obj");
    _cat.SetShaderTextureNamePrefix("material.");
    Model _table("resources/objects/table/picnic_table.obj");
    _table.SetShaderTextureNamePrefix("material.");
    Model _gardenFlowers("resources/objects/gardenFlowers/scene.gltf");
    _gardenFlowers.SetShaderTextureNamePrefix("material.");
    Model _basket("resources/objects/basket/10431_Wicker_Basket_v1_L3.obj");
    _basket.SetShaderTextureNamePrefix("material.");
    Model _basketballHoop("resources/objects/basketballHoop/scene.gltf");
    _basketballHoop.SetShaderTextureNamePrefix("material.");
    Model _ball("resources/objects/ball/basketballball.obj");
    _ball.SetShaderTextureNamePrefix("material.");
    Model _dog("resources/objects/dog/dog.gltf");
    _dog.SetShaderTextureNamePrefix("material.");
    Model _tart("resources/objects/tart/Scaniverse.obj");
    _tart.SetShaderTextureNamePrefix("material.");
    Model _flowers("resources/objects/flowers/flowers.gltf");
    _flowers.SetShaderTextureNamePrefix("material.");
    Model _wine("resources/objects/wine/uploads_files_3520947_Wine.obj");
    _wine.SetShaderTextureNamePrefix("material.");
    Model _butterfly("resources/objects/butterfly/scene.gltf");
    _butterfly.SetShaderTextureNamePrefix("material.");


    PointLIght pointLight;

    pointLight.ambient = glm::vec3(0.9, 0.9, 0.9);
    pointLight.diffuse = glm::vec3(0.55, 0.55, 0.55);
    pointLight.specular = glm::vec3(0.5, 0.5, 0.5);

    pointLight.constant = 1.0f;
    pointLight.linear = 0.0f;
    pointLight.quadratic = 0.0f;


    //skybox
    float skyboxVertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };

    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    vector<std::string> faces
            {
                    FileSystem::getPath("resources/textures/skyboxgrass/posx.jpg"),
                    FileSystem::getPath("resources/textures/skyboxgrass/negx.jpg"),
                    FileSystem::getPath("resources/textures/skyboxgrass/posy.jpg"),
                    FileSystem::getPath("resources/textures/skyboxgrass/negy.jpg"),
                    FileSystem::getPath("resources/textures/skyboxgrass/posz.jpg"),
                    FileSystem::getPath("resources/textures/skyboxgrass/negz.jpg")
            };

    unsigned int cubemapTexture = loadCubemap(faces);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    glEnable(GL_BLEND);


    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    float rotationAngle=0.0f;
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        float rotationSpeed=2.0f;
        rotationAngle += rotationSpeed * deltaTime;
        // input
        // -----
        processInput(window);


        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // reset viewport
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        // don't forget to enable shader before setting uniforms
        ourShader.use();

        pointLight.position=glm::vec3(15.0f*sin(2*currentFrame), 40.0f, -100.0f*cos(2*glfwGetTime()));
        ourShader.setVec3("pointLight.position", pointLight.position);
        ourShader.setVec3("pointLight.ambient", pointLight.ambient);
        ourShader.setVec3("pointLight.diffuse", pointLight.diffuse);
        ourShader.setVec3("pointLight.specular", pointLight.specular);
        ourShader.setFloat("pointLight.constant", pointLight.constant);
        ourShader.setFloat("pointLight.linear", pointLight.linear);
        ourShader.setFloat("pointLight.quadratic", pointLight.quadratic);
        ourShader.setVec3("viewPosition", camera.Position);
        ourShader.setFloat("material.shininess", 32.0f);
        ourShader.setInt("blinn", blinn);

//        std::cout << (blinn ? "Blinn-Phong" : "Phong") << std::endl;

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 300.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        //Face culling
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // render the loaded model
        //cat
        glm::mat4 catMat = glm::mat4(1.0f);
        catMat = glm::translate(catMat,glm::vec3 (-30.6134f, 0.0f, -81.4441f));
        catMat = glm::scale(catMat, glm::vec3(0.3f));
        catMat = glm::rotate(catMat, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f));;
        catMat=glm::rotate(catMat, glm::radians(-145.0f), glm::vec3 (0.0f, 0.0f, 1.0f));
        ourShader.setMat4("model", catMat);
        _cat.Draw(ourShader);


        //table
        glm::mat4 tableMat = glm::mat4(1.0f);
        tableMat = glm::translate(tableMat,glm::vec3 (15.0f, 0.0f, -100.0f));
        tableMat = glm::scale(tableMat, glm::vec3(19.0f));
        tableMat=glm::rotate(tableMat, glm::radians(30.0f), glm::vec3 (.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", tableMat);
        _table.Draw(ourShader);

        //basket
        glm::mat4 basketMat = glm::mat4(1.0f);
        basketMat = glm::translate(basketMat,glm::vec3 (45.0f, 0.0f, -93.0f));
        basketMat=glm::rotate(basketMat, glm::radians(-90.0f), glm::vec3 (1.0f, 0.0f, 0.0f));
        basketMat=glm::rotate(basketMat, glm::radians(-15.0f), glm::vec3 (0.0f, 0.0f, 1.0f));
        basketMat=glm::scale(basketMat,glm::vec3 (0.25f));
        ourShader.setMat4("model", basketMat);
        _basket.Draw(ourShader);


        //dog
        glm::mat4 dogMat = glm::mat4(1.0f);
        dogMat = glm::translate(dogMat,glm::vec3 (-80.0f, .0f, -150.0f));
        dogMat=glm::scale(dogMat,glm::vec3 (3.0f));
        ourShader.setMat4("model", dogMat);
        _dog.Draw(ourShader);

        //ball
        float maxHeight = 10.0f; // Maksimalna visina
        float speed = 3.0f; // Brzina kretanja
        float heightOffset = 2.0f; // Offset visine
        float objectHeight = maxHeight * sin(speed * currentFrame) + heightOffset;
        glm::mat4 ballMat = glm::mat4(1.0f);
        ballMat = glm::translate(ballMat,glm::vec3 (-100.0f, .0f, -60.0f));
        ballMat = glm::translate(ballMat, glm::vec3(0.0f, objectHeight, 0.0f));
        ballMat=glm::scale(ballMat,glm::vec3 (0.36f));
        ourShader.setMat4("model", ballMat);
        _ball.Draw(ourShader);


        //tart
        glm::mat4 tartMat = glm::mat4(1.0f);
        tartMat = glm::translate(tartMat,glm::vec3 (17.5f, 14.0f, -93.0f));
        tartMat=glm::scale(tartMat,glm::vec3 (20.5f));
        ourShader.setMat4("model", tartMat);
        _tart.Draw(ourShader);

        //flowers bouquet
        glm::mat4 flowersMat = glm::mat4(1.0f);
        flowersMat = glm::translate(flowersMat,glm::vec3 (15.5f, 15.740f, -105.0f));
        flowersMat=glm::rotate(flowersMat, glm::radians(110.0f), glm::vec3 (1.0f, 1.0f, 0.0f));
        flowersMat=glm::scale(flowersMat,glm::vec3 (11.5f));
        ourShader.setMat4("model", flowersMat);
        _flowers.Draw(ourShader);


        //wine
        glm::mat4 wineMat = glm::mat4(1.0f);
        wineMat = glm::translate(wineMat,glm::vec3 (24.3f, 13.940f, -87.0f));
        wineMat=glm::scale(wineMat,glm::vec3 (15.3f));
        ourShader.setMat4("model", wineMat);
        _wine.Draw(ourShader);


        //garden flowers
        std::vector<glm::vec4> FlowersPos;
        FlowersPos.push_back(glm::vec4(20.0f, 0.0f, -71.0f, 0.4f));
        FlowersPos.push_back(glm::vec4(-35.0f, .0f, -150.0f,0.5f));
        FlowersPos.push_back(glm::vec4(-170.0f, .0f, -70.0f, 0.7f));
        FlowersPos.push_back(glm::vec4(-20.0f, .0f, -50.0f, 0.3f));
        FlowersPos.push_back(glm::vec4(-70.0f, .0f, -100.0f, 0.5f));
        FlowersPos.push_back(glm::vec4(-50, 0.0f, -20.0f, 0.28f));
        FlowersPos.push_back(glm::vec4(-150, 0.0f, -150.0f, 0.6f));


        for(int i=0;i<FlowersPos.size();i++){
            glm::mat4 gardenFlowersMat = glm::mat4(1.0f);
            gardenFlowersMat = glm::translate(gardenFlowersMat,glm::vec3 (FlowersPos[i].x, FlowersPos[i].y, FlowersPos[i].z));
            gardenFlowersMat=glm::scale(gardenFlowersMat, glm::vec3(FlowersPos[i].w));
            ourShader.setMat4("model", gardenFlowersMat);
            _gardenFlowers.Draw(ourShader);
        }


        //butterfly
        glm::mat4 butterflyMat = glm::mat4(1.0f);
        butterflyMat = glm::translate(butterflyMat,glm::vec3 (-80.0f, 15.f, -150.0f));
        butterflyMat = glm::translate(butterflyMat,glm::vec3 (100*cos(rotationAngle), 0.0f, 100*sin(rotationAngle)));
        butterflyMat=glm::scale(butterflyMat, glm::vec3(5.0f));
        float maxAngle=45.0f;
        float speedB = 25.0f;
        float angle = maxAngle * sin(speedB * currentFrame);
        butterflyMat=glm::rotate(butterflyMat, glm::radians(100.0f), glm::vec3 (0.0f, 1.0f, 0.0f));
        butterflyMat=glm::rotate(butterflyMat, glm::radians(angle), glm::vec3 (0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", butterflyMat);
        _butterfly.Draw(ourShader);


        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default




        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------

    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);

//
//    // optional: de-allocate all resources once they've outlived their purpose:
//    // ------------------------------------------------------------------------
//    glDeleteVertexArrays(1, &planeVAO);
//    glDeleteBuffers(1, &planeVBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime*10.5);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime*10.5);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime*10.5);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime*10.5);

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed)
    {
        blinn = !blinn;
        blinnKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
    {
        blinnKeyPressed = false;
    }

}


void renderScene(const Shader &shader)
{
    // floor
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;


    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}


// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}