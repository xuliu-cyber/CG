#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<tool/camera.h>
#include<tool/shader.h>
#define STB_IMAGE_IMPLEMENTATION
#include<tool/stb_image.h>
#include<geometry/Sphere.h>

#include<iostream>
#include<fstream>
std::string Shader::dirName;
using namespace std;
using namespace glm;

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1000;
const GLfloat PI= 3.14159265358979323846f;
//将球横纵划分成50*50的网格
const int Y_SEGMENTS = 50;
const int X_SEGMENTS = 50;
float deltaTime = 0.0; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

float lastX=SCR_WIDTH/2.0; //上一帧的鼠标横坐标
float lastY=SCR_HEIGHT/2.0;
bool firstmove=true;
bool flashon=false;

vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

Camera camera(cameraPos,cameraUp);

void framebuffer_size_callback(GLFWwindow* window, int width, int height); //分辨率动态调整
void processInput(GLFWwindow *window); //处理键盘输入
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset); //处理鼠标滚轮
void mouse_callback(GLFWwindow* window, double xpos, double ypos); //处理鼠标移动
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) ; //鼠标点击
unsigned int loadTexture(const char *path);

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
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    //load glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    //深度测试
    glEnable(GL_DEPTH_TEST);

    //加载着色器
    Shader objshader("./shader/obj_vert.glsl","./shader/obj_frag.glsl");
    Shader lightshader("./shader/light_vert.glsl","./shader/light_frag.glsl");
    Shader cubeshader("./shader/cube_vert.glsl","./shader/cube_frag.glsl");

    //cube data
    float vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
    
    //love data
    std::vector<float> loveVertices;
	std::vector<int> loveIndices;

    ifstream fin("src\\summary\\basic_light\\love.txt");
    for(int i=0;i<272;++i){
        char line_1[256];
        stringstream ss;
        fin.getline(line_1,sizeof(line_1));
        string line_2 = line_1;
        float v;
        ss << line_2;
        while(ss >> v) {
            if(i<92)
                loveVertices.push_back(v); 
            else
                loveIndices.push_back(int(v));
        }
    }
    fin.close();

    // sphere data
	Sphere sphere;
    //love
    unsigned int love_VBO, love_VAO;
	glGenVertexArrays(1, &love_VAO);
	glGenBuffers(1, &love_VBO);
	//生成并绑定球体的VAO和VBO
	glBindVertexArray(love_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, love_VBO);
	//将顶点数据绑定至当前默认的缓冲中
	glBufferData(GL_ARRAY_BUFFER, loveVertices.size() * sizeof(float), &loveVertices[0], GL_STATIC_DRAW);

	GLuint love_EBO;//EBO
	glGenBuffers(1, &love_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, love_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, loveIndices.size() * sizeof(int), &loveIndices[0], GL_STATIC_DRAW);

	//设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
    //cube 
    unsigned int cube_VBO, cube_VAO;
	glGenVertexArrays(1, &cube_VAO);
	glGenBuffers(1, &cube_VBO);
	//生成并绑定球体的VAO和VBO
	glBindVertexArray(cube_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);
	//将顶点数据绑定至当前默认的缓冲中
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //加载贴图
    unsigned int diffusemap=loadTexture("./static/texture/container.jpg");
    cubeshader.use(); 
    cubeshader.setInt("material.diffuse", 0);

    mat4 projection    = mat4(1.0f);
    projection=perspective(radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    vec3 lightcolor[3]={
        vec3(1.0,0.0,0.0),
        vec3(0.0,1.0,0.0),
        vec3(0.0,0.0,1.0),
    };
    vec3 lighttrans[3]={
        vec3(5.0,0.0,0.0),
        vec3(0.0,5.0,0.0),
        vec3(0.0,0.0,5.0),
    };
    vec3 lightrotate[3]={
        vec3(1.0,0.5,-1.5),
        vec3(-1.0,0.5,-1.5),
        vec3(0.0,1.0,0.0),
    };
    while (!glfwWindowShouldClose(window))
	{
        processInput(window);
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
		//清空颜色缓冲
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightshader.use();
		//绘制球
		
		glBindVertexArray(sphere.VAO);
        mat4 view          = camera.GetViewMatrix();
        vec3 lightposition[3];
		for(int i=0;i<3;++i){
            mat4 model         = mat4(1.0f); // make sure to initialize matrix to identity matrix first
            float angle=glfwGetTime()*(i/2.0+2.0);
            model=rotate(model, angle, lightrotate[i]);
            model=translate(model,lighttrans[i]);
            model=scale(model,vec3(0.3,0.3,0.3));
            lightposition[i]=model*vec4(0.0,0.0,0.0,1.0);
            lightshader.setMat4("view", view);
            lightshader.setMat4("projection", projection);
            lightshader.setMat4("model", model);
            lightshader.setVec3("lightcolor", lightcolor[i]);
            glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);

        }
        //绘制love
        objshader.use();
        glBindVertexArray(love_VAO);
        objshader.setMat4("view", view);
        objshader.setMat4("projection", projection);
        mat4 model         = mat4(1.0f);
        model=translate(model,vec3(0.0,-4.0,0.0));
        model=scale(model,vec3(0.1,0.1,0.1));
        objshader.setMat4("model", model);
        objshader.setVec3("viewPos",camera.Position);
        objshader.setVec3("light1.position", lightposition[0]);
        objshader.setVec3("light2.position", lightposition[1]);
        objshader.setVec3("light3.position", lightposition[2]);
        
        objshader.setVec3("light1.ambient", lightcolor[0]); // note that all light colors are set at full intensity
        objshader.setVec3("light1.diffuse", lightcolor[0]);
        objshader.setVec3("light1.specular", lightcolor[0]);
        objshader.setVec3("light2.ambient", lightcolor[1]); // note that all light colors are set at full intensity
        objshader.setVec3("light2.diffuse", lightcolor[1]);
        objshader.setVec3("light2.specular", lightcolor[1]);
        objshader.setVec3("light3.ambient", lightcolor[2]); // note that all light colors are set at full intensity
        objshader.setVec3("light3.diffuse", lightcolor[2]);
        objshader.setVec3("light3.specular", lightcolor[2]);
        // Cyan Plastic
        objshader.setVec3("material.ambient", 0.7f, 0.6f, 0.6f);
        objshader.setVec3("material.diffuse", 1.0f, 0.753f, 0.796f);
        objshader.setVec3("material.specular", 1.0f, 0.753f, 0.796f);
        objshader.setFloat("material.shininess", 64);
        //点光源衰减
        objshader.setFloat("light1.constant",  1.0f);
        objshader.setFloat("light1.linear",    0.09f);
        objshader.setFloat("light1.quadratic", 0.032f);
        objshader.setFloat("light2.constant",  1.0f);
        objshader.setFloat("light2.linear",    0.09f);
        objshader.setFloat("light2.quadratic", 0.032f);
        objshader.setFloat("light3.constant",  1.0f);
        objshader.setFloat("light3.linear",    0.09f);
        objshader.setFloat("light3.quadratic", 0.032f);

        objshader.setBool("flashlighton",flashon);
        objshader.setVec3("flashlight.position",camera.Position);
        objshader.setVec3("flashlight.direction",camera.Front);
        objshader.setFloat("flashlight.cutOff",  cos(radians(3.0f)));
        objshader.setFloat("flashlight.outerCutOff",  cos(radians(8.0f)));
        glDrawElements(GL_TRIANGLES, 180*3, GL_UNSIGNED_INT, 0);
        
        //绘制cube
        /* cubeshader.use();
        glBindVertexArray(cube_VAO);
        model  = mat4(1.0f);
        model=translate(model,vec3(0.0,0.0,0.0));
        cubeshader.setMat4("view", view);
        cubeshader.setMat4("projection", projection);
        cubeshader.setMat4("model", model);
        cubeshader.setVec3("viewPos",camera.Position);
        cubeshader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        cubeshader.setFloat("material.shininess", 64);
        cubeshader.setVec3("light.ambient", lightcolor[0]); // note that all light colors are set at full intensity
        cubeshader.setVec3("light.diffuse", lightcolor[0]);
        cubeshader.setVec3("light.specular", lightcolor[0]);      
        cubeshader.setVec3("light.position", lightposition[0]);
        glDrawArrays(GL_TRIANGLES,0,36); */
		//交换缓冲并且检查是否有触发事件(比如键盘输入、鼠标移动)

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	/*6-结束*/
	//删除VAO和VBO，EBO
	sphere.dispose();
    glDeleteVertexArrays(1, &love_VAO);
	glDeleteBuffers(1, &love_VBO);
    glDeleteBuffers(1, &love_EBO);
    glDeleteVertexArrays(1, &cube_VAO);
	glDeleteBuffers(1, &cube_VBO);
	//清理所有的资源并正确退出程序
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
        camera.ProcessKeyboard(FORWARD, deltaTime*2);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime*2);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime*2);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime*2);
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}
//滚轮监听
void scroll_callback(GLFWwindow *window, double x, double y)
{
    camera.ProcessMouseScroll(y);
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if(button == GLFW_MOUSE_BUTTON_LEFT&& action == GLFW_PRESS){
        flashon=!flashon;
    }
}
//load texture
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