# issue tracking

- [x] 完善Test部分，使用cmake调整VS的Code Generation 
- [ ] 使游戏Project使用dll部分 , SDL2_mixer, etc
- [ ] 区分Public和Private Headers的逻辑, Public to library user
- [x] imGUI不能在第一次cmake的时候找到SDL2 library
    - 将root中的add_subdirctory(imgui)放到find_library(SDL2...)之后即可在imgui中直接找到SDL2 library
- [x] ResourceManager应该如何找到对应的Resource folder？ ResourcePath. Develoeprs now can set the resource directory in preferences, but GameEditor displays the resource directory in full length
- [ ] Scene Editor Layout
- [x] 将Camera3D和Camera2D合并位Camera 
- [ ] Component应该使用Allocator而不是一个固定的Allocator（FixedArrayAllocator）。若不使用自定义的Allocator就应该只用两个参数
- [ ] Remove RenderGroup 
- [ ] GlobalShaderParameter should manage the uniform buffer index itself, instead of letting user manage it