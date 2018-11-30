#version 150 core

in vec3 Color;
in vec3 vertNormal;
in vec3 pos;
in vec3 lightDir;
in vec2 texcoord;

out vec4 outColor;

uniform sampler2D wallTex;
uniform sampler2D doorTex;
uniform sampler2D goldTex;
uniform sampler2D floorTex;
uniform sampler2D wallNorm;
uniform sampler2D floorNorm;

const vec3 colors[6]=vec3[6](
            vec3(0.5294, 0.7098, 0.90588),
            vec3(0.5294, 0.9058, 0.7098),
            vec3(0.90588, 0.5294, 0.7098),
            vec3(0.807, 0.5294, 0.905),
            vec3(0.6639, 0.6314, 0.4431),
            vec3(0,0,0)
            );

uniform int texID;
uniform int colorID;

const float ambient = .3;

void main() {
    vec3 color;
    vec3 normal = normalize(vertNormal);

    if(texID == -1)
        color = Color;
    else if (texID == 0){ //walls
        color = texture(wallTex, texcoord).rgb;
        // normal += (2.0 * texture2D(wallNorm, texcoord).rgb - 1.0); // Need to work on local coordinate space..this doesn't work
        // normal = normalize(normal);
    }
    else if (texID == 1){ //door
        color = texture(doorTex, texcoord).rgb;
        color += colors[colorID];
    }
    else if (texID == 2){ // Gold
        color = texture(goldTex, texcoord).rgb;
        color += colors[colorID];
    }
    else if (texID == 3){ //floor
        color = texture(floorTex, texcoord).rgb;
        // normal += (2.0 * texture2D(floorNorm, texcoord).rgb - 1.0);
        // normal = normalize(normal);
    }
    else{
        outColor = vec4(0,0,1,1);
        return; //ERROR
    }

    vec3 diffuseC = color*max(dot(-lightDir, normal),0.0);
    vec3 ambC = color*ambient;
    vec3 viewDir = normalize(-pos);
    vec3 reflectDir = reflect(viewDir, normal);
    float spec = max(dot(reflectDir, lightDir),0.0);
    if(dot(-lightDir,normal) <= 0.0) spec = 0;
    vec3 specC = vec3(.8,.8,.8)*pow(spec,4);
    vec3 oColor = ambC + diffuseC + specC;
    outColor = vec4(oColor, 1);

}