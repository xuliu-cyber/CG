#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<tool/camera.h>
#include<tool/shader.h>
#define STB_IMAGE_IMPLEMENTATION
#include<tool/stb_image.h>

#include<iostream>
std::string Shader::dirName;
using namespace std;
using namespace glm;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float deltaTime = 0.0; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

float lastX=SCR_WIDTH/2.0; //上一帧的鼠标横坐标
float lastY=SCR_HEIGHT/2.0;
bool firstmove=true;
// camera value
vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

//顶点坐标
float vertices[]={
    0.6,-1.5,0.0,0.0,0.0,
    0.6, 1.5,0.0,1.0,0.0,
    -0.8,0.0,0.0,0.5,1.0,
    0.0, 0.0,1.5,0.5,0.5
};
//索引
unsigned int indexs[]={
    0,1,2,
    0,1,3,
    0,2,3,
    1,2,3,
};
//不同几何体的位置
vec3 cubePositions[] = {
    vec3( 0.0f,  0.0f,  0.0f), 
    vec3( 2.0f,  5.0f, -15.0f), 
    vec3(-1.5f, -2.2f, -2.5f),  
    vec3(-3.8f, -2.0f, -12.3f),  
    vec3( 2.4f, -0.4f, -3.5f),  
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height); //分辨率动态调整
void processInput(GLFWwindow *window); //处理键盘输入
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset); //处理鼠标滚轮
void mouse_callback(GLFWwindow* window, double xpos, double ypos); //处理鼠标移动

//加载相机,放在全局因为回调函数使用
Camera mycamera(cameraPos,cameraUp);

int main(int argc, char *argv[]){
    Shader::dirName = argv[1];
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    //设置窗口的一些属性
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //使光标不可见
    glfwSetScrollCallback(window, scroll_callback); //设置鼠标滚轮效果
    glfwSetCursorPosCallback(window, mouse_callback); //设置鼠标移动效果

    //load glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //深度测试
    glEnable(GL_DEPTH_TEST);

    //加载着色器
    cout<<"1";
    Shader myshader("./shader/vertex.glsl","./shader/fragment.glsl");

    //数据绑定
    unsigned int VBO,VAO,EBO;
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);   
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE,5*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indexs),indexs,GL_STATIC_DRAW);
     
    //纹理生成
    unsigned int texture1,texture2;
    //纹理1
    glGenTextures(1,&texture1);
    glBindTexture(GL_TEXTURE_2D,texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load("./static/texture/container.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); //注意有些图片为RGBA格式
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    
    //纹理2
    glGenTextures(1,&texture2);
    glBindTexture(GL_TEXTURE_2D,texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    data = stbi_load("./static/texture/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //注意有些图片为RGBA格式
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
   
    //向着色器传递uniform的纹理变量，注意要先use
    myshader.use();
    myshader.setInt("texture1", 0);
    myshader.setInt("texture2", 1);

    //开始渲染
    while (!glfwWindowShouldClose(window))
    {
        //处理输入
        processInput(window);
        //更新时间
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清空深度信息
        //绑定纹理
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        //不需要绑定VAO了
        //glBindVertexArray(VAO);
        myshader.use();

        //传递MVP矩阵，使用camera类
        for(int i=0;i<5;++i){
            mat4 model         = mat4(1.0f); // make sure to initialize matrix to identity matrix first
            mat4 view          = mycamera.GetViewMatrix();
            mat4 projection    = mat4(1.0f);

            model = translate(model, cubePositions[i]);
            float angle=radians(i*20.0)+(float)glfwGetTime();
            model = rotate(model, angle , vec3(1.0f, 0.3f, 0.5f));
            projection=perspective(radians(mycamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            myshader.setMat4("view", view);
            myshader.setMat4("projection", projection);
            myshader.setMat4("model", model);

            glDrawElements(GL_TRIANGLES,size(indexs),GL_UNSIGNED_INT,0);//索引缓冲对象绘制
        }
        


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();    
    return 0;
}
// 窗口变动监听
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// 键盘输入监听
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    // 相机按键控制
    // 相机移动
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        mycamera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        mycamera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        mycamera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        mycamera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

// 鼠标移动监听
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if(firstmove){
        lastX=xpos;
        lastY=ypos;
        firstmove=false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    mycamera.ProcessMouseMovement(xoffset, yoffset);
}
//滚轮监听
void scroll_callback(GLFWwindow *window, double x, double y)
{
    mycamera.ProcessMouseScroll(y);
}