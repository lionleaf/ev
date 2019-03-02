#version 410 core
out vec4 FragColor;

in vec2 vTexCoord;

void main() {
  vec2 centerPoint = vec2(0.5f,0.5f);
  float dist = distance(vTexCoord, centerPoint)*2;
  float circle = 1.0f - floor(dist);
  FragColor = vec4(circle, circle, circle, circle);
}
