/* 
配置片段着色器，使颜色随时间变化
uniform变量

 */
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include<math.h>
using namespace std;

float vertices[]={
  -0.3,0.4,0,0,
  0.3, 0.4,0.0,
  0.0,-0.4,0.0
};
float colorvertices[] = {
    // 位置              // 颜色
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
};
const char* vertexshadersource="#version 330 core\n"
"layout (location=0) in vec3 apos;\n"
"void main()\n"
"{ gl_Position = vec4(apos, 1.0); }\0";
//随时间变化的片段着色器
const char* timefragmentshadersource="#version 330 core\n"
" out vec4 fragcolor;\n"
"uniform vec4 timecolor;\n"
"void main()\n"
"{ fragcolor = timecolor; }\0";

const char* vertexshadersource2="#version 330 core\n"
"layout (location=0) in vec3 apos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{ gl_Position = vec4(apos, 1.0); \n"
"ourColor=aColor;\n"
"}\0";

//不同颜色的片段着色器
const char* colorsfragmentshadersource="#version 330 core\n"
"in vec3 ourColor;\n"
" out vec4 fragcolor;\n"
"void main()\n"
"{ fragcolor = vec4(ourColor, 1.0); }\0";

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

int main(){
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
  glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window=glfwCreateWindow(600,600,"hello opengl",NULL,NULL);
  glfwMakeContextCurrent(window);
  gladLoadGL(); 
  glViewport(0,0,600,600);
  glEnable(GL_PROGRAM_POINT_SIZE);
  
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  //顶点着色器
  unsigned int vertexshader,vertexshader2;
  vertexshader=glCreateShader(GL_VERTEX_SHADER);
  vertexshader2=glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexshader,1,&vertexshadersource,NULL);
  glShaderSource(vertexshader2,1,&vertexshadersource2,NULL);
  glCompileShader(vertexshader);
  glCompileShader(vertexshader2);
  //片段着色器
  unsigned int timefragmentshader,colorsfragmentshader;
  timefragmentshader=glCreateShader(GL_FRAGMENT_SHADER);
  colorsfragmentshader=glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(timefragmentshader, 1, &timefragmentshadersource, NULL);
  glShaderSource(colorsfragmentshader, 1, &colorsfragmentshadersource, NULL);
  glCompileShader(timefragmentshader);
  glCompileShader(colorsfragmentshader);
  //着色器程序
  unsigned int shaderprogram,shaderprogram2;
  shaderprogram=glCreateProgram(); //创建着色器
  shaderprogram2=glCreateProgram(); //创建着色器
  glAttachShader(shaderprogram,vertexshader); //将顶点着色器附加到着色器
  glAttachShader(shaderprogram2,vertexshader2); //将顶点着色器附加到着色器
  glAttachShader(shaderprogram,timefragmentshader); //将片段着色器附加到着色器
  glAttachShader(shaderprogram2,colorsfragmentshader); //将片段着色器附加到着色器
  glLinkProgram(shaderprogram); //链接
  glLinkProgram(shaderprogram2); //链接

  glDeleteShader(vertexshader); //删除顶点着色器
  glDeleteShader(timefragmentshader); //删除片段着色器
  glDeleteShader(vertexshader2); //删除顶点着色器
  glDeleteShader(colorsfragmentshader); //删除片段着色器

  unsigned int VBO[2],VAO[2];
  glGenBuffers(2,VBO);
  glGenVertexArrays(2,VAO);
  glBindVertexArray(VAO[0]);
  glBindBuffer(GL_ARRAY_BUFFER,VBO[0]);
  glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float), (void*)0);//设置顶点属性指针
  glEnableVertexAttribArray(0);

  glBindVertexArray(VAO[1]);
  glBindBuffer(GL_ARRAY_BUFFER,VBO[1]);
  glBufferData(GL_ARRAY_BUFFER,sizeof(colorvertices),colorvertices,GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float), (void*)0);//设置顶点属性指针
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float), (void*)(3* sizeof(float)));//设置顶点属性指针
  glEnableVertexAttribArray(1);

  glUseProgram(shaderprogram2);

  while(!glfwWindowShouldClose(window))
  {
    // 输入
    processInput(window);

    // 渲染
    // 清除颜色缓冲
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // 记得激活着色器
    //glUseProgram(shaderprogram);

    // 更新uniform颜色
    /* float timevalue=glfwGetTime();
    float redvalue=sin(timevalue)/2.0+0.5;
    float greevalue=sin(timevalue+1.0)/2.0+0.5;
    float bluevalue=sin(timevalue+2.0)/2.0+0.5;
    int vertexcolorlocation=glGetUniformLocation(shaderprogram, "timecolor");
    glUseProgram(shaderprogram);
    glUniform4f(vertexcolorlocation,redvalue,greevalue,bluevalue,1.0);
 */

    // 绘制三角形
    glBindVertexArray(VAO[1]);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // 交换缓冲并查询IO事件
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glDeleteVertexArrays(2,VAO);
  glDeleteBuffers(2,VBO);
  glDeleteProgram(shaderprogram);
  glDeleteProgram(shaderprogram2);
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