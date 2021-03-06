xof 0303txt 0032
template ColorRGBA {
 <35ff44e0-6c7c-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <d3e16e81-7835-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template Material {
 <3d82ab4d-62da-11cf-ab39-0020af71e433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template TextureFilename {
 <a42790e1-7810-11cf-8f52-0040333594a3>
 STRING filename;
}

template Frame {
 <3d82ab46-62da-11cf-ab39-0020af71e433>
 [...]
}

template Matrix4x4 {
 <f6f23f45-7686-11cf-8f52-0040333594a3>
 array FLOAT matrix[16];
}

template FrameTransformMatrix {
 <f6f23f41-7686-11cf-8f52-0040333594a3>
 Matrix4x4 frameMatrix;
}

template Vector {
 <3d82ab5e-62da-11cf-ab39-0020af71e433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template MeshFace {
 <3d82ab5f-62da-11cf-ab39-0020af71e433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template Mesh {
 <3d82ab44-62da-11cf-ab39-0020af71e433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

template MeshNormals {
 <f6f23f43-7686-11cf-8f52-0040333594a3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshMaterialList {
 <f6f23f42-7686-11cf-8f52-0040333594a3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material <3d82ab4d-62da-11cf-ab39-0020af71e433>]
}

template Coords2d {
 <f6f23f44-7686-11cf-8f52-0040333594a3>
 FLOAT u;
 FLOAT v;
}

template MeshTextureCoords {
 <f6f23f40-7686-11cf-8f52-0040333594a3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}


Material Material__54 {
 1.000000;1.000000;1.000000;1.000000;;
 10.000000;
 0.000000;0.000000;0.000000;;
 0.000000;0.000000;0.000000;;

 TextureFilename {
  "T_Floor0_D.png";
 }
}

Material Material__51 {
 1.000000;1.000000;1.000000;1.000000;;
 10.000000;
 0.000000;0.000000;0.000000;;
 0.000000;0.000000;0.000000;;

 TextureFilename {
  "T_Floor0_D.png";
 }
}

Material Material__48 {
 1.000000;1.000000;1.000000;1.000000;;
 10.000000;
 0.000000;0.000000;0.000000;;
 0.000000;0.000000;0.000000;;

 TextureFilename {
  "T_Floor0_D.png";
 }
}

Material Material__49 {
 1.000000;1.000000;1.000000;1.000000;;
 10.000000;
 0.000000;0.000000;0.000000;;
 0.000000;0.000000;0.000000;;

 TextureFilename {
  "T_Floor0_D.png";
 }
}

Material Material__50 {
 1.000000;1.000000;1.000000;1.000000;;
 10.000000;
 0.000000;0.000000;0.000000;;
 0.000000;0.000000;0.000000;;

 TextureFilename {
  "T_Floor0_D.png";
 }
}

Material Material__52 {
 1.000000;1.000000;1.000000;1.000000;;
 10.000000;
 0.000000;0.000000;0.000000;;
 0.000000;0.000000;0.000000;;

 TextureFilename {
  "T_Floor0_D.png";
 }
}

Material Material__53 {
 1.000000;1.000000;1.000000;1.000000;;
 10.000000;
 0.000000;0.000000;0.000000;;
 0.000000;0.000000;0.000000;;

 TextureFilename {
  "T_Floor0_D.png";
 }
}

Material Material__57 {
 1.000000;1.000000;1.000000;1.000000;;
 10.000000;
 0.000000;0.000000;0.000000;;
 0.000000;0.000000;0.000000;;

 TextureFilename {
  "T_Floor0_D.png";
 }
}

Material Material__56 {
 1.000000;1.000000;1.000000;1.000000;;
 10.000000;
 0.000000;0.000000;0.000000;;
 0.000000;0.000000;0.000000;;

 TextureFilename {
  "T_Floor0_D.png";
 }
}

Material Material__55 {
 1.000000;1.000000;1.000000;1.000000;;
 10.000000;
 0.000000;0.000000;0.000000;;
 0.000000;0.000000;0.000000;;

 TextureFilename {
  "T_Floor0_D.png";
 }
}

Material Material__46 {
 1.000000;1.000000;1.000000;1.000000;;
 10.000000;
 0.000000;0.000000;0.000000;;
 0.000000;0.000000;0.000000;;

 TextureFilename {
  "T_Floor0_D.png";
 }
}

Material Material__47 {
 1.000000;1.000000;1.000000;1.000000;;
 10.000000;
 0.000000;0.000000;0.000000;;
 0.000000;0.000000;0.000000;;

 TextureFilename {
  "T_Floor0_D.png";
 }
}

Frame Pannel19 {
 

 FrameTransformMatrix {
  -2.121320,0.000000,-2.121320,0.000000,0.000000,3.000000,0.000000,0.000000,2.121320,0.000000,-2.121320,0.000000,110.417165,30.544541,238.475571,1.000000;;
 }

 Mesh Pannel19 {
  25;
  -3.375000;0.755001;-1.687500;,
  -3.037500;0.755001;-1.687500;,
  -3.375000;0.755001;-2.025000;,
  -3.037500;0.755001;-2.025000;,
  -2.700000;0.755001;-1.687500;,
  -2.700000;0.755001;-2.025000;,
  -2.362500;0.755001;-1.687500;,
  -2.362500;0.755001;-2.025000;,
  -2.025000;0.755001;-1.687500;,
  -2.025000;0.755001;-2.025000;,
  -3.375000;0.755001;-1.350000;,
  -3.037500;0.755001;-1.350000;,
  -2.700000;0.755001;-1.350000;,
  -2.362500;0.755001;-1.350000;,
  -2.025000;0.755001;-1.350000;,
  -3.375000;0.755001;-1.012500;,
  -3.037500;0.755001;-1.012500;,
  -2.700000;0.755001;-1.012500;,
  -2.362500;0.755001;-1.012500;,
  -2.025000;0.755001;-1.012500;,
  -3.375000;0.755001;-0.675000;,
  -3.037500;0.755001;-0.675000;,
  -2.700000;0.755001;-0.675000;,
  -2.362500;0.755001;-0.675000;,
  -2.025000;0.755001;-0.675000;;
  32;
  3;0,1,2;,
  3;3,2,1;,
  3;1,4,3;,
  3;5,3,4;,
  3;4,6,5;,
  3;7,5,6;,
  3;6,8,7;,
  3;9,7,8;,
  3;10,11,0;,
  3;1,0,11;,
  3;11,12,1;,
  3;4,1,12;,
  3;12,13,4;,
  3;6,4,13;,
  3;13,14,6;,
  3;8,6,14;,
  3;15,16,10;,
  3;11,10,16;,
  3;16,17,11;,
  3;12,11,17;,
  3;17,18,12;,
  3;13,12,18;,
  3;18,19,13;,
  3;14,13,19;,
  3;20,21,15;,
  3;16,15,21;,
  3;21,22,16;,
  3;17,16,22;,
  3;22,23,17;,
  3;18,17,23;,
  3;23,24,18;,
  3;19,18,24;;

  MeshNormals {
   25;
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;;
   32;
   3;0,1,2;,
   3;3,2,1;,
   3;1,4,3;,
   3;5,3,4;,
   3;4,6,5;,
   3;7,5,6;,
   3;6,8,7;,
   3;9,7,8;,
   3;10,11,0;,
   3;1,0,11;,
   3;11,12,1;,
   3;4,1,12;,
   3;12,13,4;,
   3;6,4,13;,
   3;13,14,6;,
   3;8,6,14;,
   3;15,16,10;,
   3;11,10,16;,
   3;16,17,11;,
   3;12,11,17;,
   3;17,18,12;,
   3;13,12,18;,
   3;18,19,13;,
   3;14,13,19;,
   3;20,21,15;,
   3;16,15,21;,
   3;21,22,16;,
   3;17,16,22;,
   3;22,23,17;,
   3;18,17,23;,
   3;23,24,18;,
   3;19,18,24;;
  }

  MeshMaterialList {
   12;
   32;
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0;
   { Material__54 }
   { Material__51 }
   { Material__48 }
   { Material__49 }
   { Material__50 }
   { Material__52 }
   { Material__53 }
   { Material__57 }
   { Material__56 }
   { Material__55 }
   { Material__46 }
   { Material__47 }
  }

  MeshTextureCoords c1 {
   25;
   0.000000;0.750000;,
   0.250000;0.750000;,
   0.000000;1.000000;,
   0.250000;1.000000;,
   0.500000;0.750000;,
   0.500000;1.000000;,
   0.750000;0.750000;,
   0.750000;1.000000;,
   1.000000;0.750000;,
   1.000000;1.000000;,
   0.000000;0.500000;,
   0.250000;0.500000;,
   0.500000;0.500000;,
   0.750000;0.500000;,
   1.000000;0.500000;,
   0.000000;0.250000;,
   0.250000;0.250000;,
   0.500000;0.250000;,
   0.750000;0.250000;,
   1.000000;0.250000;,
   0.000000;0.000000;,
   0.250000;0.000000;,
   0.500000;0.000000;,
   0.750000;0.000000;,
   1.000000;0.000000;;
  }
 }
}