#ifndef POLYGON_H
#define POLYGON_H

#include <math.h>

#define MAX_POINTS = 100;

class Polygon {
    private:
        int max_y, max_x, min_y, min_x;
        double * x_array;
        double * y_array;
        double * x_array_no_rotation;
        double * y_array_no_rotation;
        double current_degrees;
        
    
    public:
        int size;
        Polygon();
        void addPoint(int x, int y);
        double getPointX(int i);
        double getPointY(int i);        
        int getMinimumYIndex(int i_0, int i_1);
        int getMidY();
        
        // Transformations
        void move(int d_x, int d_y);
        void scale(double f_x, double f_y);
        void shearX(double factor);
        void shearY(double factor);
        void rotate(int degree);
        void doRotate(double degree);
        void rotate(int x_0, int y_0, int x_1, int y_1);
};

Polygon::Polygon() {
    
    size = 0;
    
    current_degrees = 0.0;
    //~ current_d_x = 0;
    //~ current_d_y = 0;
    //~ current_shear_x = 0;
    //~ current_shear_y = 0;
    //~ current_scale_x = 0;
    //~ current_scale_y = 0;
    
    max_y = -1;
    max_x = -1;
    
    min_y = 1000;
    min_x = 1000;
    
    x_array = new double[360];
    y_array = new double[360];
    x_array_no_rotation = new double[360];
    y_array_no_rotation = new double[360];
}

void Polygon::addPoint(int x, int y) {
    
    if (x < min_x) {
        min_x = x;   
    }
    
    if (y < min_y) {
        min_y = y;
    }
        
    if (y > max_y) {
        max_y = y;
    }
        
    if (x > max_x) {
        max_x = x;
    }
    
    x_array[size] = x;
    y_array[size] = y;
    x_array_no_rotation[size] = x;
    y_array_no_rotation[size] = y;
    size = size + 1;
}


double Polygon::getPointX(int i) {
    return x_array[i];
}

double Polygon::getPointY(int i) {
    return y_array[i];
}

int Polygon::getMinimumYIndex(int i_1, int i_2) {
    if (getPointY(i_1) < getPointY(i_2)) {
        return i_1;
    } else {
        return i_2;
    }
}

void Polygon::move(int d_x, int d_y) {
    
    min_x += d_x;
    min_y += d_y;
    min_x += d_y;
    min_y += d_x;
    
    for (int i = 0; i < size; i++){
        x_array[i] += d_x;
        y_array[i] += d_y;
        x_array_no_rotation[i] += d_x;
        y_array_no_rotation[i] += d_y;
    }
}

void Polygon::scale(double s_x, double s_y) {
    
    double scale_x = 1.0 + s_x / 100.0;
    double scale_y = 1.0 + s_y / 100.0;
    
    // Check polygon doesn't reflect when going through size limits
    if (scale_x <= 0 || scale_y <= 0) 
        return;
    
    int mid_x = (int) ((max_x + min_x) / 2.0); 
    int mid_y = (int) ((max_y + min_y) / 2.0);
    
    // Move polygon to middle the scale over central point
    move(-mid_x, -mid_y); 
    
    for (int i = 0; i < size; i++){
        x_array[i] *= scale_x;
        y_array[i] *= scale_y;
        x_array_no_rotation[i] *= scale_x;
        y_array_no_rotation[i] *= scale_y;
    }
    
    // Bring polygon back to its original place
    move(mid_x,mid_y); 
}

void Polygon::shearX(double factor) {
    
    double x = factor / 100.0;
    
    int mid_x = (int) ((max_x + min_x) / 2.0); 
    int mid_y = (int) ((max_y + min_y) / 2.0);
    
    // Move polygon to shear over central point
    move(-mid_x,-mid_y); 
    
    for (int i = 0; i < size; i++){
        x_array[i] = x_array[i] + x * y_array[i];
        x_array_no_rotation[i] = x_array[i];
    }
    
    // Bring polygon back to its original place
    move(mid_x,mid_y);
}

void Polygon::shearY(double factor) {
    
    double y = factor / 100.0;
    
    int mid_x = (int) ((max_x + min_x) / 2.0); 
    int mid_y = (int) ((max_y + min_y) / 2.0);
    
    // Move polygon to shear over central point
    move(-mid_x,-mid_y); 
    
    for (int i = 0; i < size; i++){
        y_array[i] = y_array[i] + y * x_array[i];
        y_array_no_rotation[i] = y_array[i];
    }
    
    // Bring polygon back to its original place
    move(mid_x,mid_y);
}

int Polygon::getMidY() {
    return (int) ((max_y + min_y)/2.0);
}

void Polygon::rotate(int degrees) {
    current_degrees += (degrees * (PI/180.0));
    doRotate(current_degrees);
}

// Not used
void Polygon::rotate(int x_0, int y_0, int x_1, int y_1) {
    
}

void Polygon::doRotate(double angle) {
    
    double cosine = cos(angle);
    double sine = sin(angle);
    
    // Move polygon to shear over central point
    int mid_x = (int) ((max_x + min_x)/2.0);
    int mid_y = (int) ((max_y + min_y)/2.0);
    move(-mid_x,-mid_y);
    
    for (int i = 0; i < size; i++){
        x_array[i] = cosine * x_array_no_rotation[i] - sine * y_array_no_rotation[i];
        y_array[i] = sine * x_array_no_rotation[i] + cosine * y_array_no_rotation[i];
    }
    
    // Bring polygon back to its original place
    move(mid_x,mid_y);
}
#endif
