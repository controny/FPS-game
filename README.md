# FPS-game

## 5.16 Update
###新的 singleton component 接口

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