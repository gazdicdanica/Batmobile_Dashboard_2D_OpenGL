#version 330 core //Koju verziju GLSL jezika da koristim (za programabilni pipeline: GL verzija * 100) i koji profil (core - samo programabilni pipeline, compatibility - core + zastarjele stvari)

//Kanali (in, out, uniform)
layout(location = 0) in vec2 inPos; //Pozicija tjemena
layout(location = 1) in vec4 inCol; //Boja tjemena - ovo saljemo u fragment sejder
out vec4 channelCol; //Izlazni kanal kroz koji saljemo boju do fragment sejdera

uniform vec2 uPos;
uniform int one;
uniform float rotationAngle;

void main() //Glavna funkcija sejdera
{
	if(one == 1){
		if(gl_VertexID == 0){
			gl_Position = vec4(inPos, 0.0, 1.0);
		}
		else{
			vec2 center = vec2(-0.55, -0.45);
			vec2 translatedPosition = inPos - center;
			float cosA = cos(radians(rotationAngle));
			float sinA = sin(radians(rotationAngle));
			mat2 rotationMatrix = mat2(cosA, -sinA, sinA, cosA);
			vec2 final = rotationMatrix * translatedPosition + center;
			if(final.x < -0.72) {final.x = -0.72;}
			gl_Position = vec4(final, 0.0, 1.0);
		}
		
	}else{
		gl_Position = vec4(inPos.x + uPos.x, inPos.y + uPos.y, 0.0, 1.0); //gl_Position je predefinisana promjenljiva za pozicije u koju stavljamo nase koordinate. Definisana je kao vec4 pa zbog toga konvertujemo	
	}

	channelCol = inCol;
}