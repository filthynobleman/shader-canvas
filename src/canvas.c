/**
 * @file        canvas.c
 * 
 * @brief       A small application for fast testing of fragment shaders in OpenGL.
 * 
 * @details     This is a small application that runs OpenGL fragment shaders on a blank canvas.\n
 *              Shaders can be recompiled in real-time while the application runs.
 * 
 * @author      Filippo Maggioli\n 
 *              (maggioli@di.uniroma1.it, maggioli.filippo@gmail.com)\n 
 *              Sapienza, University of Rome - Department of Computer Science
 * @date        2021-12-01
 */

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image_write.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef __cplusplus
#define bool    int
#define true    (0 == 0)
#define false   (0 == 1)
#endif


typedef struct CanvasArgs
{
    int width;
    int height;
    const char* fshader_file;
    bool fullscreen;
} canvargs_t;



void resize_callback(GLFWwindow* window, int width, int height);
bool load_file(const char* filename, char** content);
bool compile_shader(GLuint* sid, const char* source, GLenum stype);
bool compile_program(GLuint* pid, GLuint vid, GLuint fid);
bool check_compile_errors(GLuint sid, GLenum stype);
void parse_args(canvargs_t* argstruct, int argc, char const *argv[]);
void usage(const char const* arg0, FILE* stream);



const char* vshader_src = "#version 440 core\n"\
"layout(location = 0) in vec2 aPos;\n"\
"out vec2 UV;\n"\
"void main() {\n"\
"gl_Position = vec4(aPos, 0.0f, 1.0f);\n"\
"UV = aPos / 2.0f + 0.5f;\n"\
"}\n";


const char* default_fshader_src = "#version 440 core\n"\
"in vec2 UV;\n"\
"out vec4 FragColor;\n"\
"void main() {\n"\
"FragColor = vec4(UV, 0.0f, 1.0f);\n"\
"}\n";


const float screen_coords[12] = {
    -1.0f, -1.0f,
     1.0f,  1.0f,
    -1.0f,  1.0f,

    -1.0f, -1.0f,
     1.0f, -1.0f,
     1.0f,  1.0f
};

unsigned char* fb_content;

int main(int argc, char const *argv[])
{
    // Parse arguments
    canvargs_t args;
    parse_args(&args, argc, argv);

    // Initialize GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Cannot initialize GLFW.\n");
        exit(-1);
    }

    // Create a window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window;
    if (args.fullscreen)
    {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);
        window = glfwCreateWindow(vidmode->width, vidmode->height, "Shader Toy", monitor, NULL);
    }
    else
        window = glfwCreateWindow(args.width, args.height, "Shader Toy", NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "Cannot create a window.\n");
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, resize_callback);
    

    // Initialize GLAD
    int glad_status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (!glad_status)
    {
        fprintf(stderr, "Cannot initialize GLAD.\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(-1);
    }


    // Read fragment shader, if not default
    char* fshader_data = NULL;
    if (args.fshader_file != NULL)
        load_file(args.fshader_file, &fshader_data);
    const char* fshader_src = (fshader_data == NULL) ? default_fshader_src : fshader_data;


    // Compile shaders
    GLuint vid, fid, pid;
    if (!compile_shader(&vid, vshader_src, GL_VERTEX_SHADER))
        exit(-1);
    if (!compile_shader(&fid, fshader_src, GL_FRAGMENT_SHADER))
        exit(-1);
    if (!compile_program(&pid, vid, fid))
        exit(-1);
    glDeleteShader(vid);
    glDeleteShader(fid);
    if (args.fshader_file != NULL)
        free(fshader_data);


    // Create the quad
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), screen_coords, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // Print instructions
    printf("Press R to recompile the fragment shader from the same file.\n");
    printf("Press E to export the currentlt rendered frame.\n");
    printf("Press ESC to quit the application.\n");


    int stride = 3 * args.width;
    stride += (stride % 4) * (4 - stride % 4);
    fb_content = (unsigned char*)malloc(stride * args.height * sizeof(unsigned char));
    if (fb_content == NULL)
        fprintf(stderr, "WARNING: Export functionality is disabled!\n");


    // Main loop
    float Start = glfwGetTime();
    int CurFrame = 0;
    char frame_name[512];
    while(!glfwWindowShouldClose(window))
    {
        // ESC quits the application
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        // R reloads the fragment shader
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            // Read fragment shader, if not default
            fshader_data = NULL;
            if (args.fshader_file != NULL)
                load_file(args.fshader_file, &fshader_data);
            fshader_src = (fshader_data == NULL) ? default_fshader_src : fshader_data;


            // Compile shaders
            if (!compile_shader(&vid, vshader_src, GL_VERTEX_SHADER))
                exit(-1);
            if (!compile_shader(&fid, fshader_src, GL_FRAGMENT_SHADER))
                exit(-1);
            if (!compile_program(&pid, vid, fid))
                exit(-1);
            glDeleteShader(vid);
            glDeleteShader(fid);
            if (args.fshader_file != NULL)
                free(fshader_data);
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && fb_content != NULL)
        {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            int stride = 3 * width;
            stride += (stride % 4) * (4 - stride % 4);
            glPixelStorei(GL_PACK_ALIGNMENT, 4);
            glReadBuffer(GL_FRONT);
            glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, fb_content);
            sprintf(frame_name, "ShaderCanvas-frame%05d.png", CurFrame);
            stbi_flip_vertically_on_write(true);
            stbi_write_png(frame_name, width, height, 3, fb_content, stride);
        }


        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);


        glUseProgram(pid);
        int TimeLoc = glGetUniformLocation(pid, "Time");
        if (TimeLoc >= 0)
        {
            float Time = glfwGetTime() - Start;
            glUniform1f(TimeLoc, Time);
        }        
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);


        glfwSwapBuffers(window);
        glfwPollEvents();
        
        CurFrame++;
    }



    
    // Terminate GLFW
    glfwTerminate();

    return 0;
}


void resize_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    int stride = 3 * width;
    stride += (stride % 4) * (4 - stride % 4);
    fb_content = (unsigned char*)malloc(stride * height * sizeof(unsigned char));
    if (fb_content == NULL)
        fprintf(stderr, "WARNING: Export functionality is disabled!\n");
}



bool compile_shader(GLuint* sid, const char* source, GLenum stype)
{
    *sid = glCreateShader(stype);
    glShaderSource(*sid, 1, &source, NULL);
    glCompileShader(*sid);
    return check_compile_errors(*sid, stype);
}

bool compile_program(GLuint* pid, GLuint vid, GLuint fid)
{
    *pid = glCreateProgram();
    glAttachShader(*pid, vid);
    glAttachShader(*pid, fid);
    glLinkProgram(*pid);
    return check_compile_errors(*pid, GL_PROGRAM);
}

bool check_compile_errors(GLuint sid, GLenum stype)
{
    int succ;
    char log[4096];

    glGetShaderiv(sid, GL_COMPILE_STATUS, &succ);
    if (!succ)
    {
        char* type_str;
        switch (stype)
        {
        case GL_VERTEX_SHADER: type_str = "vertex shader"; break;
        case GL_FRAGMENT_SHADER: type_str = "fragment shader"; break;
        case GL_PROGRAM: type_str = "shader program"; break;
        
        default:
            break;
        }
        glGetShaderInfoLog(sid, 4096, NULL, log);
        fprintf(stderr, "Error compiling %s.\n", type_str);
        fprintf(stderr, "==========================================================\n");
        fprintf(stderr, "%s\n", log);
        fprintf(stderr, "==========================================================\n");
        return false;
    }
    return true;
}

bool load_file(const char* filename, char** content)
{
    FILE* stream = fopen(filename, "r");
    if (stream == NULL)
    {
        fprintf(stderr, "Cannot open file %s for reading.\n", filename);
        exit(-1);
    }

    *content = (char*)calloc(4096, sizeof(char));
    if (*content == NULL)
    {
        fprintf(stderr, "Cannot allocate memory for reading file %s.\n", filename);
        exit(-1);
    }


    char* block = *content;
    size_t bsize;
    size_t csize = 4096;
    while((bsize = fread(block, sizeof(char), 4096, stream)) == 4096)
    {
        csize += 4096;
        *content = (char*)realloc(*content, csize * sizeof(char));
        if (*content == NULL)
        {
            fprintf(stderr, "Cannot allocate memory for reading file %s.\n", filename);
            exit(-1);
        }
        block += 4096;
    }
    block[bsize] = '\0';

    fclose(stream);
    return true;
}

void usage(const char const* arg0, FILE* stream)
{
    fprintf(stream, "Usage:\n");
    fprintf(stream, "%s --help\n", arg0);
    fprintf(stream, "    Print this help message and quits the application.\n\n");

    fprintf(stream, "%s [FILENAME] [-w WIDTH] [-h HEIGHT] [-f]\n", arg0);
    fprintf(stream, "Without arguments, run a simple hardcoded shader blending red and green with screen coordinates.\n");
    fprintf(stream, "FILENAME is a file path to a fragment shader to run.\n");
    fprintf(stream, "-w and -h are used for the initial size of the window.\n");
    fprintf(stream, "-f executes in fullscreen.\n\n\n");
}

void parse_args(canvargs_t* argstruct, int argc, char const *argv[])
{
    argstruct->width = 800;
    argstruct->height = 600;
    argstruct->fshader_file = NULL;
    argstruct->fullscreen = false;


    int i = 1;
    while (i < argc)
    {
        const char* arg = argv[i];
        if (arg[0] == '-')
        {
            // Then it's an option
            // Check if help
            if (strcmp(arg, "--help") == 0)
            {
                usage(argv[0], stdout);
                exit(0);
            }
            // Width
            if (strcmp(arg, "-w") == 0)
            {
                argstruct->width = atoi(argv[i + 1]);
                i += 2;
                continue;
            }
            // Height
            if (strcmp(arg, "-h") == 0)
            {
                argstruct->height = atoi(argv[i + 1]);
                i += 2;
                continue;
            }
            // Fullscreen
            if (strcmp(arg, "-f") == 0)
            {
                argstruct->fullscreen = true;
                continue;
            }
            // Invalid option
            fprintf(stderr, "%s is not a valid option.\n", arg);
            usage(argv[0], stderr);
        }
        else
        {
            // Then it's the filename
            if (argstruct->fshader_file != NULL)
            {
                fprintf(stderr, "Cannot provide multiple fragment shaders.\n");
                usage(argv[0], stderr);
                exit(-1);
            }
            argstruct->fshader_file = arg;
            i++;
        }
    }
}