# issue tracking

- [x] 完善Test部分，使用cmake调整VS的Code Generation 
- [ ] 使游戏Project使用dll部分 
- [ ] 区分Public和Private Headers的逻辑
- [x] imGUI不能在第一次cmake的时候找到SDL2 library
    - 将root中的add_subdirctory(imgui)放到find_library(SDL2...)之后即可在imgui中直接找到SDL2 library
- [ ] ResourceManager应该如何找到对应的Resource folder？ ResourcePath. Develoeprs now can set the resource directory in preferences, but GameEditor displays the resource directory in full length

 