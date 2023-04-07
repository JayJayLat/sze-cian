#include<gl/glew.h>
#include <GLFW/glfw3.h>
#include<glm.hpp>
#include<gtc/matrix_transform.hpp>
#include<gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


std::string wczytajShader(std::string path)
{
    std::ifstream plik(path.c_str());
    std::string src;
    std::ostringstream stringstream;
    stringstream << plik.rdbuf();
    src = stringstream.str();
    //std::cout << src << std::endl;
    return src;
}

GLfloat vektory[]
{
    //x     y     z
    //     dó³
     -0.25f, 0.0f, 0.25f,  //0
      0.25f, 0.0f, 0.25f,  //1
     -0.25f, 0.0f, -0.25f, //2
      0.25f, 0.0f, -0.25f, //3
   //      góra   
      -0.25f, 0.5f, 0.25f, //4
      0.25f, 0.5f, 0.25f,  //5
     -0.25f, 0.5f, -0.25f, //6
      0.25f, 0.5f, -0.25f, //7
};

GLuint indeksy[]
{
    //dó³
    0, 1, 2,
    1, 2, 3,
    //góra
    4, 5, 6,
    5, 6, 7,
    //lewo
    0, 3, 4,
    3, 7, 4,
    //prawo
    1, 2, 5,
    2, 5, 6,
    //przód
    0, 1, 4,
    1, 5, 4,
    //ty³
    2, 3, 7,
    2, 6, 7
};

int main(void)
{

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Cube", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW not initialized" << std::endl;
        return -1;
    }

   

    //przygotowanie do rysowania cube'a
    glViewport(0, 0, 800, 600);

    //wczytanie kodu shaderów
    std::string vertexCode = wczytajShader("shader.vert");
    std::string fragmentCode = wczytajShader("shader.frag");
    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();

    //kompilacja vertex shader'a
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);


    //kompilacja fragment shader'a
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    //stworzenie shaderProgram'u
    GLuint shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //stworzenie VAO
    GLuint VAO;
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    

    //stworzenie VBO
    GLuint VBO;
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vektory), vektory, GL_DYNAMIC_DRAW);

    //stworzenie EBO
    GLuint EBO;
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indeksy), indeksy, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    


    //cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    std::cout << glGetError() << std::endl;

    //inicjalizacja zmiennych zwi¹zanych z rotacj¹
    float rotacja = 0.0f;

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {

        glm::mat4 proj = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 model = glm::mat4(1.0f);
        
        
        rotacja += 0.5f;
        
        model = glm::rotate(model, glm::radians(rotacja), glm::vec3(1.0f, 1.0f, 1.0f));
        view = glm::translate(view, glm::vec3(0.0f, -0.5f, -4.0f));
        proj = glm::perspective(glm::radians(45.0f), (float)(800 / 800), 0.1f, 100.0f);

        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        int projLoc = glGetUniformLocation(shaderProgram, "proj");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sizeof(indeksy) / sizeof(int), GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
        std::cout << glGetError() << std::endl;
    }

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;

}