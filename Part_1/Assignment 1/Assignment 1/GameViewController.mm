//
//  GameViewController.m
//  Flashlight
//
//  Created by Borna Noureddin.
//  Copyright (c) 2014 BCIT. All rights reserved.
//

#import "GameViewController.h"
#import <OpenGLES/ES2/glext.h>

#include <chrono>
#include <deque>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// Shader uniform indices
enum
{
    UNIFORM_MODELVIEWPROJECTION_MATRIX,
    UNIFORM_VERTEX_COLOR,
    //UNIFORM_NORMAL_MATRIX,
    //UNIFORM_MODELVIEW_MATRIX,
    /* more uniforms needed here... */
    //UNIFORM_TEXTURE,
    //UNIFORM_FLASHLIGHT_POSITION,
    //UNIFORM_DIFFUSE_LIGHT_POSITION,
    //UNIFORM_SHININESS,
    //UNIFORM_AMBIENT_COMPONENT,
    //UNIFORM_DIFFUSE_COMPONENT,
    //UNIFORM_SPECULAR_COMPONENT,
    NUM_UNIFORMS
};
GLint uniforms[NUM_UNIFORMS];

@interface GameViewController () {
    //shader program
    GLuint _program;

    // Shader uniforms
    GLKMatrix4 _modelViewProjectionMatrix;
    GLKMatrix4 _modelViewMatrix;
    //GLKMatrix3 _normalMatrix;

    // Lighting parameters
    /* specify lighting parameters here...e.g., GLKVector3 flashlightPosition; */
    //GLKVector3 flashlightPosition;
    //GLKVector3 diffuseLightPosition;
    //GLKVector4 diffuseComponent;
    //float shininess;
    //GLKVector4 specularComponent;
    //GLKVector4 ambientComponent;
    
    GLKVector3 vertexColor;
    
    // Transformation parameters
    bool isRotating;
    float xRot, yRot;
    float xTrans, yTrans;
    float scale;
    
    //Gesture start points
    CGPoint dragStart;
    CGPoint doubleDragStart;
    CGFloat pinchStart;
    
    //switch between GLK and asm multiply counter
    MatrixMultiplyType matrixFunct;

    // Shape vertices, etc. and textures
    GLfloat *vertices, *normals, *texCoords;
    GLuint numIndices, *indices;
    /* texture parameters ??? */
    //GLuint crateTexture, extraTexture;

    // GLES buffer IDs
    GLuint _vertexArray;
    GLuint _vertexBuffers[3];
    GLuint _indexBuffer;
    //outlets to access labels
    __weak IBOutlet UILabel *PositionLabel;
    __weak IBOutlet UILabel *CounterLabel;
    __weak IBOutlet UILabel *RotationLabel;
}


@property (strong, nonatomic) EAGLContext *context;
//initialize openGL - compile shaders, get uniform locations, set up VBOs and VAO
- (void)setupGL;
//free openGL resources
- (void)tearDownGL;
//loading shaders
- (BOOL)loadShaders;
- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file;
- (BOOL)linkProgram:(GLuint)prog;
- (BOOL)validateProgram:(GLuint)prog;
//button click actions
- (IBAction)toggle:(id)sender;
- (IBAction)reset:(id)sender;
//helper to update counter label text
-(void) updateData;
@end

@implementation GameViewController

- (void)viewDidLoad
{
    [super viewDidLoad];

    // Set up iOS gesture recognizers
    UITapGestureRecognizer *doubleTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(doDoubleTap:)];
    doubleTap.numberOfTapsRequired = 2;
    [self.view addGestureRecognizer:doubleTap];
    
    UIPanGestureRecognizer *rotObj = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(doDrag:)];
    rotObj.minimumNumberOfTouches = 1;
    rotObj.maximumNumberOfTouches = 1;
    [self.view addGestureRecognizer:rotObj];
    
    UIPanGestureRecognizer *transObj = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(doDoubleDrag:)];
    transObj.minimumNumberOfTouches = 2;
    transObj.maximumNumberOfTouches = 2;
    [self.view addGestureRecognizer:transObj];
    
    UIPinchGestureRecognizer *pinchObj = [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(doPinch:)];
    [self.view addGestureRecognizer:pinchObj];
    
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    
    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;

    // Set up transformation parameters
    isRotating = NO;
    yRot = xRot = 0.0;
    xTrans = yTrans = 0.0;
    scale = 1.0;
    vertexColor = GLKVector3Make(0.0, 1.0, 0.0); //green
    matrixFunct = GLK;
    // Set up GL
    [self setupGL];
}

- (void)dealloc
{
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    
    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;
        
        [self tearDownGL];
        
        if ([EAGLContext currentContext] == self.context) {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];

    // Load shaders
    [self loadShaders];
    
    // Get uniform locations.
    uniforms[UNIFORM_MODELVIEWPROJECTION_MATRIX] = glGetUniformLocation(_program, "modelViewProjectionMatrix");
    uniforms[UNIFORM_VERTEX_COLOR] = glGetUniformLocation(_program, "vColor");
    
    // Initialize GL and get buffers
    glEnable(GL_DEPTH_TEST);
    
    glGenVertexArraysOES(1, &_vertexArray);
    glBindVertexArrayOES(_vertexArray);
    
    glGenBuffers(3, _vertexBuffers);
    glGenBuffers(1, &_indexBuffer);

    // Generate vertices
    int numVerts;
    numIndices = generateCube(1.5, &vertices, &normals, &texCoords, &indices, &numVerts);

    // Set up GL buffers
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*numVerts, vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(GLKVertexAttribPosition);
    glVertexAttribPointer(GLKVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), BUFFER_OFFSET(0));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*numIndices, indices, GL_STATIC_DRAW);

    glBindVertexArrayOES(0);
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];

    // Delete GL buffers
    glDeleteBuffers(3, _vertexBuffers);
    glDeleteBuffers(1, &_indexBuffer);
    glDeleteVertexArraysOES(1, &_vertexArray);

    // Delete vertices buffers
    if (vertices)
        free(vertices);
    if (indices)
        free(indices);
    if (normals)
        free(normals);
    if (texCoords)
        free(texCoords);

    // Delete shader program
    if (_program) {
        glDeleteProgram(_program);
        _program = 0;
    }
}


#pragma mark - iOS gesture events

- (IBAction)doDoubleTap:(UITapGestureRecognizer *)recognizer
{
    //toggle rotation
    isRotating = !isRotating;
}

- (IBAction)doDrag:(UIPanGestureRecognizer *)recognizer
{
    if(recognizer.state == UIGestureRecognizerStateBegan)
    {
        //get initial point
        dragStart = [recognizer locationInView:self.view];
    }
    else if (!isRotating &&
             recognizer.state != UIGestureRecognizerStateEnded)
    {
        //handle dragging: rotate in x,y
        CGPoint newPt = [recognizer locationInView:self.view];
        xRot += (newPt.x - dragStart.x) / 100;
        yRot -= (newPt.y - dragStart.y) / 100;//invert y
        //refresh initial point
        dragStart = newPt;
    }
}

- (IBAction)doDoubleDrag:(UIPanGestureRecognizer *)recognizer
{
    if(recognizer.state == UIGestureRecognizerStateBegan)
    {
        //get initial point
        doubleDragStart = [recognizer locationInView:self.view];
    }
    else if (!isRotating &&
             recognizer.state != UIGestureRecognizerStateEnded)
    {
        //handle double dragging: translate in x, y
        CGPoint newPt = [recognizer locationInView:self.view];
        xTrans += (newPt.x - doubleDragStart.x) / 100;
        yTrans -= (newPt.y - doubleDragStart.y) / 100;//invert y
        //refresh initial point
        doubleDragStart = newPt;
    }
}

- (IBAction)doPinch:(UIPinchGestureRecognizer *)recognizer
{
    if(recognizer.state == UIGestureRecognizerStateBegan)
    {
        //get initial distance
        pinchStart = recognizer.scale;
    }
    else if (!isRotating &&
             recognizer.state != UIGestureRecognizerStateEnded)
    {
        //handle pinch: scale in, out
        CGFloat newScale = recognizer.scale;
        scale -= newScale - pinchStart;
        //don't scale into negatives
        scale = MAX(scale, 0);
        //update initial distance
        pinchStart = newScale;
    }
}

- (IBAction)toggle:(id)sender
{
    //toggle counter
    if(matrixFunct == GLK)
    {
        CounterLabel.text = @"NEON Matrix Multiply";
        matrixFunct = NEON;
    }
    else
    {
        CounterLabel.text = @"GLK Matrix Multiply";
        matrixFunct = GLK;
    }
}

- (IBAction)reset:(id)sender
{
    //reset all translations, rotations, stop rotating
    xTrans = yTrans = 0;
    xRot = yRot = 0;
    isRotating = NO;
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
    if(isRotating)
    {
        yRot += 0.1;
    }
    //update position, rotation information
    [self updateData];
    // Set up base model view matrix (place camera)
    GLKMatrix4 world = GLKMatrix4MakeTranslation(xTrans, yTrans, 0.0);
    world = MatrixMultiply(world, GLKMatrix4MakeRotation(yRot, 0.0, 1.0, 0.0), matrixFunct);
    world = MatrixMultiply(world, GLKMatrix4MakeRotation(xRot, 1.0, 0.0, 0.0), matrixFunct);
    world = GLKMatrix4Scale(world, scale, scale, scale);
    GLKMatrix4 view = GLKMatrix4MakeLookAt(0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    
    _modelViewMatrix = MatrixMultiply(view, world, matrixFunct);

    // Calculate projection matrix
    float aspect = fabsf(self.view.bounds.size.width / self.view.bounds.size.height);
    GLKMatrix4 projectionMatrix = GLKMatrix4MakePerspective(GLKMathDegreesToRadians(65.0f), aspect, 0.1f, 100.0f);

    _modelViewProjectionMatrix = GLKMatrix4Multiply(projectionMatrix, _modelViewMatrix);
}

-(void) updateData
{
    [PositionLabel setText:[NSString stringWithFormat:@"x: %1.2f y: %1.2f z: 0.00",xTrans, yTrans]];
    [RotationLabel setText:[NSString stringWithFormat:@"rx: %1.2f ry: %1.2f rz: 0.00", xRot, yRot]];
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    // Clear window
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Select VAO and shaders
    glBindVertexArrayOES(_vertexArray);
    glUseProgram(_program);

    // Set up uniforms
    glUniformMatrix4fv(uniforms[UNIFORM_MODELVIEWPROJECTION_MATRIX], 1, 0, _modelViewProjectionMatrix.m);
    //set the vertex color.
    glUniform3fv(uniforms[UNIFORM_VERTEX_COLOR], 1, vertexColor.v);
    // Select VBO and draw
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);}

#pragma mark -  OpenGL ES 2 shader compilation

- (BOOL)loadShaders
{
    GLuint vertShader, fragShader;
    NSString *vertShaderPathname, *fragShaderPathname;
    
    // Create shader program.
    _program = glCreateProgram();
    
    // Create and compile vertex shader.
    vertShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"vsh"];
    if (![self compileShader:&vertShader type:GL_VERTEX_SHADER file:vertShaderPathname]) {
        NSLog(@"Failed to compile vertex shader");
        return NO;
    }
    
    // Create and compile fragment shader.
    fragShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"fsh"];
    if (![self compileShader:&fragShader type:GL_FRAGMENT_SHADER file:fragShaderPathname]) {
        NSLog(@"Failed to compile fragment shader");
        return NO;
    }
    
    // Attach vertex shader to program.
    glAttachShader(_program, vertShader);
    
    // Attach fragment shader to program.
    glAttachShader(_program, fragShader);
    
    // Bind attribute locations.
    // This needs to be done prior to linking.
    glBindAttribLocation(_program, GLKVertexAttribPosition, "position");
    
    // Link program.
    if (![self linkProgram:_program]) {
        NSLog(@"Failed to link program: %d", _program);
        
        if (vertShader) {
            glDeleteShader(vertShader);
            vertShader = 0;
        }
        if (fragShader) {
            glDeleteShader(fragShader);
            fragShader = 0;
        }
        if (_program) {
            glDeleteProgram(_program);
            _program = 0;
        }
        
        return NO;
    }
    
    // Release vertex and fragment shaders.
    if (vertShader) {
        glDetachShader(_program, vertShader);
        glDeleteShader(vertShader);
    }
    if (fragShader) {
        glDetachShader(_program, fragShader);
        glDeleteShader(fragShader);
    }
    
    return YES;
}

- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file
{
    GLint status;
    const GLchar *source;
    
    source = (GLchar *)[[NSString stringWithContentsOfFile:file encoding:NSUTF8StringEncoding error:nil] UTF8String];
    if (!source) {
        NSLog(@"Failed to load vertex shader");
        return NO;
    }
    
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);
    
#if defined(DEBUG)
    GLint logLength;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(*shader, logLength, &logLength, log);
        NSLog(@"Shader compile log:\n%s", log);
        free(log);
    }
#endif
    
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        glDeleteShader(*shader);
        return NO;
    }
    
    return YES;
}

- (BOOL)linkProgram:(GLuint)prog
{
    GLint status;
    glLinkProgram(prog);
    
#if defined(DEBUG)
    GLint logLength;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        NSLog(@"Program link log:\n%s", log);
        free(log);
    }
#endif
    
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == 0) {
        return NO;
    }
    
    return YES;
}

- (BOOL)validateProgram:(GLuint)prog
{
    GLint logLength, status;
    
    glValidateProgram(prog);
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        NSLog(@"Program validate log:\n%s", log);
        free(log);
    }
    
    glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
    if (status == 0) {
        return NO;
    }
    
    return YES;
}
#pragma mark - Matrix Multiply
GLKMatrix4 MatrixMultiply(GLKMatrix4 a, GLKMatrix4 b, MatrixMultiplyType funct)
{
    static std::deque<int> queue;
    GLKMatrix4 ret;
    std::chrono::high_resolution_clock::time_point before, after;
    switch(funct)
    {
        case GLK:
            before = std::chrono::high_resolution_clock::now();
            ret = GLKMatrix4Multiply(a, b);
            after = std::chrono::high_resolution_clock::now();
            break;
        case NEON:
            before = std::chrono::high_resolution_clock::now();
            ret = MatrixMultiply_NEON(a, b);
            after = std::chrono::high_resolution_clock::now();
            break;
    }
    queue.push_back((int)std::chrono::duration_cast<std::chrono::microseconds>(after - before).count());
    if(queue.size() > 50)
    {
        queue.pop_front();
    }
    //calculate the rolling average
    int sum = 0;
    for(int time : queue)
    {
        sum += time;
    }
    NSLog(@"average time to mult: %lu microseconds", sum / queue.size());
    return ret;
}

GLKMatrix4 MatrixMultiply_NEON(GLKMatrix4 a, GLKMatrix4 b)
{
    GLKMatrix4 ret;
    float* data_a = a.m;
    float* data_b = b.m;
    float* data_ret = ret.m;
    __asm__ volatile("mov r4, %[data_a]                   \n"
                     "mov r5, %[data_b]                   \n"
                     "mov r6, %[ret]                      \n"
                     "vld1.32  {d16-d19}, [r4]!            @ load first eight elements of matrix 0\n"
                     "vld1.32  {d20-d23}, [r4]!            @ load second eight elements of matrix 0\n"
                     "vld1.32  {d0-d3}, [r5]!              @ load first eight elements of matrix 1\n"
                     "vld1.32  {d4-d7}, [r5]!              @ load second eight elements of matrix 1\n"
                     
                     "vmul.f32    q12, q8, d0[0]              @ rslt col0  = (mat0 col0) * (mat1 col0 elt0)\n"
                     "vmul.f32    q13, q8, d2[0]              @ rslt col1  = (mat0 col0) * (mat1 col1 elt0)\n"
                     "vmul.f32    q14, q8, d4[0]              @ rslt col2  = (mat0 col0) * (mat1 col2 elt0)\n"
                     "vmul.f32    q15, q8, d6[0]              @ rslt col3  = (mat0 col0) * (mat1 col3 elt0)\n"
                     
                     "vmla.f32    q12, q9, d0[1]              @ rslt col0 += (mat0 col1) * (mat1 col0 elt1)\n"
                     "vmla.f32    q13, q9, d2[1]              @ rslt col1 += (mat0 col1) * (mat1 col1 elt1)\n"
                     "vmla.f32    q14, q9, d4[1]              @ rslt col2 += (mat0 col1) * (mat1 col2 elt1)\n"
                     "vmla.f32    q15, q9, d6[1]              @ rslt col3 += (mat0 col1) * (mat1 col3 elt1)\n"
                     
                     "vmla.f32    q12, q10, d1[0]             @ rslt col0 += (mat0 col2) * (mat1 col0 elt2)\n"
                     "vmla.f32    q13, q10, d3[0]             @ rslt col1 += (mat0 col2) * (mat1 col1 elt2)\n"
                     "vmla.f32    q14, q10, d5[0]             @ rslt col2 += (mat0 col2) * (mat1 col2 elt2)\n"
                     "vmla.f32    q15, q10, d7[0]             @ rslt col3 += (mat0 col2) * (mat1 col2 elt2)\n"
                     
                     "vmla.f32    q12, q11, d1[1]             @ rslt col0 += (mat0 col3) * (mat1 col0 elt3)\n"
                     "vmla.f32    q13, q11, d3[1]             @ rslt col1 += (mat0 col3) * (mat1 col1 elt3)\n"
                     "vmla.f32    q14, q11, d5[1]             @ rslt col2 += (mat0 col3) * (mat1 col2 elt3)\n"
                     "vmla.f32    q15, q11, d7[1]             @ rslt col3 += (mat0 col3) * (mat1 col3 elt3)\n"
                     
                     "vst1.32  {d24-d27}, [r6]!            @ store first eight elements of result\n"
                     "vst1.32  {d28-d31}, [r6]!            @ store second eight elements of result\n"
                     :
                     : [data_a]"r"(data_a), [data_b]"r"(data_b), [ret]"r"(data_ret)
                     : "r4", "r5", "r6", "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
                     );
    
//    float32x4x4_t iMatrixLeft = *(float32x4x4_t *)&a;
//    float32x4x4_t iMatrixRight = *(float32x4x4_t *)&b;
//    float32x4x4_t m;
//    m.val[0] = vmulq_n_f32(iMatrixLeft.val[0], vgetq_lane_f32(iMatrixRight.val[0], 0));
//    m.val[1] = vmulq_n_f32(iMatrixLeft.val[0], vgetq_lane_f32(iMatrixRight.val[1], 0));
//    m.val[2] = vmulq_n_f32(iMatrixLeft.val[0], vgetq_lane_f32(iMatrixRight.val[2], 0));
//    m.val[3] = vmulq_n_f32(iMatrixLeft.val[0], vgetq_lane_f32(iMatrixRight.val[3], 0));
//    
//    m.val[0] = vmlaq_n_f32(m.val[0], iMatrixLeft.val[1], vgetq_lane_f32(iMatrixRight.val[0], 1));
//    m.val[1] = vmlaq_n_f32(m.val[1], iMatrixLeft.val[1], vgetq_lane_f32(iMatrixRight.val[1], 1));
//    m.val[2] = vmlaq_n_f32(m.val[2], iMatrixLeft.val[1], vgetq_lane_f32(iMatrixRight.val[2], 1));
//    m.val[3] = vmlaq_n_f32(m.val[3], iMatrixLeft.val[1], vgetq_lane_f32(iMatrixRight.val[3], 1));
//    
//    m.val[0] = vmlaq_n_f32(m.val[0], iMatrixLeft.val[2], vgetq_lane_f32(iMatrixRight.val[0], 2));
//    m.val[1] = vmlaq_n_f32(m.val[1], iMatrixLeft.val[2], vgetq_lane_f32(iMatrixRight.val[1], 2));
//    m.val[2] = vmlaq_n_f32(m.val[2], iMatrixLeft.val[2], vgetq_lane_f32(iMatrixRight.val[2], 2));
//    m.val[3] = vmlaq_n_f32(m.val[3], iMatrixLeft.val[2], vgetq_lane_f32(iMatrixRight.val[3], 2));
//    
//    m.val[0] = vmlaq_n_f32(m.val[0], iMatrixLeft.val[3], vgetq_lane_f32(iMatrixRight.val[0], 3));
//    m.val[1] = vmlaq_n_f32(m.val[1], iMatrixLeft.val[3], vgetq_lane_f32(iMatrixRight.val[1], 3));
//    m.val[2] = vmlaq_n_f32(m.val[2], iMatrixLeft.val[3], vgetq_lane_f32(iMatrixRight.val[2], 3));
//    m.val[3] = vmlaq_n_f32(m.val[3], iMatrixLeft.val[3], vgetq_lane_f32(iMatrixRight.val[3], 3));
    
    return ret;
}
#pragma mark - Utility functions

// Generate vertices, normals, texture coordinates and indices for cube
//      Adapted from Dan Ginsburg, Budirijanto Purnomo from the book
//      OpenGL(R) ES 2.0 Programming Guide
int generateCube(float scale, GLfloat **vertices, GLfloat **normals,
                 GLfloat **texCoords, GLuint **indices, int *numVerts)
{
    int i;
    int numVertices = 24;
    int numIndices = 36;
    
    GLfloat cubeVerts[] =
    {
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f,  0.5f, 0.5f,
        0.5f,  0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
    };
    
    GLfloat cubeNormals[] =
    {
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
    };
    
    GLfloat cubeTex[] =
    {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
    };
    
    // Allocate memory for buffers
    if ( vertices != NULL )
    {
        *vertices = (GLfloat*) malloc ( sizeof ( GLfloat ) * 3 * numVertices );
        memcpy ( *vertices, cubeVerts, sizeof ( cubeVerts ) );
        
        for ( i = 0; i < numVertices * 3; i++ )
        {
            ( *vertices ) [i] *= scale;
        }
    }
    
    if ( normals != NULL )
    {
        *normals = (GLfloat*) malloc ( sizeof ( GLfloat ) * 3 * numVertices );
        memcpy ( *normals, cubeNormals, sizeof ( cubeNormals ) );
    }
    
    if ( texCoords != NULL )
    {
        *texCoords = (GLfloat*) malloc ( sizeof ( GLfloat ) * 2 * numVertices );
        memcpy ( *texCoords, cubeTex, sizeof ( cubeTex ) ) ;
    }
    
    
    // Generate the indices
    if ( indices != NULL )
    {
        GLuint cubeIndices[] =
        {
            0, 2, 1,
            0, 3, 2,
            4, 5, 6,
            4, 6, 7,
            8, 9, 10,
            8, 10, 11,
            12, 15, 14,
            12, 14, 13,
            16, 17, 18,
            16, 18, 19,
            20, 23, 22,
            20, 22, 21
        };
        
        *indices = (GLuint*) malloc ( sizeof ( GLuint ) * numIndices );
        memcpy ( *indices, cubeIndices, sizeof ( cubeIndices ) );
    }
    
    if (numVerts != NULL)
        *numVerts = numVertices;
    return numIndices;
}
@end
