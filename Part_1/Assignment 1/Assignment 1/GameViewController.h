//
//  GameViewController.h
//  Assignment 1
//
//  Created by Brian Thomas on 2015-02-10.
//  Copyright (c) 2015 Brian Thomas. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
    
@interface GameViewController : GLKViewController
    typedef enum __matrix_multiply_type { GLK, NEON } MatrixMultiplyType;
    GLKMatrix4 MatrixMultiply(GLKMatrix4 a, GLKMatrix4 b, MatrixMultiplyType funct);
@end
