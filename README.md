# FPS-game

5.16 Update

## 任务说明

### 当前重要任务

#### 场景搭建部分

* 地板、墙绘制：主要是处理纹理和 resource。
* 天空盒：晴天即可。
* 模型导入。
* 粒子系统：火焰。场景中的火炬、火堆。

#### 游戏逻辑部分

* 物理系统：物体三维移动（速度系统）。

### 其他重要任务

#### 场景搭建部分

* 粒子系统：烟雾。可以先做一个 sample，之后结合碰撞系统生成效果（血可以作为用特定参数的烟雾模拟）。
* 阴影。
* 光照：不同物体材质、光照贴图。
* 子弹绘制与移动。

#### 游戏逻辑部分

* **物理系统：碰撞检测；包括人物跳上物体，子弹碰撞。**
* 瞄准准心与文字绘制。

### 有余力再完成的任务

#### 场景搭建部分

* 粒子系统：爆炸。


* GUI 控制不同天气：阴天、傍晚天气，更换天空盒和光照颜色；雾天，更换天空盒，更改光照着色器 https://blog.csdn.net/u010223072/article/details/45022745。


* 树的绘制：最好能用分形来绘制。
* 草的绘制：简单贴图。
* 高级光照：不同物体材质，光照贴图，多光源（场景中的火焰），泛光

#### 游戏逻辑部分

* 视角变更：瞄准镜；第一人称第三人称

## 更新备注

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