#include "test/subtle.inc"    
#include "colors.inc"    

// first, the camera position

camera {
  location <0,4,-15>
  look_at <0,0,0>
}

// now, some ligh

light_source {
  <0,20,-10>
  color rgb 0.8
}
light_source {
  <10,0,-10>
  color rgb 0.9
}
light_source {
  <2,2,-10>
  color rgb 0.4
}

// the sphere

background { 
  color rgb 0.79687
}

sphere {
  <0,0,0>, 5
  pigment { 
	gradient <0, -1, 0>
	translate 0.5
	color_map { subtle }
	scale 10
  }
  finish { phong 0.1 }
}