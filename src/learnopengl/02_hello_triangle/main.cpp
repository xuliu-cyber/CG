/* #include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);"
    "gl_PointSize = 20.0f;"
    "}\n";

const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{"
    "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);"
    "}";

int main()
{
  glfwInit();
  // 设置主要和次要版本
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // 创建窗口对象
  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
  // 设置视口
  glViewport(0, 0, 800, 600);
  glEnable(GL_PROGRAM_POINT_SIZE);

  // 注册窗口变化监听
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // 定义顶点数组
  float vertices[] = {
      0.5f, 0.5f, 0.0f,   // 右上角
      0.5f, -0.5f, 0.0f,  // 右下角
      -0.5f, -0.5f, 0.0f, // 左下角
      -0.5f, 0.5f, 0.0f   // 左上角
  };
  unsigned int indexes[] = {
      0, 1, 3, // 第一个三角形
      1, 2, 3  // 第二个三角形
  };

  // 创建缓冲对象
  unsigned int VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  // 绑定VAO缓冲对象
  glBindVertexArray(VAO);

  // 绑定VBO缓对象
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // 填充VBO数据
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // 绑定EBO对象
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  // 填充EBO数据
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);

  // 设置顶点属性指针
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);

  // 创建顶点和片段着色器
  unsigned int vertexShader, fragmentShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

  // 附加着色器代码
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  // 检测是否编译成功
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR:SHADER::VERTEX::COMPILATION_FAILED\n"
              << std::endl;
  }

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR:SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << std::endl;
  }

  // 创建程序对象
  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();
  // 将着色器附加到程序对象上
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);

  // 链接
  glLinkProgram(shaderProgram);
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success)
  {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR:SHADER_PROGRAM_LINK_FAILED" << std::endl;
  }

  // 使用着色器程序
  glUseProgram(shaderProgram);

  // 删除着色器
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // 设置线框绘制模式
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (!glfwWindowShouldClose(window))
  {
    processInput(window);

    // 渲染指令
    // ...
    glClearColor(25.0 / 255.0, 25.0 / 255.0, 25.0 / 255.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);// 不需要每次都绑定，对于当前程序其实只需要绑定一次就可以了
    // glDrawArrays(GL_POINTS, 0, 6);
    // glDrawArrays(GL_LINE_LOOP, 0, 6);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteProgram(shaderProgram);

  glfwTerminate();
  return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, true);
  }
} */

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>

using namespace std;

float first_vertices[]={
  // first triangle
  -0.9f, -0.5f, 0.0f,  // left 
  -0.0f, -0.5f, 0.0f,  // right
  -0.45f, 0.5f, 0.0f,  // top 
};
float second_vertices[]={
  // second triangle
    0.0f, -0.5f, 0.0f,  // left
    0.9f, -0.5f, 0.0f,  // right
    0.45f, 0.5f, 0.0f   // top 
};
unsigned int indices[]={
  0,1,2,
  1,2,3
};
//顶点着色器
const char *vertexShaderSource="#version 330 core\n"
"layout (location=0) in vec3 apos;\n"
"void main()\n"
"{ gl_Position = vec4(apos.x, apos.y, apos.z, 1.0); }\0";

//片段着色器
const char *fragmentshadersource="#version 330 core\n"
"out vec4 fragcolor;\n"
"void main()\n"
"{ fragcolor=vec4(0.0, 1.0, 0.0, 0.5); }\0";

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

int main(){
  glfwInit();
  // 设置主要和次要版本
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // 创建窗口对象
  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
  // 设置视口
  glViewport(0, 0, 800, 600);
  glEnable(GL_PROGRAM_POINT_SIZE);

  // 注册窗口变化监听
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  
  unsigned int vertexshader;
  vertexshader=glCreateShader(GL_VERTEX_SHADER);//创建顶点着色器
  glShaderSource(vertexshader,1,&vertexShaderSource,NULL); //将顶点着色器源码加到顶点着色器
  glCompileShader(vertexshader); //编译顶点着色器源码
  /* 
  检测编译时错误 略。。
   */
  unsigned int fragmentshader;
  fragmentshader=glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentshader, 1, &fragmentshadersource, NULL);
  glCompileShader(fragmentshader);

  unsigned int shaderprogram;
  shaderprogram=glCreateProgram(); //创建着色器
  glAttachShader(shaderprogram,vertexshader); //将顶点着色器附加到着色器
  glAttachShader(shaderprogram,fragmentshader); //将片段着色器附加到着色器
  glLinkProgram(shaderprogram); //链接
  glUseProgram(shaderprogram); 

  glDeleteShader(vertexshader); //删除顶点着色器
  glDeleteShader(fragmentshader); //删除片段着色器

  unsigned int VBO[2];
  unsigned int VAO[2];
  glGenBuffers(2,VBO); //生成顶点缓冲对象
  glGenVertexArrays(2, VAO); //生成顶点数组对象
  glBindVertexArray(VAO[0]); //绑定VAO
  glBindBuffer(GL_ARRAY_BUFFER,VBO[0]); //将VBO绑定到GL_ARRAY_BUFFER
  glBufferData(GL_ARRAY_BUFFER,sizeof(first_vertices),first_vertices,GL_STATIC_DRAW); //将顶点数据复制到缓冲的内存中
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float), (void*)0);//设置顶点属性指针
  glEnableVertexAttribArray(0);

  glBindVertexArray(VAO[1]); //绑定VAO
  glBindBuffer(GL_ARRAY_BUFFER,VBO[1]); //将VBO绑定到GL_ARRAY_BUFFER
  glBufferData(GL_ARRAY_BUFFER,sizeof(second_vertices),second_vertices,GL_STATIC_DRAW); //将顶点数据复制到缓冲的内存中
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float), (void*)0);//设置顶点属性指针
  glEnableVertexAttribArray(0);
  unsigned int EBO;
  glGenBuffers(1,&EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);
  
  glBindVertexArray(0); //解绑VAO，之后绘制物体时，只需要绑定对应的VAO就行了


  // 设置线框绘制模式
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  
  while (!glfwWindowShouldClose(window))
  {
    processInput(window);
    glClearColor(0.2,0.3,0.3,1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderprogram);
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    glBindVertexArray(VAO[1]);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    //glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);//索引缓冲对象绘制
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(2,VAO);
  glDeleteBuffers(2,VBO);
  glDeleteProgram(shaderprogram);
  glfwTerminate();
  return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, true);
  }
}