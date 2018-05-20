# FPS-game

5.20 Update

## 任务说明

### 当前重要任务（下周日5月27日前）

#### 场景搭建部分

* **粒子系统（可以借助 velocity component）：火焰。场景中的火炬、火堆。烟雾。可以先做一个 sample，之后结合碰撞系统生成效果（血可以作为用特定参数的烟雾模拟），爆炸。**

* 生成player entity，将camera与其绑定。

* 骨骼动画（如：player移动时手拿枪动画）。

#### 游戏逻辑部分

* **物理系统：碰撞检测；包括人物跳上物体，子弹碰撞(点击左键发射子弹)。**

* 后坐力系统：准心大小变化， camera视角移动。

---

### 之后的重要任务

#### 场景搭建部分

* 阴影。

* 高级光照：不同物体材质，光照贴图，多光源（场景中的火焰），泛光

* GUI 控制不同天气：阴天、傍晚天气，更换天空盒和光照颜色；雾天，更换天空盒，更改光照着色器 https://blog.csdn.net/u010223072/article/details/45022745。

* 树的绘制：最好能用分形来绘制。

* 草的绘制：简单贴图。



#### 游戏逻辑部分

* 视角变更：瞄准镜；第一人称第三人称

---

### 已完成

#### 场景搭建部分

* ~~地板、墙绘制：主要是处理 resource 生成数据部分和处理纹理。~~
* ~~天空盒：晴天即可。可以把天空盒需要的数据作为 singleton component~~
* ~~assimp 模型导入。需要考虑如何用 component 保存数据。~~

#### 游戏逻辑部分

* **~~物理系统：物体三维移动（速度系统）；可以参考使用的 ECS 那个库的 github README 里的示例，给 entity 附上 velocity component。~~**
* ~~文字渲染与准星。要处理 FreeType 库以及渲染文字的 system 和着色器。~~

---

## 更新备注

### 5.20 物体移动
新增 PositionComponent, MovementComponent
新增 MovementSystem

#### 用法
对要移动的实体assign一个MovementComponent（指明速度和加速度），即可实现移动。

### 5.18 ModelComponent, shift 加速
MeshComponent 改为 ModelComponent，统一处理 assimp 模型和 cube 原始数据。cube resource 可以生成长方体。
按住左边的 shift 键可以加速移动。

### 5.16 新的 singleton component 接口

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

### 5.18更新：
1. 把Mesh归入ObjectComponent;修改RenderSystem,只需渲染ObjectComponent；把Model加入game；把使用“MeshComponent.h”的Headers改为"ObjectComponent.h"
2. 修改CubeResource，其init函数以中心位置、长宽高以及贴图对象作为参数，可用于实现地板和墙。
