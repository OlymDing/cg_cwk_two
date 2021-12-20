# version 330 core

uniform vec3 inputColor;

out vec4 FragColor;

void main () {
    
    FragColor = vec4(inputColor, 1.0);
};