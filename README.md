# FPS-game

6.1 Update

## 任务说明

### 当前重要任务

#### 场景搭建部分

* 粒子系统
    * 在场景中构建一个火炬或火堆等，反正能演示粒子系统就行。
    * 烟雾。结合碰撞系统，在撞击点生成效果。
    * 血。结合碰撞系统，在撞击点生成效果。

* 骨骼动画
    * 找到能用的人物动画模型
    * 找到能用的敌人模型（合适，能动就行，不用在意细节）
* 阴影渲染

#### 游戏逻辑部分

* 物理系统
    人物转向的碰撞盒变更。

* 后坐力系统
    * 准心大小变化。
    * 准心做成正方形的（现在准心会适配窗口的比例，变成长方形的）。
    * 随机后坐力计算。
    * 连续开枪时的射速处理。

    http://www.te5.com/pubg/2018/284309.html

---

### 之后的重要任务

#### 场景搭建部分

* 模板测试：加入道具，给道具进行描边。
* 用实例化渲染处理同时渲染多个相同物体。
* 织物模拟：在场景中弄个旗子之类的东西。
* 高级光照：不同物体材质，光照贴图，多光源（场景中的火焰），泛光
* GUI 控制不同天气：阴天、傍晚天气，更换天空盒和光照颜色；雾天，更换天空盒，更改光照着色器 https://blog.csdn.net/u010223072/article/details/45022745。
* 树的绘制：最好能用分形来绘制。
* 草的绘制：learnopengl -> 高级 OpenGL -> 混合



#### 游戏逻辑部分

* 视角变更：瞄准镜 http://tieba.baidu.com/p/5200318856；第一人称第三人称

---

## 更新备注

### 6.3
#### 结合`HitProcessingSystem`产生流血或冒烟效果
触发`HitEvent`后，在击中处可产生流血或冒烟效果。射击箱子上部可看到明显的效果，其余部位效果不佳，可通过让`HitEvent`提供射入方向来改善。

### 6.1
#### 新增子弹碰撞检测  & HitProcessingSystem
- 鼠标左键点击后，子弹沿当前准星对准方向射出（没有真正生成子弹实体），进行子弹碰撞检测，若子弹与具有 `CollisionComponent` 的实体碰撞（即相交），会触发 `HitEvent`。
- 另外，新增 `HitProcessingSystem` ，用于接收 `HitEvent` 并 进行子弹射中后的处理（如生成流血，爆炸，弹孔等效果）。
注：`HitEvent` 中存有被击中的实体对应的指针，以及被击中的位置（世界坐标下）。

### 5.30
#### 绝对路径
把所有路径改为绝对路径，这样直接用 exe 也能运行. 并且可以用 RenderDoc 工具进行 debug（推荐！）

### 5.29
#### 新增骨骼动画模型
新建了 BoneObjectComponent 组件，用于加载骨骼动画模型。在 game 中加入骨骼模型，在 rendersystem 中渲染

#### 新增AABB碰撞检测

- 新增 `CollisionComponent` 和 `CollisionSystem`，对需要进行碰撞检测的实体assign一个 `CollisionComponent` （指明宽度，深度和高度 或者 直接指明body的八个值 x_min, x_max, y_min, y_max, z_min, z_max），即可进行碰撞检测；
- 按空格键可以实现player的跳跃。

注：由于碰撞检测的需要，Player需要有个**向下且较小**的Y轴初速度。

### 5.28

#### 整理 RenderSystem

把各部分渲染的逻辑变成一个个 private 函数，这样在 tick 函数中不会太乱，只需要设置好 shader。

#### 分离 Camera.h 逻辑

删除了 `CameraInfoSingletonComponent`, `CameraMovingSystem`, `Camera.h`，改为用 `CameraComponent` 保存 camera 的数据，并把 `Front, Right` 等向量数据保存在 `PositionComponent` 中。

#### 用 PlayerComponent 代表 player entity

拥有 `PlayerComponent` 的 entity 就是 player，component 中可以存放 player 游戏中需要的数据。目前只有这个 entity 拥有 `CameraComponent`，根据这个 entity 的 `PositionComponent` 位置来更新 camera 的位置。这个 entity 的 model 也会根据视角方向 `Front, Right` 来进行旋转；通过视角方向计算出 xz 方向的速度，利用 `MovementComponent` 实现移动。

### 5.26 

#### 物体移动：修改object渲染方式 & MovementSystem

移动时, `ObjectComponent` 在GPU中的 vertices 数据不实时更新，而是在着色器中通过原始 vertices 数据 + `PositionComponent` 中的 Position 数据（作为 model 矩阵的位移值）来确定渲染位置。
因此，对于每一个 assign 了 `ObjectComponent` 的实体，都需要 assign 一个 `PositionComponent` 来确定其在世界坐标中的位置，否则无法正常将其渲染。

对要移动的实体assign一个MovementComponent（指明速度和加速度），即可实现移动。

#### 粒子系统

- 新增 ParticleComponent, 提供接口设置粒子个数、喷射位置、喷射方向、粒子颜色等，可模拟不同的粒子效果。
- RenderSystem 新增 particle 的渲染。

### 5.18 

#### ObjectComponent, shift 加速

把 Mesh 归入ObjectComponent；修改 RenderSystem，只需渲染 ObjectComponent；把 Model 加入game；把使用 “MeshComponent.h” 的 Headers 改为 "ObjectComponent.h"

修改CubeResource，其init函数以中心位置、长宽高以及贴图对象作为参数，可用于实现地板和墙。

按住左边的 shift 键可以加速移动。

### 5.16 

#### 新的 singleton component 接口

相当于 world 的全局变量，不被某实体所拥有。

```c++
template<typename T, typename... Args>
void createSingletonComponent(Args&&... args) {
    this->singletons->assign<T>(args...);
}

template<typename T>
ComponentHandle<T> getSingletonComponent() {
    return this->singletons->get<T>();
}
```
#### 用法
在 `Game.h` 中使用 `createSingletonComponent` 函数注册对应类型的 singleton component 到 world 实例中。
然后在 system 中调用 `world->getSingletonComponent<T>()` 获得对应 singleton component 的 handler（可以看成是指针）。

#### 备注
目前只有 `MeshComponent` 不是 singleton 的，即可以被实体所拥有。可以参考 `RenderSystem` 中获得和使用非 singleton 组件的例子。以后应该大部分的 component 都不是 singleton 的。
