# DraconisEngine

https://github.com/solidajenjo/DraconisEngine

Game Engine based on OpenGL 3.3 & SDL.

Uses DeVIL, Assimp, IMGui, IMGuizmo, PhysFS.

Developed by Rubén Ajenjo Rodriguez for the UPC Master degree of videogame development.

- Controls

  - Right Click to rotate camera
  - Alt + Right Click to orbitate camera
  - Right Click + WASD to move camera
  - F to focus on selected GameObject
  
- Importing assets

  - Drop a fbx file anywhere on the editor. The engine will import it and store it in the Assets hierarchy on Files section.
  - Drop any kind of image anywhere on the editor. The engine will import it and generate a Map Component ready to be used on the materials of the scene.
  
- Instantiating meshes

  -Drop any of the imported meshes anywhere on the editor viewport. The mesh will be world instantiated as instance of the imported mesh. Any change on the materials of the instantiated mesh will affect at all the instances of the mesh. Break the instance using the break instance button on the inspector. This will generate an independent clone with its own materials.

- Rendering

  - The engine has 2 render modes, Forward rendering and Deferred rendering. Switch between the two modes on the configuration section.
  
  - Antialiasing
  
    - SSAAx2: Renders the game viewport at twice the game viewport resolution and downscales the render to the original resolution.
    - MSAAx4: Multisampling x4, only on Forward rendering available.
    
- Space partitioning

  - The engine uses a KDTree to store the static gameobjects to perform frustum culling when a scene camera is instantiated and marked as main camera
  - The engine uses an AABBTree to store the non static gameobjec to perform frustum culling as well
  - The engine uses an AABBTree with the scene lights to send to the renderer only the ones which its light volume is inside or intersects the frustum
    
  

Disclaimer: No programmers were harmed in the making of this engine.
