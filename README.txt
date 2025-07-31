# FiscionX

**FiscionX** is a modern and lightweight C++ framework built on OpenGL for game development. It offers out-of-the-box support for skinned and static 3D models, spatial audio, embedded GLB model loading, and a powerful shader system — all designed for fast prototyping and efficient runtime performance.

## ✨ Features

- **Static and Skinned Models**
  - Load and render both static meshes and animated characters with skeletal animation.
  
- **GLB 3D Model Loading**
  - Fully supports `.glb` files with embedded textures, animations, and materials.
  
- **Custom & Built-in Shaders**
  - Easily create your own GLSL shaders or use pre-made shaders included with the engine.
  
- **Spatial Audio with FMOD**
  - Immersive 3D sound support using FMOD for realistic audio positioning and effects.
  
- **Binary Caching System**
  - Models and shaders are cached in binary format for ultra-fast loading and reloading.

- **Multimedia Support**
  - Render **images**, **text**, and **videos** as part of your scenes with high flexibility.
  
- **Antialiasing**
  - Built-in antialiasing for clean, high-quality visuals.

## 🛠 Dependencies

- OpenGL 4
- FMOD
- TinyGLTF
- stb_image

## 🔧 Build & Setup
Download a library release and copy FiscionX.h, core.cpp, glfw3 dll, fmod dll and .lib files into your project and include FiscionX.h on your main file.
After, link the .lib files with your project and run.
