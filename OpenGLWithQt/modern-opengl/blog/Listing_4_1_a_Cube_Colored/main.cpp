#include <QtGui/QGuiApplication>
#include <QtGui/QKeyEvent>

#include <QtGui/QOpenGLWindow>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLVertexArrayObject>

static QString vertexShader =
        "#version 100\n"
        "\n"
        "attribute vec3 position;\n"
        "attribute vec3 color;\n"
        "\n"
        "uniform mat4 mvp;\n"
        "\n"
        "varying vec3 v_color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    v_color = color;\n"
        "    gl_Position = mvp * vec4(position, 1);\n"
        "}\n"
        ;

static QString fragmentShader =
        "#version 100\n"
        "\n"
        "varying vec3 v_color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = vec4(v_color, 1);\n"
        "}\n"
        ;

struct Window : QOpenGLWindow, QOpenGLFunctions
{
    Window() :
        m_vbo(QOpenGLBuffer::VertexBuffer),
        m_ibo(QOpenGLBuffer::IndexBuffer)
    {
    }

    void createShaderProgram()
    {
        if ( !m_pgm.addShaderFromSourceCode( QOpenGLShader::Vertex, vertexShader)) {
            qDebug() << "Error in vertex shader:" << m_pgm.log();
            exit(1);
        }
        if ( !m_pgm.addShaderFromSourceCode( QOpenGLShader::Fragment, fragmentShader)) {
            qDebug() << "Error in fragment shader:" << m_pgm.log();
            exit(1);
        }
        if ( !m_pgm.link() ) {
            qDebug() << "Error linking shader program:" << m_pgm.log();
            exit(1);
        }
    }

    void createGeometry()
    {
        // Initialize and bind the VAO that's going to capture all this vertex state
        m_vao.create();
        m_vao.bind();

        // we need 24 vertices, 24 normals, and 24 colors (6 faces, 4 vertices per face)
        // since we can't share normal data at the corners (each corner gets 3 normals)
        // and since we're not using glVertexAttribDivisor (not available in ES 2.0)

         struct Vertex {
             GLfloat position[3],
                     color   [3];
         } attribs[24]= {
            // Top face (y = 1.0f)
            { { 1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f} },     // Green
            { {-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f} },     // Green,
            { {-1.0f, 1.0f,  1.0f}, {0.0f, 1.0f, 0.0f} },     // Green,
            { { 1.0f, 1.0f,  1.0f}, {0.0f, 1.0f, 0.0f} },     // Green,

            // Bottom face (y = -1.0f)
            { { 1.0f, -1.0f,  1.0f}, {1.0f, 0.5f, 0.0f} },    // Orange
            { {-1.0f, -1.0f,  1.0f}, {1.0f, 0.5f, 0.0f} },    // Orange
            { {-1.0f, -1.0f, -1.0f}, {1.0f, 0.5f, 0.0f} },    // Orange
            { { 1.0f, -1.0f, -1.0f}, {1.0f, 0.5f, 0.0f} },    // Orange

            // Front face  (z = 1.0f)
            { { 1.0f,  1.0f, 1.0f}, {1.0f, 0.0f, 0.0f} },     // Red
            { {-1.0f,  1.0f, 1.0f}, {1.0f, 0.0f, 0.0f} },     // Red
            { {-1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f} },     // Red
            { { 1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f} },     // Red

            // Back face (z = -1.0f)
            { { 1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 0.0f} },    // Yellow
            { {-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 0.0f} },    // Yellow
            { {-1.0f,  1.0f, -1.0f}, {1.0f, 1.0f, 0.0f} },    // Yellow
            { { 1.0f,  1.0f, -1.0f}, {1.0f, 1.0f, 0.0f} },    // Yellow

            // Left face (x = -1.0f)
            { {-1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 1.0f} },    // Blue
            { {-1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, 1.0f} },    // Blue
            { {-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 1.0f} },    // Blue
            { {-1.0f, -1.0f,  1.0f}, {0.0f, 0.0f, 1.0f} },    // Blue

            // Right face (x = 1.0f)
            { {1.0f,  1.0f, -1.0f}, {1.0f, 0.0f, 1.0f} },     // Magenta
            { {1.0f,  1.0f,  1.0f}, {1.0f, 0.0f, 1.0f} },     // Magenta
            { {1.0f, -1.0f,  1.0f}, {1.0f, 0.0f, 1.0f} },     // Magenta
            { {1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 1.0f} },     // Magenta
        };

        // Put all the attribute data in a FBO
        m_vbo.create();
        m_vbo.setUsagePattern( QOpenGLBuffer::StaticDraw );
        m_vbo.bind();
        m_vbo.allocate(&attribs, sizeof(attribs));

        // Configure the vertex streams for this attribute data layout
        m_pgm.enableAttributeArray("position");
        m_pgm.setAttributeBuffer("position", GL_FLOAT, offsetof(Vertex, position), 3, sizeof(Vertex) );

        m_pgm.enableAttributeArray("color");
        m_pgm.setAttributeBuffer("color",   GL_FLOAT, offsetof(Vertex, color), 3, sizeof(Vertex) );

        // we need 36 indices (6 faces, 2 triangles per face, 3 vertices per triangle)
        struct {
            GLubyte cube[36];
        } indices;
        m_cnt=0; for (GLsizei i=0, v=0; v<6*4; v+=4)
        {
            // first triangle (ccw winding)
            indices.cube[i++] = v + 0;
            indices.cube[i++] = v + 1;
            indices.cube[i++] = v + 2;

             // second triangle (ccw winding)
            indices.cube[i++] = v + 0;
            indices.cube[i++] = v + 2;
            indices.cube[i++] = v + 3;

            m_cnt = i;
        }

        // Put all the index data in a IBO
        m_ibo.create();
        m_ibo.setUsagePattern( QOpenGLBuffer::StaticDraw );
        m_ibo.bind();
        m_ibo.allocate(&indices, sizeof(indices));

        // Okay, we've finished setting up the vao
        m_vao.release();
    }

    void initializeGL()
    {
        QOpenGLFunctions::initializeOpenGLFunctions();
        createShaderProgram(); m_pgm.bind();
        createGeometry();
        m_view.setToIdentity();
        glEnable(GL_DEPTH_TEST);
    }

    void resizeGL(int w, int h)
    {
        glViewport(0, 0, w, h);
        m_projection.setToIdentity();
        if (w <= h)
            m_projection.ortho(-2.f, 2.f, -2.f*h/w, 2.f*h/w, -2.f, 2.f);
        else
            m_projection.ortho(-2.f*w/h, 2.f*w/h, -2.f, 2.f, -2.f, 2.f);
        update();
    }

    void paintGL()
    {
        static unsigned cnt;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        QMatrix4x4 model;
        model.rotate(cnt%360, 1,0,0);
        model.rotate(45, 0,0,1);
        m_pgm.bind();
        m_pgm.setUniformValue("mvp", m_projection * m_view * model);
        m_vao.bind();
        glDrawElements(GL_TRIANGLES, m_cnt, GL_UNSIGNED_BYTE, 0);

        update();
        ++cnt;
    }

    void keyPressEvent(QKeyEvent * ev)
    {
        switch (ev->key()) {
           case Qt::Key_Escape:
              exit(0);
              break;
           default:
              QOpenGLWindow::keyPressEvent(ev);
              break;
        }

    }

    QMatrix4x4 m_projection, m_view;
    QOpenGLShaderProgram     m_pgm;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer            m_vbo;
    QOpenGLBuffer            m_ibo;
    GLsizei                  m_cnt;
};

int main(int argc, char *argv[])
{
    QGuiApplication a(argc,argv);
    Window w;
    w.setWidth(640); w.setHeight(480);
    w.show();
    return a.exec();
}
