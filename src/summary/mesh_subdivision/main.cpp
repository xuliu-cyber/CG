/* 
读取obj格式文件，基于loop算法进行Mesh Subdivision
使用最小二乘法进行Mesh Simplification
https://www.ri.cmu.edu/pub_files/pub2/garland_michael_1997_1/garland_michael_1997_1.pdf
 */

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<tool/camera.h>
#include<tool/shader.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include<algorithm>
#include<queue>
std::string Shader::dirName;
using namespace std;
using namespace glm;

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1000;
const GLfloat PI= 3.14159265358979323846f;

float deltaTime = 0.0; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

float lastX=SCR_WIDTH/2.0; //上一帧的鼠标横坐标
float lastY=SCR_HEIGHT/2.0;
bool firstmove=true;

vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

Camera camera(cameraPos,cameraUp);

void framebuffer_size_callback(GLFWwindow* window, int width, int height); //分辨率动态调整
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset); //处理鼠标滚轮
void mouse_callback(GLFWwindow* window, double xpos, double ypos); //处理鼠标移动
void Loop_subdivision(); //mesh细分
void mesh_simplify(); //mesh简化
void readObj(std::string filepath, vector<vec3>& vertices, vector<int>& indices);

bool subdivision=false;
bool simplify=false;

unsigned int love_VBO, love_VAO, love_EBO;//EBO;
vector<vec3> loveVertices;
vector<int> loveIndices;

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
    glfwSetKeyCallback(window, key_callback);
    //load glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    //深度测试
    glEnable(GL_DEPTH_TEST);
    Shader objshader("./shader/obj_vert.glsl","./shader/obj_frag.glsl");
    //加载obj
    //readObj("src\\summary\\ray_tracing\\models\\sphere.obj",loveVertices, loveIndices);
    readObj("src\\summary\\mesh_subdivision\\love.obj",loveVertices, loveIndices);
	glGenVertexArrays(1, &love_VAO);
	glGenBuffers(1, &love_VBO);
	//生成并绑定球体的VAO和VBO
	glBindVertexArray(love_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, love_VBO);
	//将顶点数据绑定至当前默认的缓冲中
	glBufferData(GL_ARRAY_BUFFER, loveVertices.size() * sizeof(vec3), &loveVertices[0], GL_STATIC_DRAW);

	
	glGenBuffers(1, &love_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, love_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, loveIndices.size() * sizeof(int), &loveIndices[0], GL_STATIC_DRAW);

	//设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
    
    mat4 projection    = mat4(1.0f);
    projection=perspective(radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (!glfwWindowShouldClose(window))
	{
        processInput(window);
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
		//清空颜色缓冲
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(subdivision){
            Loop_subdivision();
            subdivision=false;
        }
        if(simplify){
            mesh_simplify();
            simplify=false;
        }
        mat4 view          = camera.GetViewMatrix();
        objshader.use();
        glBindVertexArray(love_VAO);
        objshader.setMat4("view", view);
        objshader.setMat4("projection", projection);
        mat4 model         = mat4(1.0f);
        model=translate(model,vec3(0.0,0,-4.0));
        model=scale(model,vec3(0.1,0.1,0.1));
        objshader.setMat4("model", model);
        if(loveIndices.size()>0)
        glDrawElements(GL_TRIANGLES, size(loveIndices), GL_UNSIGNED_INT, 0);
        else 
        glDrawArrays(GL_TRIANGLES,0,size(loveVertices)/3);
        glfwSwapBuffers(window);
		glfwPollEvents();
    }

    glDeleteVertexArrays(1, &love_VAO);
	glDeleteBuffers(1, &love_VBO);
    glDeleteBuffers(1, &love_EBO);
    return 0;
}


// 读取 obj
void readObj(std::string filepath, vector<vec3>& vertices, vector<int>& indices) {

    // 打开文件流
    std::ifstream fin(filepath);
    vector<vec3> ori;
    std::string line;
    if (!fin.is_open()) {
        std::cout << "文件 " << filepath << " 打开失败" << std::endl;
        exit(-1);
    }

    // 按行读取
    while (std::getline(fin, line)) {
        std::istringstream sin(line);   // 以一行的数据作为 string stream 解析并且读取
        std::string type;
        float x, y, z;
        int v0, v1, v2;
        int vn0, vn1, vn2;
        int vt0, vt1, vt2;
        char slash;
        bool flag;
        // 统计斜杆数目，用不同格式读取
        int slashCnt = 0;
        int pos1=0,pos2=0;
        for (int i = 0; i < line.length(); i++) {
            if (line[i] == '/') slashCnt++;
            if(slashCnt==1&&pos1==0) pos1=i;
            if(slashCnt==2&&pos2==0) pos2=i;
        }
        
        // 读取obj文件
        sin >> type;
        if (type == "v") {
            sin >> x >> y >> z;
            ori.push_back(vec3(x,y,z));
            if(find(vertices.begin(),vertices.end(),vec3(x,y,z))==vertices.end())
            vertices.push_back(vec3(x,y,z));
        }
        if (type == "f") {
            if (slashCnt == 6) {
                //cout<<pos1<<endl<<pos2<<endl;
                if(pos2==pos1+1){
                    //cout<<" ??\n";
                    sin >> v0 >> slash  >> slash >> vn0;
                    sin >> v1 >> slash  >> slash >> vn1;
                    sin >> v2 >> slash  >> slash >> vn2;
                }
                else{
                    sin >> v0 >> slash >> vt0  >> slash >> vn0;
                    sin >> v1 >> slash  >> vt1 >> slash >> vn1;
                    sin >> v2 >> slash  >> vt2 >> slash >> vn2;
                }
            }
            else if (slashCnt == 3) {
                sin >> v0 >> slash >> vt0;
                sin >> v1 >> slash >> vt1;
                sin >> v2 >> slash >> vt2;
            }
            else {
                sin >> v0 >> v1 >> v2;
            }

            indices.push_back(std::distance(vertices.begin(), find(vertices.begin(),vertices.end(),ori[v0-1])));
            indices.push_back(std::distance(vertices.begin(), find(vertices.begin(),vertices.end(),ori[v1-1])));
            indices.push_back(std::distance(vertices.begin(), find(vertices.begin(),vertices.end(),ori[v2-1])));
        }
    }
}

// 窗口变动监听
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// 键盘输入监听
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        subdivision=true;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        simplify=true;
    }

}
//
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

//--------------------------------------------------//
//下面为细分和简化函数

//自定义比较函数myComp，重载操作符 ()
bool eq(vec3& a, vec3& b)
{
	return abs(a.x-b.x)<0.000001&&abs(a.y-b.y)<0.000001&&abs(a.z-b.z)<0.000001;	

}
//网格细分

void Loop_subdivision(){

    vector<vec3> newlovevertices;
    vector<int> newloveindeces;
    int n=loveVertices.size();
    int m=loveIndices.size()/3;
    //cout<<n<<' '<<m<<endl;
    vector<vector<int>> v_f;
    v_f.resize(n);
    vector<vector<vec3>> f_v;
    f_v.resize(m);
    for(int i=0;i<m;++i){
        v_f[loveIndices[i*3]].push_back(i);
        v_f[loveIndices[i*3+1]].push_back(i);
        v_f[loveIndices[i*3+2]].push_back(i);
    }
    //计算原顶点的新坐标
    vector<vec3> new_verteces;
    for(int v1=0;v1<n;++v1){
        vec3 newv=vec3(0);
        vec3 oldsum=vec3(0);
        for(int face_index1: v_f[v1]){
            for(int i=0;i<3;++i){
                int v2=loveIndices[3*face_index1+i];
                if(v2==v1) continue;
                oldsum+=loveVertices[v2];
            }
        }
        float u=3.0/(8*v_f[v1].size());
        if(v_f[v1].size()==3) u=3.0/16.0;
        newv=loveVertices[v1]*(float)(1.0-v_f[v1].size()*u)+u*oldsum/2.0f;
        new_verteces.push_back(newv);
    }
    
    //添加新顶点
    for(int v1=0;v1<n;++v1){
        newlovevertices.push_back(new_verteces[v1]);          
        int v1index=newlovevertices.size()-1;    
        vec3 newv1=vec3(0),newv2=vec3(0);
        for(int face_index1: v_f[v1]){
            int v2,v3,v4;
            for(int i=0;i<3;++i){ 
                if(loveIndices[3*face_index1+i]==v1){
                    v2=loveIndices[3*face_index1+(i+1)%3];
                    v3=loveIndices[3*face_index1+(i+2)%3];
                    break;
                }
            }
            //
            for(int face_index2: v_f[v2]){
                if(face_index2!=face_index1&&find(v_f[v1].begin(),v_f[v1].end(),face_index2)!=v_f[v1].end()){
                    for(int i=0;i<3;++i){
                        v4=loveIndices[3*face_index2+i];
                        if(v4!=v2&&v4!=v1&&v4!=v3)
                        break;
                    }
                    break;
                }
            }
            newv1=3.0f/8.0f*(loveVertices[v1]+loveVertices[v2]);
            newv1+=1.0f/8.0f*(loveVertices[v3]+loveVertices[v4]);
            for(int face_index2: v_f[v3]){
                if(face_index2!=face_index1&&find(v_f[v1].begin(),v_f[v1].end(),face_index2)!=v_f[v1].end()){
                    for(int i=0;i<3;++i){
                        v4=loveIndices[3*face_index2+i];
                        if(v4!=v3&&v4!=v1&&v4!=v2)
                        break;
                        
                    }
                    break;
                }
            }
            newv2=3.0f/8.0f*(loveVertices[v1]+loveVertices[v3]);
            newv2+=1.0f/8.0f*(loveVertices[v2]+loveVertices[v4]);
            vector<vec3>::iterator iter;

            newloveindeces.push_back(v1index);

            iter=std::find(newlovevertices.begin(),newlovevertices.end(),newv1);
            if(iter!=newlovevertices.end()){
                newloveindeces.push_back(std::distance(newlovevertices.begin(),iter));
            }
            else{

                newlovevertices.push_back(newv1);          

                newloveindeces.push_back(newlovevertices.size()-1);
            }

            iter=std::find(newlovevertices.begin(),newlovevertices.end(),newv2);
            if(iter!=newlovevertices.end()){
                newloveindeces.push_back(std::distance(newlovevertices.begin(),iter));
            }
            else{
           
                    newlovevertices.push_back(newv2);          
                
                newloveindeces.push_back(newlovevertices.size()-1);
            }
            f_v[face_index1].push_back(newv1);
            f_v[face_index1].push_back(newv2);
           // cout<<face_index1<<endl;
        }
        
    }
    
    //生成索引
    for(int i=0;i<m;++i){
        vec3 v1=f_v[i][0],v2,v3;
        //if(i%10==0) cout<<f_v[i].size()<<endl;
        if(eq(v1,f_v[i][1])){
            v2=f_v[i][2];
        }
        else{
            v2=f_v[i][1];
        }
        for(int j=3;j<6;j++){
            v3=f_v[i][j];
            if(!eq(v3,v1)&&!eq(v3,v2))
            break;
        }
        vector<vec3>::iterator iter=std::find(newlovevertices.begin(),newlovevertices.end(),v1);
        if(iter!=newlovevertices.end()){
            newloveindeces.push_back(std::distance(newlovevertices.begin(),iter));
        }
        else{
            cout<<"!!\n";
        }
        iter=std::find(newlovevertices.begin(),newlovevertices.end(),v2);
        if(iter!=newlovevertices.end()){
            newloveindeces.push_back(std::distance(newlovevertices.begin(),iter));
        }
        else{
            cout<<"!!\n";
        }
        iter=std::find(newlovevertices.begin(),newlovevertices.end(),v3);
        if(iter!=newlovevertices.end()){
            newloveindeces.push_back(std::distance(newlovevertices.begin(),iter));
        }
        else{
            cout<<"!!\n";
        }
    }
    loveIndices.clear();
    loveVertices.clear();
    loveVertices.assign(newlovevertices.begin(),newlovevertices.end());
    loveIndices.assign(newloveindeces.begin(),newloveindeces.end());
    cout<<loveVertices.size()<<' '<<loveIndices.size()/3<<endl;

	//生成并绑定球体的VAO和VBO
	glBindVertexArray(love_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, love_VBO);
	//将顶点数据绑定至当前默认的缓冲中
	glBufferData(GL_ARRAY_BUFFER, loveVertices.size() * sizeof(vec3), &loveVertices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, love_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, loveIndices.size() * sizeof(int), &loveIndices[0], GL_STATIC_DRAW);

	//设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

class Edge{
    public:
    int v1,v2;
    float loss;
    vec3 newv;
    bool valid;
    Edge(int _v1, int _v2, vec3 _newv, float _loss):v1(_v1),v2(_v2),newv(_newv),loss(_loss){
        valid=true;
    }
    bool operator== (const Edge &b) const   //写在里面只用一个b，但是要用const和&修饰，并且外面还要const修饰;
    {
        return v1==b.v1&&v2==b.v2;
    }
};
struct cmp{
    bool operator() (Edge* &a, Edge* &b) const   //写在里面只用一个b，但是要用const和&修饰，并且外面还要const修饰;
    {
        return a->loss > b->loss;
    }
};

mat4 Q_plane(vec3 v1, vec3 v2,vec3 v3){
    vec3 v=cross(v1-v2,v1-v3);
    float a=v[0];
    float b=v[1];
    float c=v[2];
    float d=-a*v1[0]-b*v1[1]-c*v1[2];
    return mat4(a*a,a*b,a*c,a*d,
                a*b,b*b,b*c,b*d,
                a*c,b*c,c*c,c*d,
                a*d,b*d,c*d,d*d);
    
}

void loss_and_newv(vector<vector<int>>& v_f, int v1, int v2, vec4& newv, float& loss){
    mat4 Q(0);
    int f1=-1,f2=-1;
    for(int face1:v_f[v1]){
        for(int face2:v_f[v2]){
            if(face1==face2){
                if(f1==-1) f1=face1;
                else f2=face1;
            }
        }
    }
    for(int face:v_f[v1]){
        if(face==f1||face==f2) continue;
        Q+=Q_plane(loveVertices[loveIndices[3*face]],loveVertices[loveIndices[3*face+1]],loveVertices[loveIndices[3*face+2]]);
    }
    for(int face:v_f[v2]){
        Q+=Q_plane(loveVertices[loveIndices[3*face]],loveVertices[loveIndices[3*face+1]],loveVertices[loveIndices[3*face+2]]);
    }
    mat4 Q2=Q;
    Q2[0][3]=Q2[1][3]=Q2[2][3]=0;
    Q2[3][3]=1;
    Q2=inverse(Q2); 

    newv=Q2*vec4(0.0f,0.0f,0.0f,1.0f);

    loss=dot(Q*newv,newv);
}

int Find(vector<Edge*> es, Edge& e){
    int n=es.size();
    for(int i=0;i<n;++i){
        if(es[i]->v1==e.v1&&es[i]->v2==e.v2)
        return i;
    }
    return -1;
}
//网格简化
void mesh_simplify(){
    priority_queue<Edge*, vector<Edge*>, cmp> pq;
    vector<Edge*> es;
    vector<vector<int>> v_f;
    int n=loveVertices.size(), m=loveIndices.size()/3;
    if(n<=3) return;
    v_f.resize(6*n);
    for(int i=0;i<m;++i){
        v_f[loveIndices[i*3]].push_back(i);
        v_f[loveIndices[i*3+1]].push_back(i);
        v_f[loveIndices[i*3+2]].push_back(i);
    }
    for(int i=0;i<m;++i){
        for(int j=0;j<3;++j){
            int v1=std::min(loveIndices[i*3+j%3], loveIndices[i*3+(j+1)%3]);
            int v2=std::max(loveIndices[i*3+j%3], loveIndices[i*3+(j+1)%3]);
                      
            vec4 newv=vec4(0);
            float loss=0;
            loss_and_newv(v_f,v1,v2,newv, loss);
            Edge* e=new Edge(v1,v2,newv,loss);
            if(Find(es,*e)==-1){
                es.push_back(e);
                pq.push(e);
            }
        }
    }
    int cnt=0;
    
    while(!pq.empty()){
        Edge* e=pq.top();
        pq.pop();
        if(!e->valid) continue;
        loveVertices.push_back(e->newv);
        int newvindex=loveVertices.size()-1;
        int v1=e->v1, v2=e->v2;
        e->valid=false;

        //更新面集
        vector<int>::iterator iter;
        for(int face:v_f[v1]){
            int j=0;
            for(int i=0;i<3;++i){
                if(loveIndices[3*face+i]==v2){
                    j=-1;break;
                }
                if(loveIndices[3*face+i]==v1){
                    j=i;
                }
            }
            if(j!=-1){
                loveIndices.push_back(loveIndices[3*face+(j+1)%3]);
                loveIndices.push_back(loveIndices[3*face+(j+2)%3]);
                loveIndices.push_back(newvindex);
                v_f[newvindex].push_back(loveIndices.size()/3-1);
                iter=find(v_f[loveIndices[3*face+(j+1)%3]].begin(),v_f[loveIndices[3*face+(j+1)%3]].end(),face);
                if(iter!=v_f[loveIndices[3*face+(j+1)%3]].end()) v_f[loveIndices[3*face+(j+1)%3]].erase(iter);
                v_f[loveIndices[3*face+(j+1)%3]].push_back(loveIndices.size()/3-1);
                iter=find(v_f[loveIndices[3*face+(j+2)%3]].begin(),v_f[loveIndices[3*face+(j+2)%3]].end(),face);
                if(iter!=v_f[loveIndices[3*face+(j+2)%3]].end()) v_f[loveIndices[3*face+(j+2)%3]].erase(iter);
                v_f[loveIndices[3*face+(j+2)%3]].push_back(loveIndices.size()/3-1);

            }
        }
        for(int face:v_f[v2]){
            int j=0;
            for(int i=0;i<3;++i){
                if(loveIndices[3*face+i]==v1){
                    j=-1;break;
                }
                if(loveIndices[3*face+i]==v2){
                    j=i;
                }
            }
            if(j!=-1){
                loveIndices.push_back(loveIndices[3*face+(j+1)%3]);
                loveIndices.push_back(loveIndices[3*face+(j+2)%3]);
                loveIndices.push_back(newvindex);
                v_f[newvindex].push_back(loveIndices.size()/3-1);
                iter=find(v_f[loveIndices[3*face+(j+1)%3]].begin(),v_f[loveIndices[3*face+(j+1)%3]].end(),face);
                if(iter!=v_f[loveIndices[3*face+(j+1)%3]].end()) v_f[loveIndices[3*face+(j+1)%3]].erase(iter);
                v_f[loveIndices[3*face+(j+1)%3]].push_back(loveIndices.size()/3-1);
                iter=find(v_f[loveIndices[3*face+(j+2)%3]].begin(),v_f[loveIndices[3*face+(j+2)%3]].end(),face);
                if(iter!=v_f[loveIndices[3*face+(j+2)%3]].end()) v_f[loveIndices[3*face+(j+2)%3]].erase(iter);
                v_f[loveIndices[3*face+(j+2)%3]].push_back(loveIndices.size()/3-1);
            }
        }
        for(int face1:v_f[v1]){
            for(int face2:v_f[v2]){
                if(face1==face2){
                    for(int i=0;i<3;++i)
                    if(loveIndices[face1*3+i]!=v1&&loveIndices[face1*3+i]!=v2){
                        iter=find(v_f[loveIndices[face1*3+i]].begin(),v_f[loveIndices[face1*3+i]].end(),face1);
                        if(iter!=v_f[loveIndices[face1*3+i]].end()) v_f[loveIndices[face1*3+i]].erase(iter);
                        break;
                    }
                }
            }
        }
        //更新边集
        for(Edge* edge:es){
            if(!edge->valid) continue;
            
            if(edge->v1==v1){
                
                edge->valid=false;
                vec4 newv=vec4(0);
                float loss=0;
                loss_and_newv(v_f,edge->v2,newvindex,newv, loss);
                
                Edge* E=new Edge(edge->v2, newvindex, newv, loss);
                if(Find(es,*E)==-1){
                es.push_back(E);
                pq.push(E);}
            }
            else if(edge->v2==v1){
                edge->valid=false;
                vec4 newv=vec4(0);
                float loss=0;
                loss_and_newv(v_f,edge->v1,newvindex,newv, loss);
                Edge* E=new Edge(edge->v1, newvindex, newv, loss);
                if(Find(es,*E)==-1){
                es.push_back(E);
                pq.push(E);}
            }
            else if(edge->v1==v2){
                edge->valid=false;
                vec4 newv=vec4(0);
                float loss=0;
                loss_and_newv(v_f,edge->v2,newvindex,newv, loss);
                Edge* E=new Edge(edge->v2, newvindex, newv, loss);
                if(Find(es,*E)==-1){
                es.push_back(E);
                pq.push(E);}
            }
            else if(edge->v2==v2){
                edge->valid=false;
                vec4 newv=vec4(0);
                float loss=0;
                loss_and_newv(v_f,edge->v1,newvindex,newv, loss);
                Edge* E=new Edge(edge->v1, newvindex, newv, loss);
                if(Find(es,*E)==-1){
                es.push_back(E);
                pq.push(E);}
            }
        }
        
        cnt++;
        delete(e);
        if(cnt*2>=n) break;
    }
    int Index[loveVertices.size()*3];
    int v_times[loveVertices.size()*3];
    vector<vec3> newlovev;
    vector<int> newlovei;
    int num=0;
    memset(Index,-1,sizeof(Index));
    memset(v_times,0,sizeof(v_times));
    for(Edge* e:es){
        if(!e->valid) continue;
        
        if(Index[e->v1]==-1){
            newlovev.push_back(loveVertices[e->v1]);
            Index[e->v1]=num++;
        }
        if(Index[e->v2]==-1){
            newlovev.push_back(loveVertices[e->v2]);
            Index[e->v2]=num++;
        }
    }
    m=loveIndices.size()/3;
    for(int i=0;i<m;++i){
        if(Index[loveIndices[3*i]]!=-1&&Index[loveIndices[3*i+1]]!=-1&&Index[loveIndices[3*i+2]]!=-1)
        {
            newlovei.push_back(Index[loveIndices[3*i]]);
            newlovei.push_back(Index[loveIndices[3*i+1]]);
            newlovei.push_back(Index[loveIndices[3*i+2]]);
        }
    }
    while(!pq.empty()){
        Edge* e=pq.top();
        pq.pop();
        delete(e);
    }
    loveVertices=newlovev;
    loveIndices=newlovei;
    glBindVertexArray(love_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, love_VBO);
	//将顶点数据绑定至当前默认的缓冲中
	glBufferData(GL_ARRAY_BUFFER, loveVertices.size() * sizeof(vec3), &loveVertices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, love_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, loveIndices.size() * sizeof(int), &loveIndices[0], GL_STATIC_DRAW);

	//设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

}   