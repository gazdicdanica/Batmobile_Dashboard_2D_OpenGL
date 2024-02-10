layout(location = 0) in vec2 inPos; //Pozicija tjemena
layout(location = 1) in vec4 inCol; //Boja tjemena - ovo saljemo u fragment sejder
out vec4 channelCol;


uniform float newPos;

void main(){
	if(gl_VertexID >= 2){
		gl_Position = vec4(inPos, 0.0, 1.0);
	}else{
		gl_Position = vec4(inPos.x, newPos, 0.0, 1.0);
	}

	channelCol = inCol;
}