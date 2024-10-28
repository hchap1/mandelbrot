#version 400 core

in float x_pos;
in float y_pos;

uniform float x_min; 
uniform float x_mag; 
uniform float y_min; 
uniform float y_mag; 
uniform float zoom;
uniform float center_x; 
uniform float center_y; 
uniform float o;
uniform float offset_a;
uniform float offset_b;

out vec4 FragColor;

vec4 hsvToRgb(float h, float s, float v) {
    float c = v * s;                     // Chroma
    float x = c * (1.0 - abs(mod(h * 6.0, 2.0) - 1.0));
    float m = v - c;

    vec3 rgb;
    if (h < 1.0 / 6.0) {
        rgb = vec3(c, x, 0.0);
    } else if (h < 2.0 / 6.0) {
        rgb = vec3(x, c, 0.0);
    } else if (h < 3.0 / 6.0) {
        rgb = vec3(0.0, c, x);
    } else if (h < 4.0 / 6.0) {
        rgb = vec3(0.0, x, c);
    } else if (h < 5.0 / 6.0) {
        rgb = vec3(x, 0.0, c);
    } else {
        rgb = vec3(c, 0.0, x);
    }
    
    return vec4(rgb + vec3(m), 1.0);
}

struct Complex {
    float a;
    float b;
};

Complex createComplex(float a, float b) {
    Complex z;
    z.a = a;
    z.b = b;
    return z;
}

Complex iter(Complex z, float oa, float ob) {
    float newA = z.a * z.a - z.b * z.b + oa;
    float newB = 2.0 * z.a * z.b + ob;
    return createComplex(newA, newB);
}

float isStable(Complex z, float oa, float ob, float order) {
    for (float i = 0.0; i < order; ++i) {
        z = iter(z, oa, ob);
        if ((z.a - oa) * (z.a - oa) + (z.b - ob) * (z.b - ob) > 4.0) {
            return i * 0.5;
        }
    }
    return order;
}

void main() {
    float x = (((x_pos + 1.25) / 2.0) * x_mag + x_min) / zoom + center_x;
    float y = (((y_pos + 1.0) / 2.0) * y_mag + y_min) / zoom + center_y;
    
    Complex z = createComplex(x, y);
    
    float colour = 0.0;
    if (offset_a != 0.0 || offset_b != 0.0) {
        colour = isStable(z, offset_a, offset_b, o);
    } else {
        colour = isStable(z, z.a, z.b, o);
    }
    if (colour < o * 0.5) {
        FragColor = hsvToRgb(colour / o + 0.2, pow(colour / o * 1.2, 1.2) + 0.2, 1.0); // Output color
    } else {
        FragColor = vec4(0.0,0.0,0.0,1.0);
    }
}